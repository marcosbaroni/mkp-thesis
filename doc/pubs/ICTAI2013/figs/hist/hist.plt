reset
n=20 #number of intervals
max=1.00 #max value
min=0.98 #min value
width=(max-min)/n #interval width
#function used to map a value to the intervals
hist(x,width)=width*floor(x/width)+width/2.0

set terminal epslatex standalone color
#set terminal png
set xrange [min:max]
set yrange [0:500]

#set offset graph 0.05,0.05,0.05,0.00

set ylabel "Frequency"
set xtics min,(max-min)/5,max
set boxwidth width
set style fill solid 0.1 #fillstyle
set tics out nomirror

set output "lpgavsmip.tex"
#set output "lpgavsmip.png"
set xlabel "Distribution of the ratio (LPGA/MIP) considering all alphas"
plot "lpgavsmip.dat" u (hist($1,width)):(1.0) smooth freq w boxes lc rgb"gray" notitle

set output "lptsvsmip.tex"
#set output "lptsvsmip.png"
set xlabel "Distribution of the ratio (LPTS/MIP) considering all alphas"
plot "lptsvsmip.dat" u (hist($1,width)):(1.0) smooth freq w boxes lc rgb"gray" notitle
