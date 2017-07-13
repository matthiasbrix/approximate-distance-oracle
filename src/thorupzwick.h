#ifndef THORUPZWICK_H
#define THORUPZWICK_H

#include <time.h>
#include <assert.h>
#include <unistd.h>
#include "graph.h"
#include "filehandling.h"
#include "sodium.h"

extern int offset;

struct aseq {
  struct node *nodes;
  int *added; // mark whether node in A_i or not
  int setlength;
};

struct cluster {
  struct node *w;
  struct node *nodes;
  int num_nodes;
};

struct clusterlist {
  struct cluster *clusters;
  int num_clusters;
};

struct bunch {
  struct node *v;
  struct node *piv; // witnesses p_i(v) and distances d(p_i(v), v) = d(A_i. v)
  struct node *nodes;
  int num_nodes;
};

struct bunchlist {
  struct bunch *bunches;
  int num_bunches;
  size_t bunch_size;
};

struct prepro {
  struct node *nodes;
  struct bunchlist *bunchlist;
  bool success;
  int k;
};

struct prepro *prepro (struct graph *graph, int k);
int dist (struct node *u, struct node *v, struct bunchlist *bunchlist);

#endif
