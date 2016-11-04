#!/usr/bin/python
from sys import argv
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt

from math import log
from subprocess import Popen, PIPE

#
#         Classe  N
#
#    |                            |
#  t |                         c  |
#  i |                         o  |
#  m |                         m  |
#  e |                         p  |
#    |                         s  |
#    +-------------------         +---------------------
#           nitem                       nitem
#
#   1. Classe
#   2. Dim
#   3. Nitem
#

class X: pass

def call(func, args): # without star
    func(*args)

def med(l):
    return sum(l)/float(len(l))

def getClasses(table):
    d = {}
    for l in table:
        d[l[1]] = 1
    return d.keys()

def getNitems(table, c, d):
    di = {}
    for l in table:
        if l[1] == c and l[2] == d:
            di[l[0]] = 1
    return di.keys()

def getDims(table, c):
    d = {}
    for l in table:
        if l[1] == c:
            d[l[2]] = 1
    return d.keys()

def getDic(table):
    cs = getClasses(table)
    dic = {}
    for c in cs:
        dic[c] = {}
        ds = getDims(table, c)
        for d in ds:
            dic[c][d] = {}
            ns = getNitems(table, c, d)
            for n in ns:
                dic[c][d][n] = X()
                ts, cs = ([], [])
                for l in table:
                    if l[0] == n and l[1] == c and l[2] == d:
                        cs += [l[5]]
                        ts += [l[6]]
                dic[c][d][n] = X()
                dic[c][d][n].t = med(ts)
                dic[c][d][n].c = med(cs)
    return dic


def lblDim(dim):
    cdic = {0: 'black', 1: 'green', 2: 'blue', 3: 'yellow'}
    # setting label
    if dim == 0: lbl = 'list'
    elif dim == 1: lbl = 'AVL'
    else: lbl = 'KD-tree ' + str(dim)
    # setting color
    return lbl

def plotTime(c, dic, di):
    s = ''
    s += 'set term png\n'
    s += 'set output \'' + di + '/out' + str(c) + '.png\'\n'
    s += 'set title \'Classe ' + str(c) + '\'\n'
    s += 'plot '
    ps = []
    for d in dic[c].keys():
        ps += [" '-' lw 2 with lines t '" + lblDim(d) + "'"]
    s +=  ','.join(ps) + '\n'
    for d in dic[c].keys():
        ns = dic[c][d].keys()
        for n in ns:
            s += str(n) + ' ' + str(dic[c][d][n].t) + "\n"
        s += "e\n"
    #print(s)
    p = Popen(['gnuplot'], stdin=PIPE)
    p.communicate(s)

def myplot(c, dic, ls, field, title, ylbl):
    (ns, cs, ds) = ls
    for d in ds:
        plt.plot(ns, [dic[c][d][n][field] for n in ns], '-', label=lblDim(d), linewidth=2.0)
    plt.legend(loc='upper right')
    plt.title('Class ' + str(c) + ' - ' + title)
    plt.yscale('logit')
    plt.ylabel(ylbl)
    plt.xlabel('size')
    plt.savefig(field+str(c)+'.png')

if __name__ == '__main__':
    if len(argv) < 3:
        print(argv[0] + ' input.csv output.gplot')
        exit(1)

    f = open(argv[1], "r")
    di = argv[2]
    lines = f.readlines()
    table = map(lambda x: map(lambda x: float(x), x.rstrip().split(';')), lines)
    table = [(int(n), int(c), int(d), int(i), int(nn), int(nc), t) for (n,c,d,i,nn,nc,t) in table]
    dic = getDic(table)
    for c in dic.keys():
        plotTime(c, dic, di)
