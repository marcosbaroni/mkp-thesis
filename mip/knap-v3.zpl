# Autor: MArcos Daniel Baroni
# Data: Ter Jan 21 18:05:11 BRST 2014
# 
# Modelagem do problema de recuperação de perdas:
#   - Sem dependência entre as ações
#	- Considerando o lucro dos periodos a mais (fora do plano)

# Load data
include "data.zpl";

# Extra Set ("Doubled Pers"): All periods, 
set DPers := {1 .. 2*Y*P};

#####################
# Decision Variable #
#####################

# Number of actions by given period
var x[Acs*Pers] integer;


####################
# Others variables #
####################

# Energy recover for a given period caused by a given action
#   rec[a, k1, k2] is the energy recovered by action "a" (taken made on period "k1"),
#   "k2" after it
var rec[Acs*DPers];

# Profit for energy recovering for a given period
var prof[DPers];

# Total cost of all actions executed on a given period
var cost[Pers];


#############
# Equations #
#############
# Total energy recovered on Period "k" by action "i"
subto rec_def:
  forall <i, k> in Acs*DPers do
  	sum <k2> in Pers with 1 <= k-k2 and k-k2 <= Y*P do
	  x[i, (k-k2)]*e[i, k2] == rec[i, k];

# Profit by energy recovery for period k>
subto prof_def:
  forall <k> in DPers do
    sum <i> in Acs do
	  rec[i, k]*v[i] == prof[k];

# Cost of all actions on period K
subto cost_def:
  forall <k> in Pers do
    sum <i, l> in Acs*Res do
	  x[i, k]*c[i, l] == cost[k];

###############
# Constraints #
###############

# Anual Goal
subto anual_goal:
  forall <j> in Yrs do
    sum <i, k> in Acs*YPers[j] do
	  rec[i, k] <= g[j];

# Global Budgets
#subto global_budget:
#  forall <l> in Res do
#    sum <i, k> in Acs*Pers do
#	  x[i, k]*c[i, l] <= o[l];

# Anual Budgets
subto anual_budget:
  forall <j, l> in Yrs*Res do
    sum <i, k> in Acs*YPers[j] do
	  x[i, k]*c[i, l] <= p[l, j];

# Periodal Budgets
#subto periodal_budget:
#  forall <k, l> in Pers*Res do
#    sum <i> in Acs do
#	  x[i, k] <= s[l, k];

# Global Market
subto global_market:
  forall <i> in Acs do
    sum <k> in Pers do
	  x[i, k] <= m[i];

# Anual Market
subto anual_market:
  forall <i, j> in Acs*Yrs do
    sum <k> in YPers[j] do
	  x[i, k] <= u[i, j];

# Periodal Market
subto periodal_market:
  forall <i, k> in Acs*Pers do
	  x[i, k] <= z[i, k];

# Dependecy between actions
# subto dependency_cardinality:
#	forall <i,j,k> in I*J*J with (dep[j,k] >= 0) do
#		sum <l> in I with (i >= l) do
#			x[l,j] <= 
#		sum <l> in I with (i >= l) do
#		 	x[l,k]*dep[j,k];


######################
# Objective Function #
######################

maximize npv: 
	sum <k> in DPers do 
		prof[k]/(1+r)^k -
	sum <k> in Pers do 
		cost[k]/(1+r)^k;

