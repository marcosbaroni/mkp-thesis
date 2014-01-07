import random as r
import json

from util import *
from Action import *

#             ACTION CLASSES
# ^
# |
# |  4 4
# | 4    4
# |   4 
# |
# | 4 
# |
# |      4
# |
# |
# |
#    4
# U
# C
#      2
# |
# |
# |
# |
# |  2 
# |
# |
# |  2 
# |     2
# | 1 2  1       1
# | 21  2   1          1         1                  3      3  3
# | 1 1 21    1                              3            3  3 3
# +---------- COST ------------------------------------------------>

class Instance:
	# Distribution for respective action group
	stddist = [.36, .36, .14, .14]

	""" Mock constructor. """
	def __init__(self):
		self.nyears = 0
		self.nacts = 0
		self.dtir = 0.0
		self.acts = []
		self.goals = []
		self.budgets = []

	""" Random constructor. """
	@staticmethod
	def random(nyears, nacts, dtir):
		inst =  Instance()
		inst.nyears = nyears 	# number of years
		inst.nacts = nacts 		# number of actions
		inst.dtir = dtir		# delta tir (tir variation)
		inst.acts = []			# set of actions
		for i in range(inst.nacts):		# Actions
			inst.acts.append(Action.random(nyears, dtir))
		inst.budgets = inst.rand_budgets()	# Yearly budgets
		inst.goals = inst.rand_goals()		# Yearly goals
		return inst

	""" Returns a random list of budgets for the Instance,
	based on total cost of the actions. """
	def rand_budgets(self):
		tot_cost = 0
		bs = []
		for a in self.acts:
			tot_cost += a.uc*a.cost
		med = tot_cost/self.nyears
		for i in range(self.nyears):
			bs.append(0.2*med + 0.4*med*r.random())
		return bs

	""" Returns a random list of energy reduction goal for the Instance,
	based on total energy reduction of the actions. """
	def rand_goals(self):
		total = 0
		gs = []
		for a in self.acts:
			total += a.uc*sum(a.energy)
		med = total/self.nyears
		for i in range(self.nyears):
			gs.append(0.9*med + 0.2*med*r.random())
		return gs

	""" Returns a string version of the Instance on SCIP format. """
	def to_scip(self):
		s = ""
		s += "# N of YEARS"				# Recursos
		s += "\n" +  str(self.nyears)
		s += "\n# N of ACTIONS"				# Recursos
		s += "\n" +  str(self.nacts)
		s += "\n# N of RESOURCES"				# Recursos
		s += "\n1"				# Recursos
		s += "\n# RATE"			# 
		s += "\n0.15"
		s += "\n\n# YEARLY GOALS"
		for g in self.goals:	# Metas
			s += "\n" +  str(g)
		s += "\n\n# YEARLY BUDGETS"
		for b in self.budgets:  # Orcamentos
			s += "\n" +  "{:.2f}".format(b)
		s += "\n" +  "\n# MARKETS"
		for a in self.acts:		# Market
			s += "\n" +  str(a.uc)
		s += "\n" +  "\n# COSTS"
		for a in self.acts:		# Custo
			s += "\n" +  "{:.2f}".format(a.cost)
		s += "\n" +  "\n# PROFITS"
		for a in self.acts:
			s += "\n" +  "{:.3f}".format(a.profit)
		s += "\n\n# ENERGY\n"
		for i in range(len(self.acts[0].energy)):
			for a in self.acts:
				s += "{:.3f}".format(a.energy[i]) + " "
			s += "\n"
		s += "\n# ACTION LOWER BOUND\n" 
		s += ("-1 "*(len(self.acts)) + "\n")*self.nyears # lowerbound of actions
		s += "\n# ACTION UPPER BOUND\n" 
		s += ("-1 "*(len(self.acts)) + "\n")*self.nyears # upperbound of actions
		s += "\n# DEPEDENCY BETWEEN ACTIONS\n" 
		s += ("-1 "*(len(self.acts)) + "\n")*len(self.acts) # depedency between actions
		s += "\n"
		return s

	def to_JSON(self, ind=None):
	    return json.dumps(self, default=lambda o: o.__dict__, \
				sort_keys=False, indent=ind)
	
	""" Loads an Instance from a JSON string. """
	@staticmethod
	def from_JSON(json_str):
		dic = json.loads(json_str)
		self.__dict__.update(d)

	def __str__(self):
		s = ""
		s += "Years: " + str(self.nyears) + "\n"
		s += "Budgets: " + fs2str(self.budgets) + "\n"
		s += "Goals: " + str(self.goals) + "\n"
		s += "Acts: " + str(self.nacts) + "\n"
		for a in self.acts:
			s += "\n" + str(a)
		return s

