#include "Menu.h"
#include"GraphBuilder.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <random>
#include <thread>

Menu::Menu() : johnsonAlgorithm(std::make_unique<JohnsonAlgorithm>()) {
}

void Menu::displayMainMenu() {
    std::cout << "\n=================================" << std::endl;
    std::cout << "    JOHNSON'S ALGORITHM MENU    " << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "1. Create new graph" << std::endl;
    std::cout << "2. Display current graph" << std::endl;
    std::cout << "3. Run sequential Johnson's algorithm" << std::endl;
    std::cout << "4. Run parallel Johnson's algorithm" << std::endl;
    std::cout << "5. Compare sequential vs parallel performance" << std::endl;
    std::cout << "6. Exit" << std::endl;
    std::cout << "=================================" << std::endl;
}

void Menu::createGraph() {
    int vertices = getIntInput("Enter number of vertices: ");
    int edges = getIntInput("Enter number of edges: ", 0, vertices * (vertices - 1));

    GraphBuilder builder;
    builder.setVertices(vertices).setTargetEdges(edges);

    std::cout << "\nChoose edge input method:" << std::endl;
    std::cout << "1. Manual input" << std::endl;
    std::cout << "2. Random generation" << std::endl;

    int choice = getIntInput("Enter choice (1-2): ", 1, 2);

    if (choice == 1) {
        // Manual edge input
        std::cout << "\nEnter edges (from to weight):" << std::endl;
        for (int i = 0; i < edges; ++i) {
            std::cout << "Edge " << (i + 1) << ": ";
            int from = getIntInput("From vertex: ", 0, vertices - 1);
            int to = getIntInput("To vertex: ", 0, vertices - 1);
            double weight = getDoubleInput("Weight: ");

            builder.addManualEdge(from, to, weight);
        }
    }
    else {
        // Random edge generation
        double minWeight = 1.0, maxWeight = 100.0;

        std::cout << "Choose weight range:" << std::endl;
        std::cout << "1. Positive weights only (1-100)" << std::endl;
        std::cout << "2. Mixed weights (-50 to 100)" << std::endl;
        std::cout << "3. Custom range" << std::endl;

        int weightChoice = getIntInput("Enter choice (1-3): ", 1, 3);

        switch (weightChoice) {
        case 2:
            minWeight = -50.0;
            maxWeight = 100.0;
            break;
        case 3:
            minWeight = getDoubleInput("Enter minimum weight: ");
            maxWeight = getDoubleInput("Enter maximum weight: ");
            break;
        }

        builder.setWeightRange(minWeight, maxWeight).useRandomGeneration();
    }

    graph = builder.build();
    std::cout << "\nGraph created successfully!" << std::endl;
}

void Menu::displayGraph() {
    if (!graph) {
        std::cout << "No graph exists. Please create a graph first." << std::endl;
        return;
    }

    std::cout << "\nCurrent Graph:" << std::endl;
    std::cout << "Vertices: " << graph->getVertices() << std::endl;
    std::cout << "Has negative weights: " << (graph->hasNegativeWeights() ? "Yes" : "No") << std::endl;
    std::cout << "\nAdjacency List:" << std::endl;
    graph->printGraph();
}

void Menu::runSequentialJohnson() {
    if (!graph) {
        std::cout << "No graph exists. Please create a graph first." << std::endl;
        return;
    }

    std::cout << "\nRunning Sequential Johnson's Algorithm..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    auto distances = johnsonAlgorithm->findAllPairsShortestPaths(*graph);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Sequential execution completed in " << duration.count() << " ms" << std::endl;
}

void Menu::runParallelJohnson() {
    if (!graph) {
        std::cout << "No graph exists. Please create a graph first." << std::endl;
        return;
    }

    std::cout << "\nRunning Parallel Johnson's Algorithm..." << std::endl;
    std::cout << "Automatically selecting optimal thread configuration..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    auto distances = johnsonAlgorithm->findAllPairsShortestPathsParallel(*graph);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Parallel execution completed in " << duration.count() << " ms" << std::endl;

    
}

void Menu::compareAlgorithms() {
    if (!graph) {
        std::cout << "No graph exists. Please create a graph first." << std::endl;
        return;
    }

    int maxThreads = std::thread::hardware_concurrency();
    std::cout << "Available CPU threads: " << maxThreads << std::endl;

    int numThreads = getIntInput("Enter number of threads for parallel version: ", 1, maxThreads );

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PERFORMANCE COMPARISON" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    auto result = johnsonAlgorithm->comparePerformance(*graph, numThreads);

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "SUMMARY" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    if (result.speedup > 1.0) {
        std::cout << "Parallel version is " << std::fixed << std::setprecision(2)
            << result.speedup << "x FASTER" << std::endl;
    }
    else if (result.speedup < 1.0) {
        std::cout << "Sequential version is " << std::fixed << std::setprecision(2)
            << (1.0 / result.speedup) << "x FASTER" << std::endl;
    }
    else {
        std::cout << "Both versions have similar performance" << std::endl;
    }
}

bool Menu::isValidVertex(int vertex) {
    return graph && vertex >= 0 && vertex < graph->getVertices();
}

int Menu::getIntInput(const std::string& prompt, int min, int max) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        else {
            std::cout << "Invalid input. Please enter a number between " << min << " and " << max << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

double Menu::getDoubleInput(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        else {
            std::cout << "Invalid input. Please enter a valid number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void Menu::run() {
    int choice;

    std::cout << "Welcome to Johnson's Algorithm Implementation!" << std::endl;
    std::cout << "This program finds all-pairs shortest paths in weighted graphs." << std::endl;

    do {
        displayMainMenu();
        choice = getIntInput("Enter your choice (1-6): ", 1, 6);

        switch (choice) {
        case 1:
            createGraph();
            break;
        case 2:
            displayGraph();
            break;
        case 3:
            runSequentialJohnson();
            break;
        case 4:
            runParallelJohnson();
            break;
        case 5:
            compareAlgorithms();
            break;
        case 6:
            std::cout << "Thank you for using Johnson's Algorithm Implementation!" << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }

        if (choice != 6) {
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
        }

    } while (choice != 6);
}