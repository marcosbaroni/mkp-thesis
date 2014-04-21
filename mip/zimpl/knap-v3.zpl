# Autor: Marcos Daniel Baroni
# Created: Mon Jan 21 10:39:57 BRST 2014
# Updated: Mon Jan 27 16:09:43 BRST 2014
#
#####################
# Decision Variable #
#####################

# Number of actions by given year
var x[Acs*Yrs] integer;


####################
# Others variables #
####################

# (Rec) rec[i, j]: Energy recovered by actions 'i' on the jth-year of the plan
var rec[Acs*Yrs];

# (Rec') rec2[i, j]: Energy recovered by actions 'i' on the jth-year AFTER the plan
var rec2[Acs*Yrs];

# Cost from all resources for a given action executed on a given year
var cost[Acs*Yrs];


#############
# Equations #
#############
# Total energy recovered on Year "j" by action "i"
subto rec_def:
  forall <i, j> in Acs*Yrs do
  	sum <j2> in Yrs with j2 <= j do
		x[i, j2]*e[i, (j-j2+1)] == rec[i, j];

# Total energy recovered on the "j"-th year AFTER plan, by action "i"
subto rec_def2:
  forall <i, j> in Acs*Yrs do
  	sum <j2> in Yrs with j2 >= j+1 do
		x[i, j2]*e[i, (Y+j-j2+1)] == rec2[i, j];

# Cost of actions "i" on k-th year
subto cost_def:
  forall <i, j> in Acs*Yrs do
    sum <l> in Res do
	  x[i, j]*c[i, l] == cost[i, j];

###############
# Constraints #
###############

# Yearly Goal Constraint
subto yearly_goal:
  forall <j> in Yrs do
    sum <i> in Acs do
	  rec[i, j] <= g[j];

# Global Budgets Constraint
#subto global_budget:
#  forall <l> in Res do
#    sum <i, k> in Acs*Pers do
#	  x[i, k]*c[i, l] <= o[l];

# Yearly Budgets
subto yearly_budget:
  forall <j, l> in Yrs*Res do
    sum <i> in Acs do
	  x[i, j]*c[i, l] <= p[j, l];

# PERIODIC Budgets
#subto PERIODIC_budget:
#  forall <k, l> in Pers*Res do
#    sum <i> in Acs do
#	  x[i, k] <= s[l, k];

# Global Market
subto global_market:
  forall <i> in Acs do
    sum <j> in Yrs do
	  x[i, j] <= m[i];

# Yearly Market
subto yearly_market:
  forall <i, j> in Acs*Yrs do
	  x[i, j] <= u[i, j];

# PERIODIC Market
#subto periodic_market:
#  forall <i, k> in Acs*Pers do
#	  x[i, k] <= z[i, k];

# Dependecy between actions
subto dependency:
	forall <i1, i2, q> in D do
		forall <j> in Yrs do
			sum <j2> in Yrs with (j2 < j) do
				x[i1, j2] <= 
			sum <j3> in Yrs with (j3 < j) do
		 		q*x[i2, j3];

# Min yealy energy recovery
#subto min_recovery:
#	forall <j> in Yrs do
#		sum <i> in Acs do
#			rec[j,i] >= 0.8*g[j];

######################
# Objective Function #
######################
maximize npv:
	sum <i> in Acs do
		sum <j> in Yrs do
			(rec[i, j]*v[i] - cost[i, j])/((1+r)^j)
	+
	sum <i> in Acs do
		sum <j> in Yrs do
			rec2[i, j]*v[i]/((1+r)^(Y+j));

