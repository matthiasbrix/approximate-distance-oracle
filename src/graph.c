#include "graph.h"

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

struct adjlistnode *add_adj_node (int v, int weight)
{
	struct adjlistnode *node = malloc (sizeof (struct adjlistnode));
	node->v_id = v;
	node->weight = weight;
	node->next = NULL;

	return node;
}

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

/* struct graph *copy_graph_struct (struct graph *old_graph, struct heap *heap) */
/* { */
/* 	struct graph *new_graph = malloc (sizeof (struct graph)); */
/* 	new_graph->adjlists = malloc ((old_graph->V+1) * sizeof (struct adjlist)); */
/* 	// cpy all adjlists to write graph struct */
/* 	for (unsigned int a = 0; a < old_graph->V; a++) { */
/* 		memcpy (&new_graph->adjlists[a], &old_graph->adjlists[a], sizeof (struct adjlist)); */
/* 		memcpy (&new_graph->adjlists[a].nd, &heap->nodes[a], sizeof (struct node*)); */
/* 	} */
/* 	new_graph->V = old_graph->V; */

/* 	return new_graph; */
/* } */

// O(n)
struct heap *initialise_single_source_tz (struct graph *graph)
{
	struct heap *heap = malloc (sizeof (struct heap));
	// One extra slot for node s (later used for dijkstra)
	heap->nodes = malloc ((graph->V+1) * sizeof(struct node*));

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

	return heap;
}

// O((m + n) lg n)
// pop takes lg n, V = E = m = n, loop m times
struct node *dijkstra_alg_tz (struct graph *graph, struct heap *Q)
{
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
		// avoiding the u node can be relaxed again
		graph->adjlists[u->v_id].nd = NULL;
	}
	return S;
}

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
