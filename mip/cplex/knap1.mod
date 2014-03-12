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

