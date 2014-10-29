/*
//	make virtual network
//	test simple routing algorithms on virtual network
//	dont forget the report!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// class: connection between two nodes
typedef struct {
    char end1;
    char end2;
    int distance;
    int maxLoad;
} _link;
typedef _link * Link;
// malloc-er
Link newLink(void);

// class: request for network load
typedef struct {
    double timeToConnect; 
    char origin;
    char destination;
    double timeToLive;
} _request;
typedef _request * Request;
// malloc-er
Request newRequest(void);

// ALGORITHMS FOR ROUTING PACKETS OR CIRCUITS
int routeSHP(Request request, Link link[]){
    return EXIT_SUCCESS;
}
int routeSDP(Request request, Link link[]) {
    return EXIT_SUCCESS;
}
int routeLLP(Request request, Link link[]) {
    return EXIT_SUCCESS;
}

// printing functions
void printAllLinks(Link * linkArray, int linkSize);
void printAllRequests(Request * requestArray, int requestSize);

// MAIN
int main (int argc, char* argv[]) {
    // Process args and store their values - 5 args
    char * network_scheme = argv[1];
    char * routing_scheme = argv[2];
    char * topology_file = argv[3];
    char * workload_file = argv[4];
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
    while(EOF != fscanf(tFile, "%[^\n]\n", buffer)){
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

    // print out things 
    /*
    printAllLinks(linkArray, lArrayCount);
    printAllRequests(requestArray, rArrayCount);
    printf("files read!s\n");
    */

    // run algorithms, store results 

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



// Function implementation

void printAllLinks(Link * linkArray, int linkSize) {
    for (int i=0; i < linkSize; i++) {
        printf("\nLink %d: \n", i);
        printf("End1:    %c\nEnd2:    %c\nDistance:%d\nmaxLoad :%d\n\n",
            linkArray[i]->end1,
            linkArray[i]->end2,
            linkArray[i]->distance,
            linkArray[i]->maxLoad);
    }
}

void printAllRequests(Request * requestArray, int requestSize){
    for (int i=0; i < requestSize; i++) {
        printf("\nRequest %d: \n", i);
        printf("TTC:     %f\nOrigin:  %c\nDest:    %c\nTTL:     %f\n\n",
            requestArray[i]->timeToConnect,
            requestArray[i]->origin,
            requestArray[i]->destination,
            requestArray[i]->timeToLive);
    }
}

Link newLink(void){
    Link temp = malloc(sizeof(_link));
    temp->end1 = '0';
    temp->end2 = '0';
    temp->distance = 0;
    temp->maxLoad = 0;
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

