#include <iostream>
#include <algorithm>
#include "Kruskal.h"
#include "Graph.h"
#include "UnionFind.h"
using namespace std;

static int chosenEdges[MAX_EDGES];      // edge ids selected for repair
static int chosenCount = 0;
static bool connectable = false;

// sorts edge ids in ascending order of repair effort
static bool compareEdges(int a, int b) {
    return edges[a].weight < edges[b].weight;
}

int computeMinimumRepairNetwork() {
    chosenCount = 0;
    connectable = false;
    if (numVertices == 0) {
        return 0;
    }

    initUnionFind(numVertices);
    int candidates[MAX_EDGES];
    int candidateCount = 0;

    // repaired edges are free, so join their nodes first
    for (int i = 0; i < numEdges; i++) {
        if (edges[i].status == REPAIRED) {
            unionSets(edges[i].u, edges[i].v);
        } else {
            candidates[candidateCount] = i;
            candidateCount++;
        }
    }

    sort(candidates, candidates + candidateCount, compareEdges);

    int totalEffort = 0;
    for (int i = 0; i < candidateCount; i++) {
        int e = candidates[i];
        if (unionSets(edges[e].u, edges[e].v)) {    // false means it would form a cycle
            chosenEdges[chosenCount] = e;
            chosenCount++;
            totalEffort = totalEffort + edges[e].weight;
        }
    }

    connectable = (getComponentCount() == 1);
    return totalEffort;
}

int getRepairEdgeCount() {
    return chosenCount;
}

int getRepairEdge(int index) {
    if (index < 0 || index >= chosenCount) {
        return -1;
    }
    return chosenEdges[index];
}

bool isGridConnectable() {
    return connectable;
}

void printMinimumRepairNetwork() {
    int total = 0;
    cout << "Minimum repair network (" << chosenCount << " edges):" << endl;
    for (int i = 0; i < chosenCount; i++) {
        int e = chosenEdges[i];
        cout << "  repair " << nodes[edges[e].u].name << " - "
             << nodes[edges[e].v].name << " (effort " << edges[e].weight << ")" << endl;
        total = total + edges[e].weight;
    }
    cout << "Total effort: " << total << endl;
    if (!connectable) {
        cout << "Note: some nodes cannot be connected to the rest of the grid." << endl;
    }
}