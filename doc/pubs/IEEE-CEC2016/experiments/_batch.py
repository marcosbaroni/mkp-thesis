#!/usr/bin/python3
from sys import argv, stdout, exit
from subprocess import Popen, PIPE, getoutput
from time import sleep

ns = [100, 250, 500][:1]
ms = [5, 10, 30][:1]
ts = [0.25, 0.5, 0.75][:1]
iis = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10][:3]
ss = range(10)[:5]

jobs = [(n,m,t,i,s) for n in ns for m in ms for t in ts for i in iis for s in ss]
instdir = "/home/mbaroni/Computacao/dmkp/insts/chubeas/"

class JobEater:
    def __init__(self, jobs, output):
        self.output = output
        self.n, self.m, self.t, self.i, self.s = jobs.pop(0)
        self.inst = instdir + "OR{0}x{1}-{2:1.2f}_{3}.dat".format(self.m, self.n, self.t, self.i)
        self.p = Popen(['mkp-sfl', self.inst, str(20), str(20), str(5), str(300), str(20), str(self.s)], stdout=PIPE, universal_newlines=True)
        self.core = False
        self.sout = "{0};{1};{2};{3};{4};".format(self.n, self.m, self.t, self.i, self.s)

    # Checks if associated process is finished. If so, starts the next process instances (if any).
    def isDone(self):
        pstatus = self.p.poll()
        if pstatus != None:
            sout = self.p.communicate()[0].strip()
            profit, it, tim = sout.split(';')
            self.sout += "{0};{1};".format(profit, tim)
            
            if not self.core:
                self.core = True
                ncore = self.n + (self.m/10)
                self.p = Popen(['mkp-sfl-core', self.inst, str(20), str(20), str(5), str(300), str(20), str(ncore), str(self.s)], stdout=PIPE, universal_newlines=True)
                return False
            else:
                self.p = Popen(['mkp-chubbest', str(self.n), str(self.m), "{0:1.2f}".format(self.t), str(self.i)], stdout=PIPE, universal_newlines=True)
                self.sout += getoutput("mkp-chubbest {0} {1} {2:0.2f} {3}".format(self.n, self.m, self.t, self.i)) + u'\n'
                self.output.write(self.sout)
                self.output.flush()
                return True
        return False


def main():
    if(len(argv) < 2):
        print("usage {0} <outfile>".format(argv[0]))
        exit(1)
    out = open(argv[1], "w")

    procs = []
    for i in range(4):
        procs.append(JobEater(jobs, out))

    while(len(procs) > 0):
        p = procs.pop()
        if p.isDone():
            if(len(jobs) > 0):
                procs.append(JobEater(jobs, out))
        else:
            procs.append(p)
        sleep(0.1)

if __name__ == '__main__':
    main()
