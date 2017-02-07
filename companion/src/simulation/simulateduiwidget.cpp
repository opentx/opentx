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

#include "simulateduiwidget.h"
#include "eeprominterface.h"
#include "lcdwidget.h"
#include "radiouiaction.h"
#include "simulatorinterface.h"

SimulatedUIWidget::SimulatedUIWidget(SimulatorInterface * simulator, QWidget * parent) :
  QWidget(parent),
  m_simulator(simulator),
  m_parent(parent),
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
  connect(m_screenshotAction, static_cast<void (RadioUiAction::*)(void)>(&RadioUiAction::pushed), this, &SimulatedUIWidget::captureScreenshot);
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
  return addRadioUiAction(new RadioUiAction(index, key, m_parent, text, descript));
}

RadioUiAction * SimulatedUIWidget::addRadioUiAction(int index, QList<int> keys, const QString & text, const QString & descript)
{
  return addRadioUiAction(new RadioUiAction(index, keys, m_parent, text, descript));
}

QPolygon SimulatedUIWidget::polyArc(int ctrX, int ctrY, int radius, int startAngle, int endAngle, int step)
{
  QPolygon polygon;
  float st = qDegreesToRadians((float)startAngle - 90.0f);
  float en = qDegreesToRadians((float)endAngle - 90.0f);
  float sp = qDegreesToRadians((float)step);
  for ( ; st < en; st += sp) {
    polygon << QPoint((int)(ctrX + radius * cosf(st)), (int)(ctrY + radius * sinf(st)));
  }
  return polygon;
}

void SimulatedUIWidget::updateUi()
{
  //static quint32 loop = 0;
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

void SimulatedUIWidget::captureScreenshot()
{
  QString fileName;
  if (!g.snapToClpbrd()) {
    QString path = g.snapshotDir();
    if (path.isEmpty())
      path = "./";
    QDir dir(path);
    if (!dir.exists() || !dir.isReadable()) {
      // m_simulator->traceCallback("SIMULATOR ERROR - Cannot open screenshot folder, check your settings.\n");
      qDebug() << "SIMULATOR ERROR - Cannot open screenshot folder, check your settings.";
      return;
    }
    QStringList fwid = getCurrentFirmware()->getId().split("-", QString::SkipEmptyParts);
    QString flavor = fwid.at(qMin(1, fwid.size()));
    QString fnpfx = tr("screenshot", "Simulator LCD screenshot file name prefix");
    fileName = "%1/%2_%3_%4.png";
    fileName = fileName.arg(dir.absolutePath(), fnpfx, flavor, QDateTime::currentDateTime().toString("yy-MM-dd_HH-mm-ss"));
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

void SimulatedUIWidget::setLcd(LcdWidget * lcd)
{
  m_lcd = lcd;
  Firmware * firmware = getCurrentFirmware();
  m_lcd->setData(m_simulator->getLcd(), firmware->getCapability(LcdWidth), firmware->getCapability(LcdHeight), firmware->getCapability(LcdDepth));

  if (!m_backlightColors.size())
    return;

  m_backLight = g.backLight();
  if ((int)m_backLight >= m_backlightColors.size())
    m_backLight = 0;

  m_lcd->setBackgroundColor(m_backlightColors.at(m_backLight));
}

void SimulatedUIWidget::connectScrollActions()
{
  connect(m_scrollUpAction, static_cast<void (RadioUiAction::*)(void)>(&RadioUiAction::pushed), [this](void) {
    this->simulatorWheelEvent(-1);
    m_scrollUpAction->toggle(false);
  });

  connect(m_scrollDnAction, static_cast<void (RadioUiAction::*)(void)>(&RadioUiAction::pushed), [this](void) {
    simulatorWheelEvent(1);
    m_scrollDnAction->toggle(false);
  });
}
