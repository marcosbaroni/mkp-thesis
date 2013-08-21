#set logscale y;plot "< awk '{print $3}' real.csv|sort" title "TIR"
#set logscale x
#set logscale y
#set logscale z
set xlabel "Invest"
set ylabel "UCs"
set zlabel "TIR"
set grid xtics ytics ztics
splot 'real.csv' using 1:2:3 with points pointsize 3

pause -1

