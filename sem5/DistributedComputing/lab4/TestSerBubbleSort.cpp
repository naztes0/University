#include <iostream>
#include <algorithm>

// Function for simple setting the initial data
void DummyDataInitialization(double *&pData, int &DataSize)
{
    for (int i = 0; i < DataSize; i++)
        pData[i] = DataSize - i;
}
// Function for initializing the data by the random generator
void RandomDataInitialization(double *&pData, int &DataSize)
{
    srand((unsigned)time(0));
    for (int i = 0; i < DataSize; i++)
        pData[i] = double(rand()) / RAND_MAX * 1000;
}
// Function for formatted data output
void PrintData(double *pData, int DataSize)
{
    for (int i = 0; i < DataSize; i++)
        printf("%7.4f ", pData[i]);
    printf("\n");
}
// Function for allocating the memory and setting the initial values
void ProcessInitialization(double *&pData, int &DataSize)
{
    pData = new double[DataSize];
    RandomDataInitialization(pData, DataSize);
}

// Function for the serial bubble sort algorithm
void SerialBubble(double *pData, int DataSize)
{
    double Tmp;
    for (int i = 1; i < DataSize; i++)
        for (int j = 0; j < DataSize - i; j++)
            if (pData[j] > pData[j + 1])
            {
                Tmp = pData[j];
                pData[j] = pData[j + 1];
                pData[j + 1] = Tmp;
            }
}
// Function for sorting by the standard library algorithm
void SerialStdSort(double *pData, int DataSize)
{
    std::sort(pData, pData + DataSize);
}
// Function for computational process termination
void ProcessTermination(double *pData, double *testpdata)
{
    delete[] pData;
    delete[] testpdata;
}

int main()
{
    double *pData; // Data to be sorted
    int DataSize;  // Size of data to be sorted
    clock_t start, finish;
    double duration = 0.0;
    int arrSizes[] = {10, 100, 10000, 20000, 30000, 40000, 50000};
    printf("Serial bubble sort program\n");

    for (int i = 0; i < 7; i++)
    {
        DataSize = arrSizes[i];
        printf("\nChosen data size: %d\n", DataSize);
        // Process initialization
        ProcessInitialization(pData, DataSize);
        double *testpdata = new double[DataSize];
        std::copy(pData, pData + DataSize, testpdata);

        printf("\nSerial bubble sort:\n");
        // Serial bubble sort
        start = clock();
        SerialBubble(pData, DataSize);
        finish = clock();
        duration = (finish - start) / double(CLOCKS_PER_SEC);
        printf("Time of execution: %f\n", duration);

        printf("\nSTD sort:\n");
        start = clock();
        SerialStdSort(testpdata, DataSize);
        finish = clock();
        duration = (finish - start) / double(CLOCKS_PER_SEC);
        printf("Time of execution lib alg: %f\n", duration);

        // Process termination
        ProcessTermination(pData, testpdata);
    }
    return 0;
}