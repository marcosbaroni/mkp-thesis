/*
 * Solution.cpp
 *
 *  Created on: 2010/01/16
 *      Author: disbeat
 */

#include "headers/Solution.h"
#include "headers/GlobalData.h"

Solution::Solution() : Item()
{
	for ( int p = 0; p < NUM_OBJECTIVES; p++ )
		profits[p] = 0;

	weight = 0;


}

Solution::~Solution() { }


void Solution::addItem( RankingItem * point, int k )
{
	this->weight += point->weight;

	for ( int p = 0; p < NUM_OBJECTIVES; p++ )
		this->profits[p] += point->profits[p];

}


Solution* Solution::clone()
{
	Solution * clone = new Solution();

	clone->weight = this->weight;

	for (int i = 0; i < NUM_OBJECTIVES; i++)
		clone->profits[i] = this->profits[i];


	return clone;
}

Solution* Solution::clone( RankingItem *p, int k )
{
	Solution * res = new Solution();

	res->weight = this->weight + p->weight ;

	for (int i = 0; i < NUM_OBJECTIVES; i++)
	{
		res->profits[i] =  this->profits[i] + p->profits[i] ;
	}

	return res;
}


void Solution::print()
{
	cout << profits[0] << "\t" << profits[1] << endl;
}

string Solution::toString()
{
	ostringstream ss;

	ss << weight << "\t";

	for (int i = 0; i < NUM_OBJECTIVES; i++)
		ss << profits[i] << "\t";

	return ss.str();
}
