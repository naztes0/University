#include <gtest/gtest.h>
#include"Graph.h"

class GraphTest : public ::testing::Test {
protected:
    void SetUp() override {
        // ��������� ���� � 4 ��������� ��� �����
        graph = std::make_unique<Graph>(4);
    }

    void TearDown() override {
        graph.reset();
    }

    std::unique_ptr<Graph> graph;
};

// ���� ������������
TEST_F(GraphTest, ConstructorTest) {
    EXPECT_EQ(graph->getVertices(), 4);
}

// ���� ��������� �����
TEST_F(GraphTest, AddEdgeTest) {
    graph->addEdge(0, 1, 5.0);
    graph->addEdge(0, 2, 3.0);
    graph->addEdge(1, 3, 2.0);

    const auto& neighbors0 = graph->getNeighbors(0);
    EXPECT_EQ(neighbors0.size(), 2);

    // ���������� ����� ����� (0 -> 1, ���� 5.0)
    EXPECT_EQ(neighbors0[0].to, 1);
    EXPECT_DOUBLE_EQ(neighbors0[0].weight, 5.0);

    // ���������� ����� ����� (0 -> 2, ���� 3.0)
    EXPECT_EQ(neighbors0[1].to, 2);
    EXPECT_DOUBLE_EQ(neighbors0[1].weight, 3.0);
}

// ���� ��������� �����
TEST_F(GraphTest, RemoveEdgeTest) {
    graph->addEdge(0, 1, 5.0);
    graph->addEdge(0, 2, 3.0);

    // ����������, �� ����� �����
    EXPECT_EQ(graph->getNeighbors(0).size(), 2);

    // ��������� �����
    graph->removeEdge(0, 1);

    // ����������, �� ����� ��������
    const auto& neighbors = graph->getNeighbors(0);
    EXPECT_EQ(neighbors.size(), 1);
    EXPECT_EQ(neighbors[0].to, 2);
    EXPECT_DOUBLE_EQ(neighbors[0].weight, 3.0);
}

// ���� ����������� ������� ������
TEST_F(GraphTest, InvalidVertexTest) {
    // ��������� ����� � ������������ ��������� �� ������� ���������� ����
    graph->addEdge(-1, 1, 5.0);
    graph->addEdge(0, 5, 3.0);
    graph->addEdge(5, 1, 2.0);

    // ���� ������� ���������� �������
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(graph->getNeighbors(i).size(), 0);
    }
}

// ���� ��������� ����� ��� ���������� �������
TEST_F(GraphTest, GetNeighborsInvalidVertexTest) {
    const auto& neighbors = graph->getNeighbors(-1);
    EXPECT_EQ(neighbors.size(), 0);

    const auto& neighbors2 = graph->getNeighbors(5);
    EXPECT_EQ(neighbors2.size(), 0);
}

// ���� �������� �������� ��'����� ���
TEST_F(GraphTest, HasNegativeWeightsTest) {
    // �������� �� �� ��'����� ���
    EXPECT_FALSE(graph->hasNegativeWeights());

    // ������ �������� ����
    graph->addEdge(0, 1, 5.0);
    graph->addEdge(1, 2, 3.0);
    EXPECT_FALSE(graph->hasNegativeWeights());

    // ������ ��'���� ����
    graph->addEdge(2, 3, -2.0);
    EXPECT_TRUE(graph->hasNegativeWeights());
}

// ���� � ��������� ������
TEST_F(GraphTest, ZeroWeightTest) {
    graph->addEdge(0, 1, 0.0);
    EXPECT_FALSE(graph->hasNegativeWeights());

    const auto& neighbors = graph->getNeighbors(0);
    EXPECT_EQ(neighbors.size(), 1);
    EXPECT_DOUBLE_EQ(neighbors[0].weight, 0.0);
}

// ���� ��������� ����� �� ���� ������ ���������
TEST_F(GraphTest, MultipleEdgesTest) {
    graph->addEdge(0, 1, 5.0);
    graph->addEdge(0, 1, 3.0);

    const auto& neighbors = graph->getNeighbors(0);
    EXPECT_EQ(neighbors.size(), 2);

    // ������ ����� ������ ��������
    EXPECT_EQ(neighbors[0].to, 1);
    EXPECT_EQ(neighbors[1].to, 1);
    EXPECT_DOUBLE_EQ(neighbors[0].weight, 5.0);
    EXPECT_DOUBLE_EQ(neighbors[1].weight, 3.0);
}