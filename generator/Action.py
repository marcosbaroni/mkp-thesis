import random as r
import json

from util import *

class Action:
	groups_distribution = [(1, 0.3), (2, 0.3), (3, 0.2), (4, 0.2)]
	""" Mock contructor for an Action. """
	def __init__(self):
		self.nyears = 0
		self.tir = 0.0
		self.curve = []
		self.group = 0
		self.energy = []
	
	""" Returns a random action. """
	@staticmethod
	def random(nyears, dtir):
		act = Action()
		act.nyears = nyears
		act.tir = 0.15 + 1.0*r.random()*dtir
		act.set_curve(r.randint(1,3))
		act.set_group(weighted_random(Action.groups_distribution))
		act.set_energy()
		return act
	
	""" Returns a list with energy reduction ratio for each year, based on the
		specified curve ('c' argument):

							CURVE TYPES
		   
		  1   _______________      2    __                 3  _ 
		    _/                        _/  \                    \_ 
		                                   \__________           \__________
	"""
	def set_curve(self, c):
		nyears = self.nyears
		if c == 1:
			self.curve = [r.randint(4,7)/10.0] + [1.0]*(nyears-1)
		elif c == 2:
			self.curve = [0.5, 0.85, 0.7, 0.5, 0.25] + [0.0]*(nyears-5)
			self.curve = self.curve[0:nyears]
		elif c == 3:
			self.curve = [0.5, 0.25, 0.15] + [0.0]*(nyears-3)
		# Cutting curve
		self.curve = self.curve[:nyears]
	
	""" Set the (UC, COST) group of Action, based on the group parameter given
		(see diagram below).

		    ^ 4
		    | |
		    | |
		    | v
		  U |            CLASSES
		  C | ^
		    | |
		    | 2
		    |  1-->        <--3
		    +----- COST --------->
	"""
	# argument: class of group (uc/cost)
	def set_group(self, g):
		self.group = g
		self.profit = 0.2

		MIN_UC, MAX_UC = (150, 450000)
		MIN_COST, MAX_COST = (100, 16000)

		#MIN_UC, MAX_UC = (150, 300)
		#MIN_COST, MAX_COST = (100, 160)

		MEAN_UC = (MAX_UC + MIN_UC)/2
		MEAN_COST = (MAX_COST + MAX_COST)/2

		DIF_UC = (MAX_UC - MIN_UC)
		DIF_COST = (MAX_COST - MIN_COST)

		# Classe "g" (see diagram above)
		if g == 1:   	# CENTER --> COST AXIS
			self.cost = exp_gauss(MIN_COST, MAX_COST)
			self.uc = int(max(MIN_UC, r.gauss(MIN_UC+ 0.01*DIF_UC, 0.1*DIF_UC)))
		elif g == 2: 	# CENTER --> UCs AXIS
			self.cost = max(MIN_COST, r.gauss(MIN_COST+0.01*DIF_COST, 0.05*DIF_COST))
			self.uc = int(exp_gauss(MIN_UC, MAX_UC))
		elif g == 3: 	# HIGH COST --> CENTER
			self.cost = exp_gauss(MAX_COST, MAX_COST)
			self.uc = int(max(MIN_UC, r.gauss(MIN_UC+ 0.01*DIF_UC, 0.05*DIF_UC)))
		elif g == 4: 	# HIGH UCs --> CENTER
			self.cost = max(MIN_COST, r.gauss(MIN_COST+0.01*DIF_COST, 0.02*DIF_COST))
			self.uc = int(exp_gauss(MAX_UC, MAX_UC))
		return

	def set_energy(self):
		s = 0.0
		for i in range(len(self.curve)):
			s += self.curve[i]/math.pow((1. + self.tir), i)
		k = (self.cost/s)/self.profit
		self.energy = []
		for i in range(len(self.curve)):
			self.energy.append(k*self.curve[i])

	def to_gplot(self):
		s = str(self.cost) + " "
		s += str(self.uc) + " "
		s += str(self.tir*100.)
		return s
	
	def from_dict(dict):
		pass
	
	def __str__(self):
		s = ""
		s += "Cost: " + "{:.3f}".format(self.cost) + "\n"
		s += "UC: " + str(self.uc) + "\n"
		s += "Curve: " + fs2str(self.curve, 2) + "\n"
		s += "Energy: " + fs2str(self.energy) + "\n"
		s += "Tir: " + "{:.3f}".format(self.tir) + "\n"
		return s

