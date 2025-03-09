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


