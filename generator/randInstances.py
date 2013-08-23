#from instances 
import random as r
import math
import sys

real_inst = "\n\
01000 450493 034\n\
01350 001000 046\n\
15500 000140 051\n\
15500 000297 077\n\
15500 000726 030\n\
00166 119140 114\n\
00218 037889 06\n\
00327 021865 016\n\
00350 001000 016\n\
00479 020000 026\n\
00560 019815 042\n\
06355 000657 041\n\
06355 000911 018\n\
00790 414697 012\n\
e e e\n\
\n\
"

def sign(x):
	if x > 0. : return  1
	if x < 0. : return -1
	return 0

#def over_line(g):
#	(ax, ay) = (math.log(150.0), math.log(120000.))
#	(bx, by) = (math.log(20000.), math.log(200.))
#	b = (bx*ay - ax*by)/(bx - ax)
#	a = (by-b)/bx
#	if g == 1:
#		x = (0.35 + 0.1*r.gauss(0.5, 0.3))*(-b/a)
#	elif g == 2:
#		x = (0.6 + 0.1*r.gauss(0.5, 0.3))*(-b/a)
#	y = a*x + b
#	y *= r.gauss(1., 0.1)
#	return (x, y)

def exp_gauss(c, b): # (center, bound)
	x = -1
	while x < 0. or x > b:
		aux = r.gauss(0.0, 10.0)
		x = c + b*sign(aux)*math.pow(abs(aux), 3.5)
	return x

def broad_gauss(med, var, wid):
	x = r.random()*wid + med
	return x

class Action:
	years = 0
	cost = 0.0
	uc = 0
	tir = 0.0
	curve = []
	def __init__(self, inst):
		self.inst = inst
		self.years = self.inst.years
		self.tir = 15 + exp_gauss(10, 120)
		#self.tir = 15 + r.random()*45 + math.exp(r.gauss(0,1.2))*45
		self.set_curve(r.randint(1,3))
		self.set_group(r.randint(1,4))
	
	def set_curve(self, c):
		years = self.inst.years
		if   c == 1:
			self.curve = [r.randint(4,7)/10.0] + [1.0]*(years-1)
		elif c == 2:
			self.curve = [0.5, 0.85, 0.7, 0.5, 0.25] + [0.0]*(years-5)
		elif c == 3:
			self.curve = [0.5, 0.25, 015] + [0.0]*(years-3)
	
	def to_gplot(self):
		s = str(self.cost) + " "
		s += str(self.uc) + " "
		s += str(self.tir)
		return s
	
	def set_group(self, g):
		self.group = g
		if g == 1:
			self.cost = exp_gauss(500, 16000)
			self.uc = max(100, r.gauss(50000, 10000)) #r.random()*50000
		elif g == 2:
			self.cost = max(100, r.gauss(500, 300)) #r.random()*500
			self.uc = exp_gauss(50000, 450000)
		elif g == 3:
			self.cost = exp_gauss(15000, 16000)
			self.uc = max(100, r.gauss(50000, 10000)) #r.random()*50000
		elif g == 4:
			self.cost = max(100, r.gauss(500, 300)) #r.random()*500
			self.uc = exp_gauss(400000, 450000)
		return

	def __str__(self):
		s = "Curve:\n" + str(self.curve)
		return s

class Instance:
	years = 0
	camp = 0
	alpha = 0.0
	budgets = []
	goals = []
	acts = []
	stddist = [.36, .36, .14, .14] # Distribution for respective action group
	dist = [] # number of actions from respective group
	def __init__(self, seed, alpha, years, camp):
		r.seed(seed)
		self.years = years
		self.camp = camp
		self.alpha = alpha
		self.set_budgets()		# yearly budgets
		self.set_goals()		# yearly goals
		for i in range(self.camp):
			self.acts.append(Action(self))
	
	def set_budgets(self):
		medianOrc = r.randint(700,750)
		varOrc = r.randint(10,15)
		for i in range(self.years):
			self.budgets.append(r.gauss(medianOrc, varOrc))

	def set_goals(self):
		for i in range(self.years):
			self.goals.append(0.0)

	def set_dist(self):
		na = self.camp-4
		int(round(gauss(na,1)))
	
	def gplot(self):
		s = ""
		#s += "set term dumb size 159, 48"
		#s += "set logscale x\n"
		#s += "set logscale y\n"
		#s += "set xrange [0:1.]\n"
		#s += "set yrange [0:1.]\n"
		#s += "set ytics 0.1\n"
		#s += "set xrange [100:30000]\n"
		#s += "set yrange [100:1000000]\n"
		s += "set xlabel \"Invest\"\n"
		s += "set ylabel \"UCs\"\n"
		s += "set zlabel \"TIR\"\n"
		s += "set grid xtics ytics ztics\n"
		s += "splot '-' using 1:2:3 with points pointsize 3"
		s += " ,'-' using 1:2:3 with points pointsize 3\n"
		for a in self.acts:
			s += a.to_gplot() + "\n"
		s += "e e e\n"
		s += real_inst
		s += "pause -1\n"
		return s


	def to_scip(self):
		print self.years
		print "budgets"
		for b in self.budgets:
			print b
		print "goals"
		for g in self.goals:
			print g
		print "acts"
		for a in self.acts:
			print a

inst = Instance(None, 0.0, 4, 15)
print inst.gplot()

