#from instances 
import random as r
import math
import sys

class Action:
	years = 0
	cost = 0.0
	uc = 0
	tir = 0.0
	curve = []
	def __init__(self, inst):
		self.inst = inst
		self.years = years
		self.tir = 15 + math.exp(r.gauss(0,1.2))*45
		#self.tir = 15 + r.random()*45 + math.exp(r.gauss(0,1.2))*45
		if r.randint(1,2) == 1:
			# Alto Custo
			self.uc = r.random()
		else:
			# Alto UC
		self.set_curve(r.randint(1,3))
	
	def set_curve(self, c):
		years = self.inst.years
		if   c == 1:
			self.curve = [r.randint(4,7)/10.0] + [1.0]*(years-1)
		elif c == 2:
			self.curve = [0.5, 0.85, 0.7, 0.5, 0.25] + [0.0]*(years-5)
		elif c == 3:
			self.curve = [0.5, 0.25, 015] + [0.0]*(years-3)
	
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
	def __init__(self, seed, alpha, years, camp):
		r.seed(seed)
		self.years = years
		self.camp = camp
		self.alpha = alpha
		self.set_budgets()
		self.set_goals()
		self.set_acts()
	
	def set_budgets(self):
		medianOrc = r.randint(700,750)
		varOrc = r.randint(10,15)
		for i in range(self.years):
			self.budgets.append(r.gauss(medianOrc, varOrc))

	def set_goals(self):
		for i in range(self.years):
			self.goals.append(0.0)

	def set_acts(self):
		for i in range(self.camp):
			self.acts.append(Action(self.years, self.alpha, self))

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

inst = Instance(1, 0.0, 4, 5)
inst.to_scip()

