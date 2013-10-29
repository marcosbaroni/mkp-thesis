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
		conv = Converter(sys.argv)

if __name__ == "__main__":
	main()
