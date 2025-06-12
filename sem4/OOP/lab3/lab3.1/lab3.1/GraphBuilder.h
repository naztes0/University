#pragma once
#include "Graph.h"
#include <memory>
#include <random>


/**
 * @file GraphBuilder.h
 * @brief Builder pattern implementation for graph construction
 */

 /**
  * @class GraphBuilder
  * @brief Builder class for creating graphs with various configurations
  *
  * Uses the Builder pattern to provide a fluent interface for graph construction.
  * Supports both manual edge specification and random graph generation.
  */
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
    /**
     * @brief Enables random edge generation mode
     * @return Reference to this builder for method chaining
     */
    GraphBuilder& useRandomGeneration();
    /**
* @brief Adds a manual edge to the graph
* @param from Source vertex
* @param to Destination vertex
* @param weight Edge weight
* @return Reference to this builder for method chaining
*/
    GraphBuilder& addManualEdge(int from, int to, double weight);

    /**
     * @brief Builds and returns the configured graph
     * @return Unique pointer to the constructed graph
     */
    std::unique_ptr<Graph> build();

private:
    void addRandomEdgesToGraph(Graph& graph);
    void addManualEdgesToGraph(Graph& graph);
};