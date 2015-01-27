#!/usr/bin/python
from sys import argv

if len(argv) < 2:
	exit(1);

ngems = 7
nmtls = 7

def cmpDmg(base, gem, mtl, qual):
	dmg = base*(1+0.1*gem)*(1+0.1*mtl)
	dif = max(1, dmg*0.1)
	return dmg + (qual-2)*dif

def cmpDmg2(base, gem, mtl, qual):
	dif = max(1, base*0.1)
	dmg = base + (qual-2)*dif
	dmg += gem*0.1*base
	dmg += mtl*0.1*base
	return dmg

b = int(argv[1])
for g in range(ngems+1):
	for m in range(nmtls):
		for q in range(1, 4):
			print "%d;%d;%d;%.2f" % (g, m, q, cmpDmg(b, g, m, q))

