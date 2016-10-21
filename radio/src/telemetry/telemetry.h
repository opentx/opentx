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

#ifndef _TELEMETRY_H_
#define _TELEMETRY_H_

#if defined(TELEMETRY_FRSKY)
  // FrSky Telemetry
  #include "frsky.h"
#elif defined(TELEMETRY_JETI)
  // Jeti-DUPLEX Telemetry
  #include "jeti.h"
#elif defined(TELEMETRY_ARDUPILOT)
  // ArduPilot Telemetry
  #include "ardupilot.h"
#elif defined(TELEMETRY_NMEA)
  // NMEA Telemetry
  #include "nmea.h"
#elif defined(TELEMETRY_MAVLINK)
  // Mavlink Telemetry
  #include "mavlink.h"
#endif

#if defined(CROSSFIRE)
  #include "crossfire.h"
#endif
#if defined(MULTIMODULE)
  #include "spektrum.h"
#endif

extern uint8_t telemetryStreaming; // >0 (true) == data is streaming in. 0 = no data detected for some time

#if defined(WS_HOW_HIGH)
extern uint8_t wshhStreaming;
#endif

extern uint8_t link_counter;

#if defined(CPUARM)
enum TelemetryStates {
  TELEMETRY_INIT,
  TELEMETRY_OK,
  TELEMETRY_KO
};
extern uint8_t telemetryState;
#endif

#define TELEMETRY_TIMEOUT10ms          100 // 1 second

enum TelemetrySerialMode {
  TELEMETRY_SERIAL_8N1,
  TELEMETRY_SERIAL_8E2
};

#if defined(CROSSFIRE)
#define TELEMETRY_RX_PACKET_SIZE       64
#else
#define TELEMETRY_RX_PACKET_SIZE       19  // 9 bytes (full packet), worst case 18 bytes with byte-stuffing (+1), multimodule Spektrum telemetry is 18 bytes
#endif

extern uint8_t telemetryRxBuffer[TELEMETRY_RX_PACKET_SIZE];
extern uint8_t telemetryRxBufferCount;

#if defined(CPUARM)
#define TELEMETRY_AVERAGE_COUNT        3

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

void setTelemetryValue(TelemetryProtocol protocol, uint16_t id, uint8_t subId, uint8_t instance, int32_t value, uint32_t unit, uint32_t prec);
void delTelemetryIndex(uint8_t index);
int availableTelemetryIndex();
int lastUsedTelemetryIndex();

int32_t getTelemetryValue(uint8_t index, uint8_t & prec);
int32_t convertTelemetryValue(int32_t value, uint8_t unit, uint8_t prec, uint8_t destUnit, uint8_t destPrec);

void frskySportSetDefault(int index, uint16_t id, uint8_t subId, uint8_t instance);
void frskyDSetDefault(int index, uint16_t id);
#endif

#define IS_DISTANCE_UNIT(unit)         ((unit) == UNIT_METERS || (unit) == UNIT_FEET)
#define IS_SPEED_UNIT(unit)            ((unit) >= UNIT_KTS && (unit) <= UNIT_MPH)

#if defined(CPUARM)
extern uint8_t telemetryProtocol;
#define IS_FRSKY_D_PROTOCOL()          (telemetryProtocol == PROTOCOL_FRSKY_D)
#define IS_FRSKY_SPORT_PROTOCOL()      (telemetryProtocol == PROTOCOL_FRSKY_SPORT)
#define IS_SPEKTRUM_PROTOCOL()         (telemetryProtocol == PROTOCOL_SPEKTRUM)
#else
#define IS_FRSKY_D_PROTOCOL()          (true)
#define IS_FRSKY_SPORT_PROTOCOL()      (false)
#endif

#if defined(PCBTARANIS) || defined(PCBHORUS)
inline uint8_t modelTelemetryProtocol()
{
#if defined(CROSSFIRE)
  if (g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_CROSSFIRE) {
    return PROTOCOL_PULSES_CROSSFIRE;
  }
#endif
     
  if (g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM) {
    return g_model.telemetryProtocol;
  }
  
#if defined(MULTIMODULE)
  if (g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_MULTIMODULE) {
    if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false) == MM_RF_PROTO_DSM2)
      return PROTOCOL_SPEKTRUM;
    else if ((g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false) == MM_RF_PROTO_FRSKY) &&
     (g_model.moduleData[EXTERNAL_MODULE].subType == MM_RF_FRSKY_SUBTYPE_D16 || g_model.moduleData[EXTERNAL_MODULE].subType == MM_RF_FRSKY_SUBTYPE_D16_8CH))
      return PROTOCOL_FRSKY_SPORT;
    else
      return PROTOCOL_FRSKY_D;
  }
#endif

  // default choice
  return PROTOCOL_FRSKY_SPORT;
}
#define MODEL_TELEMETRY_PROTOCOL()     modelTelemetryProtocol()
#elif defined(CPUARM)
#define MODEL_TELEMETRY_PROTOCOL()     g_model.telemetryProtocol
#endif

#if defined(CPUARM)
  #include "telemetry_sensors.h"
#endif

#if defined(LOG_TELEMETRY) && !defined(SIMU)
void logTelemetryWriteStart();
void logTelemetryWriteByte(uint8_t data);
#define LOG_TELEMETRY_WRITE_START()    logTelemetryWriteStart()
#define LOG_TELEMETRY_WRITE_BYTE(data) logTelemetryWriteByte(data)
#else
#define LOG_TELEMETRY_WRITE_START()
#define LOG_TELEMETRY_WRITE_BYTE(data)
#endif

#define TELEMETRY_OUTPUT_FIFO_SIZE 16
extern uint8_t outputTelemetryBuffer[TELEMETRY_OUTPUT_FIFO_SIZE] __DMA;
extern uint8_t outputTelemetryBufferSize;
extern uint8_t outputTelemetryBufferTrigger;

inline void telemetryOutputPushByte(uint8_t byte)
{
  outputTelemetryBuffer[outputTelemetryBufferSize++] = byte;
}

inline void telemetryOutputSetTrigger(uint8_t byte)
{
  outputTelemetryBufferTrigger = byte;
}

#if defined(LUA)
#define LUA_TELEMETRY_INPUT_FIFO_SIZE  256
extern Fifo<uint8_t, LUA_TELEMETRY_INPUT_FIFO_SIZE> * luaInputTelemetryFifo;
#endif

#endif // _TELEMETRY_H_
