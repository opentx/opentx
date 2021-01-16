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

#include "sensordata.h"

#include "radiodata.h"
#include "modeldata.h"
#include "eeprominterface.h"
#include "compounditemmodels.h"

void SensorData::updateUnit()
{
  if (type == TELEM_TYPE_CALCULATED) {
    if (formula == TELEM_FORMULA_CONSUMPTION)
      unit = UNIT_MAH;
  }
}

//  TODO depreciated
QString SensorData::unitString() const
{
  return unitToString(unit);
}

//  static
QString SensorData::unitToString(const int value)
{
  switch (value) {
    case UNIT_RAW:
      return tr("Raw (-)");
    case UNIT_VOLTS:
      return tr("V");
    case UNIT_AMPS:
      return tr("A");
    case UNIT_MILLIAMPS:
      return tr("mA");
    case UNIT_KTS:
      return tr("kts");
    case UNIT_METERS_PER_SECOND:
      return tr("m/s");
    case UNIT_KMH:
      return tr("km/h");
    case UNIT_MPH:
      return tr("mph");
    case UNIT_METERS:
      return tr("m");
    case UNIT_FEET:
      return tr("f");
    case UNIT_CELSIUS:
      return tr("°C");
    case UNIT_FAHRENHEIT:
      return tr("°F");
    case UNIT_PERCENT:
      return tr("%");
    case UNIT_MAH:
      return tr("mAh");
    case UNIT_WATTS:
      return tr("W");
    case UNIT_MILLIWATTS:
        return tr("mW");
    case UNIT_DB:
      return tr("dB");
    case UNIT_RPMS:
      return tr("rpms");
    case UNIT_G:
      return tr("g");
    case UNIT_DEGREE:
      return tr("°");
    case UNIT_RADIANS:
      return tr("Rad");
    case UNIT_HOURS:
      return tr("hours");
    case UNIT_MINUTES:
      return tr("minutes");
    case UNIT_SECONDS:
      return tr("seconds");
    case UNIT_CELLS:
      return tr("V");
    case UNIT_MILLILITERS_PER_MINUTE:
      return tr("ml/minute");
    case UNIT_HERZ:
      return tr("Hertz");
    case UNIT_MS:
      return tr("mS");
    case UNIT_US:
      return tr("uS");
    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

QString SensorData::nameToString(int index) const
{
  return RadioData::getElementName(tr("TELE"), index + 1, label);
}

QString SensorData::getOrigin(const ModelData * model) const
{
  if (type != TELEM_TYPE_CUSTOM || !id)
    return QString();

  const ModuleData & module = model->moduleData[moduleIdx];
  if (module.isPxx2Module() && rxIdx <= 2 && module.access.receivers & (1 << rxIdx)) {
    return QString(module.access.receiverName[rxIdx]);
  }
  else {
    return QString();
  }
}

bool SensorData::isEmpty() const
{
  return (!isAvailable() && type == 0 && id == 0 && subid == 0 && instance == 0 && rxIdx == 0 && moduleIdx == 0 && unit == 0 && ratio == 0 && prec == 0 && offset == 0 && autoOffset == 0 && filter == 0 && onlyPositive == 0 && logs == 0);
}

//  static
QString SensorData::typeToString(const int value)
{
  switch(value) {
    case TELEM_TYPE_CUSTOM:
      return tr("Custom");
    case TELEM_TYPE_CALCULATED:
      return tr("Calculated");
    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

//  static
QString SensorData::formulaToString(const int value)
{
  switch(value) {
    case TELEM_FORMULA_ADD:
      return tr("Add");
    case TELEM_FORMULA_AVERAGE:
      return tr("Average");
    case TELEM_FORMULA_MIN:
      return tr("Minimum");
    case TELEM_FORMULA_MAX:
      return tr("Maximum");
    case TELEM_FORMULA_MULTIPLY:
      return tr("Multiply");
    case TELEM_FORMULA_TOTALIZE:
      return tr("Totalize");
    case TELEM_FORMULA_CELL:
      return tr("Cell");
    case TELEM_FORMULA_CONSUMPTION:
      return tr("Consumption");
    case TELEM_FORMULA_DIST:
      return tr("Distance");
    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

//  static
QString SensorData::cellIndexToString(const int value)
{
  if (value == TELEM_CELL_INDEX_LOWEST)
    return tr("Lowest");
  if (value > TELEM_CELL_INDEX_LOWEST && value < TELEM_CELL_INDEX_HIGHEST)
    return tr("Cell %1").arg(value - TELEM_CELL_INDEX_LOWEST);
  if (value== TELEM_CELL_INDEX_HIGHEST)
    return tr("Highest");
  if (value == TELEM_CELL_INDEX_DELTA)
    return tr("Delta");
  return CPN_STR_UNKNOWN_ITEM;
}

//  static
QString SensorData::precisionToString(const int value)
{
  return QString("0.%1").arg(QString(value, '0'));
}

//  static
QString SensorData::sourceToString(const ModelData * model, const int index, const bool sign)
{
  if (model) {
    const QString prfx = sign ? index < 0 ? "-" : "+" : "";

    if (abs(index) > 0) {
      const SensorData &sd = model->sensorData[abs(index) - 1];
      if (sd.type == SensorData::TELEM_TYPE_CUSTOM)
        return QString("%1%2 (%3)").arg(prfx).arg(sd.label).arg(sd.getOrigin(model));
      else
        return QString("%1%2").arg(prfx).arg(sd.label);
    }
    else
      return CPN_STR_NONE_ITEM;
  }

  return "";
}

//  static
bool SensorData::isSourceAvailable(const ModelData * model, const int index)
{
  Firmware * firmware = getCurrentFirmware();
  const int sensorcnt = firmware->getCapability(Sensors);
  const int i = abs(index);

  if (model && sensorcnt > 0 && i <= sensorcnt) {
    if (i > 0)
      return model->sensorData[i - 1].isAvailable();
    else
      return true;
  }

  return false;
}

//  static
AbstractStaticItemModel * SensorData::typeItemModel()
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName("sensordata.type");

  for (int i = 0; i <= TELEM_TYPE_LAST; i++) {
    mdl->appendToItemList(typeToString(i), i);
  }

  mdl->loadItemList();
  return mdl;
}

//  static
AbstractStaticItemModel * SensorData::formulaItemModel()
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName("sensordata.formula");

  for (int i = 0; i <= TELEM_FORMULA_LAST; i++) {
    mdl->appendToItemList(formulaToString(i), i);
  }

  mdl->loadItemList();
  return mdl;
}

//  static
AbstractStaticItemModel * SensorData::cellIndexItemModel()
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName("sensordata.cellindex");

  for (int i = 0; i <= TELEM_CELL_INDEX_LAST; i++) {
    mdl->appendToItemList(cellIndexToString(i), i);
  }

  mdl->loadItemList();
  return mdl;
}

//  static
AbstractStaticItemModel * SensorData::unitItemModel()
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName("sensordata.unit");

  for (int i = 0; i <= UNIT_MAX; i++) {
    QString str = unitToString(i);
    if (str != CPN_STR_UNKNOWN_ITEM)
      mdl->appendToItemList(str, i);
  }

  mdl->loadItemList();
  return mdl;
}

//  static
PrecisionItemModel * SensorData::precisionItemModel()
{
  PrecisionItemModel * mdl = new PrecisionItemModel(0, 2);
  mdl->setName("sensordata.precision");
  return mdl;
}
