######################
# Problem Parameters #
######################
# N of ACTIONS (Y)
param N := 3;
# N of YEARS (Y)
param Y := 4;
# N of PERIODS per year (P)
param P := 1;
# N of RESOURCES (R)
param R := 1;
# Rate of Return (r)
param r := 0.15;

##############
# Index Sets #
##############
# Actions
set Acs := {1 .. N};
# Years
set Yrs := {1 .. Y};
# Periods
set Pers := {1 .. Y*P};
# SubYears Periods
set YPers[<j> in Yrs] := { (P*(j-1)+1) .. (P*j)};
# Resources
set Res := {1 .. R};
# Extra Set ("Doubled Pers"): All periods, 
set DPers := {1 .. 2*Y*P};

######################
# Indexed Parameters #
######################
###  ANUALS ###
# YEARLY GOALS (g)
param g[Yrs] :=
<1> 1.0e+99,
<2> 1.0e+99,
<3> 1.0e+99,
<4> 1.0e+99;
# GLOBAL BUDGET (o)
param o[Res] :=
<1> 4000;
# YEARLY BUDGETS (p)
param p[Yrs*Res] :=
  |	1|
| 1|1000|
| 2|1000|
| 3|1000|
| 4|1000|;
# PERIODIC BUDGETS (s)
param s[Pers*Res] :=
  |	1|
| 1|1000|
| 2|1000|
| 3|1000|
| 4|1000|;
###  ACTIONS ###
# GLOBAL MARKET (m)
param m[Acs] :=
<1> 1000,
<2> 1000,
<3> 1000;
# ANUAL MARKET (z)
param u[Acs*Yrs] :=
  |	1, 	2, 	3, 	4|
| 1|250, 250, 250, 250|
| 2|250, 250, 250, 250|
| 3|250, 250, 250, 250|;
# PERIODIC MARKET (c)
param z[Acs*Pers] :=
  |	1, 	2, 	3, 	4|
| 1|250, 250, 250, 250|
| 2|250, 250, 250, 250|
| 3|250, 250, 250, 250|;
# COST (c)
param c[Acs*Res] :=
   |  1|
| 1|1.0|
| 2|1.0|
| 3|1.0|;
# ENERGY VALUE (v)
param v[Acs] :=
<1> 2,
<2> 2,
<3> 2;
# ENERGY RECOVER (e)
param e[Acs*Pers] :=
   | 1,	2, 3, 4|
| 1| 1, 0, 0, 0|
| 2| 1, 0, 0, 0|
| 3| 1, 0, 0, 0|;
# DEPEDENCY GROUPS
set D := {
};

include "knap-v3.zpl";

