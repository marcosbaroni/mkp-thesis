#set logscale x
#set logscale y
#set logscale z
#set xlabel "Invest"
#set ylabel "UCs"
set xlabel "TIR"
set grid xtics ytics ztics
plot 'real.csv' using 3 with points pointsize 3

pause -1

