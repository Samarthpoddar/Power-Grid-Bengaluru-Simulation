// Intelligent Power Grid Restoration Decision-Support System
// Interactive menu that uses the Graph, Connectivity, Dijkstra, Kruskal and Scheduler modules.
#include <iostream>
#include "Graph.h"
#include "Connectivity.h"
#include "Dijkstra.h"
#include "Kruskal.h"
#include "Scheduler.h"
using namespace std;

// sample grid: every line starts out damaged, as after a storm
void loadSampleGrid() {
    int s1 = addNode("Model Grid Node A - Central Bengaluru", SUBSTATION);
    int s2 = addNode("Model Grid Node B - North Bengaluru", SUBSTATION);
    int h  = addNode("Victoria Hospital", HOSPITAL);
    int w1 = addNode("Jayanagar Water Tank (BWSSB)", WATER_TREATMENT);
    int c  = addNode("BSNL Telephone Exchange - Indiranagar", COMMUNICATIONS);
    int w2 = addNode("BTM Water Tank (BWSSB)", WATER_TREATMENT);
    int w3 = addNode("Indiranagar Water Tank (BWSSB)", WATER_TREATMENT);

    // These are illustrative model connections, not real Bengaluru power lines.
    addEdge(s1, s2, 5);
    addEdge(s1, h, 3);
    addEdge(s2, w1, 4);
    addEdge(s2, c, 2);
    addEdge(h, w2, 6);
    addEdge(w1, w3, 7);
    addEdge(w2, w3, 8);
}

void printMenu() {
    cout << endl;
    cout << "===== Power Grid Restoration =====" << endl;
    cout << "1. Show grid" << endl;
    cout << "2. Show power status" << endl;
    cout << "3. Repair an edge" << endl;
    cout << "4. Report new damage on an edge" << endl;
    cout << "5. Cheapest repair plan for a node" << endl;
    cout << "6. Minimum repair network for the whole grid" << endl;
    cout << "7. Suggested repair schedule" << endl;
    cout << "8. Carry out the next scheduled repair" << endl;
    cout << "9. Exit" << endl;
    cout << "Enter your choice: ";
}

// reads one whole number, returns -1 if the input is not a number
// and -2 if the input has ended
int readNumber() {
    int value;
    if (!(cin >> value)) {
        if (cin.eof()) {
            return -2;
        }
        cin.clear();
        cin.ignore(1000, '\n');
        return -1;
    }
    return value;
}

void showPowerStatus() {
    computePowered();
    printPowerStatus();
    cout << "Critical facilities without power: " << countUnpoweredCritical() << endl;
}

int main() {
    loadSampleGrid();
    int choice = 0;

    while (choice != 9) {
        printMenu();
        choice = readNumber();

        if (choice == -2) {
            break;
        }

        switch (choice) {
            case 1:
                printGraph();
                break;

            case 2:
                showPowerStatus();
                break;

            case 3: {
                cout << "Enter the edge id to repair: ";
                int id = readNumber();
                if (id < 0 || id >= numEdges) {
                    cout << "Invalid edge id." << endl;
                } else if (edges[id].status == REPAIRED) {
                    cout << "That edge is already repaired." << endl;
                } else {
                    markRepaired(id);
                    cout << "Edge e" << id << " repaired." << endl;
                    showPowerStatus();
                }
                break;
            }

            case 4: {
                cout << "Enter the edge id that was damaged: ";
                int id = readNumber();
                if (id < 0 || id >= numEdges) {
                    cout << "Invalid edge id." << endl;
                } else if (edges[id].status == DAMAGED) {
                    cout << "That edge is already damaged." << endl;
                } else {
                    edges[id].status = DAMAGED;
                    cout << "Edge e" << id << " marked as damaged." << endl;
                    showPowerStatus();
                }
                break;
            }

            case 5: {
                cout << "Enter the node id: ";
                int id = readNumber();
                if (id < 0 || id >= numVertices) {
                    cout << "Invalid node id." << endl;
                } else {
                    computeRepairCosts();
                    printRepairPlan(id);
                }
                break;
            }

            case 6:
                computeMinimumRepairNetwork();
                printMinimumRepairNetwork();
                break;

            case 7:
                buildRepairSchedule();
                printRepairSchedule();
                break;

            case 8: {
                buildRepairSchedule();
                int next = getScheduledEdge(0);
                if (next == -1) {
                    cout << "No repair is left that would restore power to anything new." << endl;
                } else {
                    markRepaired(next);
                    cout << "Repaired " << nodes[edges[next].u].name << " - "
                         << nodes[edges[next].v].name << " (effort " << edges[next].weight << ")." << endl;
                    showPowerStatus();
                }
                break;
            }

            case 9:
                cout << "Goodbye." << endl;
                break;

            default:
                cout << "Invalid choice. Please enter a number from 1 to 9." << endl;
        }
    }
    return 0;
}