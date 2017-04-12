#ifndef GRAPH_H
#define GRAPH_H

#include "priorityqueue.h"
#include <math.h>
#include <stdbool.h>

struct Graph* init_graph (int V);
void pp_nodes (struct node *S, int i);
void pp_graph (struct Graph* graph);
struct node *dijkstra_alg (struct Graph *graph, int s);
struct heap_t *initialise_single_source (struct Graph *graph, int s);
struct AdjListNode* add_node (int v, int weight);
void add_edges (struct Graph *graph, int u, int v, unsigned int w);

#endif
