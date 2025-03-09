#include"SegmentTree.h"

#include "SegmentTree.h"

int main() {

    /////////////////////////////////////////TEST 1//////////////////////////////////////////////////////////////
    //SegmentTree tree;

    //// Adding intervals (correct sum values)
    //tree.insert(1, 10, 0);
    //tree.insert(1, 5, 15);
    //tree.insert(1, 3, 6);
    //tree.insert(4, 5, 9);
    //tree.insert(6, 10, 40);
    //tree.insert(6, 8, 21);
    //tree.insert(9, 10, 19);

    //std::cout << "\n=======================================";
    //std::cout << "\nBegin tree:\n";
    //tree.printTree();

    // //Query sum tests
    //std::cout << "\nQuery SUM [1, 3]: " << tree.query(1, 3) << "\n";
    //std::cout << "Query SUM [6, 10]: " << tree.query(6, 10) << "\n";
    //std::cout << "Query SUM [4, 5]: " << tree.query(4, 5) << "\n";
    //std::cout << "Query SUM [6, 8]: " << tree.query(6, 8) << "\n";
    //std::cout << "Query SUM [1, 8]: " << tree.query(1, 8) << "\n";
    //std::cout << "=======================================\n";


    // //Removing an interval
    //std::cout << "\n===========Deleting [6, 8]===========\n\n";
    //tree.remove(6,8);
    //tree.printTree();

    //// Query sum after deletion
    //std::cout << "\nQuery SUM [1, 5] after deletion: " << tree.query(1, 5) << "\n";
    //std::cout << "Query SUM [6, 10] after deletion: " << tree.query(6, 10) << "\n\n";


    //std::cout << "\n===========Deleting [6, 10]===========\n\n";
    //tree.remove(6, 10);
    //tree.printTree();

    //// Query sum after deletion
    //std::cout << "\nQuery SUM [1, 5] after deletion: " << tree.query(1, 5) << "\n";
    //std::cout << "Query SUM [6, 10] after deletion: " << tree.query(6, 10) << "\n\n";

    //std::cout << "\n===========Deleting [1, 3]===========\n\n";
    //tree.remove(1, 3);
    //tree.printTree();

    //// Query sum after deletion
    //std::cout << "\nQuery SUM [1, 5] after deletion: " << tree.query(1, 5) << "\n";
    //std::cout << "Query SUM [6, 10] after deletion: " << tree.query(6, 10) << "\n\n";

    //return 0;

    /////////////////////////////////////////////TEST 2//////////////////////////////////////////////////////////////
    
    SegmentTree tree;
    tree.insert(1.1, 20.9, 0.0);  
    tree.insert(1.1, 5.9, 15.3);
    tree.insert(1.1, 3.7, 6.7);
    tree.insert(3.7, 5.8, 9.2);
    tree.insert(6.2, 10.8, 0);
    tree.insert(6.2, 8.9, 21.8);
    tree.insert(9.5, 10.7, 19.6);

    std::cout << "\n=======================================";
    std::cout << "\nBegin tree:\n";
    tree.printTree();

    // Query sum tests
    std::cout << "\nQuery SUM [1.1, 3.7]: " << tree.query(1.1, 3.7) << "\n";
    std::cout << "Query SUM [6.2, 10.8]: " << tree.query(6.2, 10.8) << "\n";
    std::cout << "Query SUM [4.6, 5.7]: " << tree.query(4.6, 5.7) << "\n";
    std::cout << "Query SUM [6.2, 8.9]: " << tree.query(6.2, 8.9) << "\n";
    std::cout << "Query SUM [1.2, 8.9]: " << tree.query(1.2, 8.9) << "\n";
    std::cout << "=======================================\n";

    // Removing an interval
    std::cout << "\n===========Deleting [6.3, 8.9]===========\n\n";
    tree.remove(6.3, 8.9);
    tree.printTree();

    // Query sum after deletion
    std::cout << "\nQuery SUM [1.2, 5.8] after deletion: " << tree.query(1.2, 5.8) << "\n";
    std::cout << "Query SUM [6.2, 10.8] after deletion: " << tree.query(6.2, 10.8) << "\n\n";

    std::cout << "\n===========Deleting [6.2, 10.8]===========\n\n";
    tree.remove(6.2, 10.8);
    tree.printTree();

    // Query sum after deletion
    std::cout << "\nQuery SUM [1.2, 5.8] after deletion: " << tree.query(1.2, 5.8) << "\n";
    std::cout << "Query SUM [6.2, 10.8] after deletion: " << tree.query(6.2, 10.8) << "\n\n";

    std::cout << "\n===========Deleting [1.3, 3.5]===========\n\n";
    tree.remove(1.3, 3.5);
    tree.printTree();

    // Query sum after deletion
    std::cout << "\nQuery SUM [1.2, 5.8] after deletion: " << tree.query(1.2, 5.8) << "\n";
    std::cout << "Query SUM [6.2, 10.8] after deletion: " << tree.query(6.2, 10.8) << "\n\n";

}
