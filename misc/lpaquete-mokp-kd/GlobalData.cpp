/*
 * GlobalData.cpp
 *
 *  Created on: 2010/01/16
 *      Author: disbeat
 */

#include "headers/GlobalData.h"

int GlobalData::MAX_WEIGHT = 0;
int GlobalData::NUM_POINTS = 0;
int GlobalData::SOLUTION_SIZE = 0;
int GlobalData::COUNTER = 0;
unsigned int *GlobalData::POWVALUES = (unsigned int *) malloc( 8*sizeof(int) * sizeof(int));

void GlobalData::setSOLUTION_SIZE()
{
	SOLUTION_SIZE =  NUM_POINTS / (8.0 * sizeof(int)) > NUM_POINTS / (8 * sizeof(int)) ?
			NUM_POINTS / (8 * sizeof(int) ) + 1 :
			NUM_POINTS / (8 * sizeof(int));

	unsigned int temp = 1;
	for(unsigned int i = 0; i < 8*sizeof(int); i++)
	{
		POWVALUES[i] = temp;
		temp = temp * 2;
	}

}

vector<string> GlobalData::tokenize(string s)
{
	istringstream iss(s);

	vector<string> tokens;
	copy(istream_iterator<string>(iss),
			 istream_iterator<string>(),
			 back_inserter<vector<string> >(tokens));
	return tokens;
}
