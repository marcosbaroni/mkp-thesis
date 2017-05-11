#!/usr/bin/python
from sys import argv

nsA = range(10, 130, 20);
nsC = range(10, 70, 10);
nps = [2, 3, 4, 5];
iis = range(1, 11);
dims = [0, 3];

argList = []
for np in nps:
    for idx in range(0, 6):
        # Classe A
        for ns in [nsA, nsC]:
            n = ns[idx];
            for i in iis:
                for d in dims:
                    argList.append((np, n, i, d));

if len(argv) < 2:
    print len(argList);
    exit(1)

arg = (int)(argv[1]);
args = argList[arg];

print "%d %03d %d %d" % args

