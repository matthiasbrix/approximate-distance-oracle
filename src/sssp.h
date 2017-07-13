#ifndef SSP_H
#define SSP_H

#include "graph.h"
#include "minheap.h"
#include "thorupzwick.h"

struct ssp_res *dijkstra_opt_alg (struct graph *graph, int s, int t);
struct ssp_res *dijkstra_alg (struct graph *graph, int s);
struct ssp_res *bidirectional_dijkstra (struct graph *gf, int u, int v);
struct ssp_res *astar (struct graph *graph, struct prepro *pp, int s, int t);
struct heap *initialise_single_source (struct graph *graph, int s);

#endif
