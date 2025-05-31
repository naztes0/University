#include "FibonacciHeap.h"
#include <cmath>
#include <vector>

template<typename T>
void FibonacciHeap<T>::consolidate() {
    int maxDegree = static_cast<int>(std::log2(nodeCount)) + 1;
    std::vector<std::shared_ptr<FibonacciNode<T>>> degreeTable(maxDegree + 1, nullptr);

    std::vector<std::shared_ptr<FibonacciNode<T>>> rootNodes;
    if (minNode != nullptr) {
        auto current = minNode;
        do {
            rootNodes.push_back(current);
            current = current->right;
        } while (current != minNode);
    }

    for (auto node : rootNodes) {
        int degree = node->degree;

        while (degreeTable[degree] != nullptr) {
            auto other = degreeTable[degree];

            if (node->key > other->key) {
                std::swap(node, other);
            }

            removeFromRootList(other);
            other->parent = node;

            if (node->child == nullptr) {
                node->child = other;
                other->left = other;
                other->right = other;
            }
            else {
                auto child = node->child;
                other->left = child->left;
                other->right = child;
                child->left->right = other;
                child->left = other;
            }

            other->marked = false;
            node->degree++;
            degreeTable[degree] = nullptr;
            degree++;
        }

        degreeTable[degree] = node;
    }

    minNode = nullptr;
    for (auto node : degreeTable) {
        if (node != nullptr) {
            if (minNode == nullptr || node->key < minNode->key) {
                minNode = node;
            }
        }
    }
}

template<typename T>
void FibonacciHeap<T>::cut(std::shared_ptr<FibonacciNode<T>> node,
    std::shared_ptr<FibonacciNode<T>> parent) {

    if (node->right == node) {
        parent->child = nullptr;
    }
    else {
        if (parent->child == node) {
            parent->child = node->right;
        }
        node->left->right = node->right;
        node->right->left = node->left;
    }

    parent->degree--;
    addToRootList(node);
    node->parent = nullptr;
    node->marked = false;
}

template<typename T>
void FibonacciHeap<T>::cascadingCut(std::shared_ptr<FibonacciNode<T>> node) {
    auto parent = node->parent;

    if (parent != nullptr) {
        if (!node->marked) {
            node->marked = true;
        }
        else {
            cut(node, parent);
            cascadingCut(parent);
        }
    }
}

template<typename T>
void FibonacciHeap<T>::addToRootList(std::shared_ptr<FibonacciNode<T>> node) {
    if (minNode == nullptr) {
        minNode = node;
        node->left = node;
        node->right = node;
    }
    else {
        node->left = minNode->left;
        node->right = minNode;
        minNode->left->right = node;
        minNode->left = node;
    }
}

template<typename T>
void FibonacciHeap<T>::removeFromRootList(std::shared_ptr<FibonacciNode<T>> node) {
    if (node->right == node) {
        return;
    }

    node->left->right = node->right;
    node->right->left = node->left;
}

template class FibonacciHeap<double>;