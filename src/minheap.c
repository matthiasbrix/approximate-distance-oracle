#include "minheap.h"

/**
 * add_node - A utility function to create a new heap node
 * @id: vertex id
 * @distance: distance of node
 * @index: index in heap of node
 * Running time: O(1)
 * The handle in each heap element is the vertex id (4 bit integer)
 */
struct node *add_node (int id, int distance, int index)
{
	struct node *s = malloc (sizeof(struct node));
	s->v_id = id;
	s->index = index;
	s->sp_est = distance;
	s->pi = NULL;
	return s;
}

/**
 * swap_nodes - A utility function to swap nodes in heap
 * @ptr1: heap node
 * @ptr2: heap node
 * Running time: O(1)
 * Swapping nodes in the heap, e.g parent with child
 */
void swap_nodes (struct node **ptr1, struct node **ptr2)
{
	struct node *temp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = temp;
}

/**
 * copy_heap_struct - Deep copy of heap
 * @old_heap: heap to be copied
 * Running time: O(n)
 * Making a deep copy of the heap to utilize a work heap
 */
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

/**
 * min_heapify - Function for maintaining the min heap property
 * @heap: heap to be heapified
 * @i: the index to start at, e.g index 0 indicates root node
 * Running time: O(lg n)
 * Lets the node heap->nodes[i] float down to in the min-heap such that
 * the min heap property is hold. This assumes that heap->nodes[i] is higher in
 * value than at least it's children.
 */
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

/**
 * build_min_heap - Building a min heap
 * @heap: allocated heap to build
 * Running time: O(n)
 * Build a min heap by looping through the left half of the length
 * as floor(n/2)+1..n are all min heapified by definition
 */
void build_min_heap (struct heap *heap)
{
	for (int i = ((heap->heap_size - 1) >> 1); i >= 0; i--) {
		min_heapify (heap, i);
	}
	return;
}

/**
 * minimum - returning root node of heap
 * @heap: minimum heap
 * Running time: O(1)
 * returns root node without extracting
 */
struct node *minimum (struct heap *heap)
{
	return heap->nodes[0];
}

/**
 * extract_min - extracts and returns root node of heap
 * @heap: minimum heap
 * Running time: O(lg n)
 * returns root node and heapifies the heap tree afterwards
 */
struct node *extract_min (struct heap *heap)
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

/**
 * find_node_pos - extracts and returns root node of heap
 * @heap: minimum heap
 * @u: index of node in heap
 * Running time: O(lg n)
 * traverses the tree up from the node to the root to find the right position.
 * Maintains the min heap property again
 */
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

/**
 * decrease_key - decreasing key of node
 * @heap: minimum heap
 * @v: the node to be decreased/updated
 * @u: the predecessor
 * @sp_est: the distance to update v with
 * Running time: O(lg n)
 * decreases the node v and updates the predecessor pointer and then
 * yields find_node_pos to maintain the heap property again
 */
void decrease_key (struct heap *heap, struct node *v, struct node *u, int sp_est)
{
	v->sp_est = sp_est;
	v->pi = u;
	int i = v->index;

	if (v->sp_est > heap->nodes[i]->sp_est) {
		printf ("Error (%d) with decreasing node vertex:%d, spest:%d\n", KEY_SIZE, v->v_id, v->sp_est);
		return;
	}

	heap->nodes[i]->sp_est = v->sp_est;
	heap->nodes[i]->pi = u;
	heap->nodes[i]->index = v->index;

	find_node_pos (heap, i);
}

/**
 * min_heap_insert - Insert node to heap
 * @heap: minimum heap
 * @id: the id of the vertex to be inserted (Note: has to be unique!)
 * @distance: the initial distance (likely infinity or 0)
 * @graph: graph struct for passing the update pointer reference
 * Running time: O(lg n)
 * Inserting a node into the heap. Adding the node and ensuring min heap
 * property.
 */
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

/**
 * free_graph - Utility function to free the min heap
 * @heap: heap to be freed from memory
 * Running time: O(n)
 * Freeing all entities in the heap struct from memory
 */
void free_heap (struct heap *heap)
{
	for (int i = 0; i < heap->heap_size; i++)
		FREE (heap->nodes[i]);
	FREE (heap);
}

/**
 * pp_graph - Pretty print function for the heap struct
 * @hp: heap to be printed
 * Running time: O(n)
 * Printing all entities in a user friendly matter
 */
void pp_heap (struct heap *hp)
{
	printf ("\nPRINT HEAP NODES\n");
	printf ("SPEST ID INDEX PRED.\n");
	for (int i = 0; i < hp->heap_size; i++) {
		if (hp->nodes[i]->pi != NULL) {
			printf ("%d     %d    %d    %d\n", hp->nodes[i]->sp_est,
					hp->nodes[i]->v_id+offset, hp->nodes[i]->index,
					hp->nodes[i]->pi->v_id+offset);
		} else {
			printf ("%d    %d    %d    %s\n", hp->nodes[i]->sp_est,
					hp->nodes[i]->v_id+offset, hp->nodes[i]->index,
					"NULL");
		}

	}
	printf ("\n");
	return;
}
