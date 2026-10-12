#include <iostream>
#include "Scheduler.h"
#include "Graph.h"
#include "Connectivity.h"
using namespace std;

struct Candidate {
    int edgeId;
    int gain;       // value of the nodes this repair newly powers
    int effort;     // repair effort of the edge
};

// priority queue stored as a plain array
static Candidate pq[MAX_EDGES];
static int pqSize = 0;

// the repair plan
static int scheduleEdges[MAX_EDGES];
static int scheduleGain[MAX_EDGES];
static int scheduleLength = 0;
static int scheduleEffort = 0;

static int nodeValue(int nodeId) {
    int t = nodes[nodeId].type;
    if (t == HOSPITAL) return 10;
    if (t == WATER_TREATMENT) return 8;
    if (t == COMMUNICATIONS) return 6;
    if (t == CONSUMER) return 1;
    return 0;                           // substations are always powered
}

// total value of all nodes that currently have power
static int poweredValue() {
    int total = 0;
    for (int i = 0; i < numVertices; i++) {
        if (isNodePowered(i)) {
            total = total + nodeValue(i);
        }
    }
    return total;
}

static void pqClear() {
    pqSize = 0;
}

static bool pqEmpty() {
    return pqSize == 0;
}

static void pqInsert(int edgeId, int gain, int effort) {
    if (pqSize >= MAX_EDGES) {
        return;
    }
    pq[pqSize].edgeId = edgeId;
    pq[pqSize].gain = gain;
    pq[pqSize].effort = effort;
    pqSize++;
}

// a is better than b when gain/effort of a is larger.
// Cross multiplying avoids decimal numbers.
static bool isBetter(Candidate a, Candidate b) {
    return a.gain * b.effort > b.gain * a.effort;
}

// removes and returns the candidate with the highest priority
static Candidate pqExtractMax() {
    int best = 0;
    for (int i = 1; i < pqSize; i++) {
        if (isBetter(pq[i], pq[best])) {
            best = i;
        }
    }
    Candidate result = pq[best];
    pq[best] = pq[pqSize - 1];
    pqSize--;
    return result;
}

int buildRepairSchedule() {
    // remember the real statuses so they can be restored at the end
    int savedStatus[MAX_EDGES];
    for (int i = 0; i < numEdges; i++) {
        savedStatus[i] = edges[i].status;
    }
    scheduleLength = 0;
    scheduleEffort = 0;

    while (true) {
        pqClear();
        computePowered();
        int base = poweredValue();

        // try each damaged edge and see how much value it would add
        for (int e = 0; e < numEdges; e++) {
            if (edges[e].status == DAMAGED) {
                edges[e].status = REPAIRED;
                computePowered();
                int gain = poweredValue() - base;
                edges[e].status = DAMAGED;
                if (gain > 0) {
                    int effort = edges[e].weight;
                    if (effort < 1) {
                        effort = 1;     // avoid dividing by zero
                    }
                    pqInsert(e, gain, effort);
                }
            }
        }

        if (pqEmpty()) {
            break;                      // no repair can power anything new
        }

        Candidate best = pqExtractMax();
        edges[best.edgeId].status = REPAIRED;
        scheduleEdges[scheduleLength] = best.edgeId;
        scheduleGain[scheduleLength] = best.gain;
        scheduleLength++;
        scheduleEffort = scheduleEffort + edges[best.edgeId].weight;
    }

    for (int i = 0; i < numEdges; i++) {
        edges[i].status = savedStatus[i];
    }
    computePowered();
    return scheduleLength;
}

int getScheduledEdge(int step) {
    if (step < 0 || step >= scheduleLength) {
        return -1;
    }
    return scheduleEdges[step];
}

int getScheduleEffort() {
    return scheduleEffort;
}

void printRepairSchedule() {
    cout << "Repair schedule (" << scheduleLength << " repairs):" << endl;
    for (int i = 0; i < scheduleLength; i++) {
        int e = scheduleEdges[i];
        cout << "  " << (i + 1) << ". " << nodes[edges[e].u].name << " - "
             << nodes[edges[e].v].name << "  effort=" << edges[e].weight
             << "  gain=" << scheduleGain[i] << endl;
    }
    cout << "Total effort: " << scheduleEffort << endl;
}