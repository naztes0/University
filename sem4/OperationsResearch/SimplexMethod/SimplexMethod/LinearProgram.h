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

	void isValidNumber(int& num, const std::string& message); // check the correnctnes of input data (included check for lettes and other symbols)


public:
	LinearProgram();

	void readInput();
	void printData() const;
	
	//Getters 
	int getVariablesNumb() const { return variablesNumb; }
	int getConstraintsNumb() const { return constraintsNumb; }
	vector<int> getObjectiveFunc() const { return objectiveFunc; }
	vector<vector<int>> getConstraintsMatrix() const { return constraintsMatrix; }
	vector<int> getConstraintsValues() const { return constraintsValues; }

};

