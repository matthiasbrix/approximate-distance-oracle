#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include "graph.h"
#include <time.h>

// result and measurement of Thorup-Zwick
struct tz_res {
  int dist;
  int k;
  double prepro_time;
  double dist_time;
  int prepro_memory_consump;
  int dist_memory_consump;
};

// result and measurement of Dijkstra's Algorithm
struct dijkstra_res {
  int dist;
  double dist_time;
  int memory_consump;
};

// |V|=n vertices and |E|=m edges of a graph G=(V,E)
struct graph_data {
  int n;
  int m;
};

int get_current_vm ();
int get_vm_peak ();
void read_from_file (struct graph *graph, const char *fname);
int read_offset_in_file (const char *fname);
struct graph_data *read_vertices_and_edges (const char *fname);
void write_to_csv (const char *fname, const char *input_file, int n,
                    int m, int u, int v, struct tz_res *tz,
                   struct dijkstra_res *dijkstra, struct dijkstra_res *bdj);

#endif
