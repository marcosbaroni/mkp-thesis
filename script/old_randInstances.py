import random
import sys

# primeiro arguemnto: seed

def gen():
    alpha = float(sys.argv[2])
    years = int(sys.argv[3])
    camp = int(sys.argv[4])

    print "data;\n"
    #years = random.randint(3,5)
    print "param duracao := " + str(years) + ";\n"
    #camp = random.randint(5,6)
    print "param acoes := " + str(camp) + ";\n"
    

    medianOrc = random.randint(700,750)
    varOrc = random.randint(10,15)

    print "param orcamento := "
    orcSum =  0
    minOrc = 999999999
    for i in range(years):
        orcAux = abs(random.gauss(medianOrc, varOrc))
        orcSum += orcAux
        minOrc = min(minOrc, orcAux)
        print str(i + 1) + " " + str(orcAux)
    print ";\n"


    vecCost = []
    print "param custo := "
    for i in range(camp):
        vecCost.append(min(int(minOrc),random.randint(10,50)))
        print str(i + 1) + " " + str(vecCost[-1])
    print ";\n"


    print "param uc := "
    for i in range(camp):
        print str(i + 1) + " " + str(int(orcSum/vecCost[i]*(2.0/years)))
    print ";\n"



    vecCostAux = []
    for j in range(camp):
        vecCostAux.append([])
        for i in range(years):
            vecCostAux[-1].append(random.random())
        vecCostAux[-1].sort()
        vecCostAux[-1].reverse()


    for j in range(camp):
        s = 0
        for i in range(years):
            s += vecCostAux[j][i]
        for i in range(years):
            vecCostAux[j][i] /= s

    print "param energia : " + " ".join([str(x) for x in range(1,camp+1)]) + ":=" 
    for i in range(years):
        toPrint = str(i+1) + " "
        for j in range(camp):
            toPrint += str(max(0,abs((1-alpha)*vecCost[j]*vecCostAux[j][i] + random.randint(-100,100)*alpha))) + " "
        print toPrint
    print ";\n"

#    print "param u : " + " ".join([str(x) for x in range(1,camp+1)]) + ":="
#    for i in range(years):
#        print str(i+1) + " " + " ".join((["-1"]*camp)) 
#    print ";\n"

    print "end;"
        


def main():
    if(len(sys.argv) != 5):
        print "seed, alpha, years, camps"
        return
#    random.seed(int(sys.argv[1]))
    gen()

main()

