#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <limits>
#include <mutex>


template<typename T>
struct FibonacciNode {
    T key;
    int vertex;
    int degree;
    bool marked;
    std::shared_ptr<FibonacciNode<T>> parent;
    std::shared_ptr<FibonacciNode<T>> child;
    std::shared_ptr<FibonacciNode<T>> left;
    std::shared_ptr<FibonacciNode<T>> right;

    FibonacciNode(T key, int vertex)
        : key(key), vertex(vertex), degree(0), marked(false),
        parent(nullptr), child(nullptr), left(nullptr), right(nullptr) {}
};
template<typename T>

class FibonacciHeap {
private:
    std::shared_ptr<FibonacciNode<T>> minNode;
    int nodeCount;
    std::unordered_map<int, std::shared_ptr<FibonacciNode<T>>> nodeMap;
    mutable std::mutex heapMutex;

    void consolidate();
    void cut(std::shared_ptr<FibonacciNode<T>> node,
        std::shared_ptr<FibonacciNode<T>> parent);
    void cascadingCut(std::shared_ptr<FibonacciNode<T>> node);
    void addToRootList(std::shared_ptr<FibonacciNode<T>> node);
    void removeFromRootList(std::shared_ptr<FibonacciNode<T>> node);

public:
    FibonacciHeap();
    ~FibonacciHeap() = default;

    void insert(T key, int vertex);
    std::pair<T, int> extractMin();
    void decreaseKey(int vertex, T newKey);

    bool empty() const;
    int size() const;
    void clear();

    // Thread-safe 
    void insertSafe(T key, int vertex);
    std::pair<T, int> extractMinSafe();
    void decreaseKeySafe(int vertex, T newKey);
};


template<typename T>
FibonacciHeap<T>::FibonacciHeap() : minNode(nullptr), nodeCount(0) {}

template<typename T>
void FibonacciHeap<T>::insert(T key, int vertex) {
    auto newNode = std::make_shared<FibonacciNode<T>>(key, vertex);
    nodeMap[vertex] = newNode;

    if (minNode == nullptr) {
        minNode = newNode;
        newNode->left = newNode;
        newNode->right = newNode;
    }
    else {
        addToRootList(newNode);
        if (key < minNode->key) {
            minNode = newNode;
        }
    }
    nodeCount++;
}

template<typename T>
std::pair<T, int> FibonacciHeap<T>::extractMin() {
    if (minNode == nullptr) {
        return { std::numeric_limits<T>::max(), -1 };
    }

    auto oldMin = minNode;
    T minKey = oldMin->key;
    int minVertex = oldMin->vertex;

    if (oldMin->child != nullptr) {
        auto child = oldMin->child;
        do {
            auto next = child->right;
            addToRootList(child);
            child->parent = nullptr;
            child = next;
        } while (child != oldMin->child);
    }

    removeFromRootList(oldMin);
    nodeMap.erase(minVertex);

    if (oldMin == oldMin->right) {
        minNode = nullptr;
    }
    else {
        minNode = oldMin->right;
        consolidate();
    }

    nodeCount--;
    return { minKey, minVertex };
}

template<typename T>
void FibonacciHeap<T>::decreaseKey(int vertex, T newKey) {
    auto it = nodeMap.find(vertex);
    if (it == nodeMap.end()) return;

    auto node = it->second;
    if (newKey > node->key) return;

    node->key = newKey;
    auto parent = node->parent;

    if (parent != nullptr && node->key < parent->key) {
        cut(node, parent);
        cascadingCut(parent);
    }

    if (node->key < minNode->key) {
        minNode = node;
    }
}

template<typename T>
bool FibonacciHeap<T>::empty() const {
    return minNode == nullptr;
}

template<typename T>
int FibonacciHeap<T>::size() const {
    return nodeCount;
}

template<typename T>
void FibonacciHeap<T>::clear() {
    minNode = nullptr;
    nodeCount = 0;
    nodeMap.clear();
}

// Methods for thread realisations 
template<typename T>
void FibonacciHeap<T>::insertSafe(T key, int vertex) {
    std::lock_guard<std::mutex> lock(heapMutex);
    insert(key, vertex);
}

template<typename T>
std::pair<T, int> FibonacciHeap<T>::extractMinSafe() {
    std::lock_guard<std::mutex> lock(heapMutex);
    return extractMin();
}

template<typename T>
void FibonacciHeap<T>::decreaseKeySafe(int vertex, T newKey) {
    std::lock_guard<std::mutex> lock(heapMutex);
    decreaseKey(vertex, newKey);
}

