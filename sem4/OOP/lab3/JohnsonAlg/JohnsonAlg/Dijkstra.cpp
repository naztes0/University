#include "Graph.h"
#include "Dijkstra.h"
#include <iostream>
#include <limits>
#include <vector>
using namespace std;

vector<double> Dijkstra::run(const Graph& graph, int source) {
    int numVertices = graph.getNumVertices();
    vector<double> distances(numVertices, numeric_limits<double>::infinity());
    vector<bool> visited(numVertices, false);

    // Use a more cache-friendly approach for small graphs
    if (numVertices <= 1000) {
        return runSimple(graph, source);
    }

    // Priority queue (min-heap) for larger graphs
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    distances[source] = 0.0;
    pq.emplace(source, 0.0);

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        int u = current.vertex;

        if (visited[u]) continue;
        visited[u] = true;

        // Relax adjacent edges
        for (const auto& edge : graph.getAdjacent(u)) {
            int v = edge.to;
            double weight = edge.weight;

            if (!visited[v] && distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
                pq.emplace(v, distances[v]);
            }
        }
    }

    return distances;
}

// Optimized version for smaller graphs - avoids priority queue overhead
vector<double> Dijkstra::runSimple(const Graph& graph, int source) {
    int numVertices = graph.getNumVertices();
    vector<double> distances(numVertices, numeric_limits<double>::infinity());
    vector<bool> visited(numVertices, false);

    distances[source] = 0.0;

    for (int count = 0; count < numVertices; ++count) {
        // Find minimum distance vertex
        int u = -1;
        double minDist = numeric_limits<double>::infinity();

        for (int v = 0; v < numVertices; ++v) {
            if (!visited[v] && distances[v] < minDist) {
                minDist = distances[v];
                u = v;
            }
        }

        if (u == -1) break; // No more reachable vertices

        visited[u] = true;

        // Relax adjacent edges
        for (const auto& edge : graph.getAdjacent(u)) {
            int v = edge.to;
            double weight = edge.weight;

            if (!visited[v] && distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
            }
        }
    }

    return distances;
}