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

MultiModuleStatus multiModuleStatus;
uint8_t multiBindStatus = MULTI_NORMAL_OPERATION;


enum MultiPacketTypes : uint8_t {
  MultiStatus = 1,
  FrSkySportTelemtry,
  FrSkyHubTelemetry,
  SpektrumTelemetry,
  DSMBindPacket,
  FlyskyIBusTelemetry,
};

enum MultiBufferState : uint8_t {
  NoProtocolDetected,
  MultiFirstByteReceived,
  ReceivingMultiProtocol,
  SpektrumTelemetryFallback,
  FrskyTelemetryFallbackFirstByte,
  FrskyTelemetryFallbackNextBytes,
  FlyskyTelemetryFallback
};

MultiBufferState guessProtocol()
{
  if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false) == MM_RF_PROTO_DSM2)
    return SpektrumTelemetryFallback;
  else if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false) == MM_RF_PROTO_FS_AFHDS2A)
    return FlyskyTelemetryFallback;
  else
    return FrskyTelemetryFallbackFirstByte;
}

static void processMultiStatusPacket(const uint8_t *data)
{
  // At least two status packets without bind flag
  bool wasBinding = (multiModuleStatus.isBinding());

  multiModuleStatus.flags = data[0];
  multiModuleStatus.major = data[1];
  multiModuleStatus.minor = data[2];
  multiModuleStatus.revision = data[3];
  multiModuleStatus.patch = data[4];
  multiModuleStatus.lastUpdate = get_tmr10ms();

  if (wasBinding && !multiModuleStatus.isBinding() && multiBindStatus == MULTI_BIND_INITIATED)
      multiBindStatus = MULTI_BIND_FINISHED;

  // set moduleFlag to bind status
  /*
  if (moduleFlag[EXTERNAL_MODULE] != MODULE_RANGECHECK)
    // Two times the same status in a row to avoid race conditions
    if (multiModuleStatus.isBinding() == wasBinding) {
      multiModuleStatus.isBinding() ? moduleFlag[EXTERNAL_MODULE] = MODULE_BIND : MODULE_NORMAL_MODE;
    }
    */

}

static void processMultiTelemetryPaket(const uint8_t *packet)
{
  uint8_t type = packet[0];
  uint8_t len = packet[1];
  const uint8_t *data = packet + 2;

  // Switch type
  switch (type) {
    case MultiStatus:
      if (len >= 5)
        processMultiStatusPacket(data);
      break;
    case DSMBindPacket:
      if (len >= 10)
        processDSMBindPacket(data);
      break;
    case SpektrumTelemetry:
      // processSpektrumPacket expects data[0] to be the telemetry indicator 0xAA but does not check it,
      // just send one byte of our header instead
      if (len >= 17)
        processSpektrumPacket(data - 1);
      else
        TRACE("[MP] Received spektrum telemetry len %d < 17", len);
      break;
    case FlyskyIBusTelemetry:
      if (len >= 28)
        processFlySkyPacket(data);
      else
        TRACE("[MP] Received IBUS telemetry len %d < 28", len);
      break;
    case FrSkyHubTelemetry:
      if (len >= 4)
        frskyDProcessPacket(data);
      else
        TRACE("[MP] Received Frsky HUB telemetry len %d < 4", len);
      break;
    case FrSkySportTelemtry:
      if (len >= 4)
        sportProcessTelemetryPacket(data);
      else
        TRACE("[MP] Received sm telemetry len %d < 4", len);
      break;
    default:
      TRACE("[MP] Unkown multi packet type 0x%02X, len %d", type, len);
      break;
  }
}

// sprintf does not work AVR ARM
// use a small helper function
static void appendInt(char* buf, uint32_t val)
{
  while(*buf)
    buf++;

  int len=1;
  int32_t tmp = val / 10;
  while (tmp) {
    len++;
    tmp /= 10;
  }

  buf[len]='\0';
  for (uint8_t i=1;i<=len; i++) {
    div_t qr = div(val, 10);
    char c = qr.rem + '0';
    buf[len - i] = c;
    val = qr.quot;
  }
}


void MultiModuleStatus::getStatusString(char *statusText)
{
  if (get_tmr10ms()  - lastUpdate > 200) {
#if defined(PCBTARANIS) || defined(PCBHORUS)
    if (g_model.moduleData[INTERNAL_MODULE].rfProtocol != RF_PROTO_OFF)
      strcpy(statusText, STR_DISABLE_INTERNAL);
    else
#endif
      strcpy(statusText, STR_MODULE_NO_TELEMETRY);
    return;
  }
  if (!protocolValid()) {
    strcpy(statusText, STR_PROTOCOL_INVALID);
    return;
  } else if (!serialMode()) {
    strcpy(statusText, STR_MODULE_NO_SERIAL_MODE);
    return;
  } else if (!inputDetected()) {
    strcpy(statusText, STR_MODULE_NO_INPUT);
    return;
  }

  strcpy(statusText, "V");
  appendInt(statusText, major);
  strcat(statusText, ".");
  appendInt(statusText, minor);
  strcat(statusText, ".");
  appendInt(statusText, revision);
  strcat(statusText, ".");
  appendInt(statusText, patch);
  strcat(statusText, " ");

  if (isBinding())
    strcat(statusText, STR_MODULE_BINDING);
}


static MultiBufferState multiTelemetryBufferState;

static void processMultiTelemetryByte(const uint8_t data)
{
  if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
    telemetryRxBuffer[telemetryRxBufferCount++] = data;
  } else {
    TRACE("[MP] array size %d error", telemetryRxBufferCount);
    multiTelemetryBufferState = NoProtocolDetected;
  }

  // Length field does not count the header
  if (telemetryRxBufferCount >= 2 && telemetryRxBuffer[1] == telemetryRxBufferCount - 2) {
    // debug print the content of the packet
#if 0
    debugPrintf("[MP] Packet type %02X len 0x%02X: ",
                telemetryRxBuffer[0], telemetryRxBuffer[1]);
    for (int i=0; i<(telemetryRxBufferCount+3)/4; i++) {
      debugPrintf("[%02X%02X %02X%02X] ", telemetryRxBuffer[i*4+2], telemetryRxBuffer[i*4 + 3],
                  telemetryRxBuffer[i*4 + 4], telemetryRxBuffer[i*4 + 5]);
    }
    debugPrintf("\r\n");
#endif
    // Packet is complete, process it
    processMultiTelemetryPaket(telemetryRxBuffer);
    multiTelemetryBufferState = NoProtocolDetected;
  }
}

void processMultiTelemetryData(const uint8_t data)
{
  switch (multiTelemetryBufferState) {
    case NoProtocolDetected:
      if (data == 'M') {
        multiTelemetryBufferState = MultiFirstByteReceived;
      } else if (data == 0x55 || data == 0x7e) {
        multiTelemetryBufferState = guessProtocol();
        if (multiTelemetryBufferState == FrskyTelemetryFallbackFirstByte)
          processFrskyTelemetryData(data);
      } else {
        TRACE("[MP] invalid start byte 0x%02X", data);
      }
      break;

    case FrskyTelemetryFallbackFirstByte:
      multiTelemetryBufferState = FrskyTelemetryFallbackNextBytes;

    case FrskyTelemetryFallbackNextBytes:
      processFrskyTelemetryData(data);
      if (data == 0x7e)
        // might start a new packet
        multiTelemetryBufferState = FrskyTelemetryFallbackFirstByte;
      else if (telemetryRxBufferCount == 0 && data != 0x7d)
        // Should be in a frame (no bytestuff), but the Frsky parser has discarded the byte
        multiTelemetryBufferState = NoProtocolDetected;
      break;

    case FlyskyTelemetryFallback:
      processFlySkyTelemetryData(data);
      if (telemetryRxBufferCount == 0)
        multiTelemetryBufferState = NoProtocolDetected;
      break;

    case SpektrumTelemetryFallback:
      processSpektrumTelemetryData(data);
      if (telemetryRxBufferCount == 0)
        multiTelemetryBufferState = NoProtocolDetected;
      break;

    case MultiFirstByteReceived:
      if (data == 'P') {
        telemetryRxBufferCount = 0;
        multiTelemetryBufferState = ReceivingMultiProtocol;
      } else {
        TRACE("[MP] invalid second byte 0x%02X", data);
        multiTelemetryBufferState = NoProtocolDetected;
      }
      break;

    case ReceivingMultiProtocol:
      processMultiTelemetryByte(data);
      break;
  }

}
