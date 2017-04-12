#include "graph.h"

#define DEBUG

struct Graph* init_graph (int V)
{
	struct Graph *graph = malloc (sizeof (struct Graph));
	graph->V = V;
	graph->adjlists = malloc (V * sizeof (struct Adjlist));
	for (int i = 0; i < V; i++) {
		graph->adjlists[i].head = NULL;
	}
	return graph;
}

struct AdjListNode* add_node (int v, int weight)
{
	struct AdjListNode *node = malloc (sizeof (struct AdjListNode));
	node->v_id = v;
	node->weight = weight;
	node->next = NULL;

	return node;
}

void add_edges (struct Graph *graph, int u, int v, unsigned int w)
{
	struct AdjListNode *node = add_node (v, w);
	node->next = graph->adjlists[u].head;
	graph->adjlists[u].head = node;
	// undirected -> add other way as well
	node = add_node (u, w);
	node->next = graph->adjlists[v].head;
	graph->adjlists[v].head = node;
}

// O(n)
struct heap_t* initialise_single_source (struct Graph *graph, int s)
{

	struct heap_t *heap = malloc (sizeof (struct heap_t));
	heap->nodes = malloc(graph->V * sizeof(struct node*));

	if (heap == NULL || heap->nodes == NULL) {
		printf ("Pointer error of heap\n");
		return NULL;
	}

	int val = (int) INFINITY;

	for (unsigned int i = 0; i < graph->V; i++) {
		struct node *tmp = add_heap_node (i, val);
		heap->nodes[i] = tmp;
		graph->adjlists[i].nd = tmp;
	}

	heap->heap_size = graph->V;
	struct node *v = graph->adjlists[s].nd;
	heap->nodes[v->v_id]->sp_est = 0;

	return heap;
}

/*
1) Det burde ikke være nødvendigt at bruge en hash-tabel. Hvis jeg forstår dit problem korrekt, så burde du kunne nøjes med bidirected pointers mellem hver knude i grafen og hver knude i din heap. Disse pointers kræver tilsammen O(n) plads, men den mængde plads skal du alligevel bruge på at gemme grafen, dvs. dit asymptotiske pladsforbrug øges ikke med disse pointers. Hvis du bruger heap-implementationen fra Cormen, kan du i stedet for pointers bruge heltal mellem 0 og n-1 (eller mellem 1 og n hvis du bruger 1-indekseringen fra Cormen) til at indikere knuder i heap'en, dvs. et heltal indikerer positionen for knuden i heap-array'et. Ligeledes kan grafknuder repræsenteres med heltal mellem 0 og n-1, hvis de er gemt i et array.
 */

// The total run time is O(V lg V + E lg V), which is O(E lg V) because V is O(E) assuming a connected graph.
struct node* dijkstra_alg (struct Graph *graph, int s)
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

void pp_graph (struct Graph* graph)
{
	printf ("PRINT ADJLIST RESULTS\n");
	// Skip first, which is not exist in DIMACS files
	for (unsigned int i = 0; i < graph->V; i++) {
		struct AdjListNode* node = graph->adjlists[i].head;
		printf("vertex: %d", i);
		while (node) {
			printf(" -> v:%i w:%d", node->v_id,
				   node->weight);
			node = node->next;
		}
		printf("\n");
	}
}

void pp_nodes (struct node *S, int i)
{
	printf ("PRINT SSP RESULTS\n");
	for (int j = 0; j < i; j++) {
		if (S[j].pi == NULL) {
			printf ("vertex: %d v.d: %d v.pi NULL\n", S[j].v_id, S[j].sp_est);
		} else {
			printf ("vertex: %d v.d: %d v.pi %d\n", S[j].v_id, S[j].sp_est, S[j].pi->v_id);
		}
	}
}
