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

#include "modeledit.h"
#include "helpers.h"
#include "ui_modeledit.h"
#include "setup.h"
#include "heli.h"
#include "flightmodes.h"
#include "inputs.h"
#include "mixes.h"
#include "channels.h"
#include "curves.h"
#include "verticalscrollarea.h"
#include "logicalswitches.h"
#include "customfunctions.h"
#include "telemetry.h"
#include "appdata.h"

ModelEdit::ModelEdit(QWidget * parent, RadioData & radioData, int modelId, Firmware * firmware) :
  QDialog(parent),
  ui(new Ui::ModelEdit),
  modelId(modelId),
  radioData(radioData),
  firmware(firmware)
{
  Stopwatch s1("ModelEdit");
  gStopwatch.report("ModelEdit start constructor");

  ui->setupUi(this);
  setWindowIcon(CompanionIcon("edit.png"));
  restoreGeometry(g.modelEditGeo());
  ui->pushButton->setIcon(CompanionIcon("simulate.png"));
  SetupPanel * setupPanel = new SetupPanel(this, radioData.models[modelId], radioData.generalSettings, firmware);
  addTab(setupPanel, tr("Setup"));
  if (firmware->getCapability(Heli))
    addTab(new HeliPanel(this, radioData.models[modelId], radioData.generalSettings, firmware), tr("Heli"));
  addTab(new FlightModesPanel(this, radioData.models[modelId], radioData.generalSettings, firmware), tr("Flight Modes"));
  addTab(new InputsPanel(this, radioData.models[modelId], radioData.generalSettings, firmware), tr("Inputs"));
  s1.report("inputs");
  addTab(new MixesPanel(this, radioData.models[modelId], radioData.generalSettings, firmware), tr("Mixes"));
  s1.report("Mixes");
  Channels * chnPanel = new Channels(this, radioData.models[modelId], radioData.generalSettings, firmware);
  addTab(chnPanel, tr("Outputs"));
  s1.report("Outputs");
  addTab(new Curves(this, radioData.models[modelId], radioData.generalSettings, firmware), tr("Curves"));
  addTab(new LogicalSwitchesPanel(this, radioData.models[modelId], radioData.generalSettings, firmware), tr("Logical Switches"));
  s1.report("LS");
  addTab(new CustomFunctionsPanel(this, &radioData.models[modelId], radioData.generalSettings, firmware), tr("Special Functions"));
  s1.report("CF");
  if (firmware->getCapability(Telemetry) & TM_HASTELEMETRY)
    addTab(new TelemetryPanel(this, radioData.models[modelId], radioData.generalSettings, firmware), tr("Telemetry"));

  onTabIndexChanged(ui->tabWidget->currentIndex());  // make sure to trigger update on default tab panel

  connect(setupPanel, &SetupPanel::extendedLimitsToggled, chnPanel, &Channels::refreshExtendedLimits);
  connect(ui->tabWidget, &QTabWidget::currentChanged, this, &ModelEdit::onTabIndexChanged);
  connect(ui->pushButton, &QPushButton::clicked, this, &ModelEdit::launchSimulation);

  s1.report("end");
  gStopwatch.report("ModelEdit end constructor");
}

ModelEdit::~ModelEdit()
{
  delete ui;
}

void ModelEdit::closeEvent(QCloseEvent *event)
{
  g.modelEditGeo( saveGeometry() );
}

void ModelEdit::addTab(GenericPanel *panel, QString text)
{
  panels << panel;
  QWidget * widget = new QWidget(ui->tabWidget);
  QVBoxLayout *baseLayout = new QVBoxLayout(widget);
  VerticalScrollArea * area = new VerticalScrollArea(widget, panel);
  baseLayout->addWidget(area);
  ui->tabWidget->addTab(widget, text);
  connect(panel, &GenericPanel::modified, this, &ModelEdit::modified);
}

void ModelEdit::onTabIndexChanged(int index)
{
  if (index < panels.size())
    panels.at(index)->update();
}

void ModelEdit::launchSimulation()
{
  startSimulation(this, radioData, modelId);
}
