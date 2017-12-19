/*
 * IOHandler.h
 *
 *  Created on: 2010/04/26
 *      Author: disbeat
 */

#pragma once

#include <list>
#include <string.h>
#include "RankingItem.h"
#include "Comparer.h"
#include "Solution.h"

using namespace std;

class IOHandler {

	void formatPaths();

public:

	int n_items;
	int n_instance;
	string type;
	char input[200], output[200];
	char result[200];
	int alfa;
	int nruns;
	int version;

	IOHandler();
	IOHandler(int n_items, int n_instance, string type);
        IOHandler(int n_items, int n_instance, int alfa,int nruns,
                  int version,string &type);
	void setParameters(int n_items, int n_instance, string type);
	bool readInstance(list<RankingItem*> & points, long & cumulativeWeights,
			 multiset<RankingItem*, ComparerProfit0> & Order0, multiset<RankingItem*, ComparerProfit1> & Order1);

	void writeOutput(list<Solution*> * solutionSet, int maxK, double time);
  void writeOutput1(set<Solution*,ComparerM1> solutionSet, double time);
	bool readResult(set<Solution*,ComparerM1> * S,int &num_points);

};

