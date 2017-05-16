#ifndef PRIOTYQUEUE_H
#define PRIOTYQUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../lib/uthash.h"

#define HEAP_UNDERFLOW (-1)
#define KEY_SIZE (-2)

#define FREE(x) free(x); x = NULL;
#define MULTBY2(x) (x << 1)
#define LCHILD(x) MULTBY2(x) + 1
#define RCHILD(x) MULTBY2(x) + 2
#define PARENT(x) (x - 1) / 2

extern int offset;

// TODO: sp_est should be unsigned int
struct node {
  int v_id;
  int sp_est;
  int index;
  struct node *pi; // predecessor
  UT_hash_handle hh; // makes this structure hashable
};

struct heap {
  struct node **nodes;
  int heap_size;
};

struct adjlistnode {
  int v_id;
  int weight; // weights are signed integers in DIMACS files
  struct adjlistnode *next;
};

struct adjlist {
  struct node* nd;
  struct adjlistnode* head; // start of list
};

struct graph {
  unsigned int V; // nNumber of vertices
  struct adjlist *adjlists;
};

struct node *add_node(int id, int distance, int index);
void swap_nodes (struct node **ptr1, struct node **ptr2);
void min_heapify (struct heap *heap, unsigned int i);
void build_min_heap (struct heap *heap);
struct node* minimum (struct heap *heap);
struct node* extract_min (struct heap *heap);
void min_heap_insert (struct heap *heap, int id, int distance, struct graph *graph);
void pp_heap (struct heap *hp);
void decrease_key (struct heap *heap, struct node *v, struct node *u, int sp_est);
struct heap* copy_heap_struct (struct heap *old_heap);
void find_node_pos (struct heap *heap, int u);
void free_heap (struct heap *heap);

#endif
