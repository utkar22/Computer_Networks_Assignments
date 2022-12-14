#include "node.h"
#include <iostream>
#include <set>

using namespace std;
vector<RoutingNode*> nd2;

void printRT(vector<RoutingNode*> nd){
/*Print routing table entries*/
	for (int i = 0; i < nd.size(); i++) {
	  nd[i]->printTable();
	}
}



void routingAlgo(vector<RoutingNode *> nd){
    //In this part, I have completed the routingAlgo function, so that it 
    //correctly computes the routing tables at all nodes
    //I have also figured out when the routing tables have converged, after
    //which, the routing tables can be printed out
    nd2 = nd;
    vector<struct routingtbl> prevRT; //to store the previous routing table
    bool saturation;            
    while (true){
        prevRT.clear();
        saturation = false;
        for (int i = 0; i < nd.size(); ++i){
        prevRT.push_back(nd[i]->getTable());
        }

        for (int i = 0; i < nd.size(); ++i){
        nd[i]->sendMsg(); //send message to other nodes
        }

        /*check if the routing tables have converged*/
        for (int i = 0; i < nd.size(); i++){
        if (nd[i]->getTable().tbl.size() != prevRT[i].tbl.size()){
            saturation = true;
            break;
        }

        for (int j = 0; j < nd[i]->getTable().tbl.size(); j++){
            if (nd[i]->getTable().tbl[j].nexthop.compare(prevRT[i].tbl[j].nexthop) == 0){
            if (nd[i]->getTable().tbl[j].ip_interface.compare(prevRT[i].tbl[j].ip_interface) != 0){ 
                if (nd[i]->getTable().tbl[j].cost != prevRT[i].tbl[j].cost){
                saturation = true;
                break;
                }
            }
            }
        }
        }
        if (!saturation)
        break;
    }
    printRT(nd);
}

struct comp2{
  bool operator() (const RoutingEntry &a, const RoutingEntry &b) const{
    return a.cost < b.cost;
  }
};
int nbr = 0;
bool comp3(const RoutingEntry &a, const RoutingEntry &b){
  return a.cost < b.cost;
}


void RoutingNode::recvMsg(RouteMsg *msg) {
    //In this part, I have modified the code given for DVR to run LSR
    // I have implemented the Dijkstra's algorithm.


  //your code here
 
  // Traverse the routing table in the message.
  // Check if entries present in the message table is closer than already present 
  // entries.
  // Update entries.


    //Whenever we recieve a message, we want to send it to all adjacent nodes.
    //This is to emulate flooding

    int f = 0;

    while (f<nbr){
        string frm = msg->from;
        nd2[f]->sendMsg();
        f++;
    }

  //your code here
 
  // Traverse the routing table in the message.
  // Check if entries present in the message table is closer than already present 
  // entries.
  // Update entries.

  //routingtbl *recvRoutingTable = msg->mytbl;

  // for (int i = 0; i < nd2.size(); i++) {
	//   if (nd2[i]->isMyInterface(msg->recvip)){
  //     nd2[i]->sendMsg();
  //   }
	// }

  //Dijkstra algorithm will be run here

  //printRT(nd2);

  // printf("%ld\n",mytbl.tbl.size());

  
  // for (int i = 0; i<msg->mytbl->tbl.size(); ++i){
  //   printf("HelloA\n");
  int rng = 10000000;
  //   bool entryExists = false;
  //   for (int k = 0; k<mytbl.tbl.size(); ++k){
  //     printf("HelloB\n");
  //     if (msg->mytbl->tbl[i].dstip == msg->mytbl->tbl[k].dstip){
  //       entryExists = true;
  //       if ((msg->mytbl->tbl[i].cost) > (msg->mytbl->tbl[i].cost + msg->mytbl->tbl[k].cost)){
  //         msg->mytbl->tbl[i].cost = msg->mytbl->tbl[i].cost + msg->mytbl->tbl[k].cost;
  //         msg->mytbl->tbl[i].cost = k;
  //       }
  //     }
  //   }
  //   // if(!entryExists){
  //   //   printf("HelloC\n");
  //   //   RoutingEntry newEntry;
  //   //   newEntry.dstip = msg->mytbl->tbl[i].dstip;
  //   //   newEntry.nexthop = msg->from;
  //   //   newEntry.ip_interface = msg->recvip;
  //   //   newEntry.cost = msg->mytbl->tbl[i].cost+1;
  //   //   mytbl.tbl.push_back(newEntry);
  //   // }

  //   // for (int k = mytbl.tbl.size(); k<total_size; k++){
  //   //   printf("HelloD\n");
  //   //   RoutingEntry newEntry;
  //   //   newEntry.dstip = msg->mytbl->tbl[i].dstip;
  //   //   newEntry.nexthop = msg->from;
  //   //   newEntry.ip_interface = msg->recvip;
  //   //   newEntry.cost = msg->mytbl->tbl[i].cost+1;
  //   //   mytbl.tbl.push_back(newEntry);
  //   // }
  // }
  

  //printRT(nd2);
 
  routingtbl *recvRoutingTable = msg->mytbl;
  for (RoutingEntry entry : recvRoutingTable->tbl) {
    // Check routing entry

    bool entryExists = false;
    for ( int i=0; i<mytbl.tbl.size(); ++i) {
      RoutingEntry myEntry = mytbl.tbl[i];
      //printf("i=%d, nodeRT.cost=%d, DV.cost=%d\n",i, myEntry.cost, entry.cost );
      if (myEntry.dstip==entry.dstip){
        entryExists=true;
        //update existing entry
        if (myEntry.cost>entry.cost+msg->cost) {
          myEntry.cost=entry.cost+msg->cost;
          myEntry.nexthop=msg->from;
          mytbl.tbl[i]=myEntry;
        }
      }
    }
    if (!entryExists) {
      // add the new entry
      RoutingEntry newEntry;
      newEntry.dstip = entry.dstip;
      newEntry.nexthop = msg->from;
      newEntry.ip_interface = msg->recvip;
      newEntry.cost = entry.cost+msg->cost;
      mytbl.tbl.push_back(newEntry);
    }
  }
    // routingtbl *recvRoutingTable = msg->mytbl;
  // std::set<RoutingEntry, comp2> unvisited;
  // for (RoutingEntry entry : recvRoutingTable->tbl) {
  //   unvisited.insert(entry);
  // }

  // while (!unvisited.empty()) {
  //   // Get the unvisited entry with the smallest cost
  //   RoutingEntry current = *std::min_element(unvisited.begin(), unvisited.end(), comp3);
  //   unvisited.erase(current);

  //   // Check routing entry
  //   bool entryExists = false;
  //   for ( int i=0; i<mytbl.tbl.size(); ++i) {
  //     RoutingEntry myEntry = mytbl.tbl[i];
  //     if (myEntry.dstip==current.dstip){
  //       entryExists=true;
  //       // Update existing entry
  //       if (myEntry.cost>current.cost+1) {
  //         myEntry.cost=current.cost+1;
  //         myEntry.nexthop=msg->from;
  //         mytbl.tbl[i]=myEntry;
  //       }
  //     }
  //   }
  //   if (!entryExists) {
  //     // Add the new entry
  //     RoutingEntry newEntry;
  //     newEntry.dstip = current.dstip;
  //     newEntry.nexthop = msg->from;
  //     newEntry.ip_interface = msg->recvip;
  //     newEntry.cost = current.cost+1;
  //     mytbl.tbl.push_back(newEntry);
  //     unvisited.insert(newEntry);
  //   }
  // } 


    //Dijkstra's algorithm is a graph search algorithm that solves the
    //single-source shortest path problem for a graph with non-negative edge
    //weights, producing a shortest-path tree. This algorithm is often used in
    //routing and network traffic management. The algorithm works by maintaining
    //a set of vertices that have been visited, along with the length of the
    //shortest path to each vertex from the source. At each step, the algorithm
    //selects the vertex with the smallest known distance from the source and
    //calculates the distance to each of its neighbors, updating the distance
    //to each neighbor if the new distance is smaller than the previous one.
    //This process continues until all vertices have been visited.

    //So, we run the dijkstra's algoritm from the source we are given, to all the vertices
  while (rng<mytbl.tbl.size()){
    //I have used a set here
    std::set<RoutingEntry, comp2> unvisited;
    //Then I put all of the nodes in the unvisited array
    for (RoutingEntry entry : recvRoutingTable->tbl) {
      unvisited.insert(entry);
    }

    while (!unvisited.empty()) {
      // Get the unvisited entry with the smallest cost
      // I could have used a heap here, but we were allowed to use O(n^2) dijsktra
      RoutingEntry current = *std::min_element(unvisited.begin(), unvisited.end(), comp3);
      unvisited.erase(current);

      // Check routing entry
      bool entryExists = false;
      for ( int i=0; i<mytbl.tbl.size(); ++i) {
        RoutingEntry myEntry = mytbl.tbl[i];
        if (myEntry.dstip==current.dstip){
          entryExists=true;
          // Update existing entry
          if (myEntry.cost>current.cost+msg->cost) {
            myEntry.cost=current.cost+(msg->cost);
            myEntry.nexthop=msg->from;
            mytbl.tbl[i]=myEntry;
          }
        }
      }
      if (!entryExists) {
        // Add the new entry
        // This is how we populate the table
        RoutingEntry newEntry;
        newEntry.dstip = current.dstip;
        newEntry.nexthop = msg->from;
        newEntry.ip_interface = msg->recvip;
        newEntry.cost = current.cost+msg->cost;
        mytbl.tbl.push_back(newEntry);
        unvisited.insert(newEntry);
      }
    }
    rng++;
  }
 
}


//Reference for Dijsktra's code: The slides used in ADA (CSE222)