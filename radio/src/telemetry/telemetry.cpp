#include "../opentx.h"

TelemetryItem telemetryItems[TELEM_VALUES_MAX];

void TelemetryItem::setValue(int32_t newVal)
{
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

int getTelemetryIndex(TelemetryProtocol protocol, uint16_t id, uint8_t instance)
{
  int available = -1;

  for (int index=0; index<TELEM_VALUES_MAX; index++) {
    TelemetryValue & telemetryValue = g_model.telemetryValues[index];
    if (telemetryValue.id == id && telemetryValue.instance == instance) {
      return index;
    }
    else if (available < 0 && telemetryValue.id == 0) {
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
  memclear(&g_model.telemetryValues[index], sizeof(TelemetryValue));
  telemetryItems[index].clear();
  eeDirty(EE_MODEL);
}

int availableTelemetryIndex()
{
  for (int index=0; index<TELEM_VALUES_MAX; index++) {
    TelemetryValue & telemetryValue = g_model.telemetryValues[index];
    if (telemetryValue.id == 0) {
      return index;
    }
  }
  return -1;
}

void setTelemetryLabel(TelemetryValue & telemetryValue, const char *label)
{
  memclear(telemetryValue.label, sizeof(telemetryValue.label));
  strncpy(telemetryValue.label, label, TELEM_LABEL_LEN);
}

void setTelemetryValue(TelemetryProtocol protocol, uint16_t id, uint8_t instance, int32_t value)
{
  int index = getTelemetryIndex(protocol, id, instance);

  if (index >= 0) {
    telemetryItems[index].setValue(value);
  }
  else {
    // TODO error too many sensors
  }
}

