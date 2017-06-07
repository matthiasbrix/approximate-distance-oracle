#include "testinglibrary.h"

int bidirectional_dijkstra (struct graph *graph, int xi, int xg);

void test_prepro ()
{
	int k, n, u, v, d;
	double cpu_time_spent;
	k = 3, n = 5, u = 1, v = 4;
	struct graph *graph = init_graph (n);
	add_edges (graph, 0, 1, 10);
	add_edges (graph, 0, 2, 5);
	add_edges (graph, 0, 4, 7);
	add_edges (graph, 1, 2, 2);
	add_edges (graph, 1, 3, 1);
	add_edges (graph, 2, 3, 9);
	add_edges (graph, 2, 4, 2);
	add_edges (graph, 3, 4, 4);

	clock_t begin = clock();
	printf ("Memory before running prepro = %d KB %d KB\n", get_vm_peak(), get_current_vm ());
	struct prepro *pp = malloc (sizeof (struct prepro));
	pp->success = false;
	while (!pp->success) {
		pp = prepro (graph, k);
		sleep (1);
	}
	clock_t end = clock();
	cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf ("Time spent on prepro Thorup-Zwick: %f\n", cpu_time_spent);
	printf ("Memory usage of prepro = %d KB %d KB\n", get_vm_peak(), get_current_vm ());

	clock_t begin2 = clock();
	d = dist (&pp->nodes[u], &pp->nodes[v], pp->bunchlist);
	clock_t end2 = clock();
	cpu_time_spent = (double)(end2 - begin2) / CLOCKS_PER_SEC;
	printf ("Time spent on query Thorup-Zwick: %f\n", cpu_time_spent);
	printf ("Result of Thorup-Zwick dist (%d, %d) = %d\n", u, v, d);
	printf ("Memory usage of dist = %d KB %d KB\n", get_vm_peak(), get_current_vm ());

	graph = init_graph (n);
	add_edges (graph, 0, 1, 10);
	add_edges (graph, 0, 2, 5);
	add_edges (graph, 0, 4, 7);
	add_edges (graph, 1, 2, 2);
	add_edges (graph, 1, 3, 1);
	add_edges (graph, 2, 3, 9);
	add_edges (graph, 2, 4, 2);
	add_edges (graph, 3, 4, 4);

	clock_t begin3 = clock();
	struct node *S = dijkstra_alg (graph, u);
	clock_t end3 = clock();
	cpu_time_spent = (double)(end3 - begin3) / CLOCKS_PER_SEC;
	printf ("Result of Dijkstra SSP (%d, %d) = %d\n", u, v, S[v-offset].sp_est);
	printf ("Time spent on running Dijkstra: %f\n", cpu_time_spent);
	printf ("Memory usage of Dijkstra = %d KB %d KB\n", get_vm_peak(), get_current_vm ());

	graph = init_graph (n);
	add_edges (graph, 0, 1, 10);
	add_edges (graph, 0, 2, 5);
	add_edges (graph, 0, 4, 7);
	add_edges (graph, 1, 2, 2);
	add_edges (graph, 1, 3, 1);
	add_edges (graph, 2, 3, 9);
	add_edges (graph, 2, 4, 2);
	add_edges (graph, 3, 4, 4);
	printf ("BIDIRECTIONAL DIJKSTRA %d\n", bidirectional_dijkstra (graph, u, v));

	return;
}

// Hardcoded tests from various sources
void hardcoded_tests ()
{
	// From the author
	struct graph* graph_1 = init_graph (5);
	add_edges (graph_1, 0, 1, 3);
	add_edges (graph_1, 0, 2, 5);
	add_edges (graph_1, 1, 2, 1);
	add_edges (graph_1, 2, 3, 10);
	add_edges (graph_1, 3, 4, 2);
	/* struct node *S_1 = dijkstra_alg (graph_1, 0); */
	/* pp_nodes (S_1, 5); */
	int out;
	out = bidirectional_dijkstra (graph_1, 0, 4);
	printf ("out: %d\n", out);
	pp_graph (graph_1);

	printf ("------------------------\n");
	// From: http://www.geeksforgeeks.org/greedy-algorithms-set-7-dijkstras-algorithm-for-adjacency-list-representation/
	struct graph* graph = init_graph (9);
	add_edges(graph, 0, 1, 4);
	add_edges(graph, 0, 7, 8);
	add_edges(graph, 1, 2, 8);
	add_edges(graph, 1, 7, 11);
	add_edges(graph, 2, 3, 7);
	add_edges(graph, 2, 8, 2);
	add_edges(graph, 2, 5, 4);
	add_edges(graph, 3, 4, 9);
	add_edges(graph, 3, 5, 14);
	add_edges(graph, 4, 5, 10);
	add_edges(graph, 5, 6, 2);
	add_edges(graph, 6, 7, 1);
	add_edges(graph, 6, 8, 6);
	add_edges(graph, 7, 8, 7);
	/* pp_graph (graph); */
	/* struct node *S_2 = dijkstra_alg (graph, 3); */
	out = bidirectional_dijkstra (graph, 0, 1);
	printf ("geeks: out: 1 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 2);
	printf ("geeks: out: 2 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 3);
	printf ("geeks: out: 3 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 4);
	printf ("geeks: out: 4 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 5);
	printf ("geeks: out: 5 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 6);
	printf ("geeks: out: 6 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 7);
	printf ("geeks: out: 7 %d\n", out);
	out = bidirectional_dijkstra (graph, 0, 8);
	printf ("geeks: out: 8 %d\n", out);
	/* printf ("------------------------\n"); */

	/* // From CLRS p. 659 (though it is directed) */
	struct graph* graph_3 = init_graph (5);
	add_edges (graph_3, 0, 1, 10);
	add_edges (graph_3, 0, 2, 5);
	add_edges (graph_3, 0, 4, 7);
	add_edges (graph_3, 1, 2, 2);
	add_edges (graph_3, 1, 3, 1);
	add_edges (graph_3, 2, 3, 9);
	add_edges (graph_3, 2, 4, 2);
	add_edges (graph_3, 3, 4, 4);
	/* pp_graph (graph_3); */
	/* pp_graph (graph_3); */
	struct node *S_3 = dijkstra_alg (graph_3, 0);
	pp_nodes (S_3, 5);
	out = bidirectional_dijkstra (graph_3, 0, 1);
	printf ("out: %d\n", out);
	out = bidirectional_dijkstra (graph_3, 0, 2);
	printf ("out: %d\n", out);
	out = bidirectional_dijkstra (graph_3, 0, 3);
	printf ("out: %d\n", out);
	out = bidirectional_dijkstra (graph_3, 0, 4);
	printf ("out: %d\n", out);

	// http://liu.diva-portal.org/smash/get/diva2:114159/FULLTEXT01 (p. 26)
	struct graph* graph_5 = init_graph (6);
	add_edges (graph_5, 0, 1, 3);
	add_edges (graph_5, 0, 2, 1);
	add_edges (graph_5, 1, 3, 1);
	add_edges (graph_5, 2, 4, 4);
	add_edges (graph_5, 4, 5, 1);
	pp_graph (graph_5);
	out = bidirectional_dijkstra (graph_5, 0, 5);
	printf ("her: out: %d\n", out);

	struct graph* graph_4 = init_graph (3);
	add_edges (graph_4, 0, 1, 1);
	add_edges (graph_4, 1, 2, 1);
	add_edges (graph_4, 0, 2, 5);
	pp_graph (graph_4);
	out = bidirectional_dijkstra (graph_4, 0, 2);
	printf ("out: %d\n", out);

	struct graph* graph_6 = init_graph (9);
	add_edges (graph_6, 0, 1, 4);
	add_edges (graph_6, 0, 2, 5);
	add_edges (graph_6, 0, 3, 7);
	add_edges (graph_6, 3, 5, 4);
	add_edges (graph_6, 3, 6, 6);
	add_edges (graph_6, 1, 4, 9);
	add_edges (graph_6, 2, 5, 3);
	add_edges (graph_6, 5, 7, 5);
	add_edges (graph_6, 5, 4, 12);
	add_edges (graph_6, 7, 8, 3);
	add_edges (graph_6, 4, 8, 13);
	add_edges (graph_6, 6, 8, 9);
	pp_graph (graph_6);
	out = bidirectional_dijkstra (graph_6, 1, 6);
	printf ("out: %d\n", out);
	struct node *S = dijkstra_alg (graph_6, 1);
	pp_nodes (S, 9);
	exit (0);
}

// O(n)
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

// O(m + n lg n)
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

// where they suggest the correct stopping condition is that the sum of the values at the top of each heap (forward and reverse) >= the length of the shortest path seen so far. Once that condition holds, the shortest path seen so far is the shortest path.
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
	struct graph *gb = copy_graph_struct (gf, QI);
	min_heap_insert (QI, u, 0, gf);
	min_heap_insert (QG, v, 0, gb);
	int best_estimate = (int) INFINITY;

	while (QI->heap_size != 0 && QG->heap_size != 0) {
		int topqi = (minimum (QI))->sp_est;
		int topqg = (minimum (QG))->sp_est;
		if (topqi == (int) INFINITY || topqg == (int) INFINITY) {
			return (int) INFINITY;
		}
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
					in_QI_heap[s->v_id] = 1;
				} else if (v != NULL && v->sp_est > sp_est && !relaxed_1[s->v_id]) {
					decrease_key (QI, v, fir, sp_est);
				}
			}
			relaxed_1[fir->v_id] = 1;
			if (relaxed_1[fir->v_id] && relaxed_2[fir->v_id]) {
				if (best_estimate >= S_1[fir->v_id].sp_est + S_2[fir->v_id].sp_est) {
					best_estimate = S_1[fir->v_id].sp_est + S_2[fir->v_id].sp_est;
				} else {
					return best_estimate;
				}
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
				} else if (v != NULL && v->sp_est > sp_est && !relaxed_2[s->v_id]) {
					decrease_key (QG, v, sec, sp_est);
				}
			}
			relaxed_2[sec->v_id] = 1;
			if (relaxed_1[sec->v_id] && relaxed_2[sec->v_id]) {
				if (best_estimate >= S_1[sec->v_id].sp_est + S_2[sec->v_id].sp_est) {
					best_estimate = S_1[sec->v_id].sp_est + S_2[sec->v_id].sp_est;
				} else {
					return best_estimate;
				}
			}
		}
	}

	return best_estimate;

}
