#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
    This script parses debug log events related to Lua memory (de)allocations ( lines
    starting wiht "LT: ") and produces a data for gnuplot program

    Usage:

        ./simu 2>&1 | grep "^LT" | ../radio/util/lua_trace2plot.py > data.plot
        gnuplot  -e 'set xtics rotate;  plot "data.plot" using 2:xtic(1) ; pause mouse close'
"""

from __future__ import print_function

import sys


if len(sys.argv) > 1:
    inputFile = sys.argv[1]
    inp = open(inputFile, "r")
else:
    inp = sys.stdin


x = 0
memUsed = 0
while True:
    skip = True
    line = inp.readline()
    if len(line) == 0:
        break
    line = line.strip('\r\n')
    if len(line) == 0:
        skip = True
    if line.startswith("LT:"):
        skip = False

    if not skip:
        parts = line.split()
        if len(parts) >= 3:
            data = parts[1].strip("[").strip("]").split(",")
            alloc = int(data[0])
            free = int(data[1])
            line = parts[2]
            if alloc > 0:
                memUsed += alloc
                print("'%s'\t%d" % (line, memUsed))
                x += 1
            if free < 0:
                memUsed += free
                print("'%s'\t%d" % (line, memUsed))
                x += 1

inp.close()
