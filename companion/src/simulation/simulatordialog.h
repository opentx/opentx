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

#ifndef _SIMULATORDIALOG_H_
#define _SIMULATORDIALOG_H_

#include "constants.h"
#include "helpers.h"
#include "radiodata.h"

#include <QDialog>
#include <QVector>
#include <QMutex>

#define SIMULATOR_FLAGS_NOTX              0x01  // simulating a single model from Companion
#define SIMULATOR_FLAGS_STANDALONE        0x02  // started from stanalone simulator

#define FLASH_DURATION 10

#define CSWITCH_ON    "QLabel { background-color: #4CC417 }"
#define CSWITCH_OFF   "QLabel { }"
#define CBEEP_ON      "QLabel { background-color: #FF364E }"
#define CBEEP_OFF     "QLabel { }"

typedef QPair<QString, QString> keymapHelp_t;

void traceCb(const char * text);

class Firmware;
class SimulatorInterface;
class SimulatedUIWidget;
class TelemetrySimulator;
class TrainerSimulator;
class DebugOutput;
class RadioWidget;
class RadioSwitchWidget;
class VirtualJoystickWidget;
#ifdef JOYSTICKS
class Joystick;
#endif

class QWidget;
class QSlider;
class QDial;
class QLabel;
class QFrame;

namespace Ui {
  class SimulatorDialog;
}

class SimulatorDialog : public QDialog
{
  Q_OBJECT

  public:
    explicit SimulatorDialog(QWidget * parent, SimulatorInterface *simulator, quint8 flags=0);
    virtual ~SimulatorDialog();

    void setRadioProfileId(int value);
    void setDataPath(const QString & dataPath);
    void setPaths(const QString & sdPath, const QString & dataPath);
    void setRadioSettings(const GeneralSettings settings);
    void setEepromData(const QByteArray & eeprom = NULL, bool fromFile = false);
    void setRadioData(RadioData * radioData);
    void setUiAreaStyle(const QString & style);
    void traceCallback(const char * text);
    void start();

  private:
    void setupUi();
    void setupRadioWidgets();
    void setupOutputsDisplay();
    void setupGVarsDisplay();
    QFrame * createLogicalSwitch(QWidget * parent, int switchNo, QVector<QLabel *> & labels);
    void setupTimer();

    void setValues();
    void getValues();
    void setTrims();


    Ui::SimulatorDialog * ui;
    SimulatorInterface * simulator;
    Firmware * firmware;
    GeneralSettings radioSettings;

    QTimer * timer;
    QString windowName;
    QString traceBuffer;
    QMutex traceMutex;
    QList<QString> traceList;
    QVector<keymapHelp_t> keymapHelp;

    SimulatedUIWidget     * radioUiWidget;
    VirtualJoystickWidget * vJoyLeft;
    VirtualJoystickWidget * vJoyRight;
    TelemetrySimulator    * TelemetrySimu;
    TrainerSimulator      * TrainerSimu;
    DebugOutput           * DebugOut;

    QVector<RadioSwitchWidget *> switches;
    QVector<RadioWidget       *> analogs;
    QVector<QLabel  *> logicalSwitchLabels;
    QVector<QSlider *> channelSliders;
    QVector<QLabel  *> channelValues;
    QVector<QLabel  *> gvarValues;

    QString sdCardPath;
    QString radioDataPath;
    QByteArray eepromData;
    Board::Type m_board;
    quint8 flags;
    int radioProfileId;
    int lastPhase;
    int buttonPressed;
    int trimPressed;
    bool eepromDataFromFile;
    bool middleButtonPressed;

#ifdef JOYSTICKS
    Joystick *joystick;
    int jscal[8][4];
    int jsmap[8];
#endif

  private slots:
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    void onTimerEvent();
    void onTrimPressed(int which);
    void onTrimReleased();
    void onTrimSliderMoved(int which, int value);
    void centerSticks();
    void openTelemetrySimulator();
    void openTrainerSimulator();
    void openDebugOutput();
    void updateDebugOutput();
    void luaReload();
    void showHelp();
#ifdef JOYSTICKS
    void onjoystickAxisValueChanged(int axis, int value);
#endif

};

#endif // _SIMULATORDIALOG_H_
