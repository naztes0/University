#include <iostream>

// Initialization of data
void dummyDataInit(double *pMatrix, double *pVector, int size)
{
    for (int i = 0; i < size; i++)
    {
        pVector[i] = 1;
        for (int j = 0; j < size; j++)
        {
            pMatrix[i * size + j] = i;
        }
    }
}

// Function for random setting of the matrix and vector elements
void randomDataInitialization(double *pMatrix, double *pVector, int Size)
{
    srand(unsigned(clock()));
    for (int i = 0; i < Size; i++)
    {
        pVector[i] = rand() / double(1000);
        for (int j = 0; j < Size; j++)
            pMatrix[i * Size + j] = rand() / double(1000);
    }
}

// Function for memory allocation and data init
void processInitialization(double *&pMatrix, double *&pVector, double *&pResult, int &size)
{
    // Size for initial matrix and vectors
    do
    {
        std::cout << "\nEnter the size of the initial objects: ";
        std::cin >> size;
        if (size <= 0)
            std::cout << "\nThe size value must be positive number";
    } while (size <= 0);
    std::cout << "\nChosen object size: " << size << std::endl;

    pMatrix = new double[size * size];
    pVector = new double[size];
    pResult = new double[size];
    randomDataInitialization(pMatrix, pVector, size);
}

// Function for formatted matrix output
void printMatrix(double *pMatrix, int RowCount, int ColCount)
{
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
            printf("%9.4f ", pMatrix[i * ColCount + j]);
        printf("\n");
    }
}
// Function for formatted vector output
void printVector(double *pVector, int Size)
{
    for (int i = 0; i < Size; i++)
        printf("%7.4f ", pVector[i]);
    printf("\n");
}

// Function for matrix-vector multiplication
void ResultCalculation(double *pMatrix, double *pVector, double *pResult, int size)
{
    for (int i = 0; i < size; i++)
    {
        pResult[i] = 0;
        for (int j = 0; j < size; j++)
            pResult[i] += pMatrix[i * size + j] * pVector[j];
    }
}

void ProcessTermination(double *pMatrix, double *pVector, double *pResult)
{
    delete[] pMatrix;
    delete[] pVector;
    delete[] pResult;
}

int main()
{
    double *pMatrix;
    double *pVector;
    double *pResult;
    int size;
    time_t start, finish;
    double duration;

    processInitialization(pMatrix, pVector, pResult, size);
    std::cout << "\nInitial matrix:\n";
    printMatrix(pMatrix, size, size);
    std::cout << "Initial vector:\n";
    printVector(pVector, size);

    start = clock();
    // Matrix-vector multiplication
    ResultCalculation(pMatrix, pVector, pResult, size);
    finish = clock();
    // Result vector
    printVector(pResult, size);

    duration = (finish - start) / double(CLOCKS_PER_SEC);

    // Printing the time spent by matrix-vector multiplication
    printf("\n Time of execution: %f \n\n", duration);
    // Computational process termination
    ProcessTermination(pMatrix, pVector, pResult);

    return 0;
}