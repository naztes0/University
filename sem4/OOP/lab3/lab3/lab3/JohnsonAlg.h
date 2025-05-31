#pragma once

#include "Graph.h"
#include "ShortestPathStr.h"
#include <vector>
#include <memory>
#include <chrono>

struct JohnsonResult {
    std::vector<std::vector<double>> distances;
    bool hasNegativeCycle;
    std::chrono::milliseconds executionTime;

    explicit JohnsonResult(int vertices)
        : distances(vertices, std::vector<double>(vertices, std::numeric_limits<double>::infinity())),
        hasNegativeCycle(false),
        executionTime(0) {
        for (int i = 0; i < vertices; ++i) {
            distances[i][i] = 0.0;
        }
    }
};

// Основний клас алгоритму Джонсона (Template Method Pattern)
class JohnsonAlgorithm {
private:
    std::unique_ptr<ShortestPathStrategy> bellmanFordStrategy;
protected:
    // Template Method - основний алгоритм

    std::unique_ptr<ShortestPathStrategy> dijkstraStrategy;

    virtual JohnsonResult executeAlgorithm(const Graph& graph);

    // Кроки алгоритму, які можуть бути перевизначені
    virtual Graph createAugmentedGraph(const Graph& original);
    virtual std::vector<double> computePotentials(const Graph& augmentedGraph);
    virtual Graph reweightGraph(const Graph& original, const std::vector<double>& potentials);
    virtual std::vector<std::vector<double>> computeAllPairsShortestPaths(
        const Graph& reweightedGraph, const std::vector<double>& potentials);

public:
    JohnsonAlgorithm(std::unique_ptr<ShortestPathStrategy> bfStrategy,
        std::unique_ptr<ShortestPathStrategy> dijkstraStrat);

    virtual ~JohnsonAlgorithm() = default;

    JohnsonResult findAllPairsShortestPaths(const Graph& graph);

    // Factory Method для створення стратегій
    static std::unique_ptr<JohnsonAlgorithm> createSequential();
    static std::unique_ptr<JohnsonAlgorithm> createParallel(int numThreads = 4);
};

// Паралельна версія алгоритму Джонсона
class ParallelJohnsonAlgorithm : public JohnsonAlgorithm {
private:
    int numThreads;

protected:
    std::vector<std::vector<double>> computeAllPairsShortestPaths(
        const Graph& reweightedGraph, const std::vector<double>& potentials) override;

public:
    ParallelJohnsonAlgorithm(std::unique_ptr<ShortestPathStrategy> bfStrategy,
        std::unique_ptr<ShortestPathStrategy> dijkstraStrat,
        int threads);
};