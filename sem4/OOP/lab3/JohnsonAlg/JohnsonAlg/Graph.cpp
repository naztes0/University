#include "Graph.h"
#include <iostream>
using namespace std;

Graph::Graph(int vertices) : numVertices(vertices) {
    adjList.resize(vertices);
}

void Graph::addEdge(int from, int to, double weight) {
    if (from >= 0 && from < numVertices && to >= 0 && to < numVertices) {
        adjList[from].emplace_back(to, weight);
    }
}

const vector<Edge>& Graph::getAdjacent(int vertex) const {
    return adjList[vertex];
}

int Graph::getNumVertices() const {
    return numVertices;
}

void Graph::printGraph() const {
    cout << "Graph structure:" << endl;
    for (int i = 0; i < numVertices; ++i) {
        cout << "Vertex " << i << ": ";
        for (const auto& edge : adjList[i]) {
            cout << "(" << edge.to << ", " << edge.weight << ") ";
        }
        cout << endl;
    }
}
