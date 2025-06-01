#pragma once

#include "Graph.h"
#include "ShortestPathStr.h"
#include "JohnsonAlg.h"
#include <memory>

class Menu {
private:
    std::unique_ptr<Graph> graph;
    int defaultThreads;

    void displayMainMenu();
    void createGraph();
    void addEdges();
    void runBellmanFordComparison();
    void runDijkstraComparison();
    void runJohnsonComparison();
    void displaySingleSourceResults(const ShortestPathResult& result, const std::string& algorithmName, double executionTime);
    void displayJohnsonResults(const JohnsonResult& result, const std::string& algorithmName);
    void printGraph();
    int getIntInput(const std::string& prompt);
    double getDoubleInput(const std::string& prompt);
    int getSourceVertex();
    void addSampleEdges(Graph& graph, int vertexCount = 200, int edgeCount = 2000);

public:
    Menu();
    ~Menu() = default;

    void run();
};