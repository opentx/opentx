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

#include "simulatordialog.h"
#include "ui_simulatordialog-horus.h"
#include "helpers.h"

uint32_t SimulatorDialogHorus::switchstatus = 0;

SimulatorDialogHorus::SimulatorDialogHorus(QWidget * parent, SimulatorInterface * simulator, unsigned int flags):
  SimulatorDialog(parent, simulator, flags),
  ui(new Ui::SimulatorDialogHorus)
{
  QPolygon polygon;

  lcdWidth = 480;
  lcdHeight = 272;
  lcdDepth = 16;

  initUi<Ui::SimulatorDialogHorus>(ui);

  polygon.setPoints(20, 44,159, 30,144, 35,140, 43,135, 52,131, 62,128, 72,128, 80,128, 90,130, 97,132, 103,136, 110,140, 116,145, 101,158, 95,154, 85,150, 76,148, 67,148, 57,151, 49,154);
  ui->rightbuttons->addArea(polygon, Qt::Key_Up, "Horus/right_btnU.png");
  polygon.setPoints(21, 105,160, 119,147, 126,156, 131,165, 134,177, 136,185, 135,196, 134,205, 131,213, 128,220, 124,226, 119,233, 105,219, 109,212, 113,205, 115,196, 116,189, 115,181, 112,174, 108,166, 104,161);
  ui->rightbuttons->addArea(polygon, Qt::Key_Right, "Horus/right_btnR.png");
  polygon.setPoints(19, 30,236, 44,223, 52,228, 59,231, 67,233, 75,233, 82,233, 89,231, 95,228, 99,225, 103,223, 115,235, 108,241, 100,246, 89,250, 79,252, 67,252, 54,250, 42,245);
  ui->rightbuttons->addArea(polygon, Qt::Key_Down, "Horus/right_btnD.png");
  polygon.setPoints(18, 27,233, 21,226, 17,217, 13,207, 11,192, 11,176, 15,166, 19,158, 24,152, 27,147, 41,161, 35,169, 31,181, 31,191, 31,201, 34,207, 37,215, 41,220);
  ui->rightbuttons->addArea(polygon, Qt::Key_Left, "Horus/right_btnL.png");
  polygon.setPoints(30, 33,189, 35,177, 40,168, 44,161, 52,155, 60,152, 70,149, 82,150, 91,154, 98,158, 105,164, 110,173, 113,181, 114,190, 113,198, 111,204, 108,210, 105,215, 101,220, 96,223, 91,225, 86,228, 80,229, 72,230, 65,229, 56,226, 48,222, 42,216, 37,210, 34,200);
  ui->rightbuttons->addArea(polygon, Qt::Key_Enter, "Horus/right_ent.png");
  polygon.setPoints(19, 29,232, 23,225, 19,218, 16,210, 14,200, 13,190, 14,179, 17,170, 20,162, 25,155, 30,149, 43,164, 37,172, 34,181, 33,190, 34,200, 37,209, 41,215, 44,219);
  ui->leftbuttons->addArea(polygon, Qt::Key_PageUp, "Horus/left_btn1.png");
  polygon.setPoints(22, 32,234, 46,220, 52,225, 59,228, 68,230, 75,230, 83,229, 90,227, 95,224, 101,220, 114,234, 109,239, 103,242, 98,245, 91,248, 84,249, 77,250, 69,250, 60,249, 51,247, 44,243, 38,239);
  ui->leftbuttons->addArea(polygon, Qt::Key_Escape, "Horus/left_btn2.png");
  ui->leftbuttons->addArea(9, 259, 34, 282, Qt::Key_Print, "Horus/left_scrnsht.png");

  // install simulator TRACE hook
  simulator->installTraceHook(traceCb);

  // TODO dialP_4 = ui->dialP_4;

  ui->lcd->setBackgroundColor(47, 123, 227);

  //restore switches
  if (g.simuSW())
    restoreSwitches();

  connect(ui->leftbuttons, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->rightbuttons, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->teleSim, SIGNAL(released()), this, SLOT(openTelemetrySimulator()));
  connect(ui->trainerSim, SIGNAL(released()), this, SLOT(openTrainerSimulator()));
  connect(ui->debugConsole, SIGNAL(released()), this, SLOT(openDebugOutput()));
  connect(ui->luaReload, SIGNAL(released()), this, SLOT(luaReload()));
}

SimulatorDialogHorus::~SimulatorDialogHorus()
{
  saveSwitches();
  delete ui;
}

void SimulatorDialogHorus::resetSH()
{
  ui->switchH->setValue(0);
}

void SimulatorDialogHorus::on_switchH_sliderReleased()
{
  QTimer::singleShot(400, this, SLOT(resetSH()));
}

void SimulatorDialogHorus::getValues()
{
  TxInputs inputs = {
    {
      int(1024 * vJoyLeft->getStickX()),    // LEFT HORZ
      int(-1024 * vJoyLeft->getStickY()),   // LEFT VERT
      int(-1024 * vJoyRight->getStickY()),  // RGHT VERT
      int(1024 * vJoyRight->getStickX())    // RGHT HORZ
    },

    {
      -ui->dialP_1->value(),
      ui->dialP_2->value(),
      0,
      -ui->dialP_3->value(),
      ui->dialP_4->value()
    },

    {
      ui->switchA->value() - 1,
      ui->switchB->value() - 1,
      ui->switchC->value() - 1,
      ui->switchD->value() - 1,
      ui->switchE->value() - 1,
      ui->switchF->value(),
      ui->switchG->value() - 1,
      ui->switchH->value(), 0, 0
    },

    {
      buttonPressed == Qt::Key_PageUp,
      buttonPressed == Qt::Key_Escape,
      buttonPressed == Qt::Key_Enter || middleButtonPressed,
      buttonPressed == Qt::Key_Up,
      buttonPressed == Qt::Key_Down,
      buttonPressed == Qt::Key_Right,
      buttonPressed == Qt::Key_Left
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
      trimPressed == TRIM_RH_R,
      0,0,0,0
      /*trimPressed == TRIM_T5_DN,
      trimPressed == TRIM_T5_UP,
      trimPressed == TRIM_T6_DN,
      trimPressed == TRIM_T6_UP*/
    }
  };

  simulator->setValues(inputs);
}

void SimulatorDialogHorus::saveSwitches(void)
{
  // qDebug() << "SimulatorDialogTaranis::saveSwitches()";
  switchstatus=ui->switchA->value();
  switchstatus<<=2;
  switchstatus+=ui->switchB->value();
  switchstatus<<=2;
  switchstatus+=ui->switchC->value();
  switchstatus<<=2;
  switchstatus+=ui->switchD->value();
  switchstatus<<=2;
  switchstatus+=ui->switchE->value();
  switchstatus<<=2;
  switchstatus+=ui->switchF->value();
  switchstatus<<=2;
  switchstatus+=ui->switchG->value();
  switchstatus<<=2;
  switchstatus+=ui->switchH->value();
}

void SimulatorDialogHorus::restoreSwitches(void)
{
  // qDebug() << "SimulatorDialogTaranis::restoreSwitches()";
  ui->switchH->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchG->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchF->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchE->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchD->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchC->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchB->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchA->setValue(switchstatus & 0x3);
}
