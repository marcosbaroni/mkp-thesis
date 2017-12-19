/*
 * RankingItem.cpp
 *
 *  Created on: 2010/01/18
 *      Author: msimoes
 */

#include "headers/RankingItem.h"
#include "headers/GlobalData.h"



RankingItem::RankingItem(int orderNum) {
	this->orderNum = orderNum;
}

RankingItem::RankingItem() {
	this->orderNum = -1;
}

RankingItem::~RankingItem() {
}



int RankingItem::getRankingSum()
{
	int sum = 0;
	for ( int i = 0; i < NUM_OBJECTIVES; i++ )
	{
		sum += ranking[i];
	}

	return sum;
}


double RankingItem::getRankingMax()
{
	int max = ranking[0];

	for ( int i = 1; i < NUM_OBJECTIVES; i++ )
		if (ranking[i] > max)
			max = ranking[i];

	return max + 1.0 / (GlobalData::NUM_POINTS * NUM_OBJECTIVES) * getRankingSum();
}

double RankingItem::getRankingMin()
{
	int min = ranking[0];

	for ( int i = 1; i < NUM_OBJECTIVES; i++ )
		if (ranking[i] < min)
			min = ranking[i];

	return min + 1.0 / (GlobalData::NUM_POINTS * NUM_OBJECTIVES) * getRankingSum();
}

