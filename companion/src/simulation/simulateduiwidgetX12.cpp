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

  act = addRadioUiAction(0, QList<int>() << Qt::Key_PageUp, tr("PG-UP"), tr("[ PgUp ]"));
  polygon.setPoints(19, 29,232, 23,225, 19,218, 16,210, 14,200, 13,190, 14,179, 17,170, 20,162, 25,155, 30,149, 43,164, 37,172, 34,181, 33,190, 34,200, 37,209, 41,215, 44,219);
  ui->leftbuttons->addArea(polygon, "Horus/left_btn1.png", act);

  act = addRadioUiAction(1, QList<int>() << Qt::Key_PageDown, tr("PG-DN"), tr("[ PgDn ]"));
  polygon.setPoints(22, 32,234, 46,220, 52,225, 59,228, 68,230, 75,230, 83,229, 90,227, 95,224, 101,220, 114,234, 109,239, 103,242, 98,245, 91,248, 84,249, 77,250, 69,250, 60,249, 51,247, 44,243, 38,239);
  ui->leftbuttons->addArea(polygon, "Horus/left_btn2.png", act);

  act = addRadioUiAction(3, QList<int>() << Qt::Key_Up, tr("UP"), tr("[ MDL ]"));
  polygon.setPoints(20, 44,159, 30,144, 35,140, 43,135, 52,131, 62,128, 72,128, 80,128, 90,130, 97,132, 103,136, 110,140, 116,145, 101,158, 95,154, 85,150, 76,148, 67,148, 57,151, 49,154);
  ui->rightbuttons->addArea(polygon, "Horus/right_btnU.png", act);

  act = addRadioUiAction(4, QList<int>() << Qt::Key_Down << Qt::Key_Delete << Qt::Key_Escape << Qt::Key_Backspace, tr("DN/DEL/BKSP"), tr("[ RTN ]"));
  polygon.setPoints(19, 30,236, 44,223, 52,228, 59,231, 67,233, 75,233, 82,233, 89,231, 95,228, 99,225, 103,223, 115,235, 108,241, 100,246, 89,250, 79,252, 67,252, 54,250, 42,245);
  ui->rightbuttons->addArea(polygon, "Horus/right_btnD.png", act);

  act = addRadioUiAction(6, QList<int>() << Qt::Key_Left, tr("LEFT"), tr("[ SYS ]"));
  polygon.setPoints(18, 27,233, 21,226, 17,217, 13,207, 11,192, 11,176, 15,166, 19,158, 24,152, 27,147, 41,161, 35,169, 31,181, 31,191, 31,201, 34,207, 37,215, 41,220);
  ui->rightbuttons->addArea(polygon, "Horus/right_btnL.png", act);

  act = addRadioUiAction(5, QList<int>() << Qt::Key_Right, tr("RIGHT"), tr("[ TELE ]"));
  polygon.setPoints(21, 105,160, 119,147, 126,156, 131,165, 134,177, 136,185, 135,196, 134,205, 131,213, 128,220, 124,226, 119,233, 105,219, 109,212, 113,205, 115,196, 116,189, 115,181, 112,174, 108,166, 104,161);
  ui->rightbuttons->addArea(polygon, "Horus/right_btnR.png", act);

  m_keymapHelp.append(keymapHelp_t(tr("WHEEL/PAD SCRL"),  tr("Rotary Selector")));

  m_scrollUpAction = addRadioUiAction(-1, QList<int>() << Qt::Key_Minus << Qt::Key_X, tr("-/X"), tr("Rotary UP"));
  m_scrollDnAction = addRadioUiAction(-1, QList<int>() << Qt::Key_Plus << Qt::Key_Equal << Qt::Key_C, tr("+/C"), tr("Rotary DOWN"));

  act = addRadioUiAction(2, QList<int>() << Qt::Key_Enter << Qt::Key_Return, tr("ENTER/MOUSE-MID"), tr("Selector Press"));
  polygon.setPoints(30, 33,189, 35,177, 40,168, 44,161, 52,155, 60,152, 70,149, 82,150, 91,154, 98,158, 105,164, 110,173, 113,181, 114,190, 113,198, 111,204, 108,210, 105,215, 101,220, 96,223, 91,225, 86,228, 80,229, 72,230, 65,229, 56,226, 48,222, 42,216, 37,210, 34,200);
  ui->rightbuttons->addArea(polygon, "Horus/right_ent.png", act);

  ui->leftbuttons->addArea(9, 259, 34, 282, "Horus/left_scrnsht.png", m_screenshotAction);

  m_lcd = ui->lcd;
  m_lcd->setData(simulator->getLcd(), 480, 272, 16);
  m_lcd->setBackgroundColor(47, 123, 227);

  connectScrollActions();

}

SimulatedUIWidgetX12::~SimulatedUIWidgetX12()
{
  delete ui;
}
