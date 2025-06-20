#include "AATree.h"

//////////////////////////PUBLIC///////////////////

void AATree::insert(double val) {
    insert(val, root, nullptr);
}

void AATree::remove(double val) {
    remove(val, root);
}

bool AATree::find(double val) {
    return find(val, root) != nullNode;
}

void AATree::print() {
    std::cout << "AA-Tree Structure:" << std::endl;
    print(root, 0);
}

/////////////////////PRIVATE///////////////////////


//Left rotate (skew) to correct horizontal left relations
void AATree::skew(Node*& node) {
    if (node == nullNode) return;

    if (node->left->level == node->level && node->level != 0) {
        Node* leftChild = node->left;
        node->left = leftChild->right;
        if (leftChild->right != nullNode) {
            leftChild->right->parent = node;
        }

        leftChild->right = node;
        leftChild->parent = node->parent;
        node->parent = leftChild;

        node = leftChild;
    }
}

//Doing the split to correct 2 serial right horizontal connnections
void AATree::split(Node*& node) {
    if (node == nullNode) return;

    if (node->right != nullNode && node->right->right != nullNode &&
        node->level == node->right->right->level && node->level != 0) {

        Node* rightChild = node->right;
        node->right = rightChild->left;
        if (rightChild->left != nullNode) {
            rightChild->left->parent = node;
        }

        rightChild->left = node;
        rightChild->parent = node->parent;
        node->parent = rightChild;

        node = rightChild;
        node->level++;
    }
}
//insertion method
void AATree::insert(double val, Node*& node, Node* parent) {
    if (node == nullNode) {
        node = new Node(val);
        node->left = node->right = nullNode;
        node->parent = parent;
        return;
    }

    if (val < node->val) {
        insert(val, node->left, node);
    }
    else if (val > node->val) {
        insert(val, node->right, node);
    }
    else {
        //It is duplicates
        return;
    }
    //std::cout << "INSERTIN OF ELEMENT: " << val << "\n\n";
    //print();
    //Balancing
    //std::cout << "SKEW:\n\n";
    skew(node);
    //print();

    //std::cout << "SPLIT:\n\n";
    split(node);
    //print();
}

// Deleting node from tree
void AATree::remove(double val, Node*& node) {
    if (node == nullNode) {
        return;
    }

    if (val < node->val) {
        remove(val, node->left);
    }
    else if (val > node->val) {
        remove(val, node->right);
    }
    else {
        // Found the node for deletion
        //CASE 1: Node is a leaf
        if (node->left == nullNode && node->right == nullNode) {
            
            delete node;
            node = nullNode;
            return;
        }
        //CASE 2.1: Node has only a right heir
        else if (node->left == nullNode) {
            
            Node* temp = node;
            node = node->right;
            node->parent = temp->parent;
            delete temp;
        }
        //CASE 2.2: Node has only a left heir
        else if (node->right == nullNode) {
            
            Node* temp = node;
            node = node->left;
            node->parent = temp->parent;
            delete temp;
        }
        // CASE 3: Node has both heirs
        else {
          
            // Searching for the min node in the right subtree
            Node* successor = findMin(node->right);
            node->val = successor->val;
            remove(successor->val, node->right);
        }
    }

    // Rebalancing after deletion
    rebalance(node);
}

// Searching for the mi node
AATree::Node* AATree::findMin(Node* node) {
    if (node == nullNode) {
        return nullNode;
    }

    while (node->left != nullNode) {
        node = node->left;
    }

    return node;
}

// Rebalancee after deletion
void AATree::rebalance(Node*& node) {
    if (node == nullNode) {
        return;
    }

    // Determine the required level
    int shouldBe = std::min(node->left->level, node->right->level) + 1;

    if (shouldBe < node->level) {
        node->level = shouldBe;
        if (shouldBe < node->right->level) {
            node->right->level = shouldBe;
        }
    }

    skew(node);
    skew(node->right);
    if (node->right != nullNode) {
        skew(node->right->right);
    }

    split(node);
    split(node->right);
}

// Search for node by the val
AATree::Node* AATree::find(double val, Node* node) {
    if (node == nullNode) {
        return nullNode;
    }

    if (val < node->val) {
        return find(val, node->left);
    }
    else if (val > node->val) {
        return find(val, node->right);
    }
    else {
        return node;
    }
}

// Print out the tree 
void AATree::print(Node* node, int space) {
    if (node == nullNode) {
        return;
    }

    space += 5;

    print(node->right, space);

    std::cout << std::endl;
    for (int i = 5; i < space; i++) {
        std::cout << " ";
    }
    std::cout << node->val << "(" << node->level << ")" << std::endl;

    print(node->left, space);
}

// Dectruction
void AATree::deleteTree(Node* node) {
    if (node == nullNode) {
        return;
    }

    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}