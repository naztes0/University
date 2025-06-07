#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <limits>
#include <functional>

struct Edge {
    int to;
    double weight;

    Edge(int to, double weight) : to(to), weight(weight) {}
};

class Graph{
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

    // Методи для паралельної обробки
    std::vector<int> getVerticesRange(int start, int end) const;
    void parallelProcessVertices(int numThreads,std::function<void(int, int)> processor) const;

};

