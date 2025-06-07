#include "ShortestPathStr.h"
#include "FibonacciHeap.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <barrier>

//realis of seq algorithm of BF
ShortestPathResult BellmanFordStrategy::findShortestPaths(const Graph& graph, int source) {
    int V = graph.getVertices();
    ShortestPathResult result(V);

    result.distances[source] = 0.0;

    
    for (int i = 0; i < V - 1; ++i) {
        for (int u = 0; u < V; ++u) {
            if (result.distances[u] != std::numeric_limits<double>::infinity()) {
                for (const auto& edge : graph.getNeighbors(u)) {
                    int v = edge.to;
                    double weight = edge.weight;

                    if (result.distances[u] + weight < result.distances[v]) {
                        result.distances[v] = result.distances[u] + weight;
                        result.predecessors[v] = u;
                    }
                }
            }
        }
    }

    // Check for negative cycles
    for (int u = 0; u < V; ++u) {
        if (result.distances[u] != std::numeric_limits<double>::infinity()) {
            for (const auto& edge : graph.getNeighbors(u)) {
                int v = edge.to;
                double weight = edge.weight;

                if (result.distances[u] + weight < result.distances[v]) {
                    result.hasNegativeCycle = true;
                    return result;
                }
            }
        }
    }

    return result;
}

// Realis of parallel alg of BF
ShortestPathResult ParallelBellmanFordStrategy::findShortestPaths(const Graph& graph, int source) {
    int V = graph.getVertices();
    ShortestPathResult result(V);

    result.distances[source] = 0.0;

    std::mutex distMutex;
    std::atomic<bool> updated{ false };

    
    for (int i = 0; i < V - 1; ++i) {
        updated = false;

        auto processVertices = [&](int start, int end) {
            bool localUpdated = false;
            std::vector<std::pair<int, double>> localUpdates;

            for (int u = start; u < end; ++u) {
                if (result.distances[u] != std::numeric_limits<double>::infinity()) {
                    for (const auto& edge : graph.getNeighbors(u)) {
                        int v = edge.to;
                        double weight = edge.weight;
                        double newDist = result.distances[u] + weight;

                        if (newDist < result.distances[v]) {
                            localUpdates.emplace_back(v, newDist);
                            localUpdated = true;
                        }
                    }
                }
            }

            //Applying updates with mutex
            if (localUpdated) {
                std::lock_guard<std::mutex> lock(distMutex);
                for (const auto& update : localUpdates) {
                    int v = update.first;
                    double newDist = update.second;
                    if (newDist < result.distances[v]) {
                        result.distances[v] = newDist;
                        updated = true;
                    }
                }
            }
            };

        graph.parallelProcessVertices(numThreads, processVertices);

        if (!updated) break; // Prev break
    }

    //Parallel check for negative cycles
    std::atomic<bool> hasNegativeCycle{ false };

    auto checkNegativeCycle = [&](int start, int end) {
        for (int u = start; u < end; ++u) {
            if (result.distances[u] != std::numeric_limits<double>::infinity()) {
                for (const auto& edge : graph.getNeighbors(u)) {
                    int v = edge.to;
                    double weight = edge.weight;

                    if (result.distances[u] + weight < result.distances[v]) {
                        hasNegativeCycle = true;
                        return;
                    }
                }
            }
        }
        };

    graph.parallelProcessVertices(numThreads, checkNegativeCycle);
    result.hasNegativeCycle = hasNegativeCycle;

    return result;
}

// Realis of seq alg of Dej
ShortestPathResult DijkstraStrategy::findShortestPaths(const Graph& graph, int source) {
    int V = graph.getVertices();
    ShortestPathResult result(V);

    FibonacciHeap<double> heap;
    std::vector<bool> visited(V, false);

    result.distances[source] = 0.0;
    heap.insert(0.0, source);

    while (!heap.empty()) {
        auto [dist, u] = heap.extractMin();

        if (visited[u]) continue;
        visited[u] = true;

        for (const auto& edge : graph.getNeighbors(u)) {
            int v = edge.to;
            double weight = edge.weight;
            double newDist = result.distances[u] + weight;

            if (newDist < result.distances[v]) {
                result.distances[v] = newDist;
                result.predecessors[v] = u;

                if (!visited[v]) {
                    heap.insert(newDist, v);
                }
            }
        }
    }

    return result;
}

// Realis of parallel alg of Dej
ShortestPathResult ParallelDijkstraStrategy::findShortestPaths(const Graph& graph, int source) {
    int V = graph.getVertices();
    ShortestPathResult result(V);

    FibonacciHeap<double> heap;
    std::vector<std::atomic<bool>> visited(V);
    std::mutex heapMutex;

    for (int i = 0; i < V; ++i) {
        visited[i] = false;
    }

    result.distances[source] = 0.0;
    heap.insertSafe(0.0, source);

    while (!heap.empty()) {
        auto [dist, u] = heap.extractMinSafe();

        if (visited[u].exchange(true)) continue;

        //Parallel neighbour proccessing
        std::vector<std::thread> threads;
        const auto& neighbors = graph.getNeighbors(u);
        int neighborsPerThread = std::max(1, static_cast<int>(neighbors.size()) / numThreads);

        auto processNeighbors = [&](int start, int end) {
            std::vector<std::pair<double, int>> localInserts;

            for (int i = start; i < end && i < neighbors.size(); ++i) {
                const auto& edge = neighbors[i];
                int v = edge.to;
                double weight = edge.weight;
                double newDist = result.distances[u] + weight;

                if (newDist < result.distances[v]) {
                    result.distances[v] = newDist;
                    result.predecessors[v] = u;

                    if (!visited[v]) {
                        localInserts.emplace_back(newDist, v);
                    }
                }
            }

            
            for (const auto& insert : localInserts) {
                heap.insertSafe(insert.first, insert.second);
            }
            };

        if (neighbors.size() >= numThreads) {
            int start = 0;
            for (int t = 0; t < numThreads; ++t) {
                int end = start + neighborsPerThread;
                if (t == numThreads - 1) end = neighbors.size();

                threads.emplace_back(processNeighbors, start, end);
                start = end;
            }

            for (auto& thread : threads) {
                thread.join();
            }
        }
        else {
            processNeighbors(0, neighbors.size());
        }
    }

    return result;
}