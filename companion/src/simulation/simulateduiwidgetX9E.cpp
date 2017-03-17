/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "simulateduiwidget.h"
#include "ui_simulateduiwidgetX9E.h"

SimulatedUIWidgetX9E::SimulatedUIWidgetX9E(SimulatorInterface *simulator, QWidget * parent):
  SimulatedUIWidget(simulator, parent),
  ui(new Ui::SimulatedUIWidgetX9E)
{
  RadioUiAction * act;
  QPolygon polygon;

  ui->setupUi(this);

  // add actions in order of appearance on the help menu

  QPoint ctr(70, 100);

  polygon << polyArc(ctr.x(), ctr.y(), 60, -90, 0) << ctr;
  act = addRadioUiAction(0, QList<int>() << Qt::Key_PageUp, tr("PG-UP"), tr("[ MENU ]"));
  ui->leftbuttons->addArea(polygon, "X9E/left_menu.png", act);

  polygon.clear();
  polygon << polyArc(ctr.x(), ctr.y(), 45, 0, 180);
  act = addRadioUiAction(3, QList<int>() << Qt::Key_PageDown, tr("PG-DN"), tr("[ PAGE ]"));
  ui->leftbuttons->addArea(polygon, "X9E/left_page.png", act);

  polygon.clear();
  polygon << polyArc(ctr.x(), ctr.y(), 60, 180, 270) << ctr;
  act = addRadioUiAction(1, QList<int>() << Qt::Key_Delete << Qt::Key_Escape << Qt::Key_Backspace, tr("DEL/BKSP/ESC"), tr("[ EXIT ]"));
  ui->leftbuttons->addArea(polygon, "X9E/left_exit.png", act);

  m_keymapHelp.append(keymapHelp_t(tr("WHEEL/PAD SCRL"),  tr("Rotary Selector")));
  m_scrollUpAction = addRadioUiAction(-1, QList<int>() << Qt::Key_Minus << Qt::Key_Equal << Qt::Key_Up, tr("-/UP"), tr("Rotary UP"));
  m_scrollDnAction = addRadioUiAction(-1, QList<int>() << Qt::Key_Plus << Qt::Key_Down, tr("+/DN"), tr("Rotary DOWN"));
  connectScrollActions();

  act = addRadioUiAction(2, QList<int>() << Qt::Key_Enter << Qt::Key_Return, tr("ENTER/MOUSE-MID"), tr("Selector Press"));
  ui->rightbuttons->addArea(polyArc(90, 100, 50), "X9E/right_enter.png", act);

  ui->leftbuttons->addArea(10, 170, 40, 200, "X9E/left_scrnshot.png", m_screenshotAction);

  m_backlightColors << QColor(47, 123, 227);  // Taranis Blue
  m_backlightColors << QColor(166,247,159);
  m_backlightColors << QColor(247,159,166);
  m_backlightColors << QColor(255,195,151);
  m_backlightColors << QColor(247,242,159);

  setLcd(ui->lcd);

  QString css = "#radioUiWidget {"
                  "background-color: qlineargradient(spread:pad, x1:1, y1:0.9, x2:0, y2:0,"
                    "stop:0 rgba(255, 255, 255, 255),"
                    "stop:0.35 rgba(250, 250, 250, 255), stop:0.5 rgba(242, 241, 241, 255),"
                    "stop:0.61 rgba(241, 241, 241, 255), stop:1.0 rgba(251, 251, 251, 255));"
                "}";

  QTimer * tim = new QTimer(this);
  tim->setSingleShot(true);
  connect(tim, &QTimer::timeout, [this, css]() {
    emit customStyleRequest(css);
  });
  tim->start(100);
}

SimulatedUIWidgetX9E::~SimulatedUIWidgetX9E()
{
  delete ui;
}

