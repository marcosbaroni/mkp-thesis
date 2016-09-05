#include <iostream>
#include <soplex.h>

using namespace soplex;
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

double mkp_get_lp_ub(double *p, double **w, double *b, int n, int m, int *fixing){ /* LP upper bound*/
	SoPlex mysoplex;
    double profit;
    double obj;
    int i, j;

    /* maximixation problem */
	mysoplex.setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MAXIMIZE);
	mysoplex.setIntParam(SoPlex::VERBOSITY, SoPlex::VERBOSITY_DEBUG);
	DSVector dummycol(0);

	/* variables */
    for( i = 0 ; i < n ; i++ ){
        profit = p[i];
        if( fixing ){
            switch( fixing[i] ){
                case 0: mysoplex.addColReal(LPCol(profit, dummycol, 0, 0)); break;
                case 1: mysoplex.addColReal(LPCol(profit, dummycol, 1, 1)); break;
                default: mysoplex.addColReal(LPCol(profit, dummycol, 1, 0)); break;
            }
        }else{
            mysoplex.addColReal(LPCol(profit, dummycol, 1, 0));
        }
    }

	/* constraints */
    for( j = 0 ; j < m ; j++ ){
	    DSVector row(n);
        for( i = 0 ; i < n ; i++ )
            row.add(i, w[j][i]);
	    mysoplex.addRowReal(LPRow(-infinity, row, b[j]));
    }

    /* dump LP file */
	mysoplex.writeFileReal("/tmp/dump.lp", NULL, NULL, NULL);
	SPxSolver::Status stat;
	stat = mysoplex.solve();

	DVector prim(n);
	//DVector dual(m);
	if( stat == SPxSolver::OPTIMAL ){
		mysoplex.getPrimalReal(prim);
		//mysoplex.getDualReal(dual);
		obj = mysoplex.objValueReal();
	}

    return obj;
}

#ifdef __cplusplus
}
#endif

