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

#define TMR_OFF     0
#define TMR_RUNNING 1
#define TMR_BEEPING 2
#define TMR_STOPPED 3

#define FLASH_DURATION 10

#define SIMULATOR_FLAGS_NOTX              1
#define SIMULATOR_FLAGS_STICK_MODE_LEFT   2
#define SIMULATOR_FLAGS_S1                4
#define SIMULATOR_FLAGS_S2                8
#define SIMULATOR_FLAGS_S3               16
#define SIMULATOR_FLAGS_S4               32  // reserved for the future
#define SIMULATOR_FLAGS_S1_MULTI         64
#define SIMULATOR_FLAGS_S2_MULTI        128
#define SIMULATOR_FLAGS_S3_MULTI        256
#define SIMULATOR_FLAGS_S4_MULTI        512  // reserved for the future
#define SIMULATOR_FLAGS_STANDALONE     1024  // started from stanalone simulator

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
    explicit SimulatorDialog(QWidget * parent, SimulatorInterface *simulator, SimulatorUiFlavor uiflavor, quint8 flags=0);
    virtual ~SimulatorDialog();

    void setRadioProfileId(int value);
    void setDataPath(const QString & dataPath);
    void setPaths(const QString & sdPath, const QString & dataPath);
    void setRadioSettings(const GeneralSettings settings);
    void setEepromData(const QByteArray & eeprom = NULL, bool fromFile = false);
    void setRadioData(RadioData * radioData);
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
    SimulatorUiFlavor uiFlavor;
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
    BoardEnum m_board;
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

/*
class SimulatorDialog9X: public SimulatorDialog
{
  Q_OBJECT

  public:
    explicit SimulatorDialog9X(QWidget * parent, SimulatorInterface *simulator, SimulatorUiFlavor uiflavor, unsigned int flags=0);
    virtual ~SimulatorDialog9X();

  protected:
    virtual void getValues();
    void timedUpdate(unsigned loop);
    void setLightOn(bool enable);
    void updateBeepButton();
    void saveSwitches(void);
    void restoreSwitches(void);

  private:
    Ui::SimulatorDialog9X * ui;
    static uint32_t switchstatus;
    unsigned int backLight;
    bool lightOn;
    int beepShow;
    int beepVal;

};

class SimulatorDialogTaranis: public SimulatorDialog
{
  Q_OBJECT

  public:
    explicit SimulatorDialogTaranis(QWidget * parent, SimulatorInterface *simulator, SimulatorUiFlavor uiflavor, unsigned int flags=0);
    virtual ~SimulatorDialogTaranis();

  protected:
    virtual void getValues();
    void saveSwitches(void);
    void restoreSwitches(void);

  private:
    Ui::SimulatorDialogTaranis * ui;
    static uint32_t switchstatus;

  private slots:
    void resetSH();
    void on_switchH_sliderReleased();
};

class SimulatorDialogFlamenco: public SimulatorDialog
{
  Q_OBJECT

  public:
    explicit SimulatorDialogFlamenco(QWidget * parent, SimulatorInterface *simulator, SimulatorUiFlavor uiflavor, unsigned int flags=0);
    virtual ~SimulatorDialogFlamenco();

  protected:
    virtual void getValues();
    void saveSwitches(void);
    void restoreSwitches(void);

  private:
    Ui::SimulatorDialogFlamenco * ui;
    static uint32_t switchstatus;

  private slots:
    void resetSH();
    void on_switchH_sliderReleased();
};

class SimulatorDialogHorus: public SimulatorDialog
{
  Q_OBJECT

  public:
    explicit SimulatorDialogHorus(QWidget * parent, SimulatorInterface *simulator, SimulatorUiFlavor uiflavor, unsigned int flags=0);
    virtual ~SimulatorDialogHorus();

  protected:
    virtual void getValues();
    void saveSwitches(void);
    void restoreSwitches(void);

  private:
    Ui::SimulatorDialogHorus * ui;
    static uint32_t switchstatus;

  private slots:
    void resetSH();
    void on_switchH_sliderReleased();
};
*/

#endif // _SIMULATORDIALOG_H_
