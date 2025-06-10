#pragma once
#include <vector>
#include <unordered_map>
#include <limits>

struct Edge {
    int to;
    double weight;

    Edge(int to, double weight) : to(to), weight(weight) {}
};

class Graph {
private:
    int numVertices;
    std::vector<std::vector<Edge>> adjList;

public:
    Graph(int vertices);
    void addEdge(int from, int to, double weight);
    const std::vector<Edge>& getAdjacent(int vertex) const;
    int getNumVertices() const;
    void printGraph() const;
};