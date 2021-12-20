#include "main.h"

/*
  indicates whether the algorithms works with 1 or 0 indexed data.
  If e.g. 1-indexed, the offset is 1 such that the backend (so how the data
  is stored) is 0-indexed. But ALL print out would still be 1-indexed as desired.
  The offset is measured in the input data, by finding the lowest value for the vertex id
 */
int offset = 0;

/**
 * help - a help utility
 * writing out a help utility to the user of the program
 * to be called with flag --help when executing the program
 */
void help () {
	printf ("To run, required arguments are as follows (separate each argument with a whitespace):\n\n");
	printf ("./main <algorithm> <inputfile> <outputfile> <k integer> <u integer> <v integer> <query times>\n");
	printf ("\nPossible input for each flag:\n\n");
	printf ("<algorithm>: dj, djopt, tz, bdj\n");
	printf ("<inputfile>: tests/USANY.txt (needs to be of DIMACS ssp file format) \n");
	printf ("<outputfile>: output.csv (will be generated automatically) \n");
	printf ("<k integer>: Any integer that satisfies k >= 1 \n");
	printf ("<u integer>: Any integer that represents a node from <inputfile> \n");
	printf ("<v integer>: Any integer that represents a node from <inputfile> \n");
	printf ("<query times>: Any positive integer, indicating how many times the Thorup-Zwick query alg. shall be executed\n\n");
}

/**
 * run_bdj - wrapper function for running bidirectional dijkstra from u to v
 * @graph: graph with vertices and edges
 * @u: source vertex u
 * @v: target vertex v
 * @n: number of vertices in graph
 * @m: number of edges in graph
 * Calls Bidirectional Dijkstra's algorithm, and measures the spent RAM and CPU time
 */
struct ssp_res *run_bdj (struct graph *graph, int u, int v, int n, int m)
{
	struct ssp_res *bdj;
	double cpu_time_spent = 0.0, avg_ins = 0.0, avg_dec = 0.0, avg_ext = 0.0;
	for (int i = 0; i < DIJKSTRA_TIMES; i++) {
		clock_t begin = clock();
		bdj = bidirectional_dijkstra (graph, u-offset, v-offset);
		clock_t end = clock();
		if (bdj == NULL) {
			perror ("No path (u, v) in djkstra_opt_alg could be found\n");
			exit (-1);
		}
		cpu_time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
		avg_ext += bdj->avg_extract_min_time /
			bdj->num_extract_min / MS;
		avg_dec += bdj->avg_decrease_key_time /
			bdj->num_decrease_key / MS;
		avg_ins += bdj->avg_min_heap_insert_time = bdj->avg_min_heap_insert_time /
			bdj->visited_nodes / MS;
	}
	bdj->memory_consump = get_vm_peak ();
	bdj->avg_extract_min_time = (avg_ext / (double)DIJKSTRA_TIMES);
	bdj->avg_decrease_key_time = (avg_dec / (double)DIJKSTRA_TIMES);
	bdj->avg_min_heap_insert_time = (avg_ins / (double)DIJKSTRA_TIMES);
	bdj->dist_time = (cpu_time_spent / DIJKSTRA_TIMES);
	bdj->visited_nodes_ratio = (double)bdj->visited_nodes/(double)(2*n);
	bdj->visited_edges_ratio = (double)bdj->visited_edges/(double)(2*m);

	bdj->query_times = DIJKSTRA_TIMES;

	printf ("\nResult of Bidirectional Dijkstra (%d, %d) = %d\n", u, v, bdj->dist);
	printf ("vertices n=%d, edges m=%d\n", n, m);
	printf ("Visiting ratio of vertices = %f, edges = %f\n",
			bdj->visited_nodes_ratio,
			bdj->visited_edges_ratio);
	printf ("%d extract-min operations. Avg time pr. operation: %.10f ms\n",
			bdj->num_extract_min, bdj->avg_extract_min_time);
	printf ("%d decrease-key operations. Avg time pr. operation: %.10f ms\n",
			bdj->num_decrease_key, bdj->avg_decrease_key_time);
	printf ("%d min-heap-insert operations. Avg time pr. operation: %.10f ms\n",
			bdj->visited_nodes, bdj->avg_min_heap_insert_time);
	printf ("Time spent on running Bidirectional Dijkstra (%d, %d) = %f sec\n", u, v, bdj->dist_time);
	printf ("Algorithm is executed %d times\n", bdj->query_times);
	printf ("Memory usage of Bidirectional Dijkstra = %d KB\n", bdj->memory_consump);

	return bdj;
}

/**
 * run_dijkstra - wrapper function for running dijkstra from source vertex
 * @graph: graph with vertices and edges
 * @u: source vertex u
 * @v: target vertex v
 * @n: number of vertices in graph
 * @m: number of edges in graph
 * Calls Dijkstra's algorithm, and measures the spent RAM and CPU time
 */
struct ssp_res *run_dijkstra (struct graph *graph, int u, int v, int n, int m)
{
	struct ssp_res *dijkstra;
	double cpu_time_spent = 0.0, avg_dec = 0.0, avg_ext = 0.0;
	for (int i = 0; i < DIJKSTRA_TIMES; i++) {
		clock_t begin = clock();
		dijkstra = dijkstra_alg (graph, u-offset);
		clock_t end = clock();
		if (dijkstra == NULL) {
			perror ("No path (u, v) in djkstra_opt_alg could be found\n");
			exit (-1);
		}
		cpu_time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
		avg_ext += dijkstra->avg_extract_min_time /
			dijkstra->visited_nodes / MS;
		avg_dec += dijkstra->avg_decrease_key_time /
			dijkstra->num_decrease_key / MS;
	}
	dijkstra->memory_consump = get_vm_peak ();
	dijkstra->avg_extract_min_time = (avg_ext / (double)DIJKSTRA_TIMES);
	dijkstra->avg_decrease_key_time = (avg_dec / (double)DIJKSTRA_TIMES);
	dijkstra->dist_time = (cpu_time_spent / DIJKSTRA_TIMES);
	dijkstra->visited_nodes_ratio = (double)dijkstra->visited_nodes/(double)n;
	dijkstra->visited_edges_ratio = (double)dijkstra->visited_edges/(double)m;

	dijkstra->dist = dijkstra->S_f[v-offset].sp_est;

	dijkstra->query_times = DIJKSTRA_TIMES;

	printf ("\nResult of Dijkstra's algorithm (%d, %d) = %d\n", u, v,
			dijkstra->dist);
	printf ("vertices n=%d, edges m=%d\n", n, m);
	printf ("Visiting ratio of vertices = %f, edges = %f\n",
			dijkstra->visited_nodes_ratio,
			dijkstra->visited_edges_ratio);
	printf ("%d extract-min operations. Avg time pr. operation: %.10f ms\n",
			dijkstra->num_extract_min, dijkstra->avg_extract_min_time);
	printf ("%d decrease-key operations. Avg time pr. operation: %.10f ms\n",
			dijkstra->num_decrease_key, dijkstra->avg_decrease_key_time);
	printf ("%d min-heap-insert operations. Avg time pr. operation: %.1f ms\n",
			0, 0.0);
	printf ("Time spent on running Dijkstra's algorithm (%d, %d) = %f sec\n",
			u, v, dijkstra->dist_time);
	printf ("Algorithm is executed %d times\n", dijkstra->query_times);
	printf ("Memory usage of Dijkstra's algorithm = %d KB\n", dijkstra->memory_consump);

	return dijkstra;
}

/**
 * run_dijkstra - wrapper function for running dijkstra from source vertex
 * @graph: graph with vertices and edges
 * @u: source vertex u
 * @v: target vertex v
 * @n: number of vertices in graph
 * @m: number of edges in graph
 * Calls Dijkstra's algorithm, and measures the spent RAM and CPU time
 */
struct ssp_res *run_opt_dijkstra (struct graph *graph, int u, int v, int n, int m)
{
	struct ssp_res *dijkstra;
	double cpu_time_spent = 0.0, avg_ins = 0.0, avg_dec = 0.0, avg_ext = 0.0;
	for (int i = 0; i < DIJKSTRA_TIMES; i++) {
		clock_t begin = clock();
		dijkstra = dijkstra_opt_alg (graph, u-offset, v-offset);
		clock_t end = clock();
		if (dijkstra == NULL) {
			perror ("No path (u, v) in dijkstra_opt_alg could be found\n");
			exit (-1);
		}
		cpu_time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
		avg_ins += dijkstra->avg_min_heap_insert_time /
			dijkstra->visited_nodes / MS;
		avg_ext += dijkstra->avg_extract_min_time /
			dijkstra->num_extract_min / MS;
		avg_dec += dijkstra->avg_decrease_key_time /
			dijkstra->num_decrease_key / MS;
	}
	dijkstra->memory_consump = get_vm_peak ();
	dijkstra->avg_min_heap_insert_time = (avg_ins / (double)DIJKSTRA_TIMES);
	dijkstra->avg_extract_min_time = (avg_ext / (double)DIJKSTRA_TIMES);
	dijkstra->avg_decrease_key_time = (avg_dec / (double)DIJKSTRA_TIMES);
	dijkstra->dist_time = (cpu_time_spent / DIJKSTRA_TIMES);
	dijkstra->visited_nodes_ratio = (double)dijkstra->visited_nodes/(double)n;
	dijkstra->visited_edges_ratio = (double)dijkstra->visited_edges/(double)m;

	dijkstra->dist = dijkstra->S_f[v-offset].sp_est;

	dijkstra->query_times = DIJKSTRA_TIMES;

	printf ("\nResult of optimised Dijkstra's algorithm (%d, %d) = %d\n", u, v,
			dijkstra->dist);
	printf ("vertices n=%d, edges m=%d\n", n, m);
	printf ("Visiting ratio of vertices = %f, edges = %f\n",
			dijkstra->visited_nodes_ratio, dijkstra->visited_edges_ratio);
	printf ("%d extract-min operations. Avg time pr. operation: %.10f ms\n",
			dijkstra->num_extract_min, dijkstra->avg_extract_min_time);
	printf ("%d decrease-key operations. Avg time pr. operation: %.10f ms\n",
			dijkstra->num_decrease_key, dijkstra->avg_decrease_key_time);
	printf ("%d min-heap-insert operations. Avg time pr. operation: %.10f ms\n",
			dijkstra->visited_nodes, dijkstra->avg_min_heap_insert_time);
	printf ("Time spent on running optimised Dijkstra (%d, %d) = %f sec\n",
			u, v, dijkstra->dist_time);
	printf ("Algorithm is executed %d times\n", dijkstra->query_times);
	printf ("Memory usage of optimised Dijkstra = %d KB\n", dijkstra->memory_consump);

	return dijkstra;
}

/**
 * run_tz - wrapper function for running Thorup-Zwick
 * @graph: graph with vertices and edges
 * @k: k integer
 * @u: source vertex u
 * @v: target vertex v
 * Calls Thorup-Zwick algorithm, and measures the spent RAM and CPU time
 * First it calls prepro, the preprocessing algorithm and then calls dist,
 * the query algorithm
 */
struct tz_res *run_tz (struct graph *graph, int k, int u, int v, int n, int m, int query_times)
{
	struct tz_res *tz = malloc (sizeof (struct tz_res));
	struct prepro *pp = malloc (sizeof (struct prepro));
	clock_t begin, end;

	tz->dist = 0, tz->dist_time = 0.0;
	tz->query_times = query_times;

	begin = clock();
	pp->success = false;
	while (!pp->success) {
	  pp = prepro (graph, k);
	}
	end = clock();
	tz->prepro_time = (double)(end - begin) / CLOCKS_PER_SEC;
	tz->prepro_memory_consump = get_vm_peak();

	for (int i = 0; i < tz->query_times; i++) {
		begin = clock();
		tz->dist += dist (&pp->nodes[u-offset], &pp->nodes[v-offset], pp->bunchlist);
		end = clock();
		tz->dist_time += (double)(end - begin) / CLOCKS_PER_SEC;
	}

	tz->dist = tz->dist / tz->query_times;
	tz->dist_time = tz->dist_time / tz->query_times;
	tz->dist_memory_consump += pp->bunchlist->bunch_size / 1000;
	tz->k = k;

	printf ("Time spent on prepro k=%d Thorup-Zwick: %f\n", tz->k, tz->prepro_time);
	printf ("vertices n=%d, edges m=%d\n", n, m);
	printf ("Memory usage of prepro = %d KB\n", tz->prepro_memory_consump);
	printf ("Result of Thorup-Zwick dist(%d, %d) = %d\n", u, v, tz->dist);
	printf ("Time spent on dist Thorup-Zwick: %f sec\n", tz->dist_time);
	printf ("Query algorithm is executed %d times\n", tz->query_times);
	printf ("Memory usage of dist (bunch size) = %d KB\n", tz->dist_memory_consump);

	begin = clock();
	struct ssp_res *test = astar (graph, pp, u-offset, v-offset);
	end = clock();
	test->dist = test->S_f[v-offset].sp_est;
	printf ("Result of A*(%d,%d)=%d in time %f sec\n", u, v, test->dist, ((double)(end - begin) / CLOCKS_PER_SEC));
	return tz;
}

int main (int argc, char *argv[])
{

	if (argc == 2 && strcmp ("--help", argv[1]) == 0) {
		help ();
	} else if ((argc-1) < MIN_REQUIRED || (((argc-1) % MIN_REQUIRED) != 0)) {
		printf ("No input and output arguments or input/output does not match!\n");
		printf ("Number of arguments is %d\n\n", argc);
		help ();
		return EXIT_FAILURE;
	} else {
		for (int i = 1; i < argc; i += MIN_REQUIRED) {
			const char *fname_read = argv[i+1];
			offset = read_offset_in_file (fname_read);
			const char *fname_write = argv[i+2];
			const int u = atoi(argv[i+4]);
			const int v = atoi(argv[i+5]);
			const int query_times = atoi(argv[i+6]);
			struct graph_data *gd = read_vertices_and_edges (fname_read);
			if (u > gd->n || v > gd->n) {
				printf ("Source vertex u or/and target vertex v is/are invalid\n");
				printf ("Please consider a valid vertex that is <= n\n\n");
				help ();
				return EXIT_FAILURE;
			}
			struct graph *graph = init_graph (gd->n);
			read_from_file (graph, fname_read, offset);
			if (strcmp ("tz", argv[i]) == 0) {
				const int k = atoi(argv[i+3]);
				if (k <= 0) {
					printf ("k is <= 0! Please set k >= 1\n");
					help ();
					return EXIT_FAILURE;
				}
				struct tz_res *tz = run_tz (graph, k, u, v, gd->n, gd->m, query_times);
				write_to_csv (fname_write, fname_read, gd->n, gd->m, u, v,
							  tz, NULL, NULL, NULL);
			} else if (strcmp ("dj", argv[i]) == 0) {
				struct ssp_res *dijkstra = run_dijkstra (graph, u, v, gd->n, gd->m);
				write_to_csv (fname_write, fname_read, gd->n, gd->m, u, v,
							  NULL, dijkstra, NULL, NULL);
			} else if (strcmp ("djopt", argv[i]) == 0) {
				struct ssp_res *djopt = run_opt_dijkstra (graph, u, v, gd->n, gd->m);
				write_to_csv (fname_write, fname_read, gd->n, gd->m, u, v,
							  NULL, NULL, djopt, NULL);
			} else if (strcmp ("bdj", argv[i]) == 0) {
				struct ssp_res *bdj = run_bdj (graph, u, v, gd->n, gd->m);
				write_to_csv (fname_write, fname_read, gd->n, gd->m, u, v,
							  NULL, NULL, NULL, bdj);
			} else {
				printf ("The algorithm is not known!\n\n");
				help ();
			}
		};
	}
	return EXIT_SUCCESS;
}
