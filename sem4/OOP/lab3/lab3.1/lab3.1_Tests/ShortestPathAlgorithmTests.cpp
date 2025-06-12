#include <gtest/gtest.h>
#include "ShortestPathAlgorithm.h"
#include "Graph.h"
#include <limits>

class ShortestPathAlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple test graph
        graph = std::make_unique<Graph>(4);
        graph->addEdge(0, 1, 5.0);
        graph->addEdge(0, 2, 3.0);
        graph->addEdge(1, 3, 2.0);
        graph->addEdge(2, 3, 7.0);
        graph->addEdge(2, 1, 1.0);
    }

    std::unique_ptr<Graph> graph;
};

// Bellman-Ford Tests
TEST_F(ShortestPathAlgorithmTest, BellmanFordBasicTest) {
    BellmanFordAlgorithm bf;

    auto distances = bf.findShortestPaths(*graph, 0);

    EXPECT_DOUBLE_EQ(distances[0], 0.0);
    EXPECT_DOUBLE_EQ(distances[1], 4.0); // 0->2->1 = 3+1 = 4
    EXPECT_DOUBLE_EQ(distances[2], 3.0); // 0->2 = 3
    EXPECT_DOUBLE_EQ(distances[3], 6.0); // 0->2->1->3 = 3+1+2 = 6
}

TEST_F(ShortestPathAlgorithmTest, BellmanFordInvalidSourceTest) {
    BellmanFordAlgorithm bf;

    auto distances = bf.findShortestPaths(*graph, -1);

    // All distances should be infinity for invalid source
    for (double dist : distances) {
        EXPECT_EQ(dist, std::numeric_limits<double>::infinity());
    }
}

TEST_F(ShortestPathAlgorithmTest, BellmanFordNegativeCycleDetection) {
    BellmanFordAlgorithm bf;

    // Graph without negative cycle
    EXPECT_FALSE(bf.hasNegativeCycle(*graph, 0));

    // Add negative cycle
    graph->addEdge(3, 0, -10.0);
    EXPECT_TRUE(bf.hasNegativeCycle(*graph, 0));
}

// Dijkstra Tests
TEST_F(ShortestPathAlgorithmTest, DijkstraBasicTest) {
    DijkstraAlgorithm dijkstra;

    auto distances = dijkstra.findShortestPaths(*graph, 0);

    EXPECT_DOUBLE_EQ(distances[0], 0.0);
    EXPECT_DOUBLE_EQ(distances[1], 4.0);
    EXPECT_DOUBLE_EQ(distances[2], 3.0);
    EXPECT_DOUBLE_EQ(distances[3], 6.0);
}

TEST_F(ShortestPathAlgorithmTest, DijkstraInvalidSourceTest) {
    DijkstraAlgorithm dijkstra;

    auto distances = dijkstra.findShortestPaths(*graph, 10);

    // All distances should be infinity for invalid source
    for (double dist : distances) {
        EXPECT_EQ(dist, std::numeric_limits<double>::infinity());
    }
}

TEST_F(ShortestPathAlgorithmTest, DijkstraSingleVertexTest) {
    Graph singleVertex(1);
    DijkstraAlgorithm dijkstra;

    auto distances = dijkstra.findShortestPaths(singleVertex, 0);

    EXPECT_EQ(distances.size(), 1);
    EXPECT_DOUBLE_EQ(distances[0], 0.0);
}

TEST_F(ShortestPathAlgorithmTest, AlgorithmNamesTest) {
    BellmanFordAlgorithm bf;
    DijkstraAlgorithm dijkstra;

    EXPECT_EQ(bf.getName(), "Bellman-Ford");
    EXPECT_EQ(dijkstra.getName(), "Dijkstra");
}