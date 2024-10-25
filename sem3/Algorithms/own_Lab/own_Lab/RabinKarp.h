#ifndef RABIN_KARP_H
#define RABIN_KARP_H


#include<iostream>
#include<vector>
#include<string>

using std::string;
using std::vector;
using std::iostream;
using std::pair;

class RabinKarp {
private:

	const int radix = 26;//Size of alph
	long mod = 1e9 + 7;//module
	long maxRowPower = 1;
	long maxColumnPower = 1;

	//Matrix size
	int textRows{};
	int textColumns{};
	int patternColumns{};
	int patternRows{};

	long powerUnderMod(int number) ;
	vector <long long> findHash(vector<string>& matrix) const;
	bool check(vector<string>& text, vector<string>& pattern, int row, int column);
	void rollingHash(vector<long long>&textHash, long long &textMatrixHash, long row);
	void collumnRollingHash(vector<string>& text, vector<long long>& textHash, int row) const;

public:
	RabinKarp() = default;
	vector<pair<int, int>> rabinKarpSearch(vector<string>&text,vector<string>&pattern);
};


#endif RABIN_KARP_H