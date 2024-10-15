#ifndef RABIN_KARP_H
#define RABIN_KARP_H


#include<iostream>
#include"C:\Users\ASUS TUF\GitRepos\University\doctest.h"
#include<vector>
#include<string>

using std::string;
using std::vector;
using std::iostream;

class RabinKarp {
private:

	int radix = 26;//Size of alph
	long mod = 1e9 + 7;//module
	int mRowPower = 1;
	int mColumnPover = 1;

	//Matrix size
	int tRows{};
	int tColumns{};
	int patternColumns{};
	int patternRows{};

	long powerUnderMod(int number) ;
	vector <long> findHash(vector<string>& matrix) const;
	


public:

};


#endif RABIN_KARP_H