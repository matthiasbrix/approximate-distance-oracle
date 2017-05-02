#ifndef THORUPZWICK_H
#define THORUPZWICK_H

#include "graph.h"
#include "filehandling.h"

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

// TODO: Make it hashable
struct bunch {
  struct node *v;
};

#endif
