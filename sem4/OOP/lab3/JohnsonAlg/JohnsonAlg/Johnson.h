#pragma once
#include <vector>

class Graph;

class Johnson {
public:
    struct Result {
        std::vector<std::vector<double>> distances;
        bool success;

        Result(int vertices) : distances(vertices, std::vector<double>(vertices)), success(false) {}
    };

    static Result run(const Graph& graph);
private:
    static Graph createModifiedGraph(const Graph& original);
};
