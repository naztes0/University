#pragma once

#include"SimplexTableau.h"
#include"LinearProgram.h"

#include<vector>
#include<string>

using std::vector;

enum class SolutionType {
	OPTIMAL, UNBOUNDED,INFEASIBLE, ERROR
};

//struct for solution print out 
struct Solution {
	SolutionType type;
	vector<double> variables;
	double objValue;
	std::string message;
	
};
class SimplexSolver{

private:
	LinearProgram lp;
	SimplexTableau tableau;
	Solution solution;

public:
	SimplexSolver(const LinearProgram& linearProgram);
	//Solving a problem of linear programming
	Solution solve();

	//Print out the result
	void printSolution() const;
};

