#include <iostream>
#include "Connectivity.h"
#include "Graph.h"
using namespace std;

static bool visited[MAX_VERTICES];      // true means the node has power
static int queueArr[MAX_VERTICES];
static int front = -1;
static int rear = -1;

static bool isQueueEmpty() {
    return front == -1 || front > rear;
}

static void enqueue(int vertex) {
    if (rear == MAX_VERTICES - 1) {
        cout << "Queue overflow." << endl;
        return;
    }
    if (front == -1) {
        front = 0;
    }
    rear++;
    queueArr[rear] = vertex;
}

static int dequeue() {
    if (isQueueEmpty()) {
        cout << "Queue underflow." << endl;
        return -1;
    }
    int vertex = queueArr[front];
    front++;
    return vertex;
}

void computePowered() {
    for (int i = 0; i < numVertices; i++) {
        visited[i] = false;
    }
    front = -1;
    rear = -1;

    // start the search from every substation at once
    for (int i = 0; i < numVertices; i++) {
        if (nodes[i].type == SUBSTATION) {
            visited[i] = true;
            enqueue(i);
        }
    }

    while (!isQueueEmpty()) {
        int currentVertex = dequeue();
        for (int edgeId : adjList[currentVertex]) {
            if (edges[edgeId].status == REPAIRED) {
                int neighbor = otherEnd(edgeId, currentVertex);
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    enqueue(neighbor);
                }
            }
        }
    }
}

bool isNodePowered(int nodeId) {
    if (nodeId < 0 || nodeId >= numVertices) {
        return false;
    }
    return visited[nodeId];
}

int countUnpoweredCritical() {
    int count = 0;
    for (int i = 0; i < numVertices; i++) {
        if (isCritical(i) && !visited[i]) {
            count++;
        }
    }
    return count;
}

void printPowerStatus() {
    for (int i = 0; i < numVertices; i++) {
        cout << "  " << nodes[i].name << ": ";
        if (visited[i]) {
            cout << "POWERED" << endl;
        } else {
            cout << "NO POWER" << endl;
        }
    }
}