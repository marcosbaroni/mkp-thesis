#set logscale y;plot "< awk '{print $3}' real.csv|sort" title "TIR"
set logscale x
set logscale y
#set logscale z
set xrange [100:30000]
set xlabel "Invest"
set ylabel "UCs"
#set zlabel "TIR"
set grid xtics ytics ztics
plot 'real.csv' using 1:2 with points pointsize 3

pause -1

