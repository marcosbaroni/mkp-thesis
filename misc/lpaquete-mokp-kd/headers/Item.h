/*
 * Point.h
 *
 *  Created on: 2010/01/13
 *      Author: msimoes
 */

#pragma once

#include <vector>
#include <stdlib.h>
#include <iostream>


using namespace std;

#define NUM_OBJECTIVES 2



class Item {


public:
	int weight;
	int profits[NUM_OBJECTIVES];


	Item();
	virtual ~Item();

	void add(Item * point);

	bool isDominated(Item* point);
	bool isDominatedByProfit(Item* point);
	bool isDominatedByWeightAndProfit(Item* point);

	bool dominates(Item* point);
	bool dominatesByProfit(Item* point);
	bool dominatesByWeightAndProfit(Item* point);

	bool equals(Item * point);
	void print();
};
