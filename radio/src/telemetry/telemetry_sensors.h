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

#ifndef _TELEMETRY_SENSORS_H_
#define _TELEMETRY_SENSORS_H_

#include "telemetry.h"

class TelemetryItem
{
  public:
    union {
      int32_t  value;           // value, stored as uint32_t but interpreted accordingly to type
      uint32_t distFromEarthAxis;
    };

    union {
      int32_t valueMin;         // min store
      int32_t pilotLongitude;
    };

    union {
      int32_t valueMax;         // max store
      int32_t pilotLatitude;
    };

    uint8_t lastReceived;       // for detection of sensor loss

    union {
      struct {
        int32_t  offsetAuto;
        int32_t  filterValues[TELEMETRY_AVERAGE_COUNT];
      } std;
      struct {
        uint16_t prescale;
      } consumption;
      struct {
        uint8_t   count;
        CellValue values[6];
      } cells;
      struct {
        uint8_t  datestate;
        uint16_t year;
        uint8_t  month;
        uint8_t  day;
        uint8_t  timestate;
        uint8_t  hour;
        uint8_t  min;
        uint8_t  sec;
      } datetime;
      struct {
        int32_t latitude;
        int32_t longitude;
        // pilot longitude is stored in min
        // pilot latitude is stored in max
        // distFromEarthAxis is stored in value
      } gps;
      char text[16];
    };

    static uint8_t now()
    {
      return (get_tmr10ms() / 10) % TELEMETRY_VALUE_TIMER_CYCLE;
    }

    TelemetryItem()
    {
      clear();
    }

    void clear()
    {
      memset(this, 0, sizeof(*this));
      lastReceived = TELEMETRY_VALUE_UNAVAILABLE;
    }

    void eval(const TelemetrySensor & sensor);
    void per10ms(const TelemetrySensor & sensor);

    void setValue(const TelemetrySensor & sensor, int32_t newVal, uint32_t unit, uint32_t prec=0);
    bool isAvailable();
    bool isFresh();
    bool isOld();
    void gpsReceived();
};

extern TelemetryItem telemetryItems[MAX_TELEMETRY_SENSORS];
extern uint8_t allowNewSensors;

#endif // _TELEMETRY_SENSORS_H_
