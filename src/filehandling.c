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
			// Because we assume 1 indexing, subtract 1
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

void write_to_csv (const char *fname, struct node *S, int i)
{
	FILE *fp = fopen (fname, "w+");

	if (fp == NULL)
		exit (EXIT_FAILURE);

	fprintf (fp, "%s,%s,%s\n", "vertex id", "shortest path", "predecessor");

	for (int j = 0; j < i; j++) {
		if (S[j].pi == NULL) {
			fprintf(fp, "%d,%d,%s\n", S[j].v_id+offset, S[j].sp_est, "NULL");
		} else {
			fprintf(fp, "%d,%d,%d\n", S[j].v_id+offset, S[j].sp_est, S[j].pi->v_id+offset);
		}
	}

	return;
}
