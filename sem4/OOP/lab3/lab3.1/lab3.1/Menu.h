#pragma once

#include "Graph.h"
#include "JohnsonAlgorithm.h"
#include <vector>
#include <limits>
#include <memory>
#include <cstdlib>
#include <ctime>

class Menu {
private:
    void displayMenu();

    // ������ ��� ������ � ������
    void createGraph(std::unique_ptr<Graph>& graph);
    void addEdge(std::unique_ptr<Graph>& graph);
    void removeEdge(std::unique_ptr<Graph>& graph);
    void displayGraph(const std::unique_ptr<Graph>& graph);

    // ������ ��� ������� ���������
    void runSequentialJohnson(const std::unique_ptr<Graph>& graph, JohnsonAlgorithm& johnson);
    void runParallelJohnson(const std::unique_ptr<Graph>& graph, JohnsonAlgorithm& johnson);
    void comparePerformance(const std::unique_ptr<Graph>& graph, JohnsonAlgorithm& johnson);

    // ������ ��� ��������� �������� ������
    void createTestGraph(std::unique_ptr<Graph>& graph);
    void createSmallTestGraph(std::unique_ptr<Graph>& graph);
    void createMediumTestGraph(std::unique_ptr<Graph>& graph);
    void createLargeTestGraph(std::unique_ptr<Graph>& graph);

    

public:
    void run();
};