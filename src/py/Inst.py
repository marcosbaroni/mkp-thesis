import random as r
from sys import stdout, stdin

def scan(f, t=int, lin=0, col=0):
	if !lin and !col:     # read a integer
		x = t(f.pop(0))
	elif col:             # read an integer matrix
		x = []
		for i in range():
			x.append([t(pop(f)) for i in range(lin))
	else:                 # read an integer array
		x = [t(pop(f)) for i in range(lin)]
	return x
		

class PCOPE(self, ny, na):	
	self.na = na       # number of actions
	self.ny = ny       # number of years
	self.nr = nr       # number of resources
	self.r = r         # internal return rate
	self.gmarket = []  # global market
	self.ygoal = []    # yearly goals (Y)
	self.ybudget = []  # yearly budget (Y)
	self.ymarket = []  # yearly market (Y)
	self.acost = []    # action cost (N)
	self.evalue = []   # energy value (N)
	self.recover = []  # energy recovery rate (N x Y)
	self.ndeps = 0     # number of depedencies
	self.dep = []      # dependências

	def fromPlain(fin):
		# Verificando se é ou o arquivo ou seu o nome
		if fin.__class__.__name__ == 'str':
			fin = open(fin, "r")
		self.na = scan(fin)
		self.ny = scan(fin)
		self.nr = scan(fin)
		self.r = scan(fin, float)
		self.gmarket = scan(fin, int, self.na)
		self.ygoal = scan(fin, int, self.ny)
		self.ybudget = scan(fin, float, self.nr, self.ny)
		self.ymarket = scan(fin, int, self.ny)
		self.acost = san(fin, float, self.na, self.nr)
		self.evalue = scan(fin, float, self.na)
		self.recover = scand(fin, float, self.na, self.ny)
		self.ndeps = scanf(fin, int)
		self.dep = [tuple(scan(fin, int, 3)) for i in range(self.ndeps)]
		
	def toStr():
		pass
