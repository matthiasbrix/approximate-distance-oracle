#include "thorupzwick.h"
#include <time.h>

// #define DEBUG

void print_seqs (struct node **A, int k, int *seqsizes)
{
	printf("Print out all A_i sequences\n");
	for (int i = 0; i < k; i++) {
		if (seqsizes[i] > 0) {
			for (int j = 0; j < seqsizes[i]; j++)
				printf ("A_{%d,%d} v_id:%d spest:%d\n", i, j,
						A[i][j].v_id, A[i][j].sp_est);
		}
	}
}

// TODO: implement modified dijkstra
struct node *dijkstra_alg_tz (struct Graph *graph, int s)
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
				v->sp_est = u->sp_est + s->weight;
				v->pi = u;
				decrease_key (Q, v);
			}
		}
		i += 1;
	}
	return S;
}

// Should have at most time complexity O(kn^{1+1/k})
void prepro (struct Graph *graph, int k)
{
	// array of k pointers
	struct node *A[k];
	// TODO: Change to adjlist or spørg Christian om det er ok.
	struct heap_t *S = initialise_single_source (graph, 0);
	struct node tmp_arr[graph->V];
	int seqsizes[k];

	// A_k <- Ø
	A[k] = NULL;
	A[0] = malloc (graph->V * sizeof(struct node));

	// A_0 <- V. Running time O(n)
	for (unsigned int i = 0; i < graph->V; i++) {
		memcpy (&A[0][i], S->nodes[i], sizeof(struct node));
	}

	seqsizes[0] = S->heap_size;
	// Set the seed
	srand((unsigned)time(NULL));

	for (int i = 1; i <= k-1; i++) {
		seqsizes[i] = 0;
		for (int j = 0; j < seqsizes[i-1]; j++) {
			// Generates 0.0 - 1.0
			double rnd = (double)rand()/RAND_MAX;
			// Check random number is <= n^{-1/k}
			if (rnd <= pow (graph->V, -1.0/(double)k)) {
				struct node *t = &A[i-1][j];
				#ifdef DEBUG
				printf ("added to A_%d from A_%d %d %d\n", i, i-1,
						t->v_id, t->sp_est);
				#endif
				memcpy (&tmp_arr[seqsizes[i]], t,
						sizeof (struct node));
				seqsizes[i] += 1;
			}
		}
		A[i] = malloc (seqsizes[i] * sizeof (struct node));

		for (int l = 0; l < seqsizes[i]; l++) {
			memcpy (&A[i][l], &tmp_arr[l], sizeof (struct node));
		}
	}
	#ifdef DEBUG
	print_seqs (A, k, seqsizes);
	#endif
	// The added node w will always have the vertex id of the no. of vertices
	for (unsigned int i = 0; i < graph->V; i++) {
		add_edges (graph, graph->V, i, 0);
	}
	graph->V += 1;
	// TODO: Add vertex s to adjacency list, and connect weight 0 for every w in A_i. Call add node and add edges
	/* for (int i = k-1; i >= 0; i--) { */

	/* } */

	return;
}

// Hardcoded tests from various sources
void hardcoded_tests ()
{
	// From the author
	struct Graph* graph_1 = init_graph (5);
	add_edges (graph_1, 0, 1, 3);
	add_edges (graph_1, 0, 2, 5);
	add_edges (graph_1, 1, 2, 1);
	add_edges (graph_1, 2, 3, 10);
	add_edges (graph_1, 3, 4, 2);
	pp_graph (graph_1);
	struct node *S_1 = dijkstra_alg (graph_1, 0);
	pp_nodes (S_1, 5);

	printf ("------------------------\n");
	// From: http://www.geeksforgeeks.org/greedy-algorithms-set-7-dijkstras-algorithm-for-adjacency-list-representation/
	struct Graph* graph = init_graph (9);
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
	pp_graph (graph);
	struct node *S_2 = dijkstra_alg (graph, 0);
	pp_nodes (S_2, 9);

	printf ("------------------------\n");
	// From CLRS p. 659 (though it is directed)
	struct Graph* graph_3 = init_graph (5);
	add_edges (graph_3, 0, 1, 10);
	add_edges (graph_3, 0, 2, 5);
	add_edges (graph_3, 0, 4, 7);
	add_edges (graph_3, 1, 2, 2);
	add_edges (graph_3, 1, 3, 1);
	add_edges (graph_3, 2, 3, 9);
	add_edges (graph_3, 2, 4, 2);
	add_edges (graph_3, 3, 4, 4);
	pp_graph (graph_3);
	struct node *S_3 = dijkstra_alg (graph_3, 0);
	pp_nodes (S_3, 5);
}

void test_prepro ()
{
	struct Graph* graph_3 = init_graph (5);
	add_edges (graph_3, 0, 1, 10);
	add_edges (graph_3, 0, 2, 5);
	add_edges (graph_3, 0, 4, 7);
	add_edges (graph_3, 1, 2, 2);
	add_edges (graph_3, 1, 3, 1);
	add_edges (graph_3, 2, 3, 9);
	add_edges (graph_3, 2, 4, 2);
	add_edges (graph_3, 3, 4, 4);
	int k = 3;
	prepro (graph_3, k);
	pp_graph (graph_3);
}

void run (const char *fname_read, const char *fname_write)
{
	int V = count_vertices (fname_read);
	struct Graph *graph = init_graph (V);
	read_from_file (graph, fname_read);
	struct node *S = dijkstra_alg (graph, 1);
	write_to_csv (fname_write, S, V);
}

int main (int argc, char *argv[])
{
	if (argc < 3 || (argc % 2 != 1)) {
		printf ("No input and output arguments or input/output does not match!\n");
		return EXIT_FAILURE;
	}

	for (int i = 1; i < argc; i++) {
		const char *fname_read = argv[1];
		const char *fname_write = argv[2];
		run (fname_read, fname_write);
	};

	test_prepro ();
	/* hardcoded_tests (); */

	return EXIT_SUCCESS;
}
