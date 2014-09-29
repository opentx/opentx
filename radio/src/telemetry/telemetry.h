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

#define TELEMETRY_AVERAGE_COUNT       3

PACK(struct CellValue
{
  uint16_t value:15;
  uint16_t state:1;

  void set(uint16_t value)
  {
    if (value > 50) {
      this->value = value;
      this->state = 1;
    }
  }
});

class TelemetryItem
{
  public:
    int32_t value;           // value, stored as uint32_t but interpreted accordingly to type
    int32_t min;             // min store
    int32_t max;             // max store
    uint8_t lastReceived;    // for detection of sensor loss

    union {
      int32_t  offsetAuto;
      int32_t  filterValues[TELEMETRY_AVERAGE_COUNT];
      struct {
        uint8_t  count;
        CellValue values[6];
      } cells;
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
      lastReceived = TELEMETRY_VALUE_UNAVAILABLE;
      value = min = max = 0;
    }

    void eval();

    void setValue(int32_t value, uint8_t flags=0);
    bool isAvailable();
    bool isFresh();
    bool isOld();
};

extern TelemetryItem telemetryItems[TELEM_VALUES_MAX];

inline bool isTelemetryFieldAvailable(int index)
{
  TelemetrySensor & sensor = g_model.telemetrySensors[index];
  return sensor.type == TELEM_TYPE_CALCULATED || sensor.id != 0;
}

void setTelemetryValue(TelemetryProtocol protocol, uint16_t id, uint8_t instance, int32_t value, uint32_t flags=0);
void delTelemetryIndex(uint8_t index);
int availableTelemetryIndex();

void frskySportSetDefault(int index, uint16_t type, uint8_t instance);

#endif
