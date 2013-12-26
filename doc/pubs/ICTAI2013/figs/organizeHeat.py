from pprint import *
import numpy

def fixLine(line):
    line = line.replace("./data_easy/","")
    line = line.replace("./data_hard/","")
    line = line.replace("-", ";")
    line = line.replace("user", "")
    return line

timeDictAllAlpha = {}
for alpha in [0.0, 0.1, 1.0]:
    timeDictAllAlpha[alpha] = {}
    for year in range(5,16):
        timeDictAllAlpha[alpha][year] = {}
        for action in range(5,16):
            timeDictAllAlpha[alpha][year][action] = []

line=fixLine(raw_input())
while(line != ""):
    print line
    years = int(line.split(";")[0])
    actions = int(line.split(";")[1])
    alpha = float(line.split(";")[2])
    time = line.split(";")[5]
    time = time.replace("m", "*60+")
    time = time.replace("s", "")
    print time
    time = eval(time)


    timeDict = timeDictAllAlpha[alpha]
    timeDict[years][actions].append(time)


    try:
        line=fixLine(raw_input())
    except EOFError:
        break

pprint(timeDictAllAlpha)

for alpha in timeDictAllAlpha:
    alphaD = timeDictAllAlpha[alpha]
    print alpha
    print ";" + ";".join([ str(x) for x in range(5,16)])
    for year in sorted(alphaD.keys()):
        toPrint = str(year) + ";"    
        for action in sorted(alphaD[year].keys()):
            toAdd = 10 - len(alphaD[year][action])
            for i in range(toAdd):
                alphaD[year][action].append(60*60)
#            toPrint += str(numpy.median(alphaD[year][action])) + ";"
#            toPrint += str(sum([i >= 60*60 for i in alphaD[year][action]])) + ";"
            print year, action, sum([i >= 60*60 for i in alphaD[year][action]])
        print ""
#        print toPrint 



