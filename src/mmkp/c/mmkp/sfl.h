/*******************************************************************************
* SHUFFLED FROG LEAPING OPTIMIZAION
*******************************************************************************/
#ifndef SFL_H
#define SFL_H 1

/* Interface
 *  1 - modifing rule of chance throught method iterations "typedef (double)(*sfl_activate)(double, double)"
 *  2 - set[0-1] function "typedef (void*)(*sfl_set)(void *, int);"
 *  3 - fitness function "typedef double(*sfl_fitness)(void*);"
 *  4 - feasibility function "typedef inf(*sfl_feasible)(void*)";
 * Call: sfl(
 *  - Problem
 *  - Probability vector
 *  - Interface
 *  - Size of population
 *  - Generated individuals
 *  - Number of iterations
 */

/* callback for setting a given variable of solution to some value */
typedef void* (*sfl_set_f)(void *, int, int);

/* callback for getting the value of a given variable */
typedef int (*sfl_get_f)(void *, int);

/* returns the fitness function of solution */
typedef double (*sfl_fitness_f)(void*);

/* repair function to be used on non feasible solutions */
typedef void* (*sfl_repair_f)(void*);

/* returns if solution is feasible */
typedef int (*sfl_feasible_f)(void*);

/* returns a new initial (probably random) solution */
typedef void* (*sfl_new_solution_f)(void*);

/* copy a solution */
typedef void* (*sfl_copy_solution_f)(void*);

/* free a solution */
typedef void* (*sfl_free_solution_f)(void*);

typedef struct SFL_Interface{
	sfl_set_f sfl_set;
	sfl_get_f sfl_get;
	sfl_fitness_f sfl_fitness;
	sfl_repair_f sfl_repair;
	sfl_feasible_f sfl_feasible;
	sfl_new_solution_f sfl_new_solution;
	sfl_copy_solution_f sfl_copy_solution;
	sfl_free_solution_f sfl_free_solution;
}SFL_Interface;

void *sfl(
	SFL_Interface *sfli; /*SFL Interface */
	void *problem,       /* the problem */
	int nvars,           /* size of solution */
	int popsize,         /* size of population */
	int niter);          /* number of iteration */

#endif

