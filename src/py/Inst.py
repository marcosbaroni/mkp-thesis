# -*- coding: utf-8 -*-
import random as r
from sys import stdout, stdin, argv

''' Pops value(s) from a list of string, parsing it. '''
def scan(f, t=int, lin=0, col=0):
	if not lin and not col:     # read a integer
		x = t(f.pop(0))
	elif col:             # read an integer matrix
		x = []
		for i in range(lin):
			x.append([t(f.pop(0)) for i in range(col)])
	else:                 # read an integer array
		x = [t(f.pop(0)) for i in range(lin)]
	return x

''' Format value(s) (this function is inteligent). '''
def fval(mat):
	pass

class PCOPE:	
	def __init__(self):
		self.na = 0        # number of actions
		self.ny = 0        # number of years
		self.nr = 0        # number of resources
		self.r = 0         # internal return rate
		self.gmarket = []  # global market
		self.ygoal = []    # yearly goals (Y)
		self.ybudget = []  # yearly budget (Y)
		self.ymarket = []  # yearly market (Y)
		self.acost = []    # action cost (N)
		self.evalue = []   # energy value (N)
		self.recover = []  # energy recovery rate (N x Y)
		self.ndeps = 0     # number of depedencies
		self.deps = []      # dependencias

	''' Reads the Instance from a plain text format. '''
	def fromPlain(self, fin):
		# Verificando se e ou o arquivo ou seu o nome
		if fin.__class__.__name__ == 'str':
			fin = open(fin, "r")
		rd = fin.read().split()
		self.na = scan(rd)
		self.ny = scan(rd)
		self.nr = scan(rd)
		self.r = scan(rd, float)
		self.gmarket = scan(rd, int, self.na)
		self.ygoal = scan(rd, float, self.ny)
		self.ybudget = scan(rd, float, self.nr, self.ny)
		self.ymarket = scan(rd, int, self.na, self.ny)
		self.acost = scan(rd, float, self.na, self.nr)
		self.evalue = scan(rd, float, self.na)
		self.recover = scan(rd, float, self.na, self.ny)
		self.ndeps = scan(rd, int)
		self.deps = [tuple(scan(rd, int, 3)) for i in range(self.ndeps)]
		
	''' Returns a readable format of the instance. '''
	def toStr(self):
		s = "# Actions: " + str(self.na) + "\n"
		s += "# Years: " + str(self.ny) + "\n"
		s += "# Resources: " + str(self.nr) + "\n"
		s += "irr: " + str(self.r) + "\n"
		s += str(self.gmarket) + "\n"
		s += str(self.ygoal) + "\n"
		s += str(self.ymarket) + "\n"
		s += str(self.acost) + "\n"
		s += str(self.evalue) + "\n"
		s += str(self.recover) + "\n"
		s += str(self.ndeps) + "\n"
		s += str(self.deps) + "\n"
		return s

if __name__ == '__main__':
	p = PCOPE()
	p.fromPlain(argv[1])
	print p.toStr()

