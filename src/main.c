#include "main.h"
#include <sys/resource.h>

int offset;

// TODO: gdb:gcc -g -o prog myfile.c another.c
// gdb prog

// VIGTIGT: Kør tz og dijkstra begge med samme (u, v) og så sammenlign sp res, køretid og plads forbrug
//
/*
CLOCKS_PER_SEC is a constant which is declared in time.h. To get the CPU time used by a task within a C application,
returns the amount of time the OS has spent running your process, and not the actual amount of time elapsed. However, this is fine for timing a block of code, but not measuring time elapsing in the real world
Fx. tænk på sleep - CPUen arbejder så ikke, således vil den ikke tælle det med
*/

// TODO: Lav i filehandling en funktion til at lave lookup af distancen (u, v) i output.csv
// TODO: tid, plads, og hvor tæt kommer jeg på Dijkstras (SSP)

// TODO: Lav script, hvor jeg kan kalde med bogtstaver, og så incrementer test fil navne
// Og så hvad k er lig med

// tid, plads skal være hardware.
// Stemmer ekspirementerne overens med teori?
// du skal dog nævne i din rapport, hvilken hash table du bruger
// Skriv i rapport om at man kan udvide med floats
// Hash table for en knude v skal indeholde netop de knuder w, der tilhører B(v), og disse knuder w er netop dem, hvor v tilhører C(w).
	// zero level: B(v) for v in V.
/*
  Finally, the algorithm constructs (2-level) hash tables for the bunches B(v), for
  v ∈ V, and outputs the witnesses p_i(v), the distances δ(p_i(v), v) = δ(A_i, v), and
  the hash tables of B(v), for every 1 ≤ i ≤ k and v ∈ V. In addition to that, the
  preprocessing algorithm also outputs, for every w ∈ V, the shortest paths tree T (w)
  that spans the cluster C(w).
 */
// HVorfor vil thorup-zwick ikke være lige så "rigtig" som normal Dijkstra?
// http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
// TODO: Læs om hash funktionen af uthash
// TODO: for every v in V, generate bunches B(v) from all C(w)
// Checks whether w in B(v) and retrieves d(w, v)
// TODO: Skal det være en 2-level table med den foreslående hash funktion?
// TODO: Husk også at gemme afstanden i Bunches
// by definition, w ∈ B(v) if and only if v ∈ C(w).
// TODO: Have all pointer be with the variable name
// Bunche gen. er som DFS? (spørg ellers Christian)
// Skriv til uthash om hash funktioner
// hvilke eksperimenter og hvorfor. Vis så resultater.

void change_stack_size ()
{
	// Setting the stack size from 8MB to 64 MB
	const rlim_t stack_size = 64L * 1024L * 1024L;
	struct rlimit rl;
	int result;

	result = getrlimit (RLIMIT_STACK, &rl);

	if (result == 0) {
		printf ("current stack size: %zu KB\n", rl.rlim_cur);
		if (rl.rlim_cur < stack_size) {
			rl.rlim_cur = stack_size;
			result = setrlimit(RLIMIT_STACK, &rl);
			if (result != 0) {
				fprintf (stderr, "setrlimit returned result = %d\n", result);
			}
		}
	}

}

void test_prepro ()
{
	int n = 5;
	struct graph* graph = init_graph (n);
	int w[8] = { 10, 5, 7, 2, 1, 9, 2, 4};

	for (int i = 0; i < 8; i += 8) {
		add_edges (graph, 0, 1, w[i]);
		add_edges (graph, 0, 2, w[i+1]);
		add_edges (graph, 0, 4, w[i+2]);
		add_edges (graph, 1, 2, w[i+3]);
		add_edges (graph, 1, 3, w[i+4]);
		add_edges (graph, 2, 3, w[i+5]);
		add_edges (graph, 2, 4, w[i+6]);
		add_edges (graph, 3, 4, w[i+7]);
	}

	int k = 3;
	struct prepro *pp = malloc (sizeof (struct prepro));
	pp->success = false;
	while (!pp->success) {
		pp = prepro (graph, k);
	}
	int u = 1;
	int v = 2;
	int d = distk (&pp->nodes[u], &pp->nodes[v], pp->bunchlist);
	printf ("result of call dist(%d, %d) = %d\n", u, v, d);
}

void run_dijkstra (const char *fname_read, const char *fname_write, int u, int v)
{
	fname_write = fname_write;
	int V = count_vertices (fname_read);
	struct graph *graph = malloc (sizeof (struct graph));
	graph =	init_graph (V);
	read_from_file (graph, fname_read);
	struct node *S = dijkstra_alg (graph, u);
	// Find ud af afstanden mellem u og v ud fra S
	v = v;
	write_to_csv (fname_write, S, V);
}

void run_tz (const char *fname_read, const char *fname_write, int k, int u, int v)
{
	fname_write = fname_write;
	int n = count_vertices (fname_read);
	struct graph *graph = malloc (sizeof (struct graph));
	graph =	init_graph (n);
	read_from_file (graph, fname_read);
	struct prepro *pp = malloc (sizeof (struct prepro));
	pp->success = false;
	while (!pp->success) {
		pp = prepro (graph, k);
	}
	int d = distk (&pp->nodes[u], &pp->nodes[v], pp->bunchlist);
	printf ("result of call dist(%d, %d) = %d\n", u, v, d);
	dijkstra_alg (graph, u);
}

int main (int argc, char *argv[])
{
	clock_t begin = clock();

	if (argc == 1) {
		offset = 0;
		test_prepro ();
	} else if ((argc-1) < 5 || (((argc-1) % 5) != 0)) {
		printf ("No input and output arguments or input/output does not match!\n");
		printf ("Number of arguments is %d\n", argc);
		printf ("To run, required is as follows ./main <inputfile> <outputfile> <k integer> <u integer> <v integer>");
		return EXIT_FAILURE;
	} else {
		offset = 1;
		for (int i = 1; i < argc; i += 5) {
			const char *fname_read = argv[i];
			const char *fname_write = argv[i+1];
			int k = atoi(argv[i+2]);
			int u = atoi(argv[i+3]);
			int v = atoi(argv[i+4]);
			run_tz (fname_read, fname_write, k, u, v);
		};
	}

	clock_t end = clock();
	double cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf ("Time spent on running the program: %f\n", cpu_time_spent);

	return EXIT_SUCCESS;
}
