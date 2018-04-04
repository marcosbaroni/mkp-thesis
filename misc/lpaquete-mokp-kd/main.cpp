/*
 *  main.cpp
 *
 *  Created on: 2010/01/13
 *      Author: msimoes
 */


#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <sys/resource.h>
#include "headers/GlobalData.h"
#include "headers/Item.h"
#include "headers/Extension.h"
#include "headers/Solution.h"
#include "headers/RankingItem.h"
#include "headers/Comparer.h"
#include "headers/Bounds.h"
#include "headers/IOHandler.h"
#include "headers/Figueira.h"
#include "../../src/utils/kdtree.h"

#include "../../src/hv-2.0/hv.h"

using namespace std;

list<RankingItem*> points;
multiset<Extension*, ComparerM> F;
long cumulativeWeights = 0;


list<RankingItem*>::iterator pointIt;

int* pointsRemoved;

void printSols(list<Solution*> *sols){
	list<Solution*>::iterator sols_it = sols->begin();
	list<Solution*>::iterator sols_end= sols->end();

	for( ; sols_it != sols_end ; sols_it++ ){
		printf("%d %d\n", (*sols_it)->profits[0], (*sols_it)->profits[1]);
	}

	return;
}

double computeHVol(list<Solution*> *sols){
	list<Solution*>::iterator sols_it = sols->begin();
	list<Solution*>::iterator sols_end= sols->end();

	double hvol;
	double data[2*sols->size()];
	double ref[2];
	int i;

	ref[0] = 0.0;
	ref[1] = 0.0;

	i = 0;
	for( ; sols_it != sols_end ; sols_it++ ){
		data[i++] = (double)-(*sols_it)->profits[0];
		data[i++] = (double)-(*sols_it)->profits[1];
	}

	hvol = fpli_hv(data, 2, sols->size(), ref);

	return hvol;
}

//time stuff
//TODO: use better profiler
double diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=diffticks*1.0/CLOCKS_PER_SEC;
	return diffms;
}

double getCpuTime(void)        
{ 
  struct timeval tim;        
  struct rusage ru;        
  
  getrusage(RUSAGE_SELF, &ru);        
  tim=ru.ru_utime;        
  double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;        
  tim=ru.ru_stime;        
  t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;        
  return t;
}


void reset()
{
	Order0.clear();
	Order1.clear();
	OrderMAX.clear();
	OrderMIN.clear();
	OrderSUM.clear();

	for (F_iterator it = F.begin(); it != F.end(); it++)
		delete(*it);

	F.clear();
	points.clear();

	pointsRemoved = 0;
	cumulativeWeights = 0;

}


void removeDominatedM(Solution * s, M_set * M, M_iterator it)
{

	M_iterator itTemp;
	M_iterator itEnd = M->end();

	for( it++; it != itEnd ; )
	{
		if ((*it)->profits[1] <= s->profits[1])
		{
			itTemp = it;
			it++;
			M->erase(itTemp);
		}
		else
			return;
	}
}

void removeDominatedF(Item * s, F_set * F, F_iterator it)
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


void mantainNonDominatedSet2(Solution *sIN, list<Solution*> *C, KDTree *M){
	Solution * s = sIN->clone();
	Solution *sol;
	KDTreeIter *it;

	it = kdtiter_new(M);
	sol = (Solution*)kdtiter_get(it);

	//M_iterator itEnd = M->end();
	//M_iterator itBegin = M->begin();

	if ( !sol )
	{
		//M->insert(s);
		kdtree_insert(M, sol);
		C->push_back(s);
		return;
	}


	M_iterator it = M->upper_bound(s), tmp;


	if ( it == itBegin )
	{
		it = M->insert(it,s);
		removeDominatedM(s,M,it);
		C->push_back(s);
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
				M->erase(tmp);
				it = M->insert(it, s);
				removeDominatedM(s,M,it);
				C->push_back(s);
			}
			else
			{
				it = M->insert(it, s);
				removeDominatedM(s,M,it);
				C->push_back(s);
			}
		}
		else
			delete(s);
	}
}

/*
 * This function receives a solution and two solution sets,
 * and tries to insert the solution in the sets
 *
 */
void mantainNonDominatedSet(Solution *sIN, list<Solution*> * C, M_set *M)
{
	Solution * s = sIN->clone();

	M_iterator itEnd = M->end();
	M_iterator itBegin = M->begin();

	/* If set M is empty */
	if ( itEnd == itBegin )
	{
		M->insert(s);
		C->push_back(s);
		return;
	}


	/* Find the one with lesser profit[0]
	 * GREATER than s->profit[0] (i.e. the one immediately "above") */
	M_iterator it = M->upper_bound(s), tmp;


	/* If the first on is already above s... */
	if ( it == itBegin )
	{
		/* Insert it */
		it = M->insert(it,s);
		/* Update M (non-dominateds) ... */
		removeDominatedM(s,M,it);
		/* Insert on C list */
		C->push_back(s);
	}
	else
	{
		/* Go back one (this one exists) */
		--it;
		/* If second profit is lower then s's... */
		if ( (*it)->profits[1] < s->profits[1])
		{
			if( (*it)->profits[0] == s->profits[0] )
			{
				tmp = it;
				it++;
				M->erase(tmp);
				it = M->insert(it, s);
				removeDominatedM(s,M,it);
				C->push_back(s);
			}
			else
			{
				it = M->insert(it, s);
				removeDominatedM(s,M,it);
				C->push_back(s);
			}
		}
		else
			delete(s);
	}

}


/*
 * Similar to function mantainNonDominated, but only related to the SolutionSet M
 * (on that function)
 */
void keepNonDominatedSet(Extension *s, multiset<Extension*, ComparerM> * F)
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
		removeDominatedF(s,F,it);
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
				removeDominatedF(s,F,it);
			}
			else
			{
				it = F->insert(it,s);
				removeDominatedF(s,F,it);
			}
		}
		else
			delete(s);
	}
}


/*
 * Remove by bound, vertion that does not compares the bases of the bound
 */

bool removeByBound(Extension * bound, F_set * F, int k){

	F_iterator itEnd = F->end();
	F_iterator itBegin = F->begin();

	F_iterator it = F->upper_bound(bound);

	if(it == itBegin)
		return false;

	--it;
	if ( (*it)->profits[1] <= bound->profits[1] )
		return false;

	return true;
}




void setOrderRanking()
{
	multiset<RankingItem*, ComparerProfit0>::iterator itO0 =Order0.begin(), *itP0;
	multiset<RankingItem*, ComparerProfit1>::iterator itO1 = Order1.begin(), *itP1;
	multiset<RankingItem*, ComparerOrderMAX>::iterator * itMAX;
	multiset<RankingItem*, ComparerOrderSUM>::iterator * itSUM;

	for ( int i = 0; i < GlobalData::NUM_POINTS; i++ )
	{
		itP0 = new multiset<RankingItem*, ComparerProfit0>::iterator();
		itP1 = new multiset<RankingItem*, ComparerProfit1>::iterator();
		*itP0 = itO0;
		*itP1 = itO1;
		(*itO0)->ranking[0] = i+1;
		(*itO1)->ranking[1] = i+1;

		(*itO0)->itProfit0 = (void*)itP0;
		(*itO1)->itProfit1 = (void*)itP1;

		itO0++;
		itO1++;
	}

	list<RankingItem*>::iterator pointsEnd = points.end();
	for ( list<RankingItem*>::iterator it = points.begin(); it != pointsEnd; it++)
	{
		itMAX = new multiset<RankingItem*, ComparerOrderMAX>::iterator();
		itSUM = new multiset<RankingItem*, ComparerOrderSUM>::iterator();

		*itMAX = OrderMAX.insert( *it );
		(*(*itMAX))->itOrderMAX = (void*)itMAX;

		*itSUM = OrderSUM.insert( *it );
		(*(*itSUM))->itOrderSUM = (void*)itSUM;
		//		itO0 = OrderMIN.insert(points[i]);
		//		(*itO0)->addIterator(itO0);
	}
}


/*
 * This function generates a SolutionSet from a previous one by
 * adding a new item to each solution
 */
list<Solution*> * knapsackAlgorithSet(list<Solution*> * Cprev, int k)
{

	list<Solution*> * C = new list<Solution*>();
	M_set * M = new M_set();

	list<Solution*>::iterator i = Cprev->begin(), j = Cprev->begin();
	list<Solution*>::iterator CprevEnd = Cprev->end();

	RankingItem* point = *pointIt;

	//keep order sets with the remaining points only

	Order0.erase(*((multiset<RankingItem*,ComparerProfit0>::iterator *)point->itProfit0));
	Order1.erase(*((multiset<RankingItem*,ComparerProfit1>::iterator *)point->itProfit1));
	OrderMAX.erase(*((multiset<RankingItem*,ComparerOrderMAX>::iterator *)point->itOrderMAX));
	OrderSUM.erase(*((multiset<RankingItem*,ComparerOrderSUM>::iterator *)point->itOrderSUM));

	//end of ordering maintenance

	for ( ; j != CprevEnd; j++ )
		if ( (*j)->weight + cumulativeWeights > GlobalData::MAX_WEIGHT )
			break;

	for ( ; i != CprevEnd; i++ )
	{
		if ( (*i)->weight + point->weight > GlobalData::MAX_WEIGHT )
			break;

		Solution * skv = (*i)->clone(point, k);


		for ( ; j != CprevEnd; j++ )
		{
			if ( ! (*j)->dominatesByWeightAndProfit(skv) )
				break;

			mantainNonDominatedSet(*j, C, M);
		}
		mantainNonDominatedSet(skv, C, M);
		delete(skv);

	}

	for ( ; j != Cprev->end(); j++ )
		mantainNonDominatedSet(*j, C, M);

	for( list<Solution*>::iterator it = Cprev->begin(); it != CprevEnd; it++)
		delete (*it);

	delete ( Cprev );

	list<Solution*>::iterator Cbegin = C->begin();
	list<Solution*>::iterator Cend = C->end();
	M_iterator Mbegin = M->begin();
	M_iterator Mend = M->end();

	//ORDER_SOLUTION->addObject(point->orderNum);

	if (k == GlobalData::NUM_POINTS-1)
	{
		list<Solution*> * result = new list<Solution*>();
		for (M_iterator it = Mbegin; it != Mend; it++)
			result->push_back( (*it)->clone() );

		for (list<Solution*>::iterator it = Cbegin; it != Cend; it++)
			delete(*it);

		delete(M);
		delete(C);

		return result;
	}
	else
	{

	//	int i = 1;
		for (list<Solution*>::iterator Sk= Cbegin; Sk != Cend; )
		{
			Extension * upperBound = computeUpperBound(*Sk);


			if (removeByBound(upperBound, &F, k))
			{
				pointsRemoved[k]++;
				delete(*Sk);
				Sk = C->erase(Sk);

			}
			else
			{
				Sk++;
			}

			delete(upperBound);

		}

	}
	cumulativeWeights -= point->weight;

	pointIt++;

	delete ( M );

	return C;
								}


int main(int argc, char** argv)
{
	//printf("KNAPSACK_FIGUEIRA\n");
	if (argc < 2)
	{
		//printf("USAGE: type size id\n");
		printf("USAGE: <input file>\n");
		printf("OUTPUT:\n");
		printf("<profit1[1]> <profit2[1]>\n");
		printf("<profit1[2]> <profit2[2]>\n");
		printf("...\n");
		printf("<profit1[<nsol>]> <profit2[<nsol>]>\n");
		printf("<nsol>;<max k>;<time (s)>;<h vol.>\n");
		return 0;
	}


	IOHandler ioHandler;

	unsigned int maxK = 0;


	//ioHandler.setParameters(atoi(argv[2]), atoi(argv[3]), argv[1]);
	maxK = 0;

	reset();

	ioHandler.saveInputFileName(argv[1]);
	ioHandler.readInstance( points, cumulativeWeights, Order0, Order1 );

	pointsRemoved = new int[GlobalData::NUM_POINTS];
	for (int i=0; i < GlobalData::NUM_POINTS; i++)
		pointsRemoved[i] = 0;


	double begin=getCpuTime();
	setOrderRanking();

	points.sort(ComparerOrderMAX());

	Figueira::generateExtensions(points, Order0, Order1, F);
	
	int initial_size = F.size();
	
	Solution * base = new Solution();

	list<Solution*> * solutionSet = new list<Solution*>();

	solutionSet->push_back(base);

	pointIt = points.begin();
	
	
	for (int k = 0; k < GlobalData::NUM_POINTS; k++)
	{
		solutionSet = knapsackAlgorithSet(solutionSet, k);

		if (solutionSet->size() > maxK)
			maxK = solutionSet->size();
	}

	double end = getCpuTime();
	double hvol = computeHVol(solutionSet);
	printSols(solutionSet);

	printf("%d;%d;%lf;%.3e\n",
		//ioHandler.type.c_str(),
		//ioHandler.n_items,
		//ioHandler.n_instance,
		(int)solutionSet->size(),
		maxK,
		end-begin,
		hvol
	);

	ioHandler.writeOutput(solutionSet, maxK, end-begin);

	//for (list<Solution*>::iterator it = solutionSet->begin(); it != solutionSet->end(); it++)
	//	(*it)->print();
	
	//cout << "# removed:;" << pointsRemoved << ";# F initial:;" << initial_size << ";# F final: " << F.size() << endl;
	//for (int i=0; i < GlobalData::NUM_POINTS; i++)
	//	cout << pointsRemoved[i] << ';';
	//cout << endl;
	
	return 0;
}
