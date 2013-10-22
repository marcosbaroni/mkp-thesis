from sys import argv

def resAppend(res, ye, ac, dt, se, ti):
	if not res.has_key(dt): res[dt] = {}
	if not res[dt].has_key(ye): res[dt][ye] = {}
	if not res[dt][ye].has_key(ac): res[dt][ye][ac] = {}
	res[dt][ye][ac][se] = float(ti)

def writeMedFile(meds):
	f = open("med.csv", "w")
	dts = meds.keys()
	dts.sort()
	for d in dts:
		years = meds[d].keys()
		years.sort()
		for y in years:
			acs = meds[d][y].keys()
			acs.sort()
			for a in acs:
				f.write(str(d) + ";" + str(y) + ";" + str(a) + ";" + str(meds[d][y][a]) + "\n");
	f.close()

def print_data(med, d):
	print "set term pngcairo"
	print "set output \"" + str(d) + ".png\""
	print "plot '-' using 2:1:3 with image"
	years = med.keys()
	years.sort()
	for y in years:
		acs = med[y].keys()
		acs.sort()
		for a in acs:
			print(str(y) + " " + str(a) + " " + str(med[y][a]));
	print("e")

def read_results(fname):
	f = open(fname, "r")
	lines = f.readlines()
	res = {}

	# Lendo dados
	for line in lines:
		ye, ac, dt, se, ti, garb = line.split(";")
		resAppend(res, int(ye), int(ac), float(dt), int(se), float(ti))
	meds = {}

	# Calculando medias
	for d in res.keys():
		meds[d] = {}
		for y in res[d].keys():
			meds[d][y] = {}
			for a in res[d][y].keys():
				l = res[d][y][a].values()
				l.sort()
				l = l[2:][:-2]
				meds[d][y][a] = sum(l)/float(len(l))
	
	# Imprimindo
	for d in meds.keys():
		print_data(meds[d], d)
	writeMedFile(meds)

if __name__ == "__main__":
	read_results(argv[1])
