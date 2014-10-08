#include "kp.h"

KP *kp_read_file(char *filename){
	KP *kp;
	FILE *fin;

	fin = fopen(filename, "r");
	kp = kp_read(fin);
	fclose(fin);

	return kp;
}

KP *kp_read(FILE *in){
	int n;
	KP *kp;

	kp = (KP*)malloc(sizeof(KP));
	fscanf(in, "%d", &n);
	kp->n = n;

	kp->p = long_long_array_read(in, n);
	kp->w = long_long_array_read(in, n);
	fscanf(in, "%lld", &(kp->b));

	return kp;
}

void kp_write(FILE *out, KP *kp){
	fprintf("%d\n", kp->n);
	long_long_array_write(out, kp->p);
	long_long_array_write(out, kp->w);
	fprintf("%lld\n", kp->b);
	return;
}

void kp_free(KP *kp){
	free(kp->p);
	free(kp->w);
	free(kp);
	return;
}

void kp_fprintf(FILE *out, KP *kp){
	return;
}

void kp_to_zimpl(FILE *fout, KP *kp){
	int n, i;
	n = kp->n;

	/* SIZES */
	fprintf(fout, "param n := %d;\n", n);

	/* SETS */
	fprintf(fout, "set N := {1 .. %d};\n", n);

	/* PARAMETERS */
	fprintf(fout, "param p[N] :=\n"); /* profits */
	long_long_array_zimpl_print(fout, kp->p, n);
	fprintf(fout, "param w[N] :=\n"); /* weights */
	long_long_array_zimpl_print(fout, kp->w, n);
	fprintf(fout, "param b := %lld;\n", kp->b);

	/* DECISION VARIABLES */
	fprintf(fout, "var x[N] binary;\n");

	/* CONSTRAINT */
	fprintf(fout, /* total weight of selected itens */
"subto capacities:\n\
	sum <i> in N do\n\
		x[i]*w[i] <= b;\n");

	/* OBJECTIVE FUNCTION */
	fprintf(fout, /* total profit of selected itens */
"maximize profit:\n\
	sum <i> in N do\n\
		x[i]*p[i];\n");
	
	return;
}

