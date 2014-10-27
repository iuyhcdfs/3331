/*
	make virtual network
	test simple routing algorithms on virtual network
	dont forget the report!
   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node{
    Node * neighbours[];
    int * distances[];
    char * name;
} Node;

int main (int argc, char* argv[]) {
    // Process args and store their values - 5 args
    char * network_scheme = argv[1];
    char * routing_scheme = argv[2];
    char * topology_file = argv[3];
    char * workload_file = argv[4];
    int packet_rate = atoi(argv[5]);
    


    printf("%s %s %s %s %d\n", network_scheme, routing_scheme, topology_file, workload_file, packet_rate);


    return EXIT_SUCCESS;
}


