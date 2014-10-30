/*
//	make virtual network
//	test simple routing algorithms on virtual network
//	dont forget the report!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define DEBUG 1

/*
==================================================================================================
CLASSES
TYPEDEFS
FUNCTION DECLARATIONS
==================================================================================================
*/

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
    int willDie;
} _packet;
typedef _packet * Packet;
Packet newPacket(Request req);

// nodes
typedef struct _node {
    Packet packet;
    struct _node * next;
} node;
typedef struct _node * Node;
typedef struct _node * Queue;


// search functions
Link getLink(char end1, char end2, Link * linkArray, int lArrayCount);

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
void printAllPackets(Queue head);


/*
==================================================================================================
MAIN
==================================================================================================
*/


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
    if(DEBUG){
        printf("\n\n\n\n\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nBEGIN ROUTING PERFORMANCE\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        printf("DEBUG args read: %s %s %s %s %s %d\n", argv[0], network_scheme, routing_scheme, topology_file, workload_file, packet_rate);
    }
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
    if(DEBUG){
        printf("debug!: topology %d workload %d\n", tCount, wCount);
    }
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
    if(DEBUG){
        printf("loaded links\n");
    }
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

    if(DEBUG){
        printf("loaded requests\n");
    }
    // print out things 
    
    //printAllLinks(linkArray, lArrayCount);
    //printAllRequests(requestArray, rArrayCount);
    //printf("files read!s\n");
    



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


    // compile our queue of packets
    Queue packetQueue = newQueue();
    //Node queueEnd = packetQueue;
    // packet rate is packets per second, change to seconds per packet
    float secondsPerPacket =  (float)1.0/packet_rate;
    printf("we send a packet every %f seconds!\n", secondsPerPacket);
    if(strcmp(network_scheme, "CIRCUIT")){
        // focus on processing every request first
        // for -> each request's index
        printf("THERE ARE %d MANY REQUESTS\n", rArrayCount);
        for(int x = 0; x < rArrayCount; x++){

            if(DEBUG){
                printf("\n\n\n\n\n==========================================\nRequest loop iteration %d\n==========================================\n", x+1);
            }
            // split our request into multiple packets.
            // take 1/packetRate = time per packet
            // then ceiling(time to live/time per packet) + 1 = packets to send
            // queue up packets per increment of time per packet from beginnin time.

            // figure out number of packets to send, do all but the last one
            int packetsToSend = ceil((requestArray[x]->timeToLive) / secondsPerPacket);
            
            if(DEBUG){
                printf("********* we are going to send %d packets\n", packetsToSend);
            }

            // make the packets for a single request
            for(double y = 1; y < packetsToSend; y++){

                if(DEBUG){
                    printf("\n================\nY loop iteration %f\n================\n", y);
                }

                Packet temp = newPacket(requestArray[x]);
                temp->startTime = requestArray[x]->timeToConnect + ((y-1) * secondsPerPacket);
                temp->endTime = requestArray[x]->timeToConnect + (y * secondsPerPacket); 
                temp->willDie = 0;
                if(DEBUG){
                    printf("adding packet for this request\n");
                }
                packetQueue = addToQueue(newNode(temp), packetQueue);
            }

            if(DEBUG){
                printf("=========\nexited y loop\n=========\n");
            }
            // do the last packet.. it might be truncated early. it might not. cant loop this one. 
            
            Packet temp = newPacket(requestArray[x]);
            temp->startTime = requestArray[x]->timeToConnect + (packetsToSend-1)*secondsPerPacket;
            temp->endTime = requestArray[x]->timeToConnect + requestArray[x]->timeToLive;
            temp->willDie = 0;
            if(DEBUG){
                printf("adding last packet for this request\n");
            }
            packetQueue = addToQueue(newNode(temp), packetQueue);
        }
    }
    // debug printing
    printAllPackets(packetQueue);

    // loop through our queue of packets
    if(DEBUG){
        printf("~~~~~~~~~~~~~~~~~~~~~~~~\nLETS PROCESS SOME FKN ALGORITHMS\n~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }


    // print out statistical results in standard output
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




/*
==================================================================================================
==================================================================================================
==================================================================================================
==================================================================================================
==================================================================================================
==================================================================================================
==================================================================================================
FUNCTION IMPLEMENTATIONS
==================================================================================================
==================================================================================================
==================================================================================================
==================================================================================================
==================================================================================================
==================================================================================================
==================================================================================================
*/


// get the link for two specific chars
Link getLink(char end1, char end2, Link * linkArray, int lArrayCount){
    for(int x = 0; x < lArrayCount; x++){
        if( linkArray[x]->end1 == end1 ){
            if( linkArray[x]->end2 == end2 ){
                return linkArray[x];
            }
        }
        if( linkArray[x]->end1 == end2 ){
            if( linkArray[x]->end2 == end1 ){
                return linkArray[x];
            }
        }
    }
    return NULL;
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

void printAllPackets(Queue head){
    Node temp = head;
    while(temp->next != NULL){
        printf("Packet\nFrom %c to %c\nStarts %f\nEnds %f\n\n",
            temp->packet->source->origin,
            temp->packet->source->destination,
            temp->packet->startTime,
            temp->packet->endTime);
        temp = temp->next;
    }
    printf("Packet\nFrom %c to %c\nStarts %f\nEnds %f\n\n",
        temp->packet->source->origin,
        temp->packet->source->destination,
        temp->packet->startTime,
        temp->packet->endTime);
}

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

Queue addToQueue(Node node, Queue q) {
    double comparison;

    // case for a completely empty queue
    if(q == NULL){
        q = newNode(node->packet);
        if(DEBUG){
            printf("\nadded FIRST node to queue\n");
        }
        return q;
    }

    // determine if we're concerned with start/end time
    if (node->packet->willDie == TRUE) {
        comparison = node->packet->endTime;
    } else {
        comparison = node->packet->startTime;
    }

    if(DEBUG){
        printf("searching filled node queue\n");
    }

    Node current = q;
    
    // FOR EVERY COMPARISON MUST ACTUALLY USE IF STATEMENTS TO CHECK IF IT WILLDIE

    // special case for for becoming earliest node
    if(current->packet->willDie == FALSE){
        if(comparison < current->packet->startTime){
            node->next = current;
            if(DEBUG){
                printf("added FIRST node to existing queue\n");
            }
            return node;
        }
    }
    if(current->packet->willDie == TRUE){
        if(comparison < current->packet->endTime){
            node->next = current;
            if(DEBUG){
                printf("added FIRST node to existing queue\n");
            }
            return node;
        }
    } 

    // now we're looping the rest. its a bit bleh for a do while loop because we're making current become the next
    while (current->next != NULL) {

        // for all these remember the difference is comparison vs current-> NEXT
        if(current->packet->willDie == FALSE){
            if(comparison < current->next->packet->startTime){
                node->next = current->next;
                current->next = node;
                if(DEBUG){
                    printf("added SOME MIDDLE node to queue\n");
                }
                return q;
            }
        }
        if(current->packet->willDie == TRUE){
            if(comparison < current->next->packet->endTime){
                node->next = current->next;
                current->next = node;
                if(DEBUG){
                    printf("added SOME MIDDLE node to queue\n");
                }
                return q;
            }
        } 
        current = current->next;
    }    
    current->next = node;
    if(DEBUG){
        printf("\nadded LAST node to queue\n");
    }
    return q;
}

Node popQueue(Queue q) {
    Node n = NULL;
    if (q != NULL) {
        n = q;
        q = q->next;
    }
    if(DEBUG){
        printf("popped a queue\n");
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
    if(DEBUG){
        //printf("made a new link\n");
    }
    return temp;
}

Request newRequest(void){
    Request temp = malloc(sizeof(_request));
    temp->timeToConnect = 0;
    temp->origin = '0';
    temp->destination = '0';
    temp->timeToLive = 0;
    if(DEBUG){
        //printf("made a new request\n");
    }
    return temp;
}

Packet newPacket(Request req){
    Packet temp = malloc(sizeof(_packet));
    temp->source = req;
    temp->startTime = 0;
    temp->endTime = 0;
    temp->willDie = 0;
    if(DEBUG){
        //printf("made a new packet\n");
    }
    return temp;
}
