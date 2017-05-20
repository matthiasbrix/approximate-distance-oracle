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

void read_from_file (struct graph *graph, const char *fname);
void write_to_file (const char *fname, const char *input_file, int u, int v,
                    struct tz_res *tz, struct dijkstra_res *dijkstra);
int count_vertices (const char *fname);
int get_current_vm ();
int get_current_pm ();
int get_vm_peak ();

#endif
