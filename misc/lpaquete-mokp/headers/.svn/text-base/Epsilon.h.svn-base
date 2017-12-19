#include <iostream>
#include <set>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <limits.h>
#include <stdlib.h>
#include <sys/resource.h>

#include "Comparer.h"
#include "Item.h"
#include "Solution.h"
#include "GlobalData.h"

#define TEST_ONLY 0
#define ALL 1


struct interval {
	int s,e;
};

class Epsilon {

public:
	Epsilon();

	list<Solution *> * selectBest(list<Solution *> *a,int alfa);
  void createMatrix(list<Solution *> a,set<Solution *,ComparerM1> b,int q,vector<int>& eps_v);

private:
	void selectNDSet(list<Solution *> * ndS,list<Solution *> * a);

  list<Solution *> * selectEpsilon(list<Solution *> *a,int alfa);

	bool test(int lines,int cols,int eps_i,int alfa,list<Solution *> *a,
				    list<interval>& v,list<Solution *> * ri,int mode);

  int find_subset(list<interval> v,list<Solution *> *a,list<Solution *> *ri,
                  int alfa,int mode);

	void extends_set(list<Solution *> * ri,int alfa,list<Solution *> * a);

	void differenceSetSpecial(list<Solution *> * a,list<Solution *> * ri,vector<int> n);

};
