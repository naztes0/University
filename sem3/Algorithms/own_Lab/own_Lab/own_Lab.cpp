/*
*  Узагальніть метод Рабіна-Карпа пошуку зразка в текстовому рядку так,

щоб він дозволив розв’язати задачу пошуку заданого зразка розміром

m на m у символьному масиві розміром n на n. Зразок можна рухати по

горизонталі та вертикалі, але не обертати.
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

vector<long long>RabinKarp::findHash(vector<string>& matrix) const {
	vector<long long> hashes;
	int col = matrix[0].size();
	for (int i = 0; i < col; i++) {
		long rowHash = 0;
		for (int j = 0; j < patternRows; j++) {
			rowHash = ((rowHash * radix) % mod + (matrix[j][i] % mod)) % mod;
		}
		hashes.push_back(rowHash);
	}
	return hashes;
}

void RabinKarp::rollingHash(vector<long long>&textHash, long long textMatrixHash, long row) {
	textMatrixHash = (textMatrixHash % mod - (textHash[row] % mod) * (maxColumnPower) % mod + mod) % mod;
	textMatrixHash = (textMatrixHash % mod * radix) % mod;
	textMatrixHash = (textMatrixHash % mod + textHash[row + patternColumns] % mod) % mod;
}
void RabinKarp::collumnRollingHash(vector<string>& text, vector<long long>& textHash, int column) const {

}

bool RabinKarp::check(vector<string>& text, vector<string>& pattern, int row, int column) {

}

vector<pair<int, int>> RabinKarp:: rabinKarpSearch(vector<string>& text, vector<string>& pattern) {
	vector<pair<int, int>> indexes;
	
	textRows = text.size();
	textColumns = text[0].size();
	patternRows = pattern.size();
	patternColumns = pattern[0].size();

	maxColumnPower = powerUnderMod(patternColumns - 1);
	maxRowPower = powerUnderMod(patternRows - 1);

	vector<long long> textHash = findHash(text);
	vector<long long> patternHash = findHash(pattern);

	long long patternMatrixHash = 0;
	for (int i = 0; i < patternColumns; i++) {
		patternMatrixHash = (patternMatrixHash * radix + patternHash[i]) % mod;
	}
	for (int i = 0; i < (textRows - patternRows); i++) {
		long long textMatrixHash = 0;
		for (int j = 0; j < patternColumns; j++) {
			textMatrixHash = (textMatrixHash * radix + textHash[j]) % mod;
		}
		for (int j = 0; j < (textColumns - patternColumns); j++) {
			if (patternMatrixHash == textMatrixHash) {
				if (check(text, pattern, i, j)) {
					pair<int, int> upperLeftIndex;
					upperLeftIndex.first = i;
					upperLeftIndex.second = j;
					indexes.emplace_back(upperLeftIndex);
				}
			}
			rollingHash(textHash, textMatrixHash, j);
		}
		if (i < (textRows - patternRows)) collumnRollingHash(text, textHash, i);
	}
	return indexes;
}



