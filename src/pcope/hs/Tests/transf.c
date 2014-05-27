#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int main (){
	int n, m, i, j, count, res;
	char vec[1000*1000];
	int c;

	count = 0;
	m = 0;
	while(res = scanf("%d", &c) == 1){
		vec[count++] = c;
		if(c) m++;
	}
	n = (int)floor(sqrt(count));
	m /= 2;

	printf("%d\n%d\n", n, m);
	for(i = 0 ; i < n ; i++ )
		for( j = 0 ; j < n ; j++ )
			if(vec[(i*n)+j])
				printf("%d %d\n", i+1, j+1);
	return;
}

