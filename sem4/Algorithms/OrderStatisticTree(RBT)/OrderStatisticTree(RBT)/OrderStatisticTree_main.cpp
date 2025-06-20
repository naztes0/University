#include "OrderStatisticTree(RBT).h"
#include <iostream>

int main()
{
    OST tree;

    // ������ �������
    std::cout << "Insertion of elements\n";
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(15);
    tree.insert(25);
    tree.insert(5);
    tree.insert(7);
    tree.insert(1);

    std::cout << "Red-Black Tree after the insert:\n";
    tree.printTree();

    // ���������� findByOrder
    int k = 2;
    try
    {
        std::cout << "\n Search by the size of the element\nElement " << k << ": " << tree.findByOrder(k) << std::endl;
    }
    catch (const std::out_of_range &e)
    {
        std::cout << "�������: " << e.what() << std::endl;
    }

    // ���������� orderOfKey
    int key = 15;
    std::cout << "\nQuantity of numbers less than " << key << ": " << tree.orderOfKey(key) << std::endl;

    // ��������� ��������
    std::cout << "\nDeleting element with key 10\n";
    tree.remove(10);
    tree.printTree();
    std::cout << "\nDeleting element with key 10\n";
    tree.remove(20);
    tree.printTree();

    std::cout << "\nFinal Red-Black Tree :\n";
    tree.printTree();

    return 0;
}