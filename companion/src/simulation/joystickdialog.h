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

#ifndef _JOYSTICKDIALOG_H_
#define _JOYSTICKDIALOG_H_

#include <QtWidgets>
#include "appdata.h"
#include "joystick.h"

class QCheckBox;
class QComboBox;
class QSlider;

namespace Ui {
    class joystickDialog;
}

class joystickDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit joystickDialog(QWidget *parent = 0, int stick=-1);
    ~joystickDialog();
    Joystick *joystick;

  private:
    Ui::joystickDialog *ui;
    int jscal[MAX_JOYSTICKS][3];
    QCheckBox * invert[MAX_JOYSTICKS];
    QComboBox * sticks[MAX_JOYSTICKS];
    QSlider * sliders[MAX_JOYSTICKS];
    int step;
    int numAxes;
    bool started;

  private slots:
    void populateSourceCombo(QComboBox * cb);
    bool loadJoysticks(int stick = -1);
    void joystickOpen(int stick);
    void joystickSetEnabled(bool enable);
    void onjoystickAxisValueChanged(int axis, int value);
    void loadStep();
    void on_backButton_clicked();
    void on_nextButton_clicked();
    void on_cancelButton_clicked();
    void on_okButton_clicked();

};

#endif // _JOYSTICKDIALOG_H_
