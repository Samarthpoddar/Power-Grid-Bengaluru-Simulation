
#include <iostream>
#include <string>
#include <sstream>
#include "Graph.h"
#include "Connectivity.h"
#include "Dijkstra.h"
#include "Kruskal.h"
#include "Scheduler.h"
using namespace std;

void loadSampleGrid() {
    int s1=addNode("Model Grid Node A - Central Bengaluru",SUBSTATION);
    int s2=addNode("Model Grid Node B - North Bengaluru",SUBSTATION);
    int h=addNode("Victoria Hospital",HOSPITAL);
    int w1=addNode("Jayanagar Water Tank (BWSSB)",WATER_TREATMENT);
    int c=addNode("BSNL Telephone Exchange - Indiranagar",COMMUNICATIONS);
    int w2=addNode("BTM Water Tank (BWSSB)",WATER_TREATMENT);
    int w3=addNode("Indiranagar Water Tank (BWSSB)",WATER_TREATMENT);
    // Illustrative model links only; these are not actual utility-grid connections.
    addEdge(s1,s2,5); addEdge(s1,h,3); addEdge(s2,w1,4); addEdge(s2,c,2);
    addEdge(h,w2,6); addEdge(w1,w3,7); addEdge(w2,w3,8);
}
string esc(const string& s) {
    string out;
    for(char c:s) { if(c=='"' || c=='\\') out+='\\'; if(c=='\n') out+="\\n"; else out+=c; }
    return out;
}
int nextPriorityEdge() {
    computePowered();
    // Priority order: hospital first, then every BWSSB water tank, then telecom.
    const int priorityTypes[] = {HOSPITAL, WATER_TREATMENT, COMMUNICATIONS};
    for (int priorityType : priorityTypes) {
        for (int target = 0; target < numVertices; ++target) {
            if (nodes[target].type != priorityType || isNodePowered(target)) continue;
            computeRepairCosts();
            int path[MAX_VERTICES];
            int count = getRepairPath(target, path);
            if (count > 0) return path[0];
        }
    }
    buildRepairSchedule();
    return getScheduledEdge(0);
}
void outputState() {
    computePowered();
    computeRepairCosts();
    int saved[MAX_EDGES];
    for(int i=0;i<numEdges;i++) saved[i]=edges[i].status;
    int schedule[MAX_EDGES], scheduleCount=0, scheduleEffort=0;
    while(scheduleCount < numEdges) {
        int e=nextPriorityEdge();
        if(e < 0) break;
        schedule[scheduleCount++]=e;
        scheduleEffort += edges[e].weight;
        edges[e].status=REPAIRED;
    }
    for(int i=0;i<numEdges;i++) edges[i].status=saved[i];
    computePowered();
    computeRepairCosts();
    int mstEffort=computeMinimumRepairNetwork();
    cout << "{\"ok\":true,\"nodes\":[";
    for(int i=0;i<numVertices;i++){
        if(i) cout<<",";
        cout<<"{\"id\":"<<nodes[i].id<<",\"name\":\""<<esc(nodes[i].name)
            <<"\",\"type\":\""<<typeToString(nodes[i].type)<<"\",\"powered\":"
            <<(isNodePowered(i)?"true":"false")<<",\"repairCost\":"<<getRepairCost(i)<<"}";
    }
    cout << "],\"edges\":[";
    for(int i=0;i<numEdges;i++){
        if(i) cout<<",";
        cout<<"{\"id\":"<<i<<",\"u\":"<<edges[i].u<<",\"v\":"<<edges[i].v
            <<",\"weight\":"<<edges[i].weight<<",\"status\":\""
            <<(edges[i].status==REPAIRED?"repaired":"damaged")<<"\"}";
    }
    cout << "],\"schedule\":[";
    for(int i=0;i<scheduleCount;i++){ if(i)cout<<","; cout<<schedule[i]; }
    cout << "],\"scheduleEffort\":"<<scheduleEffort<<",\"minimumNetworkEffort\":"<<mstEffort
         <<",\"criticalWithoutPower\":"<<countUnpoweredCritical()<<"}\n";
    cout.flush();
}
int main(){
    loadSampleGrid();
    string line;
    while(getline(cin,line)){
        istringstream in(line); string cmd; in>>cmd;
        if(cmd=="quit") break;
        if(cmd=="reset"){
            // process lifetime graph reset is implemented by restoring sample baseline statuses
            for(int i=0;i<numEdges;i++) edges[i].status=DAMAGED;
            outputState(); continue;
        }
        if(cmd=="repair" || cmd=="damage"){
            int id=-1; in>>id;
            if(id<0 || id>=numEdges){ cout<<"{\"ok\":false,\"error\":\"Invalid edge id\"}\n"; cout.flush(); continue; }
            edges[id].status=(cmd=="repair"?REPAIRED:DAMAGED);
            outputState(); continue;
        }
        if(cmd=="step"){
            int id=nextPriorityEdge();
            if(id>=0) edges[id].status=REPAIRED;
            outputState(); continue;
        }
        if(cmd=="state"){ outputState(); continue; }
        cout<<"{\"ok\":false,\"error\":\"Unknown command\"}\n"; cout.flush();
    }
    return 0;
}
