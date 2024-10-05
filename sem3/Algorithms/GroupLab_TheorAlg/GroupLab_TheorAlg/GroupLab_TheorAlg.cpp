#define DOCTEST_CONFIG_IMPLEMENT
#include "C:\Users\ASUS TUF\GitRepos\University\sem3\doctest.h"
#include<iostream>
#include <cmath>


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

TEST_CASE("testin of correctly initial of the ident matrix") {
    int n = 3;
    float** matrix = new float* [n];
    for (int i = 0; i < n; i++) {
        matrix[i] = new float[n];
    }
    initOfIdentityMatrix(n, matrix);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j)
                CHECK(matrix[i][j] == 1);
            else {
                CHECK(matrix[i][j] == 0);
            }
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


void invertMatrix(float** A, int n, float** invertedM) {
    // Create an augmented matrix [A | I]
    float** augmented = createMatrix(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            augmented[i][j] = A[i][j]; // Original matrix
            invertedM[i][j] = (i == j) ? 1.0f : 0.0f; // Identity matrix
        }
    }

    for (int i = 0; i < n; i++) {
        // Pivot row selection and normalization
        int pivotRow = i;
        for (int j = i + 1; j < n; j++) {
            if (fabs(augmented[j][i]) > fabs(augmented[pivotRow][i])) {
                pivotRow = j;
            }
        }

        if (fabs(augmented[pivotRow][i]) < 1e-9) {
            std::cerr << "Matrix can't be inverted (singular matrix)." << std::endl;
            deleteMatrix(augmented, n); // Clean up
            return;
        }

        std::swap(augmented[i], augmented[pivotRow]);
        std::swap(invertedM[i], invertedM[pivotRow]);

        float diag = augmented[i][i];
        for (int j = 0; j < n; j++) {
            augmented[i][j] /= diag;
            invertedM[i][j] /= diag;
        }

        for (int j = 0; j < n; j++) {
            if (j != i) {
                float factor = augmented[j][i];
                for (int k = 0; k < n; k++) {
                    augmented[j][k] -= factor * augmented[i][k];
                    invertedM[j][k] -= factor * invertedM[i][k];
                }
            }
        }
    }

    deleteMatrix(augmented, n); // Clean up
}

void multiplyMatrices(float** A, float** B, float** C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                C[i][j] += A[k][j] * B[i][k];
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

int main(int argc,char**argv) {
    doctest::Context context;
    int res = context.run();
    if (context.shouldExit()) {
        return res; // Exit if the tests ran and there is an exit signal
    }


    float** matrixA;
    float** matrixL;
    float** matrixU;
    float** matrixI;
    float** invertedL;
    float** invertedU;
    float** resInvertMult;

    int n;
    cout << "Enter the size n of matrix: ";
    cin >> n;

    matrixA = createMatrix(n);
    matrixL = createMatrix(n);
    matrixU = createMatrix(n);
    matrixI = createMatrix(n);
    invertedL = createMatrix(n);
    invertedU = createMatrix(n);
    resInvertMult = createMatrix(n);

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
    invertMatrix(matrixL, n, invertedL);
    invertMatrix(matrixU, n, invertedU);

    cout << "Matrix L:\n";
    printMatrix(matrixL, n);
    cout << "Matrix U:\n";
    printMatrix(matrixU, n);

    cout << "Inverted Matrix L:\n";
    printMatrix(invertedL, n);

    cout << "Inverted Matrix U:\n";
    printMatrix(invertedU, n);

    multiplyMatrices(invertedL, invertedU, resInvertMult, n);
    cout << "Result of multiplying of inverted matrix L and U\n";
    printMatrix(resInvertMult, n);


    deleteMatrix(matrixA, n);
    deleteMatrix(matrixL, n);
    deleteMatrix(matrixU, n);
    deleteMatrix(matrixI, n);
    deleteMatrix(invertedL, n);
    deleteMatrix(invertedU, n);
    deleteMatrix(resInvertMult, n);


    return 0;
}

/*
3
2 3 1
4 7 2
6 18 5

*/
