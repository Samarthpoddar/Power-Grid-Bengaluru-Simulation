// Test driver for the Graph, Union-Find, Connectivity, Dijkstra, Kruskal and Scheduler modules.
// Replace this with the simulation loop later.
#include <iostream>
#include "Graph.h"
#include "UnionFind.h"
#include "Connectivity.h"
#include "Dijkstra.h"
#include "Kruskal.h"
#include "Scheduler.h"
using namespace std;

int passed = 0;
int failed = 0;

void check(bool condition, string label) {
    if (condition) {
        passed++;
        cout << "PASS: " << label << endl;
    } else {
        failed++;
        cout << "FAIL: " << label << endl;
    }
}

int main() {
    // sample grid shared by the whole team
    int s1 = addNode("Substation A", SUBSTATION);
    int s2 = addNode("Substation B", SUBSTATION);
    int h  = addNode("City Hospital", HOSPITAL);
    int w  = addNode("Water Plant", WATER_TREATMENT);
    int c  = addNode("Telecom Tower", COMMUNICATIONS);
    int r1 = addNode("Residential 1", CONSUMER);
    int r2 = addNode("Residential 2", CONSUMER);

    int e0 = addEdge(s1, s2, 5);
    int e1 = addEdge(s1, h, 3);
    int e2 = addEdge(s2, w, 4);
    int e3 = addEdge(s2, c, 2);
    int e4 = addEdge(h, r1, 6);
    int e5 = addEdge(w, r2, 7);
    int e6 = addEdge(r1, r2, 8);

    printGraph();
    cout << endl;

    // graph tests
    check(numVertices == 7, "node count is 7");
    check(numEdges == 7, "edge count is 7");
    check(isCritical(h) && isCritical(w) && isCritical(c), "hospital, water, telecom are critical");
    check(!isCritical(r1), "residential is not critical");
    check(adjList[s2].size() == 3, "Substation B has 3 edges");
    check(otherEnd(e1, s1) == h && otherEnd(e1, h) == s1, "otherEnd works both ways");
    check(addEdge(s1, 99, 4) == -1, "edge to invalid node rejected");
    check(addEdge(s1, s1, 4) == -1, "self loop rejected");
    check(addEdge(s1, s2, -1) == -1, "negative effort rejected");
    check(addEdge(s2, s1, 9) == -1, "duplicate edge rejected");
    check(edges[e0].status == DAMAGED, "edge starts damaged");
    check(markRepaired(e0) && edges[e0].status == REPAIRED, "markRepaired works");
    check(!markRepaired(99), "markRepaired on invalid edge returns false");

    // union-find tests
    initUnionFind(numVertices);
    check(getComponentCount() == 7, "starts with 7 components");
    check(unionSets(s1, s2), "unite s1 and s2");
    check(!unionSets(s1, s2), "second unite of same pair returns false");
    unionSets(s2, h);
    unionSets(s2, w);
    check(isConnected(s1, w), "s1 and w are connected");
    check(!isConnected(s1, r1), "s1 and r1 are not connected yet");
    check(getComponentCount() == 4, "4 components after 3 unions");

    // cycle detection, the way Kruskal's will use it
    initUnionFind(3);
    unionSets(0, 1);
    unionSets(1, 2);
    check(!unionSets(0, 2), "unite on connected pair signals a cycle");

    // connectivity (BFS) tests, e0 is already repaired from the tests above
    computePowered();
    check(isNodePowered(s1) && isNodePowered(s2), "substations are powered");
    check(!isNodePowered(h), "hospital has no power while its edge is damaged");
    check(countUnpoweredCritical() == 3, "3 critical nodes without power");
    markRepaired(e1);
    computePowered();
    check(isNodePowered(h), "hospital powered after repairing e1");
    check(countUnpoweredCritical() == 2, "2 critical nodes without power");
    markRepaired(e2);
    markRepaired(e3);
    computePowered();
    check(countUnpoweredCritical() == 0, "all critical nodes powered");
    check(!isNodePowered(r1), "residential 1 still has no power");
    markRepaired(e4);
    computePowered();
    check(isNodePowered(r1), "residential 1 powered through the hospital");
    check(!isNodePowered(99), "invalid node reports no power");

    // repair cost (Dijkstra's) tests, start again with every edge damaged
    for (int i = 0; i < numEdges; i++) {
        edges[i].status = DAMAGED;
    }
    computeRepairCosts();
    check(getRepairCost(s1) == 0, "substation costs 0");
    check(getRepairCost(h) == 3, "hospital costs 3");
    check(getRepairCost(r1) == 9, "residential 1 costs 9");
    check(getRepairCost(r2) == 11, "residential 2 costs 11 through the water plant");
    int path[MAX_VERTICES];
    int count = getRepairPath(r2, path);
    check(count == 2 && path[0] == e2 && path[1] == e5, "path to residential 2 is e2 then e5");
    printRepairPlan(r2);
    markRepaired(e2);
    computeRepairCosts();
    check(getRepairCost(r2) == 7, "cost drops to 7 after repairing e2");
    check(getRepairPath(r2, path) == 1, "only one repair left on the path");
    // minimum repair network (Kruskal's) tests, only e2 is repaired at this point
    int total = computeMinimumRepairNetwork();
    check(total == 23, "minimum repair network costs 23");
    check(getRepairEdgeCount() == 5, "5 edges chosen for repair");
    check(isGridConnectable(), "grid can be fully connected");
    bool usesE6 = false;
    bool usesE2 = false;
    for (int i = 0; i < getRepairEdgeCount(); i++) {
        if (getRepairEdge(i) == e6) usesE6 = true;
        if (getRepairEdge(i) == e2) usesE2 = true;
    }
    check(!usesE6, "most expensive edge e6 is skipped because it forms a cycle");
    check(!usesE2, "already repaired edge e2 is not chosen again");
    check(getRepairEdge(99) == -1, "invalid index returns -1");
    printMinimumRepairNetwork();

    int isolated = addNode("Isolated Village", CONSUMER);
    computeRepairCosts();
    check(getRepairCost(isolated) == -1, "unconnected node cannot be reached");
    computeMinimumRepairNetwork();
    check(!isGridConnectable(), "isolated node means the grid cannot be fully connected");
    check(getRepairPath(isolated, path) == 0, "no path to unconnected node");
    check(getRepairCost(99) == -1, "invalid node returns -1");

    // repair scheduler tests, start again with every edge damaged
    for (int i = 0; i < numEdges; i++) {
        edges[i].status = DAMAGED;
    }
    check(buildRepairSchedule() == 5, "schedule has 5 repairs");
    check(getScheduledEdge(0) == e1, "hospital edge e1 is repaired first");
    check(getScheduledEdge(1) == e3, "telecom edge e3 is second");
    check(getScheduledEdge(2) == e2, "water edge e2 is third");
    check(getScheduledEdge(3) == e4 && getScheduledEdge(4) == e5, "consumer edges come last");
    check(getScheduleEffort() == 22, "total schedule effort is 22");
    check(edges[e1].status == DAMAGED, "real graph is not changed by the plan");
    check(getScheduledEdge(99) == -1, "invalid step returns -1");
    printRepairSchedule();
    markRepaired(e1);
    check(buildRepairSchedule() == 4 && getScheduledEdge(0) == e3, "schedule skips repairs already done");

    cout << endl << "Passed: " << passed << "  Failed: " << failed << endl;
    return 0;
}