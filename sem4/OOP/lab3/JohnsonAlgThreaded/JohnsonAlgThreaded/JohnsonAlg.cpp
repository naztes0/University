#include "JohnsonAlgorithm.h"
#include <thread>
#include <mutex>
#include <algorithm>

JohnsonAlgorithm::JohnsonAlgorithm(std::unique_ptr<ShortestPathStrategy> bfStrategy,
    std::unique_ptr<ShortestPathStrategy> dijkstraStrat)
    : bellmanFordStrategy(std::move(bfStrategy)), dijkstraStrategy(std::move(dijkstraStrat)) {}

JohnsonResult JohnsonAlgorithm::findAllPairsShortestPaths(const Graph& graph) {
    auto start = std::chrono::high_resolution_clock::now();

    JohnsonResult result = executeAlgorithm(graph);

    auto end = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    return result;
}

JohnsonResult JohnsonAlgorithm::executeAlgorithm(const Graph& graph) {
    int V = graph.getVertices();
    JohnsonResult result(V);

    // Крок 1: Створюємо доповнений граф з додатковою вершиною
    Graph augmentedGraph = createAugmentedGraph(graph);

    // Крок 2: Запускаємо Беллмана-Форда з додаткової вершини
    std::vector<double> potentials = computePotentials(augmentedGraph);

    // Перевіряємо на від'ємні цикли
    if (potentials.empty()) {
        result.hasNegativeCycle = true;
        return result;
    }

    // Крок 3: Перезважуємо граф
    Graph reweightedGraph = reweightGraph(graph, potentials);

    // Крок 4: Запускаємо Дейкстру для кожної вершини
    result.distances = computeAllPairsShortestPaths(reweightedGraph, potentials);

    return result;
}

Graph JohnsonAlgorithm::createAugmentedGraph(const Graph& original) {
    int V = original.getVertices();
    Graph augmented(V + 1);

    // Копіюємо всі ребра з оригінального графа
    for (int u = 0; u < V; ++u) {
        for (const auto& edge : original.getNeighbors(u)) {
            augmented.addEdge(u, edge.to, edge.weight);
        }
    }

    // Додаємо ребра з нової вершини (V) до всіх інших з вагою 0
    for (int v = 0; v < V; ++v) {
        augmented.addEdge(V, v, 0.0);
    }

    return augmented;
}

std::vector<double> JohnsonAlgorithm::computePotentials(const Graph& augmentedGraph) {
    int V = augmentedGraph.getVertices();
    ShortestPathResult bfResult = bellmanFordStrategy->findShortestPaths(augmentedGraph, V - 1);

    if (bfResult.hasNegativeCycle) {
        return {}; // Порожній вектор означає наявність від'ємного циклу
    }

    // Повертаємо тільки потенціали для оригінальних вершин
    std::vector<double> potentials(V - 1);
    for (int i = 0; i < V - 1; ++i) {
        potentials[i] = bfResult.distances[i];
    }

    return potentials;
}

Graph JohnsonAlgorithm::reweightGraph(const Graph& original, const std::vector<double>& potentials) {
    int V = original.getVertices();
    Graph reweighted(V);

    for (int u = 0; u < V; ++u) {
        for (const auto& edge : original.getNeighbors(u)) {
            int v = edge.to;
            double newWeight = edge.weight + potentials[u] - potentials[v];
            reweighted.addEdge(u, v, newWeight);
        }
    }

    return reweighted;
}

std::vector<std::vector<double>> JohnsonAlgorithm::computeAllPairsShortestPaths(
    const Graph& reweightedGraph, const std::vector<double>& potentials) {

    int V = reweightedGraph.getVertices();
    std::vector<std::vector<double>> distances(V, std::vector<double>(V, std::numeric_limits<double>::infinity()));

    // Ініціалізуємо діагональ
    for (int i = 0; i < V; ++i) {
        distances[i][i] = 0.0;
    }

    // Запускаємо Дейкстру для кожної вершини
    for (int source = 0; source < V; ++source) {
        ShortestPathResult dijkstraResult = dijkstraStrategy->findShortestPaths(reweightedGraph, source);

        for (int target = 0; target < V; ++target) {
            if (dijkstraResult.distances[target] != std::numeric_limits<double>::infinity()) {
                // Повертаємо оригінальну вагу
                distances[source][target] = dijkstraResult.distances[target] - potentials[source] + potentials[target];
            }
        }
    }

    return distances;
}

// Factory Methods
std::unique_ptr<JohnsonAlgorithm> JohnsonAlgorithm::createSequential() {
    return std::make_unique<JohnsonAlgorithm>(
        std::make_unique<BellmanFordStrategy>(),
        std::make_unique<DijkstraStrategy>()
    );
}

std::unique_ptr<JohnsonAlgorithm> JohnsonAlgorithm::createParallel(int numThreads) {
    return std::make_unique<ParallelJohnsonAlgorithm>(
        std::make_unique<ParallelBellmanFordStrategy>(numThreads),
        std::make_unique<ParallelDijkstraStrategy>(numThreads),
        numThreads
    );
}

// Паралельна реалізація
ParallelJohnsonAlgorithm::ParallelJohnsonAlgorithm(
    std::unique_ptr<ShortestPathStrategy> bfStrategy,
    std::unique_ptr<ShortestPathStrategy> dijkstraStrat,
    int threads)
    : JohnsonAlgorithm(std::move(bfStrategy), std::move(dijkstraStrat)), numThreads(threads) {}

std::vector<std::vector<double>> ParallelJohnsonAlgorithm::computeAllPairsShortestPaths(
    const Graph& reweightedGraph, const std::vector<double>& potentials) {

    int V = reweightedGraph.getVertices();
    std::vector<std::vector<double>> distances(V, std::vector<double>(V, std::numeric_limits<double>::infinity()));

    // Ініціалізуємо діагональ
    for (int i = 0; i < V; ++i) {
        distances[i][i] = 0.0;
    }

    std::mutex distancesMutex;

    auto processVertices = [&](int start, int end) {
        for (int source = start; source < end; ++source) {
            ShortestPathResult dijkstraResult = dijkstraStrategy->findShortestPaths(reweightedGraph, source);

            std::lock_guard<std::mutex> lock(distancesMutex);
            for (int target = 0; target < V; ++target) {
                if (dijkstraResult.distances[target] != std::numeric_limits<double>::infinity()) {
                    distances[source][target] = dijkstraResult.distances[target] - potentials[source] + potentials[target];
                }
            }
        }
        };

    reweightedGraph.parallelProcessVertices(numThreads, processVertices);

    return distances;
}