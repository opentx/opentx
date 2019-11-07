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

#include "rawitemdatamodels.h"
#include "generalsettings.h"
#include "eeprominterface.h"
#include "modeldata.h"

RawSourceItemModel::RawSourceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  AbstractRawItemDataModel(generalSettings, modelData, parent)
{
  const Boards board = Boards(getCurrentBoard());
  Firmware * fw = getCurrentFirmware();

  addItems(SOURCE_TYPE_NONE,           RawSource::NoneGroup,     1);
  for (int i=0; i < fw->getCapability(LuaScripts); i++)
    addItems(SOURCE_TYPE_LUA_OUTPUT,   RawSource::ScriptsGroup,  fw->getCapability(LuaOutputsPerScript), i * 16);
  addItems(SOURCE_TYPE_VIRTUAL_INPUT,  RawSource::InputsGroup,   fw->getCapability(VirtualInputs));
  addItems(SOURCE_TYPE_STICK,          RawSource::SourcesGroup,  board.getCapability(Board::MaxAnalogs));
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

void RawSourceItemModel::setDynamicItemData(QStandardItem * item, const RawSource & src) const
{
  Board::Type board = getCurrentBoard();
  item->setText(src.toString(modelData, generalSettings, board));
  item->setData(src.isAvailable(modelData, generalSettings, board), IsAvailableRole);
}

void RawSourceItemModel::addItems(const RawSourceType & type, const int group, const int count, const int start)
{
  for (int i = start; i < start + count; i++) {
    const RawSource src = RawSource(type, i);
    QStandardItem * modelItem = new QStandardItem();
    modelItem->setData(src.toValue(), ItemIdRole);
    modelItem->setData(type, ItemTypeRole);
    modelItem->setData(group, ItemFlagsRole);
    setDynamicItemData(modelItem, src);
    appendRow(modelItem);
  }
}

void RawSourceItemModel::update() const
{
  for (int i=0; i < rowCount(); ++i)
    setDynamicItemData(item(i), RawSource(item(i)->data(ItemIdRole).toInt()));
}


//
// RawSwitchItemModel
//

RawSwitchItemModel::RawSwitchItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  AbstractRawItemDataModel(generalSettings, modelData, parent)
{
  Boards board = Boards(getCurrentBoard());
  Firmware * fw = getCurrentFirmware();

  // Descending switch direction: NOT (!) switches
  addItems(SWITCH_TYPE_ACT,            -1);
  addItems(SWITCH_TYPE_SENSOR,         -CPN_MAX_SENSORS);
  addItems(SWITCH_TYPE_TELEMETRY,      -1);
  addItems(SWITCH_TYPE_FLIGHT_MODE,    -fw->getCapability(FlightModes));
  addItems(SWITCH_TYPE_VIRTUAL,        -fw->getCapability(LogicalSwitches));
  addItems(SWITCH_TYPE_ROTARY_ENCODER, -fw->getCapability(RotaryEncoders));
  addItems(SWITCH_TYPE_TRIM,           -board.getCapability(Board::NumTrimSwitches));
  addItems(SWITCH_TYPE_MULTIPOS_POT,   -(board.getCapability(Board::MultiposPots) * board.getCapability(Board::MultiposPotsPositions)));
  addItems(SWITCH_TYPE_SWITCH,         -board.getCapability(Board::SwitchPositions));

  // Ascending switch direction (including zero)
  addItems(SWITCH_TYPE_TIMER_MODE, 5);
  addItems(SWITCH_TYPE_NONE, 1);
  addItems(SWITCH_TYPE_SWITCH,         board.getCapability(Board::SwitchPositions));
  addItems(SWITCH_TYPE_MULTIPOS_POT,   board.getCapability(Board::MultiposPots) * board.getCapability(Board::MultiposPotsPositions));
  addItems(SWITCH_TYPE_TRIM,           board.getCapability(Board::NumTrimSwitches));
  addItems(SWITCH_TYPE_ROTARY_ENCODER, fw->getCapability(RotaryEncoders));
  addItems(SWITCH_TYPE_VIRTUAL,        fw->getCapability(LogicalSwitches));
  addItems(SWITCH_TYPE_FLIGHT_MODE,    fw->getCapability(FlightModes));
  addItems(SWITCH_TYPE_TELEMETRY,      1);
  addItems(SWITCH_TYPE_SENSOR,         CPN_MAX_SENSORS);
  addItems(SWITCH_TYPE_ON,             1);
  addItems(SWITCH_TYPE_ONE,            1);
  addItems(SWITCH_TYPE_ACT,            1);
}

void RawSwitchItemModel::setDynamicItemData(QStandardItem * item, const RawSwitch & rsw) const
{
  const Board::Type board = getCurrentBoard();
  item->setText(rsw.toString(board, generalSettings, modelData));
  item->setData(rsw.isAvailable(modelData, generalSettings, board), IsAvailableRole);
}

void RawSwitchItemModel::addItems(const RawSwitchType & type, int count)
{
  // Most RawSwitch() indices are one-based (vs. typical zero); these are exceptions to the rule:
  const static QVector<int> rawSwitchIndexBaseZeroTypes = QVector<int>() << SWITCH_TYPE_NONE << SWITCH_TYPE_ON << SWITCH_TYPE_OFF << SWITCH_TYPE_TIMER_MODE;
  // handle exceptions in RawSwitch() index values
  const short rawIdxAdj = rawSwitchIndexBaseZeroTypes.contains(type) ? -1 : 0;

  // determine cotext flags
  int context = RawSwitch::AllSwitchContexts;
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
    modelItem->setData(context, ItemFlagsRole);
    setDynamicItemData(modelItem, rs);
    appendRow(modelItem);
  }
}

void RawSwitchItemModel::update() const
{
  for (int i=0; i < rowCount(); ++i)
    setDynamicItemData(item(i), RawSwitch(item(i)->data(ItemIdRole).toInt()));
}
