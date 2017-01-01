#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from PyQt4 import Qt, QtCore, QtGui
import glob

app = Qt.QApplication(sys.argv)

for f in glob.glob("fonts/*.ttf"):
    QtGui.QFontDatabase.addApplicationFont(f)

chars_en = u""" !"#$%&'()*+,-./0123456789:;<=>?°ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz~|≥→←↑↓↗↘↙↖△"""
chars_fr = u"""éèàîç"""
chars_de = u"""ÄäÖöÜüß"""
chars_cz = u"""ěščřžýáíéňóůúďťĚŠČŘŽÝÁÍÉŇÓÚŮĎŤ"""

COUNT_EXTRA_CHARS = 12

chars_extra = u"".join([chr(1+i) for i in range(COUNT_EXTRA_CHARS)])
chars = chars_en + chars_extra + chars_fr + chars_de + chars_cz


def createFontBitmap(filename, fontname, fontsize, fontbold, foreground, background, coordsfile=True):
    coords = []
    font = QtGui.QFont(fontname)
    font.setPixelSize(fontsize)
    font.setBold(fontbold)
    font.setHintingPreference(QtGui.QFont.PreferNoHinting)
    font.setStyleStrategy(QtGui.QFont.PreferAntialias)
    metrics = QtGui.QFontMetrics(font)

    extraFilename = "fonts/extra_%dpx.png" % fontsize
    extraImage = QtGui.QImage(extraFilename)
    if extraImage.isNull():
        # print("No extra font file", extraFilename)
        extraImage = None
        extraWidth, extraHeight = 0, 0
    else:
        extraWidth, extraHeight = extraImage.size().width() / COUNT_EXTRA_CHARS, extraImage.size().height()

    def getCharWidth(c):
        if c in chars_extra:
            return extraWidth
        elif c == " ":
            return 4
        elif c == "." and fontsize <= 8:
            return 1
        else:
            r = metrics.boundingRect(c)
            if fontsize >= 24:
                return r.width() + 3
            else:
                return r.width() + r.left() + 1

    def getFontWidth():
        width = 0
        for c in chars:
            width += getCharWidth(c)
        return width

    def getFontTopBottom():
        top, bottom = 0, 0
        for i, c in enumerate(chars):
            r = metrics.boundingRect(chars[i])
            top = min(top, r.top())
            bottom = max(bottom, r.bottom())
        return top, bottom

    width = getFontWidth()
    top, bottom = getFontTopBottom()
    image = QtGui.QImage(width, fontsize + 4, QtGui.QImage.Format_RGB32)
    image.fill(background)
    painter = QtGui.QPainter()
    painter.begin(image)
    painter.setFont(font)
    pen = QtGui.QPen(foreground)
    painter.setPen(pen)
    width = 0
    for c in chars:
        coords.append(width)
        painter.setOpacity(1.0)
        if c in chars_extra:
            if extraImage and c == chars_extra[0]:
                for x in range(extraWidth * COUNT_EXTRA_CHARS):
                    for y in range(extraHeight):
                        rgb = extraImage.pixel(x, y)
                        painter.setOpacity(1.0 - float(QtGui.qGray(rgb)) / 256)
                        painter.drawPoint(x+width, y)
        elif c == " ":
            pass
        elif c == "." and fontsize <= 8:
            painter.drawPoint(width, fontsize)
        elif (c == u"↑" or c == u"↓") and fontsize == 16:
            rect = metrics.boundingRect(c)
            painter.drawText(width - 1, fontsize, c)  # fontsize-bottom+1 -17 / 7
        else:
            rect = metrics.boundingRect(c)
            if fontsize >= 24:
                painter.drawText(width - rect.left() + 1, fontsize - 2, c)  # fontsize-bottom+1 -17 / 7
            else:
                painter.drawText(width + 1, fontsize, c)  # fontsize-bottom+1 -17 / 7
        width += getCharWidth(c)

    coords.append(width)
    painter.end()
    image.save(filename + ".png")
    if coordsfile:
        with open(filename + ".specs", "w") as f:
            f.write(",".join(str(tmp) for tmp in coords))

    return coords

if __name__ == "__main__":
    createFontBitmap(sys.argv[4], sys.argv[1], float(sys.argv[2]), sys.argv[3] == "True", QtGui.QColor(0x00, 0x00, 0x00), QtGui.QColor(0xFF, 0xFF, 0xFF))
