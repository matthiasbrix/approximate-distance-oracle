#include "graph.h"

#define DEBUG

struct Graph* init_graph (int V)
{
	struct Graph *graph = malloc (sizeof (struct Graph));
	graph->V = V;
	graph->adjlists = malloc ((V) * sizeof (struct Adjlist));
	for (int i = 0; i < V; i++) {
		graph->adjlists[i].head = NULL;
		graph->adjlists[i].nd = NULL;
	}
	return graph;
}

struct AdjListNode* add_node (int v, int weight)
{
	struct AdjListNode *node = malloc (sizeof (struct AdjListNode));
	node->v_id = v;
	node->weight = weight;
	node->next = NULL;

	return node;
}

void add_edges (struct Graph *graph, int u, int v, unsigned int w)
{
	struct AdjListNode *node = add_node (v, w);
	node->next = graph->adjlists[u].head;
	graph->adjlists[u].head = node;
	// undirected -> add other way as well
	node = add_node (u, w);
	node->next = graph->adjlists[v].head;
	graph->adjlists[v].head = node;
}

struct Graph *copy_graph_struct (struct Graph* old_graph, struct heap_t *heap)
{
	struct Graph* new_graph = malloc (sizeof (struct Graph));
	new_graph->adjlists = malloc ((old_graph->V+1) * sizeof (struct Adjlist));
	// cpy all adjlists to write graph struct
	for (unsigned int a = 0; a < old_graph->V; a++) {
		memcpy (&new_graph->adjlists[a], &old_graph->adjlists[a], sizeof (struct Adjlist));
		memcpy (&new_graph->adjlists[a].nd, &heap->nodes[a], sizeof (struct node*));
	}
	new_graph->V = old_graph->V;

	return new_graph;
}

struct heap_t* initialise_single_source_tz (struct Graph *graph)
{

	struct heap_t *heap = malloc (sizeof (struct heap_t));
	heap->nodes = malloc ((graph->V+1) * sizeof(struct node*));

	if (heap == NULL || heap->nodes == NULL) {
		printf ("Pointer error of heap\n");
		return NULL;
	}

	int val = (int) INFINITY;

	for (unsigned int i = 0; i < graph->V; i++) {
		struct node *tmp = add_heap_node (i, val);
		heap->nodes[i] = tmp;
		memcpy (&graph->adjlists[i].nd, &heap->nodes[i], sizeof (struct node*));
	}

	heap->heap_size = graph->V;

	return heap;
}

// O(n)
struct heap_t* initialise_single_source (struct Graph *graph, int s)
{

	struct heap_t *heap = malloc (sizeof (struct heap_t));
	heap->nodes = malloc(graph->V * sizeof(struct node*));

	if (heap == NULL || heap->nodes == NULL) {
		printf ("Pointer error of heap\n");
		return NULL;
	}

	int val = (int) INFINITY;

	for (unsigned int i = 0; i < graph->V; i++) {
		struct node *tmp = add_heap_node (i, val);
		heap->nodes[i] = tmp;
		memcpy (&graph->adjlists[i].nd, &heap->nodes[i], sizeof (struct node*));
	}

	heap->heap_size = graph->V;
	struct node *v = graph->adjlists[s].nd;
	heap->nodes[v->v_id]->sp_est = 0;

	// TODO: min heapify / reorder here?

	return heap;
}

// The total run time is O(V lg V + E lg V), which is O(E lg V) because V is O(E) assuming a connected graph.
struct node* dijkstra_alg (struct Graph *graph, int s)
{
	struct heap_t *Q = initialise_single_source (graph, s);
	struct node *S = malloc (Q->heap_size * sizeof (struct node));
	int i = 0;
	while (Q->heap_size != 0) {
		struct node *u = extract_min (Q);
		memcpy (&S[i], u, sizeof (struct node));
		for (struct AdjListNode *s = graph->adjlists[u->v_id].head; s != NULL; s = s->next) {
			struct node *v = graph->adjlists[s->v_id].nd;
			if ((v != NULL) && (v->sp_est > u->sp_est + s->weight)) {
				int sp_est = u->sp_est + s->weight;
				decrease_key (Q, v, u, sp_est);
			}
		}
		i += 1;
	}
	return S;
}

// O(m lg n)
// pop takes lg n, V = E = m = n, loop m times
struct node *dijkstra_alg_tz (struct Graph *graph, struct heap_t *Q)
{
	struct node *S = malloc (Q->heap_size * sizeof (struct node));
	int i = 0;
	while (Q->heap_size != 0) {
		struct node *u = extract_min (Q);
		memcpy (&S[i], u, sizeof (struct node));
		for (struct AdjListNode *s = graph->adjlists[u->v_id].head;
			 s != NULL; s = s->next) {
			struct node *v = graph->adjlists[s->v_id].nd;
			if ((v != NULL) && (v->sp_est > u->sp_est + s->weight)) {
				int sp_est = u->sp_est + s->weight;
				decrease_key (Q, v, u, sp_est);
			}
		}
		i += 1;
	}

	return S;
}

void add_s_node_to_graph (struct Graph *graph, struct node *ai, int ailen)
{
	// The added node s will always have the vertex id of the no. of vertices
	for (int i = 0; i < ailen; i++) {
		add_edges (graph, graph->V, ai[i].v_id, 0);
	}
	return;
}

void pp_graph (struct Graph* graph)
{
	printf ("PRINT ADJLIST RESULTS\n");
	// Skip first, which is not exist in DIMACS files
	for (unsigned int i = 0; i < graph->V; i++) {
		struct AdjListNode* node = graph->adjlists[i].head;
		printf("vertex: %d", i);
		while (node) {
			printf(" -> v:%i w:%d", node->v_id,
				   node->weight);
			node = node->next;
		}
		printf("\n");
	}
}

void pp_nodes (struct node *S, int i)
{
	printf ("PRINT SSP RESULTS\n");
	for (int j = 0; j < i; j++) {
		if (S[j].pi == NULL) {
			printf ("vertex: %d v.d: %d v.pi NULL\n", S[j].v_id, S[j].sp_est);
		} else {
			printf ("vertex: %d v.d: %d v.pi %d\n", S[j].v_id, S[j].sp_est, S[j].pi->v_id);
		}
	}

	return;
}
