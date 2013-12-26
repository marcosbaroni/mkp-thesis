# Hard data
# Two ways of generating a 2D heat map from ascii data
#
set terminal epslatex standalone color
set output 'hard.tex'
set title "Random instances behavior varying the problem size with $\\alpha=0.1$"
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

7 5 0
7 6 0
7 7 0
7 8 0
7 9 0
7 10 0
7 11 0
7 12 0
7 13 1
7 14 1
7 15 0

8 5 0
8 6 0
8 7 1
8 8 0
8 9 0
8 10 1
8 11 1
8 12 2
8 13 2
8 14 2
8 15 0

9 5 0
9 6 2
9 7 1
9 8 2
9 9 3
9 10 5
9 11 5
9 12 3
9 13 6
9 14 1
9 15 8

10 5 1
10 6 3
10 7 2
10 8 7
10 9 2
10 10 2
10 11 6
10 12 5
10 13 3
10 14 8
10 15 6

11 5 0
11 6 2
11 7 4
11 8 5
11 9 5
11 10 5
11 11 8
11 12 9
11 13 9
11 14 4
11 15 6

12 5 3
12 6 4
12 7 6
12 8 6
12 9 6
12 10 10
12 11 7
12 12 8
12 13 8
12 14 8
12 15 6

13 5 0
13 6 4
13 7 5
13 8 5
13 9 9
13 10 8
13 11 9
13 12 10
13 13 7
13 14 10
13 15 10

14 5 2
14 6 5
14 7 9
14 8 7
14 9 10
14 10 9
14 11 10
14 12 9
14 13 9
14 14 7
14 15 10

15 5 3
15 6 3
15 7 5
15 8 10
15 9 8
15 10 9
15 11 9
15 12 10
15 13 10
15 14 10
15 15 10
e

