#include <iostream>
#include "Dijkstra.h"
#include "Graph.h"
using namespace std;

const int INF = 1000000000;

static int dist[MAX_VERTICES];          // cheapest repair effort to power each node
static int prevEdge[MAX_VERTICES];      // edge used to reach each node
static bool done[MAX_VERTICES];         // true once the cheapest cost is final

// returns the unfinished node with the smallest distance, or -1 if none
static int minDistance() {
    int min = INF;
    int minIndex = -1;
    for (int v = 0; v < numVertices; v++) {
        if (!done[v] && dist[v] < min) {
            min = dist[v];
            minIndex = v;
        }
    }
    return minIndex;
}

void computeRepairCosts() {
    for (int i = 0; i < numVertices; i++) {
        dist[i] = INF;
        prevEdge[i] = -1;
        done[i] = false;
        if (nodes[i].type == SUBSTATION) {
            dist[i] = 0;
        }
    }

    for (int count = 0; count < numVertices; count++) {
        int u = minDistance();
        if (u == -1) {
            break;                      // remaining nodes cannot be reached
        }
        done[u] = true;

        for (int edgeId : adjList[u]) {
            int v = otherEnd(edgeId, u);
            int cost = 0;
            if (edges[edgeId].status == DAMAGED) {
                cost = edges[edgeId].weight;
            }
            if (!done[v] && dist[u] + cost < dist[v]) {
                dist[v] = dist[u] + cost;
                prevEdge[v] = edgeId;
            }
        }
    }
}

int getRepairCost(int nodeId) {
    if (nodeId < 0 || nodeId >= numVertices || dist[nodeId] == INF) {
        return -1;
    }
    return dist[nodeId];
}

int getRepairPath(int nodeId, int pathEdges[]) {
    if (getRepairCost(nodeId) == -1) {
        return 0;
    }
    int temp[MAX_VERTICES];
    int count = 0;
    int current = nodeId;

    // walk back from the node to a substation, keeping only damaged edges
    while (prevEdge[current] != -1) {
        int edgeId = prevEdge[current];
        if (edges[edgeId].status == DAMAGED) {
            temp[count] = edgeId;
            count++;
        }
        current = otherEnd(edgeId, current);
    }

    // reverse so the first repair is the one closest to the substation
    for (int i = 0; i < count; i++) {
        pathEdges[i] = temp[count - 1 - i];
    }
    return count;
}

void printRepairPlan(int nodeId) {
    if (nodeId < 0 || nodeId >= numVertices) {
        cout << "Invalid node." << endl;
        return;
    }
    int cost = getRepairCost(nodeId);
    if (cost == -1) {
        cout << nodes[nodeId].name << ": cannot be reached." << endl;
        return;
    }
    int pathEdges[MAX_VERTICES];
    int count = getRepairPath(nodeId, pathEdges);
    cout << nodes[nodeId].name << ": total repair effort " << cost << endl;
    for (int i = 0; i < count; i++) {
        int e = pathEdges[i];
        cout << "  repair " << nodes[edges[e].u].name << " - "
             << nodes[edges[e].v].name << " (effort " << edges[e].weight << ")" << endl;
    }
}