import sqlite3
from hashlib import md5
from sys import argv
from subprocess import call

### params
CSV2DB='../../bin/csv2sqlite3'

######################################
#   auxiliaries
######################################
# return name o structure, given dimension
def idxDic(ndim):
    ndim = int(ndim)
    if( ndim == 0 ):
        return 'list'
    elif( ndim == 1 ):
        return 'avl'
    else:
        return str(ndim) + 'd-tree'

# return name o instance classe, given number
def tyDic(ty):
    ty  = int(ty)
    return ['A','B','C','D'][ty-1]

# format a list into a row
def prtRow(ls):
    return ' ; '.join(map(colpad, ls)) + '\n'

# column padding
def colpad(s):
    if( s.__class__.__name__ != 'str'):
        s = '%.2f' % s
    return s.rjust(10)


######################################
#   build tabular
######################################
def buildTabular(argv, nargs):
    if( nargs < 3 ):
        print('usage: %s %s <csv file>' % tuple(argv) )
        print('')
        print('csv pattern:')
        print('mokp')
        print('ndim;t;m;n;i;nd;ncomp;time;maxnd')
        print('int;int;int;int;int;int;int;float;int')
        print('...')
        exit(1)

    csvfname = argv[2]
    dbfname = '/tmp/out.db'
    
    # create db file
    call([CSV2DB, csvfname, dbfname])

    # process db file
    conn = sqlite3.connect(dbfname)
    c = conn.cursor();
    sql = 'SELECT DISTINCT m FROM mokp';
    c.execute(sql)
    rows = c.fetchall()
    ms = list(map(lambda x : x[0], rows))

    # TABLE for dim = m
    for m in ms:
        print('\n\nM = %d' % m)
        c.execute('SELECT DISTINCT ndim FROM mokp WHERE m = ? ORDER BY ndim ASC', (m,))
        rows = c.fetchall()
        ndims = list(map(lambda x : x[0], rows))

        # header
        s = prtRow(['Type', 'n', '|ND|'] + list(map(idxDic, ndims)))

        # types
        c.execute('SELECT DISTINCT t FROM mokp WHERE m = ? ORDER BY ndim ASC', (m,))
        rows = c.fetchall()
        types = list(map(lambda x : x[0], rows))
        for t in types:
            c.execute('SELECT DISTINCT n FROM mokp WHERE m = ? AND t = ? ORDER BY n ASC', (m, t,))
            rows = c.fetchall()
            ns = list(map(lambda x : x[0], rows))
            # ns
            for n in ns:
                # average |ND|
                c.execute('SELECT avg(nd) FROM mokp WHERE m = ? AND t = ? AND n = ?', (m, t, n,))
                rows = c.fetchall()
                nd = rows[0][0]
                # times

                line = [t, n, nd]
                for ndim in ndims:
                    c.execute('SELECT avg(time) FROM mokp WHERE m = ? AND t = ? AND n = ? AND ndim = ?', (m, t, n, ndim,))
                    rows = c.fetchall()
                    print((m, t, n, ndim,))
                    print(rows[0][0])
                    line.append(rows[0][0])

                # append line
                s += prtRow(line)

        print(s)

    # remove temp db file
    call(['rm', dbfname]);


######################################
#   usage
######################################
def printUsage():
    print('usage: %s <cmd>' % argv[0])
    print('')
    print('<cmd> can be:')
    print('  tab <csv file>: generate tabulars')
    exit(1)


######################################
#   main
######################################
if __name__ == '__main__':
    nargs = len(argv)

    # not enough arguments
    if( nargs < 2 ):
        printUsage()

    # exec dispatching
    cmd = argv[1];
    # 'tab' (generate latex table)
    if( cmd == 'tab' ):
        buildTabular(argv, nargs)
    else:
        printUsage()
