all:
	gcc -Werror -Wall -O -o test RoutingPerformance.c
	./test CIRCUIT SHP topology1.txt workload1.txt 2
