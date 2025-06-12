#pragma once
#include "Graph.h"
#include "ShortestPathAlgorithm.h"  
#include"Observer.h"
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include<iomanip>


/**
 * @file JohnsonAlgorithm.h
 * @brief Johnson's Algorithm for All-Pairs Shortest Paths
 */


 /**
  * @class JohnsonAlgorithm
  * @brief Implements Johnson's algorithm for finding all-pairs shortest paths
  *
  * Johnson's algorithm combines Bellman-Ford and Dijkstra algorithms to efficiently
  * compute shortest paths between all pairs of vertices in a sparse graph.
  * Supports both sequential and parallel execution.
  */
class JohnsonAlgorithm : public AlgorithmSubject {  // Inherit from AlgorithmSubject
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

    /**
    * @brief Finds all-pairs shortest paths using sequential execution
    * @return 2D vector of shortest distances between all vertex pairs
    */
    // Sequential version
    std::vector<std::vector<double>> findAllPairsShortestPaths(const Graph& graph);


    /**
    * @brief Finds all-pairs shortest paths using parallel execution
    * @return 2D vector of shortest distances between all vertex pairs
    */
    // Parallel version
    std::vector<std::vector<double>> findAllPairsShortestPathsParallel(const Graph& graph,
        int numThreads = std::thread::hardware_concurrency());
    /**
     * @struct PerformanceResult
     * @brief Contains performance comparison results
     */
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