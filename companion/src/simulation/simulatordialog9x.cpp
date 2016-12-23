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

#include "ui_simulatordialog-9x.h"

uint32_t SimulatorDialog9X::switchstatus = 0;

SimulatorDialog9X::SimulatorDialog9X(QWidget * parent, SimulatorInterface *simulator, unsigned int flags):
  SimulatorDialog(parent, simulator, flags),
  ui(new Ui::SimulatorDialog9X),
  beepShow(0)
{
  QPolygon polygon;

  lcdWidth = 128;
  lcdHeight = 64;
  lcdDepth = 1;

  initUi<Ui::SimulatorDialog9X>(ui);

  polygon.setPoints(6, 68, 83, 28, 45, 51, 32, 83, 32, 105, 45, 68, 83);
  ui->leftbuttons->addArea(polygon, Qt::Key_Up, "9X/9xcursup.png");
  polygon.setPoints(6, 74, 90, 114, 51, 127, 80, 127, 106, 114, 130, 74, 90);
  ui->leftbuttons->addArea(polygon, Qt::Key_Right, "9X/9xcursmin.png");
  polygon.setPoints(6, 68, 98, 28, 137, 51, 151, 83, 151, 105, 137, 68, 98);
  ui->leftbuttons->addArea(polygon, Qt::Key_Down, "9X/9xcursdown.png");
  polygon.setPoints(6, 80, 90, 20, 51, 7, 80, 7, 106, 20, 130, 80, 90);
  ui->leftbuttons->addArea(polygon, Qt::Key_Left, "9X/9xcursplus.png");
  ui->leftbuttons->addArea(5, 148, 39, 182, Qt::Key_Print, "9X/9xcursphoto.png");

  ui->rightbuttons->addArea(25, 60, 71, 81, Qt::Key_Enter, "9X/9xmenumenu.png");
  ui->rightbuttons->addArea(25, 117, 71, 139, Qt::Key_Escape, "9X/9xmenuexit.png");

  // install simulator TRACE hook
  simulator->installTraceHook(traceCb);

  backLight = g.backLight();
  if (backLight > 4) backLight = 0;
  switch (backLight) {
    case 1:
      ui->lcd->setBackgroundColor(166,247,159);
      break;
    case 2:
      ui->lcd->setBackgroundColor(247,159,166);
      break;
    case 3:
      ui->lcd->setBackgroundColor(255,195,151);
      break;
    case 4:
      ui->lcd->setBackgroundColor(247,242,159);
      break;
    default:
      ui->lcd->setBackgroundColor(159,165,247);
      break;
  }

  //restore switches
  if (g.simuSW())
    restoreSwitches();

  for (int i=0; i<pots.count(); i++) {
    pots[i]->setProperty("index", i);
    connect(pots[i], SIGNAL(valueChanged(int)), this, SLOT(dialChanged(int)));
  }
  connect(ui->leftbuttons, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->rightbuttons, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
}

SimulatorDialog9X::~SimulatorDialog9X()
{
  saveSwitches();
  delete ui;
}

void SimulatorDialog9X::dialChanged(int value)
{
  int index = sender()->property("index").toInt();
  potValues[index]->setText(QString("%1 %").arg((value*100)/1024));
}

void SimulatorDialog9X::setLightOn(bool enable)
{
  QString bg = "";
  if (enable) {
    QStringList list;
    list << "bl" << "gr" << "rd" << "or" << "yl";
    bg = QString("-") + list[backLight];
  }
  ui->top->setStyleSheet(QString("background:url(:/images/simulator/9X/9xdt%1.png);").arg(bg));
  ui->bottom->setStyleSheet(QString("background:url(:/images/simulator/9X/9xdb%1.png);").arg(bg));
  ui->left->setStyleSheet(QString("background:url(:/images/simulator/9X/9xdl%1.png);").arg(bg));
  ui->right->setStyleSheet(QString("background:url(:/images/simulator/9X/9xdr%1.png);").arg(bg));
}

void SimulatorDialog9X::updateBeepButton()
{
  #define CBEEP_ON  "QLabel { background-color: #FF364E }"
  #define CBEEP_OFF "QLabel { }"

  if (beepVal) {
    beepShow = 20;
  }

  ui->label_beep->setStyleSheet(beepShow ? CBEEP_ON : CBEEP_OFF);

  if (beepShow) {
    beepShow--;
  }
}


void SimulatorDialog9X::getValues()
{
  TxInputs inputs = {
    {
      int(1024 * vJoyLeft->getStickX()),    // LEFT HORZ
      int(-1024 * vJoyLeft->getStickY()),   // LEFT VERT
      int(-1024 * vJoyRight->getStickY()),  // RGHT VERT
      int(1024 * vJoyRight->getStickX())    // RGHT HORZ
    },

    {
      pots[0]->value(),
      pots[1]->value(),
      pots[2]->value()
    },

    {
      ui->switchTHR->isChecked(),
      ui->switchRUD->isChecked(),
      ui->switchELE->isChecked(),
      ui->switchID2->isChecked() ? 1 : (ui->switchID1->isChecked() ? 0 : -1),
      ui->switchAIL->isChecked(),
      ui->switchGEA->isChecked(),
      ui->switchTRN->isDown(),
      0, 0, 0
    },

    {
      buttonPressed == Qt::Key_Enter,
      buttonPressed == Qt::Key_Escape,
      buttonPressed == Qt::Key_Down,
      buttonPressed == Qt::Key_Up,
      buttonPressed == Qt::Key_Right,
      buttonPressed == Qt::Key_Left,
    },

    middleButtonPressed,

    {
      trimPressed == TRIM_LH_L,
      trimPressed == TRIM_LH_R,
      trimPressed == TRIM_LV_DN,
      trimPressed == TRIM_LV_UP,
      trimPressed == TRIM_RV_DN,
      trimPressed == TRIM_RV_UP,
      trimPressed == TRIM_RH_L,
      trimPressed == TRIM_RH_R
    }
  };

  simulator->setValues(inputs);
}

void SimulatorDialog9X::saveSwitches(void)
{
  // qDebug() << "SimulatorDialog9X::saveSwitches()";
  switchstatus=ui->switchTHR->isChecked();
  switchstatus<<=1;
  switchstatus+=(ui->switchRUD->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchID2->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchID1->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchID0->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchGEA->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchELE->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchAIL->isChecked()&0x1);
}

void SimulatorDialog9X::restoreSwitches(void)
{
  // qDebug() << "SimulatorDialog9X::restoreSwitches()";
  ui->switchAIL->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchELE->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchGEA->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchID0->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchID1->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchID2->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchRUD->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchTHR->setChecked(switchstatus & 0x1);
}

