#include <iostream>
#include "/home/mbaroni/Downloads/soplex-2.2.0/src/soplex.h"

using namespace soplex;
using namespace std;

int main(){
	SoPlex mysoplex;

	mysoplex.setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MAXIMIZE);
	DSVector dummycol(0);

	/* variables */
	mysoplex.addColReal(LPCol(2.0, dummycol, infinity, 15.0));
	mysoplex.addColReal(LPCol(3.0, dummycol, infinity, 20.0));

	/* constraints */
	DSVector row1(2);
	row1.add(0, 1.0);
	row1.add(1, 5.0);
	mysoplex.addRowReal(LPRow(100.0, row1, infinity));

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

