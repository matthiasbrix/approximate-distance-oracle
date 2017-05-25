#include "testinglibrary.h"

int bidirectional_dijkstra (struct graph *graph, int xi, int xg);

/* void test () { */
/* 	struct node *ja = (malloc (1024*1024*20 * sizeof(struct node)));; */
/* 	ja = ja; */
/* 	printf ("2 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm ()); */
/* } */

void test_prepro ()
{
	/* printf ("1 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm ()); */
	/* struct node *hi = (malloc (1024*1024*20 * sizeof(struct node))); */
	/* printf ("2 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm ()); */
	/* hi = hi; */
	/* /\* free (hi); *\/ */
	/* printf ("3 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm ()); */
	/* struct node *hi2 = (malloc (1024*1024*20 * sizeof(struct node))); */
	/* printf ("4 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm ()); */
	/* hi2 = hi2; */
	/* /\* free (hi2); *\/ */

	/* printf ("5 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm ()); */
	/* struct node *hi3 = (malloc (1024*1024*20 * sizeof(struct node))); */
	/* printf ("5 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm ()); */
	/* hi3 = hi3; */
	/* test (); */
	/* printf ("6 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm ()); */
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
	out = bidirectional_dijkstra (graph, 0, 2);
	printf ("out: %d\n", out);


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
	pp_graph (graph_3);
	/* pp_graph (graph_3); */
	struct node *S_3 = dijkstra_alg (graph_3, 0);
	pp_nodes (S_3, 5);
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

// O((m + n) lg n)
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
	return S;
}

// http://planning.cs.uiuc.edu/node50.html
// http://www.cs.princeton.edu/courses/archive/spr06/cos423/Handouts/EPP%20shortest%20path%20algorithms.pdf
int bidirectional_dijkstra (struct graph *graph, int xi, int xg)
{
	int *in_QI_heap = calloc (graph->V, sizeof (int));
	int *in_QG_heap = calloc (graph->V, sizeof (int));
	struct heap *QI = malloc (sizeof (struct heap));
	QI->nodes = malloc (graph->V * sizeof(struct node*));
	struct heap *QG = malloc (sizeof (struct heap));
	QG->nodes = malloc (graph->V * sizeof(struct node*));
	int *relaxed = calloc (graph->V, sizeof(int));
	int *relaxed_2 = calloc (graph->V, sizeof(int));

	struct node *S = malloc (graph->V * sizeof (struct node));
	struct node *S_2 = malloc (graph->V * sizeof (struct node));

	QI->heap_size = 0;
	QG->heap_size = 0;
	min_heap_insert (QI, xi, 0, graph);
	printf ("nej\n");
	min_heap_insert (QG, xg, 0, graph);

	in_QI_heap[xi] = 1;
	in_QG_heap[xg] = 1;

	int dist = 0;
	// Possible stopping criterion: a vertex is about to be scanned a second time
	//     * once in each direction
	//     v may not be on the shortest path
	// case 1: Begge er i heaps
	// case 2: x er i heap 1, men ikke i 2 heap (allerede relaxed)
	// case 3: x er i heap 1, y i heap 2, direkte forbundet. gemmer afstanden, som ikke er relaxed endnu, men i heap
	// case 4: direkte forbundet, men en anden vej er kortere (PROBLEM)

	while (QI->heap_size != 0 && QG->heap_size != 0) {
		pp_heap (QI);
		pp_heap (QG);
		if (QI->heap_size != 0) {
			struct node *x = extract_min (QI);
			in_QI_heap[x->v_id] = 0;
			relaxed[x->v_id] = 1;
			memcpy (&S[x->v_id], x, sizeof(struct node));
			// (in_QG_heap[x->v_id] <= QG->nodes[0]->sp_est) &&  else run dijkstra
			if ((x->v_id == xg || in_QG_heap[x->v_id] || relaxed_2[x->v_id])) {
				printf ("QI %d %d %d %d %d %d %d\n", (x->v_id == xg),in_QG_heap[x->v_id] , relaxed_2[x->v_id], x->v_id, x->sp_est, xg, S_2[x->v_id].sp_est);
				if (relaxed_2[x->v_id]) {
					return x->sp_est + S_2[x->v_id].sp_est;
				}
				printf ("test\n");
				return x->sp_est + in_QG_heap[x->v_id];
			} else {
				for (struct adjlistnode *s = graph->adjlists[x->v_id].head;
				 s != NULL; s = s->next) {
					printf ("test her %d %d %d\n", s->v_id, !(in_QG_heap[s->v_id]), !(in_QI_heap[s->v_id]));
					struct node *v = graph->adjlists[s->v_id].nd;
					int sp_est = x->sp_est + s->weight;
					if (!(in_QI_heap[s->v_id]) && !(relaxed[s->v_id])) {
						min_heap_insert (QI, s->v_id, sp_est, graph);
						graph->adjlists[s->v_id].nd->pi = x;
						in_QI_heap[s->v_id] = sp_est;
					} else {
						if ((v->sp_est > sp_est)) {
							decrease_key (QI, v, x, sp_est);
							in_QI_heap[s->v_id] = sp_est;
						}
					}
				}
			}
		}
		pp_heap (QG);
		if (QG->heap_size != 0) {
			struct node *x = extract_min (QG);
			in_QG_heap[x->v_id] = 0;
			relaxed_2[x->v_id] = 1;
			memcpy (&S_2[x->v_id], x, sizeof(struct node));
			// if (in_QI_heap[x->v_id] <= QI[0].sp_est) else run dijkstra
			if ((in_QI_heap[x->v_id] <= QI->nodes[0]->sp_est) && (x->v_id == xi || in_QI_heap[x->v_id] || relaxed[x->v_id])) {
				printf ("QG %d %d %d %d %d %d %d\n", (x->v_id == xi), in_QI_heap[x->v_id] , relaxed[x->v_id], x->v_id, x->sp_est, xg, S[x->v_id].sp_est);
				if (relaxed[x->v_id]) {
					return x->sp_est + S[x->v_id].sp_est;
				}
				printf ("test\n");
				return x->sp_est + in_QI_heap[x->v_id];
			}
			for (struct adjlistnode *s = graph->adjlists[x->v_id].head;
				 s != NULL; s = s->next) {
				struct node *v = graph->adjlists[s->v_id].nd;
				int sp_est = x->sp_est + s->weight;
				/* printf ("test %d %d\n", (in_QG_heap[s->v_id]), (in_QI_heap[s->v_id])); */
				if (!(in_QG_heap[s->v_id]) && !(relaxed_2[s->v_id])) {
					min_heap_insert (QG, s->v_id, sp_est, graph);
					graph->adjlists[s->v_id].nd->pi = x;
					in_QG_heap[s->v_id] = sp_est;
				} else {
					if ((v->sp_est > sp_est)) {
						decrease_key (QG, v, x, sp_est);
						in_QG_heap[s->v_id] = sp_est;
					}
				}
			}
		}
	}
	return dist;
}
