#include "main.h"

#define MIN_REQUIRED 6
int offset = 0;
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
// by definition, w ∈ B(v) if and only if v ∈ C(w).
// TODO: Have all pointer be with the variable name
// Bunche gen. er som DFS? (spørg ellers Christian)
// Skriv til uthash om hash funktioner
// hvilke eksperimenter og hvorfor. Vis så resultater.
// TODO: SKal jeg genindlæs grafen når jeg kører dijkstra? Skal jeg overhovedet måle indlæsningen af knuderne?
// Skriv i rapporten hvad der sker når grafen ikke er sammenhængende...
// cat /proc/cpuinfo
// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

void help () {
	printf ("To run, required arguments are as follows:\n");
	printf ("./main <algorithm> <inputfile> <outputfile> <k integer> <u integer> <v integer>\n");
	printf ("Possible input for each flag:\n");
	printf ("<algorithm>: <dj>, <tz> \n");
	return;
}

struct dijkstra_res *run_dijkstra (struct graph *graph, int u, int v)
{
	struct dijkstra_res *dijkstra = malloc (sizeof (struct dijkstra_res));
	clock_t begin = clock();
	struct node *S = dijkstra_alg (graph, u-offset);
	clock_t end = clock();
	double cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf ("\nResult of Dijkstra SSP (%d, %d) = %d\n", u, v, S[v-offset].sp_est);
	printf ("Time spent on running Dijkstra: %f\n", cpu_time_spent);
	printf ("Memory usage of Dijkstra = %d KB\n", get_vm_peak () / 1000);
	dijkstra->dist = S[v-offset].sp_est;
	dijkstra->dist_time = cpu_time_spent;
	dijkstra->memory_consump = get_vm_peak ();

	return dijkstra;
}

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
	printf ("Time spent on prepro Thorup-Zwick: %f\n", cpu_time_spent);
	printf ("Memory usage of prepro = %d KB\n", get_vm_peak() / 1000);
	tz->prepro_time = cpu_time_spent;
	tz->prepro_memory_consump = get_vm_peak();

	begin = clock();
	int d = dist (&pp->nodes[u-offset], &pp->nodes[v-offset], pp->bunchlist);
	end = clock();
	cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf ("\nResult of Thorup-Zwick dist(%d, %d) = %d\n", u, v, d);
	printf ("Time spent on query Thorup-Zwick: %f\n", cpu_time_spent);
	tz->dist_memory_consump = pp->bunchlist->bunch_size;
	printf ("Memory usage of query (bunch size) = %d KB\n", tz->dist_memory_consump);
	tz->dist_time = cpu_time_spent;
	tz->dist = d;
	tz->k = k;

	return tz;
}

int main (int argc, char *argv[])
{
	// for debugging
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
		// TODO: read offset in read file
		offset = 1;
		for (int i = 1; i < argc; i += MIN_REQUIRED) {
			const char *fname_read = argv[i+1];
			const char *fname_write = argv[i+2];
			const int u = atoi(argv[i+4]);
			const int v = atoi(argv[i+5]);
			struct graph_data *gd = count_vertices (fname_read);
			struct graph *graph = init_graph (gd->n);
			read_from_file (graph, fname_read);
			if (strcmp ("tz", argv[i]) == 0) {
				const int k = atoi(argv[i+3]);
				struct tz_res *tz = run_tz (graph, k, u, v);
				write_to_file (fname_write, fname_read, gd->n, gd->m, u, v, tz, NULL);
			} else if (strcmp ("dj", argv[i]) == 0) {
				struct dijkstra_res *dijkstra = run_dijkstra (graph, u, v);
				write_to_file (fname_write, fname_read, gd->n, gd->m, u, v, NULL, dijkstra);
			}
		};
	}
	return EXIT_SUCCESS;
}
