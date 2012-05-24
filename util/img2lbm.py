#!/usr/bin/env python

import sys
from PyQt4 import Qt, QtGui

image = QtGui.QImage(sys.argv[1])
width, height = image.size().width(), image.size().height()

f = file(sys.argv[2], "w")

if sys.argv[3] == "img":
    rows = 1
    if len(sys.argv) > 4:
        rows = int(sys.argv[4])
    f.write("%d,%d,0,\n" % (width, height/rows))
    for y in range(0, height, 8):
        for x in range(width):
            value = 0
            for z in range(8):
                if image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z
            f.write("0x%02x," % value)
        f.write("\n")

elif sys.argv[3] == "imgc":
    rows = 1
    if len(sys.argv) > 4:
        rows = int(sys.argv[4])
    f.write("%d,%d,1,\n" % (width, height/rows))
    val=0
    ocur=0
    for y in range(0, height, 8):
        for x in range(width):
            value = 0
            for z in range(8):
                if image.pixel(x, y+z) == Qt.qRgb(0, 0, 0):
                    value += 1 << z
            if (val==value) & (ocur<255):
		ocur+=1
	    else:
		if ocur>1:
		    if ocur>2:
			f.write("0xf0,0x%02x,0x%02x," % (val,ocur))
		    else:
			f.write("0x%02x,0x%02x," % (val,val))
		else:
		    if val!=0xf0:
			f.write("0x%02x," % val)
		    else:
			f.write("0xf0,0xf0,0x01,")
		val=value
		ocur=1
        f.write("\n")
    if ocur>0:
	if ocur>1: f.write("0xf0,0x%02x,0x%02x," % (val,ocur))
	else:
	    if val!=0xf0: f.write("0x%02x," % val)
	    else: f.write("0xf0,0xf0,0x01,")
        
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
