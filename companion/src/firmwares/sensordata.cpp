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
      return trUtf8("°C");
    case UNIT_FAHRENHEIT:
      return trUtf8("°F");
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
      return trUtf8("°");
    case UNIT_RADIANS:
      return trUtf8("Rad");
    case UNIT_HOURS:
      return tr("hours");
    case UNIT_MINUTES:
      return tr("minutes");
    case UNIT_SECONDS:
      return tr("seconds");
    case UNIT_CELLS:
      return tr("V");
    default:
      return "";
  }
}

QString SensorData::nameToString(int index) const
{
  return RadioData::getElementName(tr("TELE"), index + 1, label);
}
