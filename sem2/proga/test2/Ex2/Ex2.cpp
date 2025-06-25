#include<iostream>
using namespace std;

const int N = 25;

struct Node {
	int vertex;
	Node* next;
};

struct Graph {
	int numVertices;
	Node** adjLists;
};

Node* createNode(int vertex) {
	Node* newNode = new Node();
	newNode->vertex = vertex;
	newNode->next = NULL;
	return newNode;

}
void addEdge(Graph& graph, int startp, int endp) {
    Node* newNodeStart = createNode(endp);
    Node* newNodeEnd = createNode(startp);

    Node* current = graph.adjLists[startp];
    if (current == nullptr) {
        graph.adjLists[startp] = newNodeStart;
    }
    else {
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNodeStart;
    }

    current = graph.adjLists[endp];
    if (current == nullptr) {
        graph.adjLists[endp] = newNodeEnd;
    }
    else {
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNodeEnd;
    }
}

void createGraph(Graph& graph) {
    cout << "Enter the number of vertices: ";
    cin >> graph.numVertices;

    graph.adjLists = new Node * [graph.numVertices + 1];
    for (int i = 1; i <= graph.numVertices; ++i) {
        graph.adjLists[i] = nullptr;
    }

    int numEdges;
    cout << "Enter the number of edges: ";
    cin >> numEdges;

    cout << "Enter edges (source vertex and destination vertex):\n";
    for (int i = 0; i < numEdges; i++) {
        int startp, endp;
        cin >> startp >> endp;
        addEdge(graph, startp, endp);
    }
}

void printGraph(const Graph& graph) {
    cout << "Graph representation:\n";
    for (int i = 1; i <= graph.numVertices; i++) {
        cout << "Vertex " << i << " -> ";
        Node* current = graph.adjLists[i];
        while (current != nullptr) {
            cout << current->vertex << " ";
            current = current->next;
        }
        cout << endl;
    }
}



void isolatedVertices(const Graph& graph, bool* isolated) {
    bool found = false;
    int counter = 0;
    for (int i = 1; i <= graph.numVertices; i++) {
        if (!graph.adjLists[i]) {
            isolated[i] = true;
            found = true;
            counter++;
        }
    }
    if (!found) cout << "\nNo isolated vertices";
    else {
        cout << "\nIsolated vertices: ";
        for (int i = 1; i <= graph.numVertices; i++) {
            if (isolated[i] == true)
                cout << i << " ";
        }
        cout << "\nNumber of isolated vertices: " << counter;
    }

}

int main() {
    Graph graph;
    createGraph(graph);
    printGraph(graph);

    bool isolated[N] = { false };
    isolatedVertices(graph,isolated);

}