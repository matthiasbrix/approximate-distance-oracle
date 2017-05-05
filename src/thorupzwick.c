#include "thorupzwick.h"
#include <time.h>
#include <assert.h>

#define DEBUG

// TODO: en fil til preprocessing
// TODO: en fil til query og main

struct bunch *hash_bunch = NULL;

void free_heap (struct heap *heap)
{
	for (int i = 0; i < heap->heap_size; i++)
		free (heap->nodes[i]);
	free (heap);
}

void free_graph (struct graph *graph)
{
	free (graph->adjlists->head);
	graph->adjlists->head = NULL;
	for (int i = 0; i < (int) graph->V; i++)
		free (&graph->adjlists[i]);
	graph->adjlists = NULL;
	free (graph);
	graph = NULL;
}

void pp_aseqs (struct aseq **A, int k)
{
	printf("Pretty print all A_i sequences\n");
	for (int i = 0; i < k; i++) {
		if (A[i]->seqsize > 0) {
			for (int j = 0; j < A[i]->seqsize; j++)
				printf ("A_{%d,%d} v_id:%d spest:%d\n", i, j,
						A[i]->nodes[j].v_id, A[i]->nodes[j].sp_est);
		}
	}

	return;
}

void pp_clusters (struct clusterlist **C, int i)
{
	printf("Pretty print all clusters\n");
	for (int a = 0; a < C[i]->no_clusters; a++)
		for (int b = 0; b < C[i]->clusters[a].no_nodes; b++)
			printf ("cluster nodes for C(w:%d)=%d\n", C[i]->clusters[a].w->v_id, C[i]->clusters[a].cluster[b].v_id);

	return;
}

void pp_bunches (struct bunch *bunches)
{
	printf("Pretty print all bunches\n");
	struct bunch *s, *tmp;
	HASH_ITER(hh, bunches, s, tmp) {
		for (int i = 0; i < s->no_nodes; i++) {
			printf ("B(v:%d)=%d\n", s->v->v_id, s->nodes[i].v_id);
		}
	}
	return;
}

void add_s_node_to_graph (struct graph *graph, struct aseq *ai)
{
	// The added node s will always have the vertex id of the no. of vertices
	for (int i = 0; i < ai->seqsize; i++) {
		add_edges (graph, graph->V, ai->nodes[i].v_id, 0);
	}
	return;
}

int dist (struct node *u, struct node *v, struct bunch *bunches)
{
	struct node	*w = malloc (sizeof (struct node));
	struct bunch *bunch;
	int i;

	memcpy (&w, &u, sizeof (u));
	i = 0;

	// w != B(v)
	for (bunch = bunches; bunch != NULL; bunch=bunch->hh.next) {
		printf("B(%d)\n", bunch->v->v_id);
		// i <- i + 1
		i += i + 1;
		// (u, v) <- (v, u)
		struct node *tmp = malloc (sizeof (struct node));
		memcpy (&tmp, &u, sizeof (struct node));
		memcpy (&u, &v, sizeof (v));
		memcpy (&v, &tmp, sizeof (tmp));
		// w <- p_i (u)
		memcpy (&w, &bunch->)
	}

	return u->sp_est + v->sp_est;
}

// TODO: Change all no_xxx to num_xxxx

// We find p_i(v) to v in A_i as well as the distance d(v, A_i) from v to p_i(v). Note, p_i(v) in A_i and it is nearest to v which means d(p_i(v), v) = d(A_i, v). Note that as A_0 = V, d(A_0, v) = 0 and p_0(v) = v, which applies for all v in V
// Also, if A_i == NULL, we have no pivot nodes p_i(v) for i
// TODO: Hold øje med hvordan den vælger hvis flere vertices i A_i har kortest afstand til v, formentlig den første knude er pivot fordi dijsktra kræver skarpt mindre forholdet
// TODO: I beviset står noget med at hvis (*) fejler så er p_i (v) = p_i+1, men hos mig er det omvendt? Hvornår fejler (*) så?
// TODO: Use nodes (sps) instead of dist if sps is indexed by v_id
struct node* find_pivot (struct aseq *ai, struct node *aiplusone_pivot_arr,
						 struct node *nodes, unsigned int n, int *dist)
{
	struct node* pivot_arr = malloc (n * sizeof (struct node));
	// starting from one, skipping node s in nodes, assuming s is first node
	for (unsigned int i = 1; i < n+1; i++) {
		bool found = false;
		for (int j = 0; j < ai->seqsize; j++) {
			// if A_i == v, p_i(v) = v, as d(A_i, v) = 0
			if (ai->nodes[j].v_id == nodes[i].v_id) {
				memcpy (&pivot_arr[nodes[i].v_id], &ai->nodes[j], sizeof(ai->nodes[j]));
				pivot_arr[nodes[i].v_id].sp_est = 0;
				found = 1;
			}
			// if v has v.pi = A_i <=> p_i(v) = A_i
			else if (nodes[i].pi->v_id == ai->nodes[j].v_id) {
				memcpy (&pivot_arr[nodes[i].v_id], &ai->nodes[j], sizeof(ai->nodes[j]));
				pivot_arr[nodes[i].v_id].sp_est = dist[nodes[i].v_id];
				found = 1;
			}
		}
		// if v has v.pi != A_i, we traverse back from v to A_i as v is in the branch of the shortest paths tree that starts with the edge (s, w), where w ∈ A_i, then δ(A_i, v) = δ(w, v) and we can set p_i (v) ← w
		if (!found) {
			struct node *piv = NULL;
			struct node *v = nodes[i].pi;
			while (v) {
				piv = v;
				// avoid reaching the s node
				if (v->pi->v_id == nodes[0].v_id)
					break;
				v = v->pi;
			}
			// let piv be w, thus we set p_i(v) <- w
			memcpy (&pivot_arr[nodes[i].v_id], piv, sizeof(&piv));
			pivot_arr[nodes[i].v_id].sp_est = dist[nodes[i].v_id];
		}
		// (*) property, if d(A_i, v) == d(A_i+1,v), copy! Means we get p_i(v) = p_i+1(v) ∈ B(v),
		// otherwise, δ(p_i(v), v) = δ(A_i, v) < δ(A_i+1, v). Thus distance will be same, but the node would change (to A_i+1) if (*) fails
		if (aiplusone_pivot_arr != 0 && pivot_arr[nodes[i].v_id].sp_est == aiplusone_pivot_arr[nodes[i].v_id].sp_est) {
			memcpy (&pivot_arr[nodes[i].v_id], &aiplusone_pivot_arr[nodes[i].v_id], sizeof (aiplusone_pivot_arr[nodes[i].v_id]));
		}
	}
	#ifdef DEBUG
	for (unsigned int i = 1; i < n+1; i++) {
		printf ("pivot for v:%d is:%d, dist:%d, should be %d\n", nodes[i].v_id, pivot_arr[nodes[i].v_id].v_id, pivot_arr[nodes[i].v_id].sp_est, dist[nodes[i].v_id]);
	}
	#endif

	return pivot_arr;
}

// TODO: Skal de tilæføjede knuder have distance sp_est som er beregnet eller infinity?
// TODO: Hvad nu hvis en A_{i+1} er tom? Så er jo pivot element undefined, derfor inkluderes knuden ikke?
// TODO: Hvad med knude.pi? Skal den også opdateres når jeg tilføjer en knude som en "relaxering"?
// cluster C(w) is composed of all the vertices that are closer to w than any (i+1)-center
// Note that δ(A_{i+1}, v) was computed in the previous iteration so the test takes constant time.
// Note that if v not in C(w), then we never assign a finite distance, as by definition, v ∈ C(w) if and only if δ(w, v) < δ(A_i+1, v)
struct cluster *dijkstra_cluster_tz (struct graph *graph, struct node *w,
									 struct node *pivot_nodes, int k, int i)
{
	int idx = 0;
	int no_nodes = 0;
	int *in_heap = calloc (graph->V, sizeof (int));
	assert (in_heap != NULL);
	int *relaxed = calloc (graph->V, sizeof (int));
	struct heap *Q = malloc (sizeof (struct heap));
	assert (Q != NULL);
	Q->nodes = malloc (sizeof(struct node*));
	assert (Q->nodes != NULL);
	struct node *S = malloc (graph->V * sizeof (struct node));
	assert (S != NULL);
	struct cluster *cluster = malloc (sizeof (struct cluster));
	assert (cluster != NULL);

	Q->heap_size = 0;
	// w.d = 0
	min_heap_insert (Q, w->v_id, 0, graph);
	no_nodes++;

	#ifdef DEBUG
	printf ("dijkstra cluster with w:%d\n", w->v_id);
	pp_graph (graph);
	#endif

	while (Q->heap_size != 0) {
		struct node *u = extract_min (Q);
		// &S[idx] gets assigned value from u, copying to its address
		memcpy (&S[idx], u, sizeof (struct node));
		// avoiding node u can be relaxed in next iterations,
		// as u is no longer on heap
		relaxed[u->v_id] = 1;
		in_heap[u->v_id] = 0;
		for (struct adjlistnode *s = graph->adjlists[S[idx].v_id].head;
			 s != NULL; s = s->next) {
			struct node *v = graph->adjlists[s->v_id].nd;
			#ifdef DEBUG
			if (pivot_nodes != 0) {
				printf ("%d < dist[i+1][%d]: %d\n", (u->sp_est + s->weight), s->v_id,
						pivot_nodes[s->v_id].sp_est);
			}
			#endif
			// relax the edge (u,v) only if u.d + l(u,v) < d(A_{i+1}, v),
			// where d(A_{i+1}, v) = d(p_{i+1}(v), v)
			// that is, if w is strictly closer to v than all the vertices of A_i+1
			// OR: If i==k-1 and vertex hasn't been relaxed and in heap!
			if (((pivot_nodes != 0) && (v != NULL) &&
					 (u->sp_est + s->weight) < pivot_nodes[s->v_id].sp_est
					 && relaxed[s->v_id] == 0) ||
				(i == k-1 && v != NULL && relaxed[s->v_id] == 0)) {
				int sp_est = u->sp_est + s->weight;
				if (in_heap[s->v_id] == 1) {
					if ((v->sp_est > sp_est)) {
						decrease_key (Q, v, u, sp_est);
					}
				} else {
					// TODO: Indsæt for grafens knud w_id i
					min_heap_insert (Q, s->v_id,
									 sp_est, graph);
					in_heap[s->v_id] = 1;
					no_nodes += 1;
				}
			}
		}
		idx += 1;
	}

	cluster->w = w;
	cluster->cluster = S;
	cluster->no_nodes = no_nodes;
	free (Q);

	return cluster;
}

// the cluster C(w) of an i-center w ∈ A_i − A_i+1 contains all vertices whose distance to w is smaller than their distance to all (i + 1)-centers, dvs. C(w) = {v in V | d(w, v) < d(A_i+1, v)}
// Note that for every w ∈ A_k−1 we have C(w) = V, as δ(A_k, v) = ∞, for every v ∈ V
// TODO: Hvorfor ( C(w) = V, hvis w \in A_k-1, fordi A_k = infinity? ), dvs. afstanden beregnes fra w til alle knuder?
// TODO: Er antagelsen om max nummer af knuder i A_i korrekt? (seqsizes[i])
// TODO: Hvad nu hvis i+1 mængden er tom? Skal man så bare køre dijsktra ud fra den knude man er på
struct clusterlist *construct_clusters (struct graph *graph, struct aseq **A,
								  struct node *pivot_nodes, int i, int k)
{
	struct clusterlist *C = malloc (sizeof (struct clusterlist));
	struct cluster *clusters = malloc (A[i]->seqsize * sizeof (struct node));
	bool to_cluster = true;
	int no_clusters = 0;

	// For every w in A_i - A_i+1
	for (int w = 0; w < A[i]->seqsize; w++) {
		// for every w ∈ A_k−1 we have C(w) = V
		if (i == (k-1)) {
			// create cluster C(w) = V, computing the distances from w to all other v in V
			struct cluster *cw = dijkstra_cluster_tz (graph, &A[i]->nodes[w],
													  NULL, k, i);
			memcpy (&clusters[no_clusters], cw, sizeof (struct cluster));
			no_clusters += 1;
		}
		else {
			// check if w in A_i - A_i+1
			for (int j = 0; j < A[i+1]->seqsize; j++) {
				if (A[i]->nodes[w].v_id == A[i+1]->nodes[j].v_id) {
					to_cluster = false;
				}
			}
			// if indeed w in A_i - A_i+1, go on with dijkstra to create a cluster
			if (to_cluster) {
				// create cluster C(w)
				struct cluster *cw = dijkstra_cluster_tz (graph, &A[i]->nodes[w],
														  pivot_nodes, k, i);
				memcpy (&clusters[no_clusters], cw, sizeof (struct cluster));
				no_clusters += 1;
			}
			to_cluster = true;
		}
	}

	C->no_clusters = no_clusters;
	C->clusters = clusters;

	#ifdef DEBUG
	if (to_cluster) {
		printf ("debug in clusters for i:%d\n", i);
		for (int j = 0; (j < A[i]->seqsize && j < C->no_clusters) ||
				 (i == (k-1) && j < A[i]->seqsize); j++)
			pp_nodes (clusters[j].cluster, clusters[j].no_nodes);
		printf ("done debug in clusters\n");
	}
	#endif

	return C;
}

void create_aseqs (struct aseq **A, int k, struct graph *graph, struct node *nodes)
{
	int seqsizes[k];
	struct node tmp_arr[graph->V];
	// A_k <- Ø
	A[k] = NULL;
	// A_0 <- V. Running time O(n)
	A[0] = malloc (sizeof (struct aseq));
	A[0]->nodes = malloc (graph->V * sizeof(struct node));
	for (unsigned int i = 0; i < graph->V; i++) {
		memcpy (&A[0]->nodes[i], &nodes[i], sizeof(struct node));
	}

	A[0]->seqsize = graph->V;
	seqsizes[0] = graph->V;
	// Set the seed
	srand((unsigned)time(NULL));

	for (int i = 1; i <= k-1; i++) {
		seqsizes[i] = 0;
		for (int j = 0; j < seqsizes[i-1]; j++) {
			// Generates 0.0 - 1.0
			double rnd = (double)rand()/RAND_MAX;
			// Check random number is <= n^{-1/k}
			if (rnd <= pow (graph->V, -1.0/(double)k)) {
				memcpy (&tmp_arr[seqsizes[i]], &A[i-1]->nodes[j],
						sizeof (struct node));
				seqsizes[i] += 1;
			}
		}
		A[i] = malloc (sizeof (struct aseq));
		A[i]->nodes = malloc (seqsizes[i] * sizeof(struct node));
		for (int l = 0; l < seqsizes[i]; l++) {
			memcpy (&A[i]->nodes[l], &tmp_arr[l], sizeof (struct node));
		}
		A[i]->seqsize = seqsizes[i];
	}
}

// TODO: Skal jeg droppe dist og bare gemme efter S[u->v_id]?
// Should have at most time complexity O(kn^{1+1/k})
void prepro (struct graph *graph, int k)
{
	struct node nodes[graph->V];
	// k+1, for the kth set where d(A_k, v) = infinity for all v in V
	int dist[k+1][graph->V];
	// For each v the pivot node to each i, thus v*k
	struct node *pivot_nodes[k];
	struct node *pivot_arr;
	struct aseq **A = malloc (k * sizeof (struct aeseqs*));
	struct heap *heap;
	struct clusterlist *C[k];
	// TODO: return bunches?
	struct bunch *bunches = NULL;

	int val = (int) INFINITY;
	// saving all v in V in array nodes
	for (unsigned int i = 0; i < graph->V; i++) {
		memcpy (&nodes[i], add_node (i, val, i), sizeof (struct node));
	}

	// Creating all A_i sequences
	create_aseqs (A, k, graph, nodes);

	#ifdef DEBUG
	pp_aseqs (A, k);
	#endif
	// initialising the heap
	heap = initialise_single_source_tz (graph);

	// d(A_k, v) = infinity
	for (unsigned int v = 0; v < graph->V; v++) {
		dist[k][v] = (int) INFINITY;
	}

	// k iterations
	for (int i = k-1; i >= 0; i--) {

		if (A[i]->seqsize == 0) {
			// Empty set, thus d(A_i, v) = infinity
			for (unsigned int v = 0; v < graph->V; v++) {
				dist[i][v] = (int) INFINITY;
			}
			memset(&pivot_nodes[i], 0, sizeof(pivot_nodes[i]));
			continue;
		}
		// copy of heap to work with for current i
		struct heap *write_heap = copy_heap_struct (heap);
		// copy of graph to work with for current i
		struct graph *write_graph = copy_graph_struct (graph, write_heap);
		// adding source vertex s to G, weight 0 to all other vertices in A_i
		add_s_node_to_graph (write_graph, A[i]);
		// adding s to heap as well to allow computing sp
		min_heap_insert (write_heap, write_graph->V, 0, write_graph);
		write_graph->V += 1;

		// compute d(A_i, v), running dijkstra once for each i
		struct node *sps = dijkstra_alg_tz (write_graph, write_heap);
		#ifdef DEBUG
		pp_graph (write_graph);
		printf ("prepro dijkstra result for i=%d:\n", i);
		pp_nodes (sps, graph->V+1);
		#endif
		// for every v in V
		for (unsigned int v = 0; v < write_graph->V; v++) {
			// Excluding the node s
			if (sps[v].v_id != (int) (write_graph->V-1)) {
				dist[i][sps[v].v_id] = sps[v].sp_est;
			}
		}
		// No use of write heap and graph anymore
		free_heap (write_heap);
		free (write_graph);
		// Finding the pivot elements, note p_k(v) undefined as A_k = Ø (as in the else case)
		if (i != k-1) {
			pivot_arr = find_pivot (A[i], pivot_nodes[i+1], sps, graph->V, dist[i]);
		} else {
			pivot_arr = find_pivot (A[i], NULL, sps, graph->V, dist[i]);
		}
		memcpy (&pivot_nodes[i], &pivot_arr, sizeof (struct node*));
		C[i] = construct_clusters (graph, A, pivot_nodes[i+1], i, k);

		#ifdef DEBUG
		pp_clusters (C, i);
		#endif
	}
	// TODO: Use tmp array instead and then copy in the end (like for aseqs), used for b->nodes
	/* for alle v \in V skal jeg lave en
	   hash table, som så indeholder alle knuder w som v er i cluster med */
	// Constructing bunches for all v, containing all w of all C(w) v belongs to
	for (unsigned int j = 0; j < graph->V; j++) {
		struct bunch *b = malloc (sizeof (struct bunch));
		// mandatory to set b to 0 (according to uthash)
		memset (b, 0, sizeof (struct bunch));
		b->v = malloc (sizeof (struct node));
		// copy current v in V to tmp bunch
		memcpy (b->v, &nodes[j], sizeof(struct node));
		b->piv = malloc (k * sizeof (struct node));
		b->nodes = malloc (graph->V * sizeof (struct node));
		for (int i = 0; i < k; i ++) {
			if (A[i]->seqsize > 0) {
				memcpy (&b->piv[i], &pivot_nodes[i][nodes[j].v_id], sizeof (pivot_nodes[i][nodes[j].v_id]));
				for (int c = 0; c < C[i]->no_clusters; c++) {
					struct cluster *cluster = &C[i]->clusters[c];
					for (int v = 0; v < cluster->no_nodes; v++) {
						if (cluster->cluster[v].v_id == b->v->v_id) {
							memcpy (&b->nodes[b->no_nodes], cluster->w, sizeof (struct node));
							b->no_nodes += 1;
						}
					}
				}
			}
		}
		// hashhandle, struct to work with, v of bunch is key, b is temp struct
		HASH_ADD (hh, bunches, v, sizeof(b->v), b);
	}

	#ifdef DEBUG
	pp_bunches (bunches);
	#endif

	// Hash table for en knude v skal indeholde netop de knuder w, der tilhører B(v), og disse knuder w er netop dem, hvor v tilhører C(w).
	// zero level: B(v) for v in V.
	/*
	  Finally, the algorithm constructs (2-level) hash tables for the bunches B(v), for
	  v ∈ V, and outputs the witnesses p_i(v), the distances δ(p_i(v), v) = δ(A_i, v), and
	  the hash tables of B(v), for every 1 ≤ i ≤ k and v ∈ V. In addition to that, the
	  preprocessing algorithm also outputs, for every w ∈ V, the shortest paths tree T (w)
	  that spans the cluster C(w).
	 */
	// http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
	// TODO: Læs om hash funktionen af uthash
	// TODO: for every v in V, generate bunches B(v) from all C(w)
	// Checks whether w in B(v) and retrieves d(w, v)
	// TODO: Skal det være en 2-level table med den foreslående hash funktion?
	// TODO: Husk også at gemme afstanden i Bunches
	// by definition, w ∈ B(v) if and only if v ∈ C(w).

	printf ("***** prepro done ******\n");
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
	pp_graph (graph_1);
	struct node *S_1 = dijkstra_alg (graph_1, 0);
	pp_nodes (S_1, 5);

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
	pp_graph (graph);
	struct node *S_2 = dijkstra_alg (graph, 0);
	pp_nodes (S_2, 9);

	printf ("------------------------\n");
	// From CLRS p. 659 (though it is directed)
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
	struct node *S_3 = dijkstra_alg (graph_3, 0);
	pp_nodes (S_3, 5);
}

void test_prepro ()
{
	struct graph* graph_3 = init_graph (5);
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
	struct graph *graph = init_graph (V);
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
