#include "SimplexSolver.h"

#include<iomanip>

SimplexSolver::SimplexSolver(const LinearProgram& linearProgram)
	: lp(linearProgram), tableau(linearProgram), solution{ SolutionType::ERROR,{},0.0,"Not solved yet" }{}

Solution SimplexSolver::solve() {
	std::cout << "=====Solving the linear programming problem using Simplex method=====\n";

	tableau.printTableau();
	int iteration = 1;
	while (tableau.performIteration()) {
		std::cout << "\n\nIteration" << iteration++;
		tableau.printTableau();
	}

	if (tableau.isUnbounded()) {
		solution.type = SolutionType::UNBOUNDED;
		solution.message = "The problem has an unbounded solution";
	}
	else if (tableau.isInfeasible()) {
		solution.type = SolutionType::INFEASIBLE;
		solution.message = "The problem has no feasible solution";
	}
	else {
		solution.type = SolutionType::OPTIMAL;
		solution.variables = tableau.getSolution();
		solution.objValue = tableau.getObjectiveValue();
		solution.message = "Optimal solution found";
	}

	return solution;
}

void SimplexSolver::printSolution() const {
	std::cout << "\n\n\======== SOLUTION ========\n\n";
	std::cout << solution.message << "\n\n";

	if (solution.type == SolutionType::OPTIMAL) {
		for (int i = 0; i < solution.variables.size(); i++) {
			std::cout << "x" << i + 1 << " = " << std::fixed << std::setprecision(2) << solution.variables[i];
			if (i < solution.variables.size() - 1) {
				std::cout << ", ";
			}
		}
		std::cout << ", Zmax = " << std::fixed << std::setprecision(2) << solution.objValue << "\n\n";
	}
	std::cout << "==========================\n\n";
}


/////OPTIMAL////
/*
3
3
3 4 5
1 2 3
4 5 6
7 8 9
6 15 24

*/

/*
3
3
5 4 6
2 3 1
1 2 4
3 1 2
10 15 12
*/

//////UNOBUNDED/////
/*
3
3
3 4 1
1 -2 0
0 -1 0
2 -3 1
5 6 8

*/

////INFEASIBLE///////

/*
3
3
5 4 6
2 3 1
1 2 4
3 1 2
10 15 -12

*/
