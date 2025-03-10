#include "SimplexSolver.h"

#include<iomanip>

SimplexSolver::SimplexSolver(const LinearProgram& linearProgram)
	: lp(linearProgram), tableau(linearProgram), solution{ SolutionType::ERROR,{},0.0,"Not solved yet" }{}

Solution SimplexSolver::solve() {
	std::cout << "=====Solving the linear programming problem using Simplex method=====\n";

	tableau.printTableau();
	int iteration = 1;
	while (tableau.performIteration()) {
		std::cout << "\nIteration" << iteration++ << "\n";
		tableau.printTableau();
	}

	if (tableau.isUnbounded()) {
		solution.type = SolutionType::UNBOUNDED;
		solution.message = "The prblem has an unbounded solution";
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
	std::cout << "\n======== SOLUTION ========\n";
	std::cout << solution.message << std::endl;

	if (solution.type == SolutionType::OPTIMAL) {
		for (int i = 0; i < solution.variables.size(); i++) {
			std::cout << "x" << i + 1 << " = " << std::fixed << std::setprecision(2) << solution.variables[i];
			if (i < solution.variables.size() - 1) {
				std::cout << ", ";
			}
		}
		std::cout << ", Zmax = " << std::fixed << std::setprecision(2) << solution.objValue << std::endl;
	}
	std::cout << "==========================\n";
}