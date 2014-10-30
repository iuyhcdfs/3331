/*
//	make virtual network
//	test simple routing algorithms on virtual network
//	dont forget the report!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*typedef struct {
    Node first;
} _queue;

typedef _queue * Queue;*/



// class: connection between two nodes
typedef struct link{
    char end1;
    char end2;
    int distance;
    int maxLoad;
    int currentLoad;
} _link;
typedef _link * Link;
Link newLink(void);

// class: request for network load
typedef struct request{
    double timeToConnect; 
    char origin;
    char destination;
    double timeToLive;
    char circuitPath[26];
} _request;
typedef _request * Request;
Request newRequest(void);

// struct: packet to schedule a packet's transferral
typedef struct packet{
    Request source;
    double startTime;
    double endTime;
    char packetPath[26];
    struct packet * nextPacket;
} _packet;
typedef _packet * Packet;
Packet newpacket(Request req);

// class: stats for the result of a request
typedef struct stat{
    int hops;
    double delay;
    int blockedPackets;
    int routedPackets;
} _stat;
typedef _stat * Stat;

typedef struct _node {
    Packet packet;
    struct _node * next;
} node;

typedef struct _node * Node;
typedef struct _node * Queue;


Stat newStat(void);

// routing algorithms to update the request circuit
void routeLLP(Request request, Link link[]);
void routeSHP(Request request, Link link[]);
void routeSDP(Request request, Link link[]);

// queue functions - implementing a priority queue
Queue newQueue(void);
Queue addToQueue(Node newNode, Queue q);
Node popQueue(Queue q);
Node newNode(Packet packet);

// printing functions for debugging
void printAllLinks(Link * linkArray, int linkSize);
void printAllRequests(Request * requestArray, int requestSize);



int main (int argc, char* argv[]) {
    // CIRCUIT or PACKET
    char * network_scheme = argv[1];
    // SHP or SDP or LLP
    char * routing_scheme = argv[2];
    // filename
    char * topology_file = argv[3];
    // filename
    char * workload_file = argv[4];
    // int
    int packet_rate = atoi(argv[5]);

    // ./RoutingPerformance CIRCUIT SHP topology.txt workload.txt 2 
    printf("DEBUG args read: %s %s %s %s %s %d\n", argv[0], network_scheme, routing_scheme, topology_file, workload_file, packet_rate);

    // get our files
    FILE * tFile; 
    tFile = fopen(topology_file, "rt");
    FILE * wFile;
    wFile = fopen(workload_file, "rt");

    char buffer[50];
    // figure out how many lines in each file
    int tCount = 0;
    int wCount = 0;
    while(EOF != fscanf(tFile, "%[^\n]\n", buffer)){
        tCount++;
    }
    while(EOF != fscanf(wFile, "%[^\n]\n", buffer)){
        wCount++;
    }
    printf("debug!: topology %d workload %d\n", tCount, wCount);

    // make arrays of structs for the files
    Link linkArray[tCount];
    int lArrayCount = 0;
    Request requestArray[wCount];
    int rArrayCount = 0;

    char * buffer2;
    // make structs for text
    tFile = fopen(topology_file, "rt");
    while(EOF != fscanf(tFile, "%[^\n]\n", buffer)) {
        //printf("test a %s\n",buffer);
        buffer2 = buffer;
        Link temp = newLink();
        buffer2 = strtok(buffer2, " ");
        temp->end1 = buffer2[0];
        buffer2 = strtok(NULL, " ");
        temp->end2 = buffer2[0];
        buffer2 = strtok(NULL, " ");
        temp->distance = atoi(buffer2);
        buffer2 = strtok(NULL, " ");
        temp->maxLoad = atoi(buffer2);
        linkArray[lArrayCount] = temp;
        lArrayCount++;
    }
    printf("after first loop\n");

    wFile = fopen(workload_file, "rt");
    while(EOF != fscanf(wFile, "%[^\n]\n", buffer)){
        //printf("test a %s\n",buffer);
        buffer2 = buffer;
        Request temp = newRequest();
        buffer2 = strtok(buffer2, " ");
        temp->timeToConnect = atof(buffer2);
        buffer2 = strtok(NULL, " ");
        temp->origin = buffer2[0];
        buffer2 = strtok(NULL, " ");
        temp->destination = buffer2[0];
        buffer2 = strtok(NULL, " ");
        temp->timeToLive = atof(buffer2);
        requestArray[rArrayCount] = temp;
        rArrayCount++;
    }

    printf("not here lol\n");

    // print out things 
    /*
    printAllLinks(linkArray, lArrayCount);
    printAllRequests(requestArray, rArrayCount);
    printf("files read!s\n");
    */



    /*
    ==================================================================================================
    The general plan:

    if -> distinguish network type {
        loop -> to add packets to queue {
            if -> distinguish algorithm type {
                apply algorithm type and update route
                have code to repeat per packet if network time is PACKET
                function void addToQueue();
                    add to queue must place it in the correct order
            }
        }
    } 
sooo to make the loop and iterate at same time or to do it later....
for hop and delay it doesnt matter, it would have tried whatever any way

but you want to do it seperate... you have to take care of multiple requests
so youll HAVE to make a afterwards loop

so the following loop must be done afterwards
    loop -> iterate through queue of packets in time order {
        if -> is LLP {
            do some extra updating shit for the moment each packet gets routed (aka just before sending a packet, take its request and get a LLP route)
        }
        otherwise just do all the others
        for each packet
            if entire route = free
            go update the packet "existing" on the links for its path, increment load on everything
            if not then just skip the whole thing
    }

    get output results and print
    ==================================================================================================
    */

    // SHORTEST HOP PATH
    // Algorithm 1: basic dijkstra
    // find routes for packets
    // transmit. drop if link is full
    int j;
    if(strcmp(routing_scheme, "SHP")){
        // for each request apply algorithm 
        if (strcmp(network_scheme, "CIRCUIT")) {
            // Needs to set each packet path to circuit path             
            for (j=0; j < rArrayCount; j++) {
                routeSHP(requestArray[j], linkArray);
            }
            
        } else {
            // run algo for every packet
            
        }
    }

    // SHORTEST DELAY PATH
    // Algorithm 2: dijkstra with weights
    //
    if(strcmp(routing_scheme, "SDP")){
        // for each request apply algorithm 
        if (strcmp(network_scheme, "CIRCUIT")) {
            // run algo once
            for (j=0; j < rArrayCount; j++) {
                routeSDP(requestArray[j], linkArray);
            } 
 
        } else {
            // run algo for every packet
        }
    }

    // LEAST LOADED PATH
    // Algorithm 3: dijkstra with time-variable weights
    if(strcmp(routing_scheme, "LLP")){
        // for each request apply algorithm 
        if (strcmp(network_scheme, "CIRCUIT")) {
            // run algo once
            for (j=0; j < rArrayCount; j++) {
                routeLLP(requestArray[j], linkArray);
            }  
        } else {
            // run algo for every packet
        }

    }

    // print out specific results in standard output
   /*
    printf("total number of virtual circuit requests: %d\n",  );
    printf("total number of packets: %d\n",                   );
    printf("number of successfully routed packets: %d\n",     );
    percentage of successfully routed packets: 79.63
    number of blocked packets: 935
    percentage of blocked packets: 20.37
    average number of hops per circuit: 5.42
    average cumulative propagation delay per circuit: 120.54
*/


    return EXIT_SUCCESS;
}

// ALGORITHMS FOR ROUTING PACKETS OR CIRCUITS, NULL if path invalid
void routeSHP(Request request, Link link[]) {
    //Stat temp = newStat();
    
    
    return;
}

void routeSDP(Request request, Link link[]) {
    //Stat temp = newStat();

    return;
}

void routeLLP(Request request, Link link[]) {
    //Stat temp = newStat();

    return;
}



// Function implementation

void printAllLinks(Link * linkArray, int linkSize) {
    int i;
    for (i=0; i < linkSize; i++) {
        printf("\nLink %d: \n", i);
        printf("End1:    %c\nEnd2:    %c\nDistance:%d\nmaxLoad :%d\n\n",
            linkArray[i]->end1,
            linkArray[i]->end2,
            linkArray[i]->distance,
            linkArray[i]->maxLoad);
    }
}

void printAllRequests(Request * requestArray, int requestSize){
    int i;
    for (i=0; i < requestSize; i++) {
        printf("\nRequest %d: \n", i);
        printf("TTC:     %f\nOrigin:  %c\nDest:    %c\nTTL:     %f\n\n",
            requestArray[i]->timeToConnect,
            requestArray[i]->origin,
            requestArray[i]->destination,
            requestArray[i]->timeToLive);
    }
}

Queue newQueue(void) {
    return NULL;
}

Queue addToQueue(Node newNode, Queue q) {
    if (newNode->packet->startTime < q->packet->startTime) {
        newNode->next = q;
        q = newNode;
    } else {
        Node current = q;
        while (current->next != NULL && newNode->packet->startTime >= current->next->packet->startTime) {
            current = current->next;
        }    
        newNode->next = current->next->next;
        current->next = newNode;
    }
    return q;
}

Node popQueue(Queue q) {
    Node n = NULL;
    if (q != NULL) {
        n = q;
        q = q->next;
    }
    return n;
}

Node newNode(Packet packet) {
    Node new = malloc(sizeof(struct _node));
    new->packet = packet;
    new->next = NULL;
    return new;
}

Link newLink(void){
    Link temp = malloc(sizeof(_link));
    temp->end1 = '0';
    temp->end2 = '0';
    temp->distance = 0;
    temp->maxLoad = 0;
    temp->currentLoad = 0;
    return temp;
}

Request newRequest(void){
    Request temp = malloc(sizeof(_request));
    temp->timeToConnect = 0;
    temp->origin = '0';
    temp->destination = '0';
    temp->timeToLive = 0;
    return temp;
}

Packet newpacket(Request req){
    Packet temp = malloc(sizeof(_packet));
    temp->source = req;
    temp->startTime = 0;
    temp->endTime = 0;
    return temp;
}

Stat newStat(void){
    Stat temp = malloc(sizeof(_stat));
    temp->hops = 0;
    temp->delay = 0;
    temp->blockedPackets = 0;
    temp->routedPackets = 0;
    return temp;
}
