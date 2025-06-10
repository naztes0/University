#include <iostream>
#include "Graph.h"
#include "Johnson.h"
#include "ParallelJohnson.h"
#include <chrono>
#include <random>
#include <iomanip>
#include <set>
using namespace std;
using namespace chrono;

void printDistanceMatrix(const vector<vector<double>>& distances, int maxDisplay = 10) {
    int size = min(maxDisplay, (int)distances.size());
    cout << "\nDistance Matrix (showing " << size << "x" << size << " subset):" << endl;
    cout << setw(8) << " ";
    for (int j = 0; j < size; ++j) {
        cout << setw(8) << j;
    }
    cout << endl;

    for (int i = 0; i < size; ++i) {
        cout << setw(8) << i;
        for (int j = 0; j < size; ++j) {
            if (distances[i][j] == numeric_limits<double>::infinity()) {
                cout << setw(8) << "INF";
            }
            else {
                cout << setw(8) << fixed << setprecision(1) << distances[i][j];
            }
        }
        cout << endl;
    }
}

Graph generateRandomGraph(int vertices, int edges, double minWeight = -5.0, double maxWeight = 15.0) {
    Graph graph(vertices);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> vertexDist(0, vertices - 1);
    uniform_real_distribution<> weightDist(minWeight, maxWeight);

    set<pair<int, int>> addedEdges; // Prevent duplicate edges

    for (int i = 0; i < edges; ++i) {
        int attempts = 0;
        int from, to;

        do {
            from = vertexDist(gen);
            to = vertexDist(gen);
            attempts++;
        } while ((from == to || addedEdges.count({ from, to }) > 0) && attempts < 100);

        if (attempts < 100) {
            double weight = weightDist(gen);
            graph.addEdge(from, to, weight);
            addedEdges.insert({ from, to });
        }
    }

    return graph;
}

void testSmallExample() {
    cout << "=== Testing Small Example ===" << endl;

    // Create a small test graph
    Graph graph(4);
    graph.addEdge(0, 1, 5.0);
    graph.addEdge(0, 2, 3.0);
    graph.addEdge(1, 2, 2.0);
    graph.addEdge(1, 3, 6.0);
    graph.addEdge(2, 3, 7.0);
    graph.addEdge(2, 1, -2.0);  // Negative edge

    graph.printGraph();

    // Test sequential version
    auto start = high_resolution_clock::now();
    auto seqResult = Johnson::run(graph);
    auto end = high_resolution_clock::now();
    auto seqTime = duration_cast<microseconds>(end - start);

    if (seqResult.success) {
        cout << "\nSequential Johnson's Algorithm - Time: " << seqTime.count() << " microseconds" << endl;
        printDistanceMatrix(seqResult.distances);
    }

    // Test parallel version
    start = high_resolution_clock::now();
    auto parResult = ParallelJohnson::run(graph, 2);
    end = high_resolution_clock::now();
    auto parTime = duration_cast<microseconds>(end - start);

    if (parResult.success) {
        cout << "\nParallel Johnson's Algorithm - Time: " << parTime.count() << " microseconds" << endl;

        // Verify results match
        bool resultsMatch = true;
        for (int i = 0; i < 4 && resultsMatch; ++i) {
            for (int j = 0; j < 4 && resultsMatch; ++j) {
                if (abs(seqResult.distances[i][j] - parResult.distances[i][j]) > 1e-9) {
                    resultsMatch = false;
                }
            }
        }
        cout << "Results match: " << (resultsMatch ? "YES" : "NO") << endl;
    }
}

void benchmarkPerformance() {
    cout << "\n=== Performance Benchmark ===" << endl;

    // Test with larger graphs where parallelization shows benefits
    vector<int> graphSizes = { 200, 500, 1000, 2000 };

    for (int size : graphSizes) {
        cout << "\n" << string(50, '=') << endl;
        cout << "Testing graph with " << size << " vertices..." << endl;

        // Generate denser graph for better parallelization benefits
        int edges = min(size * size / 8, size * 100);  // More edges for denser graphs
        Graph graph = generateRandomGraph(size, edges, -5.0, 15.0);

        cout << "Graph generated with " << edges << " edges (density: "
            << fixed << setprecision(2) << (double)edges / (size * size) * 100 << "%)" << endl;

        // Sequential version
        cout << "Running sequential version..." << endl;
        auto start = high_resolution_clock::now();
        auto seqResult = Johnson::run(graph);
        auto end = high_resolution_clock::now();
        auto seqTime = duration_cast<milliseconds>(end - start);

        if (!seqResult.success) {
            cout << "Sequential version failed!" << endl;
            continue;
        }

        cout << "Sequential time: " << seqTime.count() << " ms" << endl;

        // Parallel version with optimal thread count
        int optimalThreads = min((int)thread::hardware_concurrency(), max(2, size / 50));
        cout << "Running parallel version with " << optimalThreads << " threads..." << endl;

        start = high_resolution_clock::now();
        auto parResult = ParallelJohnson::run(graph, optimalThreads);
        end = high_resolution_clock::now();
        auto parTime = duration_cast<milliseconds>(end - start);

        if (parResult.success) {
            cout << "Parallel time: " << parTime.count() << " ms" << endl;

            if (seqTime.count() > 0 && parTime.count() > 0) {
                double speedup = (double)seqTime.count() / parTime.count();
                cout << "Speedup: " << fixed << setprecision(2) << speedup << "x";

                if (speedup > 1.1) {
                    cout << "  (Parallel is " << setprecision(1) << (speedup - 1) * 100 << "% faster)";
                }
                else if (speedup > 0.9) {
                    cout << "  (Similar performance)";
                }
                else {
                    cout << " ?? (Sequential is faster)";
                }
                cout << endl;

                // Verify results match for correctness (sample check)
                bool resultsMatch = true;
                int sampleSize = min(20, size);
                for (int i = 0; i < sampleSize && resultsMatch; ++i) {
                    for (int j = 0; j < sampleSize && resultsMatch; ++j) {
                        if (abs(seqResult.distances[i][j] - parResult.distances[i][j]) > 1e-9) {
                            resultsMatch = false;
                        }
                    }
                }
                cout << "Results match (sample): " << (resultsMatch ? "YES" : " NO") << endl;

                // Memory usage estimation
                double memoryMB = (size * size * sizeof(double)) / (1024.0 * 1024.0);
                cout << "Memory usage: ~" << fixed << setprecision(1) << memoryMB << " MB" << endl;
            }
        }
        else {
            cout << "Parallel version failed!" << endl;
        }
    }
}

void warmupTest() {
    cout << "=== Warming up CPU ===" << endl;

    // Small warmup to get CPU to higher frequencies
    Graph warmupGraph = generateRandomGraph(100, 500);
    auto start = high_resolution_clock::now();
    auto result = Johnson::run(warmupGraph);
    auto end = high_resolution_clock::now();
    auto time = duration_cast<milliseconds>(end - start);

    cout << "Warmup completed in " << time.count() << " ms" << endl;
}

int main() {
    cout << "Johnson's Algorithm Implementation - Optimized" << endl;
    cout << "===============================================" << endl;
    cout << "Hardware concurrency: " << thread::hardware_concurrency() << " threads" << endl;
    cout << "Compiler optimizations: " <<
#ifdef NDEBUG
        "RELEASE MODE"
#else
        "DEBUG MODE (consider -O2 or -O3 for better performance)"
#endif
        << endl << endl;

    warmupTest();
    testSmallExample();
    benchmarkPerformance();


    return 0;
}