#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

struct AdjListNode {
  int v_id;
  int weight;
  struct AdjListNode *next;
};

// TODO Merge adjlist and adjlistnode
struct Adjlist {
  struct node* nd;
  struct AdjListNode* head; // end of list
};

struct Graph {
  unsigned int V; // Number of vertices
  struct Adjlist *adjlists;
};


int main () {

	struct Graph *a;
	a->adjlists[0].head

	return 0;
}
