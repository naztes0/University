#include"PerfectHashing.h"

int main() {
    PerfectHashing hashTable(10);

    // ��������� ���� ����������� �����
    vector<ComplexNumber> numbers = {
        ComplexNumber(3, 4),
        ComplexNumber(1, 2),
        ComplexNumber(5, -2),
        ComplexNumber(-2, 7),
        ComplexNumber(0, 0),
        ComplexNumber(8, 1),
        ComplexNumber(2, 3),
        ComplexNumber(-1, -1),
        ComplexNumber(10, 5),
        ComplexNumber(7, -3)
    };

    // ���������� ����� � ���-�������
    hashTable.insert(numbers);

    // �������� ��������� ���-�������
    hashTable.print();

    // ������������ ������
    std::cout << "\nDemonstration of search:\n";

    // ���������� ��������, �� � � �������
    ComplexNumber z1(3, 4);
    ComplexNumber z2(10, 5);

    // ���������� ��������, ���� ���� � �������
    ComplexNumber z3(6, 6);
    ComplexNumber z4(-5, 2);

    std::cout << "Searching for ";
    z1.printComplex();
    std::cout << ": " << (hashTable.search(z1) ? "Found" : "Not found") << "\n";

    std::cout << "Searching for ";
    z2.printComplex();
    std::cout << ": " << (hashTable.search(z2) ? "Found" : "Not found") << "\n";

    std::cout << "Searching for ";
    z3.printComplex();
    std::cout << ": " << (hashTable.search(z3) ? "Found" : "Not found") << "\n";

    std::cout << "Searching for ";
    z4.printComplex();
    std::cout << ": " << (hashTable.search(z4) ? "Found" : "Not found") << "\n";

    return 0;
}