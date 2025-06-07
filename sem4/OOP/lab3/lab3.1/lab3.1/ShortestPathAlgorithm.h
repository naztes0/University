#pragma once
#include "Graph.h"
#include <vector>
#include <unordered_map>
#include<string>
#include<queue>

// usage of Strategy Pattern 
class ShortestPathAlgorithm {
public:
    virtual ~ShortestPathAlgorithm() = default;
    virtual std::vector<double> findShortestPaths(const Graph& graph, int source) = 0;
    virtual std::string getName() const = 0;
};

class BellmanFordAlgorithm : public ShortestPathAlgorithm {
public:
    std::vector<double> findShortestPaths(const Graph& graph, int source) override;
    std::string getName() const override { return "Bellman-Ford"; }
    bool hasNegativeCycle(const Graph& graph, int source);
};

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
