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

void SensorData::updateUnit()
{
  if (type == TELEM_TYPE_CALCULATED) {
    if (formula == TELEM_FORMULA_CONSUMPTION)
      unit = UNIT_MAH;
  }
}

QString SensorData::unitString() const
{
  switch (unit) {
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
      return "";
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
