#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from PyQt4 import Qt, QtGui

try:
  app = Qt.QApplication(sys.argv)
except:
  pass

chars = u""" !"#$%&'()*+,-./0123456789:;<=>?°ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz~ ≥→←↑↓    △Ⓘ"""

def getCharWidth(fontsize, metrics, index):
    if index == 0:
        return 4
    elif index == 14 and fontsize <= 8:
        return 1
    else:
        rect = metrics.boundingRect(chars[index])
        return rect.width()
    
def getFontWidth(fontsize, metrics):
    width = 0
    for i, c in enumerate(chars):
        width += getCharWidth(fontsize, metrics, i)
    return width
    
def createFontBitmap(filename, fontname, fontsize, foreground, background, coordsfile=True):
    coords = [ ]
    font = QtGui.QFont(fontname)
    font.setPointSize(fontsize)
    metrics = QtGui.QFontMetrics(font)
    width = getFontWidth(fontsize, metrics)
    image = QtGui.QImage(width, fontsize+4, QtGui.QImage.Format_RGB32)
    image.fill(background)
    painter = QtGui.QPainter()
    painter.begin(image)
    painter.setFont(font)
    pen = QtGui.QPen(foreground)
    painter.setPen(pen)
    width = 0
    for i, c in enumerate(chars):
        coords.append(width)
        if i == 0:
            pass
        elif i == 14 and fontsize <= 8:
            painter.drawPoint(width, fontsize);
        else:
            rect = metrics.boundingRect(c)
            painter.drawText(width-rect.left(), fontsize+1, c)
        width += getCharWidth(fontsize, metrics, i)
    coords.append(width)
    painter.end()
    image.save(filename + ".png")
    if coordsfile:
      f = file(filename + ".specs", "w")
      f.write("{ ")
      f.write(",".join(str(tmp) for tmp in coords))
      f.write(" }")
      f.close()      
    return coords

if len(sys.argv) == 4:
    createFontBitmap(sys.argv[3], sys.argv[1], int(sys.argv[2]), QtGui.QColor(0x00, 0x00, 0x00), QtGui.QColor(0xFF, 0xFF, 0xFF))
