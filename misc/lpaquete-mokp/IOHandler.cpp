/*
 * IOHandler.cpp
 *
 *  Created on: 2010/04/26
 *      Author: disbeat
 */
#include <fstream>
#include <sstream>
#include "headers/IOHandler.h"
#include "headers/GlobalData.h"


IOHandler::IOHandler() { }

IOHandler::IOHandler(int n_items, int n_instance, string type){
	this->n_items = n_items;
	this->n_instance = n_instance;
	this->type = type;

	formatPaths();

}

void IOHandler::setParameters(int n_items, int n_instance, string type) {
	this->n_items = n_items;
	this->n_instance = n_instance;
	this->type = type;

	formatPaths();
}

void IOHandler::formatPaths() {
	ostringstream in;
	in << "../../INSTANCES/Type_" << this->type << "/" << this->n_items << "_items/2KP" << this->n_items << "-T"<< type <<"-" << 
this->n_instance << ".dat";

	ostringstream out;
	out << "../../INSTANCES/Type_" << this->type << "/" << this->n_items << "_items/2KP" << this->n_items << "-T"<< type <<"-" << 
this->n_instance << "_new.eff";

	strcpy( this->input, in.str().c_str() );
	strcpy( this->output, out.str().c_str() );

}

/*
 * This function reads from a file instances with
 * profits and weight, and the max weight allowed
 */

bool IOHandler::readInstance(
		list<RankingItem*> & points,
		long & cumulativeWeights,
		multiset<RankingItem*, ComparerProfit0> & Order0,
		multiset<RankingItem*, ComparerProfit1> & Order1)
{

	string line;
		vector<string> tokens;

		ifstream instanceFile(this->input, ios::in);

		int orderNum = 0;

		if (instanceFile.is_open())
		{
			RankingItem * point = NULL;
			while (! instanceFile.eof() )
			{
				getline(instanceFile,line);

				istringstream ss(line);
				string identifier;
				ss >> identifier;

				if (identifier.compare("i") == 0)
				{
					int w, p1, p2;
					ss >> w >> p1 >> p2;

					//new point instance
					point = new RankingItem(orderNum);
					point->weight = w;
					point->profits[0] = p1;
					point->profits[1] = p2;
					//point->print();

					//push point to list
					points.push_back(point);

					cumulativeWeights += w;

					Order0.insert(point);
					Order1.insert(point);

					orderNum++;
				}
				else if (identifier.compare("n") == 0)
				{
					ss >> GlobalData::NUM_POINTS;
				}
				else if (identifier.compare("W") == 0)
				{
					ss >> GlobalData::MAX_WEIGHT;
				}
			}
			instanceFile.close();

			return true;
		}
		else
		{
			cout << "Unable to open file: " << this->input;
			return false;
		}

}


void IOHandler::writeOutput(list<Solution*> * solutionSet, int maxK, double time) {

	ofstream output(this->output);


	output << "|ND|: " << solutionSet->size()<< "; max|Ck|: " << maxK << "; Execution time: " << time << "s" << endl;
	for (list<Solution*>::iterator it = solutionSet->begin(); it != solutionSet->end(); it++)
		output << (*it)->toString() << endl;


}
