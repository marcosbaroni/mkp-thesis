/*
 * Extension.cpp
 *
 *  Created on: 2010/02/06
 *      Author: disbeat
 */

#include "headers/Extension.h"
#include "headers/GlobalData.h"

Extension::Extension(int p0, int p1) : Solution(){

	std::fill(this->base, this->base + NUM_OBJECTIVES, 0);
	this->weight = 0;

	profits[0] = p0;
	profits[1] = p1;

}


Extension::Extension(Solution * base, int extension0, int extension1) : Solution()
{
	memcpy(this->base, base->profits, NUM_OBJECTIVES);

	this->weight = base->weight;

	profits[0] = this->base[0] + extension0;
	profits[1] = this->base[1] + extension1;
}

Extension::~Extension() { }
