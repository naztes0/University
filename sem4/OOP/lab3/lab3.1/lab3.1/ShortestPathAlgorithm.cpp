#include "ShortestPathAlgorithm.h"
#include <limits>
#include <iostream>
#include <queue>

// THE BELLMAN-FORD ALGORITHM REALIZATION
std::vector<double> BellmanFordAlgorithm::findShortestPaths(const Graph& graph, int source) {
    int V = graph.getVertices();
    std::vector<double> distances(V, std::numeric_limits<double>::infinity());

    // Check the correctness of data
    if (source < 0 || source >= V) {
        std::cout << "Error: Invalid source vertex " << source << std::endl;
        return distances;
    }

    distances[source] = 0.0;

    //Main BF cycle 
    for (int i = 0; i < V - 1; ++i) {
        bool updated = false; 
        for (int u = 0; u < V; ++u) {
            if (distances[u] != std::numeric_limits<double>::infinity()) {
                for (const auto& edge : graph.getNeighbors(u)) {
                    if (distances[u] + edge.weight < distances[edge.to]) {
                        distances[edge.to] = distances[u] + edge.weight;
                        updated = true;
                    }
                }
            }
        }

        // If nothing has changed->break
        if (!updated) {
            break;
        }
    }

    return distances;
}

bool BellmanFordAlgorithm::hasNegativeCycle(const Graph& graph, int source) {
    int V = graph.getVertices();

    if (source < 0 || source >= V) {
        return false;
    }

    // Run BF
    std::vector<double> distances = findShortestPaths(graph, source);


    for (int u = 0; u < V; ++u) {
        if (distances[u] != std::numeric_limits<double>::infinity()) {
            for (const auto& edge : graph.getNeighbors(u)) {
                if (distances[u] + edge.weight < distances[edge.to]) {
                    return true; // Fiund negative cycle
                }
            }
        }
    }

    return false;
}

// THE DIJKSTRA ALGORITHM 

std::vector<double> DijkstraAlgorithm::findShortestPaths(const Graph& graph, int source) {
    int V = graph.getVertices();
    std::vector<double> distances(V, std::numeric_limits<double>::infinity());

    if (source < 0 || source >= V) {
        return distances;
    }

    thread_local std::vector<bool> visited;
    thread_local std::priority_queue<std::pair<double, int>,
        std::vector<std::pair<double, int>>, Compare> pq;

    visited.assign(V, false);
    while (!pq.empty()) pq.pop();  

    distances[source] = 0.0;
    pq.push({ 0.0, source });

    while (!pq.empty()) {
        double dist = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (visited[u]) continue;

        if (dist > distances[u]) continue;

        visited[u] = true;

        const auto& neighbors = graph.getNeighbors(u);
        const size_t neighborCount = neighbors.size();

        if (neighborCount <= 4) {
            for (size_t i = 0; i < neighborCount; ++i) {
                const auto& edge = neighbors[i];
                int v = edge.to;
                double weight = edge.weight;

                if (!visited[v]) {
                    double newDist = distances[u] + weight;
                    if (newDist < distances[v]) {
                        distances[v] = newDist;
                        pq.push({ newDist, v });
                    }
                }
            }
        }
        else {
            for (const auto& edge : neighbors) {
                int v = edge.to;
                double weight = edge.weight;

                if (!visited[v]) {
                    double newDist = distances[u] + weight;
                    if (newDist < distances[v]) {
                        distances[v] = newDist;
                        pq.push({ newDist, v });
                    }
                }
            }
        }
    }

    return distances;
}