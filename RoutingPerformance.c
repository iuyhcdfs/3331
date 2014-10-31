/*
//    make virtual network
//    test simple routing algorithms on virtual network
//    dont forget the report!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
// do you want debug prints?
#define DEBUG 0
#define DEBUG2 0
#define DEBUG3 0
#define DEBUG4 0
#define DEBUGRUN 1
// displacement of capital ASCII letters
#define ASCII 65
#define MAX_PATH_SIZE 28

// lets set up all our statistics..
int totalPackets = 0;
int packetSuccessCounter = 0;
double totalHops = 0;
double totalDelay = 0;

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

// some janky ass path shit
typedef struct _path{
    int node;
    double priority;
    struct _path * parent;
    struct _path * next;
} path;
typedef struct _path * Path;
Path newPath(void);

// class: request for network load
typedef struct request{
    double timeToConnect; 
    char origin;
    char destination;
    double timeToLive;
    int * circuitPath;
    int circuitLength;
    struct request * queueNext;
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
    struct packet * original;
    struct packet * queueNext;
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
int * routeSHP(Request request, Link * linkArray, int lArrayCount);
int * routeSDP(Request request, Link * linkArray, int lArrayCount);
int * routeLLP(Request request, Link * linkArray, int lArrayCount);

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
    if(DEBUGRUN){printf("\n\n\n\n\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nBEGIN ROUTING PERFORMANCE\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
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
    if(DEBUGRUN){printf("debug!: topology %d workload %d\n", tCount, wCount);}


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
    if(DEBUGRUN){printf("loaded links\n");}

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

    if(DEBUGRUN){printf("loaded requests\n");}
    // print out things 
    if(DEBUG){printAllLinks(linkArray, lArrayCount);}
    if(DEBUG){printAllRequests(requestArray, rArrayCount);}
    if(DEBUGRUN){printf("files read! time to add things to the queue\n");}

    /*
    ==================================================================================================
    4 and 5: process the queue
    assuming that packets are sent in order we may now do this in linear time

    here is the steps
    we need to process requests and iterate packets at the same time
    for each request
    that is when new things are inserted
    therefore

    we can process all packets added before the new request is made.

    i am going to do this RIGHT.
    ==================================================================================================
    */

    // to use additional queue pointer functionality in request, get the head
    Request head = requestArray[0];

    // do something simple for circuit, occupy all nodes for an entire request.
    // you dont have to worry about packets.
    // match network scheme with circuit to enter
    if(strcmp(network_scheme, "CIRCUIT") == 0){

        // initialising important things
        
        // looping for all requests x
        int rIterator;
        for(rIterator = 0; rIterator < rArrayCount; rIterator++){
            
            
            
            // detect routing algo to route properly
            if(strcmp(routing_scheme, "SHP") == 0){
                // take our request and get a path for it.
                requestArray[rIterator]->circuitPath = routeSHP(requestArray[rIterator], linkArray, lArrayCount);
            }
            // add our request to the queue. 
            
            
            // now that we're onto the next request, process whatever packets are remaining 
            for(){
                
            }

        }

    }




    /*
    ==================================================================================================
    6: put statistics into standard output
    ==================================================================================================
    */

    // print out statistical results in standard output
    printf("total number of virtual circuit requests: %d\n", rArrayCount);
    printf("total number of packets: %d\n", totalPackets);
    printf("number of successfully routed packets: %d\n", packetSuccessCounter);
    printf("percentage of successfully routed packets: %5.2f\n", ((double)packetSuccessCounter/(double)totalPackets) * 100);
    printf("number of blocked packets: %d\n", totalPackets - packetSuccessCounter);
    printf("percentage of blocked packets: %.2f\n", (((double)totalPackets - (double)packetSuccessCounter)/(double)totalPackets) * 100);
    printf("average number of hops per circuit: %.2f\n", totalHops/packetSuccessCounter);
    printf("average cumulative propagation delay per circuit: %.2f\n", totalDelay/packetSuccessCounter);
    
    //if(DEBUG){printf("\nFox, you're becoming more like your father.\n");}
    //if(DEBUG){printf("\nFINAL MATRIX: \n");}
    // print out adjacency matrix.
    /*if(DEBUG){
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
    */

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

    // case for a completely empty queue
    if(q == NULL){
        //q = newNode(node->packet);
        if(DEBUG3){
            printf("\nadded FIRST node to queue\n");
        }
        node->next = NULL;
        return node;
    }

    // comparison double
    double comparison;
    // determine if we're concerned with start/end time
    if (node->packet->willDie == TRUE) {
        comparison = node->packet->endTime;
    } else {
        comparison = node->packet->startTime;
    }

    if(DEBUG3){
        printf("searching filled node queue\n");
    }

    Node current = q;
    
    // FOR EVERY COMPARISON MUST ACTUALLY USE IF STATEMENTS TO CHECK IF IT WILLDIE

    // special case for for becoming earliest node
    if(current->packet->willDie == FALSE){
        if(comparison < current->packet->startTime){
            node->next = current;
            if(DEBUG3){
                printf("added STARTING node to existing queue\n");
            }
            return node;
        }
    }
    if(current->packet->willDie == TRUE){
        if(comparison < current->packet->endTime){
            current->next = node;
            if(DEBUG3){
                printf("added STARTING node to existing queue\n");
            }
            return current;
        }
    } 

    // now we're looping the rest. its a bit bleh for a do while loop because we're making current become the next
    while (current->next != NULL) {
        // for all these remember the difference is comparison vs current-> NEXT
        if(current->next->packet->willDie == FALSE){
            if(comparison < current->next->packet->startTime){
                node->next = current->next;
                current->next = node;
                
                if(DEBUG3){
                    printf("added SOME MIDDLE node to queue\n");
                }
                return q;
            }
        }
        if(current->next->packet->willDie == TRUE){
            if(comparison < current->next->packet->endTime){
                node->next = current->next;
                current->next = node;
                if(DEBUG3){
                    printf("added SOME MIDDLE node to queue\n");
                }
                return q;
            }
        } 
        current = current->next;
    }    
    current->next = node;
    if(DEBUG3){
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
    temp->willDie = FALSE;
    temp->first = FALSE;
    temp->length = 0;
    if(DEBUG){
        //printf("made a new packet\n");
    }
    return temp;
}

Path newPath(void){
    Path new = malloc(sizeof(path));
    new->node = '\0';
    new->priority = 0;
    new->parent = NULL;
    new->next = NULL;
    return new;
}

// ALGORITHMS FOR ROUTING PACKETS OR CIRCUITS, NULL if path invalid
    // have packet, have link, have size of link array
    // dont edit packets, just return the char* for it with a null terminator.

int linkIndex(Link link, Link * linkArray, int lArrayCount){
    int i = 0;
    for(i = 0; i < lArrayCount; i++){
        if(link == linkArray[i]){
            if(DEBUG){printf("we got one at slot %d\n", i);}
            return i;
        }
    }
    //if(DEBUG){printf("didnt match anything unchecked.");}
    return -1;
}

int * routeSHP(Request request, Link * linkArray, int lArrayCount){
    int * finalPath = malloc(sizeof(int)*MAX_PATH_SIZE);
    int start = request->origin - ASCII;
    int end = request->destination - ASCII;
    Link * unchecked = malloc(sizeof(Link)*lArrayCount);
    // the first in the priority queue.
    Path queue = newPath();
    queue->node = start;
    // no priority weight for me
    queue->priority = 0;
    Path tail = queue;
    int i = 0;
    for(i = 0; i < 26; i++){
        finalPath[i] = -1;
    }
    for(i = 0; i < lArrayCount; i++){
        unchecked[i] = linkArray[i];
    }
    // we got all our unchecked.
    int dontDie = TRUE;
    while(dontDie == TRUE){
        // each time we get the kid we loop through to find all their neighbours
        for(i = 0; i < 26; i++){
            if(adjMatrix[queue->node][i] != NULL){
                if(DEBUG){
                    printf("\n\nthe current queue to EXPAND is at node %c\n",queue->node + ASCII);
                    printf("now, we found a match at %c\n", i + ASCII);
                    printf("and its a link between %c %c\n", adjMatrix[queue->node][i]->end1,adjMatrix[queue->node][i]->end2);
                }
                // if we find a link
                // create a new path for it
                // attach it to ... whos its parent
                // then add it to the queue
                if(-1 != linkIndex(adjMatrix[queue->node][i], unchecked, lArrayCount)){
                    if(DEBUG){ printf("also, we havent done this before!\n");}

                    // otherwise we continue the search
                    Path temp = newPath();
                    temp->node = i;
                    int hops = 0;
                    if(queue != NULL){
                        hops = queue->priority + 1;
                    }
                    if(DEBUG){printf("we have %d hops for this guy\n", hops);}
                    temp->priority = (double)hops; 
                    temp->parent = queue;
                    // were done if were done

                    // and remove from unchecked nodes
                    unchecked[linkIndex(adjMatrix[queue->node][i], unchecked, lArrayCount)] = NULL;
                    if(i == end){
                        if(DEBUG){printf("we finished. we're at %c which should be next to %c\n",queue->node + ASCII,end + ASCII);}
                        queue = temp;
                        dontDie = FALSE;
                        break;
                    }
                    tail->next = temp;
                    tail = temp;
                } else if (DEBUG) {
                    if(DEBUG){ printf("but we've already checked this one.\n");}
                }
            }
        }
        if(dontDie == TRUE){
            if(DEBUG){printf("\ntime to try a new queue\n");}
            //Path bleh = queue;
            queue = queue->next;
            //free(bleh);
        }
    }
    if(DEBUG){
        for(i = 0; i < lArrayCount; i++){
            printf("wehave unchecked %p\n", unchecked[i]);
        }
    }

    // so now QUEUE is the lowest child and also the solution.
    // copy out the path backwards!
    int hops = 0;
    for (i = queue->priority; i >= 0; i--) {
        if(DEBUG){printf("looping through final path: %c\n", queue->node + ASCII);}
        finalPath[i] = queue->node;
        queue = queue->parent;
        hops++;
    }
    if(DEBUG2){printf("recorded length hops as %d\n",hops);}
    // update the packet's hop length...
    request->circuitLength = hops;
    
    if(DEBUG){
        printf("\n\n\n\n\n\nohdear\n");
        int papa = 0;
        while(papa < 26){
            printf("%d",finalPath[papa]);
            papa++;
        }
        printf("\n");
    }
    return finalPath;
}
/*

same as shp except change priority for the cumulative dept



*/
int * routeSDP(Request request, Link * linkArray, int lArrayCount){
    return NULL;
}

/*

same as shp except change priority for already-live-updated in-usage ratios for links

*/
int * routeLLP(Request request, Link * linkArray, int lArrayCount){
    return NULL;
}





// graveyard









    /*
    ==================================================================================================
    4: add the queue of packets
    ==================================================================================================
    */


/*
    Packet firstPack = NULL;
    // compile our queue of packets
    Queue packetQueue = newQueue();
    // packet rate is packets per second, change to seconds per packet
    double secondsPerPacket =  (double)1.0/packet_rate;
    if(DEBUG){printf("we send a packet every %f seconds!\n", secondsPerPacket);}
        // focus on processing every request first
        // for -> each request's index

    if(DEBUGRUN){printf("THERE ARE %d MANY REQUESTS\n", rArrayCount);}
    
    for(x = 0; x < rArrayCount; x++){

        if(DEBUGRUN){printf("\n\n\n\n\n==========================================\nRequest loop iteration %d\n==========================================\n", x+1);}
        // split our request into multiple packets.
        // take 1/packetRate = time per packet
        // then ceiling(time to live/time per packet) + 1 = packets to send
        // queue up packets per increment of time per packet from beginnin time.

        // figure out number of packets to send PER REQUEST, do all but the last one
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
            temp->startTime = requestArray[x]->timeToConnect + (((double)y-1) * secondsPerPacket);
            temp->endTime = requestArray[x]->timeToConnect + ((double)y * secondsPerPacket); 
            temp->willDie = FALSE;
            if(DEBUG){printf("ROUTING SDP OR SHP, and adding packet for this request\n");}
            // special: if we are circuit, each request is on the same circuit! figure out the route NOW.
            if(0 == strcmp(network_scheme, "CIRCUIT")){
                if(firstPacket == TRUE){

                    if(DEBUG){printf("first packet! leaving trigger for circuit LLP\n");}

                    if(0 == strcmp(routing_scheme, "SDP")){
                        requestArray[x]->circuitPath = routeSDP(temp, linkArray, lArrayCount);
                    }
                    if(0 == strcmp(routing_scheme, "SHP")){
                        requestArray[x]->circuitPath = routeSHP(temp, linkArray, lArrayCount);
                    }
                    // also we have to note for LLP that the first packet is the first here...
                    // all packets will now have pointers to the first packet
                    temp->first = TRUE;
                    firstPacket = FALSE;
                    firstPack = temp;
                }
                // EVERY TIME in circuit, the original circuit path is used.
                temp->original = firstPack;
                temp->length = temp->original->length;
                temp->packetPath = requestArray[x]->circuitPath;
            }
            // special: if we are in packet, just route each thing every time
            if(0 == strcmp(network_scheme, "PACKET")){
                if(0 == strcmp(routing_scheme, "SDP")){
                    temp->packetPath = routeSDP(temp, linkArray, lArrayCount);
                }
                if(0 == strcmp(routing_scheme, "SHP")){
                    temp->packetPath = routeSHP(temp, linkArray, lArrayCount);
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
        temp->willDie = FALSE;

        // STILL NEED TO ROUTE LAST PACKET BLARRGH
        if(0 == strcmp(network_scheme, "CIRCUIT")){
            if(firstPacket == TRUE){

                if(DEBUG){printf("first packet! also the last! leaving trigger for circuit LLP\n");}

                if(0 == strcmp(routing_scheme, "SDP")){
                    requestArray[x]->circuitPath = routeSDP(temp, linkArray, lArrayCount);
                    
                }
                if(0 == strcmp(routing_scheme, "SHP")){
                    requestArray[x]->circuitPath = routeSHP(temp, linkArray, lArrayCount);

                }
                temp->first = TRUE;
                firstPacket = FALSE;
                firstPack = temp;
                
                temp->original = firstPack;
                temp->packetPath = requestArray[x]->circuitPath;
            }
                // EVERY TIME in circuit, the original circuit path is used.
        }
            // special: if we are in packet, just route each thing every time
        if(0 == strcmp(network_scheme, "PACKET")){
            if(0 == strcmp(routing_scheme, "SDP")){
                temp->packetPath = routeSDP(temp, linkArray, lArrayCount);
            }
            if(0 == strcmp(routing_scheme, "SHP")){
                temp->packetPath = routeSHP(temp, linkArray, lArrayCount);
            }
                // youll just route LLP every time mindlessly in phase 2.
        }

        if(DEBUG){printf("adding last packet for this request\n");}
        packetQueue = addToQueue(newNode(temp), packetQueue);
    }

    // debug printing
    if(DEBUG){printAllPackets(packetQueue);}
    // loop through our queue of packets
    if(DEBUGRUN){printf("we have %d many packets processed!\n", totalPackets);}
    if(DEBUGRUN){printf("~~~~~~~~~~~~~~~~~~~~~~~~\nLETS PROCESS THE GODDAMN QUEUE\n~~~~~~~~~~~~~~~~~~~~~~~~\n");}
*/

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
/*
    // this loop will go through all the oncoming packets!
    while(packetQueue != NULL){
        if(DEBUGRUN){printf("\nprocessing a packet... for %f to %f\n", packetQueue->packet->startTime, packetQueue->packet->endTime);}

        // WE WILL ENCOUNTER TWO TYPES OF PACKETS, THAT LIVE OR DIE

        // ************* LIFE **************
        // the packet will be treated UTTERLY DIFFERENTLY if its set to be born or to die
        // the packets are in fact, events.
        
        if(packetQueue->packet->willDie == FALSE){
            if(DEBUG){printf("processing a live packet...\n");}
            // ************* LLP ***************
            // firstly, we have to do the LLP routing
            if(0 == strcmp(routing_scheme, "LLP")){
                // if its circuit, then we only need one computation of the route
                if(0 == strcmp(network_scheme, "CIRCUIT")){
                    // so if we encounter the first of its request, the packet shall be routed
                    if(packetQueue->packet->first == TRUE){
                        packetQueue->packet->source->circuitPath = routeLLP(packetQueue->packet, linkArray, lArrayCount);
                    } 
                    // we are always just copying the circuit path of the respective request otherwise.
                    packetQueue->packet->packetPath = packetQueue->packet->source->circuitPath;
                }
                // otherwise if its packet, just mindlessly route it every time.
                if(0 == strcmp(network_scheme, "PACKET")){
                    packetQueue->packet->packetPath = routeLLP(packetQueue->packet, linkArray, lArrayCount);
                }
            }
            // ************* check if we  *************
            // now we just increase the load of the route for whatever
            // its already been routed for the other two algorithms by the way
            int pathClear = TRUE;
            for(x = 0; x < packetQueue->packet->length - 1; x++){
                int loadMax = adjMatrix[packetQueue->packet->packetPath[x]][packetQueue->packet->packetPath[x+1]]->maxLoad;
                int loadCurrent = adjMatrix[packetQueue->packet->packetPath[x]][packetQueue->packet->packetPath[x+1]]->currentLoad;
                if(DEBUG){printf("loadmax was %d and current load was %d...\n",loadMax,loadCurrent);}
                if(loadCurrent >= loadMax){
                    if(DEBUG){printf("packet failed!\n");}
                    // then the thing has failed!
                    // we have failed, so set to not increase everything
                    pathClear = FALSE;
                    break;
                }
            }
            if(pathClear == TRUE){
                // a packet succeeded! INCREMENT THE COUNTER
                if(DEBUG){printf("packet routed!\n");}
                packetSuccessCounter++;
                if(DEBUG){printf("is our length meant to be %d\n",packetQueue->packet->length);}
                // go through the route and load up the links each by 1
                // add up the total delay and the number hops to the global numbers while youre at it
            // ************* INCREMENT UP if we can *************
                for(x = 0; x + 1 < packetQueue->packet->length; x++){
                    // so now were going through the packet's path
                    // and we're updating the links
                    if(DEBUG2){printf("now incrementing the load of link between %c and %c\n",packetQueue->packet->packetPath[x] + ASCII,packetQueue->packet->packetPath[x+1] + ASCII);}
                    adjMatrix[packetQueue->packet->packetPath[x]][packetQueue->packet->packetPath[x+1]]->currentLoad++;
                    if(DEBUG2){printf("current load is %d\n",adjMatrix[packetQueue->packet->packetPath[x]][packetQueue->packet->packetPath[x+1]]->currentLoad);}
                    // update stats
                    totalDelay += adjMatrix[packetQueue->packet->packetPath[x]][packetQueue->packet->packetPath[x+1]]->distance;
                }
                // update stats
                totalHops += packetQueue->packet->length;
                // once we've added this, we have to add the packet's kill time to the queue...

                Node blahh = newNode(packetQueue->packet);
                blahh->next = NULL;
                packetQueue = addToQueue(blahh, packetQueue);
                blahh->packet->willDie = TRUE;
            }
        }

        // ************* DEATH **************
        // dying packets must have their burden removed from the appropriate links.
        else if(packetQueue->packet->willDie == TRUE){

//            if(DEBUG){printf("shit\n");}
            if(DEBUG){printf("killing a packet!\n");}
            // ************** INCREMENT DOWN EACH LINK *****************
            for(x = 0; x + 1 < packetQueue->packet->length; x++){
                    // so now were going through the packet's path
                    // and we're updating the links
                if(DEBUG){printf("now DECREASING the load of link between %c and %c\n",packetQueue->packet->packetPath[x] + ASCII,packetQueue->packet->packetPath[x+1] + ASCII);}
                adjMatrix[packetQueue->packet->packetPath[x]][packetQueue->packet->packetPath[x+1]]->currentLoad--;
                if(DEBUG2){
                    printf("current load %d\n", adjMatrix[packetQueue->packet->packetPath[x]][packetQueue->packet->packetPath[x+1]]->currentLoad);
                }
            }
        }
        packetQueue = popQueue(packetQueue);
    }
    // everything is free!

    // we're done!
    if(DEBUG){printf("\n\nMISSSHHON COMPLEEEE!\n\n\nDebriefing:\n");}
*/