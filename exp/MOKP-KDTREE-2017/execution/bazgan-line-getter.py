#!/usr/bin/python
from sys import argv

# m = 2
c2 = ( 2      # np
  , [1, 2]  # ndims
  , [ ('A', list(range(40, 121, 20))) # types / n ranges
    , ('B', list(range(100, 301, 50)))
    , ('C', list(range(20, 101, 20)))
    , ('D', list(range(20, 51, 10)))
    ]
  )

# m = 3
c3 = ( 3        # np
  , [1, 2, 3]  # ndims
  , [ #('A', list(range(50, 81, 10))) # types / n ranges
    ('B', list(range(100, 401, 100)))
    #, ('C', list(range(20, 41, 10)))
    , ('D', list(range(10, 31, 10)))
    ]
  );

# m = 4
c4 = ( 4        # np
  , [1, 2, 3, 4]  # ndims
  , [ ('A', list(range(20, 51, 10))) # types / n ranges
    , ('C', list(range(10, 31, 10)))
    ]
  )

#config = [c2, c3, c4]
config = [c3]
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

