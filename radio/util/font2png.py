#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from PyQt4 import Qt, QtCore, QtGui
import glob

app = Qt.QApplication(sys.argv)

for f in glob.glob("fonts/*.ttf"):
    QtGui.QFontDatabase.addApplicationFont(f)

chars = u""" !"#$%&'()*+,-./0123456789:;<=>?°ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz~ ≥→←↑↓↗↘↙↖△"""

def getCharWidth(fontsize, metrics, index):
    if index == 0:
        return 4
    elif index == 14 and fontsize <= 8:
        return 1
    else:
        rect = metrics.boundingRect(chars[index])
        if fontsize >= 24:
            return rect.width() + 3
        else:
            return rect.width() + 1

def getFontTopBottom(fontsize, metrics):
    top, bottom = 0, 0
    for i, c in enumerate(chars):
         rect = metrics.boundingRect(chars[i])
         top = min(top, rect.top())
         bottom = max(bottom, rect.bottom())
    return top, bottom

def getFontWidth(fontsize, metrics):
    width = 0
    for i, c in enumerate(chars):
        width += getCharWidth(fontsize, metrics, i)
    return width
    
def createFontBitmap(filename, fontname, fontsize, fontbold, foreground, background, coordsfile=True):
    coords = [ ]
    font = QtGui.QFont(fontname)
    font.setPixelSize(fontsize)
    font.setBold(fontbold)
    font.setHintingPreference(QtGui.QFont.PreferNoHinting)
    font.setStyleStrategy(QtGui.QFont.PreferAntialias)
    metrics = QtGui.QFontMetrics(font)
    
    width = getFontWidth(fontsize, metrics)
    top, bottom = getFontTopBottom(fontsize, metrics)
    extraFilename = "fonts/extra_%dpx.png" % fontsize
    extraImage = QtGui.QImage(extraFilename)
    if extraImage.isNull():
        print "No extra font file", extraFilename
        extraWidth = 0
    else:
        extraWidth = extraImage.size().width()
    image = QtGui.QImage(width + extraWidth, fontsize+2, QtGui.QImage.Format_RGB32)
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
            if fontsize >= 24:
                painter.drawText(width-rect.left()+1, fontsize-2, c) # fontsize-bottom+1 -17 / 7
            else:
                painter.drawText(width-rect.left(), fontsize-1, c) # fontsize-bottom+1 -17 / 7
        width += getCharWidth(fontsize, metrics, i)
    if extraWidth:
        painter.drawImage(QtCore.QPoint(width, 0), extraImage)                             
    
    coords.append(width)
    painter.end()
    image.save(filename + ".png")
    if coordsfile:
      f = file(filename + ".specs", "w")
      f.write("{ ")
      f.write(",".join(str(tmp) for tmp in coords))
      if extraWidth:
          for i in range(1, 14):
              f.write(", %d" % (int(coords[-1])+i*(extraWidth/12)))
          # f.write(file("fonts/extra_%dpx.specs" % fontsize).read())
      f.write(" }")
      f.close()      
    return coords

if __name__ == "__main__":
  createFontBitmap(sys.argv[4], sys.argv[1], float(sys.argv[2]), sys.argv[3] == "True", QtGui.QColor(0x00, 0x00, 0x00), QtGui.QColor(0xFF, 0xFF, 0xFF))
