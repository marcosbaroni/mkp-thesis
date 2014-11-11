#ifndef PCOPE_H
#define PCOPE_H 1

#define COMPUTE_FURTHER_RECUP 1

#include <stdlib.h>

typedef struct PCOPE{
	/* Basic attributes */
	int nacts;             // Number of actions
	int nyears;            // Number of years
	int npers;             // Number of periods per year
	int ntotpers;          // Total number of periods
	int nres;              // Number of resources
	/* Instance */
	double irr;            // Internal rate of return
	double ggoal;          // global goal       (NOT USED YET)
	double *ygoals;        // yearly goal
	double *pgoals;        // periodal goal     (NOT USED YET)
	double *gbudget;       // global budget
	double **ybudgets;     // yearly budget
	double **pbudgets;     // periodal budgets
	/* Actions */
	int *gmarket;          // global market
	int **ymarket;         // yearly market
	int **pmarket;         // periodal market
	double **cost;         // Cost of action (each resource)
	double *evalue;        // energy value of action
	double **recup;        // recuperation curve
}PCOPE;

/* Recovery curve setter fuction */
typedef double* (*curve_f)(
	double*,               // the array
	int,                   // Number of years
	int);                  // Number of periods

typedef struct RandConf{
	int nacts;
	int nyears;
	int npers;
	int nres;
	/* Internal rate of return */
	double irr;
	/* Tir variation for normal distribution */
	double mtir;             // mean tir
	double vtir;             // var tir
	/* Seed for random generation */
	int seed;
	/* Rigid Bound parameters */
	int min_market;                // Minimum market bound (periodal reference)
	int max_market;                // Maximum market bound (periodal reference)
	double min_tot_cost;           // Minimum total cost of an action
	double max_tot_cost;           // Maximum total cost of an action
	/* Recuperation Curves */
	int ncurves;                   // Number of different curves
	double curves_prob[20];        // Probability distribution of curves
	curve_f curves_f[20];          // Function pointers for curve setters
}RandConf;

RandConf *randconf_default(
	int nacts,
	int nyears,
	int npers,
	int nres,
	int seed);
void randconf_free(RandConf *rc);

RandConf *register_curve_f(RandConf *rc, curve_f f, double prob);
RandConf* rc_from_args(int argc, char **argv);

/*** INSTANCE GERENATION ***/
PCOPE *pcope_new(int nacts, int nyears, int npers, int nres); /* Allocs a blank
	problem instance. */
PCOPE *pcope_random(RandConf *rc);


/*** INPUT and OUTPUT ***/
//PCOPE *pcope_from_json(FILE *fin);                  /* JSON inputing */
//void pcope_to_json(PCOPE *pcope, FILE *fout);       /* JSON outputing */
PCOPE *pcope_from_plain(FILE *fin);                 /* plain text inputting */
void pcope_to_plain(PCOPE *p, FILE *fout);          /* plain text outpuing */ 
void pcope_to_scip(PCOPE *p, FILE *fout);           /* SCIP outpuing */ 

typedef struct Solution{
	/*** Basic Variables ***/
    struct PCOPE *pcope;        // The problem instance
	int **x;            		// decision variable [action, period]
	double obj;                 // objective function value
	char viable;                // if solution is viable
	/*** Auxiliary info, to help on computing ***/
	/* Goal */
	double ggoal_left;          // global goal       (NOT USED YET)
	double *ygoals_left;        // yearly goal
	double *pgoals_left;        // periodal goal     (NOT USED YET)
	/* Budget */
	double *gbudget_left;       // global budget
	double **ybudgets_left;     // yearly budget
	double **pbudgets_left;     // periodal budgets
	/* Markets */
	int *gmarket_left;          // global market
	int **ymarket_left;         // yearly market
	int **pmarket_left;         // periodal market
}Solution;

/* IO */
Solution *sol_from_plain(PCOPE *p, FILE *fin);
void sol_to_plain(Solution *sol, FILE *fout);
void sol_fprint(FILE *fout, Solution *sol);
/* Solution manipulation */
Solution *increment_action(Solution *sol, int act, int per);
Solution *decrement_action(Solution *sol, int act, int per);
Solution *rm_action(Solution *sol, int act, int per, int n);
Solution *add_action(Solution *sol, int act, int per, int n);

#endif
