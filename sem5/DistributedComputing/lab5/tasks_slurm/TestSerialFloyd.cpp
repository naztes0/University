#include <iostream>

// Function for simple setting the initial data
void DummyDataInitialization(int *pMatrix, int Size)
{
    for (int i = 0; i < Size; i++)
        for (int j = i; j < Size; j++)
        {
            if (i == j)
                pMatrix[i * Size + j] = 0;
            else if (i == 0)
                pMatrix[i * Size + j] = j;
            else
                pMatrix[i * Size + j] = -1;
            pMatrix[j * Size + i] = pMatrix[i * Size + j];
        }
}
// Function for initializing the data by the random generator
void RandomDataInitialization(int *pMatrix, int Size)
{
    int InfinitiesPercent = 50;
    srand((unsigned)time(0));
    for (int i = 0; i < Size; i++)
        for (int j = 0; j < Size; j++)
            if (i != j)
            {
                if ((rand() % 100) < InfinitiesPercent)
                    pMatrix[i * Size + j] = -1;
                else
                    pMatrix[i * Size + j] = rand() % 1000000 + 1;
            }
            else
                pMatrix[i * Size + j] = 0;
}

// Function for formatted matrix output
void PrintMatrix(int *pMatrix, int RowCount, int ColCount)
{
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
            printf("%9d", pMatrix[i * ColCount + j]);
        printf("\n");
    }
}
// Function for allocating the memory and setting the initial values
void ProcessInitialization(int *&pMatrix, int &Size)
{
    // Allocate memory for the adjacency matrix
    pMatrix = new int[Size * Size];
    // Data initalization
    RandomDataInitialization(pMatrix, Size);
}

int Min(int A, int B)
{
    int Result = (A < B) ? A : B;
    if ((A < 0) && (B >= 0))
        Result = B;
    if ((B < 0) && (A >= 0))
        Result = A;
    if ((A < 0) && (B < 0))
        Result = -1;
    return Result;
}
// Function for the serial Floyd algorithm
void SerialFloyd(int *pMatrix, int Size)
{
    int t1, t2;
    for (int k = 0; k < Size; k++)
        for (int i = 0; i < Size; i++)
            for (int j = 0; j < Size; j++)
                if ((pMatrix[i * Size + k] != -1) &&
                    (pMatrix[k * Size + j] != -1))
                {
                    t1 = pMatrix[i * Size + j];
                    t2 = pMatrix[i * Size + k] + pMatrix[k * Size + j];
                    pMatrix[i * Size + j] = Min(t1, t2);
                }
}
// Function for computational process termination
void ProcessTermination(int *pMatrix)
{
    delete[] pMatrix;
}
int main()
{
    int *pMatrix; // Adjacency matrix
    int Size;     // Size of adjacency matrix
    time_t start, finish;
    double duration = 0.0;
    int sizesArr[] = {10, 500, 600, 700, 800, 900, 1000};

    printf("Serial Floyd algorithm\n");

    for (int i = 0; i < 7; i++)
    {
        Size = sizesArr[i];
        printf("\nChosen size: %d \n", Size);
        // Process initialization
        ProcessInitialization(pMatrix, Size);
        // printf("The matrix before Floyd algorithm\n");
        // PrintMatrix(pMatrix, Size, Size);

        start = clock();
        // Serial Floyd algorithm
        SerialFloyd(pMatrix, Size);
        finish = clock();
        // printf("The matrix after Floyd algorithm\n");
        // PrintMatrix(pMatrix, Size, Size);

        duration = (finish - start) / double(CLOCKS_PER_SEC);
        printf("Time of execution: %f\n", duration);
        // Process termination
        ProcessTermination(pMatrix);
    }
    return 0;
}