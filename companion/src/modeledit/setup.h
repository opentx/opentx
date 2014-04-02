#ifndef SETUP_H
#define SETUP_H

#include "modelpanel.h"
#include <QVector>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>

namespace Ui {
  class Setup;
  class Timer;
  class Module;
}

class TimerPanel : public ModelPanel
{
    Q_OBJECT

  public:
    TimerPanel(QWidget *parent, ModelData & model, TimerData & timer, GeneralSettings & generalSettings);
    virtual ~TimerPanel();

    virtual void update();

  private slots:
    void on_mode_currentIndexChanged(int index);
    void on_value_editingFinished();
    void on_persistent_toggled(bool checked);
    void on_minuteBeep_toggled(bool checked);
    void on_countdownBeep_currentIndexChanged(int index);

  private:
    TimerData & timer;
    Ui::Timer *ui;
};

class ModulePanel : public ModelPanel
{
    Q_OBJECT

  public:
    ModulePanel(QWidget *parent, ModelData & model, ModuleData & module, GeneralSettings & generalSettings, int moduleIdx);
    virtual ~ModulePanel();

    virtual void update();

  private slots:
    void on_trainerMode_currentIndexChanged(int index);
    void on_protocol_currentIndexChanged(int index);
    void on_ppmDelay_editingFinished();
    void on_channelsCount_editingFinished();
    void on_channelsStart_editingFinished();
    void on_ppmPolarity_currentIndexChanged(int index);
    void on_ppmFrameLength_editingFinished();
    void on_rxNumber_editingFinished();
    void on_failsafeMode_currentIndexChanged(int value);
    void onFailsafeSpinChanged(double value);

  private:
    ModuleData & module;
    int moduleIdx;
    Ui::Module *ui;
    QVector<QDoubleSpinBox *> failsafeSpins;
};

class Setup : public ModelPanel
{
    Q_OBJECT

  public:
    Setup(QWidget *parent, ModelData & model, GeneralSettings & generalSettings);
    virtual ~Setup();

    virtual void update();

  private slots:
    void on_name_editingFinished();
    void on_throttleSource_currentIndexChanged(int index);
    void on_throttleTrim_toggled(bool checked);
    void on_extendedLimits_toggled(bool checked);
    void on_extendedTrims_toggled(bool checked);
    void on_throttleWarning_toggled(bool checked);
    void on_throttleReverse_toggled(bool checked);
    void on_displayText_toggled(bool checked);
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
    ModulePanel * modules[C9X_NUM_MODULES+1];
    TimerPanel * timers[C9X_MAX_TIMERS];
    void updateStartupSwitches();
    void updatePotWarnings();
    void updateBeepCenter();
    void populateThrottleSourceCB();
};

#endif // SETUP_H
