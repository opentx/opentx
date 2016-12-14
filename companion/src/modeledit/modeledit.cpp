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
#include "../helpers.h"
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
  model(radioData.models[modelId]),
  generalSettings(radioData.generalSettings),
  firmware(firmware)
{
  Stopwatch s1("ModelEdit");
  gStopwatch.report("ModelEdit start constructor");

  ui->setupUi(this);
  setWindowIcon(CompanionIcon("edit.png"));
  restoreGeometry(g.modelEditGeo());
  ui->pushButton->setIcon(CompanionIcon("simulate.png"));
  if(GetCurrentFirmware()->getBoard() == BOARD_HORUS && !HORUS_READY_FOR_RELEASE()) {
    ui->pushButton->setEnabled(false);
  }
  SetupPanel * setupPanel = new SetupPanel(this, model, generalSettings, firmware);
  addTab(setupPanel, tr("Setup"));
  if (firmware->getCapability(Heli))
    addTab(new HeliPanel(this, model, generalSettings, firmware), tr("Heli"));
  addTab(new FlightModesPanel(this, model, generalSettings, firmware), tr("Flight Modes"));
  addTab(new InputsPanel(this, model, generalSettings, firmware), tr("Inputs"));
  s1.report("inputs");
  addTab(new MixesPanel(this, model, generalSettings, firmware), tr("Mixes"));
  s1.report("Mixes");
  Channels * chnPanel = new Channels(this, model, generalSettings, firmware);
  addTab(chnPanel, tr("Outputs"));
  s1.report("Outputs");
  addTab(new Curves(this, model, generalSettings, firmware), tr("Curves"));
  addTab(new LogicalSwitchesPanel(this, model, generalSettings, firmware), tr("Logical Switches"));
  s1.report("LS");
  addTab(new CustomFunctionsPanel(this, &model, generalSettings, firmware), tr("Special Functions"));
  s1.report("CF");
  if (firmware->getCapability(Telemetry) & TM_HASTELEMETRY)
    addTab(new TelemetryPanel(this, model, generalSettings, firmware), tr("Telemetry"));

  connect(setupPanel, SIGNAL(extendedLimitsToggled()), chnPanel, SLOT(refreshExtendedLimits()));
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
  connect(panel, SIGNAL(modified()), this, SLOT(onTabModified()));
}

void ModelEdit::onTabModified()
{
  emit modified();
}

void ModelEdit::on_tabWidget_currentChanged(int index)
{
  panels[index]->update();
}

void ModelEdit::on_pushButton_clicked()
{
  if (GetCurrentFirmware()->getBoard() == BOARD_HORUS && !HORUS_READY_FOR_RELEASE()) {
    QMessageBox::warning(this, "Unavailable", "Horus simulation not currently supported within Companion");
  }
  else {
    launchSimulation();
  }
}

void ModelEdit::launchSimulation()
{
  RadioData *simuData = new RadioData();
  simuData->generalSettings = generalSettings;
  simuData->models[0] = model;
  startSimulation(this, *simuData, 0);
  delete simuData;
}
