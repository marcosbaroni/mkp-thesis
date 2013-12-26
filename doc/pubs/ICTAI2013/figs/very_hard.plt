# Hard data
# Two ways of generating a 2D heat map from ascii data
#
set terminal epslatex standalone color
set output 'very_hard.tex'
set title "Random instances behavior varying the problem size with $\\alpha=0.0$"
unset key
set tic scale 0

# Color runs from white to green
set palette rgbformula 3,3,3
#set cbrange [0:10]
set cblabel "Number of aborted instances"
set cbtics scale 0

set xrange [4.5:15.5]
set yrange [4.5:15.5]

set xtics 5,1,15
set ytics 5,1,15

set xlabel "Number of years"
set ylabel "Number of actions"

set view map
splot '-' using 1:2:3 with image
5 5 0
5 6 0
5 7 0
5 8 0
5 9 1
5 10 1
5 11 1
5 12 1
5 13 1
5 14 3
5 15 1

6 5 2
6 6 0
6 7 2
6 8 2
6 9 4
6 10 4
6 11 1
6 12 5
6 13 4
6 14 4
6 15 6

7 5 7
7 6 5
7 7 9
7 8 3
7 9 8
7 10 8
7 11 8
7 12 10
7 13 10
7 14 10
7 15 7

8 5 9
8 6 8
8 7 10
8 8 9
8 9 10
8 10 8
8 11 10
8 12 10
8 13 10
8 14 9
8 15 10

9 5 10
9 6 10
9 7 10
9 8 10
9 9 10
9 10 10
9 11 10
9 12 10
9 13 10
9 14 10
9 15 10

10 5 10
10 6 10
10 7 10
10 8 10
10 9 10
10 10 10
10 11 10
10 12 10
10 13 10
10 14 10
10 15 10

11 5 10
11 6 10
11 7 10
11 8 10
11 9 10
11 10 10
11 11 10
11 12 10
11 13 10
11 14 10
11 15 10

12 5 10
12 6 10
12 7 10
12 8 10
12 9 10
12 10 10
12 11 10
12 12 10
12 13 10
12 14 10
12 15 10

13 5 10
13 6 10
13 7 10
13 8 10
13 9 10
13 10 10
13 11 10
13 12 10
13 13 10
13 14 10
13 15 10

14 5 10
14 6 10
14 7 10
14 8 10
14 9 10
14 10 10
14 11 10
14 12 10
14 13 10
14 14 10
14 15 10

15 5 10
15 6 10
15 7 10
15 8 10
15 9 10
15 10 10
15 11 10
15 12 10
15 13 10
15 14 10
15 15 10
e

