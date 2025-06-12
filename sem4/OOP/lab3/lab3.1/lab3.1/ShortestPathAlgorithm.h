#pragma once
#include "Graph.h"
#include <vector>
#include <unordered_map>
#include<string>
#include<queue>


/**
 * @file ShortestPathAlgorithm.h
 * @brief Strategy pattern implementation for shortest path algorithms
 */

 /**
  * @class ShortestPathAlgorithm
  * @brief Abstract base class for shortest path algorithms
  */
// usage of Strategy Pattern 
class ShortestPathAlgorithm {
public:
    virtual ~ShortestPathAlgorithm() = default;
    /**
    * @brief Finds shortest paths from a source vertex
    * @param graph Input graph
    * @param source Source vertex
    * @return Vector of shortest distances from source to all vertices
    */
    virtual std::vector<double> findShortestPaths(const Graph& graph, int source) = 0;
    /**
    * @brief Gets the algorithm name
    * @return Algorithm name string
    */
    virtual std::string getName() const = 0;
};

/**
 * @class BellmanFordAlgorithm
 * @brief Bellman-Ford algorithm implementation
 *
 * Handles graphs with negative weights and detects negative cycles.
 * Time complexity: O(VE)
 */
class BellmanFordAlgorithm : public ShortestPathAlgorithm {
public:
    std::vector<double> findShortestPaths(const Graph& graph, int source) override;
    std::string getName() const override { return "Bellman-Ford"; }
    bool hasNegativeCycle(const Graph& graph, int source);
};

/**
 * @class DijkstraAlgorithm
 * @brief Dijkstra's algorithm implementation
 *
 * Efficient for graphs with non-negative weights.
 * Time complexity: O((V + E) log V)
 */
class DijkstraAlgorithm : public ShortestPathAlgorithm {
private:
    struct Compare {
        bool operator()(const std::pair<double, int>& a, const std::pair<double, int>& b) {
            return a.first > b.first; // min-heap
        }
    };

public:
    std::vector<double> findShortestPaths(const Graph& graph, int source) override;
    std::string getName() const override { return "Dijkstra"; }
};
