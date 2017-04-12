#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include "graph.h"

void read_from_file (struct Graph *graph, const char *fname);
void write_to_csv (const char *fname, struct node *S, int i);
int count_vertices (const char *fname);

#endif
