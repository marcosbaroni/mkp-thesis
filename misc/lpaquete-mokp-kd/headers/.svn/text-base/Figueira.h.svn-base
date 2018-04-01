/*
 * Figueira.h
 *
 *  Created on: 2010/07/06
 *      Author: disbeat
 */

#pragma once

#include <set>
#include <list>
#include <stdio.h>
#include "Extension.h"
#include "Comparer.h"
#include "RankingItem.h"

using namespace std;


//class RankingItem;

class Figueira {

public:

	static void generateLBRelaxed(multiset<RankingItem*, ComparerProfit0>  Order0,
                                multiset<RankingItem*, ComparerProfit1>  Order1);

	static void generateExtensions(list<RankingItem*> & points, multiset<RankingItem*, ComparerProfit0> & Order0,
		multiset<RankingItem*, ComparerProfit1> & Order1, multiset<Extension*, ComparerM> & F);

};
