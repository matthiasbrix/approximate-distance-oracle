#ifndef PRIOTYQUEUE_H
#define PRIOTYQUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uthash.h"

#define HEAP_UNDERFLOW (-1)
#define KEY_SIZE (-2)

#define ARRSIZE(arr) (sizeof(arr) / sizeof(arr[0])
// TODO Remove the above?

#define FREE(x) free(x); x = NULL;
#define MULTBY2(x) (x << 1)
#define LCHILD(x) MULTBY2(x) + 1
#define RCHILD(x) MULTBY2(x) + 2
#define PARENT(x) (x - 1) / 2

extern struct node *hash_heap;

struct node {
  int v_id;
  int sp_est;
  int index;
  struct node *pi; // predecessor
  UT_hash_handle hh; /* makes this structure hashable */
};

struct AdjListNode {
  int v_id;
  int weight;
  struct AdjListNode *next;
};

// TODO Merge adjlist and adjlistnode
struct Adjlist {
  struct node *nd;
  struct AdjListNode *head; // end of list
};

struct Graph {
  unsigned int V; // Number of vertices
  struct Adjlist *adjlists;
};

struct heap_t {
  // TODO: Change to *nodes
  struct node **nodes;
  int heap_size;
};

struct node *add_heap_node(int id, int distance);
void swap_nodes (struct node **ptr1, struct node **ptr2);
void min_heapify (struct heap_t *heap, unsigned int i);
void build_min_heap (struct heap_t *heap);
struct node* minimum (struct heap_t *heap);
struct node* extract_min (struct heap_t *heap);
void pp_heap (struct heap_t *hp);
void decrease_key (struct heap_t *heap, struct node *v);

#endif
