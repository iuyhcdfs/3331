all:
	gcc -Werror -Wall -O -o RoutingPerformance RoutingPerformance.c -lm
	./RoutingPerformance CIRCUIT SHP topology1.txt workload1.txt 1
