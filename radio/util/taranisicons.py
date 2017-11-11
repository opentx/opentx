#!/usr/bin/env python
# -*- coding: utf-8 -*-
try:
    from PyQt5 import Qt, QtGui
except:
    from PyQt4 import Qt, QtGui

menu = ["radio_setup", "model_select", "model_settings", "model_checklist", "files", "version"]

image = QtGui.QImage(24 * 8, 24, QtGui.QImage.Format_RGB32)
image.fill(QtGui.QColor(255, 255, 255))
painter = QtGui.QPainter()
painter.begin(image)
for i, str in enumerate(menu):
    icon = QtGui.QImage("../src/bitmaps/taranis/std/%s.png" % str)
    painter.drawImage(Qt.QPoint(24 * i, 0), icon)
painter.end()
image.save("../src/bitmaps/taranis/mainmenu.png")
