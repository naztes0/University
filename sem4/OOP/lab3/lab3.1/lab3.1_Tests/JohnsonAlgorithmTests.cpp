#include <gtest/gtest.h>
#include "JohnsonAlgorithm.h"
#include "Graph.h"
#include <limits>

class JohnsonAlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        johnson = std::make_unique<JohnsonAlgorithm>();

        // Create test graph
        graph = std::make_unique<Graph>(125);
        graph->addEdge(0, 1, 3.0);
        graph->addEdge(0, 2, 8.0);
        graph->addEdge(1, 3, 2.0);
        graph->addEdge(2, 1, -4.0);
        graph->addEdge(2, 3, 1.0);
        graph->addEdge(3, 0, 2.0);
    }

    std::unique_ptr<JohnsonAlgorithm> johnson;
    std::unique_ptr<Graph> graph;
};

TEST_F(JohnsonAlgorithmTest, SequentialBasicTest) {
    auto distances = johnson->findAllPairsShortestPaths(*graph);

    EXPECT_EQ(distances.size(), 125);
    EXPECT_EQ(distances[0].size(), 125);

    // Check diagonal (distance from vertex to itself should be 0)
    for (int i = 0; i < 125; ++i) {
        EXPECT_DOUBLE_EQ(distances[i][i], 0.0);
    }

    // Check some specific paths
    EXPECT_LT(distances[0][1], std::numeric_limits<double>::infinity());
    EXPECT_LT(distances[0][3], std::numeric_limits<double>::infinity());
}

TEST_F(JohnsonAlgorithmTest, ParallelBasicTest) {
    auto distances = johnson->findAllPairsShortestPathsParallel(*graph, 2);

    EXPECT_EQ(distances.size(), 125);
    EXPECT_EQ(distances[0].size(), 125);

    // Check diagonal
    for (int i = 0; i < 125; ++i) {
        EXPECT_DOUBLE_EQ(distances[i][i], 0.0);
    }
}

TEST_F(JohnsonAlgorithmTest, SequentialVsParallelConsistencyTest) {
    auto sequentialDistances = johnson->findAllPairsShortestPaths(*graph);
    auto parallelDistances = johnson->findAllPairsShortestPathsParallel(*graph, 2);

    EXPECT_EQ(sequentialDistances.size(), parallelDistances.size());

    // Results should be the same
    for (size_t i = 0; i < sequentialDistances.size(); ++i) {
        for (size_t j = 0; j < sequentialDistances[i].size(); ++j) {
            EXPECT_DOUBLE_EQ(sequentialDistances[i][j], parallelDistances[i][j]);
        }
    }
}

TEST_F(JohnsonAlgorithmTest, EmptyGraphTest) {
    Graph emptyGraph(1);

    auto distances = johnson->findAllPairsShortestPaths(emptyGraph);

    EXPECT_EQ(distances.size(), 1);
    EXPECT_EQ(distances[0].size(), 1);
    EXPECT_DOUBLE_EQ(distances[0][0], 0.0);
}

TEST_F(JohnsonAlgorithmTest, NegativeCycleDetectionTest) {
    // Create graph with negative cycle
    Graph negativeGraph(3);
    negativeGraph.addEdge(0, 1, 1.0);
    negativeGraph.addEdge(1, 2, -3.0);
    negativeGraph.addEdge(2, 0, 1.0);

    auto distances = johnson->findAllPairsShortestPaths(negativeGraph);

    // Algorithm should handle negative cycles gracefully
    EXPECT_EQ(distances.size(), 3);
}

TEST_F(JohnsonAlgorithmTest, PerformanceComparisonTest) {
    auto result = johnson->comparePerformance(*graph, 2);

    EXPECT_GT(result.sequentialTime.count(), 0);
    EXPECT_GT(result.parallelTime.count(), 0);
    EXPECT_GT(result.speedup, 0.0);
    EXPECT_EQ(result.threadsUsed, 2);
    EXPECT_EQ(result.distances.size(), 125);
}

TEST_F(JohnsonAlgorithmTest, DefaultThreadCountTest) {
    // Test with default thread count (should not crash)
    EXPECT_NO_THROW(johnson->findAllPairsShortestPathsParallel(*graph));

    auto result = johnson->comparePerformance(*graph);
    EXPECT_GT(result.threadsUsed, 0);
}