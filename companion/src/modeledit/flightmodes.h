#ifndef FLIGHTMODES_H
#define FLIGHTMODES_H

#include "modeledit.h"
#include "eeprominterface.h"

namespace Ui {
  class FlightMode;
}

class FlightModePanel : public ModelPanel
{
    Q_OBJECT

  public:
    FlightModePanel(QWidget *parent, ModelData &model, int modeIdx, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~FlightModePanel();

    virtual void update();

  signals:
    void nameModified();

  private slots:
    void phaseName_editingFinished();
    void phaseSwitch_currentIndexChanged(int index);
    void phaseFadeIn_editingFinished();
    void phaseFadeOut_editingFinished();
    void phaseTrimUse_currentIndexChanged(int index);
    void phaseTrim_valueChanged();
    void phaseTrimSlider_valueChanged();
    void GVName_editingFinished();
    void phaseGVValue_editingFinished();
    void phaseGVUse_currentIndexChanged(int index);
    void phaseGVPopupToggled(bool checked);
    void phaseREValue_editingFinished();
    void phaseREUse_currentIndexChanged(int index);
    void name_customContextMenuRequested(const QPoint & pos);
    void fmClear();

  private:
    Ui::FlightMode *ui;
    int phaseIdx;
    FlightModeData & phase;
    int reCount;
    int gvCount;
    QVector<QLabel *> trimsLabel;
    QLineEdit * gvNames[C9X_MAX_GVARS];
    QSpinBox * gvValues[C9X_MAX_GVARS];
    QCheckBox * gvPopups[C9X_MAX_GVARS];
    QSpinBox * reValues[C9X_MAX_ENCODERS];
    QVector<QComboBox *> trimsUse;
    QVector<QSpinBox *> trimsValue;
    QVector<QSlider *> trimsSlider;

    void trimUpdate(unsigned int trim);

};

class FlightModesPanel : public ModelPanel
{
    Q_OBJECT

  public:
    FlightModesPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~FlightModesPanel();

    virtual void update();

  private slots:
    void onPhaseModified();
    void onPhaseNameChanged();
    void on_tabWidget_currentChanged(int index);

  private:
    int modesCount;
    QTabWidget *tabWidget;
    QString getTabName(int index);
    QVector<FlightModePanel *> panels;

};

#endif // FLIGHTMODES_H

