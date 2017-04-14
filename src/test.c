#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

struct node {
  int v_id;
  int sp_est;
  int index;
  struct node *pi; // predecessor
};

struct heap_t {
  struct node **nodes;
  int heap_size;
};

struct heap_t* copy_heap_struct (struct heap_t* old_heap)
{
	struct heap_t* new_heap = malloc (sizeof (struct heap_t));
	new_heap->nodes = malloc ((old_heap->heap_size) * sizeof (struct node));

	for (int b = 0; b < old_heap->heap_size; b++) {
		memcpy (&new_heap->nodes[b], &old_heap->nodes[b], sizeof (struct node));
	}

	new_heap->heap_size = old_heap->heap_size;

	return new_heap;
}

struct node* add_heap_node (int id, int distance)
{
	struct node *s = malloc (sizeof(struct node));
	s->v_id = s->index = id;
	s->sp_est = distance;
	s->pi = NULL;
	return s;
}

void pp_heap (struct heap_t *hp)
{
	printf ("PRINT HEAP NODES\n");
	for (int i = 0; i < hp->heap_size; i++) {
		printf ("%d %d %d \n", hp->nodes[i]->sp_est, hp->nodes[i]->v_id,
			hp->nodes[i]->index);
	}
	printf ("\n");
	return;
}

int main () {

	struct heap_t *heap = malloc (sizeof (struct heap_t));
	heap->nodes = malloc (5 * sizeof(struct node));

	int val = (int) INFINITY;

	for (unsigned int i = 0; i < 5; i++) {
		struct node *tmp = add_heap_node (i, val);
		heap->nodes[i] = tmp;
		printf ("%d\n", heap->nodes[i]->sp_est);
		heap->heap_size += 1;
	}

	pp_heap (heap);

	struct heap_t *write_heap = copy_heap_struct (heap);

	pp_heap (write_heap);

	return 0;
}
