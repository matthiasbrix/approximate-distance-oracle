#include "main.h"

#define MIN_REQUIRED 6
int offset = 0;

// Skriv at float kan også lade sig gøre
// for test graphs: http://www.info.univ-angers.fr/pub/porumbel/graphs/index.html#vss
// generator: http://illuminations.nctm.org/Activity.aspx?id=3550
// TODO: gdb:gcc -g -o prog myfile.c another.c
// gdb prog
// Skelne mellme køretiden af prepro og dist, samt pladsforbrug
// VIGTIGT: Kør tz og dijkstra begge med samme (u, v) og så sammenlign sp res, køretid og plads forbrug
//
/*
CLOCKS_PER_SEC is a constant which is declared in time.h. To get the CPU time used by a task within a C application,
returns the amount of time the OS has spent running your process, and not the actual amount of time elapsed. However, this is fine for timing a block of code, but not measuring time elapsing in the real world
Fx. tænk på sleep - CPUen arbejder så ikke, således vil den ikke tælle det med
*/
// TODO: tid, plads, og hvor tæt kommer jeg på Dijkstras (SSP)
// tid, plads skal være hardware.
// Stemmer ekspirementerne overens med teori?
// du skal dog nævne i din rapport, hvilken hash table du bruger
// Skriv i rapport om at man kan udvide med floats
// Hash table for en knude v skal indeholde netop de knuder w, der tilhører B(v), og disse knuder w er netop dem, hvor v tilhører C(w).
// Dijkstra requies no extra space!!
// HVilken compiler, CPU, RAM osv jeg bruger.
// større k, mindre pladsforbrug
// Mange kanter contra få kanter, kig også på forskellige k og knude mængder
// Hvad der sker med prepro og query
// HVorfor vil thorup-zwick ikke være lige så "rigtig" som normal Dijkstra?
// http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
// TODO: Læs om hash funktionen af uthash
// TODO: for every v in V, generate bunches B(v) from all C(w)
// Checks whether w in B(v) and retrieves d(w, v)
// TODO: Skal det være en 2-level table med den foreslående hash funktion?
// TODO: Husk også at gemme afstanden i Bunches
// TODO: Have all pointer be with the variable name
// Bunche gen. er som DFS? (spørg ellers Christian)
// Skriv til uthash om hash funktioner
// hvilke eksperimenter og hvorfor. Vis så resultater.
// TODO: SKal jeg genindlæs grafen når jeg kører dijkstra? Skal jeg overhovedet måle indlæsningen af knuderne?
// Skriv i rapporten hvad der sker når grafen ikke er sammenhængende...
// cat /proc/cpuinfo
// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

void test_prepro ()
{
	int k, n, u, v, d;
	double cpu_time_spent;
	k = 3, n = 5, u = 1, v = 4;
	struct graph *graph = init_graph (n);
	add_edges (graph, 0, 1, 10);
	add_edges (graph, 0, 2, 5);
	add_edges (graph, 0, 4, 7);
	add_edges (graph, 1, 2, 2);
	add_edges (graph, 1, 3, 1);
	add_edges (graph, 2, 3, 9);
	add_edges (graph, 2, 4, 2);
	add_edges (graph, 3, 4, 4);

	clock_t begin = clock();
	printf ("Memory before running prepro = %d KB %d KB\n", get_vm_peak(), get_current_vm ());
	struct prepro *pp = malloc (sizeof (struct prepro));
	pp->success = false;
	while (!pp->success) {
		pp = prepro (graph, k);
		sleep (1);
	}
	clock_t end = clock();
	cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf ("Time spent on prepro Thorup-Zwick: %f\n", cpu_time_spent);
	printf ("Memory usage of prepro = %d KB %d KB\n", get_vm_peak(), get_current_vm ());

	clock_t begin2 = clock();
	d = dist (&pp->nodes[u], &pp->nodes[v], pp->bunchlist);
	clock_t end2 = clock();
	cpu_time_spent = (double)(end2 - begin2) / CLOCKS_PER_SEC;
	printf ("Time spent on query Thorup-Zwick: %f\n", cpu_time_spent);
	printf ("Result of Thorup-Zwick dist (%d, %d) = %d\n", u, v, d);
	printf ("Memory usage of dist = %d KB %d KB\n", get_vm_peak(), get_current_vm ());

	graph = init_graph (n);
	add_edges (graph, 0, 1, 10);
	add_edges (graph, 0, 2, 5);
	add_edges (graph, 0, 4, 7);
	add_edges (graph, 1, 2, 2);
	add_edges (graph, 1, 3, 1);
	add_edges (graph, 2, 3, 9);
	add_edges (graph, 2, 4, 2);
	add_edges (graph, 3, 4, 4);

	clock_t begin3 = clock();
	struct node *S = dijkstra_alg (graph, u);
	clock_t end3 = clock();
	cpu_time_spent = (double)(end3 - begin3) / CLOCKS_PER_SEC;
	printf ("Result of Dijkstra SSP (%d, %d) = %d\n", u, v, S[v-offset].sp_est);
	printf ("Time spent on running Dijkstra: %f\n", cpu_time_spent);
	printf ("Memory usage of Dijkstra = %d KB %d KB\n", get_vm_peak(), get_current_vm ());

	graph = init_graph (n);
	add_edges (graph, 0, 1, 10);
	add_edges (graph, 0, 2, 5);
	add_edges (graph, 0, 4, 7);
	add_edges (graph, 1, 2, 2);
	add_edges (graph, 1, 3, 1);
	add_edges (graph, 2, 3, 9);
	add_edges (graph, 2, 4, 2);
	add_edges (graph, 3, 4, 4);
	printf ("BIDIRECTIONAL DIJKSTRA %d\n", bidirectional_dijkstra (graph, u, v));

	return;
}

// Hardcoded tests from various sources
void hardcoded_tests ()
{
	// From the author
	struct graph* graph_1 = init_graph (5);
	add_edges (graph_1, 0, 1, 3);
	add_edges (graph_1, 0, 2, 5);
	add_edges (graph_1, 1, 2, 1);
	add_edges (graph_1, 2, 3, 10);
	add_edges (graph_1, 3, 4, 2);
	/* struct node *S_1 = dijkstra_alg (graph_1, 0); */
	/* pp_nodes (S_1, 5); */
	int out;
	out = bidirectional_dijkstra (graph_1, 0, 4);
	printf ("out: %d\n", out);
	pp_graph (graph_1);

	printf ("------------------------\n");
	// From: http://www.geeksforgeeks.org/greedy-algorithms-set-7-dijkstras-algorithm-for-adjacency-list-representation/
	struct graph* graph = init_graph (9);
	add_edges(graph, 0, 1, 4);
	add_edges(graph, 0, 7, 8);
	add_edges(graph, 1, 2, 8);
	add_edges(graph, 1, 7, 11);
	add_edges(graph, 2, 3, 7);
	add_edges(graph, 2, 8, 2);
	add_edges(graph, 2, 5, 4);
	add_edges(graph, 3, 4, 9);
	add_edges(graph, 3, 5, 14);
	add_edges(graph, 4, 5, 10);
	add_edges(graph, 5, 6, 2);
	add_edges(graph, 6, 7, 1);
	add_edges(graph, 6, 8, 6);
	add_edges(graph, 7, 8, 7);
	/* pp_graph (graph); */
	/* struct node *S_2 = dijkstra_alg (graph, 3); */
	out = bidirectional_dijkstra (graph, 0, 1);
	printf ("geeks: out: 1 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 2);
	printf ("geeks: out: 2 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 3);
	printf ("geeks: out: 3 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 4);
	printf ("geeks: out: 4 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 5);
	printf ("geeks: out: 5 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 6);
	printf ("geeks: out: 6 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 7);
	printf ("geeks: out: 7 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 8);
	printf ("geeks: out: 8 %d\n", out);
	/* printf ("------------------------\n"); */

	/* // From CLRS p. 659 (though it is directed) */
	struct graph* graph_3 = init_graph (5);
	add_edges (graph_3, 0, 1, 10);
	add_edges (graph_3, 0, 2, 5);
	add_edges (graph_3, 0, 4, 7);
	add_edges (graph_3, 1, 2, 2);
	add_edges (graph_3, 1, 3, 1);
	add_edges (graph_3, 2, 3, 9);
	add_edges (graph_3, 2, 4, 2);
	add_edges (graph_3, 3, 4, 4);
	/* pp_graph (graph_3); */
	/* pp_graph (graph_3); */
	struct node *S_3 = dijkstra_alg (graph_3, 0);
	pp_nodes (S_3, 5);
	out = bidirectional_dijkstra (graph_3, 0, 1);
	printf ("out: %d\n", out);
	out = bidirectional_dijkstra (graph_3, 0, 2);
	printf ("out: %d\n", out);
	out = bidirectional_dijkstra (graph_3, 0, 3);
	printf ("out: %d\n", out);
	out = bidirectional_dijkstra (graph_3, 0, 4);
	printf ("out: %d\n", out);

	// http://liu.diva-portal.org/smash/get/diva2:114159/FULLTEXT01 (p. 26)
	struct graph* graph_5 = init_graph (6);
	add_edges (graph_5, 0, 1, 3);
	add_edges (graph_5, 0, 2, 1);
	add_edges (graph_5, 1, 3, 1);
	add_edges (graph_5, 2, 4, 4);
	add_edges (graph_5, 4, 5, 1);
	pp_graph (graph_5);
	out = bidirectional_dijkstra (graph_5, 0, 5);
	printf ("her: out: %d\n", out);

	struct graph* graph_4 = init_graph (3);
	add_edges (graph_4, 0, 1, 1);
	add_edges (graph_4, 1, 2, 1);
	add_edges (graph_4, 0, 2, 5);
	pp_graph (graph_4);
	out = bidirectional_dijkstra (graph_4, 0, 2);
	printf ("out: %d\n", out);

	struct graph* graph_6 = init_graph (9);
	add_edges (graph_6, 0, 1, 4);
	add_edges (graph_6, 0, 2, 5);
	add_edges (graph_6, 0, 3, 7);
	add_edges (graph_6, 3, 5, 4);
	add_edges (graph_6, 3, 6, 6);
	add_edges (graph_6, 1, 4, 9);
	add_edges (graph_6, 2, 5, 3);
	add_edges (graph_6, 5, 7, 5);
	add_edges (graph_6, 5, 4, 12);
	add_edges (graph_6, 7, 8, 3);
	add_edges (graph_6, 4, 8, 13);
	add_edges (graph_6, 6, 8, 9);
	pp_graph (graph_6);
	out = bidirectional_dijkstra (graph_6, 1, 6);
	printf ("out: %d\n", out);
	struct node *S = dijkstra_alg (graph_6, 1);
	pp_nodes (S, 9);
	exit (0);
}

/**
 * help - a help utility
 * writing out a help utility to the user of the program
 * to be called with flag --help when executing the program
 */
void help () {
	printf ("To run, required arguments are as follows:\n");
	printf ("./main <algorithm> <inputfile> <outputfile> <k integer> <u integer> <v integer>\n");
	printf ("\nPossible input for each flag:\n\n");
	printf ("<algorithm>: <dj>, <tz>, <bdj>\n");
	printf ("<inputfile>: tests/USANY.txt (needs to be of DIMACS ssp file format) \n");
	printf ("<outputfile>: output.csv (will be generated automatically) \n");
	printf ("<k integer>: Any integer that satisfies k >= 1 \n");
	printf ("<u integer>: Any integer that represents a node from <inputfile> \n");
	printf ("<v integer>: Any integer that represents a node from <inputfile> \n\n");
	return;
}

/**
 * run_bdj - wrapper function for running bidirection dijkstra from u to v
 * @graph: graph with vertices and edges
 * @u: source vertex u
 * @v: target vertex v
 * Calls Bidirectional Dijkstra's algorithm, and measures the spent RAM and CPU time
 */
struct dijkstra_res *run_bdj (struct graph *graph, int u, int v)
{
	struct dijkstra_res *bdj = malloc (sizeof (struct dijkstra_res));
	clock_t begin = clock();
	int dist = bidirectional_dijkstra (graph, u-offset, v-offset);
	clock_t end = clock();
	double cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	bdj->dist = dist;
	bdj->dist_time = cpu_time_spent;
	bdj->memory_consump = get_vm_peak ();
	printf ("\nResult of Bidirectional Dijkstra (%d, %d) = %d\n", u, v, bdj->dist);
	printf ("Time spent on running Bidirectional Dijkstra (%d, %d): %f\n", u, v, bdj->dist_time);
	printf ("Memory usage of Bidirectional Dijkstra = %d KB\n", bdj->memory_consump);

	return bdj;
}

/**
 * run_dijkstra - wrapper function for running dijkstra from source vertex
 * @graph: graph with vertices and edges
 * @u: source vertex u
 * @v: target vertex v
 * Calls Dijkstra's algorithm, and measures the spent RAM and CPU time
 */
struct dijkstra_res *run_dijkstra (struct graph *graph, int u, int v)
{
	struct dijkstra_res *dijkstra = malloc (sizeof (struct dijkstra_res));
	clock_t begin = clock();
	struct node *S = dijkstra_alg (graph, u-offset);
	clock_t end = clock();
	double cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	dijkstra->dist = S[v-offset].sp_est;
	dijkstra->dist_time = cpu_time_spent;
	dijkstra->memory_consump = get_vm_peak ();
	printf ("\nResult of Dijkstra SSP (%d, %d) = %d\n", u, v, S[v-offset].sp_est);
	printf ("Time spent on running Dijkstra (%d, %d): %f\n", u, v, dijkstra->dist_time);
	printf ("Memory usage of Dijkstra = %d KB\n", dijkstra->memory_consump);

	return dijkstra;
}

/**
 * run_tz - wrapper function for running Thorup-Zwick
 * @graph: graph with vertices and edges
 * @k: k integer
 * @u: source vertex u
 * @v: target vertex v
 * Calls Thorup-Zwick algorithm, and measures the spent RAM and CPU time
 * First it calls prepro, the preprocessing algorithm and then calls dist
 * the query algorithm
 */
struct tz_res *run_tz (struct graph *graph, int k, int u, int v)
{
	struct tz_res *tz = malloc (sizeof (struct tz_res));
	struct prepro *pp = malloc (sizeof (struct prepro));
	clock_t begin, end;
	double cpu_time_spent;

	begin = clock();
	pp->success = false;
	while (!pp->success) {
		pp = prepro (graph, k);
		sleep (1);
	}
	end = clock();
	cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	tz->prepro_time = cpu_time_spent;
	tz->prepro_memory_consump = get_vm_peak();
	printf ("Time spent on prepro Thorup-Zwick: %f\n", tz->prepro_time);
	printf ("Memory usage of prepro = %d KB\n", tz->prepro_memory_consump);

	begin = clock();
	int d = dist (&pp->nodes[u-offset], &pp->nodes[v-offset], pp->bunchlist);
	end = clock();
	cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	tz->dist_memory_consump = pp->bunchlist->bunch_size / 1000;
	tz->dist_time = cpu_time_spent;
	printf ("\nResult of Thorup-Zwick dist(%d, %d) = %d\n", u, v, d);
	printf ("Time spent on dist Thorup-Zwick: %f\n", tz->dist_time);
	printf ("Memory usage of dist (bunch size) = %d KB\n", tz->dist_memory_consump);
	tz->dist = d;
	tz->k = k;

	return tz;
}

int main (int argc, char *argv[])
{
	if (argc == 1) {
		hardcoded_tests ();
		/* test_prepro (); */
		return EXIT_SUCCESS;
	}
	if (strcmp ("--help", argv[1]) == 0) {
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
			struct graph_data *gd = read_vertices_and_edges (fname_read);
			if (u > gd->n || v > gd->n) {
				printf ("Source vertex u or/and target vertex v is/are invalid\n");
				printf ("Please consider a valid vertex that is < n");
				help ();
				return EXIT_FAILURE;
			}
			struct graph *graph = init_graph (gd->n);
			read_from_file (graph, fname_read);
			if (strcmp ("tz", argv[i]) == 0) {
				const int k = atoi(argv[i+3]);
				if (k <= 0) {
					printf ("k is <= 0! Please set k >= 1\n");
					help ();
					return EXIT_FAILURE;
				}
				struct tz_res *tz = run_tz (graph, k, u, v);
				write_to_csv (fname_write, fname_read, gd->n, gd->m, u, v, tz, NULL, NULL);
			} else if (strcmp ("dj", argv[i]) == 0) {
				struct dijkstra_res *dijkstra = run_dijkstra (graph, u, v);
				write_to_csv (fname_write, fname_read, gd->n, gd->m, u, v, NULL, dijkstra, NULL);
			} else if (strcmp ("bdj", argv[i]) == 0) {
				struct dijkstra_res *bdj = run_bdj (graph, u, v);
				write_to_csv (fname_write, fname_read, gd->n, gd->m, u, v, NULL, NULL, bdj);
			}
		};
	}
	return EXIT_SUCCESS;
}
