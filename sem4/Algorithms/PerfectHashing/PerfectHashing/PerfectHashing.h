#pragma once

#include<iostream>
#include<vector>
#include<cmath>	
#include<cstdlib>

using std::vector;

class Hashable {
public:
	virtual int toInt() const = 0;
	virtual bool operator==(const Hashable& other) const = 0;
	virtual bool operator!=(const Hashable& other) const = 0;
	virtual void print() const = 0;
	virtual ~Hashable() {}
};

class ComplexNumber :public Hashable {
private:
	int real;
	int img;

public:
	ComplexNumber(int r =0, int i=0):real(r), img(i){}
	int getReal()const { return real; }
	int getImg() const {return img;}

	//== operator for complex numbers
	bool operator==(const ComplexNumber& other) const;
	bool operator!=(const ComplexNumber& other) const;

	bool operator==(const Hashable& other) const override;
	bool operator!=(const Hashable& other) const override;


	//Rverting to int to be able hash the complexNumbers
	int toInt() const override;
	//Print
	void printComplex() const;

	void print() const override { printComplex(); }
};

class ComplexVector : public Hashable {
private:
    vector<ComplexNumber> values;

public:
    ComplexVector() {}
    explicit ComplexVector(const vector<ComplexNumber>& comps) : values(comps) {}
	explicit ComplexVector(const vector<int>& ints);
	const vector<ComplexNumber>& getValues() const {
		return values;
	}

	bool operator==(const ComplexVector& other) const;

	bool operator!=(const ComplexVector& other) const;

    // Реалізація базового класу
	bool operator==(const Hashable& other) const override;
	bool operator!=(const Hashable& other) const override;

	int toInt() const override;
	void print() const override;
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

	


public:
	//Contrusctor 
	static const ComplexNumber EMPTY_CELL;
	explicit PerfectHashing(int size);

	void insert(const vector<ComplexNumber>& elements) ;
	bool search(const ComplexNumber& element) const ;
	void print()const;

	
};