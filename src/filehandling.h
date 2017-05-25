#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include "graph.h"
#include <time.h>

struct tz_res {
  int dist;
  int k;
  double prepro_time;
  double dist_time;
  int prepro_memory_consump;
  int dist_memory_consump;
};

struct dijkstra_res {
  int dist;
  double dist_time;
  int memory_consump;
};

struct graph_data {
  int n;
  int m;
};

void read_from_file (struct graph *graph, const char *fname);
void write_to_file (const char *fname, const char *input_file, int n, int m, int u, int v,
                    struct tz_res *tz, struct dijkstra_res *dijkstra);
struct graph_data *count_vertices (const char *fname);
int get_current_vm ();
int get_vm_peak ();
int read_offset_in_file (const char *fname);

#endif
