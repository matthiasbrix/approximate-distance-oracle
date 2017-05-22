#include "filehandling.h"

FILE *file;

int parse_line (char* line)
{
	// This assumes that a digit will be found and the line ends in " Kb".
	int i = strlen (line);
	const char* p = line;
	while (*p < '0' || *p > '9')
		p++;
	line[i-3] = '\0';
	i = atoi(p);
	return i;
}

// Current VM value Note: this value is in KB!
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

// Peak VM, Note: this value is in KB!
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

void read_from_file (struct graph *graph, const char *fname)
{
	char t;
	int u, v, w, i;

	i = 0;
	file = fopen (fname, "r");

	if (file == NULL)
		exit (EXIT_FAILURE);

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

// In DIMACS-like files p stands for the problem line, means it is unique and must appear as the first non-comment line. This line has the format on the right, where n and m are the number of nodes and the number of arcs, respectively.
int count_vertices (const char *fname)
{
	file = fopen (fname, "r");
	char p[256];
	char t[256];
	int n, m;

	if (file == NULL)
		exit (EXIT_FAILURE);

	while (!feof(file)) {
		int no_match = fscanf (file, "%s %s %d %d\n", p, t, &n, &m);
		if (no_match == 4) {
			break;
		}
	}

	if (fclose(file)) {
		printf("Error closing file in count_vertices.");
		exit (EXIT_FAILURE);
	}

	return n;
}

void write_to_file (const char *fname, const char *input_file, int n, int u, int v,
					struct tz_res *tz, struct dijkstra_res *dijkstra)
{
	file = fopen (fname, "a+");

	if (file == NULL)
		exit (EXIT_FAILURE);

	fseek (file, 0, SEEK_END);
	unsigned long len = (unsigned long)ftell(file);
	if (len == 0) {
		fprintf (file, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s", "Time", "Input file",
				 "Algorithm", "n=|V| (# vertices)", "k integer", "vertex u", "vertex v", "d(u - v)",
				 "prepro time (s)", "dist time (s)", "prepro memory (KB)",
				 "dist memory (KB)", "memory consumption (KB)");
	}

	time_t clk = time(NULL);
	char *time = ctime(&clk);
	time[strlen(time) - 1] = '\0';
	if (tz != NULL) {
		fprintf (file, "\n%s,%s,%s,%d,%d,%d,%d,%d,%f,%f,%d,%d,%d\n", time,
				 input_file, "Thorup-Zwick", n, tz->k, u, v, tz->dist,
				 tz->prepro_time, tz->dist_time, tz->prepro_memory_consump,
				 tz->dist_memory_consump, tz->prepro_memory_consump);
	} else if (dijkstra != NULL) {
		fprintf (file, "%s,%s,%s,%d,%s,%d,%d,%d,%s,%f,%s,%s,%d\n", time,
				 input_file, "Dijkstra", n, "", u, v, dijkstra->dist,
				 "", dijkstra->dist_time, "", "", dijkstra->memory_consump);
	}

	if (fclose(file)) {
		printf("Error closing file in write_to_file.");
		exit (EXIT_FAILURE);
	}

	return;
}
