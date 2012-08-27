#!/usr/bin/env python

import sys
from PyQt4 import Qt, QtGui

image = QtGui.QImage(sys.argv[1])
width, height = image.size().width(), image.size().height()

f = open(sys.argv[2], "w")

if sys.argv[3] == "img":
    rows = 1
    if len(sys.argv) > 4:
        rows = int(sys.argv[4])
    f.write("%d,%d,\n" % (width, height/rows))
    for y in range(0, height, 8):
        for x in range(width):
            value = 0
            for z in range(8):
                if image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z
            f.write("0x%02x," % value)
        f.write("\n")
elif sys.argv[3] == "char":
    for y in range(0, height, 8):
        for x in range(width):
            value = 0
            for z in range(8):
                if image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z
            f.write("0x%02x," % value)
        f.write("\n")
elif sys.argv[3] == "dblsize":
    for y in range(0, height, 16):
        for x in range(width):
            for l in range(0, 16, 8):
                value = 0
                for z in range(8):
                    if image.pixel(x, y+l+z) == Qt.qRgb(0, 0, 0):
                        value += 1 << z
                f.write("0x%02x," % value)
        f.write("\n")
