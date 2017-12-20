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

#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <QtCore>

#define CPN_MAX_SENSORS       32

class SensorData {
  Q_DECLARE_TR_FUNCTIONS(SensorData)

  public:

    enum
    {
      TELEM_TYPE_CUSTOM,
      TELEM_TYPE_CALCULATED
    };

    enum
    {
      TELEM_FORMULA_ADD,
      TELEM_FORMULA_AVERAGE,
      TELEM_FORMULA_MIN,
      TELEM_FORMULA_MAX,
      TELEM_FORMULA_MULTIPLY,
      TELEM_FORMULA_TOTALIZE,
      TELEM_FORMULA_CELL,
      TELEM_FORMULA_CONSUMPTION,
      TELEM_FORMULA_DIST,
      TELEM_FORMULA_LAST = TELEM_FORMULA_DIST
    };

    enum {
      TELEM_CELL_INDEX_LOWEST,
      TELEM_CELL_INDEX_1,
      TELEM_CELL_INDEX_2,
      TELEM_CELL_INDEX_3,
      TELEM_CELL_INDEX_4,
      TELEM_CELL_INDEX_5,
      TELEM_CELL_INDEX_6,
      TELEM_CELL_INDEX_HIGHEST,
      TELEM_CELL_INDEX_DELTA,
    };

    enum
    {
      UNIT_RAW,
      UNIT_VOLTS,
      UNIT_AMPS,
      UNIT_MILLIAMPS,
      UNIT_KTS,
      UNIT_METERS_PER_SECOND,
      UNIT_FEET_PER_SECOND,
      UNIT_KMH,
      UNIT_MPH,
      UNIT_METERS,
      UNIT_FEET,
      UNIT_CELSIUS,
      UNIT_FAHRENHEIT,
      UNIT_PERCENT,
      UNIT_MAH,
      UNIT_WATTS,
      UNIT_MILLIWATTS,
      UNIT_DB,
      UNIT_RPMS,
      UNIT_G,
      UNIT_DEGREE,
      UNIT_RADIANS,
      UNIT_MILLILITERS,
      UNIT_FLOZ,
      UNIT_HOURS,
      UNIT_MINUTES,
      UNIT_SECONDS,
      // FrSky format used for these fields, could be another format in the future
        UNIT_FIRST_VIRTUAL,
      UNIT_CELLS = UNIT_FIRST_VIRTUAL,
      UNIT_DATETIME,
      UNIT_GPS,
      UNIT_GPS_LONGITUDE,
      UNIT_GPS_LATITUDE,
      UNIT_GPS_LONGITUDE_EW,
      UNIT_GPS_LATITUDE_NS,
      UNIT_DATETIME_YEAR,
      UNIT_DATETIME_DAY_MONTH,
      UNIT_DATETIME_HOUR_MIN,
      UNIT_DATETIME_SEC
    };

    SensorData() { clear(); }
    unsigned int type; // custom / formula
    unsigned int id;
    unsigned int subid;
    unsigned int instance;
    unsigned int persistentValue;
    unsigned int formula;
    char label[4+1];
    unsigned int unit;
    unsigned int prec;
    bool autoOffset;
    bool filter;
    bool logs;
    bool persistent;
    bool onlyPositive;

    // for custom sensors
    unsigned int ratio;
    int offset;

    // for consumption
    unsigned int amps;

    // for cell
    unsigned int source;
    unsigned int index;

    // for calculations
    int sources[4];

    // for GPS dist
    unsigned int gps;
    unsigned int alt;

    bool isAvailable() const { return strlen(label) > 0; }
    void updateUnit();
    QString unitString() const;
    QString nameToString(int index) const;
    void clear() { memset(this, 0, sizeof(SensorData)); }
};

#endif // SENSORDATA_H
