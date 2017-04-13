#include "priorityqueue.h"

// Use a heap to implement a priority queue, need handle in each heap element
// handle could be array index in the heap

struct node *hash_heap = NULL;

// A utility function to create a new Min Heap Node
struct node* add_heap_node (int id, int distance)
{
	struct node *s = malloc (sizeof(struct node));
	s->v_id = s->index = id;
	s->sp_est = distance;
	s->pi = NULL;
	return s;
}

void swap_nodes (struct node **ptr1, struct node **ptr2)
{
	struct node *temp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = temp;
}

void min_heapify (struct heap_t *heap, unsigned int i)
{
	int left = LCHILD(i);
	int right = RCHILD(i);
	unsigned int lowest;
	if (left < heap->heap_size &&
		heap->nodes[left]->sp_est < heap->nodes[i]->sp_est) {
		lowest = left;
	} else {
		lowest = i;
	}
	if (right < heap->heap_size &&
		heap->nodes[right]->sp_est < heap->nodes[lowest]->sp_est) {
		lowest = right;
	}
	if (lowest != i) {
		heap->nodes[lowest]->index = i;
		heap->nodes[i]->index = lowest;
		swap_nodes (&heap->nodes[i], &heap->nodes[lowest]);
		min_heapify (heap, lowest);
	}
	return;
}

void build_min_heap (struct heap_t *heap)
{
	for (int i = ((heap->heap_size - 1) >> 1); i >= 0; i--) {
		min_heapify (heap, i);
	}
	return;
}

struct node* minimum (struct heap_t *heap)
{
	return heap->nodes[0];
}

struct node* extract_min (struct heap_t *heap)
{
	if (heap->heap_size < 1) {
		printf ("Heap underflow %d\n", HEAP_UNDERFLOW);
		FREE (*heap->nodes);
		return NULL;
	}
	struct node *min = heap->nodes[0];
	swap_nodes (&heap->nodes[0], &heap->nodes[heap->heap_size-1]);
	heap->heap_size -= 1;
	if (heap->heap_size == 0) {
		return min;
	}
	heap->nodes[0]->index = 0;
	heap->nodes[heap->heap_size-1]->index = heap->heap_size-1;
	min_heapify (heap, 0);
	return min;
}

void decrease_key (struct heap_t *heap, struct node *v)
{
	int i = v->index;
	if (v->sp_est > heap->nodes[i]->sp_est) {
		printf ("Error with decrease %d\n", KEY_SIZE);
		return;
	}
	// Traverse tree up while the tree is not heapified. O(lg n)
	while (i > 0 && heap->nodes[PARENT(i)]->sp_est > heap->nodes[i]->sp_est) {
		heap->nodes[i]->index = PARENT(i);
		heap->nodes[PARENT(i)]->index = i;
		// swap nodes in heap
		swap_nodes (&heap->nodes[i], &heap->nodes[PARENT(i)]);
		// move to parent index
		i = PARENT(i);
	}
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
