#ifndef GRAPH_H
#define GRAPH_H

#include "priorityqueue.h"
#include <math.h>
#include <stdbool.h>

struct Graph* init_graph (int V);
void pp_nodes (struct node *S, int i);
void pp_graph (struct Graph* graph);
struct node *dijkstra_alg (struct Graph *graph, int s);
struct node *dijkstra_alg_tz (struct Graph *graph, struct heap_t *Q);
void add_s_node_to_graph (struct Graph *graph, struct node *ai, int ailen);
struct heap_t *initialise_single_source (struct Graph *graph, int s);
struct AdjListNode* add_node (int v, int weight);
void add_edges (struct Graph *graph, int u, int v, unsigned int w);
struct Graph *copy_graph_struct (struct Graph* old_graph, struct heap_t *heap);
struct heap_t* initialise_single_source_tz (struct Graph *graph);

#endif
