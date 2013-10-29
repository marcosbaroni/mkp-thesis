import sys
import random as r

from Instance import *

def print_usage():
 	print "ARGS: <# years> <# actions> <dtir*> [seed]\n"
	print "  dtir : tir variation [0.0, 1.0]"
	#print "Will always write \"knap.dat\" and \"knap.gplot\"."

if __name__ == "__main__":
	if len(sys.argv) < 4:
		print_usage()
		exit(1)

	nyears = int(sys.argv[1])
	nacts = int(sys.argv[2])
	dtir = float(sys.argv[3])
	if len(sys.argv) > 4:
		seed = float(sys.argv[4])
		r.seed(seed)

	inst = Instance.random(nyears, nacts, dtir)
	print inst.to_scip()

