#ifndef TESTINGLIBRARY_H
#define TESTINGLIBRARY_H

#include "graph.h"
#include "priorityqueue.h"
#include "thorupzwick.h"
#include <sys/resource.h>

void test_prepro ();
struct node* dijkstra_alg (struct graph *graph, int s);

#endif
