#ifndef RADIOOUTPUTSWIDGET_H
#define RADIOOUTPUTSWIDGET_H

#include "simulator.h"

#include <QTimer>
#include <QWidget>

namespace Ui {
class RadioOutputsWidget;
}

class Firmware;
class SimulatorInterface;

class QFrame;
class QLabel;
class QSlider;

using namespace Simulator;

class RadioOutputsWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit RadioOutputsWidget(SimulatorInterface * simulator, Firmware * firmware, QWidget * parent = 0);
    ~RadioOutputsWidget();

    void start();
    void stop();
    void restart();

  protected slots:
    void saveState();
    void restoreState();
    void setValues();
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

  protected:
    void changeEvent(QEvent *e);
    void setupChannelsDisplay();
    void setupLsDisplay();
    void setupGVarsDisplay();
    QWidget * createLogicalSwitch(QWidget * parent, int switchNo, QLabel * label);

    SimulatorInterface * m_simulator;
    Firmware * m_firmware;
    QTimer * m_tmrUpdateData;

    QHash<int, QPair<QLabel *, QSlider *> > m_channelsMap;  // m_channelsMap[chanIndex] = {QLabel*, QSlider*}
    QHash<int, QLabel *> m_logicSwitchMap;                  // m_logicSwitchMap[lsIndex] = QLabel*
    QHash<int, QHash<int, QLabel *> > m_globalVarsMap;      // m_globalVarsMap[gvarIndex][fmodeIndex] = QLabel*

    int m_radioProfileId;
    int m_dataUpdateFreq;
    bool m_started;

    const static int m_dataUpdateFreqDefault;
    const static quint16 m_savedViewStateVersion;

  private:
    Ui::RadioOutputsWidget * ui;
};

#endif // RADIOOUTPUTSWIDGET_H
