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
<1> 99521582296.785,
<2> 99529825753.437,
<3> 99591315574.908,
<4> 99515377207.736;
# GLOBAL BUDGET (o)
param o[Res] :=
<1> 9999999999999;
# YEARLY BUDGETS (p)
param p[Yrs*Res] :=
  |	1|
| 1|9913444408.943|
| 2|9917493272.041|
| 3|9916958748.283|
| 4|9923143024.436|;
# PERIODAL BUDGETS (s)
param s[Pers*Res] :=
  |	1|
| 1|9913444408.943|
| 2|9917493272.041|
| 3|9916958748.283|
| 4|9923143024.436|;

###  ACTIONS ###
# GLOBAL MARKET (m)
param m[Acs] :=
<1> 992147483647,
<2> 992147483647,
<3> 992147483647;
# ANUAL MARKET (u)
param u[Acs*Yrs] :=
  |	1, 	2, 	3, 	4|
| 1|2147483647, 2147483647, 2147483647, 2147483647|
| 2|2147483647, 2147483647, 2147483647, 2147483647|
| 3|2147483647, 2147483647, 2147483647, 2147483647|;
# PERIODAL MARKET (z)
param z[Acs*Pers] :=
  |	1, 	2, 	3, 	4|
| 1|2147483647, 2147483647, 2147483647, 2147483647|
| 2|2147483647, 2147483647, 2147483647, 2147483647|
| 3|2147483647, 2147483647, 2147483647, 2147483647|;
# COST (c)
param c[Acs*Res] :=
   |  1|
| 1|1.0|
| 2|1.0|
| 3|1.0|;
# ENERGY VALUE (v)
param v[Acs] :=
<1> 2.200,
<2> 2.200,
<3> 2.200;
# ENERGY RECOVER (e)
param e[Acs*Pers] :=
   |	1, 	2, 	3, 	4|
| 1|49067.530, 24533.765, 14720.259, 0.000|
| 2|233.175, 333.107, 333.107, 333.107|
| 3|22127.293, 55318.233, 55318.233, 55318.233|;
# DEPEDENCY GROUPS
set D := {
<1,2,5>,
<2,3,12>
};

include "knap-v3.zpl";

