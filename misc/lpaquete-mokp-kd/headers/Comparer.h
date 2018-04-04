#pragma once

#include "Item.h"
#include "RankingItem.h"


double itemProfitGetter(void *it_, int dim){
	Item *it = (Item*)it_;
	return it->profits[dim];
}
double itemAxisGetter(void *it_, int dim){
	Item *it = (Item*)it_;
	if( !ndim )
		return it->weigth;
	return it->profits[dim-1];
}

class ComparerM {
  public:
    ComparerM() { }

    // comparison of elements
    bool operator() (Item* t1, Item* t2) const {
    	return t1->profits[0] > t2->profits[0];
    }
};


class ComparerProfit0 {
  public:
    ComparerProfit0() { }

    // comparison of elements
    bool operator() (Item* t1, Item* t2) const {
    	if ( (float)t1->profits[0]/t1->weight > (float)t2->profits[0]/t2->weight )
			return true;
		else if ( (float)t1->profits[0]/t1->weight == (float)t2->profits[0]/t2->weight )
			return ( (float)t1->profits[1]/t1->weight > (float)t2->profits[1]/t2->weight );
    	return false;
    }
};


class ComparerProfit1 {
  public:
    ComparerProfit1() { }

    // comparison of elements
    bool operator() (Item* t1, Item* t2) const {
    	if ( (float)t1->profits[1]/t1->weight > (float)t2->profits[1]/t2->weight )
    		return true;
    	else if ( (float)t1->profits[1]/t1->weight == (float)t2->profits[1]/t2->weight )
    		return ( (float)t1->profits[0]/t1->weight > (float)t2->profits[0]/t2->weight );
    	return false;
    }
};


class ComparerOrderMAX {
  public:
    ComparerOrderMAX() { }

    // comparison of elements
    bool operator() (RankingItem* t1, RankingItem* t2) const {

    	return ( t1->getRankingMax() < t2->getRankingMax() );
    }
};

class ComparerOrderMIN {
  public:
    ComparerOrderMIN() { }

    // comparison of elements
    bool operator() (RankingItem* t1, RankingItem* t2) const {

    	return ( t1->getRankingMin() < t2->getRankingMin() );
    }
};

class ComparerOrderSUM {
  public:
    ComparerOrderSUM() { };

    // comparison of elements
    bool operator() ( RankingItem* t1, RankingItem* t2) const {
    	return ( t1->getRankingSum() < t2->getRankingSum() );
    }
};

class ComparerOrderR {
  public:
    ComparerOrderR() { };

    // comparison of elements
    bool operator() ( RankingItem* t1, RankingItem* t2) const {
    	return ( t1->getRankingR() >= t2->getRankingR() );
    }
};

class ComparerM1 {
  public:
    ComparerM1() { }

    // comparison of elements
    bool operator() (Item* t1, Item* t2) const {
    	return ( (t1->profits[0] > t2->profits[0]) ||
				       ( (t1->profits[0] == t2->profits[0]) &&
                 (t1->profits[1] >= t2->profits[1]) ));         
    }
};

class ComparerC0 {
  public:
    ComparerC0() { }

    // comparison of elements
    bool operator() (Item* t1, Item* t2) const {
    	return t1->profits[0] < t2->profits[0];
    }
};


class ComparerC1 {
  public:
    ComparerC1() { }

    // comparison of elements
    bool operator() (Item* t1, Item* t2) const {
    	return t1->profits[1] < t2->profits[1];
    }
};

class ComparerW {
  public:
    ComparerW() { }

    // comparison of elements
    bool operator() (Item* t1, Item* t2) const {
    	return t1->weight < t2->weight;
    }
};
