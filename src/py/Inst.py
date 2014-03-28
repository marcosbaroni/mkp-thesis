# -*- coding: utf-8 -*-
import random as r
from sys import stdout, stdin, argv

class Scanner:
	''' Cria um Scanner a partir de um arquivo. '''
	def __init__(self, fin):
		self.vals = []
		if fin.__class__.__name__ == 'str':
			fin = open(fin, "r")
		for l in map(str.strip, fin.readlines()):
			if l:
				if l[0] != '#':
					self.vals += l.split()
		print self.vals
	

	''' Pops value(s) from a list of string, parsing it to the given type. '''
	def scan(self, t=int, lin=0, col=0):
		if not lin and not col:     # reads a single value
			x = t(self.vals.pop(0))
		elif col:                   # reads a matrix
			x = []
			for i in range(lin):
				x.append([t(self.vals.pop(0)) for i in range(col)])
		else:                       # reads an array
			x = [t(self.vals.pop(0)) for i in range(lin)]
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
	''' Cosntrutor: reads the Instance from a plain text format. '''
	def __init__(self, fin):
		sc = Scanner(fin)

		# Realiza leituras
		self.na = sc.scan()          # n ações
		self.ny = sc.scan()          # n anos
		self.nr = sc.scan()          # n recursos
		self.r = sc.scan(float)    # juros
		self.ygoal = sc.scan(float, self.ny)             # meta anual
		self.ybudget = sc.scan(float, self.nr, self.ny)  # orçamento anual
		self.gmarket = sc.scan(int, self.na)             # market global
		self.ymarket = sc.scan(int, self.na, self.ny)    # market anual
		self.acost = sc.scan(float, self.na, self.nr)    # custo da ação
		self.evalue = sc.scan(float, self.na)            # valor da energia
		self.recover = sc.scan(float, self.na, self.ny)  # recuperação
		self.ndeps = sc.scan(int)                        # n dependendias
		self.deps = [tuple(sc.scan(int, 3)) \
			for i in range(self.ndeps)]
		
	''' Returns a readable format of the instance. '''
	def toStr(self):
		s = "N. Actions: " + str(self.na) + "\n"
		s += "N. Years: " + str(self.ny) + "\n"
		s += "N. Resources: " + str(self.nr) + "\n"
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
	p = PCOPE(argv[1])
	print p.toStr()
	#print p.toCPLEX()

