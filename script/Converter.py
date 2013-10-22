import sys

from Instance import *

class Converter:
	def __init__(args):
		self.inst = Instance()
		self.in_format = None
		self.in_file = None
		self.out_format = None
		self.out_file = None
		self.seed = None
		self.args = args
	
	def assert_arguments():
		if len(self.args) < 4:
			print_usage()
			exit(1)
	
	def catch_option(arg):
		if len(arg) < 0:
			return False
		if arg[0] != '-':
			return False
		arg.pop(0)
		pass
		return True

	def read_output_file()
		while(catch_option()):
			continue
		try:
			nex = arg.pop(0)
			if nex != "-":
				self.out_file = nex
		except ValueError:
			pass

	def read_output_file()
		while(catch_option()):
			continue
		try:
			nex = arg.pop(0)
			if nex != "-":
				self.in_file = nex
		except ValueError:
			pass

	def parse_arguments():
		read_input_file()
		read_output_file()
	
	def main():
		gen = Generator(sys.argv)
		

def print_usage():
 	print "ARGS: <# years> <# actions> <dtir*> [seed]\n"
	print "  dtir : tir variation [0.0, 1.0]"
	#print "Will always write \"knap.dat\" and \"knap.gplot\"."


def main():
	nyears = int(sys.argv[1])
	nacts = int(sys.argv[2])
	dtir = float(sys.argv[3])

	r.seed(seed)
	inst = Instance.random(nyears, nacts, dtir)
	print inst.to_scip()

if __name__ == "__main__":
	main()
