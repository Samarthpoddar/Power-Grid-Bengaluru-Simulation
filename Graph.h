#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <list>

const int MAX_VERTICES = 100;
const int MAX_EDGES = 500;

// node types
const int SUBSTATION = 0;
const int CONSUMER = 1;
const int HOSPITAL = 2;
const int WATER_TREATMENT = 3;
const int COMMUNICATIONS = 4;

// edge status
const int DAMAGED = 0;
const int REPAIRED = 1;

struct Node {
    int id;
    std::string name;
    int type;
};

struct Edge {
    int u;
    int v;
    int weight;     // repair effort
    int status;     // DAMAGED or REPAIRED
};

// shared graph data
extern Node nodes[MAX_VERTICES];
extern Edge edges[MAX_EDGES];
extern std::list<int> adjList[MAX_VERTICES];   // stores edge ids for each node
extern int numVertices;
extern int numEdges;

int addNode(std::string name, int type);          // returns node id, or -1 if invalid
int addEdge(int u, int v, int weight);            // returns edge id, or -1 if invalid
bool markRepaired(int edgeId);
int otherEnd(int edgeId, int nodeId);             // node on the other side of the edge
bool isCritical(int nodeId);                      // hospital, water or communications
std::string typeToString(int type);
void printGraph();

#endif