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
      return QObject::tr("V");
    case UNIT_AMPS:
      return QObject::tr("A");
    case UNIT_MILLIAMPS:
      return QObject::tr("mA");
    case UNIT_KTS:
      return QObject::tr("kts");
    case UNIT_METERS_PER_SECOND:
      return QObject::tr("m/s");
    case UNIT_KMH:
      return QObject::tr("km/h");
    case UNIT_MPH:
      return QObject::tr("mph");
    case UNIT_METERS:
      return QObject::tr("m");
    case UNIT_FEET:
      return QObject::tr("f");
    case UNIT_CELSIUS:
      return QObject::trUtf8("°C");
    case UNIT_FAHRENHEIT:
      return QObject::trUtf8("°F");
    case UNIT_PERCENT:
      return QObject::tr("%");
    case UNIT_MAH:
      return QObject::tr("mAh");
    case UNIT_WATTS:
      return QObject::tr("W");
    case UNIT_MILLIWATTS:
        return QObject::tr("mW");
    case UNIT_DB:
      return QObject::tr("dB");
    case UNIT_RPMS:
      return QObject::tr("rpms");
    case UNIT_G:
      return QObject::tr("g");
    case UNIT_DEGREE:
      return QObject::trUtf8("°");
    case UNIT_RADIANS:
      return QObject::trUtf8("Rad");
    case UNIT_HOURS:
      return QObject::tr("hours");
    case UNIT_MINUTES:
      return QObject::tr("minutes");
    case UNIT_SECONDS:
      return QObject::tr("seconds");
    case UNIT_CELLS:
      return QObject::tr("V");
    default:
      return "";
  }
}
