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
			printf ("vertex: %d v.d: %d v.pi %d\n", S[j].v_id+offset,
					S[j].sp_est, S[j].pi->v_id+offset);
		}
	}

	return;
}
