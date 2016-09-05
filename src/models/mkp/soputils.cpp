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

    std::cout << "ok soplex ";

    /* maximixation problem */
	mysoplex.setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MAXIMIZE);
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
            mysoplex.addColReal(LPCol(profit, dummycol, 1, 0)); break;
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
    std::cout << " done soplex " << endl;

    return obj;
}

int main2(){
	SoPlex mysoplex;

	mysoplex.setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MAXIMIZE);
	DSVector dummycol(0);

	/* variables */
	mysoplex.addColReal(LPCol(2.0, dummycol, 10, 1.0));
	mysoplex.addColReal(LPCol(3.0, dummycol, 20, 2.0));

	/* constraints */
	DSVector row1(2);
	row1.add(0, 1.0);
	row1.add(1, 5.0);
	//mysoplex.addRowReal(LPRow(100.0, row1, infinity));
	mysoplex.addRowReal(LPRow(-infinity, row1, 100));

	mysoplex.writeFileReal("/tmp/dump.lp", NULL, NULL, NULL);

	/* solve */
	SPxSolver::Status stat;
	DVector prim(2);
	DVector dual(1);
	stat = mysoplex.solve();

	/* get solution */
	if( stat == SPxSolver::OPTIMAL ){
		mysoplex.getPrimalReal(prim);
		mysoplex.getDualReal(dual);
		cout << "Obj is: " << mysoplex.objValueReal() << endl;
		cout << "Primal sol is: [" << prim[0] << "," << prim[1] << "]" << endl;
		cout << "Dual is: " << dual[0] << "]" << endl;
	}

	return 0;
}

#ifdef __cplusplus
}
#endif

