#include"BellmanFord.h"
#include"Graph.h"
#include <iostream>
#include <limits>
using namespace std;

BellmanFord::Result BellmanFord::run(const Graph& graph, int source) {
    int numVertices = graph.getNumVertices();
    Result result(numVertices);

    // Initialize distances
    fill(result.distances.begin(), result.distances.end(), numeric_limits<double>::infinity());
    result.distances[source] = 0.0;

    // Relax edges V-1 times
    for (int i = 0; i < numVertices - 1; ++i) {
        bool relaxed = false;
        for (int u = 0; u < numVertices; ++u) {
            if (result.distances[u] != numeric_limits<double>::infinity()) {
                for (const auto& edge : graph.getAdjacent(u)) {
                    double newDist = result.distances[u] + edge.weight;
                    if (newDist < result.distances[edge.to]) {
                        result.distances[edge.to] = newDist;
                        relaxed = true;
                    }
                }
            }
        }
        if (!relaxed) break; // Early termination if no relaxation occurred
    }

    // Check for negative cycles
    for (int u = 0; u < numVertices; ++u) {
        if (result.distances[u] != numeric_limits<double>::infinity()) {
            for (const auto& edge : graph.getAdjacent(u)) {
                double newDist = result.distances[u] + edge.weight;
                if (newDist < result.distances[edge.to]) {
                    result.hasNegativeCycle = true;
                    cout << "Negative cycle detected!" << endl;
                    return result;
                }
            }
        }
    }

    return result;
}