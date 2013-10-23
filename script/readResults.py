from sys import argv
from util import *

def dic_to_csv(dic, prefix=''):
	if isinstance(dic, dict):
		s = ''
		for k, v in dic.items():
			s += dic_to_csv(v, prefix + str(k) + ";")
		return s
	else:
		return prefix + str(dic) + "\n"

def dic_append(dic, lis):
	if len(lis) == 2:
		dic[lis[0]] = lis[1]
	else:
		if not dic.has_key(lis[0]):
			dic[lis[0]] = {}
		dic_append(dic[lis[0]], lis[1:])

def dic_apply_func(dic, func=mean):
	if isinstance(dic.values()[0], dict):
		newdic = {}
		for k, v in dic.items():
			newdic[k] = dic_apply_func(v, func)
		return newdic
	else:
		return func(dic.values())
	

""" Read an CSV file, groups the values based on the first arguments and aplies
		the given function. """
def read_data(fname, funcs=[min,mean,max]):
	# Opening file
	f = open(fname, "r")
	lines = f.readlines()

	# Reading Data
	raw = []
	for line in lines:
		raw.append(line.split(";"))
	
	# Building Dictionary
	dic = {}
	for l in raw:
		dic_append(dic, l)

	return dic


if __name__ == "__main__":
	data = read_data(argv[1])
	dic = dic_apply_func(data)
	print dic_to_csv(dic)

