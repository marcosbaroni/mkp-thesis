/* Dimensions */
int N = ...;             // number of actions
int Y = ...;             // number of years
int R = ...;             // number of resources
float r = ...;           // rate of return

/* Depedency type */
tuple dep {int i; int j; int k;}

/* Groups */
range Ns = 1..N;         // actions
range Ys = 1..Y;         // years
range Rs = 1..R;         // resources

/* Parameters */
float g[Ys] = ...;       // yearly goal
float p[Rs][Ys] = ...;   // yearly budget
int m[Ns] = ...;         // global market
int u[Ns][Ys] = ...;     // anual market
float c[Ns][Rs] = ...;   // cost of action
float v[Ns] = ...;       // value of energy
float e[Ns][Ys] = ...;   // energy recovery
setof(dep) D = ...;      // depedencies

/* Variable */
dvar int+ x[Ns][Ys];

/* Equations */
// Total recovered energy on period 'k' by action 'i'
dexpr float rec[i in Ns][k in Ys] = 
	sum(k2 in Ys : k2 <= k)
		x[i, k2]*e[i, (k-k2+1)];
// Total energy recovered on the 'k'-th period after plan, by action 'i'
dexpr float rec2[i in Ns][k in Ys] =
	sum(k2 in Ys : k2 >= k+1)
		x[i, k2]*e[i, (Y+k-k2+1)];
// Cost of all actions on period k
dexpr float cost[i in Ns][k in Ys] =
	sum(l in Rs)
		x[i, k]*c[i, l];

/* Objective */
maximize
	sum ( i in Ns )
		sum ( j in Ys )
			(rec[i, j]*v[i] - cost[i, j])/((i+r)^j)
	+
	sum ( i in Ns )
		sum ( j in Ys )
			rec2[i, j]*v[i]/((1+r)^(Y+j));

/* Constraints */
subject to {
	// Anual goal
	forall( j in Ys )
		sum( i in Ns )
			x[i, j] <= g[j];

	// Anual budget
	forall( j in Ys )
		forall( l in Rs )
			sum( i in Ns )
				x[i, j]*c[i, l] <= p[l, j];

	// Mercado Global
	forall( i in Ns)
		sum( j in Ys )
			x[i, j] <= m[i];

	// Mercado Anual
	forall( i in Ns )
		forall( j in Ys )
			x[i, j] <= u[i, j];

	// Dependência entre as ações
	forall( j in Ys )
		forall( <i1, i2, q> in D )
			sum ( j2 in Ys : j2 < j)
				x[i1, j2] <=
			sum ( j2 in Ys : j2 < j)
				q*x[i2, j2];
}

