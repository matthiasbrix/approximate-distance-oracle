#include "thorupzwick.h"

#define DEBUG

void pp_aseqs (struct aseq **A, int k)
{
	printf("Pretty print all A_i sequences\n");
	for (int i = 0; i < k; i++) {
		if (A[i]->seqsize > 0) {
			for (int j = 0; j < A[i]->seqsize; j++)
				printf ("A_{%d,%d} v_id:%d spest:%d\n", i, j,
						A[i]->nodes[j].v_id+offset, A[i]->nodes[j].sp_est);
		}
	}
	return;
}

void pp_clusters (struct clusterlist **C, int i)
{
	printf("Pretty print all clusters\n");
	for (int a = 0; a < C[i]->num_clusters; a++)
		for (int b = 0; b < C[i]->clusters[a].num_nodes; b++)
			printf ("cluster nodes for C(w:%d)=v:%d, SP:%d\n", C[i]->clusters[a].w->v_id+offset, C[i]->clusters[a].nodes[b].v_id+offset, C[i]->clusters[a].nodes[b].sp_est);

	return;
}

void pp_bunches (struct bunchlist *bunchlist)
{
	printf("Pretty print all bunches\n");
	for (int i = 0; i < bunchlist->num_bunches; i++) {
		struct node *s;
		// iterate over hash elements of B(v)
		for (s = bunchlist->bunches[i].nodes; s != NULL; s = s->hh.next) {
			printf("B(v:%d)=w:%d, SP:%d\n", bunchlist->bunches[i].v->v_id+offset, s->v_id+offset, s->sp_est);
		}
	}

	return;
}

void pp_pivots (struct node *pivot_arr, struct node *nodes, unsigned int n, int i)
{
	printf("Pretty print all pivot elements (witnesses) \n");
	for (unsigned int j = 0; j < n-1; j++) {
		printf ("p_%d (%d)=%d, dist:%d\n", i, nodes[j].v_id+offset, pivot_arr[nodes[j].v_id].v_id+offset, pivot_arr[nodes[j].v_id].sp_est);
	}
}

/*************************************************************************
 * Begin of the actual functions for thorup-zwick
 *************************************************************************/

/* key: pointer to the key data, keylen is length of key data,
   hashv is an unsigned integer that you need to fill in with the
   answer
 */
// Skriv hvis den er uniformly (det er den)
// -DHASH_FUNCTION=HASH_THORUPZWICK
#define HASH_THORUPZWICK(key,keylen,hashv)           \
do {                                                 \
	srand((unsigned)time(NULL));                     \
	unsigned _ho_i;                                  \
	hashv = 0;										 \
	const uint32_t *x = (const uint32_t*)(key);      \
	for (_ho_i=0; _ho_i < keylen; _ho_i++) {         \
	  uint32_t a = (rand()|1);					     \
	  int l = ((rand() % ((33 - 0 + 0 % 2) >> 1)     \
		  + ((0 - 0 % 2) >> 1)) << 1) + 1;           \
	  (hashv) = (a*(*x)) >> (32-l);					 \
	}                                                \
} while (0)

/**
 * add_s_node_to_graph - adding the super vertex s to the graph
 * @graph: graph
 * @ai: all sequences
 * The added node s will always have the vertex id of the current
 * number of vertices in the graph
 */
void add_s_node_to_graph (struct graph *graph, struct aseq *ai)
{
	for (int i = 0; i < ai->seqsize; i++) {
		add_edges (graph, graph->V, ai->nodes[i].v_id, 0);
	}
	return;
}

/**
 * find_pivot - finding all pivot elements for v in A_i
 * @aiplusone_pivot_arr: A_i+1 set
 * @n: number of nodes in graph, |V|=n
 * Not looping through all v in V and A sets, otherwise running time could be O(n^2)
 * If v.pi = s, it is a pivot node
 * If a node has been visited by a traversing one, save the visitor, such that in a later iteration we check if the respective node has been visited. If so, we simply retrieve the visitor node's pivot node
 * (*) property, if d(A_i, v) == d(A_i+1,v), copy! Means we get p_i(v) = p_i+1(v) ∈ B(v),
 * otherwise, δ(p_i(v), v) = δ(A_i, v) < δ(A_i+1, v). Thus distance will be same, but the node would change (to A_i+1) if (*) fails
 */
struct node* find_pivot (struct node *aiplusone_pivot_arr,
						 struct node *nodes, unsigned int n)
{
	// no need to allocate for s node, therefore n-1
	int *visited_nodes = malloc ((n-1) * sizeof (int));
	for (unsigned int i = 0; i < n-1; i++)
		visited_nodes[i] = -1;

	struct node *pivot_arr = malloc ((n-1) * sizeof (struct node));

	if (pivot_arr == NULL) {
		printf ("Failed to allocate pivot array\n");
		exit (-1);
	}

	// skipping node s in nodes, assuming s is the last node
	for (unsigned int i = 0; i < n-1; i++) {
		struct node *piv = NULL;
		struct node *v = nodes[i].pi;

		if (visited_nodes[nodes[i].v_id] != -1) {
			int pivid = pivot_arr[nodes[i].v_id].v_id;
			memcpy (&pivot_arr[nodes[i].v_id], &pivot_arr[pivid], sizeof(pivot_arr[pivid]));
			pivot_arr[nodes[i].v_id].sp_est = nodes[nodes[i].v_id].sp_est;
		} else if (v->v_id == nodes[n-1].v_id) {
			memcpy (&pivot_arr[nodes[i].v_id], &nodes[i], sizeof(nodes[i]));
			pivot_arr[nodes[i].v_id].sp_est = 0;
		} else {
			while (v) {
				piv = v;
				// avoid reaching the s node
				if (v->pi != NULL && v->pi->v_id == nodes[n-1].v_id) {
					break;
				}
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

	free (visited_nodes);

	return pivot_arr;
}

/**
 * dijkstra_cluster_tz - modified version of dijkstra for constructing clusters
 * @graph: graph
 * @w: all sequences
 * @pivot_nodes: all pivot nodes
 * @k: k integer
 * @i: the current i index
 * Cluster C(w) is composed of all the vertices that are closer to w than any (i+1)-center
 * Note that δ(A_{i+1}, v) was computed in the previous iteration so the test takes constant time.
 * Note that if v not in C(w), then we never assign a finite distance, as by definition, v ∈ C(w) if and only if δ(w, v) < δ(A_i+1, v). This modified version of Dijkstra starts with w in the heap,
 * consequently adding all vertices being relaxed (or relaxed if already added to heap).
 * We relax the edge (u,v) ONLY if u.d + l(u,v) < d(A_{i+1}, v), where d(A_{i+1}, v) = d(p_{i+1}(v), v)
 * Also, if i==k-1, no pivot element to compare with, thus we just relax edge / insert node
 */
struct cluster *dijkstra_cluster_tz (struct graph *graph, struct node *w,
									 struct node *pivot_nodes, int k, int i)
{
	int idx = 0;
	int num_nodes = 0;
	int *in_heap = calloc (graph->V, sizeof (int));
	int *relaxed = calloc (graph->V, sizeof (int));
	struct cluster *cluster = malloc (sizeof (struct cluster));
	struct heap *Q = malloc (sizeof (struct heap));
	Q->nodes = malloc (graph->V * sizeof(struct node*));
	struct node *S = malloc (graph->V * sizeof (struct node));

	if (Q == NULL || S == NULL || in_heap == NULL
		|| relaxed == NULL || Q->nodes == NULL || cluster == NULL) {
		printf ("Failed to allocate memory for in_heap, relaxed, S, Q, Q->nodes or cluster\n");
		exit (-1);
	}

	Q->heap_size = 0;
	// w.d = 0
	min_heap_insert (Q, w->v_id, 0, graph);
	num_nodes++;

	while (Q->heap_size != 0) {
		struct node *u = extract_min (Q);
		// tmp_S[idx] gets assigned value from u, copying to its address
		memcpy (&S[idx], u, sizeof(struct node));
		// avoiding node u can be relaxed in next iterations,
		// as u is no longer on heap
		relaxed[u->v_id] = 1;
		in_heap[u->v_id] = 0;
		for (struct adjlistnode *s = graph->adjlists[S[idx].v_id].head;
			 s != NULL; s = s->next) {
			struct node *v = graph->adjlists[s->v_id].nd;
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

	cluster->nodes = malloc (num_nodes * sizeof (struct node));
	if (cluster->nodes == NULL) {
		printf ("Alloc of cluster->nodes failed\n");
		exit (-1);
	}
	for (int i = 0; i < num_nodes; i++)
		memcpy (&cluster->nodes[i], &S[i], sizeof (struct node));

	cluster->w = w;
	cluster->num_nodes = num_nodes;

	free (in_heap);
	free (relaxed);
	free (Q);

	return cluster;
}

/**
 * construct_clusters - constructing clusters for all w \in A_i - A_i+1
 * @graph: adjacency list rep. of the graph
 * @A: all sequences
 * @pivot_nodes: all pivot nodes
 * @i: the current i index
 * @k: k integer
 * From here we construct the clusters. If i=k-1 the clusters of all w in A_i contain
 * all vertices. Otherwise, we check if w is in A_i - A_i+1. In both cases we compute
 * a modified version of dijkstra from with w as source.
 */
struct clusterlist *construct_clusters (struct graph *graph, struct aseq **A,
										struct node *pivot_nodes, int i, int k)
{
	int num_clusters = 0;
	struct cluster *tmp_clusters = malloc (A[i]->seqsize * sizeof(struct cluster));
	struct clusterlist *C = malloc (sizeof (struct clusterlist));

	// For every w in A_i - A_i+1
	for (int w = 0; w < A[i]->seqsize; w++) {
		// for every w ∈ A_k−1 we have C(w) = V
		if (i == (k-1)) {
			// create cluster C(w) = V, computing the distances from w to all other v in V
			struct cluster *cw = dijkstra_cluster_tz (graph, &A[i]->nodes[w], NULL, k, i);
			memcpy (&tmp_clusters[num_clusters], cw, sizeof (struct cluster));
			num_clusters += 1;
		} else {
			// check if w in A_i - A_i+1
			if (A[i+1]->added[A[i]->nodes[w].v_id]) {
				continue;
			} else {
				// if indeed w in A_i - A_i+1, go on with dijkstra to create a cluster C(w)
				struct cluster *cw = dijkstra_cluster_tz (graph, &A[i]->nodes[w],
														  pivot_nodes, k, i);
				memcpy (&tmp_clusters[num_clusters], cw, sizeof (struct cluster));
				num_clusters += 1;
			}
		}
	}

	C->clusters = malloc (num_clusters * sizeof (struct node));
	if (C->clusters == NULL) {
		printf ("Alloc of C->clusters failed\n");
		exit (-1);
	}
	for (int i = 0; i < num_clusters; i++)
		memcpy (&C->clusters[i], &tmp_clusters[i], sizeof (struct cluster));

	C->num_clusters = num_clusters;

	return C;
}

/**
 * construct_bunches - constructing bunches for all v in V
 * @C: all clusters
 * @k: k integer
 * @A: all sequences
 * @bunchlist: a memory initialised struct of bunches
 * @pivot_nodes: all pivot nodes
 * Generating bunches from the clusters. Runs in linear time of the cluster size
 * such that O(\sum_{w ∈ V} |C(w)|) = O(\sum_{v ∈ V} |B(v)|)
 * For each v in V, we construct a bunch, containing all w of all C(w) v belongs to
 * Note, we get A_{k-1} \subseteq B(v) for every v in V, meaning all nodes from A_{k-1} belong to every bunch
 */
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

/**
 * create_aseqs - constructing sets of vertices of A_0, ... A_k-1
 * @A: the A sequences to fill in
 * @k: k integer
 * @graph: adjacency list rep. of graph
 * @nodes: all n nodes and their attributes
 *
 * Running time O(n)
 * Constructs non increasing sequence of sets A_0 ⊇ A_1 ⊇ ... ⊇ A_k−1, A_k = Ø
 * Begins with A_0 = V and iterates then 1 <= i < k. Each node in A_i-1 is tested
 * with n^-1/k probability whether it is obtained in A_i. Vertices in an A_i set
 * are also called i-centers
 */
bool create_aseqs (struct aseq **A, int k, struct graph *graph, struct node *nodes)
{
	int seqsizes[k];
	int *tmp = malloc (graph->V * sizeof(int));
	// A_k <- Ø
	A[k] = NULL;

	// A_0 <- V. Running time O(n)
	A[0] = malloc (sizeof (struct aseq));
	A[0]->nodes = malloc (graph->V * sizeof(struct node));
	A[0]->added = calloc (graph->V, sizeof(int));
	for (unsigned int i = 0; i < graph->V; i++) {
		memcpy (&A[0]->nodes[i], &nodes[i], sizeof(struct node));
		A[0]->added[i] = 1;
	}

	A[0]->seqsize = graph->V;
	seqsizes[0] = graph->V;
	// Set the seed
	srand((unsigned)time(NULL));

	for (int i = 1; i <= k-1; i++) {
		A[i] = malloc (sizeof (struct aseq));
		A[i]->added = calloc (graph->V, sizeof(int));
		seqsizes[i] = 0;
		for (int j = 0; j < seqsizes[i-1]; j++) {
			// Generates 0.0 - 1.0
			double rnd = (double)rand()/RAND_MAX;
			// Check random number is <= n^{-1/k}
			if (rnd <= pow (graph->V, -1.0/(double)k)) {
				int v_id = A[i-1]->nodes[j].v_id;
				tmp[seqsizes[i]] = v_id;
				A[i]->added[v_id] = 1;
				seqsizes[i] += 1;
			}
		}

		if (seqsizes[i] == 0)
			return false;

		A[i]->seqsize = seqsizes[i];
		A[i]->nodes = malloc (seqsizes[i] * sizeof(struct node));
		for (int l = 0; l < seqsizes[i]; l++) {
			memcpy (&A[i]->nodes[l], &nodes[tmp[l]], sizeof (struct node));
		}
	}

	free (tmp);

	return true;
}

/**
 * prepro - preprocessing algorithm of Thorup-Zwick
 * @graph: vertex u
 * @k: vertex v
 *
 * Running time O(kn^{1+1/k})
 * Calling various componenets (functions) for preprocessing. This includes
 * generating A_i sequences, computing the shortest path from node s to all v in V,
 * consequently determining the pivot nodes, constructing cluster and finally generating
 * the bunches.
 */
struct prepro *prepro (struct graph *graph, int k)
{
	unsigned int n = graph->V;
	int val = (int) INFINITY;
	bool empty;
	struct node *nodes = malloc (n * sizeof (struct node));
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
	empty = create_aseqs (A, k, graph, nodes);
	// Empty set, thus d(A_i, v) = infinity (thus NULL array) */
	// also, since A_{k-1} = Ø, rerun!
	if (!empty) {
		printf ("\nA_{k-1} == Ø, empty set.");
		printf (" Hence, re-running the preprocessing algorithm!\n");
		prepro->nodes = NULL;
		prepro->bunchlist = NULL;
		prepro->success = false;
		return prepro;
	}

	#ifdef DEBUG
	pp_graph (graph);
	pp_aseqs (A, k);
	#endif

	// d(A_k, v) = infinity, thus NULL
	dist[k] = NULL;
	// initialising the heap
	heap = initialise_single_source_tz (graph);

	// k iterations
	for (int i = k-1; i >= 0; i--) {
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

		#ifdef DEBUG
		pp_graph (write_graph);
		#endif

		// compute d(A_i, v), running dijkstra once for each i
		struct node *sps = dijkstra_alg_tz (write_graph, write_heap);
		dist[i] = sps;

		#ifdef DEBUG
		printf ("prepro dijkstra result for i=%d\n", i);
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
		pp_pivots (pivot_arr, nodes, n, i);
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
	prepro->k = k;

	free_heap (heap);

	return prepro;
}

/**
 * dist - distance query of (u, v) with the data structures
 * @u: vertex u
 * @v: vertex v
 * @bunchlist: bunches of all nodes
 * Running time: O(k) = O(1)
 * u and v are the two vertices whose distance is to be estimated.
 * If w in B(v) it returns d(w, u) + d(w, v). The distance d(w, u) = d(p_i(u), u) is
 * read directly from the data structure constructed in prepro. Also, d(w, v) = d(v, w)
 * at most k accesses as w in A_{k-1} and A_{k-1} \subseteq for every v in V
 */
int dist (struct node *u, struct node *v, struct bunchlist *bunchlist)
{
	struct node	*w;
	int i = 0;
	int dist = 0;

	// w = u = p_0(u)
	w = u;

	// while w not in B(v)
	while (1) {
		struct node *out, *tmp;
		printf ("B(%d) w_id:%d\n", v->v_id+offset, w->v_id+offset);
		// checking if w in B(v), where nodes are vertices w in V
		HASH_FIND_INT(bunchlist->bunches[v->v_id].nodes, &w->v_id, out);
		if (out) {
			// if w in B(v), break loop
			dist = out->sp_est;
			break;
		} else {
			printf ("i=%d\n", i);
			// i <- i + 1
			i += 1;
			// (u, v) <- (v, u)
			tmp = u;
			u = v;
			v = tmp;
			// w <- p_i (u)
			w = &bunchlist->bunches[u->v_id].piv[i];
			printf ("w_id:%d, sp:%d u_id:%d v_id:%d\n", w->v_id+offset, w->sp_est, u->v_id+offset, v->v_id+offset);
		}
	}

	return dist;
}
