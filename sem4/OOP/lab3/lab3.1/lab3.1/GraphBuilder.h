#pragma once
#include "Graph.h"
#include <memory>
#include <random>

class GraphBuilder {
private:
    int vertices = 0;
    int targetEdges = 0;
    double minWeight = 1.0;
    double maxWeight = 100.0;
    bool useRandomEdges = false;

    // For manual edges
    struct EdgeInfo {
        int from, to;
        double weight;
    };
    std::vector<EdgeInfo> manualEdges;

public:
    GraphBuilder& setVertices(int v);
    GraphBuilder& setTargetEdges(int edges);
    GraphBuilder& setWeightRange(double min, double max);
    GraphBuilder& useRandomGeneration();
    GraphBuilder& addManualEdge(int from, int to, double weight);

    std::unique_ptr<Graph> build();

private:
    void addRandomEdgesToGraph(Graph& graph);
    void addManualEdgesToGraph(Graph& graph);
};