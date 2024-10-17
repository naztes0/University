#include<iostream>

typedef double T;

//Допоміжні функції
/////////////////////////////////////////////////////////////
void allocateMatrix(double**& matrix, int n) {
	matrix = new double* [n];  
	for (int i = 0; i < n; i++) {
		matrix[i] = new double[n];
		for (int j = 0; j < n; j++) {
			matrix[i][j] = 0;
		}
	}
}

void freeMatrix(double**& matrix, int n) {
	for (int i = 0; i < n; i++) {
		delete[] matrix[i];  
	}
	delete[] matrix; 
	matrix = nullptr;
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
////////////////////////////////////////////////////////////////

//Функції для виконання завдання 
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

void forwardSubstitution(double** L, double*I,double*y,int n) {
	for (int i = 0; i < n; i++) {
		double sum = 0;
		for (int j = 0; j < i; j++) {
			sum += L[i][j] * y[j];
		}
		y[i] = (I[i] - sum);
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

void invertMatrix(double** A, double** L, double** U,double**inverse, int n) {
	luDecomposition(A, L, U, n);

	std::cout << "Matrix U:\n";
	printMatrix(U, n);
	std::cout << "Matrix L:\n";
	printMatrix(L, n);

	for (int i = 0; i < n; i++) {
		double* e = new double[n]();
		double* y = new double[n]();
		double* x = new double[n]();
		e[i] = 1;

		forwardSubstitution(L, e, y, n);
		backwardSubstitution(U, y, x, n);
		for (int j = 0; j < n; j++) {
			inverse[j][i] = x[j];
		}
		delete[] e;
		delete[] y;
		delete[] x;
	}
	for (int i = 0; i < n; i++) {
		delete[] L[i];
		delete[] U[i];
	}
	delete[] L;
	delete[] U;
}

//Gauss-Jordan method (Dana`s func) for checking 
bool inverseMatrix(T** A, T** invMatrix, int n)
{
	//creating the expanded matrix [A|I]
	T** augmented = new T * [n];

	for (int i = 0; i < n; ++i)
	{
		augmented[i] = new T[2 * n];

		for (int j = 0; j < n; ++j)
		{
			augmented[i][j] = A[i][j]; //copying elements from A
			augmented[i][j + n] = (i == j) ? 1.0 : 0.0;  //and forming a unit matrix
		}
	}

	for (int i = 0; i < n; ++i)
	{
		//if there is 0 on the diagonal, we look for a line to replace
		if (augmented[i][i] == 0)
		{
			bool found = false;

			for (int k = i + 1; k < n; ++k)
			{
				if (augmented[k][i] != 0)
				{
					std::swap(augmented[i], augmented[k]);
					found = true;
					break;
				}
			}

			if (!found)
			{
				std::cout << "The matrix is degenerate and has no inverse." << std::endl;
				return false;
			}
		}

		//dividing the current line into a diagonal element
		T diagElement = augmented[i][i];
		for (int j = 0; j < 2 * n; ++j)
		{
			augmented[i][j] /= diagElement;
		}

		//subtracting the current line from the others to get 0's under the diagonal
		for (int k = 0; k < n; ++k)
		{
			if (k != i)
			{
				T factor = augmented[k][i];

				for (int j = 0; j < 2 * n; ++j)
				{
					augmented[k][j] -= factor * augmented[i][j];
				}
			}
		}
	}

	//copying the right part of the extended matrix (inverse matrix)
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			invMatrix[i][j] = augmented[i][j + n];
		}
	}

	//clearing the memory
	for (int i = 0; i < n; ++i)
	{
		delete[] augmented[i];
	}

	delete[] augmented;

	return true;
}



int main() {
	int n;
	double** A;
	double** L;
	double** U;
	double** inverse;

	std::cout << "Enter the size n of matrix: ";
	std::cin >> n;

	allocateMatrix(A, n);
	allocateMatrix(L, n);
	allocateMatrix(U, n);
	allocateMatrix(inverse, n);

	std::cout << "Enter matrix A:\n";
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			std::cin >> A[i][j];
		}
	}
	std::cout << "\nMatrix A:\n";
	printMatrix(A, n);
	
	invertMatrix(A, L, U, inverse, n);
	std::cout << "Inversed matrix:\n";
	printMatrix(inverse, n);

	freeMatrix(inverse, n);
	allocateMatrix(inverse, n);
	inverseMatrix(A, inverse, n);
	std::cout << "Inversed by Gaus Jordan method:\n";
	printMatrix(inverse, n);

	return 0;
}

/*
3
2 3 1
4 7 2
6 18 5
*/