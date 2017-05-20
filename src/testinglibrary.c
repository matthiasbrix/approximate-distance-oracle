#include "testinglibrary.h"
#include "sys/types.h"
#include "sys/sysinfo.h"

// VM

// cat /proc/cpuinfo

// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

void test () {
	struct node *ja = (malloc (1024*1024*20 * sizeof(struct node)));;
	ja = ja;
	printf ("2 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());
}

void test_prepro ()
{
	printf ("1 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());
	struct node *hi = (malloc (1024*1024*20 * sizeof(struct node)));
	printf ("2 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());
	hi = hi;
	/* free (hi); */
	printf ("3 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());
	struct node *hi2 = (malloc (1024*1024*20 * sizeof(struct node)));
	printf ("4 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());
	hi2 = hi2;
	/* free (hi2); */

	printf ("5 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());
	struct node *hi3 = (malloc (1024*1024*20 * sizeof(struct node)));
	printf ("5 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());
	hi3 = hi3;
	test ();
	printf ("6 %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());

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
	printf ("Memory before running prepro = %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());
	struct prepro *pp = malloc (sizeof (struct prepro));
	pp->success = false;
	while (!pp->success) {
		pp = prepro (graph, k);
		sleep (1);
	}
	clock_t end = clock();
	cpu_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf ("Time spent on prepro Thorup-Zwick: %f\n", cpu_time_spent);
	printf ("Memory usage of prepro = %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());

	clock_t begin2 = clock();
	d = dist (&pp->nodes[u], &pp->nodes[v], pp->bunchlist);
	clock_t end2 = clock();
	cpu_time_spent = (double)(end2 - begin2) / CLOCKS_PER_SEC;
	printf ("Time spent on query Thorup-Zwick: %f\n", cpu_time_spent);
	printf ("Result of Thorup-Zwick dist (%d, %d) = %d\n", u, v, d);
	printf ("Memory usage of dist = %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());

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
	printf ("Memory usage of Dijkstra = %d KB %d KB %d KB\n", get_vm_peak(), get_current_vm (), get_current_pm());
}

// Hardcoded tests from various sources
void hardcoded_tests ()
{
	// From the author
	struct graph* graph_1 = init_graph (5);
	add_edges (graph_1, 1, 2, 3);
	add_edges (graph_1, 1, 3, 5);
	add_edges (graph_1, 2, 3, 1);
	add_edges (graph_1, 3, 4, 10);
	add_edges (graph_1, 4, 5, 2);
	pp_graph (graph_1);
	struct node *S_1 = dijkstra_alg (graph_1, 0);
	pp_nodes (S_1, 5);

	printf ("------------------------\n");
	// From: http://www.geeksforgeeks.org/greedy-algorithms-set-7-dijkstras-algorithm-for-adjacency-list-representation/
	struct graph* graph = init_graph (9);
	add_edges(graph, 1, 2, 4);
	add_edges(graph, 1, 8, 8);
	add_edges(graph, 2, 3, 8);
	add_edges(graph, 2, 8, 11);
	add_edges(graph, 3, 4, 7);
	add_edges(graph, 3, 9, 2);
	add_edges(graph, 3, 6, 4);
	add_edges(graph, 4, 5, 9);
	add_edges(graph, 4, 6, 14);
	add_edges(graph, 5, 6, 10);
	add_edges(graph, 6, 7, 2);
	add_edges(graph, 7, 8, 1);
	add_edges(graph, 7, 9, 6);
	add_edges(graph, 8, 9, 7);
	pp_graph (graph);
	struct node *S_2 = dijkstra_alg (graph, 3);
	pp_nodes (S_2, 9);

	printf ("------------------------\n");
	// From CLRS p. 659 (though it is directed)
	struct graph* graph_3 = init_graph (5);
	add_edges (graph_3, 1, 2, 10);
	add_edges (graph_3, 1, 3, 5);
	add_edges (graph_3, 1, 5, 7);
	add_edges (graph_3, 2, 3, 2);
	add_edges (graph_3, 2, 4, 1);
	add_edges (graph_3, 3, 4, 9);
	add_edges (graph_3, 3, 5, 2);
	add_edges (graph_3, 4, 5, 4);
	pp_graph (graph_3);
	struct node *S_3 = dijkstra_alg (graph_3, 0);
	pp_nodes (S_3, 5);
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
