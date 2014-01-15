#ifndef PCOPE_H
#define PCOPE_H 1

#include <stdlib.h>

typedef struct Action{
	double *cost;   // Cost of action (each resource)
	double evalue;  // energy value of action
	int gmarket;    // global market
	int *ymarket;   // yearly market
	int *pmarket;   // periodal market
	double *recup;  // recuperation curve
	struct PCOPE *pcope;  // The parent problem
}Action;

typedef struct PCOPE{
	int nacts;       // Number of actions
	int nyears;      // Number of years
	int nperiods;    // Number of periods per year
	int ntotpers;    // Total number of periods
	int nresources;  // Number of resources
	double irr;      // Internal rate of return
	double ggoal, *ygoals, *pgoals;        // goals (global, yearly and periodal)
	double gbudget, **ybudgets, **pbudgets;  // budgets (global, yearly and periodal)
	struct Action **acts;    // List of actions
}PCOPE;

void pcope_print_json(PCOPE *pcope, FILE *fout);
PCOPE *pcope_from_json(FILE *fin);

typedef struct Solution{
	int *x;
	double obj;
	char viable;
    struct PCOPE *pcope;
}Solution;

#endif

