#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "modelpanel.h"
#include <QGroupBox>
#include <QComboBox>
#include <QDoubleSpinBox>

namespace Ui {
  class TelemetryAnalog;
  class Telemetry;
}

class TelemetryAnalog : public QWidget
{
    Q_OBJECT

    friend class TelemetryPanel;

  public:
    TelemetryAnalog(QWidget *parent, FrSkyChannelData & analog);
    virtual ~TelemetryAnalog();

  signals:
    void modified();

  private slots:
    void on_UnitCB_currentIndexChanged(int index);
    void on_RatioSB_editingFinished();
    void on_RatioSB_valueChanged();
    void on_CalibSB_editingFinished();
    void on_alarm1LevelCB_currentIndexChanged(int index);
    void on_alarm1GreaterCB_currentIndexChanged(int index);
    void on_alarm1ValueSB_editingFinished();
    void on_alarm2LevelCB_currentIndexChanged(int index);
    void on_alarm2GreaterCB_currentIndexChanged(int index);
    void on_alarm2ValueSB_editingFinished();

  private:
    Ui::TelemetryAnalog *ui;
    FrSkyChannelData & analog;
    bool lock;

    void update();
};

class TelemetryPanel : public ModelPanel
{
    Q_OBJECT

  public:
    TelemetryPanel(QWidget *parent, ModelData & model);
    virtual ~TelemetryPanel();

  signals:
    void modified();

  private slots:
    void onAnalogModified();
    void on_frskyProtoCB_currentIndexChanged(int index);
    void on_frskyUnitsCB_currentIndexChanged(int index);
    void on_frskyBladesCB_currentIndexChanged(int index);
    void on_frskyCurrentCB_currentIndexChanged(int index);
    void on_frskyVoltCB_currentIndexChanged(int index);
    void on_AltitudeToolbar_ChkB_toggled(bool checked);
    void on_rssiAlarm1CB_currentIndexChanged(int index);
    void on_rssiAlarm2CB_currentIndexChanged(int index);
    void on_rssiAlarm1SB_editingFinished();
    void on_rssiAlarm2SB_editingFinished();
    void on_AltitudeGPS_ChkB_toggled(bool checked);
    void on_varioSourceCB_currentIndexChanged(int index);
    void on_varioLimitMin_DSB_editingFinished();
    void on_varioLimitMax_DSB_editingFinished();
    void on_varioLimitCenterMin_DSB_editingFinished();
    void on_varioLimitMinOff_ChkB_toggled(bool checked);
    void on_varioLimitCenterMax_DSB_editingFinished();
    void on_fasOffset_DSB_editingFinished();
    void on_mahCount_SB_editingFinished();
    void on_mahCount_ChkB_toggled(bool checked);
    void telBarCBcurrentIndexChanged(int index);
    void ScreenTypeCBcurrentIndexChanged(int index);
    void telMaxSBeditingFinished();
    void telMinSBeditingFinished();
    void customFieldEdited();

  private:
    Ui::Telemetry *ui;
    TelemetryAnalog * analogs[2];
    QGroupBox* barsGB[3];
    QGroupBox* numsGB[3];
    QComboBox* barsCB[12];
    QDoubleSpinBox* minSB[12];
    QDoubleSpinBox* maxSB[12];
    QComboBox* csf[36];

    void setup();
    float getBarStep(int barId);
    void telBarUpdate();
};

#endif // TELEMETRY_H
