#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

// Finds which nodes can receive power using Breadth-First Search.
// Substations are the power sources. Power flows only through REPAIRED edges.
// Call computePowered() again after every repair to refresh the result.
void computePowered();
bool isNodePowered(int nodeId);
int countUnpoweredCritical();       // critical nodes still without power
void printPowerStatus();

#endif