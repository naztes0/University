#pragma once

#include<iostream>
#include<vector>
#include<cmath>	

using std::vector;

class ComplexNumber {
private:
	int real;
	int img;

public:
	ComplexNumber(int r =0, int i=0):real(r), img(i){}
	int getReal()const { return real; }
	int getImg() const {return img;}
	//== operator for complex numbers
	bool operator == (const ComplexNumber& other)const;
	bool operator != (const ComplexNumber& other)const; 
	//Rverting to int to be able hash the complexNumbers
	int toInt() const;
	//Print
	void printComplex() const;

};


class HashFunction {

private:
	int a, b, p, m;

	//check if is prime
	int isPrime(int num);

	//get random prime in some range
	int getRandomPrime(int min, int max);
public:
	//Constructor
	HashFunction() : a(0), b(0), p(0), m(0) {};
	explicit HashFunction(int tableSize);

	//in couple with constructor defines variables for hashing
	void generateHashFunction();

	//the function itself
	int hash(ComplexNumber z) const;

	//Set size of the table
	void setTableSize(int tableSize);

};
class PerfectHashing{
private:
	int primarySize;// Size of the main hash table
	HashFunction primaryHashFunc; //main hashfunc
	vector<vector<ComplexNumber>>hashTable;//table
	vector<HashFunction> secondaryHashFuncs; //secondary Table
	vector<int> secondaryTableSizes; //size of secondaty table
	vector<bool> secondaryTableInitialized;

	static const ComplexNumber EMPTY_CELL;


public:
	//Contrusctor 
	explicit PerfectHashing(int size);

	void insert(const vector<ComplexNumber>& elements) ;
	bool search(const ComplexNumber& element) const ;
	void print()const;

	
};