all:
	gcc -Werror -Wall -O -o RoutingPerformance RoutingPerformance.c
	./RoutingPerformance CIRCUIT SHP topology1.txt workload1.txt 1
