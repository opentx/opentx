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
#include "ui_simulateduiwidgetX12.h"

SimulatedUIWidgetX12::SimulatedUIWidgetX12(SimulatorInterface *simulator, SimulatorDialog * simuDialog, QWidget * parent):
  SimulatedUIWidget(simulator, simuDialog, parent),
  ui(new Ui::SimulatedUIWidgetX12)
{
  RadioUiAction * act;
  QPolygon polygon;

  ui->setupUi(this);

  // add actions in order of appearance on the help menu

  int x = 74, y = 190, oR = 63, iR = 40;

  polygon << polyArc(x, y, oR, 225, 315) << polyArc(x, y, iR, 225, 315);
  act = addRadioUiAction(0, QList<int>() << Qt::Key_PageUp, tr("PG-UP"), tr("[ PgUp ]"));
  ui->leftbuttons->addArea(polygon, "Horus/left_btn1.png", act);

  polygon.clear();
  polygon << polyArc(x, y, oR, 135, 225) << polyArc(x, y, iR, 135, 225);
  act = addRadioUiAction(1, QList<int>() << Qt::Key_PageDown, tr("PG-DN"), tr("[ PgDn ]"));
  ui->leftbuttons->addArea(polygon, "Horus/left_btn2.png", act);

  polygon.clear();
  polygon << polyArc(x, y, oR, -45, 45) << polyArc(x, y, iR, -45, 45);
  act = addRadioUiAction(3, QList<int>() << Qt::Key_Up, tr("UP"), tr("[ MDL ]"));
  ui->rightbuttons->addArea(polygon, "Horus/right_btnU.png", act);

  polygon.clear();
  polygon << polyArc(x, y, oR, 135, 225) << polyArc(x, y, iR, 135, 225);
  act = addRadioUiAction(4, QList<int>() << Qt::Key_Down << Qt::Key_Delete << Qt::Key_Escape << Qt::Key_Backspace, tr("DN/DEL/BKSP"), tr("[ RTN ]"));
  ui->rightbuttons->addArea(polygon, "Horus/right_btnD.png", act);

  polygon.clear();
  polygon << polyArc(x, y, oR, 225, 315) << polyArc(x, y, iR, 225, 315);
  act = addRadioUiAction(6, QList<int>() << Qt::Key_Left, tr("LEFT"), tr("[ SYS ]"));
  ui->rightbuttons->addArea(polygon, "Horus/right_btnL.png", act);

  polygon.clear();
  polygon << polyArc(x, y, oR, 45, 135) << polyArc(x, y, iR, 45, 135);
  act = addRadioUiAction(5, QList<int>() << Qt::Key_Right, tr("RIGHT"), tr("[ TELE ]"));
  ui->rightbuttons->addArea(polygon, "Horus/right_btnR.png", act);

  m_keymapHelp.append(keymapHelp_t(tr("WHEEL/PAD SCRL"),  tr("Rotary Selector")));

  m_scrollUpAction = addRadioUiAction(-1, QList<int>() << Qt::Key_Minus << Qt::Key_X, tr("-/X"), tr("Rotary UP"));
  m_scrollDnAction = addRadioUiAction(-1, QList<int>() << Qt::Key_Plus << Qt::Key_Equal << Qt::Key_C, tr("+/C"), tr("Rotary DOWN"));

  act = addRadioUiAction(2, QList<int>() << Qt::Key_Enter << Qt::Key_Return, tr("ENTER/MOUSE-MID"), tr("Selector Press"));
  ui->rightbuttons->addArea(polyArc(x, y, iR), "Horus/right_ent.png", act);

  ui->leftbuttons->addArea(9, 259, 34, 282, "Horus/left_scrnsht.png", m_screenshotAction);

  m_backlightColors << QColor(47, 123, 227);

  setLcd(ui->lcd);
  connectScrollActions();
}

SimulatedUIWidgetX12::~SimulatedUIWidgetX12()
{
  delete ui;
}
