#ifndef THORUPZWICK_H
#define THORUPZWICK_H

#include "graph.h"
#include "filehandling.h"

struct aseq {
  struct node *nodes;
  int seqsize;
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
};

struct prepro {
  struct node *nodes;
  struct bunchlist *bunchlist;
  bool success;
};

#endif
