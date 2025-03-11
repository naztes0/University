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
	vector<vector<long double>> tableau;
	vector<int> basis;

	int findPivotColumn() const;
	int findPivotRow(int pivotColumn) const;
	bool isOptimal()const;

public:
	SimplexTableau(const LinearProgram& lp);

	//Pivot operation 
	bool performIteration();

	//Returns current values of basis vars and objFunc
	vector<double> getSolution() const;
	double getObjectiveValue() const;

	// Print current simplex table
	void printTableau() const;

	// Check if the task has an infinit solution 
	bool isUnbounded() const;
};

