#!/usr/bin/env python

import sys
from PyQt4 import Qt, QtGui

image = QtGui.QImage(sys.argv[1])
width, height = image.size().width(), image.size().height()

f = open(sys.argv[2], "w")
lcdwidth = int(sys.argv[3])

if len(sys.argv) > 4:
    what = sys.argv[4]  
else:
    for s in ("03x05", "04x06", "05x07", "08x10", "10x14", "22x38"):
        if s in sys.argv[2]:
            what = s
            break

def writeSize(f, width, height):
    if lcdwidth > 255:
        f.write("%d,%d,%d,%d,\n" % (width%256, width/256, height%256, height/256))
    else:
        f.write("%d,%d,\n" % (width, height))
    
if what == "1bit":
    rows = 1
    if len(sys.argv) > 5:
        rows = int(sys.argv[5])
    writeSize(f, width, height/rows)
    for y in range(0, height, 8):
        for x in range(width):
            value = 0
            for z in range(8):
                if y+z < height and image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z            
            f.write("0x%02x," % value)
        f.write("\n")
elif what == "4/4/4/4":
    colors = []
    f.write("%d,%d,\n" % (width, height))
    for y in range(height):
        for x in range(width):
            pixel = image.pixel(x, y)
            f.write("0x%1x%1x%1x%1x," % (Qt.qAlpha(pixel)/16, Qt.qRed(pixel)/16, Qt.qGreen(pixel)/16, Qt.qBlue(pixel)/16))
        f.write("\n")
elif what == "5/6/5":
    colors = []
    f.write("%d,%d,\n" % (width, height))
    for y in range(height):
        for x in range(width):
            pixel = image.pixel(x, y)
            f.write("0x%04x," % (((Qt.qRed(pixel) >> 3) << 11) + ((Qt.qGreen(pixel) >> 2) << 5) + ((Qt.qBlue(pixel) >> 3) << 0)))
        f.write("\n")
elif what == "4bits":
    colors = []
    writeSize(f, width, height)
    for y in range(0, height, 2):
        for x in range(width):
            value = 0xFF;            
            gray1 = Qt.qGray(image.pixel(x, y))
            if y+1 < height:
                gray2 = Qt.qGray(image.pixel(x, y+1))
            else:
                gray2 = Qt.qRgb(255, 255, 255)
            for i in range(4):
                if (gray1 & (1<<(4+i))):
                    value -= 1<<i
                if (gray2 & (1<<(4+i))):
                    value -= 1<<(4+i)                    
            f.write("0x%02x," % value)
        f.write("\n")
elif what == "8bits":
    colors = []
    writeSize(f, width, height)
    for y in range(height):
        for x in range(width):           
            value = Qt.qGray(image.pixel(x, y))
            value = 0x0f - (value >> 4)
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
    print("wrong argument", sys.argv[4])
    
