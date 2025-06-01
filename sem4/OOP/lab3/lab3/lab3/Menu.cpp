#include "Menu.h"
#include <iostream>
#include <limits>
#include <chrono>
#include <iomanip>
#include <random>
#include <ctime>

Menu::Menu() : graph(nullptr), defaultThreads(4) {}

void Menu::run() {
    std::cout << "=== Algorithms for Finding Shortest Paths ===" << std::endl;
    std::cout << "Johnson's algorithm implementation with Bellman-Ford and Dijkstra" << std::endl;
    std::cout << "Using Fibonacci Heap for Dijkstra's algorithm" << std::endl;

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
            if (graph) runBellmanFordComparison();
            else std::cout << "Please create a graph first!" << std::endl;
            break;
        case 5:
            if (graph) runDijkstraComparison();
            else std::cout << "Please create a graph first!" << std::endl;
            break;
        case 6:
            if (graph) runJohnsonComparison();
            else std::cout << "Please create a graph first!" << std::endl;
            break;
        case 7: {
            int threads = getIntInput("Enter number of threads for parallel algorithms: ");
            if (threads > 0) {
                defaultThreads = threads;
                std::cout << "Default thread count set to: " << defaultThreads << std::endl;
            }
            else {
                std::cout << "Thread count must be positive!" << std::endl;
            }
            break;
        }
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
    std::cout << "\n=================== MAIN MENU ===================" << std::endl;
    std::cout << "1. Create graph" << std::endl;
    std::cout << "2. Add edges" << std::endl;
    std::cout << "3. Show graph" << std::endl;
    std::cout << "4. Run Bellman-Ford comparison (sequential vs parallel)" << std::endl;
    std::cout << "5. Run Dijkstra comparison (sequential vs parallel)" << std::endl;
    std::cout << "6. Run Johnson's algorithm comparison (sequential vs parallel)" << std::endl;
    std::cout << "7. Set thread count (current: " << defaultThreads << ")" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "=================================================" << std::endl;
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
   
    std::cout << "\n=================== ADD EDGES ===================" << std::endl;
    std::cout << "1. Manualy" << std::endl;
    std::cout << "2. Randomly" << std::endl;
    
    int choice = getIntInput("Enter you choice:");

    switch (choice) {
        case 1: {

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
            break;
        }
         case 2: {
            int edges = getIntInput("Enter edges number: ");
            if (graph) addSampleEdges(*graph, graph->getVertices(), edges);
            
         }
         default: {
             std::cout << "Invalid choice!";
         }
                std::cout<<std::endl;
    }
   
}

void Menu::runBellmanFordComparison() {
    int source = getSourceVertex();
    if (source == -1) return;

    std::cout << "\n========== BELLMAN-FORD ALGORITHM COMPARISON ==========" << std::endl;
    std::cout << "Running sequential and parallel versions..." << std::endl;

    // Sequential Bellman-Ford
    BellmanFordStrategy seqStrategy;
    auto start = std::chrono::high_resolution_clock::now();
    ShortestPathResult seqResult = seqStrategy.findShortestPaths(*graph, source);
    auto end = std::chrono::high_resolution_clock::now();
    auto seqTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Parallel Bellman-Ford
    ParallelBellmanFordStrategy parStrategy(defaultThreads);
    start = std::chrono::high_resolution_clock::now();
    ShortestPathResult parResult = parStrategy.findShortestPaths(*graph, source);
    end = std::chrono::high_resolution_clock::now();
    auto parTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Display results
    displaySingleSourceResults(seqResult, "Sequential Bellman-Ford", seqTime / 1000.0);
    displaySingleSourceResults(parResult, "Parallel Bellman-Ford", parTime / 1000.0);

    std::cout << "\n--- PERFORMANCE COMPARISON ---" << std::endl;
    std::cout << "Sequential time: " << seqTime << " μs" << std::endl;
    std::cout << "Parallel time:   " << parTime << " μs" << std::endl;
    if (parTime > 0) {
        double speedup = static_cast<double>(seqTime) / parTime;
        std::cout << "Speedup:         " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
    }
    std::cout << "======================================================" << std::endl;
}

void Menu::runDijkstraComparison() {
    if (graph->hasNegativeWeights()) {
        std::cout << "Warning: Graph contains negative weights! Dijkstra may not work correctly." << std::endl;
        std::cout << "Continue anyway? (y/n): ";
        char choice;
        std::cin >> choice;
        if (choice != 'y' && choice != 'Y') return;
    }

    int source = getSourceVertex();
    if (source == -1) return;

    std::cout << "\n============ DIJKSTRA ALGORITHM COMPARISON ============" << std::endl;
    std::cout << "Running sequential and parallel versions..." << std::endl;

    // Sequential Dijkstra
    DijkstraStrategy seqStrategy;
    auto start = std::chrono::high_resolution_clock::now();
    ShortestPathResult seqResult = seqStrategy.findShortestPaths(*graph, source);
    auto end = std::chrono::high_resolution_clock::now();
    auto seqTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Parallel Dijkstra
    ParallelDijkstraStrategy parStrategy(defaultThreads);
    start = std::chrono::high_resolution_clock::now();
    ShortestPathResult parResult = parStrategy.findShortestPaths(*graph, source);
    end = std::chrono::high_resolution_clock::now();
    auto parTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Display results
    displaySingleSourceResults(seqResult, "Sequential Dijkstra", seqTime / 1000.0);
    displaySingleSourceResults(parResult, "Parallel Dijkstra", parTime / 1000.0);

    std::cout << "\n--- PERFORMANCE COMPARISON ---" << std::endl;
    std::cout << "Sequential time: " << seqTime << " μs" << std::endl;
    std::cout << "Parallel time:   " << parTime << " μs" << std::endl;
    if (parTime > 0) {
        double speedup = static_cast<double>(seqTime) / parTime;
        std::cout << "Speedup:         " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
    }
    std::cout << "======================================================" << std::endl;
}

void Menu::runJohnsonComparison() {
    std::cout << "\n=========== JOHNSON'S ALGORITHM COMPARISON ===========" << std::endl;
    std::cout << "Finding all-pairs shortest paths..." << std::endl;
    std::cout << "Running sequential and parallel versions..." << std::endl;

    // Sequential Johnson's Algorithm
    auto seqJohnson = JohnsonAlgorithm::createSequential();
    std::cout << "first";
    JohnsonResult seqResult = seqJohnson->findAllPairsShortestPaths(*graph);
    std::cout << "first";
    // Parallel Johnson's Algorithm
    auto parJohnson = JohnsonAlgorithm::createParallel(defaultThreads);
    std::cout << "first";
    JohnsonResult parResult = parJohnson->findAllPairsShortestPaths(*graph);
    std::cout << "first";

    // Display results
    displayJohnsonResults(seqResult, "Sequential Johnson's Algorithm");
    displayJohnsonResults(parResult, "Parallel Johnson's Algorithm");

    std::cout << "\n--- PERFORMANCE COMPARISON ---" << std::endl;
    std::cout << "Sequential time: " << seqResult.executionTime.count() << " ms" << std::endl;
    std::cout << "Parallel time:   " << parResult.executionTime.count() << " ms" << std::endl;
    if (parResult.executionTime.count() > 0) {
        double speedup = static_cast<double>(seqResult.executionTime.count()) / parResult.executionTime.count();
        std::cout << "Speedup:         " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
    }
    std::cout << "======================================================" << std::endl;
}

void Menu::displaySingleSourceResults(const ShortestPathResult& result, const std::string& algorithmName, double executionTime) {
    std::cout << "\n--- " << algorithmName << " Results ---" << std::endl;

    if (result.hasNegativeCycle) {
        std::cout << "NEGATIVE CYCLE DETECTED!" << std::endl;
        return;
    }

    std::cout << "Execution time: " << std::fixed << std::setprecision(3) << executionTime << " ms" << std::endl;
    std::cout << "Vertex\tDistance\tPredecessor" << std::endl;
    std::cout << "--------------------------------" << std::endl;

    for (int i = 0; i < result.distances.size(); ++i) {
        std::cout << i << "\t";

        if (result.distances[i] == std::numeric_limits<double>::infinity()) {
            std::cout << "inf\t\t";
        }
        else {
            std::cout << std::fixed << std::setprecision(2) << result.distances[i] << "\t\t";
        }

        if (result.predecessors[i] == -1) {
            std::cout << "-" << std::endl;
        }
        else {
            std::cout << result.predecessors[i] << std::endl;
        }
    }
}

void Menu::displayJohnsonResults(const JohnsonResult& result, const std::string& algorithmName) {
    std::cout << "\n--- " << algorithmName << " Results ---" << std::endl;

    if (result.hasNegativeCycle) {
        std::cout << "NEGATIVE CYCLE DETECTED!" << std::endl;
        return;
    }

    std::cout << "Execution time: " << result.executionTime.count() << " ms" << std::endl;

    int V = result.distances.size();
    if (V <= 10) { // Показуємо повну матрицю тільки для малих графів
        std::cout << "\nAll-pairs shortest distances matrix:" << std::endl;
        std::cout << "From\\To\t";
        for (int j = 0; j < V; ++j) {
            std::cout << j << "\t";
        }
        std::cout << std::endl;

        for (int i = 0; i < V; ++i) {
            std::cout << i << "\t";
            for (int j = 0; j < V; ++j) {
                if (result.distances[i][j] == std::numeric_limits<double>::infinity()) {
                    std::cout << "inf\t";
                }
                else {
                    std::cout << std::fixed << std::setprecision(1) << result.distances[i][j] << "\t";
                }
            }
            std::cout << std::endl;
        }
    }
    else {
        std::cout << "Graph is too large to display full matrix (vertices: " << V << ")" << std::endl;
        std::cout << "Sample distances from vertex 0:" << std::endl;
        for (int j = 0; j < std::min(10, V); ++j) {
            std::cout << "0 -> " << j << ": ";
            if (result.distances[0][j] == std::numeric_limits<double>::infinity()) {
                std::cout << "inf" << std::endl;
            }
            else {
                std::cout << std::fixed << std::setprecision(2) << result.distances[0][j] << std::endl;
            }
        }
    }
}

void Menu::printGraph() {
    std::cout << "\nGraph structure:" << std::endl;
    graph->printGraph();
}

int Menu::getSourceVertex() {
    int source = getIntInput("Enter source vertex: ");

    if (source < 0 || source >= graph->getVertices()) {
        std::cout << "Invalid vertex index!" << std::endl;
        return -1;
    }

    return source;
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

void Menu::addSampleEdges(Graph& graph, int vertexCount, int edgeCount ) {
    std::mt19937 rng(static_cast<unsigned int>(time(nullptr))); // генератор випадкових чисел
    std::uniform_int_distribution<int> distV(0, vertexCount - 1); // для вершин
    std::uniform_real_distribution<double> distW(1.0, 10.0);      // для ваг

    int added = 0;
    while (added < edgeCount) {
        int from = distV(rng);
        int to = distV(rng);
        if (from == to) continue; // не дозволяємо петлі

        double weight = distW(rng);
        graph.addEdge(from, to, weight);
        ++added;
    }

    std::cout << "Graph with " << vertexCount << " vertices and " << edgeCount << " edges created.\n";

}