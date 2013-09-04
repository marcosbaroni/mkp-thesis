#from instances 
import random as r
import math
import sys


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
		aux = r.gauss(0.0, 10.0)
		x = c + b*sign(aux)*math.pow(abs(aux), 3.5)
	return x

class Action:
	def __init__(self, inst):
		self.inst = inst
		self.years = self.inst.years
		self.tir = 0.15 + 0.01*exp_gauss(10., 120.)
		#self.tir = 15 + r.random()*45 + math.exp(r.gauss(0,1.2))*45
		self.set_curve(r.randint(1,3))
		self.set_group(r.randint(1,4))
		self.set_tir()
	
	# argument: class of curve
	def set_curve(self, c):
		years = self.inst.years
		if   c == 1:
			self.curve = [r.randint(4,7)/10.0] + [1.0]*(years-1)
		elif c == 2:
			self.curve = [0.5, 0.85, 0.7, 0.5, 0.25] + [0.0]*(years-5)
			self.curve = self.curve[0:years]
		elif c == 3:
			self.curve = [0.5, 0.25, 0.15] + [0.0]*(years-3)
	
	# argument: class of group (uc/cost)
	def set_group(self, g):
		self.group = g
		self.profit = 0.16 + r.random()*0.16

		#   ^ 4
		#   | |
		#   | |
		#   | v
		# U |            CLASSES
		# C | ^
		#   | |
		#   | 2
		#   |  1-->        <--3
		#   +----- COST --------->

		MIN_UC = 150
		MIN_COST = 100

		# Classe "g" (see diagram above)
		if g == 1:   	# CENTER --> COST AXIS
			self.cost = exp_gauss(500, 16000)
			self.uc = int(max(MIN_UC, r.gauss(5000, 1000)))
		elif g == 2: 	# CENTER --> UCs AXIS
			self.cost = max(MIN_COST, r.gauss(500, 300))
			self.uc = int(exp_gauss(50000, 450000))
		elif g == 3: 	# HIGH COST --> CENTER
			self.cost = exp_gauss(15000, 16000)
			self.uc = int(max(MIN_UC, r.gauss(5000, 1000)))
		elif g == 4: 	# HIGH UCs --> CENTER
			self.cost = max(MIN_COST, r.gauss(500, 300))
			self.uc = int(exp_gauss(400000, 450000))
		return

	def set_tir(self):
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
	
	def __str__(self):
		s = ""
		s += "Cost: " + "{:.3f}".format(self.cost) + "\n"
		s += "UC: " + str(self.uc) + "\n"
		s += "Curve: " + fs2str(self.curve, 2) + "\n"
		s += "Energy: " + fs2str(self.energy) + "\n"
		s += "Tir: " + "{:.3f}".format(self.tir) + "\n"
		return s

class Instance:
	stddist = [.36, .36, .14, .14] # Distribution for respective action group
	def __init__(self, seed, alpha, years, camp):
		r.seed(seed)
		self.years = years
		self.camp = camp
		self.alpha = alpha
		self.acts = []
		for i in range(self.camp):
			self.acts.append(Action(self))
		self.set_budgets()		# yearly budgets
		self.set_goals()		# yearly goals
	
	def set_budgets(self):
		self.budgets = []
		total = 0
		for a in self.acts:
			total += a.uc*a.cost
		med = total/self.years
		for i in range(self.years):
			self.budgets.append( 0.7*med + 0.4*med*r.random() )

	def set_goals(self):
		self.goals = []
		total = 0
		for a in self.acts:
			total += a.uc*sum(a.energy)
		med = total/self.years
		mini = 0.9*med
		for i in range(self.years):
			self.goals.append(mini + 0.2*med*r.random() )

	def __str__(self):
		s = ""
		s += "Years: " + str(self.years) + "\n"
		s += "Budgets: " + fs2str(self.budgets) + "\n"
		s += "Goals: " + str(self.goals) + "\n"
		s += "Acts: " + str(self.camp) + "\n"
		for a in self.acts:
			s += "\n" + str(a)
		return s
	
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
		s += "set xlabel \"Cost\"\n"
		s += "set ylabel \"UCs\"\n"
		s += "set zlabel \"TIR\"\n"
		s += "set grid xtics ytics ztics\n"
		s += "splot '-' using 1:2:3 with points pointsize 3 title \"Random\""
		s += " ,'-' using 1:2:3 with points pointsize 3 title \"Real\"\n"
		for a in self.acts:
			s += a.to_gplot() + "\n"
		s += "e e e\n"
		s += real_inst
		s += "pause -1\n"
		return s


	def scip(self):
		s = ""
		s += "# N of YEARS"				# Recursos
		s += "\n" +  str(self.years)
		s += "\n# N of ACTIONS"				# Recursos
		s += "\n" +  str(self.camp)
		s += "\n# N of RESOURCES"				# Recursos
		s += "\n1"				# Recursos
		s += "\n# RATE"				# Recursos
		s += "\n0.15"			# Rate
		s += "\n\n# YEARLY GOALS"				# Recursos
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
		s += ("-1 "*(len(self.acts)) + "\n")*self.years # lowerbound of actions
		s += "\n# ACTION UPPER BOUND\n" 
		s += ("-1 "*(len(self.acts)) + "\n")*self.years # upperbound of actions
		s += "\n# DEPEDENCY BETWEEN ACTIONS\n" 
		s += ("-1 "*(len(self.acts)) + "\n")*len(self.acts) # depedency between actions
		s += "\n"
		return s

def main():
	if len(sys.argv) < 4: print " <gs> years, actions, [seed]\n\nOPTIONS:\n  g gnuplot\n  s scip"
	else:
		if len(sys.argv) > 4: seed = int(sys.argv[3]);
		else: seed = None
		inst = Instance(seed, 0.0, int(sys.argv[2]), int(sys.argv[3]))
		o = sys.argv[1]
		if o == 's': print inst.scip()
		elif o == 'g': print inst.gplot()
main()

