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

#include "opentx.h"

#if defined(CPUARM)
void TelemetryValueWithMin::reset()
{
  memclear(this, sizeof(*this));
}
#endif

void TelemetryValueWithMin::set(uint8_t value)
{
#if defined(CPUARM)
  if (this->value == 0) {
    memset(values, value, TELEMETRY_AVERAGE_COUNT);
    this->value = value;
  }
  else {
    //calculate the average from values[] and value
    //also shift readings in values [] array
    unsigned int sum = values[0];
    for (int i=0; i<TELEMETRY_AVERAGE_COUNT-1; i++) {
      uint8_t tmp = values[i+1];
      values[i] = tmp;
      sum += tmp;
    }
    values[TELEMETRY_AVERAGE_COUNT-1] = value;
    sum += value;
    this->value = sum/(TELEMETRY_AVERAGE_COUNT+1);
  }
#else
  if (this->value == 0) {
    this->value = value;
  }
  else {
    sum += value;
    if (link_counter == 0) {
      this->value = sum / (IS_FRSKY_D_PROTOCOL() ? FRSKY_D_AVERAGING : FRSKY_SPORT_AVERAGING);
      sum = 0;
    }
  }
#endif

  if (!min || value < min) {
    min = value;
  }
}

void TelemetryValueWithMinMax::set(uint8_t value, uint8_t unit)
{
  TelemetryValueWithMin::set(value);
  if (unit != UNIT_VOLTS) {
    this->value = value;
  }
  if (!max || value > max) {
    max = value;
  }
}
