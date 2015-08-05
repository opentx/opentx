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
    union {
      int32_t  value;           // value, stored as uint32_t but interpreted accordingly to type
      uint32_t distFromEarthAxis;
    };

    union {
      int32_t  valueMin;        // min store
      uint32_t pilotLongitude;
    };

    union {
      int32_t  valueMax;        // max store
      uint32_t pilotLatitude;
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
        uint16_t longitude_bp;
        uint16_t longitude_ap;
        char     longitudeEW;
        uint16_t latitude_bp;
        uint16_t latitude_ap;
        char     latitudeNS;
        // pilot longitude is stored in min
        // pilot latitude is stored in max
        // distFromEarthAxis is stored in value
        void extractLatitudeLongitude(uint32_t * latitude, uint32_t * longitude)
        {
          div_t qr = div(latitude_bp, 100);
          *latitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + latitude_ap) * 5) / 3;
          qr = div(longitude_bp, 100);
          *longitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + longitude_ap) * 5) / 3;
        }
      } gps;
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

extern TelemetryItem telemetryItems[MAX_SENSORS];

inline bool isTelemetryFieldAvailable(int index)
{
  TelemetrySensor & sensor = g_model.telemetrySensors[index];
  return sensor.isAvailable();
}

inline bool isTelemetryFieldComparisonAvailable(int index)
{
  TelemetrySensor & sensor = g_model.telemetrySensors[index];
  if (sensor.type == TELEM_TYPE_CALCULATED)
    return true;
  if (sensor.unit >= UNIT_DATETIME)
    return false;
  return (sensor.id != 0);
}

void setTelemetryValue(TelemetryProtocol protocol, uint16_t id, uint8_t instance, int32_t value, uint32_t unit, uint32_t prec);
void delTelemetryIndex(uint8_t index);
int availableTelemetryIndex();
int lastUsedTelemetryIndex();
int32_t getTelemetryValue(uint8_t index, uint8_t & prec);
int32_t convertTelemetryValue(int32_t value, uint8_t unit, uint8_t prec, uint8_t destUnit, uint8_t destPrec);

void frskySportSetDefault(int index, uint16_t type, uint8_t instance);
void frskyDSetDefault(int index, uint16_t id);

#define IS_DISTANCE_UNIT(unit)         ((unit) == UNIT_METERS || (unit) == UNIT_FEET)
#define IS_SPEED_UNIT(unit)            ((unit) >= UNIT_KTS && (unit) <= UNIT_MPH)

#endif
