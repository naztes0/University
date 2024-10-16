#include<iostream>


const int N = 100;

void allocateMatrix(double**& matrix, int n) {
	matrix = new double* [n];  // Виділяємо пам'ять для масиву рядків
	for (int i = 0; i < n; i++) {
		matrix[i] = new double[n];
		for (int j = 0; j < n; j++) {
			matrix[i][j] = 0;
		}
	}
}

void freeMatrix(double**& matrix, int n) {
	for (int i = 0; i < n; i++) {
		delete[] matrix[i];  // Видаляємо кожен рядок
	}
	delete[] matrix;  // Видаляємо масив рядків
	matrix = nullptr;
}


void luDecomposition(double** A, double** L, double** U,int n) {
	for (int i = 0; i < n; i++) {
		//U matrix
		for (int k = i; k < n; k++) {
			double sum = 0;
			for (int j = 0; j < i; j++) {
				sum += L[i][j] * U[j][k];
			}
			U[i][k] = A[i][k] - sum;
		}
		//L matrix
		for (int k = i; k < n; k++) {
			if (i == k) L[i][i] = 1;
			else {
				double sum = 0;
				for (int j = 0; j < i; j++) {
					sum += L[k][j] * U[j][i];
				}
				L[k][i] = (A[k][i] - sum) / U[i][i];
			}
		}
	}
}

void forwardSubstitution(double** L, double*b,double*y,int n) {
	for (int i = 0; i < n; i++) {
		double sum = 0;
		for (int j = 0; j < i; j++) {
			sum += L[i][j] * y[j];
		}
		y[i] = (b[i] - sum);
	}
}
void backwardSubstitution(double**U,double*y,double*x,int n){
	for (int i = n-1; i >=0; i--) {
		double sum = 0;
		for (int j = i + 1; j < n; j++) {
			sum += U[i][j] * x[j];
		}
		x[i] = (y[i] - sum) / U[i][i];
	}

}



void printMatrix(double** matrix, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}


int main() {
	int n;
	double** A;
	double** L;
	double** U;

	std::cout << "Enter the size n of matrix: ";
	std::cin >> n;

	allocateMatrix(A, n);
	allocateMatrix(L, n);
	allocateMatrix(U, n);

	std::cout << "Enter matrix A:\n";
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			std::cin >> A[i][j];
		}
	}
	std::cout << "\nMatrix A:\n";
	printMatrix(A, n);
	std::cout << "Matrix U:\n";
	printMatrix(U, n);
	std::cout << "Matrix L:\n";
	printMatrix(L, n);

	luDecomposition(A, L, U, n);

	std::cout << "\nMatrix A:\n";
	printMatrix(A, n);
	std::cout << "Matrix U:\n";
	printMatrix(U, n);
	std::cout << "Matrix L:\n";
	printMatrix(L, n);
}

