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
#include "ui_simulateduiwidgetX9.h"

SimulatedUIWidgetX9::SimulatedUIWidgetX9(SimulatorInterface *simulator, SimulatorDialog * simuDialog, QWidget * parent):
  SimulatedUIWidget(simulator, simuDialog, parent),
  ui(new Ui::SimulatedUIWidgetX9)
{
  RadioUiAction * act;
  QPolygon polygon;

  ui->setupUi(this);

  //getCurrentFirmware()->getCapability(Capability(RotaryEncoders));  // not working for X9E ?
  bool rotaryOnly = IS_TARANIS_X9E(m_board);

  // add actions in order of appearance on the help menu

  act = addRadioUiAction(0, QList<int>() << Qt::Key_PageUp, tr("PG-UP"), tr("[ MENU ]"));
  polygon.setPoints(6, 20, 59, 27, 50, 45, 52, 56, 59, 50, 71, 26, 72);
  ui->leftbuttons->addArea(polygon, "Taranis/x9l1.png", act);

  act = addRadioUiAction(3, QList<int>() << Qt::Key_PageDown, tr("PG-DN"), tr("[ PAGE ]"));
  polygon.setPoints(6, 23, 107, 30, 99, 46, 100, 55, 106, 47, 117, 28, 117);
  ui->leftbuttons->addArea(polygon, "Taranis/x9l2.png", act);

  act = addRadioUiAction(1, QList<int>() << Qt::Key_Delete << Qt::Key_Escape << Qt::Key_Backspace, tr("DEL/BKSP/ESC"), tr("[ EXIT ]"));
  polygon.setPoints(6, 24, 154, 32, 144, 46, 146, 57, 156, 46, 167, 29, 166);
  ui->leftbuttons->addArea(polygon, "Taranis/x9l3.png", act);

  if (rotaryOnly) {
    m_keymapHelp.append(keymapHelp_t(tr("WHEEL/PAD SCRL"),  tr("Rotary Selector")));
    m_scrollUpAction = addRadioUiAction(-1, QList<int>() << Qt::Key_Minus << Qt::Key_Equal << Qt::Key_Up, tr("-/UP"), tr("Rotary UP"));
    m_scrollDnAction = addRadioUiAction(-1, QList<int>() << Qt::Key_Plus << Qt::Key_Down, tr("+/DN"), tr("Rotary DOWN"));
    connectScrollActions();
  }
  else {
    m_scrollUpAction = addRadioUiAction(4, QList<int>() << Qt::Key_Plus << Qt::Key_Equal << Qt::Key_Up, tr("+/UP"), tr("[ + ]"));
    m_scrollDnAction = addRadioUiAction(5, QList<int>() << Qt::Key_Minus << Qt::Key_Down, tr("-/DN"), tr("[ - ]"));
  }

  polygon.setPoints(6, 64, 60, 71, 50, 90, 50, 100, 60, 90, 73, 72, 73);
  ui->rightbuttons->addArea(polygon, "Taranis/x9r1.png", m_scrollUpAction);

  polygon.setPoints(6, 63, 109, 73, 100, 88, 100, 98, 109, 88, 119, 72, 119);
  ui->rightbuttons->addArea(polygon, "Taranis/x9r2.png", m_scrollDnAction);

  act = addRadioUiAction(2, QList<int>() << Qt::Key_Enter << Qt::Key_Return, tr("ENTER/MOUSE-MID"), (rotaryOnly ? tr("Selector Press") : tr("[ ENT ]")));
  polygon.setPoints(6, 63, 155, 72, 146, 90, 146, 98, 155, 88, 166, 72, 166);
  ui->rightbuttons->addArea(polygon, "Taranis/x9r3.png", act);

  ui->leftbuttons->addArea(90, 177, 118, 197, "Taranis/x9l4.png", m_screenshotAction);

  if (!rotaryOnly)
    m_keymapHelp.append(keymapHelp_t(tr("WHEEL/PAD SCRL"),  tr("[ + ]/[ - ]")));

  m_backlightColors << QColor(47, 123, 227);  // Taranis Blue
  m_backlightColors << QColor(166,247,159);
  m_backlightColors << QColor(247,159,166);
  m_backlightColors << QColor(255,195,151);
  m_backlightColors << QColor(247,242,159);

  setLcd(ui->lcd);
}

SimulatedUIWidgetX9::~SimulatedUIWidgetX9()
{
  delete ui;
}

