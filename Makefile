all:
	gcc -Werror -Wall -O -o RoutingPerformance RoutingPerformance.c -lm
	./RoutingPerformance CIRCUIT SHP topology2.txt workload2.txt 2
