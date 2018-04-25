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

#define TELEMETRY_VALUE_TIMER_CYCLE    128 /* x160ms ~= 20.5s ; must be multiple of 2 to avoid the modulo */
#define TELEMETRY_VALUE_OLD_THRESHOLD  62 /* x160ms ~= 10s */
#define TELEMETRY_VALUE_UNAVAILABLE    255
#define TELEMETRY_VALUE_OLD            254

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
        uint16_t year;          // full year (4 digits)
        uint8_t  month;
        uint8_t  day;
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
      // 160ms granularity
      return (get_tmr10ms() >> 4) & (TELEMETRY_VALUE_TIMER_CYCLE - 1);
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

    inline bool isAvailable()
    {
      return (lastReceived != TELEMETRY_VALUE_UNAVAILABLE);
    }

    inline bool isOld()
    {
      return (lastReceived == TELEMETRY_VALUE_OLD);
    }

    inline bool hasReceiveTime()
    {
      return (lastReceived < TELEMETRY_VALUE_TIMER_CYCLE);
    }

    inline uint8_t getDelaySinceLastValue()
    {
      // assumes lastReceived is not a special value (OLD / UNAVAILABLE)
      return (now() - lastReceived) & (TELEMETRY_VALUE_TIMER_CYCLE - 1);
    }

    inline bool isFresh()
    {
      return (hasReceiveTime() && getDelaySinceLastValue() <= 1);
    }

    inline void setOld()
    {
      lastReceived = TELEMETRY_VALUE_OLD;
    }

    void gpsReceived(); // TODO seems not used
};

extern TelemetryItem telemetryItems[MAX_TELEMETRY_SENSORS];
extern uint8_t allowNewSensors;
bool isFaiForbidden(source_t idx);
bool isValidIdAndInstance(uint16_t id, uint8_t instance);

#endif // _TELEMETRY_SENSORS_H_
