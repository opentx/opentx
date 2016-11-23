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

#ifndef _TRAINERSIMU_H_
#define _TRAINERSIMU_H_


#include <QShowEvent>
#include <QCloseEvent>
#include <QDialog>
#include <QTimer>
#include "modeledit/node.h"
#include "simulatorinterface.h"

namespace Ui {
  class TrainerSimulator;
}


class TrainerSimulator : public QDialog
{
  Q_OBJECT

  public:
    explicit TrainerSimulator(QWidget * parent, SimulatorInterface * simulator);
    virtual ~TrainerSimulator();


  protected:
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *event);

  private:
    Ui::TrainerSimulator * ui;
    QTimer * timer;
    SimulatorInterface *simulator;

    QGraphicsView * leftStick, * rightStick;
    Node *nodeLeft;
    Node *nodeRight;

    void centerSticks();
    void setupSticks();
    void resizeEvent(QResizeEvent *event  = 0);
    void setTrainerInputs();

  private slots:
    void onTimerEvent();

};

#endif // _TRAINERSIMU_H_

