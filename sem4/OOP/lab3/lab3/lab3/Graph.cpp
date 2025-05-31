#include "Graph.h"

#include<iostream>
#include<thread>
#include<algorithm>
#include<functional>

Graph::Graph(int vertices) :vertices(vertices) {
	adjacencyList.resize(vertices);
}

void Graph::addEdge(int from, int to, double weight) {
	if (from >= 0 && from < vertices && to >= 0 && to < vertices) {
		adjacencyList[from].emplace_back(to, weight);
	}
}

void Graph::removeEdge(int from, int to) {
	if (from >= 0 && from < vertices) {
		adjacencyList[from].erase(
			std::remove_if(adjacencyList[from].begin(), adjacencyList[from].end(),
				[to](const Edge& e) {return e.to == to; }),
			adjacencyList[from].end()
		);
	}
}

const std::vector<Edge>& Graph::getNeighbors(int vertex) const {
	static std::vector<Edge> empty;
	if (vertex >= 0 && vertex < vertices) {
		return adjacencyList[vertex];
	}
	return empty;
}

bool Graph::hasNegativeWeights()const {
	for (const auto& edges : adjacencyList) {
		for (const auto& edge : edges) {
			if (edge.weight < 0) {
				return true;
			}
		}
	}
	return false;
}

void Graph::printGraph() const {
	for (int i = 0; i < vertices; ++i) {
		std::cout << "Vertice " << i << ": ";
		for (const auto& edge : adjacencyList[i]) {
			std::cout << "(" << edge.to << ", " << edge.weight << ") ";
		}
		std::cout << std::endl;
	}
}

std::vector<int> Graph::getVerticesRange(int start, int end) const {
	std::vector<int> range;
	for (int i = start; i < end && i < vertices; ++i) {
		range.push_back(i);
	}
	return range;
}

void Graph::parallelProcessVertices(int numThreads,
	std::function<void(int, int)> processor) const {
	std::vector<std::thread> threads;
	int verticesPerThread = vertices / numThreads;
	int remainder = vertices % numThreads;

	int start = 0;
	for (int i = 0; i < numThreads; ++i) {
		int end = start + verticesPerThread + (i < remainder ? 1 : 0);
		threads.emplace_back(processor, start, end);
		start = end;
	}

	for (auto& thread : threads) {
		thread.join();
	}
}