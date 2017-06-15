#ifndef GRAPH_H
#define GRAPH_H

#include "priorityqueue.h"
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
  struct node *S_f;
  struct node *S_b;
  double avg_decrease_key_time;
  double avg_min_heap_insert_time;
  double avg_extract_min_time;
  double visited_nodes_ratio;
  double visited_edges_ratio;
  int visited_nodes;
  int visited_edges;
  int num_decrease_key;
  int num_heap_insert;
  int n;
  int m;
};

// |V|=n vertices and |E|=m edges of a graph G=(V,E)
struct graph_data {
  int n;
  int m;
};

struct graph* init_graph (int V);
void pp_nodes (struct node *S, int i);
void pp_graph (struct graph* graph);
struct dijkstra_res *dijkstra_opt_alg (struct graph *graph, int s, int t);
struct dijkstra_res *dijkstra_alg (struct graph *graph, int s);
struct node *dijkstra_alg_tz (struct graph *graph, struct heap *Q);
struct dijkstra_res *bidirectional_dijkstra (struct graph *gf, int u, int v);
struct heap *initialise_single_source (struct graph *graph, int s);
struct adjlistnode *add_adj_node (int v, int weight);
void add_edges (struct graph *graph, int u, int v, unsigned int w);
struct graph *copy_graph_struct (struct graph* old_graph, struct heap *heap);
struct heap *initialise_single_source_tz (unsigned int n);
void free_graph (struct graph *graph);

#endif
