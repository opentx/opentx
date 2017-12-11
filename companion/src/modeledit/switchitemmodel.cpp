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

RawSwitchItemModel::RawSwitchItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData):
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

void RawSwitchItemModel::add(const RawSwitchType & type, int count)
{
  // Most RawSwitch() indices are one-based (vs. typical zero); these are exceptions to the rule:
  const static QVector<int> rawSwitchIndexBaseZeroTypes = QVector<int>() << SWITCH_TYPE_NONE << SWITCH_TYPE_ON << SWITCH_TYPE_OFF << SWITCH_TYPE_TIMER_MODE;

  int rawIdxAdj = 0;
  const Board::Type board = getCurrentBoard();
  int i = (count < 0 ? count : 1);
  const int maxCount = (i < 0 ? 0 : count + i);

  // handle exceptions in RawSwitch() index values
  if (rawSwitchIndexBaseZeroTypes.contains(type))
    rawIdxAdj = -1;

  for ( ; i < maxCount; ++i) {
    RawSwitch rs(type, i + rawIdxAdj);
    QStandardItem * modelItem = new QStandardItem(rs.toString(board, generalSettings, modelData));
    modelItem->setData(rs.toValue(), Qt::UserRole);
    appendRow(modelItem);
  }
}

void RawSwitchItemModel::update()
{
  for (int i=0; i<rowCount(); i++) {
    QStandardItem * modelItem = item(i, 0);
    RawSwitch swtch(modelItem->data(Qt::UserRole).toInt());
    modelItem->setText(swtch.toString(getCurrentBoard(), generalSettings, modelData));
  }
}

RawSwitchFilterItemModel::RawSwitchFilterItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, SwitchContext context):
  generalSettings(generalSettings),
  modelData(modelData),
  context(context),
  parent(new RawSwitchItemModel(generalSettings, modelData))
{
  setSourceModel(parent);
  setDynamicSortFilter(false);
}

bool RawSwitchFilterItemModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const
{
  QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
  RawSwitch swtch(sourceModel()->data(index, Qt::UserRole).toInt());

  if (swtch.type == SWITCH_TYPE_FLIGHT_MODE && (context == MixesContext || context == GlobalFunctionsContext))
    return false;

  if (swtch.type == SWITCH_TYPE_VIRTUAL && context == GlobalFunctionsContext)
    return false;

  if (swtch.type == SWITCH_TYPE_TIMER_MODE && context != TimersContext)
    return false;

  if (swtch.type == SWITCH_TYPE_NONE && context == TimersContext)
    return false;

  if (swtch.type == SWITCH_TYPE_SENSOR && context == GlobalFunctionsContext)
    return false;

  if ((swtch.type == SWITCH_TYPE_ON || swtch.type == SWITCH_TYPE_ONE) && (context != SpecialFunctionsContext && context != GlobalFunctionsContext))
    return false;

  if (!swtch.isAvailable(modelData, generalSettings, getCurrentBoard()))
    return false;

  return true;
}

void RawSwitchFilterItemModel::update()
{
  parent->update();
  invalidate();
}
