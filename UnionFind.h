#ifndef UNIONFIND_H
#define UNIONFIND_H

// Disjoint Set (Union-Find) with path compression and union by rank.
void initUnionFind(int n);          // make n separate sets: 0 to n-1
int findParent(int x);              // root of the set containing x
bool unionSets(int a, int b);       // true if merged, false if already same set
bool isConnected(int a, int b);     // true if a and b are in the same set
int getComponentCount();            // number of separate sets left

#endif