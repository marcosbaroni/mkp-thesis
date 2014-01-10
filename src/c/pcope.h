#include <stdlib.h>

typedef struct PCOPE{
	int nyears;      // Number of years
	int nperiods;    // Number of periods
	int nresources;  // Number of resources
	double irr;      // Internal rate of return
	double ggoal, *ygoals, *pgoals;        // goals (global, yearly and periodal)
	double gbudget, **ybudgets, **pbudgets;  // budgets (global, yearly and periodal)
	struct Action *acts;    // List of actions
}PCOPE;

typedef struct Action{
	double *cost;    // Cost of action (each resource)
	double evalue;  // energy value of action
	int gmarket;    // global market
	int *ymarket;   // yearly market
	int *pmarket;   // periodal market
	double *recup;  // recuperation curve
	struct PCOPE *pcope;  // The parent problem
}Action;

typedef struct Solution{
	int *x;
	double obj;
	char viable;
    struct PCOPE *pcope;
}Solution;

