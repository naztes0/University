#include "PerfectHashing.h"

int main() {
    PerfectHashing hashTable(7);

    // ��������� ������ ��������� �� Hashable ��'����
    vector<Hashable*> elements;

    // ������ ��������� �����
    elements.push_back(new ComplexNumber(3, 4));
    elements.push_back(new ComplexNumber(1, 2));
    elements.push_back(new ComplexNumber(5, -2));
    elements.push_back(new ComplexNumber(-2, 7));
    elements.push_back(new ComplexNumber(0, 0));

    // ������ ������� ����������� �����
    vector<ComplexNumber> vec1 = { ComplexNumber(1, 1), ComplexNumber(2, 2) };
    vector<ComplexNumber> vec2 = { ComplexNumber(3, 3), ComplexNumber(4, 4), ComplexNumber(5, 5) };
    vector<ComplexNumber> vec3 = { ComplexNumber(-1, -1), ComplexNumber(-2, -2) };
    vector<ComplexNumber> vec4 = {ComplexNumber(-2, -0) };

    elements.push_back(new ComplexVector(vec1));
    elements.push_back(new ComplexVector(vec2));
    elements.push_back(new ComplexVector(vec3));
    elements.push_back(new ComplexVector(vec4));

    // ���������� �������� � ���-�������
    hashTable.insert(elements);

    // ��������� �������� �������� (��ﳿ ��� ����������� � �������)
    for (auto& element : elements) {
        delete element;
    }
    elements.clear();

    // �������� ��������� ���-�������
    hashTable.print();

    // ������������ ������
    std::cout << "\nDemonstration of search:\n";

    // ���������� ��������� �����, �� � � �������
    ComplexNumber z1(3, 4);
    ComplexNumber z2(0, 0);
    std::cout << "Searching for ";
    z1.printComplex();
    std::cout << ": " << (hashTable.search(z1) ? "Found" : "Not found") << "\n\n";

    std::cout << "Searching for ";
    z2.printComplex();
    std::cout << ": " << (hashTable.search(z2) ? "Found" : "Not found") << "\n\n";

    // ���������� ������ ����������� �����, ���� � � �������
    ComplexVector v1(vec1);
    std::cout << "Searching for vector: ";
    v1.print();
    std::cout << ": " << (hashTable.search(v1) ? "Found" : "Not found") << "\n\n";
    ComplexVector v2(vec4);
    std::cout << "Searching for vector: ";
    v2.print();
    std::cout << ": " << (hashTable.search(v2) ? "Found" : "Not found") << "\n\n";

    // ���������� ��������, ���� ���� � �������
    ComplexNumber z3(6, 6);
    vector<ComplexNumber> vec5 = { ComplexNumber(9, 9), ComplexNumber(10, 10) };
    ComplexVector v3(vec5);

    std::cout << "Searching for ";
    z3.printComplex();
    std::cout << ": " << (hashTable.search(z3) ? "Found" : "Not found") << "\n\n";

    std::cout << "Searching for vector: ";
    v3.print();
    std::cout << ": " << (hashTable.search(v3) ? "Found" : "Not found") << "\n\n";

    return 0;
}