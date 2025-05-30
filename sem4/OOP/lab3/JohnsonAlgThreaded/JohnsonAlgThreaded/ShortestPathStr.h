#pragma once

#include"Graph.h"
#include<vector>
#include<limits>

struct ShortestPathResult {
    std::vector<double> distances;
    std::vector<int> predecessors;
    bool hasNegativeCycle;

    ShortestPathResult(int vertices)
        : distances(vertices, std::numeric_limits<double>::infinity()),
        predecessors(vertices, -1),
        hasNegativeCycle(false) {}
};

// Strategy Pattern для різних алгоритмів пошуку найкоротших шляхів
class ShortestPathStrategy {
public:
    virtual ~ShortestPathStrategy() = default;
    virtual ShortestPathResult findShortestPaths(const Graph& graph, int source) = 0;
    virtual std::string getAlgorithmName() const = 0;
};

// sequential BF algorithm
class BellmanFordStrategy : public ShortestPathStrategy {
public:
    ShortestPathResult findShortestPaths(const Graph& graph, int source) override;
    std::string getAlgorithmName() const override { return "Bellman-Ford"; }
};

// parallel BF algorithm
class ParallelBellmanFordStrategy : public ShortestPathStrategy {
private:
    int numThreads;

public:
    explicit ParallelBellmanFordStrategy(int threads = 4) : numThreads(threads) {}

    ShortestPathResult findShortestPaths(const Graph& graph, int source) override;
    std::string getAlgorithmName() const override {
        return "Parallel Bellman-Ford (" + std::to_string(numThreads) + " threads)";
    }
};

// sequential Dej algorithm
class DijkstraStrategy : public ShortestPathStrategy {
public:
    ShortestPathResult findShortestPaths(const Graph& graph, int source) override;
    std::string getAlgorithmName() const override { return "Dijkstra with Fibonacci Heap"; }
};

// parallel Dej algorithm
class ParallelDijkstraStrategy : public ShortestPathStrategy {
private:
    int numThreads;

public:
    explicit ParallelDijkstraStrategy(int threads = 4) : numThreads(threads) {}

    ShortestPathResult findShortestPaths(const Graph& graph, int source) override;
    std::string getAlgorithmName() const override {
        return "Parallel Dijkstra (" + std::to_string(numThreads) + " threads)";
    }
};
