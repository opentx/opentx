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
#include "multi.h"
#include "pulses/afhds3.h"
#include "mixer_scheduler.h"

uint8_t telemetryStreaming = 0;
uint8_t telemetryRxBuffer[TELEMETRY_RX_PACKET_SIZE];   // Receive buffer. 9 bytes (full packet), worst case 18 bytes with byte-stuffing (+1)
uint8_t telemetryRxBufferCount = 0;

uint8_t telemetryState = TELEMETRY_INIT;

TelemetryData telemetryData;

uint8_t telemetryProtocol = 255;

#if defined(PCBSKY9X) && defined(REVX)
uint8_t serialInversion = 0;
#endif

#if defined(INTERNAL_MODULE_SERIAL_TELEMETRY)
uint8_t intTelemetryRxBuffer[TELEMETRY_RX_PACKET_SIZE];
uint8_t intTelemetryRxBufferCount;
#endif

uint8_t * getTelemetryRxBuffer(uint8_t moduleIdx)
{
#if defined(INTERNAL_MODULE_SERIAL_TELEMETRY)
  if (moduleIdx == INTERNAL_MODULE)
    return intTelemetryRxBuffer;
#endif
  return telemetryRxBuffer;
}

uint8_t &getTelemetryRxBufferCount(uint8_t moduleIdx)
{
#if defined(INTERNAL_MODULE_SERIAL_TELEMETRY)
  if (moduleIdx == INTERNAL_MODULE)
    return intTelemetryRxBufferCount;
#endif
  return telemetryRxBufferCount;
}

void processTelemetryData(uint8_t data)
{
#if defined(CROSSFIRE)
  if (telemetryProtocol == PROTOCOL_TELEMETRY_CROSSFIRE) {
    processCrossfireTelemetryData(data, EXTERNAL_MODULE);
    return;
  }
#endif

#if defined(GHOST)
  if (telemetryProtocol == PROTOCOL_TELEMETRY_GHOST) {
    processGhostTelemetryData(data);
    return;
  }
#endif

#if defined(MULTIMODULE)
  if (telemetryProtocol == PROTOCOL_TELEMETRY_SPEKTRUM) {
    processSpektrumTelemetryData(EXTERNAL_MODULE, data, telemetryRxBuffer, telemetryRxBufferCount);
    return;
  }
  if (telemetryProtocol == PROTOCOL_TELEMETRY_FLYSKY_IBUS) {
    processFlySkyTelemetryData(data, telemetryRxBuffer, telemetryRxBufferCount);
    return;
  }
  if (telemetryProtocol == PROTOCOL_TELEMETRY_MULTIMODULE) {
    processMultiTelemetryData(data, EXTERNAL_MODULE);
    return;
  }
#endif

#if defined(AFHDS3)
  if (telemetryProtocol == PROTOCOL_TELEMETRY_AFHDS3) {
    afhds3::processTelemetryData(EXTERNAL_MODULE, data, telemetryRxBuffer, telemetryRxBufferCount, TELEMETRY_RX_PACKET_SIZE);
    return;
  }
#endif

  processFrskyTelemetryData(data);
}

inline bool isBadAntennaDetected()
{
  if (!isRasValueValid())
    return false;

  if (telemetryData.swrInternal.isFresh() && telemetryData.swrInternal.value() > FRSKY_BAD_ANTENNA_THRESHOLD)
    return true;

  if (telemetryData.swrExternal.isFresh() && telemetryData.swrExternal.value() > FRSKY_BAD_ANTENNA_THRESHOLD)
    return true;

  return false;
}

#if defined(INTERNAL_MODULE_PXX2)
static void pollIntPXX2()
{
  uint8_t frame[PXX2_FRAME_MAXLENGTH];

  while (intmoduleFifo.getFrame(frame)) {
    processPXX2Frame(INTERNAL_MODULE, frame);
  }
}
#endif

#if defined(PXX2) && defined(EXTMODULE_USART)
static void pollExtPXX2()
{
  uint8_t frame[PXX2_FRAME_MAXLENGTH];

  while (extmoduleFifo.getFrame(frame)) {
    processPXX2Frame(EXTERNAL_MODULE, frame);
  }
}
#endif

#if !defined(PCBSKY9X)
static inline void pollIntTelemetry(void (*processData)(uint8_t,uint8_t))
{
  uint8_t data;
  if (intmoduleFifo.pop(data)) {
    LOG_TELEMETRY_WRITE_START();
    do {
      processData(data, INTERNAL_MODULE);
      LOG_TELEMETRY_WRITE_BYTE(data);
    } while (intmoduleFifo.pop(data));
  }
}
#endif

#if defined(INTERNAL_MODULE_MULTI)
static void pollIntMulti()
{
  pollIntTelemetry(processMultiTelemetryData);
}
#endif

#if defined(INTERNAL_MODULE_ELRS)
static void pollIntELRS()
{
  pollIntTelemetry(processCrossfireTelemetryData);
}
#endif

#if !defined(PCBSKY9X)
static void pollExtTelemetry()
{
  uint8_t data;
  if (telemetryGetByte(&data)) {
    LOG_TELEMETRY_WRITE_START();
    do {
      processTelemetryData(data);
      LOG_TELEMETRY_WRITE_BYTE(data);
    } while (telemetryGetByte(&data));
  }
#if defined(PXX2) && defined(EXTMODULE_USART)
  if (isModulePXX2(EXTERNAL_MODULE)) {
    pollExtPXX2();
  }
#endif
}
#endif

void telemetryWakeup()
{
  uint8_t requiredTelemetryProtocol = modelTelemetryProtocol();

#if defined(REVX)
  uint8_t requiredSerialInversion = g_model.moduleData[EXTERNAL_MODULE].invertedSerial;
  if (telemetryProtocol != requiredTelemetryProtocol || serialInversion != requiredSerialInversion) {
    serialInversion = requiredSerialInversion;
    telemetryInit(requiredTelemetryProtocol);
  }
#else
  if (telemetryProtocol != requiredTelemetryProtocol) {
    telemetryInit(requiredTelemetryProtocol);
  }
#endif

  // Poll internal modules
#if defined(INTERNAL_MODULE_PXX2)
  if (isModuleISRM(INTERNAL_MODULE)) {
    pollIntPXX2();
  }
#endif

#if defined(INTERNAL_MODULE_MULTI)
  if (isModuleMultimodule(INTERNAL_MODULE)) {
    pollIntMulti();
  }
#endif

#if defined(INTERNAL_MODULE_ELRS)
  if (isModuleCrossfire(INTERNAL_MODULE)) {
    pollIntELRS();
  }
#endif
  
#if defined(INTERNAL_MODULE_CRSF) && !defined(SIMU)
  if (IS_INTERNAL_MODULE_ENABLED()) {
    uint8_t data;
    if (intCrsfTelemetryFifo.pop(data)) {
      LOG_TELEMETRY_WRITE_START();
      do {
        processCrossfireTelemetryData(data, INTERNAL_MODULE);
        LOG_TELEMETRY_WRITE_BYTE(data);
      } while (intCrsfTelemetryFifo.pop(data));
    }
  }
#endif

#if defined(PCBSKY9X)
  if (telemetryProtocol == PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY) {
    uint8_t data;
    while (telemetrySecondPortReceive(data)) {
      processTelemetryData(data);
    }
  }
  else {
    // Receive serial data here
    rxPdcUsart(processTelemetryData);
  }
#else
  // Poll external / S.PORT telemetry
  // TODO: how to switch this OFF ???
  pollExtTelemetry();
#endif
  for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
    const TelemetrySensor & sensor = g_model.telemetrySensors[i];
    if (sensor.type == TELEM_TYPE_CALCULATED) {
      telemetryItems[i].eval(sensor);
    }
  }

#if defined(VARIO)
  if (TELEMETRY_STREAMING() && !IS_FAI_ENABLED()) {
    varioWakeup();
  }
#endif

  static tmr10ms_t alarmsCheckTime = 0;
  #define SCHEDULE_NEXT_ALARMS_CHECK(seconds) alarmsCheckTime = get_tmr10ms() + (100*(seconds))
  if (int32_t(get_tmr10ms() - alarmsCheckTime) > 0) {

    SCHEDULE_NEXT_ALARMS_CHECK(1/*second*/);

    bool sensorLost = false;
    for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
      if (isTelemetryFieldAvailable(i)) {
        TelemetryItem & item = telemetryItems[i];
        if (item.timeout == 0) {
          TelemetrySensor * sensor = & g_model.telemetrySensors[i];
          if (sensor->unit != UNIT_DATETIME) {
            item.setOld();
            sensorLost = true;
          }
        }
      }
    }

    if (sensorLost && TELEMETRY_STREAMING() && !g_model.rssiAlarms.disabled) {
      audioEvent(AU_SENSOR_LOST);
    }

#if defined(PCBTARANIS) || defined(PCBHORUS)
    if (isBadAntennaDetected()) {
      AUDIO_RAS_RED();
      POPUP_WARNING(STR_WARNING);
      const char * w = STR_ANTENNAPROBLEM;
      SET_WARNING_INFO(w, strlen(w), 0);
      SCHEDULE_NEXT_ALARMS_CHECK(10/*seconds*/);
    }
#endif

    if (!g_model.rssiAlarms.disabled) {
      if (TELEMETRY_STREAMING()) {
        if (TELEMETRY_RSSI() < g_model.rssiAlarms.getCriticalRssi() ) {
          AUDIO_RSSI_RED();
          SCHEDULE_NEXT_ALARMS_CHECK(10/*seconds*/);
        }
        else if (TELEMETRY_RSSI() < g_model.rssiAlarms.getWarningRssi() ) {
          AUDIO_RSSI_ORANGE();
          SCHEDULE_NEXT_ALARMS_CHECK(10/*seconds*/);
        }
      }

      if (TELEMETRY_STREAMING()) {
        if (telemetryState == TELEMETRY_KO) {
          AUDIO_TELEMETRY_BACK();
#if defined(CROSSFIRE)
          if (isModuleCrossfire(EXTERNAL_MODULE)) {
            moduleState[EXTERNAL_MODULE].counter = CRSF_FRAME_MODELID;
          }
#endif
        }
        telemetryState = TELEMETRY_OK;
      }
      else if (telemetryState == TELEMETRY_OK) {
        telemetryState = TELEMETRY_KO;
        if (!isModuleInBeepMode()) {
          AUDIO_TELEMETRY_LOST();
        }
      }
    }
  }
}

void telemetryInterrupt10ms()
{
  if (telemetryStreaming > 0) {
    bool tick160ms = (telemetryStreaming & 0x0F) == 0;
    for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
      const TelemetrySensor & sensor = g_model.telemetrySensors[i];
      if (sensor.type == TELEM_TYPE_CALCULATED) {
        telemetryItems[i].per10ms(sensor);
      }
      if (tick160ms && telemetryItems[i].timeout > 0) {
        telemetryItems[i].timeout--;
      }
    }
    telemetryStreaming--;
  }
  else {
#if !defined(SIMU)
    telemetryData.rssi.reset();
#endif
    for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
      const TelemetrySensor & sensor = g_model.telemetrySensors[i];
      TelemetryItem & telemetryItem = telemetryItems[i];
      if (telemetryItem.isAvailable() && !sensor.isOfflineFresh()) {
        telemetryItem.setOld();
      }
    }
  }
}

void telemetryReset()
{
  telemetryData.clear();

  for (auto & telemetryItem : telemetryItems) {
    telemetryItem.clear();
  }

  telemetryStreaming = 0; // reset counter only if valid telemetry packets are being detected

  telemetryState = TELEMETRY_INIT;
}

// we don't reset the telemetry here as we would also reset the consumption after model load
void telemetryInit(uint8_t protocol)
{
  telemetryProtocol = protocol;

#if defined(TRAINER_SPORT_SBUS)
  if (g_model.trainerData.mode == TRAINER_MODE_MASTER_SBUS_SPORT)
    return;
#endif

  if (protocol == PROTOCOL_TELEMETRY_FRSKY_D) {
    telemetryPortInit(FRSKY_D_BAUDRATE, TELEMETRY_SERIAL_DEFAULT);
  }

#if defined(MULTIMODULE)
  else if (protocol == PROTOCOL_TELEMETRY_MULTIMODULE || protocol == PROTOCOL_TELEMETRY_FLYSKY_IBUS) {
    // The DIY Multi module always speaks 100000 baud regardless of the telemetry protocol in use
    telemetryPortInit(MULTIMODULE_BAUDRATE, TELEMETRY_SERIAL_8E2);
#if defined(LUA)
    outputTelemetryBuffer.reset();
#endif
    telemetryPortSetDirectionInput();
  }
  else if (protocol == PROTOCOL_TELEMETRY_SPEKTRUM) {
    // Spektrum's own small race RX (SPM4648) uses 125000 8N1, use the same since there is no real standard
    telemetryPortInit(125000, TELEMETRY_SERIAL_DEFAULT);
  }
#endif

#if defined(CROSSFIRE)
  else if (protocol == PROTOCOL_TELEMETRY_CROSSFIRE) {
    telemetryPortInit(CROSSFIRE_BAUDRATE, TELEMETRY_SERIAL_DEFAULT);
#if defined(LUA)
    outputTelemetryBuffer.reset();
#endif
    telemetryPortSetDirectionOutput();
  }
#endif

#if defined(GHOST)
  else if (protocol == PROTOCOL_TELEMETRY_GHOST) {
    telemetryPortInit(GHOST_BAUDRATE, TELEMETRY_SERIAL_DEFAULT);
#if defined(LUA)
    outputTelemetryBuffer.reset();
#endif
    telemetryPortSetDirectionOutput();
  }
#endif

#if defined(AUX_SERIAL) || defined(PCBSKY9X)
  else if (protocol == PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY) {
    telemetryPortInit(0, TELEMETRY_SERIAL_DEFAULT);
    auxSerialTelemetryInit(PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY);
  }
#endif

#if defined(AFHDS3)
  else if (protocol == PROTOCOL_TELEMETRY_AFHDS3) {
    telemetryPortInvertedInit(AFHDS3_BAUDRATE);
    telemetryPortSetDirectionInput();
  }
#endif

  else {
    telemetryPortInit(FRSKY_SPORT_BAUDRATE, TELEMETRY_SERIAL_WITHOUT_DMA);
#if defined(LUA)
    outputTelemetryBuffer.reset();
#endif
  }

#if defined(REVX) && !defined(SIMU)
  if (serialInversion) {
    setMFP();
  }
  else {
    clearMFP();
  }
#endif
}


#if defined(LOG_TELEMETRY) && !defined(SIMU)
extern FIL g_telemetryFile;
void logTelemetryWriteStart()
{
  static tmr10ms_t lastTime = 0;
  tmr10ms_t newTime = get_tmr10ms();
  if (lastTime != newTime) {
    struct gtm utm;
    gettime(&utm);
    f_printf(&g_telemetryFile, "\r\n%4d-%02d-%02d,%02d:%02d:%02d.%02d0:", utm.tm_year+TM_YEAR_BASE, utm.tm_mon+1, utm.tm_mday, utm.tm_hour, utm.tm_min, utm.tm_sec, g_ms100);
    lastTime = newTime;
  }
}

void logTelemetryWriteByte(uint8_t data)
{
  f_printf(&g_telemetryFile, " %02X", data);
}
#endif

#if defined(RADIO_FAMILY_TBS)
uint8_t outputTelemetryBufferTrigger = 0;
#endif

OutputTelemetryBuffer outputTelemetryBuffer __DMA;

#if defined(LUA)
Fifo<uint8_t, LUA_TELEMETRY_INPUT_FIFO_SIZE> * luaInputTelemetryFifo = NULL;
#endif

#if defined(HARDWARE_INTERNAL_MODULE)
static ModuleSyncStatus moduleSyncStatus[NUM_MODULES];

ModuleSyncStatus &getModuleSyncStatus(uint8_t moduleIdx)
{
  return moduleSyncStatus[moduleIdx];
}
#else
static ModuleSyncStatus moduleSyncStatus;

ModuleSyncStatus &getModuleSyncStatus(uint8_t moduleIdx)
{
  return moduleSyncStatus;
}
#endif

ModuleSyncStatus::ModuleSyncStatus()
{
  memset(this, 0, sizeof(ModuleSyncStatus));
}

void ModuleSyncStatus::update(uint16_t newRefreshRate, int16_t newInputLag)
{
  if (!newRefreshRate)
    return;

  if (newRefreshRate < MIN_REFRESH_RATE)
    newRefreshRate = newRefreshRate * (MIN_REFRESH_RATE / (newRefreshRate + 1));
  else if (newRefreshRate > MAX_REFRESH_RATE)
    newRefreshRate = MAX_REFRESH_RATE;

  refreshRate = newRefreshRate;
  inputLag    = newInputLag;
  currentLag  = newInputLag;
  lastUpdate  = get_tmr10ms();

  //TRACE("[SYNC] update rate = %dus; lag = %dus",refreshRate,currentLag);
}

uint16_t ModuleSyncStatus::getAdjustedRefreshRate()
{
  int16_t lag = currentLag;
  int32_t newRefreshRate = refreshRate;

  if (lag == 0) {
    return refreshRate;
  }

  newRefreshRate += lag;

  if (newRefreshRate < MIN_REFRESH_RATE) {
      newRefreshRate = MIN_REFRESH_RATE;
  }
  else if (newRefreshRate > MAX_REFRESH_RATE) {
    newRefreshRate = MAX_REFRESH_RATE;
  }

  currentLag -= newRefreshRate - refreshRate;
  //TRACE("[SYNC] mod rate = %dus; lag = %dus",newRefreshRate,currentLag);

  return (uint16_t)newRefreshRate;
}

void ModuleSyncStatus::getRefreshString(char * statusText)
{
  if (!isValid()) {
    return;
  }

  char * tmp = statusText;
#if defined(DEBUG)
  *tmp++ = 'L';
  tmp = strAppendSigned(tmp, inputLag, 5);
  tmp = strAppend(tmp, "R");
  tmp = strAppendUnsigned(tmp, refreshRate, 5);
#else
  tmp = strAppend(tmp, "Sync ");
  tmp = strAppendUnsigned(tmp, refreshRate);
#endif
  tmp = strAppend(tmp, "us");
}
