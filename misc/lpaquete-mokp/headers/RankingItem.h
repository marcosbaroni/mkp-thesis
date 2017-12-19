/*
 * RankingItem.h
 *
 *  Created on: 2010/01/18
 *      Author: disbeat
 */

#pragma once

#include "Item.h"

#include <list>
#include <set>

using namespace std;


class RankingItem: public Item {
public:	
	int ranking[NUM_OBJECTIVES];
	//iterators for orders: Order0, Order1, OrderMAX, OrderSUM

	int orderNum;

	void * itProfit0;
	void * itProfit1;
	void * itOrderMAX;
	void * itOrderSUM;

  // New orders
  void * itOrderR;
  void * itOrderC0;
  void * itOrderC1;


	RankingItem();
	RankingItem(int orderNum);
	virtual ~RankingItem();

	void setRanking(int index, int value);
	int getRanking(int index);

	int getRankingSum();

	double getRankingMax();

	double getRankingMin();

  // Used in OrderR
	double getRankingR();

	void addIterator(int index, set<RankingItem*>::iterator it);

	multiset<RankingItem*>::iterator getIterator(int index);


};

