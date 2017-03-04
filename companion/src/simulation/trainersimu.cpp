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

#include <stdint.h>
#include "trainersimu.h"
#include "ui_trainersimu.h"
#include "helpers.h"
#include "virtualjoystickwidget.h"

TrainerSimulator::TrainerSimulator(QWidget * parent, SimulatorInterface * simulator):
  QWidget(parent),
  ui(new Ui::TrainerSimulator),
  simulator(simulator)
{
  ui->setupUi(this);

  vJoyLeft = new VirtualJoystickWidget(this, 'L', false);
  vJoyLeft->setStickColor(Qt::cyan);
  ui->leftStickLayout->addWidget(vJoyLeft);

  vJoyRight = new VirtualJoystickWidget(this, 'R', false);
  vJoyRight->setStickColor(Qt::cyan);
  ui->rightStickLayout->addWidget(vJoyRight);

  timer = new QTimer(this);
  timer->setInterval(10);
  connect(timer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
}

TrainerSimulator::~TrainerSimulator()
{
  if (timer) {
    timer->stop();
    delete timer;
  }
  if (vJoyLeft)
    delete vJoyLeft;
  if (vJoyRight)
    delete vJoyRight;

  delete ui;
}

void TrainerSimulator::showEvent(QShowEvent *event)
{
  timer->start();
  event->accept();
}

void TrainerSimulator::closeEvent(QCloseEvent *event)
{
  timer->stop();
  event->accept();
}

void TrainerSimulator::centerSticks()
{
  if (vJoyLeft)
    vJoyLeft->centerStick();

  if (vJoyRight)
    vJoyRight->centerStick();
}

void TrainerSimulator::onTimerEvent()
{
  centerSticks();
  setTrainerInputs();
}

void TrainerSimulator::setTrainerInputs()
{
  if (!simulator)
    return;

  if (vJoyLeft) {
    simulator->setTrainerInput(0, int( 512 * vJoyLeft->getStickX()));  // LEFT HORZ
    simulator->setTrainerInput(1, int(-512 * vJoyLeft->getStickY()));  // LEFT VERT
  }
  if (vJoyRight) {
    simulator->setTrainerInput(2, int(-512 * vJoyRight->getStickY()));  // RGHT VERT
    simulator->setTrainerInput(3, int( 512 * vJoyRight->getStickX()));  // RGHT HORZ
  }
}
