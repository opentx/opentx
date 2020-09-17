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
#include "rawitemdatamodels.h"

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

  GeneralSettings &generalSettings = radioData.generalSettings;
  ModelData &model = radioData.models[modelId];

  /*
    Create reusable model specific ui datamodels here and pass pointers as needed
    They should be referenced through filters in each use case
    Code that updates the source for one or more ui datamodels should emit a SIGNAL that can be trapped (see below) and the datamodels refreshed
    WARNING: beware of looping signals and slots and unnecessary data field updates from refreshing datamodels
             where practical make signals granular to only those fields that affect datamodels thus needing to be refreshed
  */
  rawSourceModel = new RawSourceItemModel(&generalSettings, &model, this);
  rawSwitchModel = new RawSwitchItemModel(&generalSettings, &model, this);
  curveModel = new CurveItemModel(&generalSettings, &model, this);
  s1.report("Init");

  SetupPanel * setupPanel = new SetupPanel(this, model, generalSettings, firmware, rawSwitchModel);
  addTab(setupPanel, tr("Setup"));
  s1.report("Setup");

  if (firmware->getCapability(Heli)) {
    HeliPanel * heliPanel = new HeliPanel(this, model, generalSettings, firmware, rawSourceModel);
    addTab(heliPanel, tr("Heli"));
    s1.report("Heli");
  }

  FlightModesPanel * flightModesPanel = new FlightModesPanel(this, model, generalSettings, firmware, rawSwitchModel);
  addTab(flightModesPanel, tr("Flight Modes"));
  s1.report("Flight Modes");

  InputsPanel * inputsPanel = new InputsPanel(this, model, generalSettings, firmware, rawSourceModel, rawSwitchModel);
  addTab(inputsPanel, tr("Inputs"));
  s1.report("Inputs");

  MixesPanel * mixesPanel = new MixesPanel(this, model, generalSettings, firmware, rawSourceModel, rawSwitchModel);
  addTab(mixesPanel, tr("Mixes"));
  s1.report("Mixes");

  ChannelsPanel * channelsPanel = new ChannelsPanel(this, model, generalSettings, firmware, curveModel);
  addTab(channelsPanel, tr("Outputs"));
  s1.report("Outputs");

  CurvesPanel * curvesPanel = new CurvesPanel(this, model, generalSettings, firmware);
  addTab(curvesPanel, tr("Curves"));
  s1.report("Curves");

  LogicalSwitchesPanel * logicalSwitchesPanel = new LogicalSwitchesPanel(this, model, generalSettings, firmware, rawSourceModel, rawSwitchModel);
  addTab(logicalSwitchesPanel, tr("Logical Switches"));
  s1.report("Logical Switches");

  CustomFunctionsPanel * customFunctionsPanel = new CustomFunctionsPanel(this, &model, generalSettings, firmware, rawSourceModel, rawSwitchModel);
  addTab(customFunctionsPanel, tr("Special Functions"));
  s1.report("Special Functions");

  if (firmware->getCapability(Telemetry)) {
    TelemetryPanel * telemetryPanel = new TelemetryPanel(this, model, generalSettings, firmware, rawSourceModel);
    addTab(telemetryPanel, tr("Telemetry"));
    connect(telemetryPanel, &TelemetryPanel::updateDataModels, rawSourceModel, &RawSourceItemModel::update);
    s1.report("Telemetry");
  }

  connect(setupPanel, &SetupPanel::extendedLimitsToggled, channelsPanel, &ChannelsPanel::refreshExtendedLimits);
  connect(setupPanel, &SetupPanel::updateDataModels, rawSourceModel, &RawSourceItemModel::update);

  connect(flightModesPanel, &FlightModesPanel::updateDataModels, rawSourceModel, &RawSourceItemModel::update);
  connect(flightModesPanel, &FlightModesPanel::updateDataModels, rawSwitchModel, &RawSwitchItemModel::update);

  connect(inputsPanel, &InputsPanel::updateDataModels, rawSourceModel, &RawSourceItemModel::update);

  connect(channelsPanel, &ChannelsPanel::updateDataModels, rawSourceModel, &RawSourceItemModel::update);

  connect(curvesPanel, &CurvesPanel::updateDataModels, curveModel, &CurveItemModel::update);

  connect(logicalSwitchesPanel, &LogicalSwitchesPanel::updateDataModels, rawSourceModel, &RawSourceItemModel::update);
  connect(logicalSwitchesPanel, &LogicalSwitchesPanel::updateDataModels, rawSwitchModel, &RawSwitchItemModel::update);

  connect(ui->tabWidget, &QTabWidget::currentChanged, this, &ModelEdit::onTabIndexChanged);
  connect(ui->pushButton, &QPushButton::clicked, this, &ModelEdit::launchSimulation);

  onTabIndexChanged(ui->tabWidget->currentIndex());  // make sure to trigger update on default tab panel

  s1.report("Signals and Slots");
  gStopwatch.report("ModelEdit end constructor");
}

ModelEdit::~ModelEdit()
{
  delete rawSourceModel;
  delete rawSwitchModel;
  delete curveModel;
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
