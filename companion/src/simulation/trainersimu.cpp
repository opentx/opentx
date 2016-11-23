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

#define GBALL_SIZE  20
#define RESX        512

TrainerSimulator::TrainerSimulator(QWidget * parent, SimulatorInterface * simulator):
  QDialog(parent),
  ui(new Ui::TrainerSimulator),
  simulator(simulator)
{
  ui->setupUi(this);
  leftStick = ui->leftStick;
  rightStick = ui->rightStick;

  setupSticks();

  // resize(0, 0); // to force min height, min width
  setFixedSize(width(), height());

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
}

TrainerSimulator::~TrainerSimulator()
{
  timer->stop();
  delete ui;
}

void TrainerSimulator::showEvent(QShowEvent *event)
{
  timer->start(10);
  event->accept();
}

void TrainerSimulator::closeEvent(QCloseEvent *event)
{
  timer->stop();
  event->accept();
}

void TrainerSimulator::centerSticks()
{
  if (leftStick->scene())
    nodeLeft->stepToCenter();

  if (rightStick->scene())
    nodeRight->stepToCenter();
}

void TrainerSimulator::setupSticks()
{
  QGraphicsScene *leftScene = new QGraphicsScene(leftStick);
  leftScene->setItemIndexMethod(QGraphicsScene::NoIndex);
  leftStick->setScene(leftScene);

  // leftStick->scene()->addLine(0,10,20,30);

  QGraphicsScene *rightScene = new QGraphicsScene(rightStick);
  rightScene->setItemIndexMethod(QGraphicsScene::NoIndex);
  rightStick->setScene(rightScene);

  // rightStick->scene()->addLine(0,10,20,30);

  nodeLeft = new Node();
  nodeLeft->setPos(-GBALL_SIZE/2,-GBALL_SIZE/2);
  nodeLeft->setBallSize(GBALL_SIZE);
  leftScene->addItem(nodeLeft);

  nodeRight = new Node();
  nodeRight->setPos(-GBALL_SIZE/2,-GBALL_SIZE/2);
  nodeRight->setBallSize(GBALL_SIZE);
  rightScene->addItem(nodeRight);
}

void TrainerSimulator::resizeEvent(QResizeEvent *event)
{
  if (leftStick->scene()) {
    QRect qr = leftStick->contentsRect();
    qreal w  = (qreal)qr.width()  - GBALL_SIZE;
    qreal h  = (qreal)qr.height() - GBALL_SIZE;
    qreal cx = (qreal)qr.width()/2;
    qreal cy = (qreal)qr.height()/2;
    leftStick->scene()->setSceneRect(-cx,-cy,w,h);

    QPointF p = nodeLeft->pos();
    p.setX(qMin(cx, qMax(p.x(), -cx)));
    p.setY(qMin(cy, qMax(p.y(), -cy)));
    nodeLeft->setPos(p);
  }

  if (rightStick->scene()) {
    QRect qr = rightStick->contentsRect();
    qreal w  = (qreal)qr.width()  - GBALL_SIZE;
    qreal h  = (qreal)qr.height() - GBALL_SIZE;
    qreal cx = (qreal)qr.width()/2;
    qreal cy = (qreal)qr.height()/2;
    rightStick->scene()->setSceneRect(-cx,-cy,w,h);

    QPointF p = nodeRight->pos();
    p.setX(qMin(cx, qMax(p.x(), -cx)));
    p.setY(qMin(cy, qMax(p.y(), -cy)));
    nodeRight->setPos(p);
  }
  QDialog::resizeEvent(event);
}


void TrainerSimulator::onTimerEvent()
{
  centerSticks();
  setTrainerInputs();
}

void TrainerSimulator::setTrainerInputs()
{
  simulator->setTrainerInput(0, int( 512* nodeLeft->getX()));  // LEFT HORZ
  simulator->setTrainerInput(1, int(-512* nodeLeft->getY()));  // LEFT VERT
  simulator->setTrainerInput(2, int(-512*nodeRight->getY()));  // RGHT VERT
  simulator->setTrainerInput(3, int( 512*nodeRight->getX()));  // RGHT HORZ
}