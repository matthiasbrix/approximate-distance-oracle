#ifndef PRIOTYQUEUE_H
#define PRIOTYQUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uthash.h"

#define HEAP_UNDERFLOW (-1)
#define KEY_SIZE (-2)

#define FREE(x) free(x); x = NULL;
#define MULTBY2(x) (x << 1)
#define LCHILD(x) MULTBY2(x) + 1
#define RCHILD(x) MULTBY2(x) + 2
#define PARENT(x) (x - 1) / 2

extern struct node *hash_node;

/**
 * TODO: HOW TO COMMENT
 * down_timeout - acquire the semaphore within a specified time
 * @sem: the semaphore to be acquired
 * @timeout: how long to wait before failing
 *
 * Attempts to acquire the semaphore.  If no more tasks are allowed to
 * acquire the semaphore, calling this function will put the task to sleep.
 * If the semaphore is not released within the specified number of jiffies,
 * this function returns -ETIME.  It returns 0 if the semaphore was acquired.
 */

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

// TODO: weight should be unsigned int
struct adjlistnode {
  int v_id;
  int weight;
  struct adjlistnode *next;
};

// TODO: Få de to nederst i graph.h
struct adjlist {
  struct node* nd;
  struct adjlistnode* head; // start of list
};

struct graph {
  unsigned int V; // Number of vertices
  struct adjlist *adjlists;
};

struct node *add_node(int id, int distance, int index);
void swap_nodes (struct node **ptr1, struct node **ptr2);
void min_heapify (struct heap *heap, unsigned int i);
void build_min_heap (struct heap *heap);
struct node* minimum (struct heap *heap);
struct node* extract_min (struct heap *heap);
void pp_heap (struct heap *hp);
void decrease_key (struct heap *heap, struct node *v, struct node *u, int sp_est);
struct heap* copy_heap_struct (struct heap *old_heap);
void find_node_pos (struct heap *heap, int u);
void min_heap_insert (struct heap *heap, int id, int distance, struct graph *graph);

#endif
