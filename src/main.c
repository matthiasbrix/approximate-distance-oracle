#include "main.h"
#include <sys/resource.h>

int offset;

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
// TODO: SKal jeg genindlæs grafen når jeg kører dijkstra? Skal jeg overhovedet måle indlæsningen af knuderne?
// Skriv i rapporten hvad der sker når grafen ikke er sammenhængende...

struct dijkstra_res *run_dijkstra (struct graph *graph, int u, int v)
{
	struct dijkstra_res *dijkstra = malloc (sizeof (struct dijkstra_res));
	struct rusage r_usage;
	clock_t begin = clock();
	struct node *S = dijkstra_alg (graph, u-offset);
	clock_t end = clock();
	double cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	getrusage (RUSAGE_SELF, &r_usage);
	printf ("\nResult of Dijkstra SSP (%d, %d) = %d\n", u, v, S[v-offset].sp_est);
	printf ("Time spent on running Dijkstra: %f\n", cpu_time_spent);
	printf ("Memory usage = %ld KB\n", r_usage.ru_maxrss);
	dijkstra->dist = S[v-offset].sp_est;
	dijkstra->dist_time = cpu_time_spent;
	dijkstra->memory_consump = r_usage.ru_maxrss;

	return dijkstra;
}

// TODO: Reset memory usage to 0 after prepro
// TODO: Brug bit fields

struct tz_res *run_tz (struct graph *graph, int k, int u, int v)
{
	struct tz_res *tz = malloc (sizeof (struct tz_res));
	clock_t begin, end;
	double cpu_time_spent;
	struct rusage r_usage;

	begin = clock();
	struct prepro *pp = malloc (sizeof (struct prepro));
	pp->success = false;
	while (!pp->success) {
		pp = prepro (graph, k);
		sleep (1);
	}
	end = clock();
	cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf ("Time spent on prepro Thorup-Zwick: %f\n", cpu_time_spent);
	tz->prepro_time = cpu_time_spent;

	begin = clock();
	int d = dist (&pp->nodes[u-offset], &pp->nodes[v-offset], pp->bunchlist);
	end = clock();
	cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	getrusage (RUSAGE_SELF, &r_usage);
	printf ("\nResult of Thorup-Zwick dist(%d, %d) = %d\n", u, v, d);
	printf ("Time spent on query Thorup-Zwick: %f\n", cpu_time_spent);
	printf ("Memory usage = %ld KB\n", r_usage.ru_maxrss);

	tz->dist = d;
	tz->k = k;
	tz->dist_time = cpu_time_spent;
	tz->memory_consump = r_usage.ru_maxrss;

	return tz;
}

int main (int argc, char *argv[])
{
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
			int n = count_vertices (fname_read);
			struct graph *graph = malloc (sizeof (struct graph));
			graph =	init_graph (n);
			read_from_file (graph, fname_read);
			struct tz_res *tz = run_tz (graph, k, u, v);
			graph = malloc (sizeof (struct graph));
			graph =	init_graph (n);
			read_from_file (graph, fname_read);
			struct dijkstra_res *dijkstra = run_dijkstra (graph, u, v);
			write_to_file (fname_write, fname_read, u, v, tz, dijkstra);
		};
	}

	return EXIT_SUCCESS;
}
