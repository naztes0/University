#include "LinearProgram.h"

LinearProgram::LinearProgram():variablesNumb(0.0),constraintsNumb(0.0){}



void LinearProgram::isValidNumber(int& num, const std::string& message) {
	while (true) {
		std::cout << message;
		std::cin >> num;

		if (std::cin.fail() || num <= 0) {
			std::cout << "Error! Enter positive number \n\n";
			std::cin.clear(); 
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
		}
		else {
			break; 
		}
	}
}



void LinearProgram::readInput() {

	isValidNumber(variablesNumb, "Enter number of variables: ");
	isValidNumber(constraintsNumb, "Enter number of constraints: ");


	objectiveFunc.resize(variablesNumb);
	constraintsMatrix.resize(constraintsNumb, vector<double>(variablesNumb));
	constraintsValues.resize(constraintsNumb);

	std::cout << "Enter coefficients of Objective Function:\n";
	for (int i = 0; i < variablesNumb; i++) {
		while (!(std::cin >> objectiveFunc[i])) {
			std::cout << "Error! Enter a number \n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}
	std::cout << "Enter coefficients of Contraints:\n";
	for (int i = 0; i < constraintsNumb; i++) {
		std::cout << "Constraint " << i+1<<": ";
		for (int j = 0; j < variablesNumb; j++) {
			while (!(std::cin >> constraintsMatrix[i][j])) {
				std::cout << "Error! Enter a number \n";
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		}
	}
	std::cout << "Enter constraints values: ";
	for (int i = 0; i < constraintsNumb; i++) {
		while (!(std::cin >> constraintsValues[i])) {
			std::cout << "Error! Enter a number \n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}

	
}

void LinearProgram:: printData() const {
	std::cout << "\n\n============== SIMPLEX METHOD ==============\n";
	//Print Objective function
	std::cout << "Objective Function: Z = ";
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

/*
3
3
5 4 6
2 3 1
1 2 4
3 1 2
10 15 12
*/