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


void invertMatrix(float** A, int n, float** invertedM) {
    // Create a copy of matrix A to avoid modifying the original matrix
    float** copyA = createMatrix(n);
    copyMatrix(A, copyA, n); // Copy A into copyA

    // Initialize invertedM to identity matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            invertedM[i][j] = (i == j) ? 1.0f : 0.0f; // Initialize as identity matrix
        }
    }

    // Perform Gaussian elimination on copyA and invertedM
    for (int i = 0; i < n; i++) {
        // Check for zero pivot and swap rows if needed
        if (copyA[i][i] == 0) {
            for (int k = i + 1; k < n; k++) {
                if (copyA[k][i] != 0) {
                    // Swap row i with row k in both copyA and invertedM
                    swap(copyA[i], copyA[k]);
                    swap(invertedM[i], invertedM[k]);
                    break;
                }
            }
        }

        float pivot = copyA[i][i];
        if (pivot == 0) {
            cout << "Matrix can't be inverted (singular matrix)." << endl;
            deleteMatrix(copyA, n); // Clean up the temporary matrix
            return;
        }

        // Normalize the pivot row
        for (int j = 0; j < n; j++) {
            copyA[i][j] /= pivot;
            invertedM[i][j] /= pivot;
        }

        // Eliminate other rows
        for (int k = 0; k < n; k++) {
            if (k != i) {
                float factor = copyA[k][i];
                for (int j = 0; j < n; j++) {
                    copyA[k][j] -= factor * copyA[i][j];
                    invertedM[k][j] -= factor * invertedM[i][j];
                }
            }
        }
    }

    // Clean up the copy of A
    deleteMatrix(copyA, n);
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
    invertMatrix(matrixL, n, invertedL);

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
