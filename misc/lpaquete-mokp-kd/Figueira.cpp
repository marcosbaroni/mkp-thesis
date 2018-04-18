/*
 * Figueira.cpp
 *
 *  Created on: 2010/07/06
 *      Author: msimoes
 */

#include <math.h>
#include "headers/Figueira.h"
#include "headers/GlobalData.h"

typedef multiset<Extension*, ComparerM>::iterator F_iterator;
typedef multiset<Extension*, ComparerM> F_set;


double V(RankingItem* xf, RankingItem* xj, int p) {

	return xj->profits[p] - (float)xf->profits[p] * xj->weight / xf->weight;
}

void removeDominatedFCopy(Item * s, F_set * F, F_iterator it)
{
	F_iterator itTemp;
	F_iterator itEnd = F->end();

	for( it++; it != itEnd ; )
	{
		if ((*it)->profits[1] <= s->profits[1])
		{
			itTemp = it;
			it++;
			F->erase(itTemp);
		}
		else
			return;
	}
}



void keepNonDominatedSetCopy(Extension *s, multiset<Extension*, ComparerM> * F)
{
	F_iterator itEnd = F->end();
	F_iterator itBegin = F->begin();

	if(itEnd == itBegin){
		F->insert(s);
		return;
	}

	F_iterator it = F->upper_bound(s), tmp;

	if ( it == itBegin )
	{
		it = F->insert(it,s);
		removeDominatedFCopy(s,F,it);
	}
	else
	{
		--it;
		if ( (*it)->profits[1] < s->profits[1])
		{
			if( (*it)->profits[0] == s->profits[0] )
			{
				tmp = it;
				it++;
				F->erase(tmp);
				it = F->insert(it, s);
				removeDominatedFCopy(s,F,it);
			}
			else
			{
				it = F->insert(it,s);
				removeDominatedFCopy(s,F,it);
			}
		}
		else
			delete(s);
	}
}

/* ======================================================================
 main
 ====================================================================== */


Extension * positive_bidimensional_improvement0(multiset<RankingItem*, ComparerProfit0> & Order, int p0, int p1, int remain_cap, double * a, int f)
{
	multiset<RankingItem*>::iterator oit, oitend = Order.end();
	RankingItem * tmp;
	list<int> to_undo;

	while(true)
	{
		tmp = NULL;
		for (oit = Order.begin(); oit != oitend; oit++)
		{
			if (a[(*oit)->orderNum] == 0 && (*oit)->orderNum != f && (*oit)->weight < remain_cap )
			{
				tmp = *oit;
				break;
			}
		}
		if (tmp == NULL)
			break;

		remain_cap -= tmp->weight;
		p0 += tmp->profits[0];
		p1 += tmp->profits[1];
		a[tmp->orderNum] = 1;
		to_undo.push_back(tmp->orderNum);
	}
	for (list<int>::iterator it = to_undo.begin(); it != to_undo.end(); it++)
		a[*it] = 0;

	return new Extension(p0, p1);
}

Extension * positive_bidimensional_improvement1(multiset<RankingItem*, ComparerProfit1> & Order, int p0, int p1, int remain_cap, double * a, int f)
{
	multiset<RankingItem*>::iterator oit, oitend = Order.end();
	RankingItem * tmp;

	list<int> to_undo;

	while(true)
	{
		tmp = NULL;
		for (oit = Order.begin(); oit != oitend; oit++)
		{
			if (a[(*oit)->orderNum] == 0 && (*oit)->orderNum != f && (*oit)->weight < remain_cap )
			{
				tmp = *oit;
				break;
			}
		}
		if (tmp == NULL)
			break;

		remain_cap -= tmp->weight;
		p0 += tmp->profits[0];
		p1 += tmp->profits[1];
		a[tmp->orderNum] = 1;
		to_undo.push_back(tmp->orderNum);
	}
	for (list<int>::iterator it = to_undo.begin(); it != to_undo.end(); it++)
		a[*it] = 0;

	return new Extension(p0, p1);
}



Extension * negative_bidimensional_improvement0(multiset<RankingItem*, ComparerProfit0> & Order, int p0, int p1, int remain_cap, double * a, int f, RankingItem * xf)
{
	multiset<RankingItem*>::reverse_iterator oit, roitend = Order.rend();
	RankingItem * tmp;

	remain_cap -= xf->weight;
	p0 += xf->profits[0];
	p1 += xf->profits[1];

	tmp = NULL;
	for (oit = Order.rbegin(); oit != roitend; oit++)
	{
		if (a[(*oit)->orderNum] == 1 && (*oit)->orderNum != f && (*oit)->weight > -1 * remain_cap )
		{
			tmp = *oit;
			break;
		}
	}

	if (tmp == NULL)
		return new Extension(p0 - xf->profits[0], p1 - xf->profits[1]);

	remain_cap += tmp->weight;
	p0 -= tmp->profits[0];
	p1 -= tmp->profits[1];
	a[tmp->orderNum] = 0;

	positive_bidimensional_improvement0(Order, p0, p1, remain_cap, a, f);

	a[tmp->orderNum] = 1;

	return new Extension(p0, p1);
}

Extension * negative_bidimensional_improvement1(multiset<RankingItem*, ComparerProfit1> & Order, int p0, int p1, int remain_cap, double * a, int f, RankingItem * xf)
{
	multiset<RankingItem*>::reverse_iterator oit, roitend = Order.rend();
	RankingItem * tmp;

	remain_cap -= xf->weight;
	p0 += xf->profits[0];
	p1 += xf->profits[1];

	tmp = NULL;
	for (oit = Order.rbegin(); oit != roitend; oit++)
	{
		if (a[(*oit)->orderNum] == 1 && (*oit)->orderNum != f && (*oit)->weight > -1 * remain_cap )
		{
			tmp = *oit;
			break;
		}
	}

	if (tmp == NULL)
		return new Extension(p0 - xf->profits[0], p1 - xf->profits[1]);

	remain_cap += tmp->weight;
	p0 -= tmp->profits[0];
	p1 -= tmp->profits[1];
	a[tmp->orderNum] = 0;

	positive_bidimensional_improvement1(Order, p0, p1, remain_cap, a, f);

	a[tmp->orderNum] = 1;

	return new Extension(p0, p1);
}


void Figueira::generateExtensions(
	list<RankingItem*> & points,
	multiset<RankingItem*, ComparerProfit0> & Order0,
	multiset<RankingItem*, ComparerProfit1> & Order1,
	multiset<Extension*, ComparerM> & F)
{
	double a[GlobalData::NUM_POINTS];
	multiset<RankingItem*, ComparerProfit0> ::iterator oit = Order0.begin(), oitend = Order0.end(), oit2;
	int i = 0, k = 0, cumulative_weight = 0, remain_capacity = 0;
	double p0 = 0, p1 = 0;
	int f = 0, j = 0;
	RankingItem *xf = *(Order0.begin()), *xj = *(Order0.begin());

	double Vratio, V0, V1, aux;
	bool isInteger = false;
	Extension *e;

	//clock_t begin=clock();

	for (i = 0; i < GlobalData::NUM_POINTS; i++)
		a[i] = 0;

	while(oit != oitend)
	{
		// sum weight until W
		cumulative_weight += (*oit)->weight;
		i = (*oit)->orderNum;

		if (cumulative_weight > GlobalData::MAX_WEIGHT)
		{
			cumulative_weight -= (*oit)->weight;
			remain_capacity = GlobalData::MAX_WEIGHT - cumulative_weight;


			a[i] = remain_capacity * 1.0 / (*oit)->weight;

			// update profits
			//p0 += a[i] * (*oit)->profits[0];
			//p1 += a[i] * (*oit)->profits[1];

			if (a[i] == 0)
				isInteger = true;

			f = i;
			xf = *oit;

			break;
		}

		a[i] = 1;
		p0 += (*oit)->profits[0];
		p1 += (*oit)->profits[1];

		oit++;

	}


	while(true)
	{

		// positive bidimensional improvement
		e = positive_bidimensional_improvement0(Order0, (int)(p0), (int)(p1), remain_capacity, a, f);
		keepNonDominatedSetCopy(e, &F);

		e = positive_bidimensional_improvement1(Order1, (int)(p0), (int)(p1), remain_capacity, a, f);
		keepNonDominatedSetCopy(e, &F);

		// negative bidimensional improvement
		e = negative_bidimensional_improvement0(Order0, (int)(p0), (int)(p1), remain_capacity, a, f, xf);
		keepNonDominatedSetCopy(e, &F);

		e = negative_bidimensional_improvement1(Order1, (int)(p0), (int)(p1), remain_capacity, a, f, xf);
		keepNonDominatedSetCopy(e, &F);

		j = -1;
		Vratio = 1;

		if (isInteger)
		{
			for (oit = Order0.begin(); oit != oitend; oit++)
			{
				i = (*oit)->orderNum;
				oit2 = oit;
				for (oit2++; oit2 != oitend; oit2++)
				{
					k = (*oit2)->orderNum;
					if (a[i] == 0 && a[j] == 1)
					{
						if ( (V0 = V(*oit, *oit2, 0)) > 0 && (V1 = V(*oit, *oit2, 1)) < 0  && V1/V0 < Vratio )
						{
							f = i;
							j = k;
							Vratio = V1/V0;
							xf = *oit;
							xj = *oit2;
						}

					}
					else if (a[i] == 1 && a[j] == 0)
					{
						if ( (V0 = V(*oit, *oit2, 0)) < 0 && (V1 = V(*oit, *oit2, 1)) > 0  && V1/V0 < Vratio )
						{
							f = i;
							j = k;
							Vratio = V1/V0;
							xf = *oit;
							xj = *oit2;
						}
					}
				}
			}

			isInteger = false;
		}
		else
		{
			for (oit = Order0.begin(); oit != oitend; oit++)
			{
				i = (*oit)->orderNum;
				if (a[i] == 0)
				{
					if ( (V0 = V(xf, *oit, 0)) < 0 && (V1 = V(xf, *oit, 1)) > 0  && V1/V0 < Vratio )
					{
						j = i;
						Vratio = V1/V0;
						xj = *oit;
					}

				}
				else if (a[i] == 1)
				{
					if ( (V0 = V(xf, *oit, 0)) > 0 && (V1 = V(xf, *oit, 1)) < 0  && V1/V0 < Vratio )
					{
						j = i;
						Vratio = V1/V0;
						xj = *oit;
					}

				}
			}

		}
		if (j == -1)
			break;

		if (a[j] == 0)
		{
			if ( (float)xf->weight / xj->weight * a[f] < 1 )
			{
				// add the j part to the profits
				a[j] = (float)xf->weight / xj->weight * a[f];

				// remove the f part to the profits
				a[f] = 0;

				// j is the new f
				f = j;
				xf = xj;
			}
			else if ( (float)xf->weight / xj->weight * a[f] > 1 )
			{
				// update the f part to the profits
				aux = (float)xj->weight / xf->weight;
				a[f] -= aux;

				// add the j part to the profits
				a[j] = 1;
				p0 += xj->profits[0];
				p1 += xj->profits[1];
				remain_capacity -= xj->weight;
			}
			else
			{
				// remove the f part to the profits
				a[f] = 0;

				// add the j part to the profits
				a[j] = 1;
				p0 += xj->profits[0];
				p1 += xj->profits[1];
				remain_capacity -= xj->weight;

				//TODO the solution is integer
				printf("is integer\n");
				isInteger = true;
			}
		}
		else
		{
			if ( (float)xf->weight / xj->weight * (1 - a[f]) < 1 )
			{
				// add the j part to the profits
				a[j] -= ( (float)xf->weight / xj->weight ) * (1 - a[f]);
				p0 -= xj->profits[0];
				p1 -= xj->profits[1];
				remain_capacity += xj->weight;

				// update the f part to the profits
				p0 += xf->profits[0];
				p1 += xf->profits[1];
				remain_capacity -= xf->weight;
				a[f] = 1;

				// j is the new f
				f = j;
				xf = xj;
			}
			else if ( (float)xf->weight / xj->weight * (1 - a[f]) > 1 )
			{
				// update the f part to the profits
				aux = (float)xj->weight / xf->weight;
				a[f] += aux;

				// remove the j part to the profits
				a[j] = 0;
				p0 -= xj->profits[0];
				p1 -= xj->profits[1];
				remain_capacity += xj->weight;
			}
			else
			{
				// add the f part to the profits
				p0 += xf->profits[0];
				p1 += xf->profits[1];
				remain_capacity -= xf->weight;
				a[f] = 1;

				// remove the j part to the profits
				a[j] = 0;
				p0 -= xj->profits[0];
				p1 -= xj->profits[1];
				remain_capacity += xj->weight;

				//TODO the solution is integer
				printf("is integer\n");
				isInteger = true;
			}
		}

	}

	//clock_t end=clock();
	//double time = (((double)(end-begin))/CLOCKS_PER_SEC);
	//printf("%lf %d\n", time, (int)F.size());

}
