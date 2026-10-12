#ifndef DIJKSTRA_H
#define DIJKSTRA_H

// Finds the cheapest repair effort needed to bring power to every node.
// All substations are the starting points. A DAMAGED edge costs its repair
// effort and a REPAIRED edge costs 0.
// Call computeRepairCosts() again after every repair to refresh the result.
void computeRepairCosts();
int getRepairCost(int nodeId);                  // -1 if the node cannot be reached
int getRepairPath(int nodeId, int pathEdges[]); // damaged edge ids, in repair order
void printRepairPlan(int nodeId);

#endif