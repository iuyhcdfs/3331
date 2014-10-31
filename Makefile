all:
	gcc -Werror -Wall -O -o RoutingPerformance RoutingPerformance.c -lm
	./RoutingPerformance CIRCUIT SHP topology.txt workload.txt 1
