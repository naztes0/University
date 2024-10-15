/*
*  ���������� ����� �����-����� ������ ������ � ���������� ����� ���,

��� �� �������� ��������� ������ ������ �������� ������ �������

m �� m � ����������� ����� ������� n �� n. ������ ����� ������ ��

���������� �� ��������, ��� �� ��������.
*/


#include"RabinKarp.h"

long RabinKarp::powerUnderMod(int number) {
	if (number == 0) {
		return 1;
	}
	if (number == 1) {
		return radix % mod;
	}
	long power = powerUnderMod(number / 2);
	if (number & 1) {
		return ((radix % mod) * (power) % mod * (power) % mod) % mod;
	}
	else {
		return ((power% mod) * (power % mod)) % mod;
	}
}

vector<long>RabinKarp::findHash(vector<string>& matrix) const {
	vector<long> hashes;
	int col = matrix[0].size();
	for (int i = 0; i < col; i++) {
		long rowHash = 0;
		for (int j = 0; j < patternRows; j++) {
			rowHash = ((rowHash * radix) % mod + (matrix[j][i] % mod)) % mod;
		}
		hashes.push_back(rowHash);
	}
}






