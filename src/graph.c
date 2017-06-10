#include "graph.h"

/**
 * init_graph - initialising a graph structure
 * @n: number of vertices to be allocated
 * Running time: O(n).
 * Allocating graph and adjacency lists structure for later user
 */
struct graph *init_graph (int n)
{
	struct graph *graph = malloc (sizeof (struct graph));
	graph->V = n;
	graph->adjlists = malloc ((n) * sizeof (struct adjlist));
	for (int i = 0; i < n; i++) {
		graph->adjlists[i].head = NULL;
		graph->adjlists[i].nd = NULL;
	}
	return graph;
}

/**
 * add_adj_node - adding adjacency node to list
 * @v: number of vertices to be allocated
 * @weight: weight of edge for node
 * Running time: O(1).
 * Utility function for allocating and adding adjacency node
 * pointer to an existing graph
 */
struct adjlistnode *add_adj_node (int v, int weight)
{
	struct adjlistnode *node = malloc (sizeof (struct adjlistnode));
	node->v_id = v;
	node->weight = weight;
	node->next = NULL;

	return node;
}

/**
 * add_edges - adding adjacency edge to list, connecting vertices (u, v)
 * @graph: graph struct
 * @u: integer u representing a vertex
 * @v: integer v representing a vertex
 * @w: weight of edge (u, v)
 * Running time: O(1).
 * Utility function for allocating and adding adjacency edges
 * between the vertices u and v
 */
void add_edges (struct graph *graph, int u, int v, unsigned int w)
{
	struct adjlistnode *node = add_adj_node (v, w);
	node->next = graph->adjlists[u].head;
	graph->adjlists[u].head = node;
	// undirected -> add other way as well
	node = add_adj_node (u, w);
	node->next = graph->adjlists[v].head;
	graph->adjlists[v].head = node;
}

/**
 * copy_graph_struct - deep copy of a graph struct
 * @old_graph: graph struct to be copied
 * @heap: heap pointer of which the nodes the graph points to nodes of heap
 * Running time: O(n + m)
 * deep copying all content from old_graph to a new. Also,
 * we keep a bidirectional pointer between graph and heap for lookup
 */
struct graph *copy_graph_struct (struct graph *old_graph, struct heap *heap)
{
	struct graph *graph = init_graph (old_graph->V+1);
	graph->V -= 1;
	for (unsigned int i = 0; i < graph->V; i++) {
		struct adjlistnode *node = old_graph->adjlists[i].head;
		memcpy (&graph->adjlists[i].nd, &heap->nodes[i], sizeof (struct node*));
		while (node) {
			if ((int) i < node->v_id) {
				add_edges (graph, i, node->v_id, node->weight);
			}
			node = node->next;
		}
	}
	return graph;
}

/**
 * initialise_single_source_tz - initialising heap for Dijkstra's alg.
 * @graph: for the graph pointers
 * Running time: O(n)
 * This copies pointers of heap nodes to graph, to keep bidirectional
 * pointers. Also it initialises a heap but with no single source actually
 * - this is set manually outside the function
 */
struct heap *initialise_single_source_tz (unsigned int n)
{
	struct heap *heap = malloc (sizeof (struct heap));
	// One extra slot for node s (later used for dijkstra)
	heap->nodes = malloc ((n+1) * sizeof(struct node*));

	if (heap == NULL || heap->nodes == NULL) {
		perror ("Pointer error of heap\n");
		exit (-1);
	}

	int val = (int) INFINITY;

	for (unsigned int i = 0; i < n; i++) {
		heap->nodes[i] = add_node (i, val, i);
	}

	heap->heap_size = n;

	return heap;
}

/**
 * dijkstra_alg_tz - Dijkstra's algorithm
 * @graph: the graph G = (V, E)
 * @Q: the heap to work with.
 * Running time: O((m + n) lg n)
 * Executing dijkstra's algorithm but without a specified single source as
 * initialise single source procedure is required to be called on before hand
 * The algorithm is implemented with a priorityqueue as a data structure.
 * pop takes lg n, V = E = m = n, loop m times
 */
struct node *dijkstra_alg_tz (struct graph *graph, struct heap *Q)
{
	struct node *S = malloc (Q->heap_size * sizeof (struct node));

	if (S == NULL) {
		perror ("Pointer error of S array\n");
		exit (-1);
	}

	while (Q->heap_size != 0) {
		struct node *u = extract_min (Q);
		memcpy (&S[u->v_id], u, sizeof (struct node));
		for (struct adjlistnode *s = graph->adjlists[u->v_id].head;
			 s != NULL; s = s->next) {
			struct node *v = graph->adjlists[s->v_id].nd;
			if ((v != NULL) && (v->sp_est > u->sp_est + s->weight)) {
				int sp_est = u->sp_est + s->weight;
				decrease_key (Q, v, u, sp_est);
			}
		}
		// avoiding the u node can be relaxed again
		graph->adjlists[u->v_id].nd = NULL;
	}

	free_heap (Q);
	return S;
}

/**
 * bidirectional_dijkstra - Bidirectional Dijkstra's algorithm
 * @gf: the graph G = (V, E) for the forward search
 * @u: source vertex
 * @v: target vertex
 * Running time: O((m + n) lg n)
 * Running Dijkstra's from the source (forward search) AND target vertex (target vertex).
 * We have two graph struct, one for the forward search, one for the backward search.
 * We start the execution by initialising two heaps, with u and v as single vertices.
 * Then we execute one search at a time.
 */
int bidirectional_dijkstra (struct graph *gf, int u, int v)
{
	// min-prioty queue for forward search
	struct heap *Q_1 = malloc (sizeof (struct heap));
	Q_1->nodes = malloc (gf->V * sizeof(struct node*));
	// min-prioty queue for backwards search
	struct heap *Q_2 = malloc (sizeof (struct heap));
	Q_2->nodes = malloc (gf->V * sizeof(struct node*));
	// Auxiliary arrays
	int *in_Q1_heap = calloc (gf->V, sizeof (int));
	int *in_Q2_heap = calloc (gf->V, sizeof (int));
	int *extracted_1 = calloc (gf->V, sizeof(int));
	int *extracted_2 = calloc (gf->V, sizeof(int));
	int rootqi;
	int rootqg;

	// For forward search
	struct node *S_1 = malloc (gf->V * sizeof (struct node));
	memset (S_1, 0, gf->V * sizeof (struct node));
	// For backwards search
	struct node *S_2 = malloc (gf->V * sizeof (struct node));
	memset (S_2, 0, gf->V * sizeof (struct node));

	if (Q_1 == NULL || Q_2 == NULL || in_Q1_heap == NULL || in_Q2_heap == NULL ||
		extracted_1 == NULL || extracted_2 == NULL || S_1 == NULL || S_2 == NULL) {
		perror ("Pointer error in bidirectional dijkstra.\n");
		return EXIT_FAILURE;
	}

	Q_1->heap_size = 0;
	Q_2->heap_size = 0;
	// Copy of graph for backwards search
	struct graph *gb = copy_graph_struct (gf, Q_2);
	min_heap_insert (Q_1, u, 0, gf);
	min_heap_insert (Q_2, v, 0, gb);
	int sp_estimate = (int) INFINITY;

	while (Q_1->heap_size != 0 && Q_2->heap_size != 0) {

		rootqi = (minimum (Q_1))->sp_est;
		rootqg = (minimum (Q_2))->sp_est;

		if (rootqi < rootqg) {
			// Forward search begins here
			struct node *u = extract_min (Q_1);
			memcpy (&S_1[u->v_id], u, sizeof(struct node));
			for (struct adjlistnode *s = gf->adjlists[u->v_id].head;
				 s != NULL; s = s->next) {
				struct node *v = gf->adjlists[s->v_id].nd;
				int sp_est = u->sp_est + s->weight;
				if (!extracted_1[s->v_id] && !in_Q1_heap[s->v_id]) {
					min_heap_insert (Q_1, s->v_id, sp_est, gf);
					gf->adjlists[s->v_id].nd->pi = u;
					in_Q1_heap[s->v_id] = sp_est;
				} else if (v != NULL && (v->sp_est > sp_est) && !extracted_1[s->v_id]) {
					decrease_key (Q_1, v, u, sp_est);
					in_Q1_heap[s->v_id] = sp_est;
				}
			}
			extracted_1[u->v_id] = 1;
			// u intersects in S_1 and S_2
			if (extracted_1[u->v_id] && extracted_2[u->v_id]) {
				if (sp_estimate >= S_1[u->v_id].sp_est + S_2[u->v_id].sp_est) {
					sp_estimate = S_1[u->v_id].sp_est + S_2[u->v_id].sp_est;
				} else {
					return sp_estimate;
				}
				// u intersects in Q_1 and Q_2
			} else if (in_Q1_heap[u->v_id] && in_Q2_heap[u->v_id] &&
				sp_estimate >= in_Q1_heap[u->v_id] + in_Q2_heap[u->v_id]) {
				sp_estimate = in_Q1_heap[u->v_id] + in_Q2_heap[u->v_id];
			}
		} else {
			// Backwards search begins here
			struct node *u = extract_min (Q_2);
			memcpy (&S_2[u->v_id], u, sizeof(struct node));
			for (struct adjlistnode *s = gb->adjlists[u->v_id].head;
				 s != NULL; s = s->next) {
				struct node *v = gb->adjlists[s->v_id].nd;
				int sp_est = u->sp_est + s->weight;
				if (!extracted_2[s->v_id] && !in_Q2_heap[s->v_id]) {
					min_heap_insert (Q_2, s->v_id, sp_est, gb);
					gb->adjlists[s->v_id].nd->pi = u;
					in_Q2_heap[s->v_id] = sp_est;
				} else if (v != NULL && (v->sp_est > sp_est) && !extracted_2[s->v_id]) {
					decrease_key (Q_2, v, u, sp_est);
					in_Q2_heap[s->v_id] = sp_est;
				}
			}
			extracted_2[u->v_id] = 1;
			// u intersects in S_1 and S_2
			if (extracted_1[u->v_id] && extracted_2[u->v_id]) {
				if (sp_estimate >= S_1[u->v_id].sp_est + S_2[u->v_id].sp_est) {
					sp_estimate = S_1[u->v_id].sp_est + S_2[u->v_id].sp_est;
				} else {
					return sp_estimate;
				}
				// u intersects in Q_1 and Q_2
			} else if (in_Q1_heap[u->v_id] && in_Q2_heap[u->v_id] &&
				sp_estimate >= in_Q1_heap[u->v_id] + in_Q2_heap[u->v_id]) {
				sp_estimate = in_Q1_heap[u->v_id] + in_Q2_heap[u->v_id];
			}
		}
	}

	free_graph (gf);
	free_graph (gb);
	free_heap (Q_1);
	free_heap (Q_2);
	FREE (S_1);
	FREE (S_2);
	FREE (in_Q1_heap);
	FREE (in_Q2_heap);
	FREE (extracted_1);
	FREE (extracted_2);

	return sp_estimate;
}

/**
 * initialise_single_source - initialising a heap from a single source
 * @graph: graph
 * @s: source vertex id
 * Running time: O(n)
 */
struct heap *initialise_single_source (struct graph *graph, int s)
{
	struct heap *heap = malloc (sizeof (struct heap));
	heap->nodes = malloc(graph->V * sizeof(struct node*));

	if (heap == NULL || heap->nodes == NULL) {
		printf ("Pointer error of heap\n");
		return NULL;
	}

	int val = (int) INFINITY;

	for (unsigned int i = 0; i < graph->V; i++) {
		struct node *tmp = add_node (i, val, i);
		heap->nodes[i] = tmp;
		memcpy (&graph->adjlists[i].nd, &heap->nodes[i], sizeof (struct node*));
	}

	heap->heap_size = graph->V;
	struct node *v = graph->adjlists[s].nd;
	heap->nodes[v->v_id]->sp_est = 0;
	find_node_pos (heap, v->v_id);

	return heap;
}

/**
 * dijkstra_alg - Dijkstra's original algorithm
 * @graph: graph
 * @s: source vertex id
 * Running time: O((m + n) lg n)
 * Searching all shortest-paths from a single source, that is, s
 */
struct node *dijkstra_alg (struct graph *graph, int s)
{
	struct heap *Q = initialise_single_source (graph, s);
	struct node *S = malloc (Q->heap_size * sizeof (struct node));
	while (Q->heap_size != 0) {
		struct node *u = extract_min (Q);
		memcpy (&S[u->v_id], u, sizeof (struct node));
		for (struct adjlistnode *s = graph->adjlists[u->v_id].head;
			 s != NULL; s = s->next) {
			struct node *v = graph->adjlists[s->v_id].nd;
			if ((v != NULL) && (v->sp_est > u->sp_est + s->weight)) {
				int sp_est = u->sp_est + s->weight;
				decrease_key (Q, v, u, sp_est);
			}
		}
		graph->adjlists[u->v_id].nd = NULL;
	}
	free_heap (Q);
	return S;
}

/**
 * free_graph - Utility function to free the graph
 * @graph: graph
 * Running time: O(m + n)
 * Freeing all entities in the graph struct from memory
 */
void free_graph (struct graph *graph)
{
	for (int i = 0; i < (int) graph->V; i++) {
		struct adjlistnode *node = graph->adjlists[i].head;
		while (node) {
			struct adjlistnode *tmp = node->next;
			FREE (node);
			node = tmp;
		}
	}
	FREE (graph);
}

/**
 * pp_graph - Pretty print function for the graph struct
 * @graph: graph
 * Running time: O(m + n)
 * Printing all entities in a user friendly matter
 */
void pp_graph (struct graph *graph)
{
	printf ("PRINT ADJLIST RESULTS\n");
	for (unsigned int i = 0; i < graph->V; i++) {
		struct adjlistnode *node = graph->adjlists[i].head;
		printf("vertex: %d", i+offset);
		while (node) {
			printf(" -> v:%d w:%d", node->v_id+offset,
				   node->weight);
			node = node->next;
		}
		printf("\n");
	}
}

/**
 * pp_nodes - Pretty print function for the nodes
 * @S: node pointer
 * @i: current i <= k
 * Running time: O(n)
 * Printing all nodes result after Dijkstra. Results are
 * vertex id, shortest path from source vertex, and predecessor
 */
void pp_nodes (struct node *S, int i)
{
	printf ("PRINT SSP RESULTS\n");
	for (int j = 0; j < i; j++) {
		if (S[j].pi == NULL) {
			printf ("vertex: %d v.d: %d v.pi NULL\n", S[j].v_id+offset, S[j].sp_est);
		} else {
			printf ("vertex: %d v.d: %d v.pi %d\n", S[j].v_id+offset, S[j].sp_est, S[j].pi->v_id+offset);
		}
	}

	return;
}
