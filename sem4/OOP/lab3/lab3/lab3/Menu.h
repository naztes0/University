#pragma once

#include "Graph.h"
#include "ShortestPathStr.h"
#include"JohnsonAlg.h"
#include <memory>

class Menu {
private:
    std::unique_ptr<Graph> graph;
    std::unique_ptr<ShortestPathStrategy> strategy;
    std::unique_ptr<JohnsonAlgorithm> johnAlg;

    void displayMainMenu();
    void createGraph();
    void addEdges();
    void selectAlgorithm();
    void runAlgorithm();
    void displayResults(const ShortestPathResult& result);
    void printGraph();
    int getIntInput(const std::string& prompt);
    double getDoubleInput(const std::string& prompt);

public:
    Menu();
    ~Menu() = default;

    void run();
};