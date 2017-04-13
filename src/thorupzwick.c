#include "thorupzwick.h"
#include <time.h>

#define DEBUG

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

// TODO: Find afstande af i-centers til alle
struct node *dijkstra_alg_tz (struct Graph *graph, struct heap_t *Q)
{
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

void add_s_node_to_graph (struct Graph *graph, struct node *ai, int ailen)
{
	// The added node s will always have the vertex id of the no. of vertices
	for (int i = 0; i < ailen; i++) {
		add_edges (graph, graph->V, ai[i].v_id, 0);
	}

	return;
}

/*
Nu er der to steder, hvor Dijkstra bruges i Thorup-Zwick-preprocessing. Den ene er helt almindelig Dijkstra fra en enkelt knude, den anden er en modificeret version af Dijkstra, der bruges til at finde clusters. Jeg går ud fra, at det er den første, du tænker på. Målet er her at finde, for hvert i og for hver knude v, den nærmeste knude p_i(v) til v i A_i samt afstanden d(v,A_i) fra v til p_i(v). Det gøres ved at udvide G ved at tilføje en ny start-knude med kanter af vægt 0 til hver knude i A_i og så køre Dijkstra fra denne nye startknude i den udvidede graf.
 */

// Should have at most time complexity O(kn^{1+1/k})
void prepro (struct Graph *graph, int k)
{
	// array of k pointers
	struct node *A[k];
	// TODO: Change to adjlist or spørg Christian om det er ok.
	struct heap_t *S = initialise_single_sourc_tz (graph);
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
				memcpy (&tmp_arr[seqsizes[i]], &A[i-1][j],
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

	for (int i = k-1; i >= 0; i--) {

		if (seqsizes[i] == 0) {
			continue;
		}
		struct Graph *write_graph = malloc (sizeof (struct Graph));
		write_graph->adjlists = malloc ((graph->V+1) * sizeof (struct Adjlist));
		// cpy all adjlists to write graph struct
		for (unsigned int a = 0; a < graph->V; a++) {
			memcpy (&write_graph->adjlists[a], &graph->adjlists[a], sizeof (struct Adjlist));
		}
		write_graph->V = graph->V;
		// adding source vertex s to G, weight 0 to all other vertices in A_i
		printf ("write graph\n");
		pp_graph (write_graph);
		add_s_node_to_graph (write_graph, A[i], seqsizes[i]);
		struct node *tmp = add_heap_node (write_graph->V, 0);
		S->nodes[S->heap_size] = tmp;
		write_graph->adjlists[write_graph->V].nd = tmp;
		S->heap_size += 1;
		write_graph->V += 1;
		pp_graph (write_graph);
		printf ("write graph %d\n", i);
		/* // for every v in V */
		/* for (unsigned int v = 0; v < graph->V; v++) { */
		/* 	// compute d(A_i, v) */
		/* 	for (int j = 0; j < seqsizes[i]; j++) { */
		/* 		dijkstra_alg_tz (write_graph, S); */
		/* 	} */
		/* } */
		/* // Prepare to overwrite the s node with a new */
		S->heap_size -= 1;
		write_graph->V -= 1;
	}

	printf ("size: %d\n", S->heap_size);
	pp_heap (S);

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
