#include <sys/resource.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* A process's resident set size is the amount of memory that belongs to it and is currently present (resident) in RAM (real RAM, not swapped or otherwise not-resident). */

/* For instance, if a process allocates a chunk of memory (say 100Mb) and uses it actively (reads/writes to it), its resident set size will be about 100Mb (plus overhead, the code segment, etc.). If after the process then stops using (but doesn't release) that memory for a while, the OS could opt to swap chunks of that memory to swap, to make room for other processes (or cache). The resident set size would then decrease by the amount the kernel swapped out. If the process wakes up and starts re-using that memory, the kernel would re-load the data from swap, and the resident set size would go up again. */

/* The ru_maxrss field of struct rusage is the "high water mark" for the resident set size. It indicates the peak RAM use for this process (when using RUSAGE_SELF). */

/* You can limit a process's resident set size to avoid having a single application "eat up" all the RAM on your system and forcing other applications to swap (or fail entirely with out-of-memory conditions). */

typedef struct {
	unsigned long size,resident,share,text,lib,data,dt;
} statm_t;

void read_off_memory_status()
{
	statm_t result;
	const char* statm_path = "/proc/self/statm";

  FILE *f = fopen(statm_path,"r");
  if(!f){
	perror(statm_path);
	abort();
  }
  if(7 != fscanf(f,"%ld %ld %ld %ld %ld %ld %ld",
	&result.size,&result.resident,&result.share,&result.text,&result.lib,&result.data,&result.dt))
  {
	perror(statm_path);
	abort();
  }
  printf ("%ld %ld %ld %ld %ld %ld %ld\n",
		  result.size, result.resident, result.share, result.text, result.lib, result.data, result.dt);
  fclose(f);
}


// cat /proc/self/statm
int main () {

	/* FILE* status = fopen( "/proc/self/status", "r" ); */
	read_off_memory_status ();
	/* int i = 0; */
	/* struct rusage r_usage; */
	/* while (++i <= 10) { */
	/* 	void *m = malloc(20*1024*1024); */
	/* 	memset(m,0,20*1024*1024); */
	/* 	getrusage(RUSAGE_SELF,&r_usage); */
	/* 	printf("Memory usage = %ld\n",r_usage.ru_maxrss); */
	/* } */
	/* printf("\nAllocated memory, sleeping ten seconds after which we will check again...\n\n"); */
	/* sleep (1); */
	/* getrusage(RUSAGE_SELF,&r_usage); */
	/* printf("Memory usage = %ld KB\n", r_usage.ru_maxrss); */

  return 0;
}

/* #include <stdlib.h> */
/* #include <stdio.h> */
/* #include "uthash.h" */
/* #include <time.h> */

/* struct my_struct { */
/* 	int id;                    /\* key *\/ */
/* 	char name[10]; */
/* 	UT_hash_handle hh;         /\* makes this structure hashable *\/ */
/* }; */

/* // 32 bit integer */
/* typedef unsigned int uint32_t; */

/* int oddrand (int min, int max) */
/* { */
/*    // This assumes end > start otherwise GIGO */
/* 	return ((rand() % ((max - min + min % 2) >> 1) */
/* 		   + ((min - min % 2) >> 1)) << 1) + 1; */
/* } */



/* int main(int argc, char *argv[]) { */
/* 	argc = argc; */
/* 	argv = argv; */

/* 	/\* srand((unsigned)time(NULL)); *\/ */

/* 	/\* for (int i = 0; i < 1000; i++) { *\/ */
/* 	/\* 	// generates [1, 31], so >= min, < max *\/ */
/* 	/\* 	/\\* int res = oddrand (1, 32); *\\/ *\/ */
/* 	/\* 	/\\* if (res == 32 || res == 0) *\\/ *\/ */
/* 	/\* 	/\\* 	printf ("STOP! %d\n", res); *\\/ *\/ */
/* 	/\* 	printf ("%d\n", (rand()|1)); *\/ */
/* 	/\* } *\/ */

/* 	/\* uint32_t test = 3; *\/ */
/* 	/\* printf ("%zu\n", sizeof(test)); *\/ */
/* 	int n = 250000; */
/* 	int *visited_nodes = malloc ((n-1) * sizeof(int)); */
/* 	visited_nodes = visited_nodes; */
/* 	/\* memset (visited_nodes, -1, n * sizeof(visited_nodes)); *\/ */
/* 	for (int i = 0; i < n; i++) { */
/* 		visited_nodes[i] = -1; */
/* 		/\* printf ("%d %d\n", i, visited_nodes[i]); *\/ */
/* 	} */

/* 	return 0; */
/* } */
