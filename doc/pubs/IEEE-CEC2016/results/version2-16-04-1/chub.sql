select n, m, a,
	round(avg(time),2) avgTime,
	round(variance(time),2) varTime,
	round(avg(timecore),2) avgTimeCr,
	round(variance(timecore),2) varTimeCr,
	round(avg(opt*100/(1.*best)),2) as avgOpt,
	round(variance(opt*100/(1.*best)),2) as varOpt,
	round(avg(optcore*100/(1.*best)),2) as avgOptCr,
	round(variance(optcore*100/(1.*best)),2) as varOptCr
from chub
group by n,m,a
order by n asc, m asc, a asc;
