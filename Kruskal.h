#ifndef KRUSKAL_H
#define KRUSKAL_H

// Finds the cheapest set of damaged edges to repair so that the whole grid
// becomes one connected network (Kruskal's minimum spanning tree).
// Edges that are already REPAIRED are free and are included first.
// Call computeMinimumRepairNetwork() again after every repair.
int computeMinimumRepairNetwork();      // returns the total repair effort
int getRepairEdgeCount();               // number of edges chosen for repair
int getRepairEdge(int index);           // edge id at that position, -1 if invalid
bool isGridConnectable();               // true if every node can be joined
void printMinimumRepairNetwork();

#endif