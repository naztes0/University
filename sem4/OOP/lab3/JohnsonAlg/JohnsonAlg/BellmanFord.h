#pragma once
#include <vector>

class Graph;

class BellmanFord {
public:
    struct Result {
        std::vector<double> distances;
        bool hasNegativeCycle;

        Result(int vertices) : distances(vertices), hasNegativeCycle(false) {}
    };

    static Result run(const Graph& graph, int source);
};