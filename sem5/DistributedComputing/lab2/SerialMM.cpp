#include <iostream>
#include <stdio.h>

// Function for simple initialization of matrix elements
void DummyDataInitialization(double *pAMatrix, double *pBMatrix, int Size)
{
    for (int i = 0; i < Size; i++)
    {
        for (int j = 0; j < Size; j++)
        {
            pAMatrix[i * Size + j] = 1;
            pBMatrix[i * Size + j] = 1;
        }
    }
}
// Function for random initialization of matrix elements
void RandomDataInitialization(double *pAMatrix, double *pBMatrix,
                              int Size)
{
    int i, j; // Loop variables
    srand(unsigned(clock()));
    for (i = 0; i < Size; i++)
        for (j = 0; j < Size; j++)
        {
            pAMatrix[i * Size + j] = rand() / double(1000000);
            pBMatrix[i * Size + j] = rand() / double(1000000);
        }
}
// Function for formatted matrix output
void PrintMatrix(double *pMatrix, int RowCount, int ColCount)
{
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
            printf("%9.4f ", pMatrix[i * ColCount + j]);
        printf("\n");
    }
}
// Function for memory allocation and initialization of matrix elements
void ProcessInitialization(double *&pAMatrix, double *&pBMatrix, double *&pCMatrix, int &Size)
{
    // Matrices size
    do
    {
        printf("\nEnter size of matrices: ");
        scanf("%d", &Size);
        printf("\nChosen matrices' size = %d", Size);
        if (Size <= 0)
            printf("\nSize of objects must be greater than 0!\n");
    } while (Size <= 0);
    // Memory allocation
    pAMatrix = new double[Size * Size];
    pBMatrix = new double[Size * Size];
    pCMatrix = new double[Size * Size];
    // Initialization of matrix elements
    RandomDataInitialization(pAMatrix, pBMatrix, Size);
    for (int i = 0; i < Size * Size; i++)
    {
        pCMatrix[i] = 0;
    }
}
// Function for matrix multiplication
void SerialResultCalculation(double *pAMatrix, double *pBMatrix, double *pCMatrix, int Size)
{

    for (int i = 0; i < Size; i++)
    {
        for (int j = 0; j < Size; j++)
        {
            for (int k = 0; k < Size; k++)
            {
                pCMatrix[i * Size + j] += pAMatrix[i * Size + k] * pBMatrix[k * Size + j];
            }
        }
    }
}
// Function for computational process termination
void ProcessTermination(double *pAMatrix, double *pBMatrix, double *pCMatrix)
{
    delete[] pAMatrix;
    delete[] pBMatrix;
    delete[] pCMatrix;
}

int main()
{
    double *pAMatrix; // First argument of matrix multiplication
    double *pBMatrix; // Second argument of matrix multiplication
    double *pCMatrix; // Result matrix
    int Size;         // Size of matrices
    double start, finish, duration;

    printf("Serial matrix multiplication program\n");

    ProcessInitialization(pAMatrix, pBMatrix, pCMatrix, Size);
    // Matrix output
    printf("\nInitial A Matrix \n");
    PrintMatrix(pAMatrix, Size, Size);
    printf("\nInitial B Matrix \n");
    PrintMatrix(pBMatrix, Size, Size);

    start = clock();
    // Matrix multiplication
    SerialResultCalculation(pAMatrix, pBMatrix, pCMatrix, Size);
    // Printing the result matrix
    finish = clock();
    duration = (finish - start) / double(CLOCKS_PER_SEC);

    printf("\n Result Matrix: \n");
    PrintMatrix(pCMatrix, Size, Size);
    // Printing the time spent by matrix multiplication
    printf("\n Time of execution: %f\n", duration);
    // Computational process termination
    ProcessTermination(pAMatrix, pBMatrix, pCMatrix);
}