#!/usr/bin/python
from sys import argv

nsA = range(10, 130, 20);
nsC = range(10, 70, 10);
ns=[
    ('1', list(range(50, 201, 50))),
    ('2', list(range(50, 301, 50))),
    ('3', list(range(50, 201, 50))),
    ('4', list(range(50, 101, 50))),
]
#nps = [2, 3, 4, 5];
nps = [2];
iis = range(1, 11);
dims = [1, 2, 3];

argList = []
# nps... 
for np in nps:
    # types
    for (t, ns) in ns:
        for n in ns:
            for i in iis:
                for d in dims:
                    argList.append((np, n, i, d));

# print number of total inputs
if len(argv) < 2:
    print(len(argList));
    exit(1)

# print all mapped inputs
if argv[1] == 'all':
    print("<np> <n> <i> <ndim>");
    for (i, args) in enumerate(argList):
        print("% 4d | " % i + "%d %03d %d %d" % args)
    exit(1)

# print the given input
arg = (int)(argv[1]);
args = argList[arg];

print("%d %03d %d %d" % args)

