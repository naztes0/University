#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>

class Graph;

class ParallelJohnson {
public:
    struct Result {
        std::vector<std::vector<double>> distances;
        bool success;

        Result(int vertices) : distances(vertices, std::vector<double>(vertices)), success(false) {}
    };

    static Result run(const Graph& graph, int numThreads = std::thread::hardware_concurrency());

private:
    static Graph createModifiedGraph(const Graph& original);

    // Optimized parallel processing with futures
    static void dijkstraWorkerBatch(const Graph& graph,
        const std::vector<double>& h,
        std::vector<std::vector<double>>& distances,
        const std::vector<int>& vertices,
        std::atomic<int>& completedVertices,
        int totalVertices);

    // Sequential fallback for small graphs
    static Result runSequential(const Graph& graph);

    // Threshold for parallel execution
    static constexpr int MIN_VERTICES_FOR_PARALLEL = 100;
    static constexpr int MIN_VERTICES_PER_THREAD = 10;
};