# -*- coding: utf-8 -*-
import codecs
from sys import argv, stdout
from string import strip

#\\usepackage[utf8]{inputenc}
#\\usepackage[T1]{fontenc}

header='''
\\documentclass{article}
\\usepackage[paperheight=11.25in,paperwidth=8.25in]{geometry}
\\usepackage{wallpaper}
\\usepackage[brazilian]{babel}
\\usepackage{fontspec}
\\usepackage{xcolor}
\\usepackage{pgf}

%%%%%%   SET FONT   %%%%%%%%
%\\setmainfont{Lobster Two}
%\\setmainfont{Alex Brush}
\\setmainfont{CAC Champagne}
%\\setmainfont{Allura}

\\definecolor{mycolor}{RGB}{73,73,73}

%%%%%%   SET COLOR %%%%%%%%
\\makeatletter
\\newcommand{\\globalcolor}[1]{%
  \\color{#1}\\global\\let\\default@color\\current@color
}
\\makeatother

\\AtBeginDocument{\\globalcolor{mycolor}}

%%%%%%   BACKGROUND   %%%%%%%
\\ULCornerWallPaper{1}{tags.pdf}

%%%%%%   DISTANCES   %%%%%%%
\\newcommand{\\xoffset}{-32}
\\newcommand{\\yoffset}{-40}
\\newcommand{\\xstep}{170}
\\newcommand{\\ystep}{-160}
\\newcommand{\\pwid}{40mm}

%%%%%%   POSITIONING   %%%%%%%
\\pgfmathsetmacro{\\yI}{int(round(\\yoffset ))}
\\pgfmathsetmacro{\\yII}{int(round(\\yoffset + \\ystep))}
\\pgfmathsetmacro{\\yIII}{int(round(\\yoffset + 2*\\ystep))}
\\pgfmathsetmacro{\\yIV}{int(round(\\yoffset + 3*\\ystep))}

\\pgfmathsetmacro{\\xI}{int(round(\\xoffset ))}
\\pgfmathsetmacro{\\xII}{int(round(\\xoffset + \\xstep))}
\\pgfmathsetmacro{\\xIII}{int(round(\\xoffset + 2*\\xstep))}
\\pgfmathsetmacro{\\xIV}{int(round(\\xoffset + 3*\\xstep))}

%%%%%%   TEXT FORMATING   %%%%%%%
\\newcommand{\yield}[1]{ \\centering \\LARGE #1}


%%%%%%   BODY   %%%%%%%
\\begin{document}
'''

footer='''
\\end{document}
'''

yList = ['\\yI', '\\yII', '\\yIII', '\\yIV']
xList = ['\\xI', '\\xII', '\\xIII']

def yeld(name, x, y):
    return '\\put(' + xList[x] + ', ' + yList[y] + '){\\parbox{\\pwid}{\\yield{' + name + '}}}\n'

def printBody(names):
    s = ''
    first = True
    while len(names) > 0:
        if first: first = False
        else: s+= '\\newpage\n'
        s += '\\begin{picture}(0,0)\n'
        s += yeld(names.pop(0), 0, 0)
        if len(names) > 0: s+= yeld(names.pop(0), 1, 0)
        if len(names) > 0: s+= yeld(names.pop(0), 2, 0)
        if len(names) > 0: s+= yeld(names.pop(0), 0, 1)
        if len(names) > 0: s+= yeld(names.pop(0), 1, 1)
        if len(names) > 0: s+= yeld(names.pop(0), 2, 1)
        if len(names) > 0: s+= yeld(names.pop(0), 0, 2)
        if len(names) > 0: s+= yeld(names.pop(0), 1, 2)
        if len(names) > 0: s+= yeld(names.pop(0), 2, 2)
        if len(names) > 0: s+= yeld(names.pop(0), 0, 3)
        if len(names) > 0: s+= yeld(names.pop(0), 1, 3)
        if len(names) > 0: s+= yeld(names.pop(0), 2, 3)
        s += '\\end{picture}\n'
    return s

if __name__ == '__main__':
    if len(argv) < 2:
        print("usage: {0} <names txt> [output tex]".format(argv[0]))
        exit(1)

    inp = codecs.open(argv[1], encoding='utf-8')
    # get output
    if len(argv) > 2: output = codecs.open(argv[2], "w", 'utf-8')
    else: output = stdout
    names = map(strip, inp.readlines())
    output.write(header)
    output.write(printBody(names).encode('utf-8'))
    output.write(footer)

