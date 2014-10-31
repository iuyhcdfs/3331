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
// do you want debug prints?
#define DEBUG 1
// displacement of capital ASCII letters
#define ASCII 65
#define MAX_PATH_SIZE 27

// lets set up all our statistics..
int totalPackets = 0;
int packetSuccessCounter = 0;
float totalHops = 0;
float totalDelay = 0;
float totalRoutes = 0;

// to make for loops compile on cse machines LIKE WHY NOT C99 GUYS
int x = 0;
int y = 0;
int z = 0;

/*
==================================================================================================
CLASSES
TYPEDEFS
FUNCTION DECLARATIONS
==================================================================================================
*/

// some janky ass path shit
typedef struct _path{
    int * path;
    int size;
} path;
typedef struct _path * Path;
Path newPath(void);


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
    int * circuitPath;
} _request;
typedef _request * Request;
Request newRequest(void);

// struct: packet to schedule a packet's transferral
typedef struct packet{
    Request source;
    double startTime;
    double endTime;
    int * packetPath;
    int willDie;
    int first;
    int length;
} _packet;
typedef _packet * Packet;
Packet newPacket(Request req);


// global adjacency matrix oh yes
Link adjMatrix[26][26];

// nodes
typedef struct _node {
    Packet packet;
    struct _node * next;
} node;
typedef struct _node * Node;
typedef struct _node * Queue;


// routing algorithms to update the request circuit
int * routeSHP(Packet packet);
int * routeSDP(Packet packet);
int * routeLLP(Packet packet);

// queue functions - implementing a priority queue
Queue newQueue(void);
Queue addToQueue(Node newNode, Queue q);
Node popQueue(Queue q);
Node newNode(Packet packet);

void initialiseArray(int*array, int size);

// printing functions for debugging
void printAllLinks(Link * linkArray, int linkSize);
void printAllRequests(Request * requestArray, int requestSize);
void printAllPackets(Queue head);


/*
==================================================================================================
MAIN
#: can be searched for appropriately commented bookmarks
1: initialise
2: read in files
3: load read data into appropriate structs
4: add the queue of packets
5: Process the queue
6: put statistics into standard output
7: exit
==================================================================================================
*/


int main (int argc, char* argv[]) {

    /*
    ==================================================================================================
    1: initialise
    ==================================================================================================
    */
    // set whole matrix to zero
    for(x = 0; x < 26; x++){
        for(y = 0; y < 26; y++){
            adjMatrix[x][y] = NULL;
        }
    }
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
    if(DEBUG){printf("\n\n\n\n\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nBEGIN ROUTING PERFORMANCE\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("DEBUG args read: %s %s %s %s %s %d\n", argv[0], network_scheme, routing_scheme, topology_file, workload_file, packet_rate);}

    /*
    ==================================================================================================
    2: read in files
    ==================================================================================================
    */

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
    if(DEBUG){printf("debug!: topology %d workload %d\n", tCount, wCount);}


    /*
    ==================================================================================================
    3: load read data into appropriate structs
    ==================================================================================================
    */

    // make arrays of structs for the files
    Link linkArray[tCount];
    int lArrayCount = 0;
    Request requestArray[wCount];
    int rArrayCount = 0;

    // setup our network!
    char * buffer2;
    // make structs for text
    tFile = fopen(topology_file, "rt");
    while(EOF != fscanf(tFile, "%[^\n]\n", buffer)) {
        //printf("test a %s\n",buffer);
        int adj1;
        int adj2;

        buffer2 = buffer;
        Link temp = newLink();
        buffer2 = strtok(buffer2, " ");
        temp->end1 = buffer2[0];

        adj1 = buffer2[0] - ASCII;

        buffer2 = strtok(NULL, " ");
        temp->end2 = buffer2[0];

        adj2 = buffer2[0] - ASCII;

        buffer2 = strtok(NULL, " ");
        temp->distance = atoi(buffer2);
        buffer2 = strtok(NULL, " ");
        temp->maxLoad = atoi(buffer2);
        linkArray[lArrayCount] = temp;

        adjMatrix[adj1][adj2] = temp;
        adjMatrix[adj2][adj1] = temp;

        lArrayCount++;
    }
    if(DEBUG){printf("loaded links\n");}

    // setup our requests!
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

    if(DEBUG){printf("loaded requests\n");}
    // print out things 
    if(DEBUG){printAllLinks(linkArray, lArrayCount);}
    if(DEBUG){printAllRequests(requestArray, rArrayCount);}
    if(DEBUG){printf("files read! time to add things to the queue\n");}


    /*
    ==================================================================================================
    4: add the queue of packets
    ==================================================================================================
    */



    // compile our queue of packets
    Queue packetQueue = newQueue();
    // packet rate is packets per second, change to seconds per packet
    float secondsPerPacket =  (float)1.0/packet_rate;
    printf("we send a packet every %f seconds!\n", secondsPerPacket);
        // focus on processing every request first
        // for -> each request's index

    printf("THERE ARE %d MANY REQUESTS\n", rArrayCount);
    

    for(x = 0; x < rArrayCount; x++){

        if(DEBUG){printf("\n\n\n\n\n==========================================\nRequest loop iteration %d\n==========================================\n", x+1);}
        // split our request into multiple packets.
        // take 1/packetRate = time per packet
        // then ceiling(time to live/time per packet) + 1 = packets to send
        // queue up packets per increment of time per packet from beginnin time.

        // figure out number of packets to send, do all but the last one
        int packetsToSend = ceil((requestArray[x]->timeToLive) / secondsPerPacket);
        // add to total amount of packets
        totalPackets += packetsToSend;

        if(DEBUG){printf("********* we are going to send %d packets\n", packetsToSend);}

        // WE ARE AT THE BEGINNING OF A NEW LOOP SO WE MUST SET THE FIRST PACKET TO TRUE
        int firstPacket = TRUE;

        // make the packets for a single request
        for(y = 1; y < packetsToSend; y++){

            if(DEBUG){printf("\n================\nY loop iteration %d\n================\n", y);}

            Packet temp = newPacket(requestArray[x]);
            temp->startTime = requestArray[x]->timeToConnect + (((float)y-1) * secondsPerPacket);
            temp->endTime = requestArray[x]->timeToConnect + ((float)y * secondsPerPacket); 
            temp->willDie = 0;

            if(DEBUG){printf("ROUTING SDP OR SHP, and adding packet for this request\n");}

            // special: if we are circuit, each request is on the same circuit! figure out the route NOW.
            if(strcmp(network_scheme, "CIRCUIT")){
                if(firstPacket == TRUE){

                    if(DEBUG){printf("first packet! leaving trigger for circuit LLP\n");}

                    if(strcmp(routing_scheme, "SDP")){
                        requestArray[x]->circuitPath = routeSDP(temp);
                    }
                    if(strcmp(routing_scheme, "SHP")){
                        requestArray[x]->circuitPath = routeSHP(temp);
                    }
                    // also we have to note for LLP that the first packet is the first here...
                    // all packets will now have pointers to the first packet
                    temp->first = TRUE;
                    firstPacket = FALSE;
                }
                // EVERY TIME in circuit, the original circuit path is used.
                temp->packetPath = requestArray[x]->circuitPath;
            }
            // special: if we are in packet, just route each thing every time
            if(strcmp(network_scheme, "PACKET")){
                if(strcmp(routing_scheme, "SDP")){
                    temp->packetPath = routeSDP(temp);
                }
                if(strcmp(routing_scheme, "SHP")){
                    temp->packetPath = routeSHP(temp);
                }
                // youll just route LLP every time mindlessly in phase 2.
            }
            // LLP does nothing! do this later when we know how paths are loaded

            // then add the things to the queue
            packetQueue = addToQueue(newNode(temp), packetQueue);
        }

        if(DEBUG){printf("=========\nexited y loop\n=========\n");}

        // do the last packet.. it might be truncated early. it might not. cant loop this one. 

        Packet temp = newPacket(requestArray[x]);
        temp->startTime = requestArray[x]->timeToConnect + (packetsToSend-1)*secondsPerPacket;
        temp->endTime = requestArray[x]->timeToConnect + requestArray[x]->timeToLive;
        temp->willDie = 0;
        
        if(DEBUG){printf("adding last packet for this request\n");}
        packetQueue = addToQueue(newNode(temp), packetQueue);
    }
    
    // debug printing
    if(DEBUG){printAllPackets(packetQueue);}
    // loop through our queue of packets
    if(DEBUG){printf("we have %d many packets processed!\n", totalPackets);}
    if(DEBUG){printf("~~~~~~~~~~~~~~~~~~~~~~~~\nLETS PROCESS THE GODDAMN QUEUE\n~~~~~~~~~~~~~~~~~~~~~~~~\n");}

    /*
    ==================================================================================================
    5: Process the queue
    ==================================================================================================
    */

    /*
    record all stats on the way!
    Packet processing
    if its LLP do the routing
    go through everything in the queue chronologically
    occupy the entire route, set the packet->willDie = TRUE,
    then use the function to put the packet back in the queue.
    if route is fully occupied, then well, i guess it fkn fails then eh
    printf eat shit and die
    i you find a packet that will die
    remove the routes it is occupying

    hops
    delays
    routes
    */


    // this loop will go through all the oncoming packets!
    while(packetQueue != NULL){

        // WE WILL ENCOUNTER TWO TYPES OF PACKETS, THAT LIVE OR DIE

        // ************* LIFE **************
        // the packet will be treated UTTERLY DIFFERENTLY if its set to be born or to die
        // the packets are in fact, events.
        if(packetQueue->packet->willDie == FALSE){

            // ************* LLP ***************
            // firstly, we have to do the LLP routing
            if(strcmp(routing_scheme, "LLP")){
                // if its circuit, then we only need one computation of the route
                if(strcmp(network_scheme, "CIRCUIT")){
                    // so if we encounter the first of its request, the packet shall be routed
                    if(packetQueue->packet->first == TRUE){
                        packetQueue->packet->source->circuitPath = routeLLP(packetQueue->packet);
                    } 
                    // we are always just copying the circuit path of the respective request otherwise.
                    packetQueue->packet->packetPath = packetQueue->packet->source->circuitPath;
                }
                // otherwise if its packet, just mindlessly route it every time.
                if(strcmp(network_scheme, "PACKET")){
                    packetQueue->packet->packetPath = routeLLP(packetQueue->packet);
                }
            }

            // ************* INCREMENT UP if we can *************
            // now we just increase the load of the route for whatever
            // its already been routed for the other two algorithms by the way
            int pathClear = TRUE;
            for(x = 0; x < packetQueue->packet->length - 1; x++){
                int loadMax = adjMatrix[packetQueue->packet->packetPath[x]][packetQueue->packet->packetPath[x+1]]->currentLoad;
                int loadCurrent = adjMatrix[packetQueue->packet->packetPath[x]][packetQueue->packet->packetPath[x+1]]->maxLoad;
                if(loadCurrent >= loadMax){
                    // then the thing has failed!
                    // update our stats

                    // we have failed, so set to not increase everything
                    pathClear = FALSE;
                }
            }
            if(pathClear == TRUE){
                // go through the route and load up the links each by 1

            }
        }

        // ************* DEATH **************
        // dying packets must have their burden removed from the appropriate links.
        if(packetQueue->packet->willDie == TRUE){
            // ************** INCREMENT DOWN *****************
            for(x = 0; packetQueue->packet->packetPath[x] != '\0'; x++){
                // go through the route and decrement the links each by 1
            
            }
        }

        // update some stats
        //averageHops = ;
        //averageDelay = ;
        packetQueue = popQueue(packetQueue);
    }
    // everything is free!

    // we're done!
    if(DEBUG){printf("\n\nMISSSHHON COMPLEEEE!\n\n\nDebriefing:\n");}

    /*
    ==================================================================================================
    6: put statistics into standard output
    ==================================================================================================
    */

    // print out statistical results in standard output
    printf("total number of virtual circuit requests: %d\n", rArrayCount);
    printf("total number of packets: %d\n", totalPackets);
    printf("number of successfully routed packets: %d\n", packetSuccessCounter);
    printf("percentage of successfully routed packets: %5.2f\n", ((float)packetSuccessCounter/(float)totalPackets) * 100);
    printf("number of blocked packets: %d\n", totalPackets - packetSuccessCounter);
    printf("percentage of blocked packets: %.2f\n", (((float)totalPackets - (float)packetSuccessCounter)/(float)totalPackets) * 100);
    printf("average number of hops per circuit: %.2f\n", totalHops/totalRoutes);
    printf("average cumulative propagation delay per circuit: %.2f\n", totalDelay/totalRoutes);
    
    if(DEBUG){printf("\nFox, you're becoming more like your father.\n");}
    if(DEBUG){printf("\nFINAL MATRIX: \n");}
    // print out adjacency matrix.
    if(DEBUG){
        printf("  ");
        for(y = 0; y < 26; y++){
            printf("%c ", y+ASCII);
        }
        printf("\n");
        for(x = 0; x < 26; x++){
            printf("%c ", x+ASCII);
            for(y = 0; y < 26; y++){
                if(adjMatrix[x][y] != NULL){
                    printf("X ");
                } else {
                    printf("  ");
                }
            }
            printf("\n");
        }
    }    
    /*
    ==================================================================================================
    7: exit
    ==================================================================================================
    */
    return EXIT_SUCCESS;
}




/*
==================================================================================================
==================================================================================================
==================================================================================================
==================================================================================================
FUNCTION IMPLEMENTATIONS
==================================================================================================
==================================================================================================
==================================================================================================
==================================================================================================
*/




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
        if(comparison <= current->packet->startTime){
            node->next = current;
            if(DEBUG){
                printf("added FIRST node to existing queue\n");
            }
            return node;
        }
    }
    if(current->packet->willDie == TRUE){
        if(comparison <= current->packet->endTime){
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
            if(comparison <= current->next->packet->startTime){
                node->next = current->next;
                current->next = node;
                if(DEBUG){
                    printf("added SOME MIDDLE node to queue\n");
                }
                return q;
            }
        }
        if(current->packet->willDie == TRUE){
            if(comparison <= current->next->packet->endTime){
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
  //  z++;
    if(DEBUG){
  //      printf("popping queue %d\n", z);
    }
    if (q != NULL) {
        n = q;
        q = q->next;
        free(n);
        return q;
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
    temp->first = FALSE;
    temp->length = 0;
    if(DEBUG){
        //printf("made a new packet\n");
    }
    return temp;
}

Path newPath(void){
    Path new = malloc(sizeof(struct _path));
    new->path = malloc(sizeof(int) * MAX_PATH_SIZE);
    initialiseArray(new->path, MAX_PATH_SIZE);
    new->size = 0;
    return new;
}
void initialiseArray(int*array, int size){
    int i;
    for(i = 0; i < size; i++){
        array[i] = -1;
    }
    return;
}


// ALGORITHMS FOR ROUTING PACKETS OR CIRCUITS, NULL if path invalid
    // have packet, have link, have size of link array
    // dont edit packets, just return the char* for it with a null terminator.

int * routeSHP(Packet packet){
    int * path = malloc(sizeof(int)*MAX_PATH_SIZE);
    initialiseArray(path, MAX_PATH_SIZE);
    int i;
    int place = 0;

    int start = packet->source->origin - ASCII;
    int end = packet->source->destination - ASCII;

    // 

    Packet * packetArray = malloc(sizeof(_packet)*MAX_PATH_SIZE);
    for (i=0; i<MAX_PATH_SIZE; i++) {

            pathArray[i] = newPath();
            pathArray[i][0]->path[0] =  ;
    }
    
    
    


    for (i=0; i < MAX_PATH_SIZE; i++) {
        free(packetArray[i]);
    }
    free(packetArray);
    return NULL;
}
int * routeSDP(Packet packet){
    return NULL;
}
int * routeLLP(Packet packet){
    return NULL;
}
