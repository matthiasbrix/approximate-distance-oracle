#ifndef THORUPZWICK_H
#define THORUPZWICK_H

#include "graph.h"
#include "filehandling.h"

/* When a pointer is passed to a function, it is passed by value. If we want to modify the */
/* original pointer and not the copy of the pointer, we need to pass it as a pointer to a pointer. */

// TODO: Make it hashable with w as key
struct cluster {
  struct node *w;
  struct node *cluster;
  int no_nodes;
};

struct cluster_list {
  int no_clusters;
  struct cluster *clusters;
};

#endif
