#include "priorityqueue.h"

// Use a heap to implement a priority queue, need handle in each heap element
// handle could be array index in the heap

struct node *hash_heap = NULL;

// A utility function to create a new Min Heap Node
struct node *add_heap_node (int id, int distance)
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

struct heap_t *copy_heap_struct (struct heap_t *old_heap)
{
	struct heap_t *new_heap = malloc (sizeof (struct heap_t));
	new_heap->nodes = malloc ((old_heap->heap_size) * sizeof (struct node*));

	if (new_heap == NULL || new_heap->nodes == NULL) {
		printf ("Pointer error in copy of heap\n");
		return NULL;
	}
	for (int i = 0; i < old_heap->heap_size; i++) {
		new_heap->nodes[i] = malloc (sizeof (struct node));
		memcpy (new_heap->nodes[i], old_heap->nodes[i], sizeof (struct node));
	}
	new_heap->heap_size = old_heap->heap_size;

	return new_heap;
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

void fix_positions (struct heap_t *heap, int u)
{
	while ((u > 0) && (heap->nodes[PARENT(u)]->sp_est > heap->nodes[u]->sp_est)) {
		heap->nodes[u]->index = PARENT(u);
		heap->nodes[PARENT(u)]->index = u;
		// swap nodes in heap
		swap_nodes (&heap->nodes[u], &heap->nodes[PARENT(u)]);
		// move to parent index
		u = PARENT (u);
	}
	return;
}

void decrease_key (struct heap_t *heap, struct node *v, struct node *u, int sp_est)
{
	v->sp_est = sp_est;
	v->pi = u;
	int i = v->index;

	if (v->sp_est > heap->nodes[i]->sp_est) {
		printf ("Error (%d) with decreasing node v %d\n", KEY_SIZE, v->v_id);
		return;
	}
	memcpy (heap->nodes[i], v, sizeof (struct node));
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
	printf ("\nPRINT HEAP NODES\n");
	printf ("SPEST ID INDEX\n");
	for (int i = 0; i < hp->heap_size; i++) {
		printf ("%d %d %d \n", hp->nodes[i]->sp_est, hp->nodes[i]->v_id, hp->nodes[i]->index);
	}
	printf ("\n");
	return;
}
