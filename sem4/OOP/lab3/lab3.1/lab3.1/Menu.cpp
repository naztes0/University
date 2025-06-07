#include "Menu.h"
#include "Graph.h"
#include "JohnsonAlgorithm.h"
#include <iostream>
#include <memory>
#include <iomanip>
#include <thread>

void Menu::displayMenu() {
    std::cout << "\n========== MENU FOR JOHNSON'S ALGORITHM ==========" << std::endl;
    std::cout << "1. Create a new graph" << std::endl;
    std::cout << "2. Edge adding" << std::endl;
    std::cout << "3. Show the graph" << std::endl;
    std::cout << "4. Run Johnson's algorithm (sequential)" << std::endl;
    std::cout << "5. Run Johnson's algorithm (multi-threaded)" << std::endl;
    std::cout << "6. Efficiency comparison" << std::endl;
    std::cout << "7. Create test graph " << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "Your choice: ";
}

void Menu::run() {
    std::unique_ptr<Graph> graph = nullptr;
    JohnsonAlgorithm johnson;
    int choice;


    do {
        displayMenu();

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Incorrect input! Try one more time" << std::endl;
            continue;
        }

        switch (choice) {
        case 1:
            createGraph(graph);
            break;
        case 2:
            addEdge(graph);
            break;
      
        case 3:
            displayGraph(graph);
            break;
        case 4:
            runSequentialJohnson(graph, johnson);
            break;
        case 5:
            runParallelJohnson(graph, johnson);
            break;
        case 6:
            comparePerformance(graph, johnson);
            break;
        case 7:
            createTestGraph(graph);
            break;
        case 0:
            std::cout << "Good bye!" << std::endl;
            break;
        default:
            std::cout << "Incorrect choice! Try one more time" << std::endl;
        }

  

    } while (choice != 0);
}

void Menu::createGraph(std::unique_ptr<Graph>& graph) {
    int vertices;
    std::cout << "Enter the amount of vertices: ";

    if (!(std::cin >> vertices) || vertices <= 0) {
        std::cout << "Incorrect edge number!" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    graph = std::make_unique<Graph>(vertices);
    std::cout << "Graph with " << vertices << " vertices succefully created!" << std::endl;
}

void Menu::addEdge(std::unique_ptr<Graph>& graph) {
    if (!graph) {
        std::cout << "You have to create a graph!" << std::endl;
        return;
    }

    int from, to;
    double weight;

    std::cout << "Enter the initial vertex (0-" << (graph->getVertices() - 1) << "): ";
    if (!(std::cin >> from)) {
        std::cout << "Incorrect input!" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << "Enter the final vertex (0-" << (graph->getVertices() - 1) << "): ";
    if (!(std::cin >> to)) {
        std::cout << "Incorrect input!" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << "Enter the edge weight: ";
    if (!(std::cin >> weight)) {
        std::cout << "Incorrect input!" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    graph->addEdge(from, to, weight);
    std::cout << "Edge (" << from << " -> " << to << ") with weight " << weight << " added!" << std::endl;
}


void Menu::displayGraph(const std::unique_ptr<Graph>& graph) {
    if (!graph) {
        std::cout << "The graph has not yet been created!" << std::endl;
        return;
    }

    std::cout << "\n========== GRAPH STRUCCTURE ==========" << std::endl;
    graph->printGraph();
    std::cout << "\nVertices number: " << graph->getVertices() << std::endl;
    std::cout << "Has negative weight: " << (graph->hasNegativeWeights() ? "Yes" : "No") << std::endl;
    std::cout << "====================================" << std::endl;
}

void Menu::runSequentialJohnson(const std::unique_ptr<Graph>& graph, JohnsonAlgorithm& johnson) {
    if (!graph) {
        std::cout << "The graph has not yet been created!" << std::endl;
        return;
    }

    std::cout << "\nRunning the Johnson's alogrithm" << std::endl;

    auto result = johnson.findAllPairsShortestPaths(*graph);

    std::cout << "\n========== Results (sequnential) ==========" << std::endl;
    johnson.printExecutionStats();
    std::cout << "===========================================" << std::endl;
}

void Menu::runParallelJohnson(const std::unique_ptr<Graph>& graph, JohnsonAlgorithm& johnson) {
    if (!graph) {
        std::cout << "The graph has not yet been created." << std::endl;
        return;
    }

    int numThreads;
    std::cout << "Enter threads amount (recomended:  " << std::thread::hardware_concurrency() << "): ";

    if (!(std::cin >> numThreads) || numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        std::cout << "It will be used the default threads amount: " << numThreads << std::endl;
    }

    std::cout << "\nRunning of multi-threaded Johnson's algorithm with " << numThreads << " threads..." << std::endl;

    auto result = johnson.findAllPairsShortestPathsParallel(*graph, numThreads);

    std::cout << "\n========== RESULT (MULTI-THREADED) ==========" << std::endl;

    johnson.printExecutionStats();
    std::cout << "=============================================" << std::endl;
}

void Menu::comparePerformance(const std::unique_ptr<Graph>& graph, JohnsonAlgorithm& johnson) {
    if (!graph) {
        std::cout << "The graph has not yet been created!" << std::endl;
        return;
    }

    std::cout << "\n Efficiency comparisson" << std::endl;
    std::cout << "It will take a while for huge grphs.\n" << std::endl;

    // Seq execution
    std::cout << "Running the sequential algorithm: " << std::endl;
    auto sequentialResult = johnson.findAllPairsShortestPaths(*graph);
    auto sequentialTime = johnson.getLastExecutionTime();

    // Multi-th execution
    int numThreads = std::thread::hardware_concurrency();
    std::cout << "Running the multi-threaded algorithm with " << numThreads << " threads..." << std::endl;
    auto parallelResult = johnson.findAllPairsShortestPathsParallel(*graph, numThreads);
    auto parallelTime = johnson.getLastExecutionTime();

    // Output of results
    std::cout << "\n========== EFFICIENCY COMPARISSON ==========" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Sequential time:     " << sequentialTime.count() << " us" << std::endl;
    std::cout << "Multi threaded time:  " << parallelTime.count() << " us" << std::endl;

    if (parallelTime.count() > 0) {
        double speedup = static_cast<double>(sequentialTime.count()) / parallelTime.count();
        std::cout << "Speed up:         " << speedup << "x" << std::endl;
        std::cout << "Efficiency:        " << (speedup / numThreads) * 100 << "%" << std::endl;
    }

    std::cout << "=============================================" << std::endl;
}

void Menu::createTestGraph(std::unique_ptr<Graph>& graph) {
    std::cout << "\nChoose type:" << std::endl;
    std::cout << "1. Small graph with negative edges" << std::endl;
    std::cout << "2. Middle graph" << std::endl;
    std::cout << "3. Huge graph for productivity test" << std::endl;
    std::cout << "Your choice: ";

    int choice;
    if (!(std::cin >> choice)) {
        std::cout << "Incorrect input!" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    switch (choice) {
    case 1:
        createSmallTestGraph(graph);
        break;
    case 2:
        createMediumTestGraph(graph);
        break;
    case 3:
        createLargeTestGraph(graph);
        break;
    default:
        std::cout << "Incorrect choice!" << std::endl;
    }
}

void Menu::createSmallTestGraph(std::unique_ptr<Graph>& graph) {
    graph = std::make_unique<Graph>(4);

    graph->addEdge(0, 1, 5);
    graph->addEdge(0, 2, 8);
    graph->addEdge(1, 2, -3);
    graph->addEdge(1, 3, 7);
    graph->addEdge(2, 3, 2);
    graph->addEdge(3, 0, -2);

    std::cout << "Created small test graph with negative edges (4 vertices)!" << std::endl;
}

void Menu::createMediumTestGraph(std::unique_ptr<Graph>& graph) {
    graph = std::make_unique<Graph>(8);

    // Creation of more complicated strucutre
    graph->addEdge(0, 1, 4);
    graph->addEdge(0, 7, 8);
    graph->addEdge(1, 2, 8);
    graph->addEdge(1, 7, 11);
    graph->addEdge(2, 3, 7);
    graph->addEdge(2, 8, 2);
    graph->addEdge(2, 5, 4);
    graph->addEdge(3, 4, 9);
    graph->addEdge(3, 5, 14);
    graph->addEdge(4, 5, 10);
    graph->addEdge(5, 6, 2);
    graph->addEdge(6, 7, 1);
    graph->addEdge(6, 8, 6);
    graph->addEdge(7, 8, 7);

    std::cout << "Created middle graph (8 edges)" << std::endl;
}

void Menu::createLargeTestGraph(std::unique_ptr<Graph>& graph) {
    //int vertices = 1000;
    //graph = std::make_unique<Graph>(vertices);

    //srand(time(nullptr));

    //for (int i = 0; i < vertices; ++i) {
    //    for (int j = 0; j < vertices; ++j) {
    //        if (i != j && rand() % 4 == 0) { // 25% ймовірність ребра
    //            double weight = (rand() % 20) - 5; // Ваги від -5 до 14
    //            graph->addEdge(i, j, weight);
    //        }
    //    }
    //}
    int V;
    std::cout << "Enter vertices amount for graph: ";
    std::cin >> V;
    if ( V <= 0) {
        std::cout << "Enter correct amount of vertices" << std::endl;
        return;
    }
    graph = std::make_unique<Graph>(V);

    int edges;
    int maxEdgesForGraph = V * (V - 1);

    std::cout << "Enter the amount of edges for the graph: ";
    std::cin >> edges;
    if(!edges || edges <=0){
        std::cout << "Enter the correct number of edges" << std::endl;
        return;
    }
    if (edges > maxEdgesForGraph) {
        std::cout << "The max amount of edges fot graph with" << V << " vertices is " << maxEdgesForGraph << std::endl;
        return;
    }

    srand(static_cast<unsigned>(time(nullptr)));
    int edgesAdded=0;
    while (edgesAdded < edges) {
        int from = rand() % V;
        int to = rand() % V;

        if (from != to) {
            bool edgeExists = false;
            for (const auto& edge : graph->getNeighbors(from)) {
                if (edge.to == to) {
                    edgeExists = true;
                    break;
                }
            }

            if (!edgeExists) {
                double weight = (rand() % 20) +1; 
                graph->addEdge(from, to, weight);
                edgesAdded++;
            }
        }
    }



    std::cout << "Created a huge test graph (" << V << " vertices) with "<< edges<<" edges for productivity testing!" << std::endl;
}

//void Menu::printDistanceMatrix(const std::vector<std::vector<double>>& distances) {
//    int size = distances.size();
//
//    if (size > 15) {
//        std::cout << "Graph is too huge for displaying as matrix adjacency." << std::endl;
//        std::cout << "Show elemnts 10x10:" << std::endl;
//        size = 10;
//    }
//
//    std::cout << std::fixed << std::setprecision(1);
//    std::cout << "\nMatrix of the shortest distances:" << std::endl;
//
//    //Headers
//    std::cout << "     ";
//    for (int j = 0; j < size; ++j) {
//        std::cout << std::setw(7) << j;
//    }
//    std::cout << std::endl;
//
//    // Rows
//    for (int i = 0; i < size; ++i) {
//        std::cout << std::setw(3) << i << ": ";
//        for (int j = 0; j < size; ++j) {
//            if (distances[i][j] == std::numeric_limits<double>::infinity()) {
//                std::cout << std::setw(7) << "infinity";
//            }
//            else {
//                std::cout << std::setw(7) << distances[i][j];
//            }
//        }
//        std::cout << std::endl;
//    }
//}

