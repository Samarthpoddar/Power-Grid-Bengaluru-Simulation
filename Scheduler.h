#ifndef SCHEDULER_H
#define SCHEDULER_H

// Plans the order in which damaged edges should be repaired.
// Each possible repair gets a priority: the value of the nodes it newly powers
// divided by its repair effort. The highest priority repair is picked first.
// Hospitals, water treatment and communications are worth more than consumers.
// The real graph is not changed, the plan is only a suggestion.
int buildRepairSchedule();              // returns the number of repairs planned
int getScheduledEdge(int step);         // edge id at that step, -1 if invalid
int getScheduleEffort();                // total repair effort of the plan
void printRepairSchedule();

#endif
