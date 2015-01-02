/*******************************************************************************
* DISCRETE EVOLUTION STRATEGY
*******************************************************************************/
#ifndef DES_H
#define DES_H 1

/* Interface
 *  1 - modifing rule of chance throught method iterations "typedef (double)(*des_activate)(double, double)"
 *  2 - set[0-1] function "typedef (void*)(*des_set)(void *, int);"
 *  3 - fitness function "typedef double(*des_fitness)(void*);"
 *  4 - feasibility function "typedef inf(*des_feasible)(void*)";
 * Call: des(
 *  - Problem
 *  - Probability vector
 *  - Interface
 *  - Size of population
 *  - Generated individuals
 *  - Number of iterations
 */

/* callback to activate probability 
 * args: 
 * arg1: trust of value to be 1
 * arg2: current value of variable
 * arg3: step of method
 * output: real probability
 * */
typedef double (*des_activate_f)(double, double, int);

/* callback for setting a given variable of solution to some value */
typedef void* (*des_set_f)(void *, int, int);

/* callback for getting the value of a given variable */
typedef int (*des_get_f)(void *, int);

/* returns the fitness function of solution */
typedef double (*des_fitness_f)(void*);

/* repair function to be used on non feasible solutions */
typedef void* (*des_repair_f)(void*);

/* returns if solution is feasible */
typedef int (*des_feasible_f)(void*);

/* returns a new initial (probably random) solution */
typedef void* (*des_new_solution_f)(void*);

/* copy a solution */
typedef void* (*des_copy_solution_f)(void*);

/* free a solution */
typedef void* (*des_free_solution_f)(void*);

typedef struct DES_Interface{
	des_activate_f activate;
	des_set_f set;
	des_get_f get;
	des_fitness_f fitness;
	des_repair_f repair;
	des_feasible_f feasible;
	des_new_solution_f new_solution;
	des_copy_solution_f copy_solution;
	des_free_solution_f free_solution;
}DES_Interface;

void *des(
	DES_Interface *,  /* the interface */
	double *trust,    /* the trust (for probabilities) */
	void *problem,    /* the problem */
	int nvars,        /* size of solution */
	int popsize,      /* size of population */
	int niter);       /* number of iteration */

#endif

