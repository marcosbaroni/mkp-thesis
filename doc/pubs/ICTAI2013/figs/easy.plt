# Hard data
# Two ways of generating a 2D heat map from ascii data
#
set terminal epslatex standalone color
set output 'easy.tex'
set title "Random instances behavior varying the problem size with $\\alpha=1.0$"
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
5 9 0
5 10 0
5 11 0
5 12 0
5 13 0
5 14 0
5 15 0

6 5 0
6 6 0
6 7 0
6 8 0
6 9 0
6 10 0
6 11 0
6 12 0
6 13 0
6 14 0
6 15 0

7 5 1
7 6 0
7 7 0
7 8 0
7 9 0
7 10 0
7 11 0
7 12 1
7 13 1
7 14 0
7 15 1

8 5 2
8 6 1
8 7 1
8 8 0
8 9 0
8 10 3
8 11 1
8 12 2
8 13 3
8 14 2
8 15 3

9 5 1
9 6 0
9 7 2
9 8 0
9 9 2
9 10 3
9 11 4
9 12 3
9 13 6
9 14 3
9 15 3

10 5 0
10 6 3
10 7 2
10 8 2
10 9 3
10 10 3
10 11 4
10 12 3
10 13 7
10 14 7
10 15 2

11 5 0
11 6 3
11 7 2
11 8 6
11 9 6
11 10 4
11 11 6
11 12 6
11 13 6
11 14 8
11 15 6

12 5 2
12 6 2
12 7 6
12 8 7
12 9 5
12 10 8
12 11 7
12 12 9
12 13 5
12 14 10
12 15 6

13 5 2
13 6 2
13 7 4
13 8 4
13 9 6
13 10 10
13 11 10
13 12 10
13 13 6
13 14 9
13 15 9

14 5 3
14 6 4
14 7 10
14 8 8
14 9 8
14 10 9
14 11 9
14 12 8
14 13 10
14 14 10
14 15 10

15 5 0
15 6 2
15 7 7
15 8 10
15 9 10
15 10 7
15 11 10
15 12 10
15 13 10
15 14 10
15 15 10
e

