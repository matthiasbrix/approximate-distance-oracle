#ifndef GRAPH_H
#define GRAPH_H

#include "priorityqueue.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

// Indicating whether 0 or 1 indexed for vertices
extern int offset;

struct graph* init_graph (int V);
void pp_nodes (struct node *S, int i);
void pp_graph (struct graph* graph);
struct node *dijkstra_alg (struct graph *graph, int s);
struct node *dijkstra_alg_tz (struct graph *graph, struct heap *Q);
struct heap *initialise_single_source (struct graph *graph, int s);
struct adjlistnode *add_adj_node (int v, int weight);
void add_edges (struct graph *graph, int u, int v, unsigned int w);
struct graph *copy_graph_struct (struct graph* old_graph, struct heap *heap);
struct heap *initialise_single_source_tz (unsigned int n);
void free_graph (struct graph *graph);

#endif
