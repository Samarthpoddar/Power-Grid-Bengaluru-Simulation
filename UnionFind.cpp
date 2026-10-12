#include "UnionFind.h"

const int MAX_UF = 100;

static int parent[MAX_UF];
static int rankArr[MAX_UF];
static int components = 0;

void initUnionFind(int n) {
    if (n < 0 || n > MAX_UF) {
        components = 0;
        return;
    }
    for (int i = 0; i < n; i++) {
        parent[i] = i;      // every element is its own root
        rankArr[i] = 0;
    }
    components = n;
}

int findParent(int x) {
    if (parent[x] == x) {
        return x;
    }
    parent[x] = findParent(parent[x]);   // path compression
    return parent[x];
}

bool unionSets(int a, int b) {
    if (a < 0 || b < 0 || a >= MAX_UF || b >= MAX_UF) {
        return false;
    }
    int rootA = findParent(a);
    int rootB = findParent(b);

    if (rootA == rootB) {
        return false;       // already in the same set, a cycle would form
    }

    // union by rank: attach the shorter tree under the taller one
    if (rankArr[rootA] < rankArr[rootB]) {
        parent[rootA] = rootB;
    } else if (rankArr[rootA] > rankArr[rootB]) {
        parent[rootB] = rootA;
    } else {
        parent[rootB] = rootA;
        rankArr[rootA]++;
    }
    components--;
    return true;
}

bool isConnected(int a, int b) {
    return findParent(a) == findParent(b);
}

int getComponentCount() {
    return components;
}