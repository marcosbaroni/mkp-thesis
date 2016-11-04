#!/usr/bin/python
from sys import argv

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

def med(l):
    return sum(l)/float(len(l))

def getDic(table):
    dc = {}
    dn = {}
    dd = {}
    for (n, c, d, i, nn, nc ,t) in table:
        if(not dn.has_key(n)): dn[n] = 1
        if(not dc.has_key(c)): dc[c] = 1
        if(not dd.has_key(d)): dd[d] = 1
    ns = map(int, dn.keys())
    cs = map(int, dc.keys())
    ds = map(int, dd.keys())

    # initializing master dict 
    dic = {}
    for c in cs:
        dic[c] = {}
        for d in ds:
            dic[c][d] = {}
            for n in ns:
                dic[c][d][n] = {'nn': [], 'nc': [], 't': []}

    # populating master dict
    for (n, c, d, i, nn, nc ,t) in table:
        dic[c][d][n]['nn'] += [nn]
        dic[c][d][n]['nc'] += [nc]
        dic[c][d][n]['t'] += [t]

    # agregating data
    for (c,d,n) in [(c,d,n) for c in cs for d in ds for n in ns]:
        ncs = dic[c][d][n]['nc']
        dic[c][d][n]['nc-med'] = med(ncs)
        ts = dic[c][d][n]['t']
        dic[c][d][n]['t-med'] = med(ts)

    return dic


if __name__ == '__main__':
    f = open(argv[1], "r")
    lines = f.readlines()
    table = map(lambda x: map(lambda x: float(x), x.rstrip().split(';')), lines)
    table = [(int(n), int(c), int(d), int(i), int(nn), int(nc), t) for (n,c,d,i,nn,nc,t) in table]
    d = getDic(table)
    print(d)
