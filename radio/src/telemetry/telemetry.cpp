#include "../opentx.h"

TelemetryItem telemetryItems[TELEM_VALUES_MAX];

void TelemetryItem::setValue(int32_t newVal, uint8_t flags)
{
  if (flags == TELEM_INPUT_CELLS) {
    uint32_t data = uint32_t(newVal);
    uint8_t cellIndex = data & 0xF;
    cells.count = (data & 0xF0) >> 4;
    cells.values[cellIndex].set(((data & 0x000FFF00) >>  8) / 5);
    if (cellIndex+1 < cells.count) {
      cells.values[cellIndex+1].set(((data & 0xFFF00000) >> 20) / 5);
    }
    if (cellIndex+1 >= cells.count) {
      newVal = 0;
      for (int i=0; i<cells.count; i++) {
        if (cells.values[i].state) {
          newVal += cells.values[i].value;
        }
        else {
          return;
        }
      }
    }
  }
  else if (flags == TELEM_INPUT_FLAGS_AUTO_OFFSET) {
    if (!isAvailable()) {
      offsetAuto = -newVal;
    }
    newVal += offsetAuto;
  }
  else if (flags == TELEM_INPUT_FLAGS_FILTERING) {
    if (!isAvailable()) {
      for (int i=0; i<TELEMETRY_AVERAGE_COUNT; i++) {
        filterValues[i] = newVal;
      }
    }
    else {
      // calculate the average from values[] and value
      // also shift readings in values [] array
      unsigned int sum = filterValues[0];
      for (int i=0; i<TELEMETRY_AVERAGE_COUNT-1; i++) {
        int32_t tmp = filterValues[i+1];
        filterValues[i] = tmp;
        sum += tmp;
      }
      filterValues[TELEMETRY_AVERAGE_COUNT-1] = newVal;
      sum += newVal;
      newVal = sum/(TELEMETRY_AVERAGE_COUNT+1);
    }
  }

  value = newVal;
  lastReceived = now();
  if (!isAvailable() || newVal < min) {
    min = value;
  }
  if (!isAvailable() || newVal > max) {
    max = value;
  }
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

void TelemetryItem::eval()
{

}

int getTelemetryIndex(TelemetryProtocol protocol, uint16_t id, uint8_t instance)
{
  int available = -1;

  for (int index=0; index<TELEM_VALUES_MAX; index++) {
    TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index];
    if (telemetrySensor.id == id && telemetrySensor.instance == instance) {
      return index;
    }
    else if (available < 0 && telemetrySensor.id == 0) {
      available = index;
    }
  }

  if (available >= 0) {
    switch (protocol) {
      case TELEM_PROTO_FRSKY_SPORT:
        frskySportSetDefault(available, id, instance);
        break;

      default:
        break;
    }
  }

  return available;
}

void delTelemetryIndex(uint8_t index)
{
  memclear(&g_model.telemetrySensors[index], sizeof(TelemetrySensor));
  telemetryItems[index].clear();
  eeDirty(EE_MODEL);
}

int availableTelemetryIndex()
{
  for (int index=0; index<TELEM_VALUES_MAX; index++) {
    TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index];
    if (telemetrySensor.id == 0) {
      return index;
    }
  }
  return -1;
}

void setTelemetryValue(TelemetryProtocol protocol, uint16_t id, uint8_t instance, int32_t value, uint32_t flags)
{
  int index = getTelemetryIndex(protocol, id, instance);

  if (index >= 0) {
    telemetryItems[index].setValue(value, flags ? flags : g_model.telemetrySensors[index].inputFlags);
  }
  else {
    // TODO error too many sensors
  }
}

void TelemetrySensor::init(const char *label, uint8_t unit, uint8_t inputFlags)
{
  memclear(this->label, TELEM_LABEL_LEN);
  strncpy(this->label, label, TELEM_LABEL_LEN);
  this->unit = unit;
  this->inputFlags = inputFlags;
}
