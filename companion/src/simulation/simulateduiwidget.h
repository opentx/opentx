#ifndef SIMULATEDUIWIDGET_H
#define SIMULATEDUIWIDGET_H

#include "constants.h"
#include "simulatordialog.h"

#include <QWidget>
#include <QMouseEvent>

class SimulatorInterface;
class LcdWidget;
class RadioUiAction;

/*
 * This is a base class for the main hardware-specific radio user interface, including LCD screen and navigation buttons/widgets.
 * It is responsible for hanlding all interactions with this part of the simulation (vs. common radio widgets like sticks/switches/knobs).
 * Sub-classes are responsible for building the actual UI form they need for presentation.
 * This base class should not be instantiated directly.
 */
class SimulatedUIWidget : public QWidget
{
  Q_OBJECT

  protected:

    explicit SimulatedUIWidget(SimulatorInterface * simulator, SimulatorDialog * simuDialog = NULL, QWidget * parent = NULL);
    ~SimulatedUIWidget();

  public:

    RadioUiAction * addRadioUiAction(RadioUiAction * act);
    RadioUiAction * addRadioUiAction(int index = -1, int key = 0, const QString &text = "", const QString &descript = "");
    RadioUiAction * addRadioUiAction(int index, QList<int> keys, const QString &text = "", const QString &descript = "");

    QVector<keymapHelp_t> * getKeymapHelp()         { return &m_keymapHelp; }
    QList<RadioUiAction *>  getActions()      const { return m_actions;   }
    RadioUiAction *         getRotEncAction() const { return m_rotEncClickAction; }

  public slots:

    void timedUpdate(unsigned loop);
    void saveScreenshot(int idx = -1);
    void captureScreenshot() { saveScreenshot(); }
    void simulatorWheelEvent(qint8 steps);

    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

  protected:
    void connectScrollActions();
    virtual void setLightOn(bool enable) { }

    SimulatorInterface * m_simulator;
    SimulatorDialog * m_simuDialog;
    LcdWidget * m_lcd;
    QVector<keymapHelp_t> m_keymapHelp;
    QList<RadioUiAction *> m_actions;
    RadioUiAction * m_scrollUpAction;
    RadioUiAction * m_scrollDnAction;
    RadioUiAction * m_rotEncClickAction;
    RadioUiAction * m_screenshotAction;
    BoardEnum m_board;
    unsigned int m_backLight;
    bool m_lightOn;
    int m_beepShow;
    int m_beepVal;

};


// Each subclass is responsible for its own Ui
namespace Ui {
  class SimulatedUIWidget9X;
  class SimulatedUIWidgetX9;
  class SimulatedUIWidgetX12;
  //class SimulatedUIWidgetFlamenco;
}

class SimulatedUIWidget9X: public SimulatedUIWidget
{
  Q_OBJECT

  public:
    explicit SimulatedUIWidget9X(SimulatorInterface * simulator, SimulatorDialog * simuDialog = NULL, QWidget * parent = NULL);
    virtual ~SimulatedUIWidget9X();

  protected:
    void setLightOn(bool enable);

  protected slots:

  private:
    Ui::SimulatedUIWidget9X * ui;

};

class SimulatedUIWidgetX9: public SimulatedUIWidget
{
  Q_OBJECT

  public:
    explicit SimulatedUIWidgetX9(SimulatorInterface * simulator, SimulatorDialog * simuDialog = NULL, QWidget * parent = NULL);
    virtual ~SimulatedUIWidgetX9();

  protected:

  private:
    Ui::SimulatedUIWidgetX9 * ui;
};

class SimulatedUIWidgetX12: public SimulatedUIWidget
{
  Q_OBJECT

  public:
    explicit SimulatedUIWidgetX12(SimulatorInterface * simulator, SimulatorDialog * simuDialog = NULL, QWidget * parent = NULL);
    virtual ~SimulatedUIWidgetX12();

  protected:

  private:
    Ui::SimulatedUIWidgetX12 * ui;
};

//class SimulatedUIWidgetFlamenco: public SimulatedUIWidget
//{
//  Q_OBJECT

//  public:
//    explicit SimulatedUIWidgetFlamenco(SimulatorInterface *simulator, QWidget * parent = NULL);
//    virtual ~SimulatedUIWidgetFlamenco();

//  protected:

//  private:
//    Ui::SimulatedUIWidgetFlamenco * ui;
//};

#endif // SIMULATEDUIWIDGET_H
