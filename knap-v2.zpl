######################
# Problem Parameters #
######################

# Number of actions
param N := read "knap.dat" as "1n" use 1 comment "#";

# Number of years
param Y := read "knap.dat" as "1n" skip 1 use 1 comment "#";

# Number of periods per year
param P := read "knap.dat" as "1n" skip 2 use 1 comment "#";

# Number of resources
param R := read "knap.dat" as "1n" skip 3 use 1 comment "#";

# Rate of return
param r := read "knap.dat" as "1n" skip 4 use 1 comment "#";


##############
# Index Sets #
##############

# Actions
set Acs := {1 .. N};

# Years
set Yrs := {1 .. Y};

# Periods
set Pers := {1 .. Y*P};

# Resources
set Res := {1 .. R};


######################
# Indexed Parameters #
######################

# Yearly goal
param g[Yrs] := read "knap.dat" as "n+" skip 5 use Y comment "#";

# Global budget
param o[Res] := read "knap.dat" as "n+" skip (5+Y) use R comment "#";

# Yearly budgets
param p[Yrs] := read "knap.dat" as "n+" skip (5+Y+R) use Y comment "#";

# Periodal Budgets
param s[Pers*Res] := read "knap.dat" as "n+" skip (5+(2*Y)+R) use (Y*R) comment "#";

# Global Market
param m[Acs] := read "knap.dat" as "n+" skip (5+(2*Y)+R+(Y*R)) use N comment "#";

# Anual Market
param z[Acs*Yrs] := read "knap.dat" as "n+" skip (5+(2*Y)+R+(Y*R)) use (N*Y) comment "#";

# Periodal Market
param c[Acs*Pers] := read "knap.dat" as "n+" skip (5+(2*Y)+R+(Y*R)+(N*Y)) use (N*P) comment "#";

# Costs of actions
param c[Acs*Res] := read "knap.dat" as "n+" skip (5+(2*Y)+R+(Y*R)+(N*Y)) use (N*R) comment "#";

# Energy Value
param v[Acs] := read "knap.dat" as "n+" skip (5+(2*Y)+R+(Y*R)+(N*Y)+(N*R)) use N comment "#";

# Energy Recover
param e[Acs*Pers] := read "knap.dat" as "n+" skip (5+(2*Y)+R+(Y*R)+(N*Y)+(N*R)+N) use (N*P) comment "#";


#####################
# Decision Variable #
#####################

# Number of actions by given period
var x[Acs*Pers] integer;


####################
# Others variables #
####################

# Energy recover for a given period caused by a given action
var rec[Act*Pers];

# Profit for energy recovering for a given period
var prof[Pers];

# Total cost of all actions executed on a given period
var cost[Pres];


#############
# Equations #
#############
# Total yearly costs
subto definition_yearly_costs:
	forall <i> in I do
		sum <j,k> in J*K do
			x[i,j]*action_costs[j,k] == total_costs[i];

# Total yearly income
subto definition_yearly_income:
	forall <i> in I do
		sum <k,j> in I*J with i>=k do
			(x[k,j]*energy[i-k+1,j]*profit[j]) == total_income[i];


##############
# Constraint #
##############

# Budgets restrictions
subto limit_budget: 
	forall <i,k> in I*K do
		sum <j> in J do
			x[i,j]*action_costs[j,k] <= budgets[i,k];
# Goal restrictions
subto limit_goal:
	forall <i> in I do
		sum <k,j> in I*J with (i >= k) do
			x[k,j]*energy[i-k+1,j] <= goal[i];

# Market restrictions
subto limit_market:
	forall <j> in J do
		sum <i> in I do
			x[i,j] <= market[j];

# Lowerbound of actions
subto lowerbound_actions:
	forall <i,j> in I*J with (lb[i,j] >= 0) do
		x[i,j] >= lb[i,j];

# Upperbound of actions
subto upperbound_actions:
	forall <i,j> in I*J with (ub[i,j] >= 0) do
		x[i,j] <= ub[i,j];

# Dependecy between actions
subto dependency_cardinality:
	forall <i,j,k> in I*J*J with (dep[j,k] >= 0) do
		sum <l> in I with (i >= l) do
			x[l,j] <= 
		sum <l> in I with (i >= l) do
		 	x[l,k]*dep[j,k];


######################
# Objective Function #
######################

maximize npv: 
	sum <i> in I do 
		(total_income[i]-total_costs[i])/(1+rate)^i;
