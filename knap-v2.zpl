######################
# Problem Parameters #
######################

# Number of years
param y := read "knap.dat" as "1n" use 1 comment "#";

# Number of periods
param p := read "knap.dat" as "1n" use 1 comment "#";

#Number of actions
param n := read "knap.dat" as "1n" skip 1 use 1 comment "#";

#Number of budgets
param R := read "knap.dat" as "1n" skip 2 use 1 comment "#";

#Minimum acceptable rate of return
param r := read "knap.dat" as "1n" skip 3 use 1 comment "#";

##############
# Index Sets #
##############

#Years
set Yrs := {1 .. y};

#Actions
set Acs := {1 .. n};

#Costs
set Res := {1 .. costs};

######################
# Indexed Parameters #
######################

#Yearly goal
param goal[I] := read "knap.dat" as "1n" skip 4 use duration comment "#";

#Yearly budgets
param budgets[I*K] := read "knap.dat" as "n+" skip (4+duration) use duration comment "#";

#Market per action
param market[J] := read "knap.dat" as "1n" skip (4+(duration*2)) use actions comment "#";

#Costs of actions
param action_costs[J*K] := read "knap.dat" as "n+" skip (4+(duration*2)+actions) use actions comment "#";

#Profit per energy unit of each action
param profit[J] := read "knap.dat" as "1n" skip (4+(duration*2)+(actions*2)) use actions comment "#";

#Yearly energy per action
param energy[I*J] := read "knap.dat" as "n+" skip (4+(duration*2)+(actions*3)) use duration comment "#";

#Lowerbound of actions
param lb[I*J] := read "knap.dat" as "n+" skip (4+(duration*3)+(actions*3)) use duration comment "#";

#Upperbound of actions
param ub[I*J] := read "knap.dat" as "n+" skip (4+(duration*4)+(actions*3)) use duration comment "#";

#Dependency between actions
param dep[J*J] := read "knap.dat" as "n+" skip (4+(duration*5)+(actions*3)) use actions comment "#";

#####################
# Decision Variable #
#####################

#Number of actions by given year
var x[I*J] integer;

####################
# Others variables #
####################

#Yearly total costs
var total_costs[I];

#Yearly total income
var total_income[I];

###############
# Definitions #
###############
#Total yearly costs
subto definition_yearly_costs:
	forall <i> in I do
		sum <j,k> in J*K do
			x[i,j]*action_costs[j,k] == total_costs[i];

#Total yearly income
subto definition_yearly_income:
	forall <i> in I do
		sum <k,j> in I*J with i>=k do
			(x[k,j]*energy[i-k+1,j]*profit[j]) == total_income[i];

################
# Restrictions #
################

#Budgets restrictions
subto limit_budget: 
	forall <i,k> in I*K do
		sum <j> in J do
			x[i,j]*action_costs[j,k] <= budgets[i,k];
#Goal restrictions
subto limit_goal:
	forall <i> in I do
		sum <k,j> in I*J with (i >= k) do
			x[k,j]*energy[i-k+1,j] <= goal[i];

#Market restrictions
subto limit_market:
	forall <j> in J do
		sum <i> in I do
			x[i,j] <= market[j];

#Lowerbound of actions
subto lowerbound_actions:
	forall <i,j> in I*J with (lb[i,j] >= 0) do
		x[i,j] >= lb[i,j];

#Upperbound of actions
subto upperbound_actions:
	forall <i,j> in I*J with (ub[i,j] >= 0) do
		x[i,j] <= ub[i,j];

#Dependecy between actions
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
