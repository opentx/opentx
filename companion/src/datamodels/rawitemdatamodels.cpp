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
#include "adjustmentreference.h"

RawSourceItemModel::RawSourceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  AbstractRawItemDataModel(generalSettings, modelData, parent)
{
  const Boards board = Boards(getCurrentBoard());
  Firmware * fw = getCurrentFirmware();

  addItems(SOURCE_TYPE_NONE,           RawSource::NoneGroup,     1);
  for (int i = 0; i < fw->getCapability(LuaScripts); i++)
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
  addItems(SOURCE_TYPE_TELEMETRY,      RawSource::TelemGroup,    fw->getCapability(Sensors) * 3);
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

void RawSourceItemModel::update()
{
  emit dataAboutToBeUpdated();

  for (int i = 0; i < rowCount(); ++i)
    setDynamicItemData(item(i), RawSource(item(i)->data(ItemIdRole).toInt()));

  emit dataUpdateComplete();
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
  addItems(SWITCH_TYPE_SENSOR,         -fw->getCapability(Sensors));
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
  addItems(SWITCH_TYPE_SENSOR,         fw->getCapability(Sensors));
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

void RawSwitchItemModel::update()
{
  emit dataAboutToBeUpdated();

  for (int i = 0; i < rowCount(); ++i)
    setDynamicItemData(item(i), RawSwitch(item(i)->data(ItemIdRole).toInt()));

  emit dataUpdateComplete();
}

//
// CurveItemModel
//

CurveItemModel::CurveItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  AbstractRawItemDataModel(generalSettings, modelData, parent)
{
  if (!modelData)
    return;

  const int count = getCurrentFirmware()->getCapability(NumCurves);

  for (int i = -count ; i <= count; ++i) {
    QStandardItem * modelItem = new QStandardItem();
    modelItem->setData(i, ItemIdRole);
    int flags = i < 0 ? DataGroups::NegativeGroup : DataGroups::PositiveGroup;
    if (i == 0)
      flags |= DataGroups::NoneGroup;
    modelItem->setData(flags, ItemFlagsRole);
    setDynamicItemData(modelItem, i);
    appendRow(modelItem);
  }
}

void CurveItemModel::setDynamicItemData(QStandardItem * item, const int index) const
{
  CurveReference cr = CurveReference(CurveReference::CURVE_REF_CUSTOM, index);
  item->setText(cr.toString(modelData, false));
  item->setData(cr.isAvailable(), IsAvailableRole);
}

void CurveItemModel::update()
{
  emit dataAboutToBeUpdated();

  for (int i = 0; i < rowCount(); ++i)
   setDynamicItemData(item(i), item(i)->data(ItemIdRole).toInt());

  emit dataUpdateComplete();
}


//
// GVarReferenceItemModel
//

GVarReferenceItemModel::GVarReferenceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  AbstractRawItemDataModel(generalSettings, modelData, parent)
{
  if (!modelData)
    return;

  const int count = getCurrentFirmware()->getCapability(Gvars);
  if (count > 0) {
    addItems(-count);
    addItems(count);
  }
}

void GVarReferenceItemModel::addItems(int count)
{
  int i = (count < 0 ? count : 1);
  count = (i < 0 ? 0 : count + i);
  for ( ; i < count; ++i) {
    QStandardItem * modelItem = new QStandardItem();
    AdjustmentReference ar(AdjustmentReference::ADJUST_REF_GVAR, i);
    modelItem->setData(ar.toValue(), ItemIdRole);
    modelItem->setData(i < 0 ? DataGroups::NegativeGroup : DataGroups::PositiveGroup, ItemFlagsRole);
    setDynamicItemData(modelItem, ar);
    appendRow(modelItem);
  }
}

void GVarReferenceItemModel::setDynamicItemData(QStandardItem * item, const AdjustmentReference & ar) const
{
  item->setText(ar.toString(modelData, false));
  item->setData(ar.isAvailable(), IsAvailableRole);
}

void GVarReferenceItemModel::update()
{
  emit dataAboutToBeUpdated();

  for (int i = 0; i < rowCount(); ++i)
   setDynamicItemData(item(i), AdjustmentReference(item(i)->data(ItemIdRole).toInt()));

  emit dataUpdateComplete();
}


//
// ThrottleSourceItemModel
//

ThrottleSourceItemModel::ThrottleSourceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  AbstractRawItemDataModel(generalSettings, modelData, parent)
{
  if (!modelData)
    return;

  for (int i = 0; i < modelData->thrTraceSrcCount(); i++) {
    QStandardItem * modelItem = new QStandardItem();
    modelItem->setData(i, ItemIdRole);
    setDynamicItemData(modelItem, i);
    appendRow(modelItem);
  }
}

void ThrottleSourceItemModel::setDynamicItemData(QStandardItem * item, const int index) const
{
  item->setText(modelData->thrTraceSrcToString(index));
  item->setData(modelData->isThrTraceSrcAvailable(generalSettings, index), IsAvailableRole);
}

void ThrottleSourceItemModel::update()
{
  emit dataAboutToBeUpdated();

  for (int i = 0; i < rowCount(); ++i)
    setDynamicItemData(item(i), item(i)->data(ItemIdRole).toInt());

  emit dataUpdateComplete();
}


//
// CustomFuncActionItemModel
//

CustomFuncActionItemModel::CustomFuncActionItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  AbstractRawItemDataModel(generalSettings, modelData, parent)
{
  for (int i = 0; i < AssignFunc::FuncCount; i++) {
    QStandardItem * modelItem = new QStandardItem();
    modelItem->setData(i, ItemIdRole);
    modelItem->setData(CustomFunctionData::funcContext(i), ItemFlagsRole);
    setDynamicItemData(modelItem, i);
    appendRow(modelItem);
  }
}

void CustomFuncActionItemModel::setDynamicItemData(QStandardItem * item, const int index) const
{
  item->setText(CustomFunctionData(AssignFunc(index)).funcToString(modelData));
  item->setData(CustomFunctionData::isFuncAvailable(index), IsAvailableRole);
}

void CustomFuncActionItemModel::update()
{
  emit dataAboutToBeUpdated();

  for (int i = 0; i < rowCount(); ++i)
    setDynamicItemData(item(i), item(i)->data(ItemIdRole).toInt());

  emit dataUpdateComplete();
}


//
// CustomFuncResetParamItemModel
//

CustomFuncResetParamItemModel::CustomFuncResetParamItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  AbstractRawItemDataModel(generalSettings, modelData, parent)
{
  for (int i = 0; i < CustomFunctionData::resetParamCount(modelData); i++) {
    QStandardItem * modelItem = new QStandardItem();
    modelItem->setData(i, ItemIdRole);
    setDynamicItemData(modelItem, i);
    appendRow(modelItem);
  }
}

void CustomFuncResetParamItemModel::setDynamicItemData(QStandardItem * item, const int index) const
{
  CustomFunctionData cfd = CustomFunctionData(AssignFunc::FuncReset);
  cfd.param = index;
  item->setText(cfd.paramToString(modelData));
  item->setData(CustomFunctionData::isResetParamAvailable(modelData, index), IsAvailableRole);
}

void CustomFuncResetParamItemModel::update()
{
  emit dataAboutToBeUpdated();

  for (int i = 0; i < rowCount(); ++i)
    setDynamicItemData(item(i), item(i)->data(ItemIdRole).toInt());

  emit dataUpdateComplete();
}


//
// TelemetrySourceItemModel
//

TelemetrySourceItemModel::TelemetrySourceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  AbstractRawItemDataModel(generalSettings, modelData, parent)
{
  if (!modelData)
    return;

  const int count = getCurrentFirmware()->getCapability(Sensors);

  for (int i = -count + 1; i < count; ++i) {
    QStandardItem * modelItem = new QStandardItem();
    modelItem->setData(i, ItemIdRole);
    int flags = i < 0 ? DataGroups::NegativeGroup : DataGroups::PositiveGroup;
    if (i == 0)
      flags |= DataGroups::NoneGroup;
    modelItem->setData(flags, ItemFlagsRole);
    setDynamicItemData(modelItem, i);
    appendRow(modelItem);
  }
}

void TelemetrySourceItemModel::setDynamicItemData(QStandardItem * item, const int index) const
{
  item->setText(SensorData::sourceToString(modelData, index));
  item->setData(SensorData::isSourceAvailable(modelData, index), IsAvailableRole);
}

void TelemetrySourceItemModel::update()
{
  emit dataAboutToBeUpdated();

  for (int i = 0; i < rowCount(); ++i)
   setDynamicItemData(item(i), item(i)->data(ItemIdRole).toInt());

  emit dataUpdateComplete();
}


//
//  CommonItemModels  TODO  rename to DynamicDataModels
//

CommonItemModels::CommonItemModels(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent) :
  QObject(parent)
{
  m_rawSourceItemModel = new RawSourceItemModel(generalSettings, modelData, parent);
  //dumpModelContents(QString("raw source"), qobject_cast<AbstractRawItemDataModel *>(m_rawSourceItemModel));

  m_rawSwitchItemModel = new RawSwitchItemModel(generalSettings, modelData, parent);
  //dumpModelContents(QString("raw switch"), qobject_cast<AbstractRawItemDataModel *>(m_rawSwitchItemModel));

  m_curveItemModel = new CurveItemModel(generalSettings, modelData, parent);
  //dumpModelContents(QString("curve"), qobject_cast<AbstractRawItemDataModel *>(m_curveItemModel));

  m_gvarReferenceItemModel = new GVarReferenceItemModel(generalSettings, modelData, parent);
  //dumpModelContents(QString("gvar ref"), qobject_cast<AbstractRawItemDataModel *>(m_gvarReferenceItemModel));

  m_throttleSourceItemModel = new ThrottleSourceItemModel(generalSettings, modelData, parent);
  //dumpModelContents(QString("thr source"), qobject_cast<AbstractRawItemDataModel *>(m_throttleSourceItemModel));

  m_customFuncActionItemModel = new CustomFuncActionItemModel(generalSettings, modelData, parent);
  //dumpModelContents(QString("cf action"), qobject_cast<AbstractRawItemDataModel *>(m_customFuncActionItemModel));

  m_customFuncResetParamItemModel = new CustomFuncResetParamItemModel(generalSettings, modelData, parent);
  //dumpModelContents(QString("cf reset param"), qobject_cast<AbstractRawItemDataModel *>(m_customFuncResetParamItemModel));

  m_telemetrySourceItemModel = new TelemetrySourceItemModel(generalSettings, modelData, parent);
  //dumpModelContents(QString("tele source"), qobject_cast<AbstractRawItemDataModel *>(m_telemetrySourceItemModel));
}

CommonItemModels::~CommonItemModels()
{
}

void CommonItemModels::update(const RadioModelObjects radioModelObjects)
{
  switch (radioModelObjects) {
    case RMO_CHANNELS:
      m_throttleSourceItemModel->update();
      //  no break
    case RMO_TIMERS:
      m_customFuncActionItemModel->update();
      //  no break
    case RMO_INPUTS:
    case RMO_TELEMETRY_SENSORS:
      m_rawSourceItemModel->update();
      m_customFuncResetParamItemModel->update();
      m_telemetrySourceItemModel->update();
      break;

    case RMO_GLOBAL_VARIABLES:
      m_gvarReferenceItemModel->update();
      m_customFuncActionItemModel->update();
      //  no break
    case RMO_FLIGHT_MODES:
    case RMO_LOGICAL_SWITCHES:
      m_rawSourceItemModel->update();
      m_rawSwitchItemModel->update();
      break;

    case RMO_CURVES:
      m_curveItemModel->update();
      break;

    case RMO_SCRIPTS:
      //  no need to refresh
      break;

    default:
      qDebug() << "Unknown RadioModelObject:" << radioModelObjects;
  }
}


void dumpModelContents(QString desc, AbstractRawItemDataModel * dataModel)
{
  if (dataModel) {
    qDebug() << "model:" << desc << "rows:" << dataModel->rowCount();
    for (int i = 0; i < dataModel->rowCount(); ++i) {
      qDebug() << "row:"    << i << "text:" << dataModel->item(i)->data(Qt::DisplayRole).toString()
               << "id:"     << dataModel->item(i)->data(AbstractRawItemDataModel::ItemIdRole).toInt()
               << "avail:"  << dataModel->item(i)->data(AbstractRawItemDataModel::IsAvailableRole).toBool()
               << "flags:"  << dataModel->item(i)->data(AbstractRawItemDataModel::ItemFlagsRole).toInt()
               << "type:"   << dataModel->item(i)->data(AbstractRawItemDataModel::ItemTypeRole).toInt();
    }
  }
  else
    qDebug() << "model:" << desc << "did not cast";
}

CurveRefTypeItemModel::CurveRefTypeItemModel(QObject * parent) :
  AbstractStandardItemModel(parent)
{
  for (int i = 0; i <= CurveReference::MAX_CURVE_REF_TYPE; i++) {
    QStandardItem * modelItem = new QStandardItem();
    modelItem->setText(CurveReference::typeToString((CurveReference::CurveRefType)i));
    modelItem->setData(i, ItemIdRole);
    modelItem->setData(CurveReference::isTypeAvailable((CurveReference::CurveRefType)i), IsAvailableRole);
    appendRow(modelItem);
  }
}

CurveRefFuncItemModel::CurveRefFuncItemModel(QObject * parent) :
  AbstractStandardItemModel(parent)
{
  for (int i = 1; i <= CurveReference::functionCount(); i++) {
    QStandardItem * modelItem = new QStandardItem();
    modelItem->setText(CurveReference::functionToString(i));
    modelItem->setData(i, ItemIdRole);
    modelItem->setData(CurveReference::isFunctionAvailable(i), IsAvailableRole);
    appendRow(modelItem);
  }
}
