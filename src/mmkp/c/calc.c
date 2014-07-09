#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	FILE *input;
	int i, n, nmax, res;
	double *v, x, sum, med;

	if(argc < 2) input = stdin;
	else input = fopen(argv[1], "r");

	nmax = 1000;
	v = (double*)malloc(nmax*sizeof(double));

	res = fscanf(input, "%lf", &x); 
	while( res > 0 ){
		if( nmax == n ){
			nmax *= 2;
			v = realloc(v, nmax*sizeof(double));
		}
		res = fscanf(input, "%lf", &x); 
		v[n++] = x;
	}

	sum = 0;
	for( i = 0 ; i < n ; i++ )
		sum += v[i];
	
	med = sum/n;

	printf("%lf\n", med);

	fclose(input);

	return 0;
}

