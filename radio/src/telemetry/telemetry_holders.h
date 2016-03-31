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

#ifndef _TELEMETRY_HOLDERS_H_
#define _TELEMETRY_HOLDERS_H_

#include <inttypes.h>

#if defined(CPUARM)
#define TELEMETRY_AVERAGE_COUNT   3     // we actually average one more reading!
#define RAW_FRSKY_MINMAX(v)       v.values[TELEMETRY_AVERAGE_COUNT-1]
class TelemetryValueWithMin {
  public:
    uint8_t value;      // fitered value (average of last TELEMETRY_AVERAGE_COUNT+1 values)
    uint8_t min;
    uint8_t values[TELEMETRY_AVERAGE_COUNT];
    void set(uint8_t value);
    void reset();
};
#else
#define RAW_FRSKY_MINMAX(v)       v.value
class TelemetryValueWithMin {
  public:
    uint8_t value;
    uint8_t min;
    uint16_t sum;
    void set(uint8_t value);
};
#endif

class TelemetryValueWithMinMax: public TelemetryValueWithMin {
  public:
    uint8_t max;
    void set(uint8_t value, uint8_t unit);
};

#endif // _TELEMETRY_HOLDERS_H_