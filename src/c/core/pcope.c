#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"
#include "pcope.h"

/*      CURVE 1
**  |                            
**  |    ***************
**  | ***                            
**  +---|---|---|---|---> (Years) */
double *curve1(double *v, int nyears, int npers){
	int j, k;
	double fst, others;

	fst = randd2(0.6, 0.4)/npers;
	others = randd2(0.9, 0.1)/npers;
	
	for( k = 0 ; k < npers ; k++ )
		v[k] = fst;
	for( j = 1 ; j < nyears ; j++ )
		for( k = 0 ; k < npers ; k++ )
			v[j*npers+k] = others;
	return v;
}

/*      CURVE 2
**  |   ****                     
**  |***    ****        
**  |           ****                 
**  +---|---|---|---|---> (Years) */
double *curve2(double *v, int nyears, int npers){
	int j, k;
	double r1, r2, r3, r4;
	r1 = randd2(0.2, 0.2)/npers;
	r2 = randd2(0.6, 0.3)/npers;
	r3 = randd2(0.3, 0.3)/npers;
	r4 = randd2(0.1, 0.2)/npers;

	/* 1º year */
	for( k = 0 ; k < npers ; k++ )
		v[k] = r1;

	/* 2º year */
	if( nyears > 1 )
		for( k = 0 ; k < npers ; k++ )
			v[npers+k] = r2;
	/* 3º year */
	if( nyears > 2 )
		for( k = 0 ; k < npers ; k++ )
			v[2*npers+k] = r3;

	/* 4º year */
	if( nyears > 3 )
		for( k = 0 ; k < npers ; k++ )
			v[3*npers+k] = r4;

	/* others years */
	for( j = 4 ; j < nyears ; j++ )
		for( k = 0 ; k < npers ; k++ )
			v[j*npers+k] = 0.0;

	return v;
}

/*      CURVE 3
**  |****                     
**  |    ****        
**  |        ****                 
**  +---|---|---|---|---> (Years) */
double *curve3(double *v, int nyears, int npers){
	int j, k;
	double r1, r2, r3, r4;
	r1 = randd2(0.8, 0.2)/npers;
	r2 = randd2(0.5, 0.2)/npers;
	r3 = randd2(0.1, 0.2)/npers;

	/* 1º year */
	for( k = 0 ; k < npers ; k++ )
		v[k] = r1;

	/* 2º year */
	if( nyears > 1 )
		for( k = 0 ; k < npers ; k++ )
			v[npers+k] = r2;
	/* 3º year */
	if( nyears > 2 )
		for( k = 0 ; k < npers ; k++ )
			v[2*npers+k] = r3;

	/* others years */
	for( j = 3 ; j < nyears ; j++ )
		for( k = 0 ; k < npers ; k++ )
			v[j*npers+k] = 0.0;

	return v;
}

RandConf* rc_from_args(int argc, char **argv){
	/* Basic */
	RandConf *rc = randconf_default(
		atoi(argv[1]),             // nacts
		atoi(argv[2]),             // nyears
		atoi(argv[3]),             // npers
		atoi(argv[4]),             // nres
		(argc > 5) ?               // seed
			atoi(argv[5]) :
			time(NULL));
	/* Others */
	if(argc > 6)
		rc->irr = atof(argv[6]);
	if(argc > 7)
		rc->mtir = atof(argv[7]);
	if(argc > 8)
		rc->vtir = atof(argv[8]);
	if(argc > 9)	
		rc->seed = atoi(argv[9]);

	return rc;
}

RandConf *register_curve_f( RandConf *rc, curve_f f, double prob){
	rc->curves_prob[rc->ncurves] = prob;
	rc->curves_f[rc->ncurves] = f;
	rc->ncurves++;
	return rc;
}

RandConf *randconf_default(
	int nacts,
	int nyears,
	int npers,
	int nres,
	int seed){
	RandConf *rc = (RandConf*)malloc(sizeof(RandConf));
	/* Basic attributtes */
	rc->seed = seed;
	rc->nacts = nacts;
	rc->nyears = nyears;
	rc->npers = npers;
	rc->nres = nres;

	/* Internal rate of return */
	rc->irr = 0.;

	/* Tir variation */
	rc->mtir = 0.0;
	rc->vtir = 0.0;

	/* Market range */
	rc->min_market = 5;
	rc->max_market = 30;

	/* Cost range*/
	rc->min_tot_cost = 10.0;
	rc->max_tot_cost = 100.0;

	/* Setting curve creators */
	rc->ncurves = 0;
	register_curve_f(rc, curve1, 0.4);
	register_curve_f(rc, curve2, 0.3);
	register_curve_f(rc, curve3, 0.3);

	return rc;
}

void randconf_free(RandConf *rc){
	free(rc);
	return;
}

/* Allocs a blank problem instance. */
PCOPE *pcope_new(int nacts, int nyears, int npers, int nres){
	int i, j, k, l;
	int ntotpers;
	PCOPE *p;

	p = (PCOPE*)malloc(sizeof(PCOPE));

	/*** Basic attributes ***/
	p->nacts = nacts;
	p->nyears = nyears;
	p->npers = npers;
	p->nres = nres;
	p->ntotpers = ntotpers = npers*nyears;

	/*** Instance attributes ***/
	/* Yearly goal */
	p->ygoals = (double*)malloc(nyears*sizeof(double));
	/* Periodal goal */
	p->pgoals = (double*)malloc(ntotpers*sizeof(double));
	/* Global budget */
	p->gbudget = (double*)malloc(nres*sizeof(double));
	/* Yearly budget */
	p->ybudgets = (double**)malloc(nres*sizeof(double*));
	for( j = 0 ; j < nres ; j++ )
		p->ybudgets[j] = (double*)malloc(nyears*sizeof(double));
	/* Periodal budget */
	p->pbudgets = (double**)malloc(nres*sizeof(double*));
	for( j = 0 ; j < nres ; j++ )
		p->pbudgets[j] = (double*)malloc(ntotpers*sizeof(double));

	/*** Actions ***/
	/* Global market */
	p->gmarket = (int*)malloc(nacts*sizeof(int));
	/* Yearly market */
	p->ymarket = (int**)malloc(nacts*sizeof(int*));
	for( i = 0 ; i < nacts ; i++ )
		p->ymarket[i] = (int*)malloc(nyears*sizeof(int));
	/* Periodal market */
	p->pmarket = (int**)malloc(nacts*sizeof(int*));
	for( i = 0 ; i < nacts ; i++ )
		p->pmarket[i] = (int*)malloc(ntotpers*sizeof(int));
	/* Cost */
	p->cost = (double**)malloc(nacts*sizeof(double*));
	for( i = 0 ; i < nacts ; i++ )
		p->cost[i] = (double*)malloc(nres*sizeof(double));
	/* Energy value */
	p->evalue = (double*)malloc(nacts*sizeof(double));
	/* Recovery curve */
	p->recup = (double**)malloc(nacts*sizeof(double*));
	for( i = 0 ; i < nacts ; i++ )
		p->recup[i] = (double*)malloc(ntotpers*sizeof(double));

	return p;
}


/*
 * Frees a problem instance.
*/
void pcope_free(PCOPE *p){
	int i;

	/* Instance */
	for( i = 0 ; i < p->nres; i++){
		free(p->ybudgets[i]);
		free(p->pbudgets[i]);
	}
	free(p->ygoals);
	free(p->pgoals);
	free(p->gbudget);
	free(p->ybudgets);
	free(p->pbudgets);

	/* Actions */
	for( i = 0 ; i < p->nacts ; i++ ){
		free(p->ymarket[i]);
		free(p->pmarket[i]);
		free(p->cost[i]);
		free(p->recup[i]);
	}
	free(p->gmarket);
	free(p->ymarket);
	free(p->pmarket);
	free(p->cost);
	free(p->evalue);
	free(p->recup);

	free(p);

	return;
}

curve_f get_rand_curve_f(RandConf *rc){
	return rc->curves_f[distributed_rand_int(rc->curves_prob, rc->ncurves)];
}

void set_random_acts(RandConf *rc, PCOPE *p){
	int i, j, k, l;
	int cls, tot_market;
	int nacts, nyears, npers, nres, ntotpers;
	double tot_cost, tir, daux;
	double profit, recup;
	curve_f f;

	nacts = p->nacts;
	nyears = p->nyears;
	npers = p->npers;
	nres = p->nres;
	ntotpers = nyears*npers;

	for( i = 0 ; i < nacts ; i++ ){
		/* Total Market */
		tot_market = randint(rc->min_market*ntotpers, rc->max_market*ntotpers);
		p->gmarket[i] = tot_market;

		/* Random Yearly market */
		 int_rand_fill_with_total(
		 	p->ymarket[i],                      // the array
			nyears,                             // array size
		 	rint(tot_market*randd2(1.0, 1.0)),  // total
			randd());                           // min rate

		/* Random Periodal market */
		 int_rand_fill_with_total(
		 	p->pmarket[i],                      // the array
			ntotpers,                           // array size
		 	rint(tot_market*randd2(1.0, 1.0)),  // total
			randd());                           // min rate

		/* Random total cost */
		tot_cost = randd2(rc->min_tot_cost, rc->max_tot_cost);

		/* Random Resources Cost */
		double_rand_fill_with_total(
			p->cost[i],                         // the array
			nres,                               // array size
			tot_cost,                           // total
			randd());                           // min rate

		/* Tir */
		tir = 1.0;

		/* Energy Value */
		p->evalue[i] = randd2(0.2, 0.5);

		/* Energy Recuperation */
		// Choosing class of recuperation curve
		f = get_rand_curve_f(rc);
		f(p->recup[i], nyears, npers);

		// Computing internal return (profit)
		profit = 0;
		daux = 1.0;
		for( k = 0 ; k < ntotpers ; k++ ){
			profit += p->recup[i][k]/daux;
			daux *= (1+tir);
		}
		// Setting recuperation
		profit *= p->evalue[i];
		profit /= tot_cost;
		for( k = 0 ; k < ntotpers ; k++ )
			p->recup[i][k] /= profit;
	}

	return;
}

void set_random_budgets(RandConf *rc, PCOPE *p){
	int l, i, nacts, nres;
	double daux;

	nres = p->nres;
	nacts = p->nacts;

	/*** Setting budgets ***/
	for( l = 0 ; l < nres ; l++ ){
		// Computing overall cost for all actions (global market)
		daux = 0;
		for( i = 0 ; i < nacts ; i++ )
			daux += p->cost[i][l]*p->gmarket[i];

		/* Global budgets */
		p->gbudget[l] = daux*randd2(0.5, 0.3);

		/* Yearly budgets */
		double_rand_fill_with_total(
			p->ybudgets[l],
			p->nyears,
			daux,
			randd2(0.5, 0.4));

		/* Periodal budgets */
		double_rand_fill_with_total(
			p->pbudgets[l],
			p->ntotpers,
			daux,
			randd2(0.5, 0.4));
	}

	return;
}

void set_random_goals(RandConf *rc, PCOPE *p){
	int i, k, nacts, ntotpers;
	double daux;
	
	nacts = p->nacts;
	ntotpers = p->ntotpers;

	// Computing overall energy recuperation
	daux = 0;
	for( i = 0 ; i < nacts ; i++ )
		for( k = 0 ; k < ntotpers ; k++ )
			daux += p->gmarket[i]*p->recup[i][k];

	// Global goal
	p->ggoal = daux*randd2(0.5, 0.3);

	// Yearly goals
	double_rand_fill_with_total(
		p->ygoals,
		p->nyears,
		daux,
		randd2(0.5, 0.4));

	// Periodal goals
	double_rand_fill_with_total(
		p->pgoals,
		p->ntotpers,
		daux,
		randd2(0.5, 0.4));

	return;
}

/* Creates a random instance of the problem.  */
PCOPE *pcope_random(RandConf *rc){
	PCOPE *p;

	if(rc->seed) srand(rc->seed);
	else srand(time(NULL));

	/* Basic attributes */
	p = pcope_new(rc->nacts, rc->nyears, rc->npers, rc->nres);
	p->irr = rc->irr;

	/* Actions */
	set_random_acts(rc, p);
	/* Budgets */
	set_random_budgets(rc, p);
	/* Goals */
	set_random_goals(rc, p);

	return p;
}

void pcope_to_plain(PCOPE *p, FILE *fout){
	int i, j, k, l;
	int nacts, nyears, npers, nres, ntotpers;

	nacts = p->nacts;
	nyears = p->nyears;
	npers = p->npers;
	nres = p->nres;
	ntotpers = nyears*npers;

	/*** Basic attributes ***/
	fprintf(fout, "%d %d %d %d\n",
		p->nacts, p->nyears, p->npers, p->nres);
	fprintf(fout, "%.3f\n", p->irr);

	/* Global goal */
//	fprintf(fout, "%.3f\n", p->ggoal);
	/* Yearly goal */
	for( j = 0 ; j < nyears ; j++ )
		fprintf(fout, "%.3f ", p->ygoals[j]);
	fprintf(fout, "\n");
	/* Periodal goal */
//	for( k = 0 ; k < p->ntotpers ; k++ )
//		fprintf(fout, "%.3f ", p->pgoals[k]);
//	fprintf(fout, "\n");
	/* global budget */
	for( l = 0 ; l < nres ; l++ )
		fprintf(fout, "%.3lf ", p->gbudget[l]);
	fprintf(fout, "\n");
	/* Yearly budget */
	for( l = 0 ; l < nres ; l++ ){
		for( j = 0 ; j < nyears ; j++ )
			fprintf(fout, "%.3lf ", p->ybudgets[l][j]);
		fprintf(fout, "\n");
	}
	/* Periodal budget */
	for( l = 0 ; l < nres ; l++ ){
		for( k = 0 ; k < nyears ; k++ )
			fprintf(fout, "%.3lf ", p->pbudgets[l][k]);
		fprintf(fout, "\n");
	}

	/*** Actions ***/
	for( i = 0 ; i < nacts ; i++ ){
		/* Global market */
		fprintf(fout, "%d\n", p->gmarket[i]);
		/* Yearly market */
		for( j = 0 ; j < nyears ; j++ )
			fprintf(fout, "%d ", p->ymarket[i][j]);
		fprintf(fout, "\n");
		/* Periodal market */
		for( k = 0 ; k < ntotpers ; k++ )
			fprintf(fout, "%d ", p->pmarket[i][k]);
		fprintf(fout, "\n");
		/* Cost of action (each resource) */
		for( l = 0 ; l < nres ; l++ )
			fprintf(fout, "%.3lf ", p->cost[i][l]);
		fprintf(fout, "\n");
		/* Energy value of action */
		fprintf(fout, "%.3lf\n", p->evalue[i]);
		/* Recuperation curve */
		for( k = 0 ; k < ntotpers ; k++ )
			fprintf(fout, "%.3lf ", p->recup[i][k]);
		fprintf(fout, "\n");
	}
	
	return;
}

PCOPE *pcope_from_plain(FILE *fin){
	int i, j, k, l;
	int ret;
	int nacts, nyears, npers, nres, ntotpers;
	PCOPE *p;

	/* Basic attributes */
	ret = fscanf(fin, "%d %d %d %d", &nacts, &nyears, &npers, &nres);
	ntotpers = nyears*npers;
	p = pcope_new(nacts, nyears, npers, nres);

	/* internal rate of return */
	ret = fscanf(fin, "%lf", &(p->irr));

	/* Global goal */
	//ret = fscanf(fin, "%f", &(p->ggoal));

	/* Yearly goal */
	for( j = 0 ; j < nyears ; j++ )
		ret = fscanf(fin, "%lf", &(p->ygoals[j]));

	/* Periodal goal */
	//for( k = 0 ; k < ntotpers ; k++ )
	//	ret = fscanf(fin, "%f", &(p->pgoals[k]));

	/* Global budget */
	for( l = 0 ; l < nres ; l++ )
		ret = fscanf(fin, "%lf", &(p->gbudget[l]));

	/* Yearly budget */
	for( l = 0 ; l < nres ; l++ )
		for( j = 0 ; j < nyears ; j++ )
			ret = fscanf(fin, "%lf", &(p->ybudgets[l][j]));

	/* Periodal budget */
	for( l = 0 ; l < nres ; l++ )
		for( k = 0 ; k < ntotpers ; k++ )
			ret = fscanf(fin, "%lf", &(p->pbudgets[l][k]));
	
	/* Actions */
	/*** Actions ***/
	for( i = 0 ; i < nacts ; i++ ){
		/* Global market */
		ret = fscanf(fin, "%d", &(p->gmarket[i]));
		/* Yearly market */
		for( j = 0 ; j < nyears ; j++ )
			ret = fscanf(fin, "%d", &(p->ymarket[i][j]));
		/* Periodal market */
		for( k = 0 ; k < ntotpers ; k++ )
			ret = fscanf(fin, "%d", &(p->pmarket[i][k]));
		/* Cost of action (each resource) */
		for( l = 0 ; l < nres ; l++ )
			ret = fscanf(fin, "%lf", &(p->cost[i][l]));
		/* Energy value of action */
		ret = fscanf(fin, "%lf", &(p->evalue[i]));
		/* Recuperation curve */
		for( k = 0 ; k < ntotpers ; k++ )
			ret = fscanf(fin, "%lf", &(p->recup[i][k]));
	}
	
	return p;
}

//PCOPE *pcope_from_json(FILE *fin){
//	PCOPE *p;
//
//	return p;
//}

//void pcope_to_json(PCOPE *pcope, FILE *fout){
//	int i;
//	json_t *p_aux;
//
//	json_t *p_obj = json_object();
//	json_t *p_nyrs, *p_nacts, *p_npers, *p_nres;
//	json_t *p_irr;
//
//	json_t *p_ggoal;
//	json_t *p_ygoals, *p_pgoals;
//	json_t *p_gbudget, *p_ybudgets, *p_pbudgets;
//	json_t *p_acts;
//
//	p_nacts = json_integer(pcope->nacts);
//	p_nyrs = json_integer(pcope->nyears);
//	p_npers = json_integer(pcope->npers);
//	p_nres = json_integer(pcope->nres);
//	p_irr = json_real(pcope->irr);
//
//	//p_ggoal = json_real(pcope->ggoal);
//
//	p_ygoals = json_array();
//	for( i = 0 ; i < pcope->nyears ; i++ )
//		json_array_append(p_ygoals, json_real(pcope->ygoals[i]));
//
//	//p_pgoals = json_array();
//	//for( i = 0 ; i < pcope->ntotpers; i++ )
//	//	json_array_append(p_pgoals, json_real(pcope->pgoals[i]));
//
//	json_object_set(p_obj, "N", p_nacts);
//	json_object_set(p_obj, "Y", p_nyrs);
//	json_object_set(p_obj, "P", p_npers);
//	json_object_set(p_obj, "R", p_nres);
//	json_object_set(p_obj, "r", p_irr);
//	//json_object_set(p_obj, "", p_ggoal);
//	json_object_set(p_obj, "g", p_ygoals);
//	//json_object_set(p_obj, "g", p_pgoals);
//
//	return;
//}

void pcope_to_scip(PCOPE *p, FILE *fout){
	int i, j, k, l;
	int nacts, nyears, npers, nres, ntotpers;

	nacts = p->nacts;
	nyears = p->nyears;
	npers = p->npers;
	nres = p->nres;
	ntotpers = nyears*npers;

	fprintf(fout, "param N := %d;\n", nacts);
	fprintf(fout, "param Y := %d;\n", nyears);
	fprintf(fout, "param P := %d;\n", npers);
	fprintf(fout, "param R := %d;\n", nres);
	fprintf(fout, "param r := %f;\n", p->irr);

	fprintf(fout, "set Acs := {1 .. N};\n");
	fprintf(fout, "set Yrs := {1 .. Y};\n");
	fprintf(fout, "set Pers := {1 .. Y*P};\n");
	fprintf(fout, "set YPers[<j> in Yrs] := { (P*(j-1)+1) .. (P*j)};\n");
	fprintf(fout, "set Res := {1 .. R};\n");
	fprintf(fout, "set DPers := {1 .. 2*Y*P};\n");
	
	/* Yearly goal */
	fprintf(fout, "param g[Yrs] :=\n");
	fprint_scip_double_array(fout, p->ygoals, nyears);

	/* Global budget */
	fprintf(fout, "param o[Res] :=\n");
	fprint_scip_double_array(fout, p->gbudget, nres);

	/* Yearly budget */
	fprintf(fout, "param p[Yrs*Res] :=\n");
	fprint_scip_double_matrix(fout, p->ybudgets, nres, nyears, 1);

	/* Periodal budget */
//	fprintf(fout, "param s[Res*Pers] :=\n");
//	fprint_scip_double_matrix(fout, p->pbudgets, nres, ntotpers, 0);

	/* Global market */
	fprintf(fout, "param m[Acs] :=\n");
	fprint_scip_int_array(fout, p->gmarket, nacts);

	/* Yearly market */
	fprintf(fout, "param u[Acs*Yrs] :=\n");
	fprint_scip_int_matrix(fout, p->ymarket, nacts, nyears);

	/* Periodal market */
	fprintf(fout, "param z[Acs*Pers] :=\n");
	fprint_scip_int_matrix(fout, p->pmarket, nacts, ntotpers);

	/* Cost */
	fprintf(fout, "param c[Acs*Res] :=\n");
	fprint_scip_double_matrix(fout, p->cost, nacts, nres, 0);

	/* Energy value */
	fprintf(fout, "param v[Acs] :=\n");
	fprint_scip_double_array(fout, p->evalue, nacts);

	/* Recovery curve */
	fprintf(fout, "param e[Acs*Pers] :=\n");
	fprint_scip_double_matrix(fout, p->recup, nacts, ntotpers, 0);

	/* Depedency groups */
	fprintf(fout, "set D := {};\n");

	/* Import of model */
	fprintf(fout, "include \"knap-v3.zpl\";\n");

	return;
}

Solution *sol_new_blank(PCOPE *p){
	Solution *sol;
	int i, k, j, l;

	sol = (Solution*)malloc(sizeof(Solution));
	/* Decision variable */
	sol->x = (int**)malloc(p->nacts*sizeof(int*));
	for( i = 0 ; i < p->nacts ; i++ ){
		sol->x[i] = (int*)malloc(p->ntotpers*sizeof(int));
		for( k = 0 ; k < p->ntotpers ; k++ )
		 sol->x[i][k] = 0;
	}
	/* Basic variables */
	sol->obj = 0.0;
	sol->viable = 1;
	sol->pcope = p;

	/* Goals */
	sol->ggoal_left = p->ggoal;
	sol->ygoals_left = (double*)malloc(p->nyears*sizeof(double));
	for( j = 0 ; j < p->nyears ; j++ )
		sol->ygoals_left[j] = p->ygoals[j];
	sol->pgoals_left = (double*)malloc(p->ntotpers*sizeof(double));
	for( k = 0 ; k < p->ntotpers ; k++ )
		sol->pgoals_left[k] = p->pgoals[k];
	/* Budgets */
	sol->gbudget_left = (double*)malloc(p->nres*sizeof(double));
	sol->ybudgets_left = (double**)malloc(p->nres*sizeof(double*));
	sol->pbudgets_left = (double**)malloc(p->nres*sizeof(double*));
	for( l = 0 ; l < p->nres ; l++ ){
		sol->gbudget_left[l] = p->gbudget[l];
		sol->ybudgets_left[l] = (double*)malloc(p->nyears*sizeof(double));
		for( j = 0 ; j < p->nyears ; j++ )
			sol->ybudgets_left[l][j] = p->ybudgets[l][j];
		sol->pbudgets_left[l] = (double*)malloc(p->ntotpers*sizeof(double));
		for( k = 0 ; k < p->ntotpers ; k++ )
			sol->pbudgets_left[l][k] = p->pbudgets[l][k];
	}
	/* Markets */
	sol->gmarket_left = (int*)malloc(p->nacts*sizeof(int));
	sol->ymarket_left = (int**)malloc(p->nacts*sizeof(int*));
	sol->pmarket_left = (int**)malloc(p->nacts*sizeof(int*));
	for( i = 0 ; i < p->nacts ; i++ ){
		sol->gmarket_left[i] = p->gmarket[i];
		sol->ymarket_left[i] = (int*)malloc(p->nyears*sizeof(int));
		for( j = 0 ; j < p->nyears ; j++ )
			sol->ymarket_left[i][j] = p->ymarket[i][j];
		sol->pmarket_left[i] = (int*)malloc(p->ntotpers*sizeof(int));
		for( k = 0 ; k < p->ntotpers ; k++ )
			sol->pmarket_left[i][k] = p->pmarket[i][k];
	}

	return sol;
}

void sol_free(Solution *sol){
	int i, j, k, l;
	
	for( l = 0 ; l < sol->pcope->nres ; l++ ){
		free(sol->ybudgets_left[l]);
		free(sol->pbudgets_left[l]);
	}
	for( i = 0 ; i < sol->pcope->nacts ; i++ ){
		free(sol->x[i]);
		free(sol->ymarket_left[i]);
		free(sol->pmarket_left[i]);
	}
	free(sol->x);
	free(sol->ybudgets_left);
	free(sol->pbudgets_left);
	free(sol->ymarket_left);
	free(sol->pmarket_left);

	free(sol->ygoals_left);
	free(sol->pgoals_left);
	free(sol->gbudget_left);
	free(sol->gmarket_left);
	free(sol);
	return;
}

Solution *sol_from_plain(PCOPE *p, FILE *fin){
	int i, k, res, n;
	Solution *sol = sol_new_blank(p);

	while(!feof(fin)){
		res = fscanf(fin, "%d %d %d", &i, &k, &n);
		if( res > 2 )
			add_action(sol, i-1, k-1, n);
	}
	
	return sol;
}

void sol_to_plain(Solution *sol, FILE *fout){
	int i, k;
	PCOPE *p;

	p = sol->pcope;

	for( i = 0 ; i < p->nacts ; i++ ){
		for( k = 0 ; k < p->ntotpers ; k++ )
			fprintf(fout, "%d", sol->x[i][k]);
		fprintf(fout, "\n");
	}
	return;
}

void sol_fprint(FILE *fout, Solution *sol){
	fprintf(fout, "Objective: %f\n", sol->obj);
	fprintf(fout, "Viable: %s\n", sol->viable ? "true" : "false");
	return;
}

#define assert_positive(x) if(x<0) return 0

char recompute_viability(Solution *sol, int act, int per){
	int i, j, k, l, year;
	int nacts, nyears, npers, nres, ntotpers;

	nacts = sol->pcope->nacts;
	nyears = sol->pcope->nyears;
	npers = sol->pcope->npers;
	nres = sol->pcope->nres;
	ntotpers = nyears*npers;

	/* Goals */
	/*************************************************** olhar periodos/anos na frente tambem */
	//assert_positive(sol->ggoal_left);   (NOT USED YET)
	for( j = (per/npers) ; j < nyears ; j++ ){
		printf("ygoal_left[%d]:%lf\n", j, sol->ygoals_left[j]);
		assert_positive(sol->ygoals_left[j]);
	}
	//for( k = per ; k < ntotpers ; k++ )
	//	assert_positive(sol->pgoals_left[k]); (NOT USED YET)

	/* Budgets */
	for( l = 0 ; l < nres ; l++ ){
		printf("gbudget_left[%d]:%lf\n", l, sol->gbudget_left[l]);
		assert_positive(sol->gbudget_left[l]);
		printf("ybudgets_left[%d][%d]:%lf\n", l, per/npers, sol->ybudgets_left[l][per/npers]);
		assert_positive(sol->ybudgets_left[l][per/npers]);
		printf("pbudgets_left[%d][%d]:%lf\n", l, per, sol->pbudgets_left[l][per]);
		assert_positive(sol->pbudgets_left[l][per]);
	}
	
	/* Market */
	printf("gmarket_left:%d\n", sol->gmarket_left[act]);
	assert_positive(sol->gmarket_left[act]);
	printf("ymarket_left[%d]:%d\n", per/npers, sol->ymarket_left[act][per/npers]);
	assert_positive(sol->ymarket_left[act][per/npers]);
	printf("pmarket_left[%d]:%d\n", per, sol->pmarket_left[act][per]);
	assert_positive(sol->pmarket_left[act][per]);
	
	return 1;
}

Solution *add_action(Solution *sol, int act, int per, int n){
	int i, j, k, l, year;
	int nacts, nyears, npers, nres, ntotpers;
	double rec, overall_cost, cost, profit, daux;
	PCOPE *p;

	nacts = sol->pcope->nacts;
	nyears = sol->pcope->nyears;
	npers = sol->pcope->npers;
	nres = sol->pcope->nres;
	ntotpers = nyears*npers;

	p = sol->pcope;

	/* Updating goals/profit */
	profit = 0;
	daux = pow(1 + p->irr, per);
	for( k = per ; k < ntotpers ; k++ ){
		rec = n*p->recup[act][k-per];
		/* Global */
		sol->ggoal_left -= rec;
		/* Yearly */
		sol->ygoals_left[(k/npers)] -= rec;
		/* Periodal */
		sol->pgoals_left[k] -= rec;

		/* Accumulating profit */
		profit += rec*p->evalue[act]/daux;
		daux *= (1+p->irr);
	}
#if COMPUTE_FURTHER_RECUP >= 1
	for( k = ntotpers ; k < ntotpers+per ; k++ ){
		rec = n*p->recup[act][k-per];
		profit += rec*p->evalue[act]/daux;
		daux *= (1+p->irr);
	}
#endif

	/* Updating budgets */
	overall_cost = 0.0;
	for( l = 0 ; l < nres ; l++ ){
		cost = n*p->cost[act][l];
		/* Global */
		sol->gbudget_left[l] -= cost;
		/* Yearly */
		sol->ybudgets_left[l][per/npers] -= cost;
		/* Periodal */
		sol->pbudgets_left[l][per] -= cost;

		/* Accumulating cost */
		overall_cost += cost;
	}

	/* Updating markets */
	sol->gmarket_left[act] -= n;
	sol->ymarket_left[act][per/npers] -= n;
	sol->pmarket_left[act][per] -= n;

	/* Updating objective */
	overall_cost /= pow(1 + p->irr, per);
	sol->obj += profit - overall_cost;

	/* Updating decision variable */
	sol->x[act][per] += n;

	/* Checking viability */
	if( sol->viable )
		sol->viable = recompute_viability(sol, act, per);

	return sol;
}

