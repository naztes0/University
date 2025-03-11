#pragma once

#include<iostream>
#include<vector>

using std::vector;

class LinearProgram
{

private:
	int variablesNumb; //The number of variables
	int constraintsNumb; //The number of constraints
	vector<double> objectiveFunc; //"c"- the vector with objective func coeff
	vector<vector<double>>constraintsMatrix;// "A" - matrix of constraints coeff
	vector<double>constraintsValues; //"b" - the vecor of constraints values

	void isValidNumber(int& num, const std::string& message); // check the correnctnes of input data (included check for lettes and other symbols)


public:
	LinearProgram();

	void readInput();
	void printData() const;
	
	//Getters 
	int getVariablesNumb() const { return variablesNumb; }
	int getConstraintsNumb() const { return constraintsNumb; }
	vector<double> getObjectiveFunc() const { return objectiveFunc; }
	vector<vector<double>> getConstraintsMatrix() const { return constraintsMatrix; }
	vector<double> getConstraintsValues() const { return constraintsValues; }

};

