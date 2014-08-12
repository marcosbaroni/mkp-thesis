#ifndef MKP_H
#define MKP_H 1

long MAX_MKP_COEFICIENT;    /* for random generation */

typedef struct MKP{
	int n;       /* Number of itens */
	int m;       /* Number of dimensions */
	long *p;     /* Profit of itens [n] */
	long **w;    /* Weight of itens [m x n] */
	long *b;     /* Knapsack capacities [m] */
}MKP;

/*** memory management ***/
MKP *mkp_alloc(int n, int m);
MKP *mkp_random(int n, int m, double beta);
void mkp_free(MKP *mkp);

/*** READ/WRITE functions ***/
MKP *mkp_read_from_filename(char *filename);
MKP *mkp_read_from_file(FILE *fin);
void mkp_write_to_filename(MKP *mkp, char *filename);
void mkp_write_to_file(MKP *mkp, FILE *fout);

/*** Print functions ***/
void mkp_fprint(FILE *out, MKP *mkp);
void mkp_to_zimpl(FILE *fout, MKP *mkp);

/*** MKP SOLUTION ***/
typedef struct MKPSol{
	long *x;          /* The solution vector [n] */
	long *b_left;     /* Resource left on the knapsack [m] */
	long obj;         /* Objective function */
	int viable;       /* If solution is viable */
	MKP *mkp;         /* The problem instance */
}MKPSol;

MKPSol *mkpsol_new(MKP *mkp); /* new empty solution */
void mkpsol_free(MKPSol *mkpsol); /* new empty solution */

/* MKP TABU SEARCH*/

#endif

