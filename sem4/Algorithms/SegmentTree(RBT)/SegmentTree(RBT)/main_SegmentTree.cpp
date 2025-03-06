#include"SegmentTree.h"

#include "SegmentTree.h"

int main() {
    SegmentTree tree;

    // Adding intervals (correct sum values)
    tree.insert(1, 10, 0);
    tree.insert(1, 5, 15);
    tree.insert(1, 3, 6);
    tree.insert(4, 5, 9);
    tree.insert(6, 10, 40);
    tree.insert(6, 8, 21);
    tree.insert(9, 10, 19);


    std::cout << "Begin tree:\n";
    tree.printTree();

    // Query sum tests
    std::cout << "\nQuery SUM [1, 3]: " << tree.query(1, 3) << "\n";
    std::cout << "Query SUM [6, 10]: " << tree.query(6, 10) << "\n";
    std::cout << "Query SUM [4, 5]: " << tree.query(4, 5) << "\n";
    std::cout << "Query SUM [6, 8]: " << tree.query(6, 8) << "\n";
    std::cout << "Query SUM [1, 8]: " << tree.query(1, 8) << "\n";
   
    // Removing an interval
    std::cout << "\nDeleting [1, 10]\n";
    tree.remove(6,8);
    tree.printTree();

    // Query sum after deletion
    std::cout << "\nQuery SUM [1, 5] after deletion: " << tree.query(1, 5) << "\n";
    std::cout << "Query SUM [6, 10] after deletion: " << tree.query(6, 10) << "\n";

    return 0;
}
