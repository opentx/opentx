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

#ifndef _SETUP_H_
#define _SETUP_H_

#include "modeledit.h"
#include "eeprominterface.h"

namespace Ui {
  class Setup;
  class Timer;
  class Module;
}

class TimerPanel : public ModelPanel
{
    Q_OBJECT

  public:
    TimerPanel(QWidget *parent, ModelData & model, TimerData & timer, GeneralSettings & generalSettings, Firmware * firmware, QWidget *prevFocus);
    virtual ~TimerPanel();

    virtual void update();
    QWidget * getLastFocus();

  private slots:
    void on_mode_currentIndexChanged(int index);
    void on_value_editingFinished();
    void on_minuteBeep_toggled(bool checked);
    void on_name_editingFinished();

  private:
    TimerData & timer;
    Ui::Timer *ui;
};

class ModulePanel : public ModelPanel
{
  static const int maxChannels = 16;

  struct ChannelFailsafeWidgetsGroup {
    QComboBox * combo;
    QDoubleSpinBox * spinbox;
    QLabel * label;
  };

    Q_OBJECT

  public:
    ModulePanel(QWidget *parent, ModelData & model, ModuleData & module, GeneralSettings & generalSettings, Firmware * firmware, int moduleIdx);
    virtual ~ModulePanel();
    virtual void update();

  protected:
    void updateFailsafe(int channel);

  private slots:
    void on_trainerMode_currentIndexChanged(int index);
    void on_protocol_currentIndexChanged(int index);
    void on_ppmDelay_editingFinished();
    void on_channelsCount_editingFinished();
    void on_channelsStart_editingFinished();
    void on_ppmPolarity_currentIndexChanged(int index);
    void on_ppmOutputType_currentIndexChanged(int index);
    void on_ppmFrameLength_editingFinished();
    void on_antennaMode_currentIndexChanged(int index);
    void on_rxNumber_editingFinished();
    void on_failsafeMode_currentIndexChanged(int value);
    void onFailsafeComboIndexChanged(int index);
    void onFailsafeSpinChanged(double value);
    void on_multiProtocol_currentIndexChanged(int index);
    void on_multiSubType_currentIndexChanged(int index);
    void on_autoBind_stateChanged(int state);
    void on_lowPower_stateChanged(int state);

  private:
    ModuleData & module;
    int moduleIdx;
    Ui::Module *ui;
    ChannelFailsafeWidgetsGroup failsafeGroups[maxChannels];
};

class SetupPanel : public ModelPanel
{
    Q_OBJECT

  public:
    SetupPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~SetupPanel();

    virtual void update();

  signals:
    void extendedLimitsToggled();

  private slots:
    void on_name_editingFinished();
    void on_throttleSource_currentIndexChanged(int index);
    void on_throttleTrim_toggled(bool checked);
    void on_extendedLimits_toggled(bool checked);
    void on_extendedTrims_toggled(bool checked);
    void on_throttleWarning_toggled(bool checked);
    void on_throttleReverse_toggled(bool checked);
    void on_displayText_toggled(bool checked);
    void on_gfEnabled_toggled(bool checked);
    void on_image_currentIndexChanged(int index);
    void on_trimIncrement_currentIndexChanged(int index);
    void onBeepCenterToggled(bool checked);
    void startupSwitchEdited(int value);
    void startupSwitchToggled(bool checked);
    void potWarningToggled(bool checked);
    void on_potWarningMode_currentIndexChanged(int index);
    void onChildModified();

  private:
    Ui::Setup *ui;
    QVector<QSlider *> startupSwitchesSliders;
    QVector<QCheckBox *> startupSwitchesCheckboxes;
    QVector<QCheckBox *> potWarningCheckboxes;
    QVector<QCheckBox *> centerBeepCheckboxes;
    ModulePanel * modules[CPN_MAX_MODULES+1];
    TimerPanel * timers[CPN_MAX_TIMERS];
    void updateStartupSwitches();
    void updatePotWarnings();
    void updateBeepCenter();
    void populateThrottleSourceCB();
};

#endif // _SETUP_H_
