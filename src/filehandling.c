#include "filehandling.h"

FILE *file;

/**
 * parse_line - parsing line from /proc/self/status
 * @line: the line being passed
 * Parsing the line for memory reading as a string and
 * return the memory consumption as an integer
 */
int parse_line (char *line)
{
	// This assumes that a digit will be found
	// and the line ends in " Kb".
	int i = strlen (line);
	const char* p = line;
	while (*p < '0' || *p > '9')
		p++;
	line[i-3] = '\0';
	i = atoi(p);
	return i;
}

/**
 * get_current_vm - retrieves current virtual memory consumption
 * Current VM value is in kilobytes
 * Note: Works only on Linux
 */
int get_current_vm ()
{
	file = fopen("/proc/self/status", "r");
	int result = -1;
	char line[128];

	while (fgets(line, 128, file) != NULL) {
		if (strncmp(line, "VmSize:", 7) == 0) {
			result = parse_line (line);
			break;
		}
	}
	if (fclose(file)) {
		printf("Error closing file.");
		exit (EXIT_FAILURE);
	}
	return result;
}

/**
 * get_vm_peak - peak virtual memory use
 * Note: Works only on Linux and value is in kilobytes
 */
int get_vm_peak ()
{
	file = fopen("/proc/self/status", "r");
	int result = -1;
	char line[128];

	while (fgets(line, 128, file) != NULL) {
		if (strncmp(line, "VmPeak:", 7) == 0) {
			result = parse_line(line);
			break;
		}
	}
	if (fclose(file)) {
		printf("Error closing file.");
		exit (EXIT_FAILURE);
	}
	return result;
}

/**
 * read_offset_in_file - reading from the input file whether 0 or 1 indexed
 * @fname: The input file name
 * iterates through all to check whether there are some vertices in between
 * that have lowest vertex id. The vertex id indicates the index
 */
int read_offset_in_file (const char *fname)
{
	char t;
	int u, v, w, i, offset;

	i = 0;
	offset = (int) INFINITY;
	file = fopen (fname, "r");

	if (file == NULL) {
		printf("File %s does not exist!", fname);
		exit(EXIT_FAILURE);
	}

	while (!feof(file)) {
		int num_match = fscanf (file, "%c %d %d %d\n", &t, &u, &v, &w);
		if (num_match == 4 && (i % 2) == 0 && t == 'a') {
			offset = u < offset ? u : offset;
			offset = v < offset ? v : offset;
		}
		i++;
	}

	if (fclose(file)) {
		printf("Error closing file.");
		exit (EXIT_FAILURE);
	}

	return offset;
}

/**
 * read_from_file - reading node & edges from the input graph file
 * @graph: the graph to be filled with content
 * @fname: the file name to be read
 * Reading the graph in the file. The function is tailored for
 * the DIMACS ssp graph format (.gr)
 * http://www.diag.uniroma1.it/challenge9/format.shtml
 */
void read_from_file (struct graph *graph, const char *fname, int offset)
{
	char t;
	int u, v, w, i;

	i = 0;
	file = fopen (fname, "r");

	if (file == NULL) {
		printf("File %s does not exist!", fname);
		exit(EXIT_FAILURE);
	}

	while (!feof(file)) {
		int num_match = fscanf (file, "%c %d %d %d\n", &t, &u, &v, &w);
		if (num_match == 4 && (i % 2) == 0 && t == 'a') {
			// If e.g 1 is used in input,
			// subtract the offset such that begin is 0
			add_edges (graph, u-offset, v-offset, w);
		}
		i++;
	}

	if (fclose(file)) {
		printf("Error closing file.");
		exit (EXIT_FAILURE);
	}

	return;
}

/**
 * read_vertices_and_edges - reading number of vertices and edges
 * @fname: the file name to be read
 * In DIMACS-like files, p stands for the problem line, means it is unique and
 * must appear as the first non-comment line.
 * In the p-line number of nodes and the number of edges can be read.
 */
struct graph_data *read_vertices_and_edges (const char *fname)
{
	file = fopen (fname, "r");
	char p;
	char t[256];
	int n, m;

	if (file == NULL)
		exit (EXIT_FAILURE);

	while (!feof(file)) {
		int no_match = fscanf (file, "%s %s %d %d\n", &p, t, &n, &m);
		if (no_match == 4 && p == 'p') {
			break;
		}
	}

	if (fclose(file)) {
		printf("Error closing file in count_vertices.");
		exit (EXIT_FAILURE);
	}

	struct graph_data *gd = malloc (sizeof (struct graph_data));
	gd->n = n;
	gd->m = m;

	return gd;
}

/**
 * write_to_file - writing all results to a csv file
 * @fname: the file name to be read
 * @input_file: the initial graph input file that as been applied
 * @n: number of vertices
 * @m: number of edges
 * @u: source vertex in the distance query
 * @v: target vertex in the distance query
 * @tz: the Thorup-Zwick result structure
 * @dijkstra: the Dijkstra's result structure
 * Writing all results of this program to a comma separated value (csv) file
 */
void write_to_csv (const char *fname, const char *input_file, int n, int m,
					int u, int v, struct tz_res *tz, struct ssp_res
				   *dijkstra, struct ssp_res *dijkstra_opt,
				   struct ssp_res *bdj)
{
	file = fopen (fname, "a+");

	if (file == NULL)
		exit (EXIT_FAILURE);

	fseek (file, 0, SEEK_END);
	unsigned long len = (unsigned long)ftell(file);
	if (len == 0) {
		fprintf (file, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
				 "Time", "Input file", "Algorithm", "n=|V| (# vertices)",
				 "m=|E| (# edges)","k integer", "vertex u",
				 "vertex v", "d(u - v)", "prepro time (s)",
				 "query time (s)", "num. query exe.", "prepro memory (KB)",
				 "query memory (KB)", "peak memory consumption (KB)",
				 "ratio visited nodes", "ratio visited edges",
				 "min-heap-insert operations", "decrease-key operations",
				 "extract-min operations", "min-heap-insert avg time",
				 "decrease-key avg time", "extract-min avg time");
	}
	time_t clk = time(NULL);
	char *time = ctime(&clk);
	time[strlen(time) - 1] = '\0';
	if (tz != NULL) {
		fprintf (file, "\n%s,%s,%s,%d,%d,%d,%d,%d,%d,%f,%f,%d,%d,%d,%d,%s,%s,%s,%s,%s,%s,%s,%s", time,
				 input_file, "Thorup-Zwick", n, m, tz->k, u, v, tz->dist,
				 tz->prepro_time, tz->dist_time, tz->query_times, tz->prepro_memory_consump,
				 tz->dist_memory_consump, tz->prepro_memory_consump, "", "", "",
				 "", "", "", "", "");
	} else if (dijkstra != NULL) {
		fprintf (file, "\n%s,%s,%s,%d,%d,%s,%d,%d,%d,%s,%f,%d,%s,%s,%d,%f,%f,%d,%d,%d,%f,%f,%f", time,
				 input_file, "Dijkstra's", n, m, "", u, v, dijkstra->dist,
				 "", dijkstra->dist_time, dijkstra->query_times, "", "", dijkstra->memory_consump,
				 dijkstra->visited_nodes_ratio, dijkstra->visited_edges_ratio,
				 dijkstra->visited_nodes, dijkstra->num_decrease_key, dijkstra->num_extract_min,
				 dijkstra->avg_min_heap_insert_time, dijkstra->avg_decrease_key_time,
				 dijkstra->avg_extract_min_time);
	} else if (dijkstra_opt != NULL) {
		fprintf (file, "\n%s,%s,%s,%d,%d,%s,%d,%d,%d,%s,%f,%d,%s,%s,%d,%f,%f,%d,%d,%d,%f,%f,%f", time,
				 input_file, "Dijkstra's optimised", n, m, "", u, v, dijkstra_opt->dist,
				 "", dijkstra_opt->dist_time, dijkstra_opt->query_times, "", "", dijkstra_opt->memory_consump,
				 dijkstra_opt->visited_nodes_ratio, dijkstra_opt->visited_edges_ratio,
				 dijkstra_opt->visited_nodes, dijkstra_opt->num_decrease_key,
				 dijkstra_opt->num_extract_min,
				 dijkstra_opt->avg_min_heap_insert_time,
				 dijkstra_opt->avg_decrease_key_time,
				 dijkstra_opt->avg_extract_min_time);
	} else if (bdj != NULL) {
		fprintf (file, "\n%s,%s,%s,%d,%d,%s,%d,%d,%d,%s,%f,%d,%s,%s,%d,%f,%f,%d,%d,%d,%f,%f,%f", time,
				 input_file, "Bidirectional Dijkstra", n, m, "", u, v, bdj->dist,
				 "", bdj->dist_time, bdj->query_times, "", "", bdj->memory_consump,
				 bdj->visited_nodes_ratio, bdj->visited_edges_ratio,
				 bdj->visited_nodes, bdj->num_decrease_key, bdj->num_extract_min,
				 bdj->avg_min_heap_insert_time, bdj->avg_decrease_key_time,
				 bdj->avg_extract_min_time);
	}

	if (fclose(file)) {
		printf("Error closing file in write_to_file.");
		exit (EXIT_FAILURE);
	}

	return;
}
