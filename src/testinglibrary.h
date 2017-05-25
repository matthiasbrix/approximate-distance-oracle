#ifndef TESTINGLIBRARY_H
#define TESTINGLIBRARY_H

#include "graph.h"
#include "priorityqueue.h"
#include "thorupzwick.h"
#include "filehandling.h"
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>

void test_prepro ();
void hardcoded_tests ();
struct node *dijkstra_alg (struct graph *graph, int s);
int bidirectional_dijkstra (struct graph *graph, int xi, int xg);

#endif
