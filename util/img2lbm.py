#!/usr/bin/env python

import sys
from PyQt4 import Qt, QtGui

image = QtGui.QImage(sys.argv[1])
width, height = image.size().width(), image.size().height()

f = open(sys.argv[2], "w")

if sys.argv[3] == "largimg":
    rows = 1
    if len(sys.argv) > 4:
        rows = int(sys.argv[4])
    f.write("255,%d, %d,\n" % (width-255, height/rows))
    for y in range(0, height, 8):
        for x in range(width):
            value = 0
            for z in range(8):
                if y+z < height and image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z            
            f.write("0x%02x," % value)
        f.write("\n")
elif sys.argv[3] == "img":
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
elif sys.argv[3] == "bmp":
    rows = 1
    colors = []
    if len(sys.argv) > 4:
        rows = int(sys.argv[4])
    f.write("%d,%d,\n" % (width, height/rows))
    for y in range(0, height, 8):
        for x in range(width):
            values = [255, 255, 255, 255]
            for z in range(8):
                if y+z < height:
                    gray = Qt.qGray(image.pixel(x, y+z))
                    for i in range(4):
                        if (gray & (1<<(4+i))):
                            values[i] -= 1 << z
            for value in values:
                f.write("0x%02x," % value)
        f.write("\n")
elif sys.argv[3] == "3x5":
    for y in range(0, height, 5):
        for x in range(width):
            value = 0
            for z in range(5):
                if image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z
            f.write("0x%02x," % value)
        f.write("\n")        
elif sys.argv[3] == "4x6":
    for y in range(0, height, 7):
        for x in range(width):
            value = 0
            for z in range(7):
                if image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z
            f.write("0x%02x," % value)
        f.write("\n")        
elif sys.argv[3] == "5x7":
    for y in range(0, height, 8):
        for x in range(width):
            value = 0
            for z in range(8):
                if image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z
            f.write("0x%02x," % value)
        f.write("\n")
elif sys.argv[3] == "8x10":
    for y in range(0, height, 12):
        for x in range(width):
            for l in range(0, 12, 8):
                value = 0
                for z in range(8):
                    if l+z < 12 and image.pixel(x, y+l+z) == Qt.qRgb(0, 0, 0):
                        value += 1 << z
                f.write("0x%02x," % value)
        f.write("\n")
elif sys.argv[3] == "10x14":
    for y in range(0, height, 16):
        for x in range(width):
            for l in range(0, 16, 8):
                value = 0
                for z in range(8):
                    if image.pixel(x, y+l+z) == Qt.qRgb(0, 0, 0):
                        value += 1 << z
                f.write("0x%02x," % value)
        f.write("\n")
else:
    print "wrong argument", sys.argv[3]
