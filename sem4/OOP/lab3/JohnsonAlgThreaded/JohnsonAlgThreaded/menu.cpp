#include "Menu.h"
#include <iostream>
#include <limits>
#include <chrono>

Menu::Menu() : graph(nullptr), strategy(nullptr) {}

void Menu::run() {
    std::cout << "=== Johnson algorithm for finding the shortest paths ===" << std::endl;

    while (true) {
        displayMainMenu();
        int choice = getIntInput("Your choice: ");

        switch (choice) {
        case 1:
            createGraph();
            break;
        case 2:
            if (graph) addEdges();
            else std::cout << "Please create a graph first!" << std::endl;
            break;
        case 3:
            if (graph) printGraph();
            else std::cout << "Graph not created!" << std::endl;
            break;
        case 4:
            selectAlgorithm();
            break;
        case 5:
            if (graph && strategy) runAlgorithm();
            else std::cout << "Create a graph and select an algorithm first!" << std::endl;
            break;
        case 0:
            std::cout << "Goodbye!" << std::endl;
            return;
        default:
            std::cout << "Invalid choice!" << std::endl;
        }
        std::cout << std::endl;
    }
}

void Menu::displayMainMenu() {
    std::cout << "\n=============== MAIN MENU ===============" << std::endl;
    std::cout << "1. Create graph" << std::endl;
    std::cout << "2. Add edges" << std::endl;
    std::cout << "3. Show graph" << std::endl;
    std::cout << "4. Select algorithm" << std::endl;
    std::cout << "5. Run algorithm" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "=========================================" << std::endl;
}

void Menu::createGraph() {
    int vertices = getIntInput("Enter number of vertices: ");
    if (vertices > 0) {
        graph = std::make_unique<Graph>(vertices);
        std::cout << "Graph with " << vertices << " vertices created!" << std::endl;
    }
    else {
        std::cout << "Number of vertices must be positive!" << std::endl;
    }
}

void Menu::addEdges() {
    std::cout << "Adding edges (enter -1 -1 to stop)" << std::endl;

    while (true) {
        std::cout << "\nEnter edge (from, to, weight):" << std::endl;
        int from = getIntInput("From vertex: ");

        if (from == -1) break;

        int to = getIntInput("To vertex: ");
        if (to == -1) break;

        double weight = getDoubleInput("Weight: ");

        if (from >= 0 && from < graph->getVertices() &&
            to >= 0 && to < graph->getVertices()) {
            graph->addEdge(from, to, weight);
            std::cout << "Edge added: " << from << " -> " << to << " (weight: " << weight << ")" << std::endl;
        }
        else {
            std::cout << "Invalid vertex indices!" << std::endl;
        }
    }
}

void Menu::selectAlgorithm() {
    std::cout << "\nSelect an algorithm:" << std::endl;
    std::cout << "1. Bellman-Ford (sequential)" << std::endl;
    std::cout << "2. Bellman-Ford (parallel)" << std::endl;
    std::cout << "3. Dijkstra (sequential)" << std::endl;
    std::cout << "4. Dijkstra (parallel)" << std::endl;

    int choice = getIntInput("Your choice: ");

    switch (choice) {
    case 1:
        strategy = std::make_unique<BellmanFordStrategy>();
        std::cout << "Selected: Bellman-Ford (sequential)" << std::endl;
        break;
    case 2: {
        int threads = getIntInput("Number of threads: ");
        strategy = std::make_unique<ParallelBellmanFordStrategy>(threads);
        std::cout << "Selected: Bellman-Ford (parallel, " << threads << " threads)" << std::endl;
        break;
    }
    case 3:
        if (graph && graph->hasNegativeWeights()) {
            std::cout << "Warning: the graph contains negative weights. Dijkstra may not work correctly!" << std::endl;
        }
        strategy = std::make_unique<DijkstraStrategy>();
        std::cout << "Selected: Dijkstra (sequential)" << std::endl;
        break;
    case 4: {
        if (graph && graph->hasNegativeWeights()) {
            std::cout << "Warning: the graph contains negative weights. Dijkstra may not work correctly!" << std::endl;
        }
        int threads = getIntInput("Number of threads: ");
        strategy = std::make_unique<ParallelDijkstraStrategy>(threads);
        std::cout << "Selected: Dijkstra (parallel, " << threads << " threads)" << std::endl;
        break;
    }
    default:
        std::cout << "Invalid choice!" << std::endl;
    }
}

void Menu::runAlgorithm() {
    int source = getIntInput("Enter source vertex: ");

    if (source < 0 || source >= graph->getVertices()) {
        std::cout << "Invalid vertex index!" << std::endl;
        return;
    }

    std::cout << "\nRunning algorithm..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    ShortestPathResult result = strategy->findShortestPaths(*graph, source);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    displayResults(result);
    std::cout << "\nExecution time: " << duration.count() << " microseconds" << std::endl;
}

void Menu::displayResults(const ShortestPathResult& result) {
    if (result.hasNegativeCycle) {
        std::cout << "\nNegative cycle detected!" << std::endl;
        return;
    }

    std::cout << "\nResults:" << std::endl;
    std::cout << "Vertex\tDistance\tPredecessor" << std::endl;
    std::cout << "--------------------------------" << std::endl;

    for (int i = 0; i < result.distances.size(); ++i) {
        std::cout << i << "\t";

        if (result.distances[i] == std::numeric_limits<double>::infinity()) {
            std::cout << "∞\t\t";
        }
        else {
            std::cout << result.distances[i] << "\t\t";
        }

        if (result.predecessors[i] == -1) {
            std::cout << "-" << std::endl;
        }
        else {
            std::cout << result.predecessors[i] << std::endl;
        }
    }
}

void Menu::printGraph() {
    std::cout << "\nGraph structure:" << std::endl;
    graph->printGraph();
}

int Menu::getIntInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        else {
            std::cout << "Invalid input! Please try again." << std::endl;
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
            std::cout << "Invalid input! Please try again." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}
