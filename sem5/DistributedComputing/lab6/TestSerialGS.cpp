#include <iostream>
#include <cmath>

// Function for simple setting the grid node values
void DummyDataInitialization(double *pMatrix, int Size)
{
    int i, j; // Loop variables
    // Setting the grid node values
    for (i = 0; i < Size; i++)
    {
        for (j = 0; j < Size; j++)
            if ((i == 0) || (i == Size - 1) || (j == 0) || (j == Size - 1))
                pMatrix[i * Size + j] = 100;
            else
                pMatrix[i * Size + j] = 0;
    }
}
// Function for setting the grid node values by a random generator
void RandowmDataInitialization(double *pMatrix, int Size)
{
    int i, j; // Loop variables
    srand(unsigned(clock()));
    // Setting the grid node values
    for (i = 0; i < Size; i++)
    {
        for (j = 0; j < Size; j++)
            if ((i == 0) || (i == Size - 1) || (j == 0) || (j == Size - 1))
                pMatrix[i * Size + j] = 100;
            else
                pMatrix[i * Size + j] = rand() / double(1000000);
    }
}
// Function for memory allocation and initialization of grid nodes
void ProcessInitialization(double *&pMatrix, int &Size, double &Eps)
{
    // Memory allocation
    pMatrix = new double[Size * Size];
    // Setting the grid node values
    RandowmDataInitialization(pMatrix, Size);
    printf("\n");
}
// Function for formatted matrix output
void PrintMatrix(double *pMatrix, int RowCount, int ColCount)
{
    int i, j; // Loop variables
    for (i = 0; i < RowCount; i++)
    {
        for (j = 0; j < ColCount; j++)
            printf("%7.4f ", pMatrix[i * ColCount + j]);
        printf("\n");
    }
}

// Function for the Gauss-Seidel algoritm
void ResultCalculation(double *pMatrix, int Size, double &Eps,
                       int &Iterations)
{
    int i, j; // Loop variables
    double dm, dmax, temp;
    Iterations = 0;
    do
    {
        dmax = 0;
        for (i = 1; i < Size - 1; i++)
            for (j = 1; j < Size - 1; j++)
            {
                temp = pMatrix[Size * i + j];
                pMatrix[Size * i + j] = 0.25 * (pMatrix[Size * i + j + 1] +
                                                pMatrix[Size * i + j - 1] +
                                                pMatrix[Size * (i + 1) + j] +
                                                pMatrix[Size * (i - 1) + j]);
                dm = fabs(pMatrix[Size * i + j] - temp);
                if (dmax < dm)
                    dmax = dm;
            }
        Iterations++;
    } while (dmax > Eps);
}
// Function for computational process termination
void ProcessTermination(double *pMatrix)
{
    delete[] pMatrix;
}

int main()
{
    double *pMatrix; // Matrix of the grid nodes
    int Size;        // Matrix size
    double Eps;      // Required accuracy
    int Iterations;  // Iteration number
    time_t start, finish;
    double duration;
    int matrixSizes[] = {10, 100, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};

    printf("Serial Gauss - Seidel algorithm\n");
    for (int i = 0; i < 12; i++)
    {
        Size = matrixSizes[i];
        Eps = 1;
        printf("\nChosen size: %d with eps %f", Size, Eps);
        // Process initialization
        ProcessInitialization(pMatrix, Size, Eps);
        // Gauss-Seidel method
        start = clock();
        ResultCalculation(pMatrix, Size, Eps, Iterations);
        finish = clock();
        duration = (finish - start) / double(CLOCKS_PER_SEC);

        // Printing the time spent by the Gauss-Seidel method
        printf("Execution time: %f\n", duration);
        printf("Iterations: %d\n", Iterations);

        // Computational process termination
        ProcessTermination(pMatrix);
    }
}