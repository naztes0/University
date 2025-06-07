#pragma once
#include "ShortestPathAlgorithm.h"
#include "Graph.h"
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

// Additional interface for algorithms working with all pairs of vertices
class AllPairsShortestPath {
public:
    virtual ~AllPairsShortestPath() = default;
    virtual std::vector<std::vector<double>> findAllPairsShortestPaths(const Graph& graph) = 0;
    virtual std::vector<std::vector<double>> findAllPairsShortestPathsParallel(const Graph& graph, int numThreads = std::thread::hardware_concurrency()) = 0;
    virtual std::string getName() const = 0;
    virtual std::chrono::microseconds getLastExecutionTime() const = 0;
};

// Johnson's algorithm implementation
class JohnsonAlgorithm : public AllPairsShortestPath {
private:
    std::unique_ptr<ShortestPathAlgorithm> bellmanFord;
    std::unique_ptr<ShortestPathAlgorithm> dijkstra;
    mutable std::chrono::microseconds lastExecutionTime{ 0 };

    // Helper methods
    Graph reweightGraph(const Graph& originalGraph, const std::vector<double>& h) const;
    void processVertexRange(const Graph& reweightedGraph, const std::vector<double>& h,
        std::vector<std::vector<double>>& result, int start, int end) const;

public:
    JohnsonAlgorithm();

    std::vector<std::vector<double>> findAllPairsShortestPaths(const Graph& graph) override;
    std::vector<std::vector<double>> findAllPairsShortestPathsParallel(const Graph& graph,
        int numThreads = std::thread::hardware_concurrency()) override;

    // Alternative parallel implementation with batched processing
    std::vector<std::vector<double>> findAllPairsShortestPathsParallelBatched(const Graph& graph,
        int numThreads = std::thread::hardware_concurrency());

    std::string getName() const override { return "Johnson"; }
    std::chrono::microseconds getLastExecutionTime() const override { return lastExecutionTime; }

    // Additional methods for testing
    bool canHandleNegativeWeights() const { return true; }
    void printExecutionStats() const;
};