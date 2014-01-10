#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Graph{
	int n;
	int m;
	int *dgrs;
	int **adjs;
}Graph;

void print_g(Graph *g){
	int i, j;
	printf("N:%d\n", g->n);
	printf("M:%d\n", g->m);
	for( i = 0 ; i < g->n ; i++ ){
		printf("%d: [", i+1);
		fflush(stdout);
		if( g->dgrs[i] )
			printf("%d", g->adjs[i][0]);
		for( j = 1 ; j < g->dgrs[i] ; j++)
			printf(",%d", g->adjs[i][j]);
		printf("]\n");
	}
	return;
}

Graph *read_graph(char *name){
	int n, m, a_current, a, b, dgr, res;
	int *adj_temp, *adj;
	FILE *in;
	Graph *g = (Graph*)malloc(sizeof(Graph));
	
	in = fopen(name, "r");

	fscanf(in, "%d", &n);
	fscanf(in, "%d", &m);

	g->n = n;
	g->m = m;
	g->dgrs = (int*)malloc(g->n*sizeof(int));
	g->adjs = (int**)malloc(g->n*sizeof(int*));
	adj_temp = (int*)malloc(g->n*sizeof(int));

	dgr = 0;
	a_current = 0;
	res = fscanf(in, "%d %d", &a, &b);
	while(!feof(in) && res == 2){
		a--; b--;
		if( a > a_current ){          // an adjlist ended!!!
			adj = (int*)malloc((dgr+1)*sizeof(int));
			memcpy(adj, adj_temp, dgr*sizeof(int));
			g->adjs[a_current] = adj;
			g->dgrs[a_current] = dgr;
			dgr = 0;
			a_current++;
		}
		while(a_current < a){
			g->adjs[a_current] = (int*)malloc(1);
			g->dgrs[a_current] = 0;
			a_current++;
		}
		adj_temp[dgr++] = b;
		res = fscanf(in, "%d %d", &a, &b);
	}
	adj = (int*)malloc((dgr+1)*sizeof(int));
	memcpy(adj, adj_temp, dgr*sizeof(int));
	g->adjs[a_current] = adj;
	g->dgrs[a_current] = dgr;

	fclose(in);
	free(adj_temp);

	return g;
}

void print_v(double *v, int n){
	int i;
	for( i = 0 ; i < n ; i++ )
		printf("%f\n", v[i]);
	return;
}

double *eigen(Graph *g, int k){
	int i, j, n, m, *adj;
	double *v_new, *v_old, *v_aux, v_max;
	n = g->n;
	v_new = (double*)malloc(n*sizeof(double));
	v_old = (double*)malloc(n*sizeof(double));
	for( i = 0 ; i < n ; i++ )
		v_new[i] = 1.0;
	while( k-- ){
		v_max = -1;
		v_aux = v_new; v_new = v_old; v_old = v_aux;
		for( i = 0 ; i < n ; i++ ){
			m = g->dgrs[i];
			adj = g->adjs[i];
			v_new[i] = 0.0;
			for( j = 0 ; j < m ; j++ )
				v_new[i] += v_old[adj[j]];
			if( v_max < v_new[j] )
				v_max = v_new[j];
		}
		for( i = 0 ; i < n ; i++ )
			v_new[i] /= v_max;
	}
	free(v_old);
	return v_new;
}

void free_g(Graph *g){
	int i;
	for( i = 0 ; i < g->n ; i++ )
		free(g->adjs[i]);
	free(g->adjs);
	free(g->dgrs);
	free(g);
	return ;
}

int main(int argc, char** argv){
	Graph *g;
	double *v;

	g = read_graph(argv[1]);
	v = eigen(g, 10);
	print_v(v, g->n);
	fflush(stdout);

	free(v);
	free_g(g);
	return 0;
}

