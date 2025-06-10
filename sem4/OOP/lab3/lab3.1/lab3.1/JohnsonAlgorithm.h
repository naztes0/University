#pragma once
#include "Graph.h"
#include "ShortestPathAlgorithm.h"  
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include<iomanip>

class JohnsonAlgorithm {
private:
    std::unique_ptr<ShortestPathAlgorithm> bellmanFord;
    std::unique_ptr<ShortestPathAlgorithm> dijkstra;

    // Helper methods
    Graph createAugmentedGraph(const Graph& original);

    Graph createReweightedGraph(const Graph& graph, const std::vector<double>& h);

    void restoreOriginalWeights(std::vector<std::vector<double>>& distances,
        const std::vector<double>& h);

    double calculateGraphDensity(const Graph& graph);


public:
    JohnsonAlgorithm();

    // Sequential version
    std::vector<std::vector<double>> findAllPairsShortestPaths(const Graph& graph);

    // Parallel version
    std::vector<std::vector<double>> findAllPairsShortestPathsParallel(const Graph& graph,
        int numThreads = std::thread::hardware_concurrency());

    // Performance comparison
    struct PerformanceResult {
        std::vector<std::vector<double>> distances;
        std::chrono::milliseconds sequentialTime;
        std::chrono::milliseconds parallelTime;
        double speedup;
        int threadsUsed;
    };

    PerformanceResult comparePerformance(const Graph& graph,
        int numThreads = std::thread::hardware_concurrency());
};