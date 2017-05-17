#include "filehandling.h"

FILE *file;

void read_from_file (struct graph *graph, const char *fname)
{
	char t;
	int u, v, w, i;

	i = 0;
	file = fopen (fname, "r");

	if (file == NULL)
		exit (EXIT_FAILURE);

	while (!feof(file)) {
		int no_match = fscanf (file, "%c %d %d %d\n", &t, &u, &v, &w);
		if (no_match == 4 && (i % 2) == 0 && t == 'a') {
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

int count_vertices (const char *fname)
{
	file = fopen (fname, "r");
	char t;
	int u, v, w, i, count;

	i = 0;
	count = 0;

	if (file == NULL)
		exit (EXIT_FAILURE);

	while (!feof(file)) {
		int no_match = fscanf (file, "%c %d %d %d\n", &t, &u, &v, &w);
		if (no_match == 4 && (i % 2) == 0 && t == 'a') {
			count = u > count ? u : count;
			count = v > count ? v : count;
		}
		i++;
	}

	if (fclose(file)) {
		printf("Error closing file in count_vertices.");
		exit (EXIT_FAILURE);
	}

	return count;
}

void write_to_file (const char *fname, const char *input_file, int u, int v,
					struct tz_res *tz, struct dijkstra_res *dijkstra)
{
	file = fopen (fname, "a+");

	if (file == NULL)
		exit (EXIT_FAILURE);

	fseek (file, 0, SEEK_END);
	unsigned long len = (unsigned long)ftell(file);
	if (len == 0) {
		fprintf (file, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", "Time", "Input file",
				 "Algorithm", "k integer", "vertex u", "vertex v", "d(u - v)",
				 "prepro time (s)", "dist time (s)", "memory consumption (KB)");
	}

	time_t clk = time(NULL);
	char *time = ctime(&clk);
	time[strlen(time) - 1] = '\0';
	fprintf (file, "\n%s,%s,%s,%d,%d,%d,%d,%f,%f,%ld\n", time,
			 input_file, "Thorup-Zwick", tz->k, u, v, tz->dist,
			 tz->prepro_time, tz->dist_time, tz->memory_consump);
	fprintf (file, "%s,%s,%s,%s,%d,%d,%d,%s,%f,%ld\n", time,
			 input_file, "Dijkstra", "", u, v, dijkstra->dist,
			 "", dijkstra->dist_time, dijkstra->memory_consump);

	if (fclose(file)) {
		printf("Error closing file in write_to_file.");
		exit (EXIT_FAILURE);
	}

	return;
}
