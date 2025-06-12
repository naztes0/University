#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <limits>
#include <functional>
/**
 * @file Graph.h
 * @brief Weighted directed graph implementation using adjacency list
 */

 /**
  * @struct Edge
  * @brief Represents a weighted edge in the graph
  */

struct Edge {
    int to;      
    double weight; 

    Edge(int to, double weight) : to(to), weight(weight) {}
};


/**
 * @class Graph
 * @brief Weighted directed graph using adjacency list representation
 */
class Graph {
private:

    int vertices;
    std::vector<std::vector<Edge>> adjacencyList;

public:

    explicit Graph(int vertices);

    void addEdge(int from, int to, double weight);
    void removeEdge(int from, int to);

    int getVertices() const { return vertices; }
    const std::vector<Edge>& getNeighbors(int vertex) const;

    bool hasNegativeWeights() const;
    void printGraph() const;

  

};

