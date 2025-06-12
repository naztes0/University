#pragma once
#include "ShortestPathAlgorithm.h"
#include <memory>
#include <string>

/**
 * @file AlgorithmFactory.h
 * @brief Factory pattern implementation for algorithm creation
 */

 /**
  * @class AlgorithmFactory
  * @brief Factory class for creating shortest path algorithm instances
  */
// usage of Factory Pattern 
class AlgorithmFactory {
public:
    /**
    * @enum AlgorithmType
    * @brief Enumeration of available algorithm types
    */
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