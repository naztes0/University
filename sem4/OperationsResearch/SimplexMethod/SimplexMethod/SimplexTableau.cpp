#include "SimplexTableau.h"


SimplexTableau::SimplexTableau(const LinearProgram& lp) {
	int n = lp.getVariablesNumb();
	int m = lp.getConstraintsNumb();


	rows = m + 1; //Constraints + Obj Func
	cols = n + m + 1; //Constraints + slck vars + "b " col

	//Init simplex table
	tableau.resize(rows, vector<double>(cols, 0));
	basis.resize(m);

	for (int i = 0; i < m; i++) {
		const auto& constraints = lp.getConstraintsMatrix();
		for (int j = 0; j < n; j++) {
			tableau[i][j] = constraints[i][j];
		}
		// Fill in identity matrix for slack vars
		tableau[i][n + i] = 1;

		//fill in "b" column
		tableau[i][cols - 1] = lp.getConstraintsValues()[i];

		basis[i] = n + i;

	}
	//Fill in row for obj func with negative numbers 
	const auto& objectiveFunc = lp.getObjectiveFunc();
	for (int j = 0; j < n; j++) {
		tableau[rows-1][j] = -objectiveFunc[j];
	}
}

bool SimplexTableau::isOptimal()const {
	for (int j = 0; j < cols - 1; j++) {
		if (tableau[rows - 1][j] < 0) {
			return false;
		}
	}
	return true;
}

int SimplexTableau::findPivotColumn() const {
	//Find for a column with the most nrgative numebr and return it 
	int pivotColumn = -1;
	double minValue = 0;
	for (int i = 0; i < cols - 1; i++) {
		if (tableau[rows - 1][i] < minValue) {
			minValue = tableau[rows - 1][i];
			pivotColumn = i;
		}
	}
	return pivotColumn;
}

int SimplexTableau::findPivotRow(int pivotColumn)const {
	int pivotRow = -1;
	double minRatio = 10e13;
	for (int i = 0; i < rows - 1; i++) {
		double ratio = tableau[i][cols - 1] / tableau[i][pivotColumn];
		if (ratio < minRatio) {
			minRatio = ratio;
			pivotColumn = i;
		}
	}
	return pivotRow;
}

bool SimplexTableau::performIteration() {
	if (isOptimal()) return false;

	//Find col
	int pivotCol = findPivotColumn();
	if (pivotCol==-1) return false;

	//Find row
	int pivotRow = findPivotRow(pivotCol);
	if (pivotRow == -1)	return false;
	
	//Update basis var
	basis[pivotRow] = pivotCol;

	//Pivot element
	double pivotElement = tableau[pivotRow][pivotCol];

	//Normalizing of a pivot el row
	for (int j = 0; j < cols; j++) {
		tableau[pivotCol][j] /= pivotElement;
	}

	for (int i= 0; i < rows; i++) {
		if (i != pivotRow) {
			double factor = tableau[i][pivotCol];
			for (int j = 0; j < cols; j++) {
				tableau[i][j] -= factor * tableau[pivotRow][j];
			}
		}
	}
	return true;
}

bool SimplexTableau::isUnbounded()const {
	int pivotCol = findPivotColumn();
	if (pivotCol == -1) {
		return false;
	}
	//if all elements in da column <=0 than the task is infinite�������
	for (int i = 0; i < rows - 1; i++) {
		if (tableau[i][pivotCol] > 0) {
			return false;
		}
	}

	return true;
}


vector<double> SimplexTableau::getSolution() const {
	int n = cols - basis.size() - 1; 
	vector<double> solution(n, 0); 


	//Set values for basis vars
	for (int i = 0; i < basis.size(); i++) {
		if (basis[i] < n) { 
			solution[basis[i]] = tableau[i][cols - 1];
		}
	}

	return solution;
}

double SimplexTableau::getObjectiveValue() const {
	//Value of obj Func placed in right down corner tableau
	return -tableau[rows - 1][cols - 1]; 
}

void SimplexTableau::printTableau() const {
	std::cout << "\n=== Simplex Tableau ===\n";

	// �������� ��������� ��������
	std::cout << std::setw(6) << "Basis";
	for (int j = 0; j < cols - basis.size() - 1; j++) {
		std::cout << std::setw(10) << "x" + (j + 1);
	}
	for (int j = 0; j < basis.size(); j++) {
		std::cout << std::setw(10) << "s" + (j + 1);
	}
	std::cout << std::setw(10) << "RHS" << std::endl;

	// �������� ����� � �����������
	for (int i = 0; i < rows - 1; i++) {
		std::cout << std::setw(6) << "x" + (basis[i] + 1);
		for (int j = 0; j < cols; j++) {
			std::cout << std::setw(10) << std::fixed << std::setprecision(2) << tableau[i][j];
		}
		std::cout << std::endl;
	}

	// �������� ����� ������� �������
	std::cout << std::setw(6) << "Z";
	for (int j = 0; j < cols; j++) {
		std::cout << std::setw(10) << std::fixed << std::setprecision(2) << tableau[rows - 1][j];
	}
	std::cout << std::endl;
}