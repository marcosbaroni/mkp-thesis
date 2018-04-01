/*
 * Extension.h
 *
 *  Created on: 2010/02/06
 *      Author: disbeat
 */

#ifndef EXTENSION_H_
#define EXTENSION_H_

#include "Solution.h"
#include <string.h>

class Extension: public Solution {
public:
	int base[NUM_OBJECTIVES];
//	unsigned int* objects;

//	void addObject(int k);
	Extension(int p0, int p1);
	Extension(Solution * base, int extension0, int extension1);
	~Extension();

//	bool equalsBase(Extension * item, int k, list<RankingItem*>* points, unsigned int * BASE);

};

#endif /* EXTENSION_H_ */
