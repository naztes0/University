#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
int *pSerialPivotPos;  // Number of pivot rows selected at the iterations
int *pSerialPivotIter; // Iterations, at which the rows were pivots

void RandomDataInitialization(double *pMatrix, double *pVector, int Size)
{

    srand(unsigned(clock()));
    for (int i = 0; i < Size; i++)
    {
        pVector[i] = rand() / double(1000000);
        for (int j = 0; j < Size; j++)
        {
            if (j <= i)
                pMatrix[i * Size + j] = rand() / double(1000000);
            else
                pMatrix[i * Size + j] = 0;
        }
    }
}
// Function for memory allocation and data initialization
void ProcessInitialization(double *&pMatrix, double *&pVector, double *&pResult, int &Size)
{
    // Memory allocation
    pMatrix = new double[Size * Size];
    pVector = new double[Size];
    pResult = new double[Size];
    // Initialization of the matrix and the vector elements
    // DummyDataInitialization(pMatrix, pVector, Size);
    RandomDataInitialization(pMatrix, pVector, Size);
}

int FindPivotRow(double *pMatrix, int Size, int Iter)
{
    int PivotRow = -1;   // Index of the pivot row
    double MaxValue = 0; // Value of the pivot elemen
    // Choose the row, that stores the maximum element
    for (int i = 0; i < Size; i++)
    {
        if ((pSerialPivotIter[i] == -1) &&
            (fabs(pMatrix[i * Size + Iter]) > MaxValue))
        {
            PivotRow = i;
            MaxValue = fabs(pMatrix[i * Size + Iter]);
        }
    }
    return PivotRow;
}
// Function for the column elimination
void SerialColumnElimination(double *pMatrix, double *pVector, int Pivot, int Iter, int Size)
{
    double PivotValue = pMatrix[Pivot * Size + Iter];
    double PivotFactor;

    // Loop over all rows except the pivot row
    for (int i = 0; i < Size; i++)
    {
        if (pSerialPivotIter[i] == -1 && i != Pivot)
        {
            PivotFactor = pMatrix[i * Size + Iter] / PivotValue;
            for (int j = Iter; j < Size; j++)
                pMatrix[i * Size + j] -= PivotFactor * pMatrix[Pivot * Size + j];
            pVector[i] -= PivotFactor * pVector[Pivot];
        }
    }
}

// Function for the Gaussian elimination
void SerialGaussianElimination(double *pMatrix, double *pVector, int Size)
{
    int Iter;     // Number of the iteration of the Gaussian elimination
    int PivotRow; // Number of the current pivot row
    for (Iter = 0; Iter < Size; Iter++)
    {
        // Finding the pivot row
        PivotRow = FindPivotRow(pMatrix, Size, Iter);
        pSerialPivotPos[Iter] = PivotRow;
        pSerialPivotIter[PivotRow] = Iter;
        SerialColumnElimination(pMatrix, pVector, PivotRow, Iter, Size);
    }
}
// Function for the back substitution (fixed)
void SerialBackSubstitution(double *pMatrix, double *pVector, double *pResult, int Size)
{
    for (int i = Size - 1; i >= 0; i--)
    {
        int Row = pSerialPivotPos[i];
        double sum = 0.0;

        // Sum over already calculated x[j]
        for (int j = i + 1; j < Size; j++)
            sum += pMatrix[Row * Size + j] * pResult[j];

        pResult[i] = (pVector[Row] - sum) / pMatrix[Row * Size + i];
    }
}

// Function for the execution of the Gauss algorithm
void SerialResultCalculation(double *pMatrix, double *pVector, double *pResult, int Size)
{
    // Memory allocation
    pSerialPivotPos = new int[Size];
    pSerialPivotIter = new int[Size];
    for (int i = 0; i < Size; i++)
    {
        pSerialPivotIter[i] = -1;
    }
    // Gaussian elimination
    SerialGaussianElimination(pMatrix, pVector, Size);
    // Back substitution
    SerialBackSubstitution(pMatrix, pVector, pResult, Size);
    // Memory deallocation
    delete[] pSerialPivotPos;
    delete[] pSerialPivotIter;
}
// Function for computational process termination
void ProcessTermination(double *pMatrix, double *pVector, double *pResult)
{
    delete[] pMatrix;
    delete[] pVector;
    delete[] pResult;
}
int main()
{
    double *pMatrix; // Matrix of the linear system
    double *pVector; // Right parts of the linear system
    double *pResult; // Result vector
    int Size;        // Sizes of the initial matrix and the vector
    time_t start, finish;
    double duration;

    int arrSizes[] = {10, 100, 500, 1000, 1500, 2000, 2500, 3000};

    printf("Serial Gauss algorithm for solving linear systems\n");

    for (int i = 0; i < 8; i++)
    {
        printf("Gauss system size: %d", arrSizes[i]);
        // Memory allocation and definition of objects' elements
        ProcessInitialization(pMatrix, pVector, pResult, arrSizes[i]);

        // The matrix and the vector output
        // printf("\nInitial Matrix \n");
        // PrintMatrix(pMatrix, Size, Size);
        // printf("Initial Vector \n");
        // PrintVector(pVector, Size);

        // Execution of the Gauss algorithm
        start = clock();
        SerialResultCalculation(pMatrix, pVector, pResult, arrSizes[i]);
        finish = clock();
        duration = (finish - start) / double(CLOCKS_PER_SEC);

        // Printing the result vector
        // printf("\nResult Vector: \n");
        // PrintVector(pResult, Size);

        // Printing the execution time of the Gauss method
        printf("\nTime of execution: %f\n", duration);

        // Computational process termination
        ProcessTermination(pMatrix, pVector, pResult);
    }
}