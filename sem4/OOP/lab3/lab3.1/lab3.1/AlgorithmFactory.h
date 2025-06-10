#pragma once
#include "ShortestPathAlgorithm.h"
#include <memory>
#include <string>

// usage of Factory Pattern 
class AlgorithmFactory {
public:
    enum class AlgorithmType {
        BELLMAN_FORD,
        DIJKSTRA
    };

    static std::unique_ptr<ShortestPathAlgorithm> createAlgorithm(AlgorithmType type);
    static std::unique_ptr<ShortestPathAlgorithm> createAlgorithm(const std::string& name);
    static std::unique_ptr<DijkstraAlgorithm> createDijkstra() {
        return std::make_unique<DijkstraAlgorithm>();
    }
};