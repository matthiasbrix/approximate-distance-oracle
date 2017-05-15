#include "priorityqueue.h"

// Use a heap to implement a priority queue, need handle in each heap element. Handle is index in heap array

// A utility function to create a new min heap node
struct node *add_node (int id, int distance, int index)
{
	struct node *s = malloc (sizeof(struct node));
	s->v_id = id;
	s->index = index;
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

struct heap *copy_heap_struct (struct heap *old_heap)
{
	struct heap *new_heap = malloc (sizeof (struct heap));
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

void min_heapify (struct heap *heap, unsigned int i)
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

void build_min_heap (struct heap *heap)
{
	for (int i = ((heap->heap_size - 1) >> 1); i >= 0; i--) {
		min_heapify (heap, i);
	}
	return;
}

struct node* minimum (struct heap *heap)
{
	return heap->nodes[0];
}

struct node* extract_min (struct heap *heap)
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

// traverses the tree up from the node to the root to find the right position. Makes it heapified again. Running time: O(lg n)
void find_node_pos (struct heap *heap, int u)
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

void decrease_key (struct heap *heap, struct node *v, struct node *u, int sp_est)
{
	v->sp_est = sp_est;
	v->pi = u;
	int i = v->index;

	#ifdef DEBUG
	printf ("decrease: %d %d %d %d\n", u->v_id+1, v->v_id+1, i, v->sp_est);
	#endif

	if (v->sp_est > heap->nodes[i]->sp_est) {
		printf ("Error (%d) with decreasing node vertex:%d, spest:%d\n", KEY_SIZE, v->v_id, v->sp_est);
		return;
	}

	heap->nodes[i]->sp_est = v->sp_est;
	heap->nodes[i]->pi = u;
	heap->nodes[i]->index = v->index;

	find_node_pos (heap, i);
}

// Insert node to heap. Graph is passed to update pointer reference. O(lg n)
void min_heap_insert (struct heap *heap, int id,
					  int distance, struct graph *graph)
{
	heap->nodes[heap->heap_size] = add_node (id, distance, heap->heap_size);
	graph->adjlists[id].nd = heap->nodes[heap->heap_size];
	heap->heap_size += 1;
	int index = heap->nodes[heap->heap_size-1]->index;
	// Updating the position of the node in the heap. O(lg n)
	find_node_pos (heap, index);
	return;
}

void free_heap (struct heap *heap)
{
	for (int i = 0; i < heap->heap_size; i++)
		free (heap->nodes[i]);
	free (heap);
}

void pp_heap (struct heap *hp)
{
	printf ("\nPRINT HEAP NODES\n");
	printf ("SPEST ID INDEX\n");
	for (int i = 0; i < hp->heap_size; i++) {
		printf ("%d %d %d \n", hp->nodes[i]->sp_est, hp->nodes[i]->v_id+1, hp->nodes[i]->index);
	}
	printf ("\n");
	return;
}
