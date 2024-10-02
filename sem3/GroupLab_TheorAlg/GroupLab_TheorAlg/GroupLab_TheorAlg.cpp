#include<iostream>
using namespace std;
const int N = 100;

float** createMatrix(int n) {
    float** matrix = new float* [n];
    for (int i = 0; i < n; i++) {
        matrix[i] = new float[n]();
    }
    return matrix;
}

void deleteMatrix(float** matrix, int n) {
    for (int i = 0; i < n; i++) {
        delete[]matrix[i];
    }
    delete[]matrix;
}

void decomposeLU(float** A, float** L, float** U, int n) {
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

void initOfIdentityMatrix(int n, float** identMatrix) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            identMatrix[i][j] = 0.0f;
            if (i == j) identMatrix[i][i] = 1.0f;
        }
    }
}

void copyMatrix(float** matrix, float** copiedM, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            copiedM[i][j] = matrix[i][j];
        }
    }
}

void invertMatrix(float** matrix, int n, float** identM, float** invertedM) {
    // Copy the matrix to invertedM as the working matrix
    copyMatrix(matrix, invertedM, n);

    // Augment matrix with identity matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            identM[i][j] = (i == j) ? 1.0f : 0.0f; // Initialize identity matrix
        }
    }

    // Gaussian elimination process
    for (int i = 0; i < n; i++) {
        // Check if pivot is zero
        if (invertedM[i][i] == 0) {
            cout << "Matrix can't be inverted (singular matrix)." << endl;
            return;
        }

        // Normalize pivot row (divide row i by invertedM[i][i])
        float pivot = invertedM[i][i];
        for (int j = 0; j < n; j++) {
            invertedM[i][j] /= pivot;
            identM[i][j] /= pivot;
        }

        // Eliminate other rows
        for (int k = 0; k < n; k++) {
            if (k != i) {
                float factor = invertedM[k][i]; // Element to eliminate
                for (int j = 0; j < n; j++) {
                    invertedM[k][j] -= factor * invertedM[i][j]; // Eliminate k-th row
                    identM[k][j] -= factor * identM[i][j];       // Apply same operations to identity matrix
                }
            }
        }
    }

    // The inverted matrix is now stored in identM (the identity matrix has transformed)
    copyMatrix(identM, invertedM, n); // Copy the result to invertedM
}

void printMatrix(float** matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

int main() {
    float** matrixA;
    float** matrixL;
    float** matrixU;
    float** matrixI;
    float** invertedL;
    float** invertedU;

    int n;
    cout << "Enter the size n of matrix: ";
    cin >> n;

    matrixA = createMatrix(n);
    matrixL = createMatrix(n);
    matrixU = createMatrix(n);
    matrixI = createMatrix(n);
    invertedL = createMatrix(n);
    invertedU = createMatrix(n);

    cout << "Enter matrix A:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> matrixA[i][j];
            matrixU[i][j] = matrixA[i][j];
        }
    }

    cout << "\nMatrix A:\n";
    printMatrix(matrixA, n);
    cout << "Matrix U:\n";
    printMatrix(matrixU, n);
    cout << "Matrix L:\n";
    printMatrix(matrixL, n);

    decomposeLU(matrixA, matrixL, matrixU, n);
    initOfIdentityMatrix(n, matrixI);
    invertMatrix(matrixL, n, matrixI, invertedL);

    cout << "Matrix L:\n";
    printMatrix(matrixL, n);
    cout << "Matrix U:\n";
    printMatrix(matrixU, n);

    cout << "Inverted Matrix L:\n";
    printMatrix(invertedL, n);

    deleteMatrix(matrixA, n);
    deleteMatrix(matrixL, n);
    deleteMatrix(matrixU, n);
    deleteMatrix(matrixI, n);
    deleteMatrix(invertedL, n);
    deleteMatrix(invertedU, n);

    return 0;
}

/*
3
2 3 1
4 7 2
6 18 5

*/
