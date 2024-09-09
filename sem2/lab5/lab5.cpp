//Realization of the algorithm for checking graph for planarity 
#include<iostream>
#include<vector>
using namespace std;

const int N = 100;

struct Node {
    int vertex;
    Node* next;
};
struct Graph {
    int numEdges;
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
    for (int i = 1; i <= graph.numVertices; i++) {
        graph.adjLists[i] = NULL;
    }
    cout << "Enter the number of edges: ";
    cin >> graph.numEdges;

    int startp, endp;
    cout << "Enter start and end points:\n";
    for (int i = 0; i < graph.numEdges; i++) {
        cin >> startp >> endp;
        addEdge(graph, startp, endp);

    }
}

void printGraph(Graph graph) {
    cout << "Graph representation:\n";
    for (int i = 1; i <= graph.numVertices; i++) {
        cout << "Vertex " << i << ": ";
        Node* current = graph.adjLists[i];
        while (current != NULL) {
            cout << current->vertex << " ";
            current = current->next;
        }
        cout << "\n";
    }
    cout << "\n";
}

// Перевірка наявності ребра між вершинами a та b
bool isEdge(Graph graph, int a, int b) {
    Node* current = graph.adjLists[a];
    while (current) {
        if (current->vertex == b) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Перевірка на підграф K5
bool checkK5(Graph graph, int a, int b, int c, int d, int e) {
    if (isEdge(graph, a, b) && isEdge(graph, a, c) && isEdge(graph, a, d) && isEdge(graph, a, e) &&
        isEdge(graph, b, c) && isEdge(graph, b, d) && isEdge(graph, b, e) &&
        isEdge(graph, c, d) && isEdge(graph, c, e) &&
        isEdge(graph, d, e)) {
        return true;
    }
    return false;
}

//Функції для перевірки графа на підграф К5
bool hasForbiddenSubgraph5(Graph graph) {
    vector<int> neighbors;
    // Перевірка на підграф K5
    for (int i = 1; i <= graph.numVertices; i++) {
        if (!graph.adjLists[i]) continue; // вершина ізольована
        int count = 0;
        Node* current = graph.adjLists[i];
        while (current) {
            neighbors.push_back(current->vertex);
            current = current->next;

        }

        if (neighbors.size() >= 5) {
            for (int a = 0; a < neighbors.size() - 4; a++) {
                for (int b = a + 1; b < neighbors.size() - 3; b++) {
                    for (int c = b + 1; c < neighbors.size() - 2; c++) {
                        for (int d = c + 1; d < neighbors.size() - 1; d++) {
                            for (int e = d + 1; e < neighbors.size(); e++) {

                                if (checkK5(graph, neighbors[a], neighbors[b], neighbors[c], neighbors[d], neighbors[e])) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

// Перевірка на підграф K33
bool checkK33(Graph graph, int a, int b, int c) {
    if (!isEdge(graph, a, b) && !isEdge(graph, b, c) && !isEdge(graph, a, c)) {
        return true;
    }
    return false;
}

//Функції для перевірки графа на підграф К33
bool hasForbiddenSubgraph33(Graph& graph) {
    for (int i = 1; i <= graph.numVertices; i++) {
        if (!graph.adjLists[i]) continue;
        vector<int> neighbors;
        Node* current = graph.adjLists[i];
        while (current) {
            neighbors.push_back(current->vertex);
            current = current->next;
        }
        if (neighbors.size() == 3) {
            // Перебираємо всі можливі комбінації трьох сусідів
            for (int a = 0; a < neighbors.size() - 2; a++) {
                for (int b = a + 1; b < neighbors.size() - 1; b++) {
                    for (int c = b + 1; c < neighbors.size(); c++) {
                        if (checkK33(graph, neighbors[a], neighbors[b], neighbors[c])) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}


int main() {
    Graph graph;
    createGraph(graph);
    printGraph(graph);
    // Перевірка на заборонені підграфи K5 або K3,3

    if (hasForbiddenSubgraph5(graph))
        cout << "Graph has forbidden subgraph K5\n";

    if (hasForbiddenSubgraph33(graph))
        cout << "Graph has forbiden subgraph K3,3\n";

    cout << (hasForbiddenSubgraph5(graph) || hasForbiddenSubgraph33(graph) ? "The graph is NOT planar" : "The graph is planar");

    return 0;
}


/*
К5
5
10
1 2
1 3
1 4
1 5
2 3
2 4
2 5
3 4
3 5
4 5
*/


/*
К3,3
6
9
1 4
1 5
1 6
2 4
2 5
2 6
3 4
3 5
3 6
*/

/*
К5 К3,3
10
19
1 4
2 4
1 5
2 5
2 6
3 5
3 6
4 7
5 7
5 8
5 9
5 10
6 8
7 8
7 9
7 10
8 9
8 10
9 10
*/
