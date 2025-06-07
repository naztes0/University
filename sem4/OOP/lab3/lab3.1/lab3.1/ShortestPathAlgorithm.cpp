#include "ShortestPathAlgorithm.h"

#include<limits>
#include<iostream>


//THE BELLMAN-FORD ALGORITHMS REALIS
std::vector<double>BellmanFordAlgorithm::findShortestPaths(const Graph& graph, int source) {
	int V = graph.getVertices();
	std::vector<double> distances(V, std::numeric_limits<double>::infinity());
	distances[source] = 0.0;

    for (int i = 0; i < V - 1; ++i) {
        for (int u = 0; u < V; ++u) {
            if (distances[u] != std::numeric_limits<double>::infinity()) {
                for (const auto& edge : graph.getNeighbors(u)) {
                    if (distances[u] + edge.weight < distances[edge.to]) {
                        distances[edge.to] = distances[u] + edge.weight;
                    }
                }
            }
        }
    }
    return distances;
}
bool BellmanFordAlgorithm::hasNegativeCycle(const Graph& graph, int source) {
    std::vector<double> distances = findShortestPaths(graph, source);
    int V = graph.getVertices();


    for (int u = 0; u < V; ++u) {
        if (distances[u] != std::numeric_limits<double>::infinity()) {
            for (const auto& edge : graph.getNeighbors(u)) {
                if (distances[u] + edge.weight < distances[edge.to]) {
                    return true; 
                }
            }
        }
    }

    return false;
}

//THE DEJKSTRA ALGORITHMS
std::vector<double> DijkstraAlgorithm::findShortestPaths(const Graph& graph, int source) {
    int V = graph.getVertices();
    std::vector<double> distances(V, std::numeric_limits<double>::infinity());
    std::vector<bool> visited(V, false);

    std::priority_queue<std::pair<double, int>,
        std::vector<std::pair<double, int>>,
        Compare> pq;

    distances[source] = 0.0;
    pq.push({ 0.0, source });

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;

        for (const auto& edge : graph.getNeighbors(u)) {
            int v = edge.to;
            double weight = edge.weight;

            if (!visited[v] && distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
                pq.push({ distances[v], v });
            }
        }
    }

    return distances;
}

