#include "JohnsonAlgorithm.h"
#include "AlgorithmFactory.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <atomic>

JohnsonAlgorithm::JohnsonAlgorithm() {
    bellmanFord = AlgorithmFactory::createAlgorithm(AlgorithmFactory::AlgorithmType::BELLMAN_FORD);
    dijkstra = AlgorithmFactory::createAlgorithm(AlgorithmFactory::AlgorithmType::DIJKSTRA);
}

std::vector<std::vector<double>> JohnsonAlgorithm::findAllPairsShortestPaths(const Graph& graph) {
    auto start = std::chrono::high_resolution_clock::now();

    int V = graph.getVertices();
    std::vector<std::vector<double>> result(V, std::vector<double>(V, std::numeric_limits<double>::infinity()));

    // Step 1: Creation of auxiliary graph
    Graph augmentedGraph(V + 1);

    // Copy all edges from original graph
    for (int u = 0; u < V; ++u) {
        for (const auto& edge : graph.getNeighbors(u)) {
            augmentedGraph.addEdge(u, edge.to, edge.weight);
        }
    }

    // Add edges from a new vertex to rest one with weight 0
    for (int v = 0; v < V; ++v) {
        augmentedGraph.addEdge(V, v, 0.0);
    }

    // Step 2: Running Bellman-Ford from the new vertex
    std::vector<double> h = bellmanFord->findShortestPaths(augmentedGraph, V);

    // Check graph for negative cycles
    auto* bellmanFordPtr = dynamic_cast<BellmanFordAlgorithm*>(bellmanFord.get());
    if (bellmanFordPtr && bellmanFordPtr->hasNegativeCycle(augmentedGraph, V)) {
        std::cout << "Graph contains negative cycles!" << std::endl;
        return result;
    }

    // Step 3: reweighting the graph
    Graph reweightedGraph = reweightGraph(graph, h);

    // Step 4: Running Dijkstra from each vertex sequentially
    for (int u = 0; u < V; ++u) {
        std::vector<double> distances = dijkstra->findShortestPaths(reweightedGraph, u);

        // Restore original distances
        for (int v = 0; v < V; ++v) {
            if (distances[v] != std::numeric_limits<double>::infinity()) {
                result[u][v] = distances[v] + h[v] - h[u];
            }
        }
        result[u][u] = 0.0; // The distance from the vertex to itself
    }

    auto end = std::chrono::high_resolution_clock::now();
    lastExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    return result;
}

std::vector<std::vector<double>> JohnsonAlgorithm::findAllPairsShortestPathsParallel(const Graph& graph, int numThreads) {
    auto start = std::chrono::high_resolution_clock::now();

    int V = graph.getVertices();
    
    // Use parallel processing only for larger graphs
    const int MIN_VERTICES_FOR_PARALLEL = 50;
    if (V < MIN_VERTICES_FOR_PARALLEL) {
        std::cout << "Graph too small for efficient parallelization, using sequential version..." << std::endl;
        return findAllPairsShortestPaths(graph);
    }

    // Limit thread count to reasonable values
    numThreads = std::min(numThreads, std::max(1, V / 10));
    numThreads = std::min(numThreads, static_cast<int>(std::thread::hardware_concurrency()));

    std::vector<std::vector<double>> result(V, std::vector<double>(V, std::numeric_limits<double>::infinity()));

    // Steps 1-3 same as sequential algorithm
    Graph augmentedGraph(V + 1);

    for (int u = 0; u < V; ++u) {
        for (const auto& edge : graph.getNeighbors(u)) {
            augmentedGraph.addEdge(u, edge.to, edge.weight);
        }
    }

    for (int v = 0; v < V; ++v) {
        augmentedGraph.addEdge(V, v, 0.0);
    }

    std::vector<double> h = bellmanFord->findShortestPaths(augmentedGraph, V);

    auto* bellmanFordPtr = dynamic_cast<BellmanFordAlgorithm*>(bellmanFord.get());
    if (bellmanFordPtr && bellmanFordPtr->hasNegativeCycle(augmentedGraph, V)) {
        std::cout << "Graph contains negative cycles!" << std::endl;
        return result;
    }

    Graph reweightedGraph = reweightGraph(graph, h);

    // Step 4: Optimized parallel Dijkstra execution
    std::vector<std::thread> threads;
    std::atomic<int> nextVertex(0);
    
    // Create worker threads
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, this]() {
            // Create local Dijkstra instance once per thread
            auto localDijkstra = AlgorithmFactory::createAlgorithm(AlgorithmFactory::AlgorithmType::DIJKSTRA);
            
            int vertex;
            // Dynamic work distribution - each thread takes next available work
            while ((vertex = nextVertex.fetch_add(1)) < V) {
                std::vector<double> distances = localDijkstra->findShortestPaths(reweightedGraph, vertex);
                
                // Store results (each thread works on different rows)
                for (int v = 0; v < V; ++v) {
                    if (distances[v] != std::numeric_limits<double>::infinity()) {
                        result[vertex][v] = distances[v] + h[v] - h[vertex];
                    }
                }
                result[vertex][vertex] = 0.0;
            }
        });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    lastExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    return result;
}

std::vector<std::vector<double>> JohnsonAlgorithm::findAllPairsShortestPathsParallelBatched(const Graph& graph, int numThreads) {
    auto start = std::chrono::high_resolution_clock::now();

    int V = graph.getVertices();
    std::vector<std::vector<double>> result(V, std::vector<double>(V, std::numeric_limits<double>::infinity()));

    // Steps 1-3 same as before
    Graph augmentedGraph(V + 1);

    for (int u = 0; u < V; ++u) {
        for (const auto& edge : graph.getNeighbors(u)) {
            augmentedGraph.addEdge(u, edge.to, edge.weight);
        }
    }

    for (int v = 0; v < V; ++v) {
        augmentedGraph.addEdge(V, v, 0.0);
    }

    std::vector<double> h = bellmanFord->findShortestPaths(augmentedGraph, V);

    auto* bellmanFordPtr = dynamic_cast<BellmanFordAlgorithm*>(bellmanFord.get());
    if (bellmanFordPtr && bellmanFordPtr->hasNegativeCycle(augmentedGraph, V)) {
        std::cout << "Graph contains negative cycles!" << std::endl;
        return result;
    }

    Graph reweightedGraph = reweightGraph(graph, h);

    // Step 4: Batched parallel processing
    std::vector<std::thread> threads;
    const int BATCH_SIZE = std::max(1, V / (numThreads * 4)); // Process multiple vertices per thread acquisition
    std::atomic<int> nextBatch(0);
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, this]() {
            auto localDijkstra = AlgorithmFactory::createAlgorithm(AlgorithmFactory::AlgorithmType::DIJKSTRA);
            
            int batchStart;
            while ((batchStart = nextBatch.fetch_add(BATCH_SIZE)) < V) {
                int batchEnd = std::min(batchStart + BATCH_SIZE, V);
                
                for (int vertex = batchStart; vertex < batchEnd; ++vertex) {
                    std::vector<double> distances = localDijkstra->findShortestPaths(reweightedGraph, vertex);
                    
                    for (int v = 0; v < V; ++v) {
                        if (distances[v] != std::numeric_limits<double>::infinity()) {
                            result[vertex][v] = distances[v] + h[v] - h[vertex];
                        }
                    }
                    result[vertex][vertex] = 0.0;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    lastExecutionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    return result;
}

Graph JohnsonAlgorithm::reweightGraph(const Graph& originalGraph, const std::vector<double>& h) const {
    int V = originalGraph.getVertices();
    Graph reweightedGraph(V);

    for (int u = 0; u < V; ++u) {
        for (const auto& edge : originalGraph.getNeighbors(u)) {
            double newWeight = edge.weight + h[u] - h[edge.to];
            reweightedGraph.addEdge(u, edge.to, newWeight);
        }
    }

    return reweightedGraph;
}

void JohnsonAlgorithm::processVertexRange(const Graph& reweightedGraph, const std::vector<double>& h,
    std::vector<std::vector<double>>& result, int start, int end) const {
    // Create a separate Dijkstra instance for this thread
    auto localDijkstra = AlgorithmFactory::createAlgorithm(AlgorithmFactory::AlgorithmType::DIJKSTRA);

    for (int u = start; u < end; ++u) {
        std::vector<double> distances = localDijkstra->findShortestPaths(reweightedGraph, u);

        // Restore the original distances 
        for (int v = 0; v < static_cast<int>(distances.size()); ++v) {
            if (distances[v] != std::numeric_limits<double>::infinity()) {
                result[u][v] = distances[v] + h[v] - h[u];
            }
        }
        result[u][u] = 0.0;
    }
}

void JohnsonAlgorithm::printExecutionStats() const {
    std::cout << "Algorithm: " << getName() << std::endl;
    std::cout << "Execution time: " << lastExecutionTime.count() << " us" << std::endl;
}