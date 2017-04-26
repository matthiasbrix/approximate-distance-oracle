#include "thorupzwick.h"
#include <time.h>
#include <assert.h>

#define DEBUG

// TODO:
void deallocate_heap (struct heap_t *heap)
{
	free (heap->nodes);
	free (heap);
}

// TODO:
void deallocate_graph (struct Graph *graph)
{
	free (graph->adjlists->nd);
	free (graph->adjlists->head->next);
	free (graph->adjlists->head);
	free (graph->adjlists);
	free (graph);
}

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

// TODO: relax the edge (u, v) only if u.d + l(u,v) < d(A_{i+1}, v)
// TODO: Christian - skal man bare køre ud fra w og ikke andet? Ikke noget med at forbinde w til andre?
// TODO: Hvad nu hvis A_i+1 er tom?
// TODO: SKal jeg remove edge?
// cluster C(w) is composed of all the vertices that are closer to w than any (i+1)-center
// Note that δ(A_{i+1}, v) was computed in the previous iteration so the test takes constant time.
// Note that if v not in C(w), then we never assign a finite distance, as by definition, v ∈ C(w) if and only if δ(w, v) < δ(A_i+1, v)
struct node *dijkstra_cluster_tz (struct Graph *graph, struct heap_t *Q,
								  struct node *w, int *dist, int i)
{
	struct node *S = malloc (Q->heap_size * sizeof (struct node));
	assert (S != NULL);
	int idx = 0;

	// w.d = 0
	Q->nodes[w->index]->sp_est = 0;
	int u = Q->nodes[w->v_id]->index;
	fix_positions (Q, u);

	#ifdef DEBUG
	int b;
	for (b = 0; b < (int) graph->V; b++)
		printf("dist[%d][%d]=%d\n", i+1, b, *((dist+(i+1)*graph->V) + b));
	/* printf ("TEST %p %p\n", Q->nodes[0], graph->adjlists[Q->nodes[0]->v_id].nd); */
	printf ("dijkstra cluster with w:%d\n", w->v_id);
	/* for (int a = 0; a < (int)graph->V; a++) { */
	/* 	printf ("%p %p\n", graph->adjlists[a].nd, Q->nodes[graph->adjlists[a].nd->index]); */
	/* } */
	#endif

	while (Q->heap_size != 0) {
		pp_heap (Q);
		struct node *u = extract_min (Q);
		memcpy (&S[idx], u, sizeof (struct node));
		for (struct AdjListNode *s = graph->adjlists[u->v_id].head;
			 s != NULL; s = s->next) {
			struct node *v = graph->adjlists[s->v_id].nd;
			#ifdef DEBUG
			printf("%d < dist[%d][%d]:%d\n", (u->sp_est + s->weight), i+1, s->v_id, *((dist+(i+1)*graph->V) + s->v_id));
			#endif
			// relax the edge (u,v) only if u.d + l(u,v) < d(A_{i+1}, v)
			if ((v != NULL) &&
				(u->sp_est + s->weight) < *((dist+(i+1)*graph->V) + v->v_id)) {
				// above is equivalent to dist[i+1][v->v_id]
				int sp_est = u->sp_est + s->weight;
				#ifdef DEBUG
				/* for (int a = 0; a < (int)graph->V; a++) */
				/* 	printf ("hp: %d\n", graph->adjlists[a].nd->v_id); */
				#endif
				decrease_key (Q, v, u, sp_est);
			}
		}
		graph->adjlists[u->v_id].nd = NULL;
		idx += 1;
	}
	printf ("dijkstra end\n");
	pp_nodes (S, graph->V);

	return S;
}

// the cluster C(w) of an i-center w ∈ A_i − A_i+1 contains all vertices whose distance to w is smaller than their distance to all (i + 1)-centers, dvs. C(w) = {v in V | d(w, v) < d(A_i+1, v)}
// Note that for every w ∈ A_k−1 we have C(w) = V, as δ(A_k, v) = ∞, for every v ∈ V
// TODO: Hvorfor ( C(w) = V, hvis w \in A_k-1, fordi A_k = infinity? )
// TODO: Er antagelsen om max nummer af knuder i A_i korrekt?
// TODO: Hvad nu hvis i+1 mængden er tom? Skal man så bare køre dijsktra ud fra den knude man er på
struct node **construct_clusters (struct Graph *graph, struct node **A,
								  struct heap_t *heap, int seqsizes[],
								  int *dist, int i, int k)
{
	struct cluster_data *C = malloc (sizeof (struct cluster_data));
	// max no of clusters is the number of vertices in A_i
	struct node **clusters = malloc (seqsizes[i] * sizeof (struct node*));
	C->clusters = clusters;
	int no_clusters = 0;
	bool to_cluster = true;

	// For every w in A_i - A_i+1
	for (int w = 0; w < seqsizes[i]; w++) {
		// for every w ∈ A_k−1 we have C(w) = V
		if (i == (k-1)) {
			clusters[w] = malloc (graph->V * sizeof (struct node));
			memcpy (&clusters[w], &A[0], sizeof (struct node*));
		}
		else {
			clusters[no_clusters] = malloc (graph->V * sizeof(struct node));
			// w in A_i - A_i+1
			for (int j = 0; j < seqsizes[i+1]; j++) {
				if (A[i][w].v_id == A[i+1][j].v_id) {
					to_cluster = false;
				}
			}
			// if indeed w in A_i - A_i+1, go on with dijkstra
			if (to_cluster) {
				struct heap_t *new_heap = copy_heap_struct (heap);
				struct Graph *write_graph = copy_graph_struct (graph, new_heap);
				// create cluster C(w)
				struct node *tmp = dijkstra_cluster_tz (write_graph, new_heap,
														&A[i][w], dist, i);
				memcpy (&clusters[no_clusters], &tmp, sizeof (struct node*));
				no_clusters += 1;
			}
			to_cluster = true;
		}
	}
	#ifdef DEBUG
	if (to_cluster) {
		printf ("debug in clusters for i:%d\n", i);
		for (int j = 0; (j < seqsizes[i] && j < no_clusters) ||
				 (i == (k-1) && j < seqsizes[i]); j++)
			pp_nodes (clusters[j], graph->V);
		printf ("done debug in clusters\n");
	}
	#endif

	return clusters;
}

// Find p_i(v) til v i A_i samt afstanden d(v,A_i) fra v til p_i(v)
// TODO: Christian - vil det være rigtigt?
// TODO: Christian går (*) ud på at det skal være ==, eller <=, i beviset står noget med at hvis (*) fejler så er p_i (v) = p_i+1, men hos mig er det omvendt? Hvornår fejler (*) så?
// TODO: Vil der er i A_i, v , i=0 overhovedet være pivot elementer? Da A_0 = V, og (s, v) = 0, gælder for alle
// TODO: Kan pivot elementerne også være dem der har vægt 0?
struct node* find_pivot (struct node* ai, struct node* aiplusone, unsigned int len)
{
	struct node* piv = malloc (sizeof (struct node));
	// indicates piv is empty / not found yet
	int piv_id = -1;
	for (unsigned int i = 0; i < len; i++) {
		if ((piv_id == -1 || ai[i].sp_est <= piv->sp_est)
			&& ai[i].sp_est > 0) {
			memcpy (piv, &ai[i], sizeof(ai[i]));
			piv_id = ai[i].v_id;
			// asterisk statement, hvis (A_i, v) == (A_i+1,v), copy!
			if (aiplusone != NULL && aiplusone[i].sp_est == piv->sp_est) {
				memcpy (piv, &aiplusone[i], sizeof (aiplusone[i]));
				piv_id = aiplusone[i].v_id;
			}
		}
	}

	#ifdef DEBUG
	if (piv->pi != NULL)
		printf ("pivot found: %d %d %d\n", piv_id, piv->sp_est, piv->pi->v_id);
	#endif
	return piv;
}

/* When a pointer is passed to a function, it is passed by value. If we want to modify the */
/* original pointer and not the copy of the pointer, we need to pass it as a pointer to a */
/* pointer. */

// TODO: Change to adjlist eller spørg Christian om det er ok.
// TODO: Skal knude s beholdes i heap (til clustering)? Derfor heap size +- 1. Også til clustering?
// TODO: Spørgs christian om jeg skal bruge dist (integers) i stedet for res (knuder)
// Should have at most time complexity O(kn^{1+1/k})
void prepro (struct Graph *graph, int k)
{
	// array of k pointers
	struct node *A[k];
	struct heap_t *heap = initialise_single_source_tz (graph);
	// k+1, for the kth set where d(A_k, v) = infinity for all v in V
	struct node *res[(k+1) * graph->V];
	int dist[k+1][graph->V];
	// For each v the pivot node to each i, thus v*k
	struct node pivot_nodes[k * graph->V];
	struct node tmp_arr[graph->V];
	int seqsizes[k];

	struct node **C = malloc (k * sizeof (struct node*));
	assert (C != NULL);

	for (unsigned int l = 0; l < ((k+1) * graph->V); l++) {
		res[l] = malloc (graph->V * sizeof(struct node));
	}

	// A_k <- Ø
	A[k] = NULL;
	A[0] = malloc (graph->V * sizeof(struct node));

	// A_0 <- V. Running time O(n)
	for (unsigned int i = 0; i < graph->V; i++) {
		memcpy (&A[0][i], heap->nodes[i], sizeof(struct node));
	}

	seqsizes[0] = heap->heap_size;
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

	// d(A_k, v) = infinity
	for (unsigned int v = 0; v < graph->V; v++) {
		dist[k][v] = (int) INFINITY;
	}

	// k iterations
	for (int i = k-1; i >= 0; i--) {

		if (seqsizes[i] == 0) {
			continue;
		}
		// copy of heap to work with for current i
		struct heap_t *write_heap = copy_heap_struct (heap);
		// copy of graph to work with for current i
		struct Graph *write_graph = copy_graph_struct (graph, write_heap);
		// adding source vertex s to G, weight 0 to all other vertices in A_i
		add_s_node_to_graph (write_graph, A[i], seqsizes[i]);
		// adding s to heap as well to allow computing sp
		struct node *tmp = add_heap_node (write_graph->V, 0);
		write_heap->nodes[write_heap->heap_size] = tmp;
		write_graph->adjlists[write_graph->V].nd = tmp;
		write_heap->heap_size += 1;
		write_graph->V += 1;
		int u = write_heap->nodes[write_heap->heap_size-1]->index;
		fix_positions (write_heap, u);

		// compute d(A_i, v), running dijkstra once for each i
		struct node *sps = dijkstra_alg_tz (write_graph, write_heap);
		memcpy (&res[i], &sps, sizeof (struct node*));
		#ifdef DEBUG
		printf ("prepro dijkstra res for i=%d:\n", i);
		pp_nodes (res[i], graph->V+1);
		#endif
		// for every v in V
		for (unsigned int v = 0; v < write_graph->V; v++) {
			if (sps[v].v_id != (int) (write_graph->V-1)) {
				/* printf ("dist[%d][%d]=%d\n", i, sps[v].v_id, sps[v].sp_est); */
				dist[i][sps[v].v_id] = sps[v].sp_est;
			}
		}
		if (i != k-1) {
			struct node *tmp = find_pivot (res[i], &pivot_nodes[i+1], graph->V+1);
			memcpy (&pivot_nodes[i], &tmp, sizeof (struct node));
		} else {
			struct node *tmp = find_pivot (res[i], NULL, graph->V+1);
			memcpy (&pivot_nodes[i], &tmp, sizeof (struct node));
		}

		C[i] = (construct_clusters (graph, A, heap, seqsizes, &dist[0][0], i, k))[0];

		free (write_heap);
		free (write_graph);
	}

	for (int p = 0; p < k; p++) {
		for (int a = 0; a < (int) graph->V; a++)
			printf ("%d\n", C[p][a].v_id);
		printf ("\n");
	}
		

	// TODO: for every v in V, generate bunches B(v) from all C(w)

	printf ("***** prepro done ******\n");
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
	argv = argv;
	if (argc < 3 || (argc % 2 != 1)) {
		printf ("No input and output arguments or input/output does not match!\n");
		return EXIT_FAILURE;
	}

	/* for (int i = 1; i < argc; i++) { */
	/* 	const char *fname_read = argv[1]; */
	/* 	const char *fname_write = argv[2]; */
	/* 	run (fname_read, fname_write); */
	/* }; */

	test_prepro ();
	/* hardcoded_tests (); */

	return EXIT_SUCCESS;
}
