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

#include "radiooutputswidget.h"
#include "ui_radiooutputswidget.h"

#include "appdata.h"
#include "constants.h"
#include "eeprominterface.h"
#include "radiodata.h"
#include "simulator.h"
#include "simulatorinterface.h"

extern AppData g;  // ensure what "g" means

const int RadioOutputsWidget::m_dataUpdateFreqDefault = 10;  // ms
const quint16 RadioOutputsWidget::m_savedViewStateVersion = 1;

RadioOutputsWidget::RadioOutputsWidget(SimulatorInterface * simulator, Firmware * firmware, QWidget *parent) :
  QWidget(parent),
  m_simulator(simulator),
  m_firmware(firmware),
  m_tmrUpdateData(new QTimer),
  m_radioProfileId(g.sessionId()),
  m_started(false),
  ui(new Ui::RadioOutputsWidget)
{
  ui->setupUi(this);

  restoreState();

  m_dataUpdateFreq = m_dataUpdateFreqDefault;
  m_tmrUpdateData->setInterval(m_dataUpdateFreq);

  connect(m_tmrUpdateData, &QTimer::timeout, this, &RadioOutputsWidget::setValues);
}

RadioOutputsWidget::~RadioOutputsWidget()
{
  stop();
  saveState();
  if (m_tmrUpdateData)
    delete m_tmrUpdateData;
  delete ui;
}

void RadioOutputsWidget::changeEvent(QEvent *e)
{
  QWidget::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

void RadioOutputsWidget::showEvent(QShowEvent * event)
{
  if (m_started)
    m_tmrUpdateData->start();
}

void RadioOutputsWidget::hideEvent(QHideEvent * event)
{
  m_tmrUpdateData->stop();
}

void RadioOutputsWidget::start()
{
  setupChannelsDisplay();
  setupGVarsDisplay();
  setupLsDisplay();
  m_tmrUpdateData->start();
  m_started = true;
}

void RadioOutputsWidget::stop()
{
  m_tmrUpdateData->stop();
  m_started = false;
}

void RadioOutputsWidget::restart()
{
  stop();
  start();
}

void RadioOutputsWidget::saveState()
{
  QByteArray state;
  QDataStream stream(&state, QIODevice::WriteOnly);
  stream << m_savedViewStateVersion
         << ui->btnLogiSw->isChecked() << ui->btnGlobalVars->isChecked()  << ui->btnChannels->isChecked()
         << ui->splitter->saveState();

  SimulatorOptions opts = g.profile[m_radioProfileId].simulatorOptions();
  opts.radioOutputsState = state;
  g.profile[m_radioProfileId].simulatorOptions(opts);
}

void RadioOutputsWidget::restoreState()
{
  quint16 ver = 0;
  QByteArray splitterState;
  bool ls = true, gv = true, ch = true;
  QByteArray state = g.profile[m_radioProfileId].simulatorOptions().radioOutputsState;
  QDataStream stream(state);

  stream >> ver;
  if (ver && ver <= m_savedViewStateVersion)
    stream >> ls >> gv >> ch >> splitterState;

  ui->btnLogiSw->setChecked(ls);
  ui->btnGlobalVars->setChecked(gv);
  ui->btnChannels->setChecked(ch);
  if (!splitterState.isEmpty())
    ui->splitter->restoreState(splitterState);
}


void RadioOutputsWidget::setupChannelsDisplay()
{
  int outputs = std::min(32, m_firmware->getCapability(Capability(Outputs)));

  // delete old widgets if already exist
  m_channelsMap.clear();

  QWidget * oldChanW = ui->channelsScroll->takeWidget();
  if (oldChanW)
    oldChanW->deleteLater();

  if (!outputs)
    return;

  QWidget * channelsWidget = new QWidget();
  QGridLayout * channelsLayout = new QGridLayout(channelsWidget);
  channelsLayout->setHorizontalSpacing(4);
  channelsLayout->setVerticalSpacing(3);
  channelsLayout->setContentsMargins(5, 5, 5, 5);

  ui->channelsScroll->setWidget(channelsWidget);

  // populate outputs
  int column = 0;
  for (int i=0; i < outputs; i++) {
    QLabel * label = new QLabel(channelsWidget);
    label->setText(" " + RawSource(SOURCE_TYPE_CH, i).toString() + " ");
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    channelsLayout->addWidget(label, 0, column, 1, 1);

    QSlider * slider = new QSlider(channelsWidget);
    slider->setEnabled(false);
    slider->setMinimum(-1024);
    slider->setMaximum(1024);
    slider->setPageStep(128);
    slider->setTracking(false);
    slider->setOrientation(Qt::Vertical);
    slider->setInvertedAppearance(false);
    slider->setTickPosition(QSlider::TicksRight);
    slider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    channelsLayout->addWidget(slider, 2, column, 1, 1);
    channelsLayout->setAlignment(slider, Qt::AlignHCenter);

    QLabel * value = new QLabel(channelsWidget);
    value->setMinimumSize(QSize(value->fontMetrics().size(Qt::TextSingleLine, "-100.0").width(), 0));
    value->setAlignment(Qt::AlignCenter);
    value->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    channelsLayout->addWidget(value, 1, column, 1, 1);

    ++column;

    m_channelsMap.insert(i, QPair<QLabel *, QSlider *>(value, slider));
  }
}

void RadioOutputsWidget::setupGVarsDisplay()
{
  int gvars = m_firmware->getCapability(Capability(Gvars));
  int fmodes = m_firmware->getCapability(Capability(FlightModes));

  // delete old widgets if already exist
  m_globalVarsMap.clear();

  QWidget * oldGv = ui->globalVarsScroll->takeWidget();
  if (oldGv)
    oldGv->deleteLater();

  if (!gvars)
    return;

  QWidget * gvarsWidget = new QWidget();
  QGridLayout * gvarsLayout = new QGridLayout(gvarsWidget);
  gvarsLayout->setContentsMargins(5, 5, 5, 5);
  gvarsLayout->setHorizontalSpacing(6);
  gvarsLayout->setVerticalSpacing(3);
  ui->globalVarsScroll->setWidget(gvarsWidget);

  QPalette::ColorRole bgrole = QPalette::AlternateBase;
  for (int fm=0; fm < fmodes; fm++) {
    QLabel * label = new QLabel(gvarsWidget);
    label->setText(QString("FM%1").arg(fm));
    label->setAlignment(Qt::AlignCenter);
    label->setBackgroundRole(bgrole);
    gvarsLayout->addWidget(label, 0, fm+1);
  }
  QHash<int, QLabel *> fmMap;
  for (int gv=0; gv < gvars; gv++) {
    bgrole = ((gv % 2) ? QPalette::Background : QPalette::AlternateBase);
    QLabel * label = new QLabel(gvarsWidget);
    label->setText(QString("GV%1").arg(gv+1));
    label->setAutoFillBackground(true);
    label->setBackgroundRole(bgrole);
    gvarsLayout->addWidget(label, gv+1, 0);
    for (int fm=0; fm < fmodes; fm++) {
      QLabel * value = new QLabel(gvarsWidget);
      value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
      value->setAutoFillBackground(true);
      value->setBackgroundRole(bgrole);
      value->setText("0");
      value->setStyleSheet("padding-right: .06em;");
      gvarsLayout->addWidget(value, gv+1, fm+1);

      fmMap.insert(fm, value);
    }
    m_globalVarsMap.insert(gv, fmMap);
  }
}

void RadioOutputsWidget::setupLsDisplay()
{
  int switches = m_firmware->getCapability(LogicalSwitches);

  // delete old widgets if already exist
  m_logicSwitchMap.clear();

  QWidget * oldLsW = ui->logicalSwitchesScroll->takeWidget();
  if (oldLsW)
    oldLsW->deleteLater();

  if (!switches)
    return;

  QWidget * logicalSwitches = new QWidget();
  logicalSwitches->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  QGridLayout * logicalSwitchesLayout = new QGridLayout(logicalSwitches);
  logicalSwitchesLayout->setHorizontalSpacing(3);
  logicalSwitchesLayout->setVerticalSpacing(2);
  logicalSwitchesLayout->setContentsMargins(5, 5, 5, 5);
  ui->logicalSwitchesScroll->setWidget(logicalSwitches);

  // populate logical switches
  int rows = switches / (switches > 16 ? 4 : 2);
  for (int i=0; i < switches; i++) {
    logicalSwitchesLayout->addWidget(createLogicalSwitch(logicalSwitches, i), i / rows, i % rows, 1, 1);
  }
}

QWidget * RadioOutputsWidget::createLogicalSwitch(QWidget * parent, int switchNo)
{
  QLabel * swtch = new QLabel(parent);
  swtch->setAutoFillBackground(true);
  swtch->setFrameStyle(QFrame::Panel | QFrame::Raised);
  swtch->setLineWidth(2);
  swtch->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  QFont font = swtch->font();
  font.setBold(true);
  swtch->setFont(font);
  swtch->setMinimumWidth(swtch->fontMetrics().width("99") + 10);
  font.setBold(false);
  swtch->setFont(font);
  swtch->setText(QString("%1").arg(RawSwitch(SWITCH_TYPE_VIRTUAL, switchNo+1).toString().remove("L"), 2, QLatin1Char('0')));
  swtch->setAlignment(Qt::AlignCenter);
  m_logicSwitchMap.insert(switchNo, swtch);
  return swtch;
}

// Read various values from firmware simulator and populate values in this UI
void RadioOutputsWidget::setValues()
{
  static int lastPhase = -1;
  static TxOutputs prevOutputs = TxOutputs();
  static bool firstGvarsRun = true;
  int currentPhase;
  TxOutputs outputs;
  QFont font;

  m_simulator->getValues(outputs);
  currentPhase = m_simulator->getPhase();

  if (ui->channelsWidget->isVisible()) {
    QHash<int, QPair<QLabel *, QSlider *> >::const_iterator ch;
    for (ch = m_channelsMap.constBegin(); ch != m_channelsMap.constEnd(); ++ch) {
      if (ch.key() >= CPN_MAX_CHNOUT)
        continue;
      ch.value().first->setText(QString("%1%").arg(outputs.chans[ch.key()] * 100 / 1024));
      ch.value().second->setValue(qMin(1024, qMax(-1024, outputs.chans[ch.key()])));
    }
  }

  if (ui->logicalSwitchesWidget->isVisible()) {
    QHash<int, QLabel* >::const_iterator ls;
    for (ls = m_logicSwitchMap.constBegin(); ls != m_logicSwitchMap.constEnd(); ++ls) {
      if (ls.key() >= CPN_MAX_CSW || prevOutputs.vsw[ls.key()] == outputs.vsw[ls.key()])
        continue;
      ls.value()->setBackgroundRole(outputs.vsw[ls.key()] ? QPalette::Dark : QPalette::Background);
      ls.value()->setForegroundRole(outputs.vsw[ls.key()] ? QPalette::BrightText : QPalette::WindowText);
      ls.value()->setFrameShadow(outputs.vsw[ls.key()] ? QFrame::Sunken : QFrame::Raised);
      font = ls.value()->font();
      font.setBold(outputs.vsw[ls.key()]);
      ls.value()->setFont(font);
      prevOutputs.vsw[ls.key()] = outputs.vsw[ls.key()];
    }
  }

  if (ui->globalVarsWidget->isVisible()) {
    QPalette::ColorRole bgrole;
    QHash<int, QHash<int, QLabel *> >::const_iterator gv;
    QHash<int, QLabel *>::const_iterator fm;
    for (gv = m_globalVarsMap.constBegin(); gv != m_globalVarsMap.constEnd(); ++gv) {
      if (gv.key() >= CPN_MAX_GVARS)
        continue;
      for (fm = gv.value().constBegin(); fm != gv.value().constEnd(); ++fm) {
        if (fm.key() >= CPN_MAX_FLIGHT_MODES)
          continue;
        if (currentPhase != lastPhase || prevOutputs.gvars[fm.key()][gv.key()] != outputs.gvars[fm.key()][gv.key()] || firstGvarsRun) {
          if (fm.key() == currentPhase)
            bgrole = QPalette::Dark;
          else
            bgrole = ((gv.key() % 2) ? QPalette::Background : QPalette::AlternateBase);
          fm.value()->setBackgroundRole(bgrole);
          fm.value()->setForegroundRole(fm.key() == currentPhase ? QPalette::BrightText : QPalette::WindowText);
          font = fm.value()->font();
          font.setBold(fm.key() == currentPhase);
          fm.value()->setFont(font);
          fm.value()->setText(QString::number(outputs.gvars[fm.key()][gv.key()]));
          prevOutputs.gvars[fm.key()][gv.key()] = outputs.gvars[fm.key()][gv.key()];
          firstGvarsRun = false;
        }
      }
    }
  }

  lastPhase = currentPhase;
}
