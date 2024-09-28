#include<iostream>
using namespace std;
const int N = 100;

int** createMatrix(int n) {
	int** matrix = new int* [n];
	for (int i = 0; i < n; i++) {
		matrix[i] = new int[n]();
	} 
	return matrix;
}

void deleteMatrix(int** matrix, int n) {
	for (int i = 0; i < n; i++) {
		delete[]matrix[i];
	}
	delete[]matrix;
}


void decomposeLU(int** A, int** L, int** U, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			U[i][j] = A[i][j];
			L[j][i] = A[j][i];
			for (int k = 0; k < i; k++) {
				U[i][j] -= L[i][k] * U[k][j];
				L[j][i] -= L[j][k] * U[k][i];
			}
			L[j][i] /= U[i][i];
		}
	}
}



void printMatrix(int** matrix, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << matrix[i][j] << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}
int main() {
	int** matrixA;
	int** matrixL;
	int** matrixU;


	int n;
	cout << "Enter the size n of matrix:";
	cin >> n;


	matrixA = createMatrix(n);
	matrixL = createMatrix(n);
	matrixU = createMatrix(n);


	cout << "Enter matrix A:\n";
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cin >> matrixA[i][j];
			matrixU[i][j] = matrixA[i][j];
		}
	}
	cout << "Matrix A:\n";
	printMatrix(matrixA, n);
	cout << "Matrix U:\n";
	printMatrix(matrixU, n);
	cout << "Matrix L:\n";
	printMatrix(matrixL, n);

	decomposeLU((int**)matrixA, matrixL, matrixU, n);
	cout << "Matrix L:\n";
	printMatrix(matrixL, n);
	cout << "Matrix U:\n";
	printMatrix(matrixU, n);

}
/*
2 3 1
4 7 2
6 18 5
*/
