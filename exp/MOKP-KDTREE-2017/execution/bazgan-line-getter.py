#!/usr/bin/python
from sys import argv

config = [
  ( 2      # np
  , [1, 2]  # ndims
  , [ ('A', list(range(50, 201, 50))) # types / n ranges
    , ('B', list(range(50, 301, 50)))
    , ('C', list(range(50, 201, 50)))
    , ('D', list(range(50, 101, 50)))
    ]
  ),
  ( 3        # np
  , [1, 2, 3]  # ndims
  , [ ('A', list(range(50, 201, 50))) # types / n ranges
    , ('C', list(range(50, 201, 50)))
    ]
  )
]
#nps = [2, 3, 4, 5];
iis = range(1, 11);

argList = []
# nps... 
for (np, ndims, tys) in config:
    # types
    for (cls, ns) in tys:
        for n in ns:
            for i in iis:
                for ndim in ndims:
                    argList.append((np, cls, n, i, ndim))

# print number of total inputs
if len(argv) < 2:
    print(len(argList));
    exit(1)

# print all mapped inputs
if argv[1] == 'all':
    print("<np> <classe> <n> <i> <ndim>");
    for (i, args) in enumerate(argList):
        print("% 4d | " % i + "%d %c %d %d %d" % args)
    exit(1)

# print the given input
arg = (int)(argv[1]);
args = argList[arg];

print("%d %c %d %d %d" % args)

