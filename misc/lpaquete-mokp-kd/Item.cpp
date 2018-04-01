/*
 * Point.cpp
 *
 *  Created on: 2010/01/13
 *      Author: msimoes
 */

#include "headers/Item.h"

Item::Item()
{
}

Item::~Item()
{
}



bool Item::dominates(Item * point)
{
	return point->isDominated(this);
}


bool Item::dominatesByProfit(Item * point)
{
	return point->isDominatedByProfit(this);
}

bool Item::dominatesByWeightAndProfit(Item * point)
{
	return point->isDominatedByWeightAndProfit(this);
}


bool Item::isDominated(Item * point)
{
	for (int p = 0; p < NUM_OBJECTIVES; p++)
	{
		if (profits[p] > point->profits[p])
			return false;
	}

	//return ! equals(point);
	return true;
}

bool Item::isDominatedByProfit(Item * point)
{
	for (int p = 0; p < NUM_OBJECTIVES; p++)
	{
		if (point->profits[p] > profits[p])
			return true;
		else if (point->profits[p] < profits[p])
			return false;
	}
	return true;
}


bool Item::isDominatedByWeightAndProfit(Item * point)
{
	if (point->weight < this->weight)
		return true;
	else if (point->weight > this->weight)
		return false;
	else
		return this->isDominatedByProfit(point);
}


void Item::print()
{
	cout << "[ " << weight;
	for (int i = 0; i < NUM_OBJECTIVES; i++)
		cout << " ; " << profits[i];

	cout << " ]" << endl;
}

bool Item::equals(Item * point)
{
	if (point->weight != weight)
		return false;

	for ( int i = 0; i < NUM_OBJECTIVES; i++ )
		if (point->profits[i] != profits[i])
			return false;
	return true;
}

void Item::add( Item * point )
{
//	points.push_back(point);

	this->weight += point->weight;

	for (int i = 0; i < NUM_OBJECTIVES; i++)
	{
		this->profits[i] += point->profits[i];
	}


}
