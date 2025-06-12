#pragma once
#include "Graph.h"
#include "JohnsonAlgorithm.h"
#include "ConcreteObservers.h"
#include <memory>

/**
 * @file Menu.h
 * @brief Interactive menu system for the application
 */

 /**
  * @class Menu
  * @brief Provides interactive command-line interface for Johnson's algorithm
  *
  * Manages user interaction, graph creation, algorithm execution,
  * and performance comparison through a menu-driven interface.
  */
class Menu {
private:
    std::unique_ptr<Graph> graph;
    std::unique_ptr<JohnsonAlgorithm> johnsonAlgorithm;
    std::shared_ptr<ConsoleObserver> consoleObserver;
    std::shared_ptr<ProgressBarObserver> progressObserver;

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
    void configureObservers();

public:
    Menu();
    /**
     * @brief Runs the main menu loop
     */
    void run();
};