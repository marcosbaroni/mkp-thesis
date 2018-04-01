/*
 * Bounds.h
 *
 *  Created on: 2011/10/15
 *      Author: aponte
 */

#pragma once

#include <set>
#include <math.h>

#include "RankingItem.h"
#include "Comparer.h"
#include "Extension.h"
#include "GlobalData.h"


multiset<RankingItem*, ComparerProfit0> Order0;
multiset<RankingItem*, ComparerProfit1> Order1;

multiset<RankingItem*, ComparerOrderMAX> OrderMAX;
multiset<RankingItem*, ComparerOrderMIN> OrderMIN;
multiset<RankingItem*, ComparerOrderSUM> OrderSUM;

// New Order
multiset<RankingItem*, ComparerOrderR> OrderR;

multiset<RankingItem*, ComparerW> OrderC0;
multiset<RankingItem*, ComparerW> OrderC1;


typedef multiset<Solution*, ComparerM>::iterator M_iterator;
typedef multiset<Extension*, ComparerM>::iterator F_iterator;
typedef multiset<Solution*, ComparerM> M_set;
typedef multiset<Extension*, ComparerM> F_set;
typedef multiset<RankingItem*, ComparerProfit0>::iterator Order0_iterator;
typedef multiset<RankingItem*, ComparerProfit1>::iterator Order1_iterator;
typedef multiset<RankingItem*, ComparerProfit0>::reverse_iterator Order0_reverse_iterator;
typedef multiset<RankingItem*, ComparerProfit1>::reverse_iterator Order1_reverse_iterator;

#define MAX(x,y) (x>y)?x:y
#define MIN(x,y) (x<y)?x:y

Extension * updateUpperBound( Solution * input, Order0_iterator &s1B, Order1_iterator &s2B, int &sum1Weights,int &sum2Weights, int &sum1Profits,int &sum2Profits){

	Extension * result = new Extension(input, 0, 0);

	int Wsk = GlobalData::MAX_WEIGHT - input->weight;
	int wB1 = 0, wB2 = 0;
	int pB1 =0, pB2 =0;
	int c1,c2;


	Order0_iterator Order0End = Order0.end();
	Order0_iterator Order0PrevEnd = Order0End;
	Order0PrevEnd--;

	Order0_reverse_iterator Order0Rend = Order0.rend();

	Order1_iterator Order1End = Order1.end();
	Order1_iterator Order1PrevEnd = Order1End;
	Order1PrevEnd--;

	Order1_reverse_iterator Order1Rend = Order1.rend();

	if( Wsk > 0){
		Order0_iterator it1 = s1B;
		if(sum1Weights <= Wsk){

			if( it1 != Order0.end()){
				c1 = Wsk - sum1Weights ;
				Order0_iterator C = it1;
				if(it1 != Order0.begin() && it1 != Order0PrevEnd){
					Order0_iterator CplusOne = C; CplusOne++;
					Order0_iterator CminusOne = C; CminusOne--;
					result->profits[0] += sum1Profits + (int) max( c1 * (*CplusOne)->profits[0]/ (1.0 * (*CplusOne)->weight), ((*C)->profits[0]) -  ((*C)->weight - c1) * (*CminusOne)->profits[0]* 1.0/((*CminusOne)->weight*1.0) );
				}else{
					result->profits[0] += sum1Profits + (int) (c1 * (*C)->profits[0] / (1.0 * (*C)->weight));
				}
			}
		}else{
			/*printf("Erreur :Numero ligne %d NOM FICHIER %s\n",__LINE__,__FILE__);*/
			//j1=*s1B-1;
			int i = 0;
			--it1;
			Order0_reverse_iterator rit1(it1);
			--rit1;
			//cout << "it1 " << *it1 << " rit1 " << *rit1 << endl; // ICI
			//press_return();
			wB1 = sum1Weights;
			pB1 = sum1Profits;
			while( rit1 != Order0Rend ){// doit etre le debut
				if (wB1 > Wsk){
					wB1 -= (*rit1)->weight;
					pB1 -= (*rit1)->profits[0];
					rit1++;
					i++;
				}else{
					break;
				}
			}
			//if (rit1 != Order0Rend)
	//			rit1--;
			it1 = rit1.base();// ICI
			//cout << "it1 " << *it1 << " rit1 " << *rit1 << endl;
			//press_return();
			//it1++;


			s1B = it1;
			sum1Weights = wB1;
			sum1Profits = pB1  ;
			c1 = Wsk - sum1Weights ;
			Order0_iterator C = it1;
			if(it1 != Order0.begin() && it1 != Order0PrevEnd){
				Order0_iterator CplusOne = C; CplusOne++;
				Order0_iterator CminusOne = C; CminusOne--;
				result->profits[0] += sum1Profits + (int) max( c1 * (*CplusOne)->profits[0] / (1.0 * (*CplusOne)->weight), ((*CplusOne)->profits[0]) -  ((*C)->weight - c1) * (*CminusOne)->profits[0] * 1.0/((*CminusOne)->weight*1.0) );
			}else{
				result->profits[0] += sum1Profits + (int) (c1 * (*C)->profits[0] / (1.0 * (*C)->weight));
			}
		}


		Order1_iterator it2(s2B);
		if(sum2Weights <= Wsk){

			if( it2 != Order1.end()){
				c2 = Wsk - sum2Weights ;
				Order1_iterator C = it2;
				if(it2 != Order1.begin() && it2 != Order1PrevEnd){
					Order1_iterator CplusOne = C; CplusOne++;
					Order1_iterator CminusOne = C; CminusOne--;
					result->profits[1] += sum2Profits + (int) max( c2 * (*CplusOne)->profits[1]/ (1.0 * (*CplusOne)->weight), ((*C)->profits[1]) -  ((*C)->weight - c2) * (*CminusOne)->profits[1]* 1.0/((*CminusOne)->weight*1.0) );
				}else{
					result->profits[1] += sum2Profits + (int) (c2 * (*C)->profits[1] / (1.0 * (*C)->weight));
				}
			}
		}
		else
		{
			--it2;
			Order1_reverse_iterator rit2(it2);// ICI
			--rit2;

			wB2 = sum2Weights;
			pB2 = sum2Profits;
			while( rit2 != Order1Rend ){// doit etre le debut
				if (wB2 > Wsk){
					wB2 -= (*rit2)->weight;
					pB2 -= (*rit2)->profits[1];
					rit2++;
				}else{
					break;
				}
			}
			it2 = rit2.base();// ICI
			//cout << "it2 " << *it2 << " rit2 " << *rit2 << endl;
			//it2++;


			s2B = it2;
			sum2Weights = wB2;
			sum2Profits = pB2  ;
			c2 = Wsk - sum2Weights ;
			Order1_iterator C = it2;
			if(it2 != Order1.begin() && it2 != Order1PrevEnd){
				Order1_iterator CplusOne = C; CplusOne++;
				Order1_iterator CminusOne = C; CminusOne--;
				result->profits[1] += sum2Profits + (int) max( c2 * (*CplusOne)->profits[1]/ (1.0 * (*CplusOne)->weight), ((*C)->profits[1]) -  ((*C)->weight - c2) * (*CminusOne)->profits[1]* 1.0/((*CminusOne)->weight*1.0) );
			}else{
				result->profits[1] += sum2Profits + (int) (c2 * (*C)->profits[1] / (1.0 * (*C)->weight));
			}
		}

	}

	return result;
}



Extension * calculateBorne( Solution * input, Order0_iterator &s1B, Order1_iterator &s2B, int &sum1Weights,int &sum2Weights, int &sum1Profits,int &sum2Profits)
{
	Extension * result = new Extension(input, 0, 0);
	int Wsk = GlobalData::MAX_WEIGHT - input->weight;
	int wB1 = 0, wB2 = 0;
	int pB1 =0, pB2 =0;
	int c1,c2;
	bool allInserted;

	Order0_iterator Order0End = Order0.end();
	Order0_iterator Order0PrevEnd = Order0End;
	Order0PrevEnd--;

	Order1_iterator Order1End = Order1.end();
	Order1_iterator Order1PrevEnd = Order1End;
	Order1PrevEnd--;


	Order0_iterator it1 = Order0.begin();


	if( Wsk > 0){
		allInserted = true;

		while( it1 != Order0End){

			if (wB1 <= Wsk){
				wB1 += (*it1)->weight;
				pB1 += (*it1)->profits[0];
				++it1;
			}else{
				allInserted = false;
				break;
			}
		}

		if(it1 == Order0End && wB1 > Wsk){
			allInserted = false;
		}

		if(allInserted)
		{
			s1B = it1;
			sum1Weights = wB1;
			sum1Profits = pB1;
			result->profits[0] = pB1;
		}else{
			--it1;

			s1B = it1;
			sum1Weights = wB1 - (*it1)->weight ;
			sum1Profits = pB1 - (*it1)->profits[0] ;

			c1 = Wsk - sum1Weights ;

			Order0_iterator C = it1;

			if( it1 != Order0.begin() && it1 != Order0PrevEnd )
			{
				Order0_iterator CplusOne = C; CplusOne++;
				Order0_iterator CminusOne = C; CminusOne--;

				result->profits[0] = pB1 + (int) (max( c1 * (*CplusOne)->profits[0] / (1.0 *  (*CplusOne)->weight ) ,(*C)->profits[0] - ( (*C)->weight - c1) * (*CminusOne)->profits[0] * 1.0 / (*CminusOne)->weight ));
			}else{
				result->profits[0] = pB1 + (int) (c1 * (*C)->profits[0] / (1.0 * (*C)->weight));
			}
		}

		Order1_iterator it2 = Order1.begin();
		allInserted = true;
		while( it2 != Order1End )
		{
			if (wB2 <= Wsk){
				wB2 += (*it2)->weight;
				pB2 += (*it2)->profits[1];
				++it2;
			}else{
				allInserted = false;
				break;
			}
		}
		if(it2 == Order1End && wB2 > Wsk){
			allInserted = false;
		}

		if(allInserted){
			s2B = it2;
			sum2Weights = wB2;
			sum2Profits = pB2;
			result->profits[1] = pB2;
		}else{
			--it2;
			s2B = it2;
			sum2Weights = wB2 - (*it2)->weight ;
			sum2Profits = pB2 - (*it2)->profits[1] ;
			c2 = Wsk - sum2Weights ;
			Order1_iterator C = it2;
			if(it2 != Order1.begin() && it2 != Order1PrevEnd){
				Order1_iterator CplusOne = C; CplusOne++;
				Order1_iterator CminusOne = C; CminusOne--;
				result->profits[1] = pB2 + (int) (max( c2 * (*CplusOne)->profits[1] / (1.0 *  (*CplusOne)->weight ) ,(*C)->profits[1] - ( (*C)->weight - c2) * (*CminusOne)->profits[1] * 1.0 / (*CminusOne)->weight ));
			}else{
				result->profits[1] = pB2 + (int) (c2 * (*C)->profits[1] / (1.0 * (*C)->weight));
			}
		}
	}else{
		result->profits[0] = 0;
		result->profits[1] = 0;
	}

	result->profits[0] = result->profits[0] + input->profits[0];
	result->profits[1] = result->profits[1] + input->profits[1];

	return result;
}





Extension * computeUpperBound(Solution * input)
{
	Extension * result = new Extension(input, 0, 0);

	int Wsk = GlobalData::MAX_WEIGHT - input->weight;
	int wB1 = 0, wB2 = 0;
	int pB1 =0, pB2 =0;
	int c1,c2;
	bool allInserted;

	multiset<RankingItem*, ComparerProfit0>::iterator it1 = Order0.begin();

	if( Wsk > 0){
		allInserted = true;

		while( it1 != Order0.end())
		{
			if (wB1 <= Wsk){
				wB1 += (*it1)->weight;
				pB1 += (*it1)->profits[0];
				it1++;
			}else{
				allInserted = false;
				break;
			}
		}

		if(it1 == Order0.end() && wB1 > Wsk){
			allInserted = false;
		}

		if(allInserted){
			result->profits[0] = pB1;
		}else{

			--it1;

			multiset<RankingItem*, ComparerProfit0>::iterator C = it1;

			wB1 -= (*it1)->weight;
			pB1 -= (*it1)->profits[0];
			c1 = Wsk - wB1 ;

			multiset<RankingItem*, ComparerProfit0>::iterator end0 = Order0.end();
			end0--;


			if( C != Order0.begin() && C !=  end0 ){
				multiset<RankingItem*, ComparerProfit0>::iterator CminusOne = C; CminusOne--;
				multiset<RankingItem*, ComparerProfit0>::iterator CplusOne = C; CplusOne++;

				result->profits[0] = pB1 + (int) (MAX( c1 * (*CplusOne)->profits[0] / (1.0 *  (*CplusOne)->weight ) ,(*C)->profits[0] - ( (*C)->weight - c1) * (*CminusOne)->profits[0] * 1.0 / (*CminusOne)->weight ));
			}else{
				result->profits[0] = pB1 + (int) (c1 * (*C)->profits[0] / (1.0 * (*C)->weight));
			}
		}

		multiset<RankingItem*, ComparerProfit1>::iterator it2 = Order1.begin();
		allInserted = true;
		while( it2 != Order1.end()){
			if (wB2 <= Wsk){
				wB2 += (*it2)->weight;
				pB2 += (*it2)->profits[1];
				++it2;
			}else{
				allInserted = false;
				break;
			}
		}
		if(it2 == Order1.end() && wB2 > Wsk){
			allInserted = false;
		}

		if(allInserted){
			result->profits[1] = pB2;
		}else{
			--it2;

			multiset<RankingItem*, ComparerProfit1>::iterator C = it2;

			wB2 -= (*it2)->weight ;
			pB2 -= (*it2)->profits[1];
			c2 = Wsk - wB2;

			multiset<RankingItem*, ComparerProfit1>::iterator end1 = Order1.end();
			end1--;

			if( C != Order1.begin() && C !=  end1){
				multiset<RankingItem*, ComparerProfit1>::iterator CminusOne = C; CminusOne--;
				multiset<RankingItem*, ComparerProfit1>::iterator CplusOne = C; CplusOne++;

				result->profits[1] = pB2 + (int) (MAX( c2 * (*CplusOne)->profits[1] / (1.0 *  (*CplusOne)->weight ) ,(*C)->profits[1] - ( (*C)->weight - c2) * (*CminusOne)->profits[1] * 1.0 / (*CminusOne)->weight ));
			}else{
				result->profits[1] = pB2 + (int) (c2 * (*C)->profits[1] / (1.0 * (*C)->weight));
			}
		}
	}else{
		/*Le sac est plein !*/
		result->profits[0] = 0;
		result->profits[1] = 0;
	}

	result->profits[0] = result->profits[0] + input->profits[0];
	result->profits[1] = result->profits[1] + input->profits[1];

	//cout << "bound: " << result->profits[0] << " " << result->profits[1] << " | " << result->weight << endl;
	return result;

}


Extension * computeUpperBound1(Solution * input)
{
	Extension * result = new Extension(input, 0, 0);

	int Wsk = GlobalData::MAX_WEIGHT - input->weight;
	int pB0 = 0, pB1 =0;
	int wB0 = 0, wB1 =0;
	int c0,c1;
	bool allInserted;

  multiset<RankingItem *>::iterator it0, ita;
	if( Wsk > 0){
		allInserted = false;

    RankingItem ri = RankingItem();
	  ri.weight = Wsk;
    it0 = OrderC0.upper_bound(&ri);
    if (it0 != OrderC0.begin()) {
      ita = it0;
		  ita--;
      pB0 = (*ita)->profits[0];
      wB0 = (*ita)->weight;
			if (it0 == OrderC0.end()) {
				if (wB0 == Wsk) 
        	it0--;
        else
        	allInserted = true;
      }    
		}

    //cout << "allInserted = " << allInserted << endl;
    //cout << "Wsk = " << Wsk << endl;
    //cout << "initOrderC0 = " << initOrderC0 << endl;*/

		if(allInserted){
			result->profits[0] = pB0;
		}else{
      //cout << "weight = " << wB0 << endl;
      //cout << "profits[0] = " << pB0 << endl;

			multiset<RankingItem*, ComparerProfit0>::iterator C = 
      *((multiset<RankingItem*, ComparerProfit0>::iterator *) (*it0)->itProfit0);

			c0 = Wsk - wB0;
			multiset<RankingItem*, ComparerProfit0>::iterator end0 = Order0.end();
			end0--;

      //cout << "C->weight = " << (*C)->weight << endl;

			if( C != Order0.begin() && C !=  end0 ){
				multiset<RankingItem*, ComparerProfit0>::iterator CminusOne = C; CminusOne--;
				multiset<RankingItem*, ComparerProfit0>::iterator CplusOne = C; CplusOne++;

				result->profits[0] = pB0 + (int) (MAX( c0 * (*CplusOne)->profits[0] / (1.0 *  (*CplusOne)->weight ) ,(*C)->profits[0] - ( (*C)->weight - c0) * (*CminusOne)->profits[0] * 1.0 / (*CminusOne)->weight ));
			}else{
				result->profits[0] = pB0 + (int) (c0 * (*C)->profits[0] / (1.0 * (*C)->weight));
			}
		}

		multiset<RankingItem *>::iterator it1;
		allInserted = false;

    it1 = OrderC1.upper_bound(&ri);
    if (it1 != OrderC1.begin()) {
			ita = it1;
		  ita--;
      pB1 = (*ita)->profits[1];
      wB1 = (*ita)->weight;
			if (it1 == OrderC1.end()) {
				if (wB1 == Wsk)
        	it1--;
				else
        	allInserted = true;
      }
    }

		if(allInserted){
			result->profits[1] = pB1;
		}else{
 			multiset<RankingItem*, ComparerProfit1>::iterator C = 
      *((multiset<RankingItem*, ComparerProfit1>::iterator *) (*it1)->itProfit1);

			c1 = Wsk - wB1 ;
			multiset<RankingItem*, ComparerProfit1>::iterator end1 = Order1.end();
			end1--;

			if( C != Order1.begin() && C !=  end1 ){
				multiset<RankingItem*, ComparerProfit1>::iterator CminusOne = C; CminusOne--;
				multiset<RankingItem*, ComparerProfit1>::iterator CplusOne = C; CplusOne++;

				result->profits[1] = pB1 + (int) (MAX( c1 * (*CplusOne)->profits[1] / (1.0 *  (*CplusOne)->weight ) ,(*C)->profits[1] - ( (*C)->weight - c1) * (*CminusOne)->profits[1] * 1.0 / (*CminusOne)->weight ));
			}else{
				result->profits[1] = pB1 + (int) (c1 * (*C)->profits[1] / (1.0 * (*C)->weight));
			}
    }

	}else{
		/*Le sac est plein !*/
		result->profits[0] = 0;
		result->profits[1] = 0;
	}

	result->profits[0] = result->profits[0] + input->profits[0];
	result->profits[1] = result->profits[1] + input->profits[1];

	//cout << "bound: " << result->profits[0] << " " << result->profits[1] << " | " << result->weight << endl;
	return result;

}



