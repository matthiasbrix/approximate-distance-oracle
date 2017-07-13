#include "sssp.h"

/**
 * diff - measuring cpu time
 * @start: start time
 * @end: end time
 * Computes the cpu time spent on some code. This measurement
 * is far more accurate than clock_t
 */
struct timespec diff (struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = BILLION+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

/**
 * initialise_single_source - initialising a heap from a single source
 * @graph: graph
 * @s: source vertex id
 * Running time: O(n)
 */
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

/**
 * bidirectional_dijkstra - Bidirectional Dijkstra's algorithm
 * @gf: the graph G = (V, E) for the forward search
 * @u: source vertex
 * @v: target vertex
 * Running time: O((m + n) lg n)
 * Running Dijkstra's from the source (forward search) AND target vertex (target vertex).
 * We have two graph struct, one for the forward search, one for the backward search.
 * We start the execution by initialising two heaps, with u and v as single vertices.
 * Then we execute one search at a time.
 */
struct ssp_res *bidirectional_dijkstra (struct graph *gf, int s, int t)
{
	// min-priority queue for forward search
	struct heap *Q_f = malloc (sizeof (struct heap));
	Q_f->nodes = malloc (gf->V * sizeof(struct node*));
	// min-priority queue for backwards search
	struct heap *Q_b = malloc (sizeof (struct heap));
	Q_b->nodes = malloc (gf->V * sizeof(struct node*));
	// Auxiliary arrays
	int *in_Qf_heap = calloc (gf->V, sizeof (int));
	int *in_Qb_heap = calloc (gf->V, sizeof (int));
	int *extracted_qf = calloc (gf->V, sizeof(int));
	int *extracted_qb = calloc (gf->V, sizeof(int));
	int rootqf, rootqb;
	struct timespec start, end;

	struct ssp_res *res = malloc (sizeof (struct ssp_res));

	// For forward search
	struct node *S_f = malloc (gf->V * sizeof (struct node));
	memset (S_f, 0, gf->V * sizeof (struct node));
	// For backwards search
	struct node *S_b = malloc (gf->V * sizeof (struct node));
	memset (S_b, 0, gf->V * sizeof (struct node));

	if (Q_f == NULL || Q_b == NULL || in_Qf_heap == NULL || in_Qf_heap == NULL ||
		extracted_qf == NULL || extracted_qb == NULL || S_f == NULL || S_b == NULL) {
		perror ("Pointer error in bidirectional dijkstra.\n");
		exit (-1);
	}

	res->avg_min_heap_insert_time = 0.0;
	res->avg_extract_min_time = 0.0;
	res->avg_decrease_key_time = 0.0;
	res->num_decrease_key = 0;
	res->num_extract_min = 0;
	res->visited_nodes = 0;
	res->visited_edges = 0;
	Q_f->heap_size = 0;
	Q_b->heap_size = 0;
	// Copy of graph for backwards search
	struct graph *gb = copy_graph_struct (gf, Q_b);
	min_heap_insert (Q_f, s, 0, gf);
	min_heap_insert (Q_b, t, 0, gb);
	int sp_estimate = (int) INFINITY;

	while (Q_f->heap_size != 0 && Q_b->heap_size != 0) {

		rootqf = (minimum (Q_f))->sp_est;
		rootqb = (minimum (Q_b))->sp_est;

		if (rootqf < rootqb) {
			// Forward search begins here
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
			struct node *u = extract_min (Q_f);
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
			res->avg_extract_min_time += diff(start,end).tv_nsec;
			res->num_extract_min += 1;
			memcpy (&S_f[u->v_id], u, sizeof(struct node));
			for (struct adjlistnode *s = gf->adjlists[u->v_id].head;
				 s != NULL; s = s->next) {
				struct node *v = gf->adjlists[s->v_id].nd;
				int sp_est = u->sp_est + s->weight;
				res->visited_edges += 1;
				if (!extracted_qf[s->v_id] && !in_Qf_heap[s->v_id]) {
					clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
					min_heap_insert (Q_f, s->v_id, sp_est, gf);
					clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
					res->avg_min_heap_insert_time += diff(start,end).tv_nsec;
					gf->adjlists[s->v_id].nd->pi = u;
					in_Qf_heap[s->v_id] = sp_est;
					res->visited_nodes += 1;
				} else if (v != NULL && (v->sp_est > sp_est) && !extracted_qf[s->v_id]) {
					clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
					decrease_key (Q_f, v, u, sp_est);
					clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
					res->avg_decrease_key_time += diff(start,end).tv_nsec;
					in_Qf_heap[s->v_id] = sp_est;
					res->num_decrease_key += 1;
				}
			}
			extracted_qf[u->v_id] = 1;
			// u intersects in S_1 and S_2
			if (extracted_qf[u->v_id] && extracted_qb[u->v_id]) {
				if (sp_estimate >= S_f[u->v_id].sp_est + S_b[u->v_id].sp_est) {
					sp_estimate = S_f[u->v_id].sp_est + S_b[u->v_id].sp_est;
				} else {
					res->dist = sp_estimate;
					res->S_f = S_f;
					res->S_b = S_b;
					return res;
				}
				// u intersects in Q_1 and Q_2
			} else if (in_Qf_heap[u->v_id] && in_Qb_heap[u->v_id] &&
				sp_estimate >= in_Qf_heap[u->v_id] + in_Qb_heap[u->v_id]) {
				sp_estimate = in_Qf_heap[u->v_id] + in_Qb_heap[u->v_id];
			}
		} else {
			// Backwards search begins here
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
			struct node *u = extract_min (Q_b);
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
			res->avg_extract_min_time += diff(start,end).tv_nsec;
			memcpy (&S_b[u->v_id], u, sizeof(struct node));
			res->num_extract_min += 1;
			for (struct adjlistnode *s = gb->adjlists[u->v_id].head;
				 s != NULL; s = s->next) {
				struct node *v = gb->adjlists[s->v_id].nd;
				int sp_est = u->sp_est + s->weight;
				res->visited_edges += 1;
				if (!extracted_qb[s->v_id] && !in_Qb_heap[s->v_id]) {
					clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
					min_heap_insert (Q_b, s->v_id, sp_est, gb);
					clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
					res->avg_min_heap_insert_time += diff(start,end).tv_nsec;
					gb->adjlists[s->v_id].nd->pi = u;
					in_Qb_heap[s->v_id] = sp_est;
					res->visited_nodes += 1;
				} else if (v != NULL && (v->sp_est > sp_est) && !extracted_qb[s->v_id]) {
					clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
					decrease_key (Q_b, v, u, sp_est);
					clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
					res->avg_decrease_key_time += diff(start,end).tv_nsec;
					in_Qb_heap[s->v_id] = sp_est;
					res->num_decrease_key += 1;
				}
			}
			extracted_qb[u->v_id] = 1;
			// u intersects in S_1 and S_2
			if (extracted_qf[u->v_id] && extracted_qb[u->v_id]) {
				if (sp_estimate >= S_f[u->v_id].sp_est + S_b[u->v_id].sp_est) {
					sp_estimate = S_f[u->v_id].sp_est + S_b[u->v_id].sp_est;
				} else {
					res->dist = sp_estimate;
					res->S_f = S_f;
					res->S_b = S_b;
					return res;
				}
				// u intersects in Q_1 and Q_2
			} else if (in_Qf_heap[u->v_id] && in_Qb_heap[u->v_id] &&
				sp_estimate >= in_Qf_heap[u->v_id] + in_Qb_heap[u->v_id]) {
				sp_estimate = in_Qf_heap[u->v_id] + in_Qb_heap[u->v_id];
			}
		}
	}

	res->dist = sp_estimate;
	res->S_f = S_f;
	res->S_b = S_b;

	free_graph (gf);
	free_graph (gb);
	free_heap (Q_f);
	free_heap (Q_b);
	FREE (S_f);
	FREE (S_b);
	FREE (in_Qf_heap);
	FREE (in_Qb_heap);
	FREE (extracted_qf);
	FREE (extracted_qb);

	return res;
}

/**
 * dijkstra_opt_alg - An optimised variant of Dijkstra's original algorithm
 * @graph: graph
 * @s: source vertex id
 * @t: target vertex id
 * Running time: O((m + n) lg n)
 * Searching all shortest-paths from the single source s, but halts as soon
 * as we add the target vertex t to the closed set S
 */
struct ssp_res *dijkstra_opt_alg (struct graph *graph, int s, int t)
{
	struct ssp_res *res = malloc (sizeof (struct ssp_res));
	int *in_heap = calloc (graph->V, sizeof(int));
	int *extracted = calloc (graph->V, sizeof(int));
	struct timespec start, end;
	struct heap *Q = malloc (sizeof (struct heap));
	Q->nodes = malloc (graph->V * sizeof(struct node*));
	struct node *S = malloc (graph->V * sizeof (struct node));

	if (res == NULL || Q == NULL || S == NULL
		|| Q->nodes == NULL || in_heap == NULL
		|| extracted == NULL) {
		perror ("Failed to allocate memory in dijkstra_opt_alg\n");
		exit (-1);
	}

	res->avg_min_heap_insert_time = 0.0;
	res->avg_extract_min_time = 0.0;
	res->avg_decrease_key_time = 0.0;
	res->num_decrease_key = 0;
	res->num_extract_min = 0;
	res->visited_nodes = 0;
	res->visited_edges = 0;

	Q->heap_size = 0;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
	min_heap_insert (Q, s, 0, graph);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	res->avg_min_heap_insert_time += diff(start,end).tv_nsec;
	res->visited_nodes += 1;

	while (Q->heap_size != 0) {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		struct node *u = extract_min (Q);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
		res->num_extract_min += 1;
		res->avg_extract_min_time += diff(start,end).tv_nsec;
		memcpy (&S[u->v_id], u, sizeof (struct node));
		extracted[u->v_id] = 1;
		in_heap[u->v_id] = 0;
		for (struct adjlistnode *s = graph->adjlists[u->v_id].head;
			 s != NULL; s = s->next) {
			struct node *v = graph->adjlists[s->v_id].nd;
			int sp_est = u->sp_est + s->weight;
			res->visited_edges += 1;
			if ((v != NULL) && (v->sp_est > sp_est) && in_heap[s->v_id]) {
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
				decrease_key (Q, v, u, sp_est);
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
				res->avg_decrease_key_time += diff(start,end).tv_nsec;
				res->num_decrease_key += 1;
			} else if (!in_heap[s->v_id] && !extracted[s->v_id]) {
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
				min_heap_insert (Q, s->v_id, sp_est, graph);
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
				res->avg_min_heap_insert_time += diff(start,end).tv_nsec;
				graph->adjlists[s->v_id].nd->pi = u;
				in_heap[s->v_id] = 1;
				res->visited_nodes += 1;
			}
		}
		graph->adjlists[u->v_id].nd = NULL;
		if (u->v_id == t) {
			free_heap (Q);
			res->S_f = S;
			return res;
		}
	}
	free_heap (Q);
	return NULL;
}

/**
 * dijkstra_alg - Dijkstra's original algorithm
 * @graph: graph
 * @s: source vertex id
 * Running time: O((m + n) lg n)
 * Searching all shortest-paths from the single source s
 */
struct ssp_res *dijkstra_alg (struct graph *graph, int s)
{
	struct ssp_res *res = malloc (sizeof (struct ssp_res));
	struct timespec start, end;
	struct heap *Q = initialise_single_source (graph, s);
	struct node *S = malloc (Q->heap_size * sizeof (struct node));

	if (Q == NULL || S == NULL) {
		perror ("Fail in dijkstra_alg\n");
		exit (-1);
	}

	res->avg_extract_min_time = 0.0;
	res->avg_decrease_key_time = 0.0;
	res->num_extract_min = 0;
	res->num_decrease_key = 0;
	res->visited_nodes = graph->V;
	res->visited_edges = 0;

	while (Q->heap_size != 0) {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		struct node *u = extract_min (Q);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
		res->avg_extract_min_time += diff(start,end).tv_nsec;
		res->num_extract_min += 1;
		memcpy (&S[u->v_id], u, sizeof (struct node));
		for (struct adjlistnode *s = graph->adjlists[u->v_id].head;
			 s != NULL; s = s->next) {
			struct node *v = graph->adjlists[s->v_id].nd;
			int sp_est = u->sp_est + s->weight;
			res->visited_edges += 1;
			if ((v != NULL) && (v->sp_est > sp_est)) {
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
				decrease_key (Q, v, u, sp_est);
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
				res->avg_decrease_key_time += diff(start,end).tv_nsec;
				res->num_decrease_key += 1;
			}
		}
		graph->adjlists[u->v_id].nd = NULL;
	}
	free_heap (Q);
	res->S_f = S;
	return res;
}

struct ssp_res *astar (struct graph *graph, struct prepro *pp, int src, int targ)
{
	struct ssp_res *res = malloc (sizeof (struct ssp_res));
	int *in_heap = calloc (graph->V, sizeof(int));
	int *extracted = calloc (graph->V, sizeof(int));
	struct timespec start, end;
	struct heap *Q = malloc (sizeof (struct heap));
	Q->nodes = malloc (graph->V * sizeof(struct node*));
	struct node *S = malloc (graph->V * sizeof (struct node));

	if (res == NULL || Q == NULL || S == NULL
		|| Q->nodes == NULL || in_heap == NULL
		|| extracted == NULL) {
		perror ("Failed to allocate memory in dijkstra_opt_alg\n");
		exit (-1);
	}

	res->avg_min_heap_insert_time = 0.0;
	res->avg_extract_min_time = 0.0;
	res->avg_decrease_key_time = 0.0;
	res->num_decrease_key = 0;
	res->num_extract_min = 0;
	res->visited_nodes = 0;
	res->visited_edges = 0;

	Q->heap_size = 0;
	min_heap_insert (Q, src, 0, graph);
	res->visited_nodes += 1;
	while (Q->heap_size != 0) {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		struct node *u = extract_min (Q);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
		res->num_extract_min += 1;
		res->avg_extract_min_time += diff(start,end).tv_nsec;
		memcpy (&S[u->v_id], u, sizeof (struct node));
		extracted[u->v_id] = 1;
		in_heap[u->v_id] = 0;

		if (u->v_id == targ) {
			free_heap (Q);
			res->S_f = S;
			return res;
		}
		for (struct adjlistnode *s = graph->adjlists[u->v_id].head;
			 s != NULL; s = s->next) {
			struct node *v = graph->adjlists[s->v_id].nd;
			int gv = u->sp_est + s->weight;
			res->visited_edges += 1;
			if ((v != NULL) && (v->gn < gv) && in_heap[s->v_id]) {
				continue;
			} else if (!in_heap[s->v_id] && extracted[s->v_id] &&
					   (S[s->v_id].gn < gv)) {
				continue;
			}
			if (in_heap[s->v_id]) {
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
				decrease_key (Q, v, u, gv);
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
				res->avg_decrease_key_time += diff(start,end).tv_nsec;
				res->num_decrease_key += 1;
			} else {
				// both for in S and not in S and not in Q
				memset (&S[s->v_id], 0, sizeof(S[s->v_id]));
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
				min_heap_insert (Q, s->v_id, gv, graph);
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
				res->avg_min_heap_insert_time += diff(start,end).tv_nsec;
				graph->adjlists[s->v_id].nd->pi = u;
				res->visited_nodes += 1;
			}
			graph->adjlists[s->v_id].nd->gn = gv;
			int hv = dist (&pp->nodes[s->v_id],
						   &pp->nodes[targ], pp->bunchlist);
			hv = hv / (2*pp->k-1);
			graph->adjlists[s->v_id].nd->sp_est = gv + hv; // f(v)
			in_heap[s->v_id] = 1;
			res->visited_nodes += 1;
		}
	}
	free_heap (Q);
	return NULL;
}
