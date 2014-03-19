# -*- coding: utf-8 -*-
import random as r
from sys import stdout, stdin, argv

''' Pops value(s) from a list of string, parsing it to the given type. '''
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

''' Retrieve input the value types and the fundamental formater string. '''
def gettypes(val):
	c1 = val.__class__.__name__
	c2 = None
	if c1 == 'list':
		c2 = val[0].__class__.__name__
		if c2 == 'list':                       # matrix case
			cf = val[0][0].__class__.__name__
		else:                                  # array case
			cf = val[0].__class__.__name__
	else:                                      # number case
		c2 = None
		cf = val.__class__.__name__
	if cf == 'float': form = '%.3f'
	else: form = '%d'
	return (c1, c2, form)

''' Format value(s) to CPLEX format (this function is inteligent). '''
def cplexformat(val):
	# Getting types
	s = ''
	c1, c2, form = gettypes(val)
	if c2 == 'list':                    # matrix case
		s += "[\n"
		for vs in val:
			s += "  [" + ' '.join([form % v for v in vs]) + "]\n"
		s += "]"
	elif c1 == 'list':                  # array case
		if c2 == 'tuple':              # array of tuples
			s += "{" + ','.join(['<' +  ','.join([str(x) for x in list(tu)]) + '>' for tu in val]) + "}"
		else:                           # array of numbers
			s += "[" + ' '.join([form % v for v in val]) + "]"
	else:                               # number case
		s += form % val
	s += ";"
	return s
		

class PCOPE:
	''' Instance contructor. '''
	def __init__(self, fin):
		fromPlain(fin)       # reads from plain text

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
		s += "G. Market: " + str(self.gmarket) + "\n"
		s += "Yearly. Goal: " + str(self.ygoal) + "\n"
		s += "Yearly. Budget: " + str(self.ybudget) + "\n"
		s += "Yearly Market: " + str(self.ymarket) + "\n"
		s += "Costs: " + str(self.acost) + "\n"
		s += "Energy Value: " + str(self.evalue) + "\n"
		s += "Recov. : " + str(self.recover) + "\n"
		#s += str(self.ndeps) + "\n"
		s += "Deps:" + str(self.deps) + "\n"
		return s
	
	def toCPLEX(self):
		s = "N = %s\n" % cplexformat(self.na)
		s += "Y = %s\n" % cplexformat(self.ny)
		s += "R = %s\n" % cplexformat(self.nr)
		s += "r = %s\n" % cplexformat(self.r)
		s += "g = %s\n" % cplexformat(self.ygoal)
		s += "p = %s\n" % cplexformat(self.ybudget)
		s += "m = %s\n" % cplexformat(self.gmarket)
		s += "u = %s\n" % cplexformat(self.ymarket)
		s += "c = %s\n" % cplexformat(self.acost)
		s += "v = %s\n" % cplexformat(self.evalue)
		s += "e = %s\n" % cplexformat(self.recover)
		s += "D = %s\n" % cplexformat(self.deps)
		return s

if __name__ == '__main__':
	p = PCOPE()
	p.fromPlain(argv[1])
	print p.toCPLEX()

