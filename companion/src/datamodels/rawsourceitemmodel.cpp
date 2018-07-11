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

#include "rawsourceitemmodel.h"
#include "generalsettings.h"
#include "eeprominterface.h"
#include "modeldata.h"

RawSourceItemModel::RawSourceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  QStandardItemModel(parent),
  generalSettings(generalSettings),
  modelData(modelData)
{
  const Boards board = Boards(getCurrentBoard());
  Firmware * fw = getCurrentFirmware();
  const int anasCount = CPN_MAX_STICKS + board.getCapability(Board::Pots) + board.getCapability(Board::Sliders) +  board.getCapability(Board::MouseAnalogs);

  addItems(SOURCE_TYPE_NONE,           RawSource::NoneGroup,     1);
  for (int i=0; i < fw->getCapability(LuaScripts); i++)
    addItems(SOURCE_TYPE_LUA_OUTPUT,   RawSource::ScriptsGroup,  fw->getCapability(LuaOutputsPerScript), i * 16);
  addItems(SOURCE_TYPE_VIRTUAL_INPUT,  RawSource::InputsGroup,   fw->getCapability(VirtualInputs));
  addItems(SOURCE_TYPE_STICK,          RawSource::SourcesGroup,  anasCount);
  addItems(SOURCE_TYPE_ROTARY_ENCODER, RawSource::SourcesGroup,  fw->getCapability(RotaryEncoders));
  addItems(SOURCE_TYPE_TRIM,           RawSource::TrimsGroup,    board.getCapability(Board::NumTrims));
  addItems(SOURCE_TYPE_MAX,            RawSource::SourcesGroup,  1);
  addItems(SOURCE_TYPE_SWITCH,         RawSource::SwitchesGroup, board.getCapability(Board::Switches));
  addItems(SOURCE_TYPE_CUSTOM_SWITCH,  RawSource::SwitchesGroup, fw->getCapability(LogicalSwitches));
  addItems(SOURCE_TYPE_CYC,            RawSource::SourcesGroup,  CPN_MAX_CYC);
  addItems(SOURCE_TYPE_PPM,            RawSource::SourcesGroup,  fw->getCapability(TrainerInputs));
  addItems(SOURCE_TYPE_CH,             RawSource::SourcesGroup,  fw->getCapability(Outputs));
  addItems(SOURCE_TYPE_SPECIAL,        RawSource::TelemGroup,    5);
  addItems(SOURCE_TYPE_TELEMETRY,      RawSource::TelemGroup,    CPN_MAX_SENSORS * 3);
  addItems(SOURCE_TYPE_GVAR,           RawSource::GVarsGroup,    fw->getCapability(Gvars));
}

void RawSourceItemModel::setDynamicItemData(QStandardItem * item, const RawSource & src)
{
  Board::Type board = getCurrentBoard();
  item->setText(src.toString(modelData, generalSettings, board));
  item->setData(src.isAvailable(modelData, generalSettings, board), IsAvailableRole);
}

void RawSourceItemModel::addItems(const RawSourceType & type, const unsigned group, const int count, const int start)
{
  for (int i = start; i < start + count; i++) {
    const RawSource src = RawSource(type, i);
    QStandardItem * modelItem = new QStandardItem();
    modelItem->setData(src.toValue(), ItemIdRole);
    modelItem->setData(type, ItemTypeRole);
    modelItem->setData(group, ItemGroupRole);
    setDynamicItemData(modelItem, src);
    appendRow(modelItem);
  }
}

void RawSourceItemModel::update()
{
  for (int i=0; i < rowCount(); ++i)
    setDynamicItemData(item(i), RawSource(item(i)->data(ItemIdRole).toInt()));
}


//
// RawSourceFilterItemModel
//

RawSourceFilterItemModel::RawSourceFilterItemModel(QAbstractItemModel * sourceModel, unsigned groups, QObject * parent) :
  QSortFilterProxyModel(parent),
  filterGroups(0)
{
  setFilterRole(RawSourceItemModel::IsAvailableRole);
  setFilterKeyColumn(0);
  setFilterGroups(groups);
  setDynamicSortFilter(true);
  setSourceModel(sourceModel);
}

RawSourceFilterItemModel::RawSourceFilterItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, unsigned groups, QObject * parent) :
  RawSourceFilterItemModel(new RawSourceItemModel(generalSettings, modelData, parent), groups, parent)
{
}

void RawSourceFilterItemModel::setFilterGroups(unsigned groups)
{
  if (filterGroups != groups) {
    filterGroups = groups;
    invalidateFilter();
  }
}

bool RawSourceFilterItemModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const
{
  bool ok = true;
  const QModelIndex & srcIdx = sourceModel()->index(sourceRow, 0, sourceParent);
  if (!sourceModel()->data(srcIdx, filterRole()).toBool())
    return false;
  if (filterGroups && filterGroups != RawSource::AllSourceGroups) {
    const unsigned group = sourceModel()->data(srcIdx, RawSourceItemModel::ItemGroupRole).toUInt(&ok);
    return (ok && (filterGroups & group));
  }
  return true;
}

void RawSourceFilterItemModel::update()
{
  RawSourceItemModel * model = qobject_cast<RawSourceItemModel *>(sourceModel());
  if (model)
    model->update();
}
