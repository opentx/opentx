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

#include "modeledit/node.h"
#include "telemetrysimu.h"
#include "trainersimu.h"
#include "debugoutput.h"

#ifdef JOYSTICKS
#include "joystick.h"
#endif

#define TMR_OFF     0
#define TMR_RUNNING 1
#define TMR_BEEPING 2
#define TMR_STOPPED 3

#define FLASH_DURATION 10

namespace Ui {
  class SimulatorDialog9X;
  class SimulatorDialogTaranis;
  class SimulatorDialogFlamenco;
  class SimulatorDialogHorus;
}

// TODO rename + move?
class LcdWidget;
class SliderWidget;
class VirtualJoystickWidget;

#define SIMULATOR_FLAGS_NOTX              1
#define SIMULATOR_FLAGS_STICK_MODE_LEFT   2
#define SIMULATOR_FLAGS_S1                4
#define SIMULATOR_FLAGS_S2                8
#define SIMULATOR_FLAGS_S3               16
#define SIMULATOR_FLAGS_S4               32 // reserved for the future
#define SIMULATOR_FLAGS_S1_MULTI         64
#define SIMULATOR_FLAGS_S2_MULTI        128
#define SIMULATOR_FLAGS_S3_MULTI        256
#define SIMULATOR_FLAGS_S4_MULTI        512 // reserved for the future

void traceCb(const char * text);

class SimulatorDialog : public QDialog
{
  Q_OBJECT

  public:
    explicit SimulatorDialog(QWidget * parent, SimulatorInterface *simulator, unsigned int flags=0);
    virtual ~SimulatorDialog();

    void start(const char * filename);
    void start(QByteArray & eeprom);
    virtual void traceCallback(const char * text);


  protected:
    template <class T> void initUi(T * ui);
    virtual void setLightOn(bool enable) { }
    virtual void updateBeepButton() { }

    unsigned int flags;
    LcdWidget * lcd;
    QVector<QDial *> pots;
    QVector<QLabel *> potLabels;
    QVector<QLabel *> potValues;
    QVector<QSlider *> sliders;
    QTabWidget * tabWidget;
    QVector<QLabel *> logicalSwitchLabels;
    QVector<QSlider *> channelSliders;
    QVector<QLabel *> channelValues;
    QVector<QLabel *> gvarValues;

    void init();
    VirtualJoystickWidget *vJoyLeft;
    VirtualJoystickWidget *vJoyRight;
    QTimer *timer;
    QString windowName;
    unsigned int backLight;
    bool lightOn;
    int switches;
    unsigned int numGvars;
    unsigned int numFlightModes;
#ifdef JOYSTICKS
    Joystick *joystick;
    int jscal[8][4];
    int jsmap[8];
#endif

    SimulatorInterface *simulator;
    unsigned int lastPhase;

    void setupTimer();
    QFrame * createLogicalSwitch(QWidget * parent, int switchNo, QVector<QLabel *> & labels);
    void setupOutputsDisplay();
    void setupGVarsDisplay();

    void centerSticks();
    void setTrims();

    void setValues();
    virtual void getValues() = 0;
    int getValue(qint8 i);
    bool getSwitch(int swtch, bool nc, qint8 level=0);

    int beepVal;

    int lcdWidth;
    int lcdHeight;
    int lcdDepth;
    TelemetrySimulator * TelemetrySimu;
    TrainerSimulator * TrainerSimu;
    DebugOutput * DebugOut;

    QString traceBuffer;
    QMutex traceMutex;
    QList<QString> traceList;
    void updateDebugOutput();

    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent *event);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    static int screenshotIdx;
    int buttonPressed;
    int trimPressed;
    bool middleButtonPressed;

  private slots:
    void onButtonPressed(int value);
    void onTimerEvent();
    void onTrimPressed(int which);
    void onTrimReleased();
    void onTrimSliderMoved(int which, int value);
    void openTelemetrySimulator();
    void openTrainerSimulator();
    void openDebugOutput();
    void onDebugOutputClose();
    void luaReload();

#ifdef JOYSTICKS
    void onjoystickAxisValueChanged(int axis, int value);
#endif
 
};

class SimulatorDialog9X: public SimulatorDialog
{
  Q_OBJECT

  public:
    explicit SimulatorDialog9X(QWidget * parent, SimulatorInterface *simulator, unsigned int flags=0);
    virtual ~SimulatorDialog9X();

  protected:
    virtual void getValues();
    virtual void setLightOn(bool enable);
    virtual void updateBeepButton();
    void saveSwitches(void);
    void restoreSwitches(void);

  protected slots:
    void dialChanged(int index);

  private:
    Ui::SimulatorDialog9X * ui;
    static uint32_t switchstatus;
    int beepShow;

};

class SimulatorDialogTaranis: public SimulatorDialog
{
  Q_OBJECT

  public:
    explicit SimulatorDialogTaranis(QWidget * parent, SimulatorInterface *simulator, unsigned int flags=0);
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
    explicit SimulatorDialogFlamenco(QWidget * parent, SimulatorInterface *simulator, unsigned int flags=0);
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
    explicit SimulatorDialogHorus(QWidget * parent, SimulatorInterface *simulator, unsigned int flags=0);
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

#endif // _SIMULATORDIALOG_H_
