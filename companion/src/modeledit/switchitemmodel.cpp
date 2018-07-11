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

#include "switchitemmodel.h"
#include "eeprominterface.h"

RawSwitchItemModel::RawSwitchItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  QStandardItemModel(parent),
  generalSettings(generalSettings),
  modelData(modelData)
{
  Boards board = Boards(getCurrentBoard());
  Board::Type btype = board.getBoardType();
  Firmware * fw = getCurrentFirmware();

  // Descending switch direction: NOT (!) switches
  if (IS_ARM(btype)) {
    add(SWITCH_TYPE_SENSOR, -CPN_MAX_SENSORS);
    add(SWITCH_TYPE_TELEMETRY, -1);
    add(SWITCH_TYPE_FLIGHT_MODE, -fw->getCapability(FlightModes));
  }
  add(SWITCH_TYPE_VIRTUAL, -fw->getCapability(LogicalSwitches));
  add(SWITCH_TYPE_ROTARY_ENCODER, -fw->getCapability(RotaryEncoders));
  add(SWITCH_TYPE_TRIM, -board.getCapability(Board::NumTrimSwitches));
  add(SWITCH_TYPE_MULTIPOS_POT, -(board.getCapability(Board::MultiposPots) * board.getCapability(Board::MultiposPotsPositions)));
  add(SWITCH_TYPE_SWITCH, -board.getCapability(Board::SwitchPositions));

  // Ascending switch direction (including zero)
  add(SWITCH_TYPE_TIMER_MODE, 5);
  add(SWITCH_TYPE_NONE, 1);
  add(SWITCH_TYPE_SWITCH, board.getCapability(Board::SwitchPositions));
  add(SWITCH_TYPE_MULTIPOS_POT, board.getCapability(Board::MultiposPots) * board.getCapability(Board::MultiposPotsPositions));
  add(SWITCH_TYPE_TRIM, board.getCapability(Board::NumTrimSwitches));
  add(SWITCH_TYPE_ROTARY_ENCODER, fw->getCapability(RotaryEncoders));
  add(SWITCH_TYPE_VIRTUAL, fw->getCapability(LogicalSwitches));
  if (IS_ARM(btype)) {
    add(SWITCH_TYPE_FLIGHT_MODE, fw->getCapability(FlightModes));
    add(SWITCH_TYPE_TELEMETRY, 1);
    add(SWITCH_TYPE_SENSOR, CPN_MAX_SENSORS);
  }
  add(SWITCH_TYPE_ON, 1);
  add(SWITCH_TYPE_ONE, 1);
}

void RawSwitchItemModel::setDynamicItemData(QStandardItem * item, const RawSwitch & rsw) const
{
  const Board::Type board = getCurrentBoard();
  item->setText(rsw.toString(board, generalSettings, modelData));
  item->setData(rsw.isAvailable(modelData, generalSettings, board), IsAvailableRole);
}

void RawSwitchItemModel::add(const RawSwitchType & type, int count)
{
  // Most RawSwitch() indices are one-based (vs. typical zero); these are exceptions to the rule:
  const static QVector<int> rawSwitchIndexBaseZeroTypes = QVector<int>() << SWITCH_TYPE_NONE << SWITCH_TYPE_ON << SWITCH_TYPE_OFF << SWITCH_TYPE_TIMER_MODE;

  // handle exceptions in RawSwitch() index values
  const short rawIdxAdj = rawSwitchIndexBaseZeroTypes.contains(type) ? -1 : 0;

  // determine cotext flags
  unsigned context = RawSwitch::AllSwitchContexts;
  switch (type) {
    case SWITCH_TYPE_FLIGHT_MODE:
      context &= ~RawSwitch::MixesContext;
    // fallthrough
    case SWITCH_TYPE_VIRTUAL:
    case SWITCH_TYPE_SENSOR:
      context &= ~RawSwitch::GlobalFunctionsContext;
      break;

    case SWITCH_TYPE_TIMER_MODE:
      context = RawSwitch::TimersContext;
      break;

    case SWITCH_TYPE_NONE:
      context &= ~RawSwitch::TimersContext;
      break;

    case SWITCH_TYPE_ON:
    case SWITCH_TYPE_ONE:
      context = RawSwitch::SpecialFunctionsContext | RawSwitch::GlobalFunctionsContext;
      break;

    default:
      break;
  }

  int i = (count < 0 ? count : 1);
  count = (i < 0 ? 0 : count + i);
  for ( ; i < count; ++i) {
    RawSwitch rs(type, i + rawIdxAdj);
    QStandardItem * modelItem = new QStandardItem();
    modelItem->setData(rs.toValue(), ItemIdRole);
    modelItem->setData(type, ItemTypeRole);
    modelItem->setData(context, ContextRole);
    setDynamicItemData(modelItem, rs);
    appendRow(modelItem);
  }
}

void RawSwitchItemModel::update() const
{
  for (int i=0; i < rowCount(); ++i)
    setDynamicItemData(item(i), RawSwitch(item(i)->data(ItemIdRole).toInt()));
}


//
// RawSwitchFilterItemModel
//

RawSwitchFilterItemModel::RawSwitchFilterItemModel(QAbstractItemModel * sourceModel, RawSwitch::SwitchContext context, QObject * parent) :
  QSortFilterProxyModel(parent)
{
  setFilterRole(RawSwitchItemModel::IsAvailableRole);
  setFilterKeyColumn(0);
  setSwitchContext(context);
  setDynamicSortFilter(true);
  setSourceModel(sourceModel);
}

RawSwitchFilterItemModel::RawSwitchFilterItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, RawSwitch::SwitchContext context, QObject * parent):
  RawSwitchFilterItemModel(new RawSwitchItemModel(generalSettings, modelData, parent), context, parent)
{
}

void RawSwitchFilterItemModel::setSwitchContext(RawSwitch::SwitchContext ctxt)
{
  if (ctxt != context) {
    context = ctxt;
    invalidateFilter();
  }
}

bool RawSwitchFilterItemModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const
{
  const QModelIndex & srcIdx = sourceModel()->index(sourceRow, 0, sourceParent);
  if (!srcIdx.isValid() || !sourceModel()->data(srcIdx, filterRole()).toBool())
    return false;

  bool ok;
  const RawSwitch::SwitchContext ctxt = RawSwitch::SwitchContext(sourceModel()->data(srcIdx, RawSwitchItemModel::ContextRole).toUInt(&ok));
  return (ok && (ctxt == RawSwitch::AllSwitchContexts || context != RawSwitch::NoSwitchContext || (context & ctxt)));
}

void RawSwitchFilterItemModel::update() const
{
  RawSwitchItemModel * model = qobject_cast<RawSwitchItemModel *>(sourceModel());
  if (model)
    model->update();
}
