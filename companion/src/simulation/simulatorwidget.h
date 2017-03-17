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

#ifndef _SIMULATORWIDGET_H_
#define _SIMULATORWIDGET_H_

#include "constants.h"
#include "helpers.h"
#include "radiodata.h"
#include "simulator.h"

#include <QWidget>
#include <QVector>

#define FLASH_DURATION 10

#define CBEEP_ON      "QLabel { background-color: #FF364E }"
#define CBEEP_OFF     "QLabel { }"

void traceCb(const char * text);

class Firmware;
class SimulatorInterface;
class SimulatedUIWidget;
class RadioWidget;
class RadioSwitchWidget;
class VirtualJoystickWidget;
#ifdef JOYSTICKS
class Joystick;
#endif

class QWidget;
class QSlider;
class QLabel;
class QFrame;

namespace Ui {
  class SimulatorWidget;
}

using namespace Simulator;

class SimulatorWidget : public QWidget
{
  Q_OBJECT

  public:
    explicit SimulatorWidget(QWidget * parent, SimulatorInterface *simulator, quint8 flags=0);
    virtual ~SimulatorWidget();

    void setSdPath(const QString & sdPath);
    void setDataPath(const QString & dataPath);
    void setPaths(const QString & sdPath, const QString & dataPath);
    void setRadioSettings(const GeneralSettings settings);
    bool setStartupData(const QByteArray & dataSource = NULL, bool fromFile = false);
    bool setRadioData(RadioData * radioData);
    bool setOptions(SimulatorOptions & options, bool withSave = true);
    bool saveRadioData(RadioData * radioData, const QString & path = "", QString * error = NULL);
    bool useTempDataPath(bool deleteOnClose = true);
    bool saveTempData();
    void deleteTempData();
    void saveState();
    void setUiAreaStyle(const QString & style);
    void captureScreenshot(bool);
    void setupJoysticks();

    QString getSdPath()   const { return sdCardPath; }
    QString getDataPath() const { return radioDataPath; }
    QVector<keymapHelp_t> * getKeymapHelp() { return &keymapHelp; }

  public slots:
    void start();
    void stop();
    void restart();
    void shutdown();

  private:
    void setRadioProfileId(int value);
    void setupRadioWidgets();
    void setupTimer();
    void restoreRadioWidgetsState();
    QList<QByteArray> saveRadioWidgetsState();

    void setValues();
    void getValues();
    void setTrims();


    Ui::SimulatorWidget * ui;
    SimulatorInterface * simulator;
    Firmware * firmware;
    GeneralSettings radioSettings;

    QTimer * timer;
    QString windowName;
    QVector<keymapHelp_t> keymapHelp;

    SimulatedUIWidget     * radioUiWidget;
    VirtualJoystickWidget * vJoyLeft;
    VirtualJoystickWidget * vJoyRight;

    QVector<RadioSwitchWidget *> switches;
    QVector<RadioWidget       *> analogs;

    QString sdCardPath;
    QString radioDataPath;
    QByteArray startupData;
    Board::Type m_board;
    quint8 flags;
    int radioProfileId;
    int lastPhase;
    int buttonPressed;
    int trimPressed;
    bool startupFromFile;
    bool deleteTempRadioData;
    bool saveTempRadioData;
    bool middleButtonPressed;
    bool firstShow;

#ifdef JOYSTICKS
    Joystick *joystick;
    int jscal[8][4];
    int jsmap[8];
#endif

  private slots:
    //virtual void showEvent(QShowEvent *);
    //virtual void closeEvent(QCloseEvent *);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    void onTimerEvent();
    void onTrimPressed(int which);
    void onTrimReleased();
    void onTrimSliderMoved(int which, int value);
    void centerSticks();
    void onjoystickAxisValueChanged(int axis, int value);

};

#endif // _SIMULATORWIDGET_H_
