#pragma once

#include<iostream>
#include<vector>
#include"LinearProgram.h"
#include <iomanip>
#include <limits>
#include <algorithm>


using std::vector;


class SimplexTableau{

private:
	int rows;
	int cols;
	vector<vector<double>> tableau;
	vector<int> basis;

	int findPivotColumn() const;
	int findPivotRow(int pivotColumn) const;
	bool isOptimal()const;

public:
	SimplexTableau(const LinearProgram& lp);


};

