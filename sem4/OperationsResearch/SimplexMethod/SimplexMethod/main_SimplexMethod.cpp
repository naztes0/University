#include"LinearProgram.h"
#include"SimplexSolver.h"

int main() {

	LinearProgram lp;
	lp.readInput();
	lp.printData();

	SimplexSolver solver(lp);
	solver.solve();
	solver.printSolution();

	return 0;
}