/*
//	make virtual network
//	test simple routing algorithms on virtual network
//	dont forget the report!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// todo: shortest hop, shortest delay, least loaded, algorithms 
int routeSHP();
int routeSDP();
int routeLLP();

// class: word for convenience
typedef struct {
    char * text;
    int length;
} _word;
typedef *_word Word;

// class: request for network load
typedef struct {

} _request;
typedef *_request Request;

// input a string get the index of the array it is meant to be
int getStringIndex(Word wordList[], int listLength, char* string){
    // start w/ failure condition
    int result = -1
    // scroll until we find a match
    // assumed the list has only unique entries
    for(int x = 0; x < listLength; x++){
        if(strcmp(wordList[x]->text, string)){
            result = x;
        }
    } 
    return result;
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

    char buffer[100];
    // find number of lines in file
    char * names[50];

    int nameCount = 0;
    while(EOF != fscanf(tFile, "%[^\n]\n", buffer)){
        printf("%s\n",buffer);
        for(){

        }
    }

    // create adjacency matrix for connections, -1 means not connected
    int networkMap[nameCount][nameCount];
    for(int x = 0; x < nameCount; x++){
        for(int y = 0; y < nameCount; y++){
            networkMap[x][y] = -1;
        }
    }
    printf("%d\n",networkMap[4][4]);
    getStringIndex(names, 4, buffer);

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



