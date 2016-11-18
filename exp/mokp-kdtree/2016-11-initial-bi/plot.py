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
    ks = d.keys();
    ks.sort()
    return ks

def getNitems(table, c, d):
    di = {}
    for l in table:
        if l[1] == c and l[2] == d:
            di[l[0]] = 1
    ks = di.keys();
    ks.sort()
    return ks

def getDims(table, c):
    d = {}
    for l in table:
        if l[1] == c:
            d[l[2]] = 1
    ks = d.keys();
    ks.sort()
    return ks

# Returns a dictionary with the data:
#   dic[classe][ndim][nitem]['t']: average time for 'this'
#   dic[classe][ndim][nitem]['c']: average comparison for 'this'
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
                dic[c][d][n] = {}
                dic[c][d][n]['t'] = med(ts)
                dic[c][d][n]['c'] = med(cs)
    cs = dic.keys()
    cs.sort()
    ds = dic[cs[0]].keys()
    ds.sort()
    ns = dic[cs[0]][ds[0]].keys()
    ns.sort()
    return (dic, cs, ds, ns)


def lblDim(dim):
    cdic = {0: 'black', 1: 'green', 2: 'blue', 3: 'yellow'}
    # setting label
    if dim == 0: lbl = 'list'
    elif dim == 1: lbl = 'AVL'
    else: lbl = str(dim) + 'd-tree'
    # setting color
    return lbl


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


def plotLineTime(c, dic, di):
    s = ''
    s += 'set term png\n'
    s += 'set output \'' + di + '/out' + str(c) + '.png\'\n'
    s += 'set title \'Classe ' + str(c) + '\'\n'
    s += 'set logscale y\n'
    s += 'set grid\n'
    s += 'plot '
    ps = []
    ds = dic[c].keys()
    ds.sort()
    for d in ds:
        ps += [" '-' lw 3 with lines t '" + lblDim(d) + "'"]
    s +=  ','.join(ps) + '\n'
    for d in ds:
        ns = dic[c][d].keys()
        ns.sort()
        for n in ns:
            s += str(n) + ' ' + str(dic[c][d][n].t) + "\n"
        s += "e\n"
    #print(s)
    p = Popen(['gnuplot'], stdin=PIPE)
    p.communicate(s)
    f = open('/home/mbaroni/Desktop/out' + str(c) + '.txt', 'w')
    f.write(s)
    f.close()

# c: classe de instancia
# dic: dicionario com os dados
# ds: lista de dimensoes
# ns: lista de nitens
def plotBars(c, dic, dout, tipo):
    s = ''

    ds = dic[c].keys(); ds.sort()
    ns = dic[c][ds[0]].keys(); ns.sort()

    tit_dic= {}
    tit_dic['t'] = 'Tempo'
    tit_dic['c'] = 'N-Comparacoes'
    titulo = tit_dic[tipo]

    ###########################
    #   Configurando o plot   #
    ###########################
    # titulo do grafico
    s += 'set title \"BiO-KP: ' + titulo + ' para classe ' + str(c) + '\"\n'

    # titulo do eixo y
    s += 'set ylabel \"' + titulo + '\"\n'

    # tipo de grafico
    s += 'set style data histogram\n'

    # borda das barras
    s += 'set style fill solid border\n'

    # escala logaritmica no y
    s += 'set logscale y\n'

    # grid no fundo
    s += 'set grid\n'

    # estilo do grafico
    s += 'set style histogram clustered\n'

    # borda das barras 
    s += 'set style fill solid border 1\n'

    # tipo de saida
    s += 'set term pngcairo font \'Arial bold\'\n'

    # arquivo de saida
    s += "set output \"" + dout + '/classe' + str(c) + '-' + titulo.lower() + ".png\"\n"

    ###########################
    #   Escrevendo os dados   #
    ###########################
    s += '$data << EOD\n'
    # header
    s += '000 ' + ' '.join(map(lblDim, ds)) + '\n'
    for n in ns:
        s += str(n) + ' '
        for d in ds:
            s += ( '%.3f' % dic[c][d][n][tipo]) + ' '
        s += '\n'
    s += 'EOD\n'

    ###########################
    #        Plotando         #
    ###########################
    s += 'plot for [COL=2:%d] "$data" using COL:xticlabels(1) title columnheader' % (len(ds) + 1)

    return s


# execute gnuplot with <script> as input
def exec_gnuplot(script):
    p = Popen(['gnuplot'], stdin=PIPE)
    p.communicate(script)


# main function
if __name__ == '__main__':
    if len(argv) < 3:
        print('usage:' + argv[0] + ' <input.csv> <output dir>')
        exit(1)

    # pasta destino
    dout = argv[2]

    # lendo os dados
    f = open(argv[1], "r")
    lines = f.readlines()
    table = map(lambda x: map(lambda x: float(x), x.rstrip().split(';')), lines)
    table = [(int(n), int(c), int(d), int(i), int(nn), int(nc), t) for (n,c,d,i,nn,nc,t) in table]
    f.close()

    # gerando dicionario
    (dic, cs, ds, ns) = getDic(table)

    # plotando graficos
    for c in cs:
        # plotando tempo
        s = plotBars(c, dic, dout, 't')
        exec_gnuplot(s)

        # plotando comparacoes
        s = plotBars(c, dic, dout, 'c')
        exec_gnuplot(s)

