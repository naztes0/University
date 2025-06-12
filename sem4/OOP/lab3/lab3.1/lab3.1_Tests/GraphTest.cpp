#include <gtest/gtest.h>
#include"Graph.h"

class GraphTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Створюємо граф з 4 вершинами для тестів
        graph = std::make_unique<Graph>(4);
    }

    void TearDown() override {
        graph.reset();
    }

    std::unique_ptr<Graph> graph;
};

// Тест конструктора
TEST_F(GraphTest, ConstructorTest) {
    EXPECT_EQ(graph->getVertices(), 4);
}

// Тест додавання ребер
TEST_F(GraphTest, AddEdgeTest) {
    graph->addEdge(0, 1, 5.0);
    graph->addEdge(0, 2, 3.0);
    graph->addEdge(1, 3, 2.0);

    const auto& neighbors0 = graph->getNeighbors(0);
    EXPECT_EQ(neighbors0.size(), 2);

    // Перевіряємо перше ребро (0 -> 1, вага 5.0)
    EXPECT_EQ(neighbors0[0].to, 1);
    EXPECT_DOUBLE_EQ(neighbors0[0].weight, 5.0);

    // Перевіряємо друге ребро (0 -> 2, вага 3.0)
    EXPECT_EQ(neighbors0[1].to, 2);
    EXPECT_DOUBLE_EQ(neighbors0[1].weight, 3.0);
}

// Тест видалення ребер
TEST_F(GraphTest, RemoveEdgeTest) {
    graph->addEdge(0, 1, 5.0);
    graph->addEdge(0, 2, 3.0);

    // Перевіряємо, що ребра додані
    EXPECT_EQ(graph->getNeighbors(0).size(), 2);

    // Видаляємо ребро
    graph->removeEdge(0, 1);

    // Перевіряємо, що ребро видалено
    const auto& neighbors = graph->getNeighbors(0);
    EXPECT_EQ(neighbors.size(), 1);
    EXPECT_EQ(neighbors[0].to, 2);
    EXPECT_DOUBLE_EQ(neighbors[0].weight, 3.0);
}

// Тест некоректних індексів вершин
TEST_F(GraphTest, InvalidVertexTest) {
    // Додавання ребра з некоректними індексами не повинно спричиняти краш
    graph->addEdge(-1, 1, 5.0);
    graph->addEdge(0, 5, 3.0);
    graph->addEdge(5, 1, 2.0);

    // Граф повинен залишитися порожнім
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(graph->getNeighbors(i).size(), 0);
    }
}

// Тест отримання сусідів для некоректної вершини
TEST_F(GraphTest, GetNeighborsInvalidVertexTest) {
    const auto& neighbors = graph->getNeighbors(-1);
    EXPECT_EQ(neighbors.size(), 0);

    const auto& neighbors2 = graph->getNeighbors(5);
    EXPECT_EQ(neighbors2.size(), 0);
}

// Тест перевірки наявності від'ємних ваг
TEST_F(GraphTest, HasNegativeWeightsTest) {
    // Спочатку не має від'ємних ваг
    EXPECT_FALSE(graph->hasNegativeWeights());

    // Додаємо позитивні ваги
    graph->addEdge(0, 1, 5.0);
    graph->addEdge(1, 2, 3.0);
    EXPECT_FALSE(graph->hasNegativeWeights());

    // Додаємо від'ємну вагу
    graph->addEdge(2, 3, -2.0);
    EXPECT_TRUE(graph->hasNegativeWeights());
}

// Тест з нульовими вагами
TEST_F(GraphTest, ZeroWeightTest) {
    graph->addEdge(0, 1, 0.0);
    EXPECT_FALSE(graph->hasNegativeWeights());

    const auto& neighbors = graph->getNeighbors(0);
    EXPECT_EQ(neighbors.size(), 1);
    EXPECT_DOUBLE_EQ(neighbors[0].weight, 0.0);
}

// Тест множинних ребер між тими самими вершинами
TEST_F(GraphTest, MultipleEdgesTest) {
    graph->addEdge(0, 1, 5.0);
    graph->addEdge(0, 1, 3.0);

    const auto& neighbors = graph->getNeighbors(0);
    EXPECT_EQ(neighbors.size(), 2);

    // Обидва ребра повинні існувати
    EXPECT_EQ(neighbors[0].to, 1);
    EXPECT_EQ(neighbors[1].to, 1);
    EXPECT_DOUBLE_EQ(neighbors[0].weight, 5.0);
    EXPECT_DOUBLE_EQ(neighbors[1].weight, 3.0);
}