#!/usr/bin/env python

import sys
from PyQt4 import Qt, QtGui

image = QtGui.QImage(sys.argv[1])
width, height = image.size().width(), image.size().height()

f = open(sys.argv[2], "w")

if len(sys.argv) > 3:
    what = sys.argv[3]
else:
    for s in ("03x05", "04x06", "05x07", "08x10", "10x14", "22x38"):
        if s in sys.argv[2]:
            what = s
            break

if what == "1bit":
    rows = 1
    if len(sys.argv) > 4:
        rows = int(sys.argv[4])
    f.write("%d,%d,\n" % (width, height/rows))
    for y in range(0, height, 8):
        for x in range(width):
            value = 0
            for z in range(8):
                if y+z < height and image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z            
            f.write("0x%02x," % value)
        f.write("\n")
elif what == "4bits":
    colors = []
    f.write("%d,%d,\n" % (width, height))
    for y in range(0, height, 2):
        for x in range(width):
            value = 0xFF;            
            gray1 = Qt.qGray(image.pixel(x, y))
            gray2 = Qt.qGray(image.pixel(x, y+1))
            for i in range(4):
                if (gray1 & (1<<(4+i))):
                    value -= 1<<i
                if (gray2 & (1<<(4+i))):
                    value -= 1<<(4+i)                    
            f.write("0x%02x," % value)
        f.write("\n")
elif what == "03x05":
    for y in range(0, height, 5):
        for x in range(width):
            value = 0
            for z in range(5):
                if image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z
            f.write("0x%02x," % value)
        f.write("\n")        
elif what == "04x06":
    for y in range(0, height, 7):
        for x in range(width):
            value = 0
            for z in range(7):
                if image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z
            if value == 0x7f:
                value = 0xff
            f.write("0x%02x," % value)
        f.write("\n")        
elif what == "05x07":
    for y in range(0, height, 8):
        for x in range(width):
            value = 0
            for z in range(8):
                if image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z
            f.write("0x%02x," % value)
        f.write("\n")
elif what == "08x10":
    for y in range(0, height, 12):
        for x in range(width):
            skip = True
            for l in range(0, 12, 8):
                value = 0
                for z in range(8):
                    if l+z < 12:
                        if image.pixel(x, y+l+z) == Qt.qRgb(0, 0, 0):
                            value += 1 << z
                        else:
                            skip = False
                if skip and l==8:
                    value = 0xff
                f.write("0x%02x," % value)
        f.write("\n")
elif what == "10x14":
    for y in range(0, height, 16):
        for x in range(width):
            for l in range(0, 16, 8):
                value = 0
                for z in range(8):
                    if image.pixel(x, y+l+z) == Qt.qRgb(0, 0, 0):
                        value += 1 << z
                f.write("0x%02x," % value)
        f.write("\n")
elif what == "22x38":
    for y in range(0, height, 40):
        for x in range(width):
            for l in range(0, 40, 8):
                value = 0
                for z in range(8):
                    if image.pixel(x, y+l+z) == Qt.qRgb(0, 0, 0):
                        value += 1 << z
                f.write("0x%02x," % value)
        f.write("\n")        
else:
    print("wrong argument", sys.argv[3])
