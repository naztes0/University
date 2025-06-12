#include <gtest/gtest.h>
#include "GraphBuilder.h"
#include "Graph.h"

class GraphBuilderTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(GraphBuilderTest, BuildBasicGraphTest) {
    GraphBuilder builder;

    auto graph = builder.setVertices(3)
        .addManualEdge(0, 1, 5.0)
        .addManualEdge(1, 2, 3.0)
        .build();

    ASSERT_NE(graph, nullptr);
    EXPECT_EQ(graph->getVertices(), 3);

    // Check edges
    const auto& neighbors0 = graph->getNeighbors(0);
    EXPECT_EQ(neighbors0.size(), 1);
    EXPECT_EQ(neighbors0[0].to, 1);
    EXPECT_DOUBLE_EQ(neighbors0[0].weight, 5.0);
}

TEST_F(GraphBuilderTest, BuildRandomGraphTest) {
    GraphBuilder builder;

    auto graph = builder.setVertices(5)
        .setTargetEdges(3)
        .setWeightRange(1.0, 10.0)
        .useRandomGeneration()
        .build();

    ASSERT_NE(graph, nullptr);
    EXPECT_EQ(graph->getVertices(), 5);
}

TEST_F(GraphBuilderTest, InvalidVerticesThrowsExceptionTest) {
    GraphBuilder builder;

    EXPECT_THROW(builder.setVertices(0), std::invalid_argument);
    EXPECT_THROW(builder.setVertices(-5), std::invalid_argument);
}

TEST_F(GraphBuilderTest, InvalidEdgesThrowsExceptionTest) {
    GraphBuilder builder;

    EXPECT_THROW(builder.setTargetEdges(-1), std::invalid_argument);
}

TEST_F(GraphBuilderTest, BuildWithoutVerticesThrowsExceptionTest) {
    GraphBuilder builder;

    EXPECT_THROW(builder.build(), std::runtime_error);
}

TEST_F(GraphBuilderTest, WeightRangeSwapTest) {
    GraphBuilder builder;

    // Should not throw - weights should be swapped internally
    EXPECT_NO_THROW(builder.setWeightRange(10.0, 1.0));
}

TEST_F(GraphBuilderTest, ChainedCallsTest) {
    GraphBuilder builder;

    // Test method chaining
    EXPECT_NO_THROW(
        builder.setVertices(4)
        .setTargetEdges(2)
        .setWeightRange(-5.0, 15.0)
        .addManualEdge(0, 1, 2.5)
        .addManualEdge(2, 3, -1.0)
    );

    auto graph = builder.build();
    ASSERT_NE(graph, nullptr);
    EXPECT_EQ(graph->getVertices(), 4);
}