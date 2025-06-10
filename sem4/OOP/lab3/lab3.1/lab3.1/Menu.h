#pragma once
#include "Graph.h"
#include "JohnsonAlgorithm.h"
#include <memory>

class Menu {
private:
    std::unique_ptr<Graph> graph;
    std::unique_ptr<JohnsonAlgorithm> johnsonAlgorithm;

    // Helper methods
    void displayMainMenu();
    void createGraph();
    void addRandomEdges(int edges);
    void displayGraph();
    void runSequentialJohnson();
    void runParallelJohnson();
    void compareAlgorithms();
    bool isValidVertex(int vertex);
    int getIntInput(const std::string& prompt, int min = 0, int max = INT_MAX);
    double getDoubleInput(const std::string& prompt);

public:
    Menu();
    void run();
};