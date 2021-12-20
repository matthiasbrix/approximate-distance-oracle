#ifndef GRAPH_H
#define GRAPH_H

#include "minheap.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define BILLION 1E9

// Indicating whether 0 or 1 indexed for vertices
extern int offset;

// result and measurement of Thorup-Zwick
struct tz_res {
  int dist;
  int query_times;
  int k;
  double prepro_time;
  double dist_time;
  int prepro_memory_consump;
  int dist_memory_consump;
};

// result and measurement of Dijkstra's Algorithm and A*
struct ssp_res {
  int dist;
  int query_times;
  double dist_time;
  int memory_consump;
  struct node *S_f;
  struct node *S_b;
  double avg_decrease_key_time;
  double avg_min_heap_insert_time;
  double avg_extract_min_time;
  double visited_nodes_ratio;
  double visited_edges_ratio;
  int visited_nodes; // also for min-heap-insert
  int visited_edges;
  int num_decrease_key;
  int num_extract_min;
  int n;
  int m;
};

// |V|=n vertices and |E|=m edges of a graph G=(V,E)
struct graph_data {
  int n;
  int m;
};

struct graph* init_graph (int V);
void pp_nodes (struct node *S, int i, int offset);
void pp_graph (struct graph* graph, int offset);
struct adjlistnode *add_adj_node (int v, int weight);
void add_edges (struct graph *graph, int u, int v, unsigned int w);
struct graph *copy_graph_struct (struct graph* old_graph, struct heap *heap);
void free_graph (struct graph *graph);

#endif
