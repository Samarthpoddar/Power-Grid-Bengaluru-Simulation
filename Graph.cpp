#include <iostream>
#include "Graph.h"
using namespace std;

Node nodes[MAX_VERTICES];
Edge edges[MAX_EDGES];
list<int> adjList[MAX_VERTICES];
int numVertices = 0;
int numEdges = 0;

int addNode(string name, int type) {
    if (numVertices >= MAX_VERTICES) {
        cout << "Graph is full." << endl;
        return -1;
    }
    nodes[numVertices].id = numVertices;
    nodes[numVertices].name = name;
    nodes[numVertices].type = type;
    numVertices++;
    return numVertices - 1;
}

int otherEnd(int edgeId, int nodeId) {
    if (edges[edgeId].u == nodeId) {
        return edges[edgeId].v;
    }
    return edges[edgeId].u;
}

int addEdge(int u, int v, int weight) {
    if (u < 0 || u >= numVertices || v < 0 || v >= numVertices) {
        cout << "Invalid edge: node does not exist." << endl;
        return -1;
    }
    if (u == v || weight < 0 || numEdges >= MAX_EDGES) {
        cout << "Invalid edge." << endl;
        return -1;
    }
    // avoid duplicate edges
    for (int id : adjList[u]) {
        if (otherEnd(id, u) == v) {
            cout << "Edge already exists." << endl;
            return -1;
        }
    }
    edges[numEdges].u = u;
    edges[numEdges].v = v;
    edges[numEdges].weight = weight;
    edges[numEdges].status = DAMAGED;
    adjList[u].push_back(numEdges);
    adjList[v].push_back(numEdges);
    numEdges++;
    return numEdges - 1;
}

bool markRepaired(int edgeId) {
    if (edgeId < 0 || edgeId >= numEdges) {
        return false;
    }
    edges[edgeId].status = REPAIRED;
    return true;
}

bool isCritical(int nodeId) {
    int t = nodes[nodeId].type;
    return t == HOSPITAL || t == WATER_TREATMENT || t == COMMUNICATIONS;
}

string typeToString(int type) {
    if (type == SUBSTATION) return "Substation";
    if (type == CONSUMER) return "Consumer";
    if (type == HOSPITAL) return "Hospital";
    if (type == WATER_TREATMENT) return "Water Treatment";
    if (type == COMMUNICATIONS) return "Communications";
    return "Unknown";
}

void printGraph() {
    cout << "Nodes (" << numVertices << "):" << endl;
    for (int i = 0; i < numVertices; i++) {
        cout << "  [" << nodes[i].id << "] " << nodes[i].name
             << " (" << typeToString(nodes[i].type) << ")" << endl;
    }
    cout << "Edges (" << numEdges << "):" << endl;
    for (int i = 0; i < numEdges; i++) {
        cout << "  e" << i << ": " << nodes[edges[i].u].name << " - "
             << nodes[edges[i].v].name << "  effort=" << edges[i].weight;
        if (edges[i].status == DAMAGED) {
            cout << "  DAMAGED" << endl;
        } else {
            cout << "  REPAIRED" << endl;
        }
    }
}