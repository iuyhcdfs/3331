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
    long double distance;
    int maxLoad;
} _link;
typedef _link * Link;

// class: request for network load
typedef struct {
    long double timeToConnect; 
    char origin;
    char destination;
    long double timeToLive;
} _request;
typedef _request * Request;

// todo: shortest hop, shortest delay, least loaded, algorithms 
int routeSHP(Request request, Link link[]){
    return EXIT_SUCCESS;
}

int routeSDP(Request request, Link link[]) {
    return EXIT_SUCCESS;
}

int routeLLP(Request request, Link link[]) {
    return EXIT_SUCCESS;
}


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
   // Link linkArray[tCount];
   // Request requestArray[wCount];

    char * buffer2;
    // make structs for text
    tFile = fopen(topology_file, "rt");
    wFile = fopen(workload_file, "rt");
    while(EOF != fscanf(tFile, "%[^\n]\n", buffer)){
        printf("test a %s\n",buffer);
        buffer2 = buffer;
        buffer2 = strtok(buffer2, " ");
        printf("test b %s\n",buffer2);
        buffer2 = strtok(NULL, " ");
        printf("test c %s\n",buffer2);
        buffer2 = strtok(NULL, " ");
        printf("test d %s\n",buffer2);
        buffer2 = strtok(NULL, " ");
        printf("test e %s\n",buffer2);
    }

    
    printf("wat\n");
    // run algorithms, print out specific results in standard output
/*
    total number of virtual circuit requests: 200
    total number of packets: 4589
    number of successfully routed packets: 3654
    percentage of successfully routed packets: 79.63
    number of blocked packets: 935
    percentage of blocked packets: 20.37
    average number of hops per circuit: 5.42
    average cumulative propagation delay per circuit: 120.54
*/    
    
    return EXIT_SUCCESS;

}



