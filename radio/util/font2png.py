#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from PyQt4 import Qt, QtGui
from wx import Width

chars = u""" !"#$%&'()*+, ./0123456789:;<=>?°ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}  """

font = sys.argv[1]
size = int(sys.argv[2])
filename = sys.argv[3]

app = QtGui.QApplication(sys.argv)
font = QtGui.QFont(font)
font.setBold(True)
font.setPointSize(size)
metrics = QtGui.QFontMetrics(font)

width = 0
for c in chars:
    rect = metrics.boundingRect(c)
    if rect.width() > width:
        width = rect.width()

image = QtGui.QImage(width*len(chars), size+4, QtGui.QImage.Format_RGB32)
image.fill(0xffffff)
painter = QtGui.QPainter()
painter.begin(image)
painter.setFont(font)
for i, c in enumerate(chars):
    rect = metrics.boundingRect(c)
    if i == 0:
        rect.setWidth(4)
    painter.drawText(width*i-rect.left(), size+1, c)
    for j in range(rect.width(), width):
        painter.drawLine(width*i+j, 0, width*i+j, width)
painter.end()
image.save(filename)
