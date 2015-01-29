#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from PyQt4 import Qt, QtGui

chars = u""" !"#$%&'()*+,-./0123456789:;<=>?°ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz~ ≥→←↑↓    △Ⓘ"""

app = QtGui.QApplication(sys.argv)

def getCharWidth(metrics):
    width = 0
    for c in chars:
        rect = metrics.boundingRect(c)
        if rect.width() > width:
            width = rect.width()
    return width

def createFontBitmap(filename, fontname, fontsize, foreground, background):
    font = QtGui.QFont(fontname)
    font.setPointSize(fontsize)
    metrics = QtGui.QFontMetrics(font)
    width = getCharWidth(metrics)
    image = QtGui.QImage(width*len(chars), fontsize+4, QtGui.QImage.Format_RGB32)
    image.fill(background)
    painter = QtGui.QPainter()
    painter.begin(image)
    painter.setFont(font)
    pen = QtGui.QPen(foreground)
    painter.setPen(pen)
    for i, c in enumerate(chars):
        rect = metrics.boundingRect(c)
        if i == 0:
            rect.setWidth(4)
        elif i == 14:
            rect.setWidth(1)
            painter.drawPoint(width*i, fontsize);
        else:
            painter.drawText(width*i-rect.left(), fontsize+1, c)
        for j in range(rect.width(), width):
            painter.drawLine(width*i+j, 0, width*i+j, fontsize+4)
    painter.end()
    image.save(filename)

if len(sys.argv) == 4:
    createFontBitmap(sys.argv[3], sys.argv[1], int(sys.argv[2]), QtGui.QColor(0x00, 0x00, 0x00), QtGui.QColor(0xFF, 0xFF, 0xFF))
