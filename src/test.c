#include <stdlib.h>
#include <stdio.h>
#include "uthash.h"
#include <time.h>

struct my_struct {
	int id;                    /* key */
	char name[10];
	UT_hash_handle hh;         /* makes this structure hashable */
};

// 32 bit integer
typedef unsigned int uint32_t;

int oddrand (int min, int max)
{
   // This assumes end > start otherwise GIGO
	return ((rand() % ((max - min + min % 2) >> 1)
		   + ((min - min % 2) >> 1)) << 1) + 1;
}

int main(int argc, char *argv[]) {
	argc = argc;
	argv = argv;

	/* srand((unsigned)time(NULL)); */

	/* for (int i = 0; i < 1000; i++) { */
	/* 	// generates [1, 31], so >= min, < max */
	/* 	/\* int res = oddrand (1, 32); *\/ */
	/* 	/\* if (res == 32 || res == 0) *\/ */
	/* 	/\* 	printf ("STOP! %d\n", res); *\/ */
	/* 	printf ("%d\n", (rand()|1)); */
	/* } */

	/* uint32_t test = 3; */
	/* printf ("%zu\n", sizeof(test)); */
	int n = 250000;
	int *visited_nodes = malloc ((n-1) * sizeof(int));
	visited_nodes = visited_nodes;
	/* memset (visited_nodes, -1, n * sizeof(visited_nodes)); */
	for (int i = 0; i < n; i++) {
		visited_nodes[i] = -1;
		/* printf ("%d %d\n", i, visited_nodes[i]); */
	}

	return 0;
}
