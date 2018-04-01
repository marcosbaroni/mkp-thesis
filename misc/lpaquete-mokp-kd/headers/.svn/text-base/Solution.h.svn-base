/*
 * Solution.h
 *
 *  Created on: 2010/01/16
 *      Author: disbeat
 */

#pragma once

#include <list>
#include "Item.h"
#include "RankingItem.h"

#define NUM_OBJECTIVES 2

using namespace std;

class Solution : public Item {

public:

//	unsigned int* objects;

	Solution();
	virtual ~Solution();

	void addItem(RankingItem * point,int k);
//	void printBinary();
	string toString();

	Solution* clone();
	Solution* clone(RankingItem * p, int k);

//	void addObject(int k);

	void print();
};
