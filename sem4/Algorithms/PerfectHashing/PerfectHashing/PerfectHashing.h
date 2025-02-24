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

	/*ComplexNumber operator+(const ComplexNumber& other) {
		return ComplexNumber(real + other.real, img + other.img);
	}
	ComplexNumber operator-(const ComplexNumber& other) {
		return ComplexNumber(real - other.real, img - other.img);
	}*/

	//== operator for complex numbers
	ComplexNumber operator == (const ComplexNumber& other)const;

	//Rverting to int to be able hash the complexNumbers
	int toInt() const;
	//Print
	void printComplex() const;

};


class HashFunction {

private:
	int isPrime(int num);//check if is prime
public:
	int a, b, p, m;
	HashFunction(int m);

	int getRandomPrime(int min, int max);//get random prime in some range
	int generateHashFunction();//in couple with constructor defines variables for hashing
	int hash(ComplexNumber z);//the function itself

};
class PerfectHashing{
public:
	vector<vector<ComplexNumber>>hashTable;
	vector<HashFunction> secondaryTable;
};