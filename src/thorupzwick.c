#include "thorupzwick.h"
#include <time.h>
#include <assert.h>
#include <unistd.h>

#define DEBUG

// TODO: en fil til preprocessing
// TODO: en fil til query og main
// TODO: tid, plads, og hvor tæt kommer jeg på Dijkstras (SSP)
// tid, plads skal være hardware.
// Stemmer ekspirementerne overens med teori?
// du skal dog nævne i din rapport, hvilken hash table du bruger
// Skriv i rapport om at man kan udvide med floats
// Hash table for en knude v skal indeholde netop de knuder w, der tilhører B(v), og disse knuder w er netop dem, hvor v tilhører C(w).
	// zero level: B(v) for v in V.
/*
  Finally, the algorithm constructs (2-level) hash tables for the bunches B(v), for
  v ∈ V, and outputs the witnesses p_i(v), the distances δ(p_i(v), v) = δ(A_i, v), and
  the hash tables of B(v), for every 1 ≤ i ≤ k and v ∈ V. In addition to that, the
  preprocessing algorithm also outputs, for every w ∈ V, the shortest paths tree T (w)
  that spans the cluster C(w).
 */
// HVorfor vil thorup-zwick ikke være lige så "rigtig" som normal Dijkstra?
// http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
// TODO: Læs om hash funktionen af uthash
// TODO: for every v in V, generate bunches B(v) from all C(w)
// Checks whether w in B(v) and retrieves d(w, v)
// TODO: Skal det være en 2-level table med den foreslående hash funktion?
// TODO: Husk også at gemme afstanden i Bunches
// by definition, w ∈ B(v) if and only if v ∈ C(w).
// TODO: Have all pointer be with the variable name
// Bunche gen. er som DFS? (spørg ellers Christian)
// Skriv til uthash om hash funktioner

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
	for (int a = 0; a < C[i]->num_clusters; a++)
		for (int b = 0; b < C[i]->clusters[a].num_nodes; b++)
			printf ("cluster nodes for C(w:%d)=v:%d, SP:%d\n", C[i]->clusters[a].w->v_id, C[i]->clusters[a].nodes[b].v_id, C[i]->clusters[a].nodes[b].sp_est);

	return;
}

void pp_bunches (struct bunchlist *bunchlist)
{
	printf("Pretty print all bunches\n");
	for (int i = 0; i < bunchlist->num_bunches; i++) {
		struct node *s;
		// iterate over hash elements of B(v)
		for(s = bunchlist->bunches[i].nodes; s != NULL; s = s->hh.next) {
			printf("B(v:%d)=w:%d, SP:%d\n", bunchlist->bunches[i].v->v_id, s->v_id, s->sp_est);
		}
		/* HASH_ITER (hh, bunchlist->bunches[i].nodes, s, tmp) { */
		/* 	printf ("B(v:%d)=w:%d, SP:%d\n", bunchlist->bunches[i].v->v_id, s->v_id, s->sp_est); */
		/* } */
	}

	return;
}

void pp_pivots (struct node *pivot_arr, struct node *nodes, unsigned int n)
{
	for (unsigned int i = 0; i < n-1; i++) {
		printf ("pivot for v:%d is:%d, dist:%d, dist should be %d\n", nodes[i].v_id, pivot_arr[nodes[i].v_id].v_id, pivot_arr[nodes[i].v_id].sp_est, pivot_arr[nodes[i].v_id].sp_est);
	}
}

/*************************************************************************
 * Begin of the actual functions for thorup-zwick
 *************************************************************************/

void add_s_node_to_graph (struct graph *graph, struct aseq *ai)
{
	// The added node s will always have the vertex id of the current
	// no. of vertices in the graph
	for (int i = 0; i < ai->seqsize; i++) {
		add_edges (graph, graph->V, ai->nodes[i].v_id, 0);
	}
	return;
}

// Not looping through all v in V and A sets, otherwise running time could be O(n^2)
// If v.pi = s, it is a pivot node
// If a node has been visited by a traversing one, save the visitor, such that in a later iteration we check if the respective node has been visited. If so, we we simply retrieve the visitor node's pivot node
// (*) property, if d(A_i, v) == d(A_i+1,v), copy! Means we get p_i(v) = p_i+1(v) ∈ B(v),
// otherwise, δ(p_i(v), v) = δ(A_i, v) < δ(A_i+1, v). Thus distance will be same, but the node would change (to A_i+1) if (*) fails
// we traverse back from v to A_i as v is in the branch of the shortest paths tree that starts with the edge (s, w), where w ∈ A_i, then δ(A_i, v) = δ(w, v) and we can set p_i (v) ← w
struct node* find_pivot (struct node *aiplusone_pivot_arr,
						 struct node *nodes, unsigned int n)
{
	int *visited_nodes = malloc ((n-1) * sizeof(int));
	memset (visited_nodes, -1, (n-1) * sizeof(visited_nodes));
	// no need to allocate for s node, therefore n-1
	struct node* pivot_arr = malloc ((n-1) * sizeof (struct node));

	// skipping node s in nodes, assuming s is the last node
	for (unsigned int i = 0; i < n-1; i++) {

		struct node *piv = NULL;
		struct node *v = nodes[i].pi;
		if (visited_nodes[nodes[i].v_id] != -1) {
			int pivid = pivot_arr[nodes[i].v_id].v_id;
			memcpy (&pivot_arr[nodes[i].v_id], &pivot_arr[pivid], sizeof(pivot_arr[pivid]));
			pivot_arr[nodes[i].v_id].sp_est = nodes[nodes[i].v_id].sp_est;
		}
		else if (v->v_id == nodes[n-1].v_id) {
			memcpy (&pivot_arr[nodes[i].v_id], &nodes[i], sizeof(nodes[i]));
			pivot_arr[nodes[i].v_id].sp_est = 0;
		}
		else {
			while (v) {
				piv = v;
				// avoid reaching the s node
				if (v->pi->v_id == nodes[n-1].v_id)
					break;
				visited_nodes[v->v_id] = nodes[i].v_id;
				v = v->pi;
			}
			// let piv be w, thus we set p_i(v) <- w
			memcpy (&pivot_arr[nodes[i].v_id], piv, sizeof(&piv));
			pivot_arr[nodes[i].v_id].sp_est = nodes[nodes[i].v_id].sp_est;
		}
		// (*) property
		if (aiplusone_pivot_arr != 0 && pivot_arr[nodes[i].v_id].sp_est == aiplusone_pivot_arr[nodes[i].v_id].sp_est) {
			memcpy (&pivot_arr[nodes[i].v_id], &aiplusone_pivot_arr[nodes[i].v_id], sizeof (aiplusone_pivot_arr[nodes[i].v_id]));
		}
	}

	return pivot_arr;
}

// cluster C(w) is composed of all the vertices that are closer to w than any (i+1)-center
// Note that δ(A_{i+1}, v) was computed in the previous iteration so the test takes constant time.
// Note that if v not in C(w), then we never assign a finite distance, as by definition, v ∈ C(w) if and only if δ(w, v) < δ(A_i+1, v)
// If A_{i+1} is empty we skip the array as the pivot element is undefined for d(A_i, v) = d(Ø, v) = infinity
struct cluster *dijkstra_cluster_tz (struct graph *graph, struct node *w,
									 struct node *pivot_nodes, int k, int i)
{
	int idx = 0;
	int num_nodes = 0;
	int *in_heap = calloc (graph->V, sizeof (int));
	assert (in_heap != NULL);
	int *relaxed = calloc (graph->V, sizeof (int));
	struct heap *Q = malloc (sizeof (struct heap));
	assert (Q != NULL);
	Q->nodes = malloc (sizeof(struct node*));
	assert (Q->nodes != NULL);
	struct node tmp_S[graph->V];
	struct cluster *cluster = malloc (sizeof (struct cluster));
	assert (cluster != NULL);
	struct node *S;

	Q->heap_size = 0;
	// w.d = 0
	min_heap_insert (Q, w->v_id, 0, graph);
	num_nodes++;

	#ifdef DEBUG
	printf ("dijkstra cluster with w:%d\n", w->v_id);
	pp_graph (graph);
	#endif

	while (Q->heap_size != 0) {
		struct node *u = extract_min (Q);
		// tmp_S[idx] gets assigned value from u, copying to its address
		tmp_S[idx] = *u;
		// avoiding node u can be relaxed in next iterations,
		// as u is no longer on heap
		relaxed[u->v_id] = 1;
		in_heap[u->v_id] = 0;
		for (struct adjlistnode *s = graph->adjlists[tmp_S[idx].v_id].head;
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
			// OR: TODO: Hvorfor? If i==k-1 and vertex hasn't been relaxed and in heap!
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
					min_heap_insert (Q, s->v_id,
									 sp_est, graph);
					graph->adjlists[s->v_id].nd->pi = u;
					in_heap[s->v_id] = 1;
					num_nodes += 1;
				}
			}
		}
		idx += 1;
	}

	S = malloc (num_nodes * sizeof (struct node));
	assert (S != NULL);
	for (int i = 0; i < num_nodes; i++)
		memcpy (&S[i], &tmp_S[i], sizeof (struct node));

	cluster->w = w;
	cluster->nodes = S;
	cluster->num_nodes = num_nodes;
	free (Q);

	return cluster;
}

// TODO: Er dobbel loop ok?
// the cluster C(w) of an i-center w ∈ A_i − A_i+1 contains all vertices whose distance to w is smaller than their distance to all (i + 1)-centers, dvs. C(w) = {v in V | d(w, v) < d(A_i+1, v)}
// Note that for every w ∈ A_k−1 we have C(w) = V, as δ(A_k, v) = ∞, for every v ∈ V
struct clusterlist *construct_clusters (struct graph *graph, struct aseq **A,
										struct node *pivot_nodes, int i, int k)
{
	struct cluster tmp_clusters[A[i]->seqsize];
	bool to_cluster = true;
	int num_clusters = 0;
	struct clusterlist *C = malloc (sizeof (struct clusterlist));
	struct cluster *clusters;

	// For every w in A_i - A_i+1
	for (int w = 0; w < A[i]->seqsize; w++) {
		// for every w ∈ A_k−1 we have C(w) = V
		if (i == (k-1)) {
			// create cluster C(w) = V, computing the distances from w to all other v in V
			struct cluster *cw = dijkstra_cluster_tz (graph, &A[i]->nodes[w],
													  NULL, k, i);
			memcpy (&tmp_clusters[num_clusters], cw, sizeof (struct cluster));
			num_clusters += 1;
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
				memcpy (&tmp_clusters[num_clusters], cw, sizeof (struct cluster));
				num_clusters += 1;
			}
			to_cluster = true;
		}
	}

	clusters = malloc (num_clusters * sizeof (struct node));
	for (int i = 0; i < num_clusters; i++)
		memcpy (&clusters[i], &tmp_clusters[i], sizeof (struct cluster));

	C->clusters = clusters;
	C->num_clusters = num_clusters;

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

// TODO: Should u and v be integers?
// u and v are the two vertices whose distance is to be estimated.
// If w in B(v) it returns d(w, u) + d(w, v). The distance d(w, u) = d(p_i(u), u) is
// read directly from the data structure constructed in prepro. Also, d(w, v) = d(v, w)
// at most k accesses as w in A_{k-1} and A_{k-1} \subseteq for every v in V
int distk (struct node *u, struct node *v, struct bunchlist *bunchlist)
{
	struct node	w;
	int i = 0;
	int dist = 0;

	// w = u = p_0(u)
	w = *u;
	
	// while w not in B(v)
	while (1) {
		struct node *out, tmp;
		printf ("dist: B(%d) w:%d\n", v->v_id, w.v_id);
		// checking if w in B(v), where nodes are vertices w in V
		HASH_FIND_INT(bunchlist->bunches[v->v_id].nodes, &w.v_id, out);
		if (out) {
			// if w in B(v), break loop
			dist = out->sp_est;
			break;
		} else {
			printf ("i=%d\n", i);
			// i <- i + 1
			i += 1;
			// (u, v) <- (v, u)
			tmp = *u;
			u = v;
			v = &tmp;
			// w <- p_i (u)
			w = bunchlist->bunches[u->v_id].piv[i];
			printf ("w:%d=%d %d %d\n", w.v_id, w.sp_est, u->v_id, v->v_id);
		}
	}
	return dist;
}

// Generating bunches from the clusters. Runs in linear time of the cluster size
// such that O(\sum_{w ∈ V} |C(w)|) = O(\sum_{v ∈ V} |B(v)|)
// For each v in V, we construct, containing all w of all C(w) v belongs to
// Note, we get A_{k-1} \subseteq B(v) for every v in V, meaning all nodes from A_{k-1} belong to every bunch
void construct_bunches (struct clusterlist **C, int k, struct aseq **A, struct bunchlist *bunchlist, struct node **pivot_nodes)
{
	for (int i = 0; i < k; i++) {
		if (A[i]->seqsize > 0) {
			for (int c = 0; c < C[i]->num_clusters; c++) {
				struct cluster *cluster = &C[i]->clusters[c];
				for (int v = 0; v < cluster->num_nodes; v++) {
					struct node *s;
					int tmp = cluster->nodes[v].v_id;
					bunchlist->bunches[tmp].piv[i] = pivot_nodes[i][tmp];
					HASH_FIND_INT (bunchlist->bunches[tmp].nodes, &cluster->w->v_id, s);
					if (s == NULL) {
						cluster->w->sp_est = cluster->nodes[v].sp_est;
						s = malloc (sizeof (struct node));
						memcpy (s, cluster->w, sizeof(&cluster->w));
						// adding node id to the hash
						HASH_ADD_INT (bunchlist->bunches[tmp].nodes, v_id, s);
						bunchlist->bunches[tmp].num_nodes += 1;
					}
				}
			}
		}
	}
	return;
}

// TODO: return bunches?
// Should have at most time complexity O(kn^{1+1/k})
struct prepro *prepro (struct graph *graph, int k)
{
	unsigned int n = graph->V;
	int val = (int) INFINITY;
	struct node nodes[n];
	// k+1, for the kth set where d(A_k, v) = infinity for all v in V
	struct node *dist[k+1];
	// For each v the pivot node to each i, thus v*k
	struct node *pivot_nodes[k];
	struct node *pivot_arr;
	struct aseq *A[k];
	struct heap *heap;
	struct clusterlist *C[k];
	struct bunchlist *bunchlist = malloc (sizeof (struct bunchlist));
	struct prepro *prepro = malloc (sizeof (struct prepro));

	bunchlist->num_bunches = n;
	bunchlist->bunches = malloc (bunchlist->num_bunches * sizeof(struct bunch));
	// saving all v in V in array nodes and Preparing for constructing the bunches
	for (unsigned int i = 0; i < n; i++) {
		memcpy (&nodes[i], add_node (i, val, i), sizeof (struct node));
		bunchlist->bunches[i].v = malloc (sizeof (struct node));
		memcpy (bunchlist->bunches[i].v, &nodes[i], sizeof(struct node));
		bunchlist->bunches[i].piv = malloc (k * sizeof (struct node));
		// Needed for hashing later, thus NULL
		bunchlist->bunches[i].nodes = NULL;
	}

	// Creating all A_i sequences
	create_aseqs (A, k, graph, nodes);

	#ifdef DEBUG
	pp_aseqs (A, k);
	#endif
	// initialising the heap
	heap = initialise_single_source_tz (graph);

	// d(A_k, v) = infinity, thus NULL
	dist[k] = NULL;

	// k iterations
	for (int i = k-1; i >= 0; i--) {

		if (i == k-1 && A[i]->seqsize == 0) {
			printf ("\nA_{k-1} == Ø, hence an empty set. Rerunning the prepro algorithm!\n");
			prepro->nodes = NULL;
			prepro->bunchlist = NULL;
			prepro->success = false;
			sleep (3);
			return prepro;
		}
		else if (A[i]->seqsize == 0) {
			// Empty set, thus d(A_i, v) = infinity (thus NULL array)
			dist[i] = NULL;
			// p_i (v) undefined.
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
		n = write_graph->V;

		// compute d(A_i, v), running dijkstra once for each i
		struct node *sps = dijkstra_alg_tz (write_graph, write_heap);
		dist[i] = sps;

		#ifdef DEBUG
		pp_graph (write_graph);
		printf ("prepro dijkstra result for i=%d:\n", i);
		pp_nodes (dist[i], n);
		#endif

		// No use of write heap and graph anymore
		free_heap (write_heap);
		free_graph (write_graph);

		// Finding the pivot elements, note p_k(v) undefined as A_k = Ø (as in the else case)
		if (i != k-1) {
			pivot_arr = find_pivot (pivot_nodes[i+1], dist[i], n);
		} else {
			pivot_arr = find_pivot (NULL, dist[i], n);
		}

		#ifdef DEBUG
		pp_pivots (pivot_arr, nodes, n);
		#endif

		memcpy (&pivot_nodes[i], &pivot_arr, sizeof (struct node*));
		C[i] = construct_clusters (graph, A, pivot_nodes[i+1], i, k);

		#ifdef DEBUG
		pp_clusters (C, i);
		#endif
	}

	construct_bunches (C, k, A, bunchlist, pivot_nodes);

	#ifdef DEBUG
	pp_bunches (bunchlist);
	#endif

	prepro->nodes = malloc (graph->V * sizeof(struct node));
	prepro->nodes = nodes;
	prepro->bunchlist = malloc (sizeof (struct bunchlist));
	prepro->bunchlist = bunchlist;
	prepro->success = true;
	printf ("***** prepro done ******\n");
	return prepro;
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
	struct prepro *pp = malloc (sizeof (struct prepro));
	pp->success = false;
	while (!pp->success) {
		pp = prepro (graph_3, k);
	}
	int u = 1;
	int v = 2;
	int d = distk (&pp->nodes[u], &pp->nodes[v], pp->bunchlist);
	printf ("result of call dist(%d, %d) = %d\n", u, v, d);
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
