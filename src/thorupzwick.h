#ifndef THORUPZWICK_H
#define THORUPZWICK_H

#include "graph.h"
#include "filehandling.h"
#include "uthash.h"

// TODO: Have all pointer be with the variable name

extern struct bunch *hash_bunch;

struct aseq {
  struct node *nodes;
  int seqsize;
};

// TODO: Make it hashable with w as key
struct cluster {
  struct node *w;
  struct node *cluster;
  int no_nodes;
};

struct clusterlist {
  int no_clusters;
  struct cluster *clusters;
};

struct bunch {
  //struct bunch *bunch;
  struct node *v;
  struct node *piv; // witnesses p_i(v) and distances d(p_i(v), v) = d(A_i. v)
  struct node *nodes;
  int no_nodes;
  UT_hash_handle hh; /* makes this structure hashable */
};

struct bunchlist {
  int no_bunches;
  struct bunch *bunches;
};

#endif
