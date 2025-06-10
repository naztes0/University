#pragma once
#include <vector>
#include <queue>
#include <functional>

class Graph;

class Dijkstra {
public:
    struct Node {
        int vertex;
        double distance;

        Node(int v, double d) : vertex(v), distance(d) {}

        bool operator>(const Node& other) const {
            return distance > other.distance;
        }
    };

    static std::vector<double> run(const Graph& graph, int source);

private:
    // Optimized version for smaller graphs
    static std::vector<double> runSimple(const Graph& graph, int source);
};