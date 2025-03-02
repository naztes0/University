#pragma once

#include<iostream>
#include<vector>

using std::vector;

class LinearProgram
{

private:
	int variablesNumb; //The number of variables
	int constraintsNumb; //The number of constraints
	vector<int> objectiveFunc; //"c"- the vector with objective func coeff
	vector<vector<int>>constraintsMatrix;// "A" - matrix of constraints coeff
	vector<int>constraintsValues; //"b" - the vecor of constraints values


public:
	LinearProgram();

	void readInput();
	void printData() const;
	

};

