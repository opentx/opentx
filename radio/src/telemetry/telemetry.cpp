#include "../opentx.h"

TelemetryItem telemetryItems[MAX_SENSORS];

void TelemetryItem::gpsReceived()
{
  if (!distFromEarthAxis) {
    gps.extractLatitudeLongitude(&pilotLatitude, &pilotLongitude);
    uint32_t lat = pilotLatitude / 10000;
    uint32_t angle2 = (lat*lat) / 10000;
    uint32_t angle4 = angle2 * angle2;
    distFromEarthAxis = 139*(((uint32_t)10000000-((angle2*(uint32_t)123370)/81)+(angle4/25))/12500);
  }
  lastReceived = now();
}

void TelemetryItem::setValue(const TelemetrySensor & sensor, int32_t val, uint32_t unit, uint32_t prec)
{
  int32_t newVal = val;

  if (unit == UNIT_CELLS) {
    uint32_t data = uint32_t(newVal);
    uint8_t cellsCount = (data >> 24);
    uint8_t cellIndex = ((data >> 16) & 0x0F);
    uint16_t cellValue = (data & 0xFFFF);
    if (cellsCount == 0) {
      cellsCount = (cellIndex >= cells.count ? cellIndex + 1 : cells.count);
      if (cellsCount != cells.count) {
        clear();
        cells.count = cellsCount;
        // we skip this round as we are not sure we received all cells values
        return;
      }
    }
    else if (cellsCount != cells.count) {
      clear();
      cells.count = cellsCount;
    }
    cells.values[cellIndex].set(cellValue);
    if (cellIndex+1 == cells.count) {
      newVal = 0;
      for (int i=0; i<cellsCount; i++) {
        if (cells.values[i].state) {
          newVal += cells.values[i].value;
        }
        else {
          // we didn't receive all cells values
          return;
        }
      }
      newVal = sensor.getValue(newVal, UNIT_VOLTS, 2);
    }
    else {
      // we didn't receive all cells values
      return;
    }
  }
  else if (unit == UNIT_DATETIME) {
    uint32_t data = uint32_t(newVal);
    if (data & 0x000000ff) {
      datetime.year = (uint16_t) ((data & 0xff000000) >> 24);
      datetime.month = (uint8_t) ((data & 0x00ff0000) >> 16);
      datetime.day = (uint8_t) ((data & 0x0000ff00) >> 8);
      if (datetime.year != 0) {
        datetime.datestate = 1;
      }
#if defined(RTCLOCK)
      if (g_eeGeneral.adjustRTC && (datetime.datestate == 1)) {
        struct gtm t;
        gettime(&t);
        t.tm_year = datetime.year+4;
        t.tm_mon = datetime.month-1;
        t.tm_mday = datetime.day;
        rtcSetTime(&t);
      }
#endif
    }
    else {
      datetime.hour = ((uint8_t) ((data & 0xff000000) >> 24) + g_eeGeneral.timezone + 24) % 24;
      datetime.min = (uint8_t) ((data & 0x00ff0000) >> 16);
      datetime.sec = (uint16_t) ((data & 0x0000ff00) >> 8);
      if (datetime.datestate == 1) {
        datetime.timestate = 1;
      }
#if defined(RTCLOCK)
      if (g_eeGeneral.adjustRTC && datetime.datestate == 1) {
        struct gtm t;
        gettime(&t);
        if (abs((t.tm_hour-datetime.hour)*3600 + (t.tm_min-datetime.min)*60 + (t.tm_sec-datetime.sec)) > 20) {
          // we adjust RTC only if difference is > 20 seconds
          t.tm_hour = datetime.hour;
          t.tm_min = datetime.min;
          t.tm_sec = datetime.sec;
          g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
          rtcSetTime(&t);
        }
      }
#endif
    }
    if (datetime.year == 0) {
      return;
    }
    newVal = 0;
  }
  else if (unit == UNIT_GPS) {
    uint32_t gps_long_lati_data = uint32_t(newVal);
    uint32_t gps_long_lati_b1w, gps_long_lati_a1w;
    gps_long_lati_b1w = (gps_long_lati_data & 0x3fffffff) / 10000;
    gps_long_lati_a1w = (gps_long_lati_data & 0x3fffffff) % 10000;
    switch ((gps_long_lati_data & 0xc0000000) >> 30) {
      case 0:
        gps.latitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        gps.latitude_ap = gps_long_lati_a1w;
        gps.latitudeNS = 'N';
        break;
      case 1:
        gps.latitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        gps.latitude_ap = gps_long_lati_a1w;
        gps.latitudeNS = 'S';
        break;
      case 2:
        gps.longitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        gps.longitude_ap = gps_long_lati_a1w;
        gps.longitudeEW = 'E';
        break;
      case 3:
        gps.longitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        gps.longitude_ap = gps_long_lati_a1w;
        gps.longitudeEW = 'W';
        break;
    }
    if (gps.longitudeEW && gps.latitudeNS) {
      gpsReceived();
    }
    return;
  }
  else if (unit >= UNIT_GPS_LONGITUDE && unit <= UNIT_GPS_LATITUDE_NS) {
    uint32_t data = uint32_t(newVal);
    switch (unit) {
      case UNIT_GPS_LONGITUDE:
        gps.longitude_bp = data >> 16;
        gps.longitude_ap = data & 0xFFFF;
        break;
      case UNIT_GPS_LATITUDE:
        gps.latitude_bp = data >> 16;
        gps.latitude_ap = data & 0xFFFF;
        break;
      case UNIT_GPS_LONGITUDE_EW:
        gps.longitudeEW = data;
        break;
      case UNIT_GPS_LATITUDE_NS:
        gps.latitudeNS = data;
        break;
    }
    if (gps.longitudeEW && gps.latitudeNS && gps.longitude_ap && gps.latitude_ap) {
      gpsReceived();
    }
    return;
  }
  else if (unit == UNIT_DATETIME_YEAR) {
    datetime.year = newVal;
    return;
  }
  else if (unit == UNIT_DATETIME_DAY_MONTH) {
    uint32_t data = uint32_t(newVal);
    datetime.month = data >> 8;
    datetime.day = data & 0xFF;
    datetime.datestate = 1;
    return;
  }
  else if (unit == UNIT_DATETIME_HOUR_MIN) {
    uint32_t data = uint32_t(newVal);
    datetime.hour = ((data & 0xFF) + g_eeGeneral.timezone + 24) % 24;
    datetime.min = data >> 8;
  }
  else if (unit == UNIT_DATETIME_SEC) {
    datetime.sec = newVal & 0xFF;
    datetime.timestate = 1;
    newVal = 0;
  }
  else if (unit == UNIT_RPMS) {
    if (sensor.custom.ratio != 0) {
      newVal = (newVal * sensor.custom.offset) / sensor.custom.ratio;
    }
  }
  else {
    newVal = sensor.getValue(newVal, unit, prec);
    if (sensor.autoOffset) {
      if (!isAvailable()) {
        std.offsetAuto = -newVal;
      }
      newVal += std.offsetAuto;
    }
    else if (sensor.filter) {
      if (!isAvailable()) {
        for (int i=0; i<TELEMETRY_AVERAGE_COUNT; i++) {
          std.filterValues[i] = newVal;
        }
      }
      else {
        // calculate the average from values[] and value
        // also shift readings in values [] array
        unsigned int sum = std.filterValues[0];
        for (int i=0; i<TELEMETRY_AVERAGE_COUNT-1; i++) {
          int32_t tmp = std.filterValues[i+1];
          std.filterValues[i] = tmp;
          sum += tmp;
        }
        std.filterValues[TELEMETRY_AVERAGE_COUNT-1] = newVal;
        sum += newVal;
        newVal = sum/(TELEMETRY_AVERAGE_COUNT+1);
      }
    }
  }

  if (!isAvailable()) {
    valueMin = newVal;
    valueMax = newVal;
  }
  else if (newVal < valueMin) {
    valueMin = newVal;
  }
  else if (newVal > valueMax) {
    valueMax = newVal;
    if (sensor.unit == UNIT_VOLTS) {
      valueMin = newVal; // the batt was changed
    }
  }

  for (int i=0; i<MAX_SENSORS; i++) {
    TelemetrySensor & it = g_model.telemetrySensors[i];
    if (it.type == TELEM_TYPE_CALCULATED && it.formula == TELEM_FORMULA_TOTALIZE && &g_model.telemetrySensors[it.consumption.source-1] == &sensor) {
      TelemetryItem & item = telemetryItems[i];
      int32_t increment = it.getValue(val, unit, prec);
      item.setValue(it, item.value+increment, it.unit, it.prec);
    }
  }

  value = newVal;
  lastReceived = now();
}

bool TelemetryItem::isAvailable()
{
  return (lastReceived != TELEMETRY_VALUE_UNAVAILABLE);
}

bool TelemetryItem::isFresh()
{
  return (lastReceived < TELEMETRY_VALUE_TIMER_CYCLE) && (uint8_t(now() - lastReceived) < 2);
}

bool TelemetryItem::isOld()
{
  return (lastReceived == TELEMETRY_VALUE_OLD);
}

void TelemetryItem::per10ms(const TelemetrySensor & sensor)
{
  switch (sensor.formula) {
    case TELEM_FORMULA_CONSUMPTION:
      if (sensor.consumption.source) {
        TelemetrySensor & currentSensor = g_model.telemetrySensors[sensor.consumption.source-1];
        TelemetryItem & currentItem = telemetryItems[sensor.consumption.source-1];
        if (!currentItem.isAvailable()) {
          return;
        }
        else if (currentItem.isOld()) {
          lastReceived = TELEMETRY_VALUE_OLD;
          return;
        }
        int32_t current = convertTelemetryValue(currentItem.value, currentSensor.unit, currentSensor.prec, UNIT_AMPS, 1);
        currentItem.consumption.prescale += current;
        if (currentItem.consumption.prescale >= 3600) {
          currentItem.consumption.prescale -= 3600;
          setValue(sensor, value+1, sensor.unit, sensor.prec);
        }
        lastReceived = now();
      }
      break;

    default:
      break;
  }
}

void TelemetryItem::eval(const TelemetrySensor & sensor)
{
  switch (sensor.formula) {
    case TELEM_FORMULA_CELL:
      if (sensor.cell.source) {
        TelemetryItem & cellsItem = telemetryItems[sensor.cell.source-1];
        if (cellsItem.isOld()) {
          lastReceived = TELEMETRY_VALUE_OLD;
        }
        else {
          unsigned int index = sensor.cell.index;
          if (index == TELEM_CELL_INDEX_LOWEST || index == TELEM_CELL_INDEX_HIGHEST || index == TELEM_CELL_INDEX_DELTA) {
            unsigned int lowest=0, highest=0;
            for (int i=0; i<cellsItem.cells.count; i++) {
              if (cellsItem.cells.values[i].state) {
                if (!lowest || cellsItem.cells.values[i].value < cellsItem.cells.values[lowest-1].value)
                  lowest = i+1;
                if (!highest || cellsItem.cells.values[i].value > cellsItem.cells.values[highest-1].value)
                  highest = i+1;
              }
              else {
                lowest = highest = 0;
              }
            }
            if (lowest) {
              switch (index) {
                case TELEM_CELL_INDEX_LOWEST:
                  setValue(sensor, cellsItem.cells.values[lowest-1].value, UNIT_VOLTS, 2);
                  break;
                case TELEM_CELL_INDEX_HIGHEST:
                  setValue(sensor, cellsItem.cells.values[highest-1].value, UNIT_VOLTS, 2);
                  break;
                case TELEM_CELL_INDEX_DELTA:
                  setValue(sensor, cellsItem.cells.values[highest-1].value - cellsItem.cells.values[lowest-1].value, UNIT_VOLTS, 2);
                  break;
              }
            }
          }
          else {
            index -= 1;
            if (index < cellsItem.cells.count && cellsItem.cells.values[index].state) {
              setValue(sensor, cellsItem.cells.values[index].value, UNIT_VOLTS, 2);
            }
          }
        }
      }
      break;

    case TELEM_FORMULA_DIST:
      if (sensor.dist.gps) {
        TelemetryItem gpsItem = telemetryItems[sensor.dist.gps-1];
        TelemetryItem * altItem = NULL;
        if (!gpsItem.isAvailable()) {
          return;
        }
        else if (gpsItem.isOld()) {
          lastReceived = TELEMETRY_VALUE_OLD;
          return;
        }
        if (sensor.dist.alt) {
          altItem = &telemetryItems[sensor.dist.alt-1];
          if (!altItem->isAvailable()) {
            return;
          }
          else if (altItem->isOld()) {
            lastReceived = TELEMETRY_VALUE_OLD;
            return;
          }
        }
        uint32_t latitude, longitude;
        gpsItem.gps.extractLatitudeLongitude(&latitude, &longitude);

        uint32_t angle = (latitude > gpsItem.pilotLatitude) ? latitude - gpsItem.pilotLatitude : gpsItem.pilotLatitude - latitude;
        uint32_t dist = EARTH_RADIUS * angle / 1000000;
        uint32_t result = dist*dist;

        angle = (longitude > gpsItem.pilotLongitude) ? longitude - gpsItem.pilotLongitude : gpsItem.pilotLongitude - longitude;
        dist = gpsItem.distFromEarthAxis * angle / 1000000;
        result += dist*dist;

        if (altItem) {
          dist = abs(altItem->value);
          uint8_t prec = g_model.telemetrySensors[sensor.dist.alt-1].prec;
          if (prec > 0)
            dist /= (prec==2 ? 100 : 10);
          result += dist*dist;
        }

        setValue(sensor, isqrt32(result), UNIT_METERS);
      }
      break;

    case TELEM_FORMULA_ADD:
    case TELEM_FORMULA_AVERAGE:
    case TELEM_FORMULA_MIN:
    case TELEM_FORMULA_MAX:
    case TELEM_FORMULA_MULTIPLY:
    {
      int32_t value=0, count=0, available=0, maxitems=4, mulprec=0;
      if (sensor.formula == TELEM_FORMULA_MULTIPLY) {
        maxitems = 2;
        value = 1;
      }
      for (int i=0; i<maxitems; i++) {
        int8_t source = sensor.calc.sources[i];
        if (source) {
          unsigned int index = abs(source)-1;
          TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index];
          TelemetryItem & telemetryItem = telemetryItems[index];
          if (sensor.formula == TELEM_FORMULA_AVERAGE) {
            if (telemetryItem.isAvailable())
              available = 1;
            else
              continue;
            if (telemetryItem.isOld())
              continue;
          }
          else {
            if (!telemetryItem.isAvailable()) {
              return;
            }
            else if (telemetryItem.isOld()) {
              lastReceived = TELEMETRY_VALUE_OLD;
              return;
            }
          }
          int32_t sensorValue = telemetryItem.value;
          if (source < 0)
            sensorValue = -sensorValue;
          count += 1;
          if (sensor.formula == TELEM_FORMULA_MULTIPLY) {
            mulprec += telemetrySensor.prec;
            value *= convertTelemetryValue(sensorValue, telemetrySensor.unit, 0, sensor.unit, 0);
          }
          else {
            sensorValue = convertTelemetryValue(sensorValue, telemetrySensor.unit, telemetrySensor.prec, sensor.unit, sensor.prec);
            if (sensor.formula == TELEM_FORMULA_MIN)
              value = (count==1 ? sensorValue : min<int32_t>(value, sensorValue));
            else if (sensor.formula == TELEM_FORMULA_MAX)
              value = (count==1 ? sensorValue : max<int32_t>(value, sensorValue));
            else
              value += sensorValue;
          }
        }
      }
      if (sensor.formula == TELEM_FORMULA_AVERAGE) {
        if (count == 0) {
          if (available)
            lastReceived = TELEMETRY_VALUE_OLD;
          return;
        }
        else {
          value = (value + count/2) / count;
        }
      }
      else if (sensor.formula == TELEM_FORMULA_MULTIPLY) {
        if (count == 0)
          return;
        value = convertTelemetryValue(value, sensor.unit, mulprec, sensor.unit, sensor.prec);
      }
      setValue(sensor, value, sensor.unit, sensor.prec);
      break;
    }

    default:
      break;
  }
}

void delTelemetryIndex(uint8_t index)
{
  memclear(&g_model.telemetrySensors[index], sizeof(TelemetrySensor));
  telemetryItems[index].clear();
  eeDirty(EE_MODEL);
}

int availableTelemetryIndex()
{
  for (int index=0; index<MAX_SENSORS; index++) {
    TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index];
    if (!telemetrySensor.isAvailable()) {
      return index;
    }
  }
  return -1;
}

int lastUsedTelemetryIndex()
{
  for (int index=MAX_SENSORS-1; index>=0; index--) {
    TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index];
    if (telemetrySensor.isAvailable()) {
      return index;
    }
  }
  return -1;
}

void setTelemetryValue(TelemetryProtocol protocol, uint16_t id, uint8_t instance, int32_t value, uint32_t unit, uint32_t prec)
{
  bool available = false;

  for (int index=0; index<MAX_SENSORS; index++) {
    TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index];
    if (telemetrySensor.type == TELEM_TYPE_CUSTOM && telemetrySensor.id == id && (telemetrySensor.instance == instance || g_model.ignoreSensorIds)) {
      telemetryItems[index].setValue(telemetrySensor, value, unit, prec);
      available = true;
      // we continue search here, because more than one sensor can have the same id and instance
    }
  }

  if (available) {
    return;
  }
  
  int index = availableTelemetryIndex();
  if (index >= 0) {
    switch (protocol) {
#if defined(FRSKY_SPORT)
      case TELEM_PROTO_FRSKY_SPORT:
        frskySportSetDefault(index, id, instance);
        break;
#endif
#if defined(FRSKY)
      case TELEM_PROTO_FRSKY_D:
        frskyDSetDefault(index, id);
        break;
#endif
      default:
        return;
    }
    telemetryItems[index].setValue(g_model.telemetrySensors[index], value, unit, prec);
  }
  else {
    POPUP_WARNING(STR_TELEMETRYFULL);
  }
}

void TelemetrySensor::init(const char * label, uint8_t unit, uint8_t prec)
{
  memclear(this->label, TELEM_LABEL_LEN);
  strncpy(this->label, label, TELEM_LABEL_LEN);
  this->unit = unit;
  if (prec > 1 && (IS_DISTANCE_UNIT(unit) || IS_SPEED_UNIT(unit))) {
    // 2 digits precision is not needed here
    prec = 1;
  }
  this->prec = prec;
}

void TelemetrySensor::init(uint16_t id)
{
  char label[4];
  label[0] = hex2zchar((id & 0xf000) >> 12);
  label[1] = hex2zchar((id & 0x0f00) >> 8);
  label[2] = hex2zchar((id & 0x00f0) >> 4);
  label[3] = hex2zchar((id & 0x000f) >> 0);
  init(label);
}

bool TelemetrySensor::isAvailable()
{
  return ZLEN(label) > 0;
}

int32_t convertTelemetryValue(int32_t value, uint8_t unit, uint8_t prec, uint8_t destUnit, uint8_t destPrec)
{
  for (int i=prec; i<destPrec; i++)
    value *= 10;

  if (unit == UNIT_METERS || unit == UNIT_METERS_PER_SECOND) {
    if (destUnit == UNIT_FEET || destUnit == UNIT_FEET_PER_SECOND) {
      // m to ft *105/32
      value = (value * 105) / 32;
    }
  }
  else if (unit == UNIT_KTS) {
    if (destUnit == UNIT_KMH) {
      // kts to km/h (1 knot = 1.85200 kilometers per hour)
      value = (value * 1852) / 1000;
    }
    else if (destUnit == UNIT_MPH) {
      // kts to mph (1 knot = 1.15077945 miles per hour)
      value = (value * 1151) / 1000;
    }
    else if (destUnit == UNIT_METERS_PER_SECOND) {
      // kts to m/s (1 knot = 0.514444444 meters / second)
      value = (value * 514) / 1000;
    }
    else if (destUnit == UNIT_FEET_PER_SECOND) {
      // kts to f/s  (1 knot = 1.68780986 feet per second)
      value = (value * 1688) / 1000;
    }
  }
  else if (unit == UNIT_CELSIUS) {
    if (destUnit == UNIT_FAHRENHEIT) {
      // T(°F) = T(°C)×1,8 + 32
      value = 32 + (value*18)/10;
    }
  }
  else if (unit == UNIT_MILLILITERS) {
    if (destUnit == UNIT_FLOZ) {
      value = (value * 100) / 2957;
    }
  }
  
  for (int i=destPrec; i<prec; i++)
    value /= 10;

  return value;
}

int32_t TelemetrySensor::getValue(int32_t value, uint8_t unit, uint8_t prec) const
{
  if (type == TELEM_TYPE_CUSTOM && custom.ratio) {
    if (this->prec == 2) {
      value *= 10;
      prec = 2;
    }
    else {
      prec = 1;
    }
    value = (custom.ratio * value + 122) / 255;
  }

  value = convertTelemetryValue(value, unit, prec, this->unit, this->prec);

  if (type == TELEM_TYPE_CUSTOM) {
    value += custom.offset;
    if (value < 0 && onlyPositive) {
      value = 0;
    }
  }

  return value;
}

bool TelemetrySensor::isConfigurable()
{
  if (type == TELEM_TYPE_CALCULATED) {
    if (formula >= TELEM_FORMULA_CELL) {
      return false;
    }
  }
  else {
    if (unit >= UNIT_FIRST_VIRTUAL)  {
      return false;
    }
  }
  return true;
}

bool TelemetrySensor::isPrecConfigurable()
{
  if (isConfigurable()) {
    return true;
  }
  else if (unit == UNIT_CELLS) {
    return true;
  }
  else {
    return false;
  }
}
