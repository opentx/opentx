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

void FilteredTelemetryValue::reset()
{
  memclear(this, sizeof(*this));
}

void FilteredTelemetryValue::set(uint8_t value)
{
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
}

void TelemetryValueWithMin::reset()
{
  memclear(this, sizeof(*this));
}

void TelemetryValueWithMin::set(uint8_t value)
{
  FilteredTelemetryValue::set(value);
  if (!min || value < min) {
    min = value;
  }
}
