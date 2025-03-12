#include"AATree.h"

int main() {
    //////////////////TEST 1/////////////////
    //AATree tree;

    ////Elements insertion
    //tree.insert(5);
    //tree.insert(4);
    //tree.insert(9);
    //tree.insert(16);
    //tree.insert(3);
    //tree.insert(8);
    //tree.insert(7);

    //tree.print();

    //
    //std::cout << "\n===== Found 7: " << (tree.find(7) ? "YES" : "NO(((") << std::endl;

    //std::cout << "\n=== Removing leaf node with value 5 ===\n";
    //tree.remove(5);

    //tree.print();

    //return 0;

    ////////////////TEST 2/////////////////

    AATree tree;

    std::cout << "=== Creating and populating AA-Tree ===\n";

    // Create vector with elements to insert
    vector<double> values = { 5.5, 3.14, 9.8, 2.71, 6.28, 7.77, 4.44, 1.618, 8.31,
                                 10.5, 12.34, 2.0, 3.0, 15.55, 11.11, 4.2, 8.88 };

    std::cout << "Inserting the following values: \n";
    for (size_t i = 0; i < values.size(); i++) {
        std::cout << std::fixed << std::setprecision(2) << values[i];
        if (i < values.size() - 1) std::cout << ", ";
        tree.insert(values[i]);
    }
    std::cout << "\n\n";

    // Print initial tree structure
    std::cout << "=== Initial tree structure ===\n";
    tree.print();
    std::cout << "\n";

    // Search testing - case 1: Element exists in the tree
    double searchValue1 = 7.77;
    std::cout << "=== Searching for element " << searchValue1 << " ===\n";
    std::cout << "Result: " << (tree.find(searchValue1) ? "Found" : "Not found") << "\n\n";

    // Search testing - case 2: Element doesn't exist in the tree
    double searchValue2 = 13.13;
    std::cout << "=== Searching for element " << searchValue2 << " ===\n";
    std::cout << "Result: " << (tree.find(searchValue2) ? "Found" : "Not found") << "\n\n";

    // Search testing - case 3: Edge case
    double searchValue3 = 15.55;  // Largest element
    std::cout << "=== Searching for edge element " << searchValue3 << " ===\n";
    std::cout << "Result: " << (tree.find(searchValue3) ? "Found" : "Not found") << "\n\n";

    // Remove a leaf node (no children)
    double removeLeaf = 1.618;
    std::cout << "=== Removing leaf node " << removeLeaf << " ===\n";
    tree.remove(removeLeaf);
    std::cout << "Tree structure after removal:\n";
    tree.print();
    std::cout << "Removal check: " << (tree.find(removeLeaf) ? "Element still present" : "Element successfully removed") << "\n\n";

    // Remove a node with one child
    double removeOneChild = 3.14;
    std::cout << "=== Removing node with one child " << removeOneChild << " ===\n";
    tree.remove(removeOneChild);
    std::cout << "Tree structure after removal:\n";
    tree.print();
    std::cout << "Removal check: " << (tree.find(removeOneChild) ? "Element still present" : "Element successfully removed") << "\n\n";

    // Remove a node with two children
    double removeTwoChildren = 5.5;
    std::cout << "=== Removing node with two children " << removeTwoChildren << " ===\n";
    tree.remove(removeTwoChildren);
    std::cout << "Tree structure after removal:\n";
    tree.print();
    std::cout << "Removal check: " << (tree.find(removeTwoChildren) ? "Element still present" : "Element successfully removed") << "\n\n";


    // Add new elements after removal
    std::vector<double> newValues = { 20.2, 0.5, 17.17 };
    std::cout << "=== Adding new elements after removal: ";
    for (size_t i = 0; i < newValues.size(); i++) {
        std::cout << newValues[i];
        if (i < newValues.size() - 1) std::cout << ", ";
    }
    std::cout << " ===\n";

    for (auto val : newValues) {
        tree.insert(val);
    }

    std::cout << "Final tree structure:\n";
    tree.print();

    // Check presence of all added elements
    std::cout << "\n=== Checking presence of all new elements ===\n";
    for (auto val : newValues) {
        std::cout << "Element " << val << ": " << (tree.find(val) ? "Present" : "Absent") << "\n";
    }

    return 0;
}