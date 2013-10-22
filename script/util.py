import random
import math

def mean(l):
	return sum(l)/float(len(l))

def weighted_random(ls):
	n = len(ls)
	tot_p = 0
	for (x, p) in ls:
		tot_p += p
	q = random.random()*tot_p
	for (x, p) in ls:
		if q < p: return x
		q -= p
	return ls[-1][0]
	

def addVirg(a, b):
	return a + ", " + b

def fs2str(ls, ds=3):
	return "[" + reduce(addVirg, map(("{:." + str(ds) + "f}").format, ls)) + "]"

def sign(x):
	if x > 0. : return  1
	if x < 0. : return -1
	return 0

def exp_gauss(c, b): # (center, bound)
	x = -1
	while x < 0. or x > b:
		aux = random.gauss(0.0, 10.0)
		x = c + b*sign(aux)*math.pow(abs(aux), 3.5)
	return x

