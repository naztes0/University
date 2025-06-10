#include "Johnson.h"
#include"Graph.h"
#include"BellmanFord.h"
#include"Dijkstra.h"
#include <iostream>
#include <limits>
using namespace std;

Johnson::Result Johnson::run(const Graph& graph) {
    int numVertices = graph.getNumVertices();
    Result result(numVertices);

    cout << "Starting Johnson's Algorithm (Sequential)" << endl;

    // Step 1: Create modified graph with additional vertex
    Graph modifiedGraph = createModifiedGraph(graph);

    // Step 2: Run Bellman-Ford from the additional vertex
    cout << "Running Bellman-Ford algorithm..." << endl;
    auto bellmanResult = BellmanFord::run(modifiedGraph, numVertices);

    if (bellmanResult.hasNegativeCycle) {
        cout << "Graph contains negative cycle. Johnson's algorithm cannot proceed." << endl;
        return result;
    }

    // Step 3: Reweight the original graph
    cout << "Reweighting graph edges..." << endl;
    Graph reweightedGraph(numVertices);
    for (int u = 0; u < numVertices; ++u) {
        for (const auto& edge : graph.getAdjacent(u)) {
            double newWeight = edge.weight + bellmanResult.distances[u] - bellmanResult.distances[edge.to];
            reweightedGraph.addEdge(u, edge.to, newWeight);
        }
    }

    // Step 4: Run Dijkstra from each vertex
    cout << "Running Dijkstra from each vertex..." << endl;
    for (int source = 0; source < numVertices; ++source) {
        cout << "Processing vertex " << source << endl;
        auto dijkstraResult = Dijkstra::run(reweightedGraph, source);

        // Step 5: Restore original weights
        for (int target = 0; target < numVertices; ++target) {
            if (dijkstraResult[target] != numeric_limits<double>::infinity()) {
                result.distances[source][target] = dijkstraResult[target] +
                    bellmanResult.distances[target] - bellmanResult.distances[source];
            }
            else {
                result.distances[source][target] = numeric_limits<double>::infinity();
            }
        }
    }

    result.success = true;
    cout << "Johnson's Algorithm completed successfully!" << endl;
    return result;
}

Graph Johnson::createModifiedGraph(const Graph& original) {
    int numVertices = original.getNumVertices();
    Graph modified(numVertices + 1);

    // Copy original edges
    for (int u = 0; u < numVertices; ++u) {
        for (const auto& edge : original.getAdjacent(u)) {
            modified.addEdge(u, edge.to, edge.weight);
        }
    }

    // Add edges from new vertex to all original vertices with weight 0
    for (int v = 0; v < numVertices; ++v) {
        modified.addEdge(numVertices, v, 0.0);
    }

    return modified;
}
