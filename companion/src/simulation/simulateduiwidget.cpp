
#include "simulateduiwidget.h"
#include "eeprominterface.h"
#include "lcdwidget.h"
#include "radiouiaction.h"
#include "simulatorinterface.h"

SimulatedUIWidget::SimulatedUIWidget(SimulatorInterface * simulator, SimulatorDialog * simuDialog, QWidget * parent) :
  QWidget(parent),
  m_simulator(simulator),
  m_simuDialog(simuDialog),
  m_lcd(NULL),
  m_scrollUpAction(NULL),
  m_scrollDnAction(NULL),
  m_rotEncClickAction(NULL),
  m_board(getCurrentBoard()),
  m_backLight(0),
  m_lightOn(false),
  m_beepShow(0),
  m_beepVal(0)
{
  m_rotEncClickAction = addRadioUiAction(-1, 0, tr("Rotary encoder click"));
  m_screenshotAction = addRadioUiAction(-1, Qt::Key_Print, tr("Take Screenshot"));
  connect(m_screenshotAction, &RadioUiAction::pushed, this, &SimulatedUIWidget::saveScreenshot);
}

SimulatedUIWidget::~SimulatedUIWidget()
{
  foreach (RadioUiAction * act, m_actions) {
    if (act)
      delete act;
  }
}

RadioUiAction * SimulatedUIWidget::addRadioUiAction(RadioUiAction * act)
{
  if (act) {
    m_actions.append(act);
    if (!act->getText().isEmpty() && !act->getDescription().isEmpty())
      m_keymapHelp.append(keymapHelp_t(act->getText(), act->getDescription()));
  }
  return act;
}

RadioUiAction * SimulatedUIWidget::addRadioUiAction(int index, int key, const QString & text, const QString & descript)
{
  return addRadioUiAction(new RadioUiAction(index, key, m_simuDialog, text, descript));
}

RadioUiAction * SimulatedUIWidget::addRadioUiAction(int index, QList<int> keys, const QString & text, const QString & descript)
{
  return addRadioUiAction(new RadioUiAction(index, keys, m_simuDialog, text, descript));
}

void SimulatedUIWidget::timedUpdate(unsigned loop)
{
  if (m_lcd->isVisible()) {
    bool lightEnable;
    if (m_simulator->lcdChanged(lightEnable)) {
      m_lcd->onLcdChanged(lightEnable);
      if (m_lightOn != lightEnable) {
        setLightOn(lightEnable);
        m_lightOn = lightEnable;
      }
    }
  }
  /*  TODO : beep indicator
      if (!(loop % 5)) {
        TxOutputs outputs;
        simulator->getValues(outputs);
        if (outputs.beep) {
          beepVal = outputs.beep;
        }
        if (beepVal) {
          beepShow = 20;
          beepVal = 0;
          QApplication::beep();
        } else if (beepShow) {
          beepShow--;
        }
        ui->label_beep->setStyleSheet(beepShow ? CBEEP_ON : CBEEP_OFF);
      } */
}

void SimulatedUIWidget::saveScreenshot(int idx)
{
  Q_UNUSED(idx)
  QString fileName = "";
  if (!g.snapToClpbrd()) {
    fileName = QString("screenshot_%1.png").arg(QDateTime::currentDateTime().toString("yy-MM-dd_HH-mm-ss"));
  }
  m_lcd->makeScreenshot(fileName);
}

// steps can be negative or positive to determine direction (negative is UP/RIGHT scroll)
void SimulatedUIWidget::simulatorWheelEvent(qint8 steps)
{
  m_simulator->wheelEvent(steps);
}

void SimulatedUIWidget::wheelEvent(QWheelEvent * event)
{
  if (event->angleDelta().isNull())
    return;
  QPoint numSteps = event->angleDelta() / 8 / 15 * -1;  // one step per 15deg
  simulatorWheelEvent(numSteps.y());
}

void SimulatedUIWidget::mousePressEvent(QMouseEvent * event)
{
  if (event->button() == Qt::MidButton && m_rotEncClickAction)
    m_rotEncClickAction->trigger(true);
  else
    event->ignore();
}

void SimulatedUIWidget::mouseReleaseEvent(QMouseEvent * event)
{
  if (event->button() == Qt::MidButton && m_rotEncClickAction)
    m_rotEncClickAction->trigger(false);
  else
    event->ignore();
}

void SimulatedUIWidget::connectScrollActions()
{
  connect(m_scrollUpAction, &RadioUiAction::pushed, [this](void) {
    this->simulatorWheelEvent(-1);
    m_scrollUpAction->toggle(false);
  });

  connect(m_scrollDnAction, &RadioUiAction::pushed, [this](void) {
    simulatorWheelEvent(1);
    m_scrollDnAction->toggle(false);
  });
}
