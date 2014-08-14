#ifndef MKP_H
#define MKP_H 1

long MAX_MKP_COEFICIENT;    /* for random generation */

/*** MKP PROBLEM INSTANCE ***/
typedef struct MKP{
	int n;       /* Number of itens */
	int m;       /* Number of dimensions */
	long *p;     /* Profit of itens [n] */
	long **w;    /* Weight of itens [m x n] */
	long *b;     /* Knapsack capacities [m] */
}MKP;

/*** Memory Management ***/
MKP *mkp_alloc(int n, int m);
MKP *mkp_random(int n, int m, double beta);
void mkp_free(MKP *mkp);

/*** read/write functions ***/
MKP *mkp_read_from_filename(char *filename);
MKP *mkp_read_from_file(FILE *fin);
void mkp_write_to_filename(MKP *mkp, char *filename);
void mkp_write_to_file(MKP *mkp, FILE *fout);

/*** Print functions ***/
void mkp_fprint(FILE *fout, MKP *mkp);
void mkp_to_zimpl(FILE *fout, MKP *mkp);


/*** MKP SOLUTION ***/
typedef struct MKPSol{
	long *x;          /* The solution vector [n] */
	long *b_left;     /* Resource left on the knapsack [m] */
	long obj;         /* Objective function */
	int viable;       /* If solution is viable */
	MKP *mkp;         /* The problem instance */
}MKPSol;

MKPSol *mkpsol_new(MKP *mkp);                   /* new empty solution */
MKPSol *mkpsol_add_item(MKPSol *mkpsol, int a); /* add item */
MKPSol *mkpsol_rm_item(MKPSol *mkpsol, int a);  /* remove item */
MKPSol *mkpsol_copy(MKPSol *mkpsol);            /* copies a solution */
MKPSol *mkpsol_read_from_filename(char *filename, MKP *mkp);
MKPSol *mkpsol_read_from_file(FILE *fin, MKP *mkp);
void mkpsol_print(FILE *fout, MKPSol *mkpsol);  /* prints a solution */
void mkpsol_free(MKPSol *mkpsol);               /* new empty solution */

/* MKP TABU SEARCH*/
MKPSol *tabu_mkp(MKPSol *mkpsol);

#endif

