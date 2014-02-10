######################
# Problem Parameters #
######################
# N of ACTIONS (Y)
param N := 40;
# N of YEARS (Y)
param Y := 8;
# N of PERIODS per year (P)
param P := 1;
# N of RESOURCES (R)
param R := 2;
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
<1> 162975799361.394,
<2> 163595253334.185,
<3> 166822631589.867,
<4> 173339664356.319,
<5> 166567827299.831,
<6> 183278429507.347,
<7> 160295971632.436,
<8> 154310418855.097;
# GLOBAL BUDGET (o)
param o[Res] :=
<1> 1.7976931348623157E308,
<2> 1.7976931348623157E308;
# YEARLY BUDGETS (p)
param p[Yrs*Res] :=
  |	1, 	2|
| 1|632960075.609, 1517637468.982|
| 2|924194370.971, 2286066954.865|
| 3|1571556569.447, 2521304427.376|
| 4|836748112.846, 935491963.077|
| 5|1735483242.491, 1475891361.060|
| 6|617281803.274, 2384698790.145|
| 7|1550709774.233, 2056530206.826|
| 8|1469857683.800, 2127984414.247|;
# PERIODIC BUDGETS (s)
param s[Pers*Res] :=
  |	1, 	2|
| 1|632960075.609, 1517637468.982|
| 2|924194370.971, 2286066954.865|
| 3|1571556569.447, 2521304427.376|
| 4|836748112.846, 935491963.077|
| 5|1735483242.491, 1475891361.060|
| 6|617281803.274, 2384698790.145|
| 7|1550709774.233, 2056530206.826|
| 8|1469857683.800, 2127984414.247|;

###  ACTIONS ###
# GLOBAL MARKET (m)
param m[Acs] :=
<1> 25234,
<2> 420704,
<3> 150,
<4> 20624,
<5> 434315,
<6> 103022,
<7> 150,
<8> 174248,
<9> 44838,
<10> 15460,
<11> 65781,
<12> 9421,
<13> 426152,
<14> 99258,
<15> 150,
<16> 25374,
<17> 21935,
<18> 150,
<19> 93443,
<20> 55992,
<21> 9197,
<22> 40163,
<23> 352921,
<24> 18232,
<25> 17812,
<26> 285218,
<27> 150,
<28> 150,
<29> 150,
<30> 3975,
<31> 79891,
<32> 13540,
<33> 394321,
<34> 259833,
<35> 236831,
<36> 150,
<37> 449998,
<38> 150,
<39> 30414,
<40> 150;
# ANUAL MARKET (u)
param u[Acs*Yrs] :=
  |	1, 	2, 	3, 	4, 	5, 	6, 	7, 	8|
| 1|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 2|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 3|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 4|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 5|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 6|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 7|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 8|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 9|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 10|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 11|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 12|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 13|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 14|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 15|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 16|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 17|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 18|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 19|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 20|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 21|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 22|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 23|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 24|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 25|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 26|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 27|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 28|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 29|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 30|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 31|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 32|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 33|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 34|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 35|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 36|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 37|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 38|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 39|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 40|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|;
# PERIODIC MARKET (z)
param z[Acs*Pers] :=
  |	1, 	2, 	3, 	4, 	5, 	6, 	7, 	8|
| 1|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 2|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 3|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 4|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 5|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 6|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 7|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 8|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 9|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 10|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 11|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 12|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 13|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 14|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 15|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 16|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 17|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 18|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 19|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 20|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 21|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 22|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 23|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 24|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 25|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 26|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 27|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 28|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 29|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 30|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 31|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 32|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 33|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 34|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 35|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 36|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 37|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 38|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 39|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|
| 40|2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647|;
# COST (c)
param c[Acs*Res] :=
  |	1, 	2|
| 1|15998.223, 15850.896|
| 2|448.740, 334.493|
| 3|9018.205, 103.369|
| 4|100.000, 2199.522|
| 5|1244.254, 562.860|
| 6|9182.497, 7816.178|
| 7|2188.283, 566.772|
| 8|100.000, 100.000|
| 9|578.482, 2665.225|
| 10|7354.743, 13612.716|
| 11|9105.167, 5311.861|
| 12|96.163, 1015.400|
| 13|100.000, 453.152|
| 14|14755.336, 8713.296|
| 15|4207.820, 9472.557|
| 16|15997.985, 14167.671|
| 17|100.000, 7054.451|
| 18|703.180, 100.001|
| 19|5982.131, 1447.900|
| 20|7620.617, 99.992|
| 21|13711.845, 15999.480|
| 22|100.000, 100.000|
| 23|467.817, 100.000|
| 24|99.715, 4231.634|
| 25|8578.611, 150.497|
| 26|8597.657, 3691.627|
| 27|4017.781, 115.408|
| 28|15999.997, 7845.912|
| 29|1809.111, 11236.468|
| 30|15821.006, 14073.178|
| 31|4883.149, 3667.484|
| 32|6735.893, 15999.637|
| 33|22999.298, 4544.760|
| 34|544.997, 614.806|
| 35|587.778, 100.000|
| 36|15490.060, 15933.386|
| 37|200.832, 100.000|
| 38|15988.095, 11122.315|
| 39|15508.921, 3564.628|
| 40|15134.387, 15989.821|;
# ENERGY VALUE (v)
param v[Acs] :=
<1> 0.200,
<2> 0.200,
<3> 0.200,
<4> 0.200,
<5> 0.200,
<6> 0.200,
<7> 0.200,
<8> 0.200,
<9> 0.200,
<10> 0.200,
<11> 0.200,
<12> 0.200,
<13> 0.200,
<14> 0.200,
<15> 0.200,
<16> 0.200,
<17> 0.200,
<18> 0.200,
<19> 0.200,
<20> 0.200,
<21> 0.200,
<22> 0.200,
<23> 0.200,
<24> 0.200,
<25> 0.200,
<26> 0.200,
<27> 0.200,
<28> 0.200,
<29> 0.200,
<30> 0.200,
<31> 0.200,
<32> 0.200,
<33> 0.200,
<34> 0.200,
<35> 0.200,
<36> 0.200,
<37> 0.200,
<38> 0.200,
<39> 0.200,
<40> 0.200;
# ENERGY RECOVER (e)
param e[Acs*Pers] :=
  |	1, 	2, 	3, 	4, 	5, 	6, 	7, 	8|
| 1|133203.029, 66601.514, 39960.909, 0.000, 0.000, 0.000, 0.000, 0.000|
| 2|1142.617, 5713.086, 5713.086, 5713.086, 5713.086, 5713.086, 5713.086, 5713.086|
| 3|20602.701, 51506.753, 51506.753, 51506.753, 51506.753, 51506.753, 51506.753, 51506.753|
| 4|6585.160, 11194.773, 9219.225, 6585.160, 3292.580, 0.000, 0.000, 0.000|
| 5|2636.300, 13181.502, 13181.502, 13181.502, 13181.502, 13181.502, 13181.502, 13181.502|
| 6|14517.031, 145170.315, 145170.315, 145170.315, 145170.315, 145170.315, 145170.315, 145170.315|
| 7|7614.339, 12690.566, 12690.566, 12690.566, 12690.566, 12690.566, 12690.566, 12690.566|
| 8|170.802, 1708.019, 1708.019, 1708.019, 1708.019, 1708.019, 1708.019, 1708.019|
| 9|9289.033, 15791.355, 13004.646, 9289.033, 4644.516, 0.000, 0.000, 0.000|
| 10|87692.504, 43846.252, 26307.751, 0.000, 0.000, 0.000, 0.000, 0.000|
| 11|41286.161, 70186.474, 57800.626, 41286.161, 20643.081, 0.000, 0.000, 0.000|
| 12|4648.904, 2324.452, 1394.671, 0.000, 0.000, 0.000, 0.000, 0.000|
| 13|1584.067, 2692.914, 2217.694, 1584.067, 792.034, 0.000, 0.000, 0.000|
| 14|69288.950, 98984.214, 98984.214, 98984.214, 98984.214, 98984.214, 98984.214, 98984.214|
| 15|40390.037, 57700.053, 57700.053, 57700.053, 57700.053, 57700.053, 57700.053, 57700.053|
| 16|86385.631, 146855.573, 120939.884, 86385.631, 43192.816, 0.000, 0.000, 0.000|
| 17|29922.162, 14961.081, 8976.648, 0.000, 0.000, 0.000, 0.000, 0.000|
| 18|2300.076, 3910.129, 3220.106, 2300.076, 1150.038, 0.000, 0.000, 0.000|
| 19|31074.725, 15537.363, 9322.418, 0.000, 0.000, 0.000, 0.000, 0.000|
| 20|22109.571, 37586.271, 30953.400, 22109.571, 11054.786, 0.000, 0.000, 0.000|
| 21|82115.293, 136858.822, 136858.822, 136858.822, 136858.822, 136858.822, 136858.822, 136858.822|
| 22|836.463, 418.231, 250.939, 0.000, 0.000, 0.000, 0.000, 0.000|
| 23|1282.515, 3206.287, 3206.287, 3206.287, 3206.287, 3206.287, 3206.287, 3206.287|
| 24|12403.722, 21086.327, 17365.210, 12403.722, 6201.861, 0.000, 0.000, 0.000|
| 25|24997.616, 42495.947, 34996.662, 24997.616, 12498.808, 0.000, 0.000, 0.000|
| 26|27757.544, 69393.860, 69393.860, 69393.860, 69393.860, 69393.860, 69393.860, 69393.860|
| 27|9335.546, 23338.864, 23338.864, 23338.864, 23338.864, 23338.864, 23338.864, 23338.864|
| 28|99731.091, 49865.546, 29919.327, 0.000, 0.000, 0.000, 0.000, 0.000|
| 29|37358.728, 63509.838, 52302.219, 37358.728, 18679.364, 0.000, 0.000, 0.000|
| 30|125026.874, 62513.437, 37508.062, 0.000, 0.000, 0.000, 0.000, 0.000|
| 31|35761.434, 17880.717, 10728.430, 0.000, 0.000, 0.000, 0.000, 0.000|
| 32|65107.919, 110683.462, 91151.087, 65107.919, 32553.960, 0.000, 0.000, 0.000|
| 33|23522.890, 235228.901, 235228.901, 235228.901, 235228.901, 235228.901, 235228.901, 235228.901|
| 34|3205.429, 5342.381, 5342.381, 5342.381, 5342.381, 5342.381, 5342.381, 5342.381|
| 35|2876.502, 1438.251, 862.951, 0.000, 0.000, 0.000, 0.000, 0.000|
| 36|131422.731, 65711.365, 39426.819, 0.000, 0.000, 0.000, 0.000, 0.000|
| 37|1258.174, 629.087, 377.452, 0.000, 0.000, 0.000, 0.000, 0.000|
| 38|61233.701, 153084.253, 153084.253, 153084.253, 153084.253, 153084.253, 153084.253, 153084.253|
| 39|79771.580, 39885.790, 23931.474, 0.000, 0.000, 0.000, 0.000, 0.000|
| 40|89130.646, 151522.098, 124782.904, 89130.646, 44565.323, 0.000, 0.000, 0.000|;
# DEPEDENCY GROUPS
set D := {
};

include "knap-v3.zpl";