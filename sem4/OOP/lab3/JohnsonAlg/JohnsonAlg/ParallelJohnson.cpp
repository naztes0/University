#include "ParallelJohnson.h"
#include "Graph.h"
#include "BellmanFord.h"
#include "Dijkstra.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <future>
#include <atomic>
using namespace std;

ParallelJohnson::Result ParallelJohnson::run(const Graph& graph, int numThreads) {
    int numVertices = graph.getNumVertices();
    Result result(numVertices);

    cout << "Starting Johnson's Algorithm (Parallel with " << numThreads << " threads)" << endl;

    // Check if parallelization is beneficial
    if (numVertices < MIN_VERTICES_FOR_PARALLEL) {
        cout << "Graph too small (" << numVertices << " vertices), using sequential approach" << endl;
        // Use sequential Johnson's algorithm for small graphs
        return runSequential(graph);
    }

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

    // Step 4: Run Dijkstra from each vertex in parallel using futures
    cout << "Running Dijkstra from each vertex in parallel..." << endl;

    // Optimize thread count based on work load
    int effectiveThreads = min(numThreads, max(1, numVertices / MIN_VERTICES_PER_THREAD));
    cout << "Using " << effectiveThreads << " threads for " << numVertices << " vertices" << endl;

    vector<future<void>> futures;
    atomic<int> completedVertices(0);

    // Distribute vertices among threads more efficiently
    int verticesPerThread = numVertices / effectiveThreads;
    int remainingVertices = numVertices % effectiveThreads;

    for (int t = 0; t < effectiveThreads; ++t) {
        int startVertex = t * verticesPerThread + min(t, remainingVertices);
        int endVertex = startVertex + verticesPerThread + (t < remainingVertices ? 1 : 0);

        // Create batch of vertices for this thread
        vector<int> vertexBatch;
        for (int v = startVertex; v < endVertex; ++v) {
            vertexBatch.push_back(v);
        }

        if (!vertexBatch.empty()) {
            futures.push_back(async(launch::async,
                dijkstraWorkerBatch,
                ref(reweightedGraph),
                ref(bellmanResult.distances),
                ref(result.distances),
                vertexBatch,
                ref(completedVertices),
                numVertices));
        }
    }

    // Wait for all futures to complete
    for (auto& future : futures) {
        future.wait();
    }

    result.success = true;
    cout << "Parallel Johnson's Algorithm completed successfully!" << endl;
    return result;
}

ParallelJohnson::Result ParallelJohnson::runSequential(const Graph& graph) {
    int numVertices = graph.getNumVertices();
    Result result(numVertices);

    // Step 1: Create modified graph with additional vertex
    Graph modifiedGraph = createModifiedGraph(graph);

    // Step 2: Run Bellman-Ford from the additional vertex
    auto bellmanResult = BellmanFord::run(modifiedGraph, numVertices);

    if (bellmanResult.hasNegativeCycle) {
        return result;
    }

    // Step 3: Reweight the original graph
    Graph reweightedGraph(numVertices);
    for (int u = 0; u < numVertices; ++u) {
        for (const auto& edge : graph.getAdjacent(u)) {
            double newWeight = edge.weight + bellmanResult.distances[u] - bellmanResult.distances[edge.to];
            reweightedGraph.addEdge(u, edge.to, newWeight);
        }
    }

    // Step 4: Run Dijkstra from each vertex sequentially
    for (int source = 0; source < numVertices; ++source) {
        auto dijkstraResult = Dijkstra::run(reweightedGraph, source);

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
    return result;
}

Graph ParallelJohnson::createModifiedGraph(const Graph& original) {
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

void ParallelJohnson::dijkstraWorkerBatch(const Graph& graph,
    const vector<double>& h,
    vector<vector<double>>& distances,
    const vector<int>& vertices,
    atomic<int>& completedVertices,
    int totalVertices) {

    // Process all vertices assigned to this thread
    for (int source : vertices) {
        // Run Dijkstra for this source vertex
        auto dijkstraResult = Dijkstra::run(graph, source);

        // Restore original weights
        for (int target = 0; target < graph.getNumVertices(); ++target) {
            if (dijkstraResult[target] != numeric_limits<double>::infinity()) {
                distances[source][target] = dijkstraResult[target] + h[target] - h[source];
            }
            else {
                distances[source][target] = numeric_limits<double>::infinity();
            }
        }

        // Update progress atomically (less frequent updates)
        int completed = completedVertices.fetch_add(1) + 1;
        if (completed % max(1, totalVertices / 10) == 0 || completed == totalVertices) {
            cout << "Completed " << completed << "/" << totalVertices << " vertices" << endl;
        }
    }
}