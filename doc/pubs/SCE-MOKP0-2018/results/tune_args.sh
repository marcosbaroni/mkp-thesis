#!/bin/bash
# usage: ./../../../../bin/mokp sce <input file> <ndim> [seed=0] [niter=300] [ncomp=20] [compsize=20] [nsubcomp=5] [nsubiter=20] [cross_ratio]
# output: <time (s)>;<n sols>;<hypervolume>;<n comparisons>
idx="\"len\""
if [ "$#" -gt 0 ]; then
	idx=$1
fi

python <<!
from sys import argv
#niter = 300;                                                                
niters = [200, 300, 400];

#ncomp = 20;                                                                 
ncomps = [10, 20, 30]

#compsize = 20;                                                              
compsizes = [10, 20, 30]

#nsubcomp = 5;                                                               
nsubcomps = [3, 5]

#nsubiter = 20;                                                              
nsubiters = [10, 20, 30]

#cross_ratio = 0.2;   
cross_ratios = [0.05, 0.1, 0.2, 0.3]

iss = range(1, 11)

fnames = [\
	"A-700-2-1.txt",\
	"B-4000-2-1.txt",\
	"C-500-2-1.txt",\
	"D-200-2-1.txt"\
]

# [seed=0] [niter=300] [ncomp=20] [compsize=20] [nsubcomp=5] [nsubiter=20] [cross_ratio]
def build_args():
	args = [(fn, i, ni, nc, cs, nsc, nsi, cr) \
		for cr in cross_ratios \
		for nsi in nsubiters \
		for cs in compsizes \
		for nc in ncomps \
		for nsc in nsubcomps \
		for ni in niters \
		for i in iss \
		for fn in fnames \
	]
	return args

if __name__ == '__main__':
	instdir="/home/mbaroni/Computacao/phd/dmkp/doc/pubs/SCE-MOKP0-2018/results/insts/bazgan/"
	args = build_args()
	if( $idx == 'len'):
		print(len(args))
	else:
		idx=(int($idx)-1)
		print(instdir + "%s 0 %s %s %s %s %s %s %s" % (args[idx]))
!
