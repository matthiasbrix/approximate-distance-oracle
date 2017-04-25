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
struct node {
  int v_id;
  int sp_est;
  int index;
  struct node *pi; // predecessor
  UT_hash_handle hh; /* makes this structure hashable */
};

// TODO: rename to heap
struct heap_t {
  struct node **nodes;
  int heap_size;
};

// TODO: rename to adjlistnode
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

// TODO: rename to graph
struct Graph {
  unsigned int V; // Number of vertices
  struct Adjlist *adjlists;
};

struct node *add_heap_node(int id, int distance);
void swap_nodes (struct node **ptr1, struct node **ptr2);
void min_heapify (struct heap_t *heap, unsigned int i);
void build_min_heap (struct heap_t *heap);
struct node* minimum (struct heap_t *heap);
struct node* extract_min (struct heap_t *heap);
void pp_heap (struct heap_t *hp);
/* void decrease_key (struct heap_t *heap, struct node *v); */
void decrease_key (struct heap_t *heap, struct node *v, struct node *u, int sp_est);
struct heap_t* copy_heap_struct (struct heap_t* old_heap);
void fix_positions (struct heap_t *heap, int u);

#endif
