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

#include "itemmodelfactory.h"

#include "rawitemdatamodels.h"
#include "rawitemfilteredmodel.h"

#include "eeprominterface.h"
#include "generalsettings.h"
#include "modeldata.h"

ItemModelFactory::ItemModelFactory(const GeneralSettings * const generalSettings, const ModelData * const modelData) :
  generalSettings(generalSettings),
  modelData(modelData)
{
  firmware = getCurrentFirmware();
  board = new Boards(getCurrentBoard());
  boardType = getCurrentBoard();
}

ItemModelFactory::~ItemModelFactory()
{
  unregisterItemModels();
  delete board;
}

void ItemModelFactory::addItemModel(const AbstractItemModel::ItemModelId id)
{
  switch (id) {
    case AbstractItemModel::RawSourceId:
      registerItemModel(new RawSourceItemModel(generalSettings, modelData, firmware, board, boardType));
      break;
    case AbstractItemModel::RawSwitchId:
      registerItemModel(new RawSwitchItemModel(generalSettings, modelData, firmware, board, boardType));
      break;
    case AbstractItemModel::CurveId:
      registerItemModel(new CurveItemModel(generalSettings, modelData, firmware, board, boardType));
      break;
    case AbstractItemModel::GVarRefId:
      registerItemModel(new GVarReferenceItemModel(generalSettings, modelData, firmware, board, boardType));
      break;
    case AbstractItemModel::ThrSourceId:
      registerItemModel(new ThrottleSourceItemModel(generalSettings, modelData, firmware, board, boardType));
      break;
    case AbstractItemModel::CustomFuncActionId:
      registerItemModel(new CustomFuncActionItemModel(generalSettings, modelData, firmware, board, boardType));
      break;
    case AbstractItemModel::CustomFuncResetParamId:
      registerItemModel(new CustomFuncResetParamItemModel(generalSettings, modelData, firmware, board, boardType));
      break;
    case AbstractItemModel::TeleSourceId:
      registerItemModel(new TelemetrySourceItemModel(generalSettings, modelData, firmware, board, boardType));
      break;
    case AbstractItemModel::CurveRefTypeId:
      registerItemModel(new CurveRefTypeItemModel(generalSettings, modelData, firmware, board, boardType));
      break;
    case AbstractItemModel::CurveRefFuncId:
      registerItemModel(new CurveRefFuncItemModel(generalSettings, modelData, firmware, board, boardType));
      break;
    default:
      qDebug() << "Error: unknown item model: id";
      break;
  }
}

void ItemModelFactory::registerItemModel(QAbstractItemModel * itemModel)
{
  if (itemModel && !getItemModel(itemModel->getId())) {
    registeredItemModels.push_back(itemModel);
  }
}

void ItemModelFactory::unregisterItemModels()
{
  foreach (QAbstractItemModel *itemModel, registeredItemModels) {
    delete itemModel;
  }
}

void ItemModelFactory::unregisterItemModel(const AbstractItemModel::ItemModelId id)
{
  QAbstractItemModel * itemModel =  getItemModel(id);
  if (itemModel)
    delete itemModel;
}

QAbstractItemModel * ItemModelsFactory::getItemModel(const AbstractItemModel::ItemModelId id) const
{
  foreach (AbstractItemModel * itemModel, registeredItemModels) {
    if (itemModel->getId() == id)
      return itemModel;
  }

  return nullptr;
}

void ItemModelFactory::update(const AbstractItemModel::UpdateTrigger trigger)
{
  foreach (AbstractItemModel * itemModel, registeredItemModels) {
    itemModel->update(trigger);
  }
}

void ItemModelFactory::dumpAllItemModelContents() const
{
  foreach (AbstractItemModel * itemModel, registeredItemModels) {
    AbstractItemModel::dumpItemModelContents(itemModel);
  }
}

//
// FilteredItemModelsFactory
//

FilteredItemModelFactory::FilteredItemModelFactory()
{
}

FilteredItemModelFactory::~FilteredItemModelFactory()
{
  unregisterItemModels();
}

int FilteredItemModelFactory::registerItemModel(FilteredItemModel * itemModel, QString name)
{
  if (itemModel) {
    if (isItemModelRegistered(name))
      qDebug() << "Warning: item model already registered with name:" << name;
    const int id = registeredItemModels.count();
    itemModel->setId(id);
    itemModel->setName(name);
    registeredItemModels.push_back(itemModel);
    return id;
  }
  return 0;
}

void FilteredItemModelFactory::unregisterItemModels()
{
  foreach (FilteredItemModel * itemModel, registeredItemModels) {
    delete itemModel;
  }
}

void FilteredItemModelFactory::unregisterItemModel(const int id)
{
  FilteredItemModel * itemModel =  getItemModel(id);
  if (itemModel)
    delete itemModel;
}

bool FilteredItemModelFactory::isItemModelRegistered(const int id) const
{
  return getItemModel(id);
}

bool FilteredItemModelFactory::isItemModelRegistered(const QString name) const
{
  return getItemModel(name);
}

FilteredItemModel * FilteredItemModelFactory::getItemModel(const int id) const
{
  foreach (FilteredItemModel * itemModel, registeredItemModels) {
    if (itemModel->getId() == id)
      return itemModel;
  }

  return nullptr;
}

FilteredItemModel * FilteredItemModelFactory::getItemModel(const QString name) const
{
  foreach (FilteredItemModel * itemModel, registeredItemModels) {
    if (itemModel->getName() == name)
      return itemModel;
  }

  return nullptr;
}

void FilteredItemModelFactory::update()
{
  foreach (FilteredItemModel * itemModel, registeredItemModels) {
    itemModel->update();
  }
}

void FilteredItemModelFactory::dumpItemModelContents(const int id) const
{
  FilteredItemModel::dumpItemModelContents(getItemModel(id));
}

void FilteredItemModelFactory::dumpItemModelContents(const QString name) const
{
  FilteredItemModel::dumpItemModelContents(getItemModel(name));
}

void FilteredItemModelFactory::dumpAllItemModelContents() const
{
  foreach (FilteredItemModel * itemModel, registeredItemModels) {
    FilteredItemModel::dumpItemModelContents(itemModel);
  }
}

