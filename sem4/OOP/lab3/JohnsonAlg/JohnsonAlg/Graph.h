#pragma once

#include<vector>
#include<unordered_map>
#include<memory>
#include<limits>

struct Edge {
	int to;
	double weight;

	Edge(int to, double weight) : to(to), weight(weight) {}
};

class Graph{

private:
	int vertices;
	std::vector<std::vector<Edge>> adjencylist;

public:

    //basic graph methods
    explicit Graph(int vertices);

    void addEdge(int from, int to, double weight);
    void removeEdge(int from, int to);

    int getVertices() const { return vertices; }
    const std::vector<Edge>& getNeighbors(int vertex) const;

    bool hasNegativeWeights() const;
    void printGraph() const;

    


};

