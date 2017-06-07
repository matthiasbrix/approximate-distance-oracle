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
 * @heap: heap pointer
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
	struct heap *QI = malloc (sizeof (struct heap));
	QI->nodes = malloc (gf->V * sizeof(struct node*));
	struct heap *QG = malloc (sizeof (struct heap));
	QG->nodes = malloc (gf->V * sizeof(struct node*));
	int *in_QI_heap = calloc (gf->V, sizeof (int));
	int *in_QG_heap = calloc (gf->V, sizeof (int));
	int *relaxed_1 = calloc (gf->V, sizeof(int));
	int *relaxed_2 = calloc (gf->V, sizeof(int));

	struct node *S_1 = malloc (gf->V * sizeof (struct node));
	memset (S_1, 0, gf->V * sizeof (struct node));
	struct node *S_2 = malloc (gf->V * sizeof (struct node));
	memset (S_2, 0, gf->V * sizeof (struct node));

	QI->heap_size = 0;
	QG->heap_size = 0;
	struct graph *gb = copy_graph_struct (gf, QG);
	min_heap_insert (QI, u, 0, gf);
	min_heap_insert (QG, v, 0, gb);
	int best_estimate = (int) INFINITY;

	while (QI->heap_size != 0 && QG->heap_size != 0) {
		int topqi = (minimum (QI))->sp_est;
		int topqg = (minimum (QG))->sp_est;

		if (topqi < topqg) {
			struct node *fir = extract_min (QI);
			memcpy (&S_1[fir->v_id], fir, sizeof(struct node));
			for (struct adjlistnode *s = gf->adjlists[fir->v_id].head;
				 s != NULL; s = s->next) {
				struct node *v = gf->adjlists[s->v_id].nd;
				int sp_est = fir->sp_est + s->weight;
				if (!relaxed_1[s->v_id] && !in_QI_heap[s->v_id]) {
					min_heap_insert (QI, s->v_id, sp_est, gf);
					gf->adjlists[s->v_id].nd->pi = fir;
					in_QI_heap[s->v_id] = sp_est;
				} else if (v != NULL && (v->sp_est > sp_est) && !relaxed_1[s->v_id]) {
					decrease_key (QI, v, fir, sp_est);
					in_QI_heap[s->v_id] = sp_est;
				}
			}
			relaxed_1[fir->v_id] = 1;
			if (relaxed_1[fir->v_id] && relaxed_2[fir->v_id]) {
				if (best_estimate >= S_1[fir->v_id].sp_est + S_2[fir->v_id].sp_est) {
					best_estimate = S_1[fir->v_id].sp_est + S_2[fir->v_id].sp_est;
				} else {
					return best_estimate;
				}
			} else if (in_QI_heap[fir->v_id] && in_QG_heap[fir->v_id] &&
				best_estimate >= in_QI_heap[fir->v_id] + in_QG_heap[fir->v_id]) {
				best_estimate = in_QI_heap[fir->v_id] + in_QG_heap[fir->v_id];
			}
		} else {
			struct node *sec = extract_min (QG);
			memcpy (&S_2[sec->v_id], sec, sizeof(struct node));
			for (struct adjlistnode *s = gb->adjlists[sec->v_id].head;
				 s != NULL; s = s->next) {
				struct node *v = gb->adjlists[s->v_id].nd;
				int sp_est = sec->sp_est + s->weight;
				if (!relaxed_2[s->v_id] && !in_QG_heap[s->v_id]) {
					min_heap_insert (QG, s->v_id, sp_est, gb);
					gb->adjlists[s->v_id].nd->pi = sec;
					in_QG_heap[s->v_id] = sp_est;
				} else if (v != NULL && (v->sp_est > sp_est) && !relaxed_2[s->v_id]) {
					decrease_key (QG, v, sec, sp_est);
					in_QG_heap[s->v_id] = sp_est;
				}
			}
			relaxed_2[sec->v_id] = 1;
			if (relaxed_1[sec->v_id] && relaxed_2[sec->v_id]) {
				if (best_estimate >= S_1[sec->v_id].sp_est + S_2[sec->v_id].sp_est) {
					best_estimate = S_1[sec->v_id].sp_est + S_2[sec->v_id].sp_est;
				} else {
					return best_estimate;
				}
			} else if (in_QG_heap[sec->v_id] && in_QI_heap[sec->v_id]) {
				if (best_estimate >= in_QI_heap[sec->v_id] + in_QG_heap[sec->v_id]) {
					best_estimate = in_QI_heap[sec->v_id] + in_QG_heap[sec->v_id];
				}
			}
		}
	}

	free_heap (QI);
	free_heap (QG);
	FREE (S_1);
	FREE (S_2);
	FREE (in_QI_heap);
	FREE (in_QG_heap);
	FREE (relaxed_1);
	FREE (relaxed_2);

	return best_estimate;
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
