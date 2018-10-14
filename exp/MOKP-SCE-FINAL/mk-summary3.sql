SELECT classe,
 np,
 n,
 avg(nnodesb),
 avg(compb),
 avg(timeb),
 avg(maxndb),
 avg(hvb),
 avg(time0),
 avg(nsol0),
 avg(hv0),
 avg(comp0),
 avg(time2),
 avg(nsol2),
 avg(hv2),
 avg(comp2),
 avg(time3),
 avg(nsol3),
 avg(hv3),
 avg(comp3)
from dim3 group by classe,
 np,
 n order by classe asc,
 np asc,
 n asc;
