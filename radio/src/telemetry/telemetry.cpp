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

#include <opentx.h>

uint8_t telemetryStreaming = 0;
uint8_t R9ModuleStreaming = 0;
uint8_t telemetryRxBuffer[TELEMETRY_RX_PACKET_SIZE];   // Receive buffer. 9 bytes (full packet), worst case 18 bytes with byte-stuffing (+1)
uint8_t telemetryRxBufferCount = 0;

#if defined(WS_HOW_HIGH)
uint8_t wshhStreaming = 0;
#endif

uint8_t telemetryState = TELEMETRY_INIT;

TelemetryData telemetryData;

uint8_t telemetryProtocol = 255;

#if defined(PCBSKY9X) && defined(REVX)
uint8_t serialInversion = 0;
#endif

void processTelemetryData(uint8_t data)
{
#if defined(CROSSFIRE)
  if (telemetryProtocol == PROTOCOL_TELEMETRY_CROSSFIRE) {
    processCrossfireTelemetryData(data);
    return;
  }
#endif

#if defined(MULTIMODULE)
  if (telemetryProtocol == PROTOCOL_TELEMETRY_SPEKTRUM) {
    processSpektrumTelemetryData(data);
    return;
  }
  if (telemetryProtocol == PROTOCOL_TELEMETRY_FLYSKY_IBUS) {
    processFlySkyTelemetryData(data);
    return;
  }
  if (telemetryProtocol == PROTOCOL_TELEMETRY_MULTIMODULE) {
    processMultiTelemetryData(data);
    return;
  }
#endif

  if (telemetryProtocol == PROTOCOL_TELEMETRY_PXX2) {
    processFrskyPXX2Data(data);
    return;
  }

  processFrskyTelemetryData(data);
}

void processRegisterFrame(uint8_t module, uint8_t * frame)
{
  if (moduleSettings[module].mode != MODULE_MODE_REGISTER) {
    return;
  }

  if (frame[3] == 0x00) {
    // RX_NAME follows, we store it for the next step
    memcpy(reusableBuffer.modelSetup.pxx2.registerRxName, &frame[4], PXX2_LEN_RX_NAME);
    reusableBuffer.modelSetup.pxx2.registerStep = REGISTER_COUNTER_ID_RECEIVED;
  }
  else if (frame[3] == 0x01) {
    // RX_NAME + PASSWORD follow, we check they are good
    if (memcmp(&frame[4], reusableBuffer.modelSetup.pxx2.registerRxName, PXX2_LEN_RX_NAME) == 0 &&
        memcmp(&frame[12], g_model.modelRegistrationID, PXX2_LEN_REGISTRATION_ID) == 0) {
      reusableBuffer.modelSetup.pxx2.registerStep = REGISTER_OK;
      moduleSettings[module].mode = MODULE_MODE_NORMAL;
      POPUP_INFORMATION(STR_REG_OK);
    }
  }
}

void processBindFrame(uint8_t module, uint8_t * frame)
{
  if (moduleSettings[module].mode != MODULE_MODE_BIND) {
    return;
  }

  if (frame[3] == 0x00) {
    bool found = false;
    for (uint8_t i=0; i<reusableBuffer.modelSetup.pxx2.bindCandidateReceiversCount; i++) {
      if (memcmp(reusableBuffer.modelSetup.pxx2.bindCandidateReceiversIds[i], &frame[4], PXX2_LEN_RX_NAME) == 0) {
        found = true;
        break;
      }
    }
    if (!found && reusableBuffer.modelSetup.pxx2.bindCandidateReceiversCount < PXX2_MAX_RECEIVERS_PER_MODULE) {
      memcpy(reusableBuffer.modelSetup.pxx2.bindCandidateReceiversIds[reusableBuffer.modelSetup.pxx2.bindCandidateReceiversCount], &frame[4], PXX2_LEN_RX_NAME);
      char * c = reusableBuffer.modelSetup.pxx2.bindCandidateReceiversNames[reusableBuffer.modelSetup.pxx2.bindCandidateReceiversCount];
      for (uint8_t i=0; i<PXX2_LEN_RX_NAME; i++) {
        uint8_t byte = frame[4 + i];
        uint8_t quartet = (byte >> 4);
        *c++ = (quartet >= 10 ? quartet + 'A' - 10 : quartet + '0');
        quartet = (byte & 0x0f);
        *c++ = (quartet >= 10 ? quartet + 'A' - 10 : quartet + '0');
        *c++ = ' ';
      }
      *c = '\0';
      ++reusableBuffer.modelSetup.pxx2.bindCandidateReceiversCount;
      reusableBuffer.modelSetup.pxx2.bindStep = BIND_RX_NAME_RECEIVED;
    }
  }
  else if (frame[3] == 0x01) {
    if (memcmp(reusableBuffer.modelSetup.pxx2.bindCandidateReceiversIds[reusableBuffer.modelSetup.pxx2.bindSelectedReceiverIndex], &frame[4], PXX2_LEN_RX_NAME) == 0) {
      reusableBuffer.modelSetup.pxx2.bindStep = BIND_WAIT;
      reusableBuffer.modelSetup.pxx2.bindWaitTimeout = get_tmr10ms() + 30;
    }
  }
}

void processSpectrumFrame(uint8_t module, uint8_t * frame)
{
  if (moduleSettings[module].mode != MODULE_MODE_SPECTRUM_ANALYSER) {
    return;
  }

  uint32_t * frequency = (uint32_t *)&frame[4];
  int8_t * power = (int8_t *)&frame[8];

  // center = 2440000000;  // 2440MHz
  // span = 40000000;  // 40MHz
  // left = 2440000000 - 20000000
  // step = 10000

  int32_t D = *frequency - (2440000000 - 40000000 / 2);

  // TRACE("Fq=%u, Pw=%d, X=%d, Y=%d", *frequency, int32_t(*power), D * 128 / 40000000, int32_t(127 + *power));
  uint8_t x = D * 128 / 40000000;

  reusableBuffer.spectrum.bars[x] = 127 + *power;
}

void processRadioFrame(uint8_t module, uint8_t * frame)
{
  switch (frame[2]) {
    case PXX2_TYPE_ID_REGISTER:
      processRegisterFrame(module, frame);
      break;

    case PXX2_TYPE_ID_BIND:
      processBindFrame(module, frame);
      break;
  }
}

void processPowerMeterFrame(uint8_t module, uint8_t * frame)
{
  switch (frame[2]) {
    case PXX2_TYPE_ID_POWER_METER:
      // TODO
      break;

    case PXX2_TYPE_ID_SPECTRUM:
      processSpectrumFrame(module, frame);
      break;
  }
}

void processModuleFrame(uint8_t module, uint8_t * frame)
{
  switch (frame[1]) {
    case PXX2_TYPE_C_MODULE:
      processRadioFrame(module, frame);
      break;

    case PXX2_TYPE_C_POWER_METER:
      processPowerMeterFrame(module, frame);
      break;

    default:
      break;
  }
}

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

#if defined(INTMODULE_USART)
  uint8_t frame[32];
  if (intmoduleFifo.getFrame(frame)) {
    processModuleFrame(INTERNAL_MODULE, frame);
  }
#endif

#if defined(STM32)
  uint8_t data;
  if (telemetryGetByte(&data)) {
    LOG_TELEMETRY_WRITE_START();
    do {
      processTelemetryData(data);
      LOG_TELEMETRY_WRITE_BYTE(data);
    } while (telemetryGetByte(&data));
  }
#elif defined(PCBSKY9X)
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

#define FRSKY_BAD_ANTENNA()            (IS_RAS_VALUE_VALID() && telemetryData.swr.value > 0x33)

  static tmr10ms_t alarmsCheckTime = 0;
  #define SCHEDULE_NEXT_ALARMS_CHECK(seconds) alarmsCheckTime = get_tmr10ms() + (100*(seconds))
  if (int32_t(get_tmr10ms() - alarmsCheckTime) > 0) {

    SCHEDULE_NEXT_ALARMS_CHECK(1/*second*/);

    bool sensor_lost = false;
    for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
      if (isTelemetryFieldAvailable(i)) {
        TelemetryItem & item = telemetryItems[i];
        if (item.hasReceiveTime() && item.getDelaySinceLastValue() > TELEMETRY_VALUE_OLD_THRESHOLD) {
          TelemetrySensor * sensor = & g_model.telemetrySensors[i];
          if (sensor->unit != UNIT_DATETIME) {
            item.setOld();
            sensor_lost = true;
          }
        }
      }
    }
    if (sensor_lost && TELEMETRY_STREAMING() &&  !g_model.rssiAlarms.disabled) {
      audioEvent(AU_SENSOR_LOST);
    }

#if defined(PCBTARANIS) || defined(PCBHORUS)
    if ((isModulePXX(INTERNAL_MODULE) || isModulePXX(EXTERNAL_MODULE)) && FRSKY_BAD_ANTENNA()) {
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
        }
        telemetryState = TELEMETRY_OK;
      }
      else if (telemetryState == TELEMETRY_OK) {
        telemetryState = TELEMETRY_KO;
        AUDIO_TELEMETRY_LOST();
      }
    }
  }
}

void telemetryInterrupt10ms()
{

  if (TELEMETRY_STREAMING()) {
    if (!TELEMETRY_OPENXSENSOR()) {
      for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
        const TelemetrySensor & sensor = g_model.telemetrySensors[i];
        if (sensor.type == TELEM_TYPE_CALCULATED) {
          telemetryItems[i].per10ms(sensor);
        }
      }
    }

  }

#if defined(WS_HOW_HIGH)
  if (wshhStreaming > 0) {
    wshhStreaming--;
  }
#endif
  if (R9ModuleStreaming > 0) {
    R9ModuleStreaming--;
  }
  if (telemetryStreaming > 0) {
    telemetryStreaming--;
  }
  else {
#if !defined(SIMU)
    telemetryData.rssi.reset();
#endif
  }
}

void telemetryReset()
{
  memclear(&telemetryData, sizeof(telemetryData));

  for (int index=0; index<MAX_TELEMETRY_SENSORS; index++) {
    telemetryItems[index].clear();
  }

  telemetryStreaming = 0; // reset counter only if valid frsky packets are being detected

  telemetryState = TELEMETRY_INIT;
}

// we don't reset the telemetry here as we would also reset the consumption after model load
void telemetryInit(uint8_t protocol)
{
  telemetryProtocol = protocol;

  if (protocol == PROTOCOL_TELEMETRY_FRSKY_D) {
    telemetryPortInit(FRSKY_D_BAUDRATE, TELEMETRY_SERIAL_DEFAULT);
  }

#if defined(MULTIMODULE)
  else if (protocol == PROTOCOL_TELEMETRY_MULTIMODULE || protocol == PROTOCOL_TELEMETRY_FLYSKY_IBUS) {
    // The DIY Multi module always speaks 100000 baud regardless of the telemetry protocol in use
    telemetryPortInit(MULTIMODULE_BAUDRATE, TELEMETRY_SERIAL_8E2);
#if defined(LUA)
    outputTelemetryBufferSize = 0;
    outputTelemetryBufferTrigger = 0x7E;
#endif
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
    outputTelemetryBufferSize = 0;
    outputTelemetryBufferTrigger = 0;
#endif
    telemetryPortSetDirectionOutput();
  }
#endif

#if defined(SERIAL2) || defined(PCBSKY9X)
  else if (protocol == PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY) {
    telemetryPortInit(0, TELEMETRY_SERIAL_DEFAULT);
    serial2TelemetryInit(PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY);
  }
#endif

  else if (protocol == PROTOCOL_TELEMETRY_PXX2) {
    telemetryPortInit(PXX2_BAUDRATE, TELEMETRY_SERIAL_WITHOUT_DMA);
#if defined(LUA)
    outputTelemetryBufferSize = 0;
    outputTelemetryBufferTrigger = 0x7E;
#endif
  }
  else {
    telemetryPortInit(FRSKY_SPORT_BAUDRATE, TELEMETRY_SERIAL_WITHOUT_DMA);
#if defined(LUA)
    outputTelemetryBufferSize = 0;
    outputTelemetryBufferTrigger = 0x7E;
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

uint8_t outputTelemetryBuffer[TELEMETRY_OUTPUT_FIFO_SIZE] __DMA;
uint8_t outputTelemetryBufferSize = 0;
uint8_t outputTelemetryBufferTrigger = 0;

#if defined(LUA)
Fifo<uint8_t, LUA_TELEMETRY_INPUT_FIFO_SIZE> * luaInputTelemetryFifo = NULL;
#endif
