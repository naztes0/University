#include "AlgorithmFactory.h"
#include<stdexcept>
#include<algorithm>

std::unique_ptr<ShortestPathAlgorithm> AlgorithmFactory::createAlgorithm(AlgorithmType type) {
    switch (type) {
    case AlgorithmType::BELLMAN_FORD:
        return std::make_unique<BellmanFordAlgorithm>();
    case AlgorithmType::DIJKSTRA:
        return std::make_unique<DijkstraAlgorithm>();
    default:
        throw std::invalid_argument("Unknown algorithm type");
    }
}

std::unique_ptr<ShortestPathAlgorithm> AlgorithmFactory::createAlgorithm(const std::string& name) {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    if (lowerName == "bellman-ford" || lowerName == "bellman") {
        return createAlgorithm(AlgorithmType::BELLMAN_FORD);
    }
    else if (lowerName == "dijkstra") {
        return createAlgorithm(AlgorithmType::DIJKSTRA);
    }
    else {
        throw std::invalid_argument("Unknown algorithm name: " + name);
    }
}
