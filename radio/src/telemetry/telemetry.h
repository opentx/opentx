/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

enum TelemetryProtocol
{
  TELEM_PROTO_FRSKY_D,
  TELEM_PROTO_FRSKY_SPORT,
};

#define TELEMETRY_VALUE_TIMER_CYCLE   200 /*20 seconds*/
#define TELEMETRY_VALUE_OLD_THRESHOLD 150 /*15 seconds*/
#define TELEMETRY_VALUE_UNAVAILABLE   255
#define TELEMETRY_VALUE_OLD           254

class TelemetryItem
{
  public:
    int32_t value;           // value, stored as uint32_t but interpreted accordingly to type
    int32_t min;             // min store
    int32_t max;             // max store
    uint8_t lastReceived;    // for detection of sensor loss

    static uint8_t now() {
      return (get_tmr10ms() / 10) % TELEMETRY_VALUE_TIMER_CYCLE;
    }

    TelemetryItem()
    {
      clear();
    }

    void clear()
    {
      lastReceived = TELEMETRY_VALUE_UNAVAILABLE;
      value = min = max = 0;
    }

    void setValue(int32_t value);
    bool isAvailable();
    bool isFresh();
    bool isOld();
};

extern TelemetryItem telemetryItems[TELEM_VALUES_MAX];

inline bool isTelemetryFieldAvailable(int index)
{
  return g_model.telemetryValues[index].id != 0;
}

void setTelemetryLabel(TelemetryValue & telemetryValue, const char *label);
void setTelemetryValue(TelemetryProtocol protocol, uint16_t id, uint8_t instance, int32_t value);
void delTelemetryIndex(uint8_t index);
int availableTelemetryIndex();

void frskySportSetDefault(int index, uint16_t type, uint8_t instance);

#endif
