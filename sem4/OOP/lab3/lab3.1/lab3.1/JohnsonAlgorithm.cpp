#include "JohnsonAlgorithm.h"
#include "AlgorithmFactory.h"
#include <limits>
#include <iostream>
#include <mutex>
#include <future>

JohnsonAlgorithm::JohnsonAlgorithm() 
    :bellmanFord(AlgorithmFactory::createAlgorithm(AlgorithmFactory::AlgorithmType::BELLMAN_FORD)),
    dijkstra( AlgorithmFactory::createAlgorithm(AlgorithmFactory::AlgorithmType::DIJKSTRA)){}


Graph JohnsonAlgorithm::createAugmentedGraph(const Graph& original) {
    int originalVertices = original.getVertices();
    Graph augmented(originalVertices + 1);

    // Copying edges from orig graph
    for (int u = 0; u < originalVertices; ++u) {
        for (const auto& edge : original.getNeighbors(u)) {
            augmented.addEdge(u, edge.to, edge.weight);
        }
    }
    //Adding new vertex s with edges to all of the vetices with 0 weight
    int s = originalVertices;
    for (int v = 0; v < originalVertices; ++v) {
        augmented.addEdge(s, v, 0.0);
    }

    return augmented;
}

Graph JohnsonAlgorithm::createReweightedGraph(const Graph& graph, const std::vector<double>& h) {
    int vertices = graph.getVertices();
    Graph reweighted(vertices);

    for (int u = 0; u < vertices; ++u) {
        for (const auto& edge : graph.getNeighbors(u)) {
            // New weight: w'(u,v) = w(u,v) + h(u) - h(v)
            double newWeight = edge.weight + h[u] - h[edge.to];
            reweighted.addEdge(u, edge.to, newWeight);
        }
    }

    return reweighted;
}

void JohnsonAlgorithm::restoreOriginalWeights(std::vector<std::vector<double>>& distances,
    const std::vector<double>& h) {
    int vertices = distances.size();

    for (int u = 0; u < vertices; ++u) {
        for (int v = 0; v < vertices; ++v) {
            if (distances[u][v] != std::numeric_limits<double>::infinity()) {
                //Restoring the original weight : d(u,v) = d'(u,v) - h(u) + h(v)
                distances[u][v] = distances[u][v] - h[u] + h[v];
            }
        }
    }
}

std::vector<std::vector<double>> JohnsonAlgorithm::findAllPairsShortestPaths(const Graph& graph) {
    int vertices = graph.getVertices();
    std::vector<std::vector<double>> distances(vertices,
        std::vector<double>(vertices, std::numeric_limits<double>::infinity()));

    // Init diagonal with 0
    for (int i = 0; i < vertices; ++i) {
        distances[i][i] = 0.0;
    }

    // Step 1 : creation of augmentes graph
    Graph augmentedGraph = createAugmentedGraph(graph);
    int augmentedVertices = augmentedGraph.getVertices();
    int source = augmentedVertices - 1; // New s vertex

    // Step 2: Running Bellman Ford from s vertex
    std::vector<double> h = bellmanFord->findShortestPaths(augmentedGraph, source);

    // Step 3^ Check for negative cycles
    auto* bf = dynamic_cast<BellmanFordAlgorithm*>(bellmanFord.get());
    if (bf && bf->hasNegativeCycle(augmentedGraph, source)) {
        std::cout << "Graph contains a negative cycle!" << std::endl;
        return distances; // Return infinits
    }

    // Removing the last elem for s vertex
    h.pop_back();

    //Step 4: creation of new reweighted graph
    Graph reweightedGraph = createReweightedGraph(graph, h);

    //Step 5: Running Dijkstra from each vertex
    for (int u = 0; u < vertices; ++u) {
        std::vector<double> singleSourceDistances = dijkstra->findShortestPaths(reweightedGraph, u);

        for (int v = 0; v < vertices; ++v) {
            distances[u][v] = singleSourceDistances[v];
        }
    }

    // Step 6: restoring original weights
    restoreOriginalWeights(distances, h);

    return distances;
}

std::vector<std::vector<double>> JohnsonAlgorithm::findAllPairsShortestPathsParallel(const Graph& graph,
    int numThreads) {
    int vertices = graph.getVertices();

    // Automatically determine optimal number of threads
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
    }

    numThreads = std::min(numThreads, vertices / 50);  // Minimum 50 vertices per thread

    std::vector<std::vector<double>> distances(vertices,
        std::vector<double>(vertices, std::numeric_limits<double>::infinity()));

    for (int i = 0; i < vertices; ++i) {
        distances[i][i] = 0.0;
    }

    // Steps 1-4 remain unchanged
    Graph augmentedGraph = createAugmentedGraph(graph);
    int augmentedVertices = augmentedGraph.getVertices();
    int source = augmentedVertices - 1;

    std::vector<double> h = bellmanFord->findShortestPaths(augmentedGraph, source);

    auto* bf = dynamic_cast<BellmanFordAlgorithm*>(bellmanFord.get());
    if (bf && bf->hasNegativeCycle(augmentedGraph, source)) {
        std::cout << "Graph contains a negative cycle!" << std::endl;
        return distances;
    }

    h.pop_back();
    Graph reweightedGraph = createReweightedGraph(graph, h);

    // Use threads instead of async with dynamic work distribution
    std::atomic<int> nextVertex{ 0 };
    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&]() {
            // Create local Dijkstra instance once per thread
            auto localDijkstra = std::make_unique<DijkstraAlgorithm>();

            int vertex;
            while ((vertex = nextVertex.fetch_add(1)) < vertices) {
                std::vector<double> singleSourceDistances =
                    localDijkstra->findShortestPaths(reweightedGraph, vertex);

                // Copy results (each thread writes to its own row)
                for (int v = 0; v < vertices; ++v) {
                    distances[vertex][v] = singleSourceDistances[v];
                }
            }
            });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Step 6: Restore original weights
    restoreOriginalWeights(distances, h);

    return distances;
}
JohnsonAlgorithm::PerformanceResult JohnsonAlgorithm::comparePerformance(const Graph& graph, int numThreads) {
    PerformanceResult result;
    int vertices = graph.getVertices();
    double density = calculateGraphDensity(graph);

    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
    }

    result.threadsUsed = numThreads;

   
    auto warmupDistances = findAllPairsShortestPaths(graph);

    // Seq case
    std::cout << "Running sequential version..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    result.distances = findAllPairsShortestPaths(graph);
    auto end = std::chrono::high_resolution_clock::now();
    result.sequentialTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    //Parallel case
        std::cout << "Running parallel version with " << numThreads << " threads..." << std::endl;
        start = std::chrono::high_resolution_clock::now();
        auto parallelDistances = findAllPairsShortestPathsParallel(graph, numThreads);
        end = std::chrono::high_resolution_clock::now();
        result.parallelTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (result.parallelTime.count() > 0) {
        result.speedup = static_cast<double>(result.sequentialTime.count()) /
            static_cast<double>(result.parallelTime.count());
    }
    else {
        result.speedup = 1.0;
    }

    //Detailed report
    std::cout << "\nPerformance Results:" << std::endl;
    std::cout << "Graph stats: " << vertices << " vertices, density: "
        << std::fixed << std::setprecision(3) << density << std::endl;
    std::cout << "Sequential time: " << result.sequentialTime.count() << " ms" << std::endl;
    std::cout << "Parallel time: " << result.parallelTime.count() << " ms" << std::endl;
    std::cout << "Speedup: " << std::fixed << std::setprecision(2) << result.speedup << "x" << std::endl;

    if (result.speedup < 1.0) {
        std::cout << "Note: Sequential version is faster due to parallelization overhead" << std::endl;
    }

    return result;
}
double JohnsonAlgorithm::calculateGraphDensity(const Graph& graph) {
    int vertices = graph.getVertices();
    int edges = 0;

    for (int i = 0; i < vertices; ++i) {
        edges += graph.getNeighbors(i).size();
    }

    int maxPossibleEdges = vertices * (vertices - 1);
    return maxPossibleEdges > 0 ? static_cast<double>(edges) / maxPossibleEdges : 0.0;
}