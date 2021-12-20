#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include "graph.h"
#include <time.h>

int get_current_vm ();
int get_vm_peak ();
void read_from_file (struct graph *graph, const char *fname, int offset);
int read_offset_in_file (const char *fname);
struct graph_data *read_vertices_and_edges (const char *fname);
void write_to_csv (const char *fname, const char *input_file, int n,
                    int m, int u, int v, struct tz_res *tz,
                   struct ssp_res *dijkstra,
                   struct ssp_res *dijkstra_opt,
                   struct ssp_res *bdj);

#endif
