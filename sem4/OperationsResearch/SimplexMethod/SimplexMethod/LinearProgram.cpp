#include "LinearProgram.h"

LinearProgram::LinearProgram():variablesNumb(0),constraintsNumb(0){}

void LinearProgram::readInput() {
	std::cout << "Enter number of variables: ";
	std::cin >> variablesNumb;

	std::cout << "Enter number of constraints: ";
	std::cin >> constraintsNumb;

	objectiveFunc.resize(variablesNumb);
	constraintsMatrix.resize(constraintsNumb, vector<int>(variablesNumb));
	constraintsValues.resize(constraintsNumb);

	std::cout << "Enter coefficients of Objective Function:\n";
	for (int i = 0; i < variablesNumb; i++) {
		std::cin >> objectiveFunc[i];
	}
	std::cout << "Enter coefficients of Contraints:\n";
	for (int i = 0; i < constraintsNumb; i++) {
		std::cout << "Constraint " << i+1<<": ";
		for (int j = 0; j < constraintsNumb; j++) {
			std::cin >> constraintsMatrix[i][j];
		}
	}
	std::cout << "Enter constraints values: ";
	for (int i = 0; i < constraintsNumb; i++) {
		std::cin >> constraintsValues[i];
	}
}

void LinearProgram:: printData() const {
	std::cout << "\n\n============== SIMPLEX METHOD ==============\n";
	//Print Objective function
	std::cout << "Objective Function: Z=";
	for (int i = 0; i < variablesNumb; i++) {
		std::cout << objectiveFunc[i] << "x" << i+1;
		if (i != variablesNumb - 1) std::cout << " + ";
	}

	std::cout << "\n\n Constraints:\n";
	for (int i = 0; i < constraintsNumb; i++) {
		int k = 1;
		for (int j = 0; j < variablesNumb; j++) {
			std::cout << constraintsMatrix[i][j] << "x" << k++;
			if (j != variablesNumb - 1) std::cout << " + ";
		}
		std::cout << " = " << constraintsValues[i];
		std::cout << "\n";
	}
	std::cout << "============================================\n";
}

/*
3
3
3 4 5
1 2 3
4 5 6
7 8 9
6 15 24

*/