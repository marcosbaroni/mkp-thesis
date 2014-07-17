#####################################################
# Converts a instances file on Chu-beasly
#  format into MMKP instance format files.
#  FIXME: reading any real number as rounded integer
#
# author: Marcos Daniel Baroni
# date: Thu Jul 17 14:43:08 BRT 2014
#####################################################
from sys import argv, stdin


def chub2mmkp(fin):
	xs = [int(round(float(x))) for x in fin.read().split())]

	np = xs.pop(0)   # number of problems
	for k in range(np):
		n  = xs.pop(0)     # n itens
		m  = xs.pop(0)     # n constraints
		fopt = xs.pop(0)   # best known opt
		p = [xs.pop(0) for i in range(n)]            # profits
		w = [[xs.pop(0) for i in range(n)] for j in range(m)] 
		b = [xs.pop(0) for j in range(m)]

		fout = 


def print_usage(argv):
	print("usage: " + argv[0] + "[file]\n\n\
		if no file is given, will read from standard input.\n")


if __name__ == "__main__":
	# assert input
	if (len(argv) > 1):
		if argv[1][0] == "-":
			print_usage()
			exit(1)
		else:
			fin = argv[1]
	else: fin = stdin
	
	chub2mmkp(fin)

