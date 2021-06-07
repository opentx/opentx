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

#include "heli.h"
#include "ui_heli.h"
#include "filtereditemmodels.h"
#include "eeprominterface.h"

constexpr char FIM_RAWSOURCE[] {"Raw Source"};

HeliPanel::HeliPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware, CompoundItemModelFactory * sharedItemModels):
  ModelPanel(parent, model, generalSettings, firmware),
  ui(new Ui::Heli)
{
  ui->setupUi(this);
  int fimId;

  tabFilteredModels = new FilteredItemModelFactory();
  fimId = tabFilteredModels->registerItemModel(new FilteredItemModel(sharedItemModels->getItemModel(AbstractItemModel::IMID_RawSource), RawSource::InputSourceGroups), FIM_RAWSOURCE);
  connectItemModelEvents(tabFilteredModels->getItemModel(fimId));

  int simId = sharedItemModels->registerItemModel(SwashRingData::typeItemModel());

  ui->swashType->setModel(sharedItemModels->getItemModel(simId));
  ui->swashType->setField(model.swashRingData.type, this);

  ui->swashRingVal->setField(model.swashRingData.value, this);
  FieldRange val = SwashRingData::getValueRange();
  ui->swashRingVal->setRange(val.min, val.max);

  ui->swashCollectiveSource->setModel(tabFilteredModels->getItemModel(FIM_RAWSOURCE));
  ui->swashCollectiveSource->setField(model.swashRingData.collectiveSource, this);

  if (firmware->getCapability(VirtualInputs)) {
    ui->swashAileronSource->setModel(tabFilteredModels->getItemModel(FIM_RAWSOURCE));
    ui->swashAileronSource->setField(model.swashRingData.aileronSource, this);

    ui->swashElevatorSource->setModel(tabFilteredModels->getItemModel(FIM_RAWSOURCE));
    ui->swashElevatorSource->setField(model.swashRingData.elevatorSource, this);

    FieldRange weight = SwashRingData::getWeightRange();

    ui->swashAileronWeight->setField(model.swashRingData.aileronWeight, this);
    ui->swashAileronWeight->setRange(weight.min, weight.max);

    ui->swashElevatorWeight->setField(model.swashRingData.elevatorWeight, this);
    ui->swashElevatorWeight->setRange(weight.min, weight.max);

    ui->swashCollectiveWeight->setField(model.swashRingData.collectiveWeight, this);
    ui->swashCollectiveWeight->setRange(weight.min, weight.max);

    ui->invertLabel->hide();
    ui->swashElevatorInvert->hide();
    ui->swashAileronInvert->hide();
    ui->swashCollectiveInvert->hide();
  }
  else {
    connect(ui->swashElevatorInvert, SIGNAL(stateChanged(int)), this, SLOT(edited()));
    connect(ui->swashAileronInvert, SIGNAL(stateChanged(int)), this, SLOT(edited()));
    connect(ui->swashCollectiveInvert, SIGNAL(stateChanged(int)), this, SLOT(edited()));
    ui->aileronLabel->hide();
    ui->elevatorLabel->hide();
    ui->swashAileronSource->hide();
    ui->swashElevatorSource->hide();
    ui->swashAileronWeight->hide();
    ui->swashElevatorWeight->hide();
    ui->swashCollectiveWeight->hide();
  }

  disableMouseScrolling();
}

HeliPanel::~HeliPanel()
{
  delete ui;
  delete tabFilteredModels;
}

void HeliPanel::update()
{
  lock = true;

  if (!firmware->getCapability(VirtualInputs)) {
    ui->swashElevatorInvert->setChecked(model->swashRingData.elevatorWeight < 0);
    ui->swashAileronInvert->setChecked(model->swashRingData.aileronWeight < 0);
    ui->swashCollectiveInvert->setChecked(model->swashRingData.collectiveWeight < 0);
  }

  lock = false;
}

void HeliPanel::edited()
{
  if (!lock) {
    if (!firmware->getCapability(VirtualInputs)) {
      model->swashRingData.elevatorWeight = (ui->swashElevatorInvert->isChecked() ? -100 : 100);
      model->swashRingData.aileronWeight = (ui->swashAileronInvert->isChecked() ? -100 : 100);
      model->swashRingData.collectiveWeight = (ui->swashCollectiveInvert->isChecked() ? -100 : 100);
    }
    emit modified();
  }
}

void HeliPanel::connectItemModelEvents(const FilteredItemModel * itemModel)
{
  connect(itemModel, &FilteredItemModel::aboutToBeUpdated, this, &HeliPanel::onItemModelAboutToBeUpdated);
  connect(itemModel, &FilteredItemModel::updateComplete, this, &HeliPanel::onItemModelUpdateComplete);
}

void HeliPanel::onItemModelAboutToBeUpdated()
{
  lock = true;
}

void HeliPanel::onItemModelUpdateComplete()
{
  update();
  lock = false;
}
