#!/bin/bash


alphas="0.00
0.05
0.10
0.15
0.20
0.25
0.30
0.35
0.40
0.45
0.50
0.55
0.60
0.65
0.70
0.75
0.80
0.85
0.90
0.95
1.00
"

for years in `seq -w 7 7`
do
    for acts in `seq -w 7 7`
    do
	for alpha in $alphas
	do
	        for seed in `seq 42 42`
	        do
	            python randInstances.py $seed 0.0 $years $acts > alpha/$years-$acts-$alpha-$seed.dat
	        done
	done
    done
done

echo DONE HARD!


# for years in `seq -w 5 15`
# do
#     for acts in `seq -w 5 15`
#     do
#         for alpha in $alphas
#         do
#             for seed in `seq 1 10`
#             do
#                 python randInstances.py $seed $alpha $years $acts > data_easy/$years-$acts-$alpha-$seed.dat
#             done
#         done
#     done
# done



