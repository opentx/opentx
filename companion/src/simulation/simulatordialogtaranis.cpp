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

#include "ui_simulatordialog-taranis.h"

uint32_t SimulatorDialogTaranis::switchstatus = 0;

SimulatorDialogTaranis::SimulatorDialogTaranis(QWidget * parent, SimulatorInterface *simulator, unsigned int flags):
  SimulatorDialog(parent, simulator, flags),
  ui(new Ui::SimulatorDialogTaranis)
{
  QPolygon polygon;

  lcdWidth = 212;
  lcdHeight = 64;
  lcdDepth = 4;

  initUi<Ui::SimulatorDialogTaranis>(ui);

  polygon.setPoints(6, 20, 59, 27, 50, 45, 52, 56, 59, 50, 71, 26, 72);
  ui->leftbuttons->addArea(polygon, Qt::Key_PageUp, "Taranis/x9l1.png");
  polygon.setPoints(6, 23, 107, 30, 99, 46, 100, 55, 106, 47, 117, 28, 117);
  ui->leftbuttons->addArea(polygon, Qt::Key_PageDown, "Taranis/x9l2.png");
  polygon.setPoints(6, 24, 154, 32, 144, 46, 146, 57, 156, 46, 167, 29, 166);
  ui->leftbuttons->addArea(polygon, Qt::Key_Escape, "Taranis/x9l3.png");
  ui->leftbuttons->addArea(90, 177, 118, 197, Qt::Key_Print, "Taranis/x9l4.png");
  polygon.setPoints(6, 64, 60, 71, 50, 90, 50, 100, 60, 90, 73, 72, 73);
  ui->rightbuttons->addArea(polygon, Qt::Key_Plus, "Taranis/x9r1.png");
  polygon.setPoints(6, 63, 109, 73, 100, 88, 100, 98, 109, 88, 119, 72, 119);
  ui->rightbuttons->addArea(polygon, Qt::Key_Minus, "Taranis/x9r2.png");
  polygon.setPoints(6, 63, 155, 72, 146, 90, 146, 98, 155, 88, 166, 72, 166);
  ui->rightbuttons->addArea(polygon, Qt::Key_Enter, "Taranis/x9r3.png");

  // install simulator TRACE hook
  simulator->installTraceHook(traceCb);

  ui->lcd->setBackgroundColor(47, 123, 227);

  // restore switches
  if (g.simuSW()) {
    restoreSwitches();
  }

  for (int i=0; i<pots.count(); i++) {
    if (flags & (SIMULATOR_FLAGS_S1_MULTI << i)) {
      pots[i]->setValue(-1024);
      pots[i]->setSingleStep(2048/5);
      pots[i]->setPageStep(2048/5);
    }
    else if (!(flags & (SIMULATOR_FLAGS_S1 << i))) {
      pots[i]->hide();
      potLabels[i]->hide();
    }
  }

  connect(ui->leftbuttons, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->rightbuttons, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->teleSim, SIGNAL(released()), this, SLOT(openTelemetrySimulator()));
  connect(ui->trainerSim, SIGNAL(released()), this, SLOT(openTrainerSimulator()));
  connect(ui->debugConsole, SIGNAL(released()), this, SLOT(openDebugOutput()));
  connect(ui->luaReload, SIGNAL(released()), this, SLOT(luaReload()));
}

SimulatorDialogTaranis::~SimulatorDialogTaranis()
{
  saveSwitches();
  delete ui;
}


void SimulatorDialogTaranis::resetSH()
{
  ui->switchH->setValue(0);
}

void SimulatorDialogTaranis::on_switchH_sliderReleased()
{
  QTimer::singleShot(400, this, SLOT(resetSH()));
}

void SimulatorDialogTaranis::getValues()
{
  for (int i=0; i<pots.count(); i++) {
    if (flags & (SIMULATOR_FLAGS_S1_MULTI << i)) {
      int s1 = round((pots[i]->value()+1024)/(2048.0/5))*(2048.0/5)-1024;
      pots[i]->setValue(s1);
    }
  }

  TxInputs inputs = {
    {
      int(1024 * vJoyLeft->getStickX()),    // LEFT HORZ
      int(-1024 * vJoyLeft->getStickY()),   // LEFT VERT
      int(-1024 * vJoyRight->getStickY()),  // RGHT VERT
      int(1024 * vJoyRight->getStickX())    // RGHT HORZ
    },

    {
      -pots[0]->value(),
      pots[1]->value(),
      ((flags & SIMULATOR_FLAGS_S3) ? pots[2]->value() : 0),
      -sliders[0]->value(),
      sliders[1]->value()
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
      buttonPressed == Qt::Key_Enter,
      buttonPressed == Qt::Key_PageDown,
      buttonPressed == Qt::Key_Plus,
      buttonPressed == Qt::Key_Minus
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

void SimulatorDialogTaranis::saveSwitches(void)
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

void SimulatorDialogTaranis::restoreSwitches(void)
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


