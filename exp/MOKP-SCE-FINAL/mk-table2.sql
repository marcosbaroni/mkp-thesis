-- CREATE TABLE summary2 (
--  classe string, 
--  np int, 
--  n int, 
--  nnodesb float, 
--  compb float, 
--  timeb float, 
--  maxndb float, 
--  hvb float, 
--  time0 float, 
--  nsol0 float, 
--  hv0 float, 
--  comp0 float, 
--  time2 float, 
--  nsol2 float, 
--  hv2 float, 
--  comp2 float);

SELECT
  classe,
  np,
  n,
  nnodesb,
  printf("%.1f", timeb) timeb,
  nsol0 nsol_sce,
  printf("%.1f", time0) time_sce_list,
  printf("%.1f", time2) time_sce_2dt,
  printf("%.1f", time2/time0) speed_up_2dt,
  printf("%.1f", comp2/comp0) comp_ratio_2dt,
  printf("%.1f", 100*hv0/hvb) quality
from summary2;

