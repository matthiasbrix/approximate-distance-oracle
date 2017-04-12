#include "filehandling.h"

FILE *file;

void read_from_file (struct Graph *graph, const char *fname)
{
	char t;
	int u, v, w;

	int i = 0;
	file = fopen (fname, "r");

	if (file == NULL)
		exit (EXIT_FAILURE);

	while(!feof(file)) {
		int no_match = fscanf (file, "%c %d %d %d\n", &t, &u, &v, &w);
		if (no_match == 4 && (i % 2) == 0 && t == 'a') {
			add_edges (graph, u, v, w);
		}
		i++;
	}

	if (fclose(file)) {
		printf("error closing file.");
		exit(-1);
	}

	return;
}

int count_vertices (const char *fname)
{
	FILE *file = fopen (fname, "r");
	char t;
	int u, v, w;
	int i = 0, count = 0;

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
	return count;
}

void write_to_csv (const char *fname, struct node *S, int i)
{
	FILE *fp = fopen (fname, "w+");

	if (fp == NULL)
		exit (EXIT_FAILURE);

	fprintf (fp, "%s,%s,%s\n", "vertex id", "shortest path", "predecessor");

	for (int j = 1; j < i; j++) {
		if (S[j].pi == NULL) {
			fprintf(fp, "%d,%d,%s\n", S[j].v_id, S[j].sp_est, "NULL");
		} else {
			fprintf(fp, "%d,%d,%d\n", S[j].v_id, S[j].sp_est, S[j].pi->v_id);
		}
	}

	return;
}
