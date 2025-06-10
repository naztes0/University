#include "GraphBuilder.h"
#include <stdexcept>
#include <iostream>

GraphBuilder& GraphBuilder::setVertices(int v) {
    if (v <= 0) {
        throw std::invalid_argument("Number of vertices must be positive");
    }
    vertices = v;
    return *this;
}

GraphBuilder& GraphBuilder::setTargetEdges(int edges) {
    if (edges < 0) {
        throw std::invalid_argument("Number of edges cannot be negative");
    }
    targetEdges = edges;
    return *this;
}

GraphBuilder& GraphBuilder::setWeightRange(double min, double max) {
    if (min > max) {
        std::swap(min, max);
    }
    minWeight = min;
    maxWeight = max;
    return *this;
}

GraphBuilder& GraphBuilder::useRandomGeneration() {
    useRandomEdges = true;
    return *this;
}

GraphBuilder& GraphBuilder::addManualEdge(int from, int to, double weight) {
    manualEdges.push_back({ from, to, weight });
    useRandomEdges = false;
    return *this;
}

std::unique_ptr<Graph> GraphBuilder::build() {
    if (vertices <= 0) {
        throw std::runtime_error("Vertices must be set before building graph");
    }

    auto graph = std::make_unique<Graph>(vertices);

    if (useRandomEdges) {
        addRandomEdgesToGraph(*graph);
    }
    else {
        addManualEdgesToGraph(*graph);
    }

    return graph;
}

void GraphBuilder::addRandomEdgesToGraph(Graph& graph) {
    if (targetEdges <= 0) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> vertexDist(0, vertices - 1);
    std::uniform_real_distribution<> weightDist(minWeight, maxWeight);

    int addedEdges = 0;
    int attempts = 0;
    const int maxAttempts = targetEdges * 3;

    while (addedEdges < targetEdges && attempts < maxAttempts) {
        int from = vertexDist(gen);
        int to = vertexDist(gen);

        if (from != to) { // Avoid self-loops
            double weight = weightDist(gen);
            graph.addEdge(from, to, weight);
            addedEdges++;
        }
        attempts++;
    }

    std::cout << "Added " << addedEdges << " random edges with weights ["
        << minWeight << ", " << maxWeight << "]" << std::endl;
}

void GraphBuilder::addManualEdgesToGraph(Graph& graph) {
    for (const auto& edge : manualEdges) {
        if (edge.from >= 0 && edge.from < vertices &&
            edge.to >= 0 && edge.to < vertices) {
            graph.addEdge(edge.from, edge.to, edge.weight);
        }
    }

    std::cout << "Added " << manualEdges.size() << " manual edges" << std::endl;
}