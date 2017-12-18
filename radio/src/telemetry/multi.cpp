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
#include "telemetry.h"
#include "multi.h"

MultiModuleStatus multiModuleStatus;
MultiModuleSyncStatus multiSyncStatus;
uint8_t multiBindStatus = MULTI_NORMAL_OPERATION;


enum MultiPacketTypes : uint8_t {
  MultiStatus = 1,
  FrSkySportTelemtry,
  FrSkyHubTelemetry,
  SpektrumTelemetry,
  DSMBindPacket,
  FlyskyIBusTelemetry,
  ConfigCommand,
  InputSync,
  FrskySportPolling
};

enum MultiBufferState : uint8_t {
  NoProtocolDetected,
  MultiFirstByteReceived,
  ReceivingMultiProtocol,
  ReceivingMultiStatus,
  SpektrumTelemetryFallback,
  FrskyTelemetryFallback,
  FrskyTelemetryFallbackFirstByte,
  FrskyTelemetryFallbackNextBytes,
  FlyskyTelemetryFallback,
  MultiStatusOrFrskyData
};

MultiBufferState guessProtocol()
{
  if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false) == MM_RF_PROTO_DSM2)
    return SpektrumTelemetryFallback;
  else if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false) == MM_RF_PROTO_FS_AFHDS2A)
    return FlyskyTelemetryFallback;
  else
    return FrskyTelemetryFallback;
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
}

static void processMultiSyncPacket(const uint8_t *data)
{
  multiSyncStatus.lastUpdate = get_tmr10ms();
  multiSyncStatus.interval = data[4];
  multiSyncStatus.target = data[5];
#if !defined(PPM_PIN_SERIAL)
  auto oldlag = multiSyncStatus.inputLag;
  (void) oldlag;
#endif

  multiSyncStatus.calcAdjustedRefreshRate(data[0] << 8 | data[1], data[2] << 8 | data[3]);

#if !defined(PPM_PIN_SERIAL)
  TRACE("MP ADJ: rest: %d, lag %04d, diff: %04d  target: %d, interval: %d, Refresh: %d, intAdjRefresh: %d, adjRefresh %d\r\n", modulePulsesData[EXTERNAL_MODULE].dsm2.rest,
        multiSyncStatus.inputLag, oldlag-multiSyncStatus.inputLag, multiSyncStatus.target, multiSyncStatus.interval, multiSyncStatus.refreshRate, multiSyncStatus.adjustedRefreshRate/50,
        multiSyncStatus.getAdjustedRefreshRate());
#endif
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
        TRACE("[MP] Received sport telemetry len %d < 4", len);
      break;
    case InputSync:
      if (len >= 6)
        processMultiSyncPacket(data);
      else
        TRACE("[MP] Received input sync len %d < 6", len);
      break;
    case ConfigCommand:
      // Just an ack to our command, ignore for now
      break;
    case FrskySportPolling:
      #if defined(LUA)
      if (len >= 1 && outputTelemetryBufferSize > 0 && data[0] == outputTelemetryBufferTrigger) {
        TRACE("MP Sending sport data out.");
        sportSendBuffer(outputTelemetryBuffer, outputTelemetryBufferSize);
      }
      #endif
      break;
    default:
      TRACE("[MP] Unkown multi packet type 0x%02X, len %d", type, len);
      break;
  }
}

// sprintf does not work AVR ARM
// use a small helper function
static void appendInt(char *buf, uint32_t val)
{
  while (*buf)
    buf++;

  strAppendUnsigned(buf, val);
}

#define MIN_REFRESH_RATE      7000

void MultiModuleSyncStatus::calcAdjustedRefreshRate(uint16_t newRefreshRate, uint16_t newInputLag)
{
  // Check how far off we are from our target, positive means we are too slow, negative we are too fast
  int lagDifference = newInputLag - inputLag;

  // The refresh rate that we target
  // Below is least common multiple of MIN_REFRESH_RATE and requested rate
  uint16_t targetRefreshRate = (uint16_t) (newRefreshRate * ((MIN_REFRESH_RATE / (newRefreshRate - 1)) + 1));

  // Overflow, reverse sample
  if (lagDifference < -targetRefreshRate/2)
    lagDifference= -lagDifference;


  // Reset adjusted refresh if rate has changed
  if (newRefreshRate != refreshRate) {
    refreshRate = newRefreshRate;
    adjustedRefreshRate = targetRefreshRate;
    if (adjustedRefreshRate >= 30000)
      adjustedRefreshRate /= 2;

    // Our refresh rate in ps
    adjustedRefreshRate*=1000;
    return;
  }

  // Caluclate how many samples went into the reported input Lag (*10)
  int numsamples = interval * 10000 / targetRefreshRate;

  // Convert lagDifference to ps
  lagDifference=lagDifference*1000;

  // Calculate the time we intentionally were late/early
  if (inputLag > target*10 +30)
   lagDifference += numsamples*500;
  else if (inputLag < target*10 - 30)
    lagDifference -= numsamples*500;

  // Caculate the time in ps each frame is to slow (positive), fast(negative)
  int perframeps = lagDifference*10/ numsamples;

  if (perframeps > 20000)
    perframeps = 20000;

  if (perframeps < -20000)
    perframeps = -20000;

  adjustedRefreshRate =(adjustedRefreshRate + perframeps);

  // Safeguards
  if (adjustedRefreshRate < 6*1000*1000)
    adjustedRefreshRate = 6*1000*1000;
  if (adjustedRefreshRate > 30*1000*1000)
    adjustedRefreshRate = 30*1000*1000;

  inputLag = newInputLag;
}

static uint8_t counter;

uint16_t MultiModuleSyncStatus::getAdjustedRefreshRate() {
  if (!isValid() || refreshRate == 0)
    return 18000;


  counter = (uint8_t) (counter + 1 % 10);
  uint16_t rate = (uint16_t) ((adjustedRefreshRate + counter * 50) / 500);
  // Check how far off we are from our target, positive means we are too slow, negative we are too fast
 if (inputLag > target*10 +30)
     return (uint16_t) (rate - 1);
  else if (inputLag < target*10 - 30)
    return (uint16_t) (rate + 1);
  else
    return rate;
}


static void prependSpaces(char * buf, int val)
{
  while (*buf)
    buf++;

  int k=10000;
  while(val/k==0 && k > 0)
  {
    *buf=' ';
    buf++;
    k/= 10;
  }
  *buf='\0';
}

void MultiModuleSyncStatus::getRefreshString(char *statusText)
{
  if (!isValid()) {
    return;
  }

  strcpy(statusText, "L ");
  prependSpaces(statusText, inputLag);
  appendInt(statusText, inputLag);
  strcat(statusText, "ns R ");
  prependSpaces(statusText, adjustedRefreshRate/1000);
  appendInt(statusText, (uint32_t) (adjustedRefreshRate / 1000));
  strcat(statusText, "ns");
}

void MultiModuleStatus::getStatusString(char *statusText)
{
  if (!isValid()) {
#if defined(PCBTARANIS) || defined(PCBHORUS)
    if (IS_INTERNAL_MODULE_ENABLED())
      strcpy(statusText, STR_DISABLE_INTERNAL);
    else
#endif
      strcpy(statusText, STR_MODULE_NO_TELEMETRY);
    return;
  }
  if (!protocolValid()) {
    strcpy(statusText, STR_PROTOCOL_INVALID);
    return;
  }
  else if (!serialMode()) {
    strcpy(statusText, STR_MODULE_NO_SERIAL_MODE);
    return;
  }
  else if (!inputDetected()) {
    strcpy(statusText, STR_MODULE_NO_INPUT);
    return;
  }
  else if(isWaitingforBind()) {
    strcpy(statusText, STR_MODULE_WAITFORBIND);
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
  }
  else {
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
  // debugPrintf("State: %d, byte received %02X, buflen: %d\r\n", multiTelemetryBufferState, data, telemetryRxBufferCount);
  switch (multiTelemetryBufferState) {
    case NoProtocolDetected:
      if (data == 'M') {
        multiTelemetryBufferState = MultiFirstByteReceived;
      }
      else if (data == 0xAA || data == 0x7e) {
        multiTelemetryBufferState = guessProtocol();

        // Process the first byte by the protocol
        processMultiTelemetryData(data);
      }
      else {
        TRACE("[MP] invalid start byte 0x%02X", data);
      }
      break;

    case FrskyTelemetryFallback:
      multiTelemetryBufferState = FrskyTelemetryFallbackFirstByte;
      processFrskyTelemetryData(data);
      break;

    case FrskyTelemetryFallbackFirstByte:
      if (data == 'M') {
        multiTelemetryBufferState = MultiStatusOrFrskyData;
      }
      else {
        processFrskyTelemetryData(data);
        if (data != 0x7e)
          multiTelemetryBufferState = FrskyTelemetryFallbackNextBytes;
      }

      break;

    case FrskyTelemetryFallbackNextBytes:
      processFrskyTelemetryData(data);
      if (data == 0x7e)
        // end of packet or start of new packet
        multiTelemetryBufferState = FrskyTelemetryFallbackFirstByte;
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
      telemetryRxBufferCount = 0;
      if (data == 'P') {
        multiTelemetryBufferState = ReceivingMultiProtocol;
      }
      else if (data >= 5 && data <= 10) {
        // Protocol indented for er9x/ersky9, accept only 5-10 as packet length to have
        // a bit of validation
        multiTelemetryBufferState = ReceivingMultiStatus;
        processMultiTelemetryData(data);
      }
      else {
        TRACE("[MP] invalid second byte 0x%02X", data);
        multiTelemetryBufferState = NoProtocolDetected;
      }
      break;

    case ReceivingMultiProtocol:
      processMultiTelemetryByte(data);
      break;

    case MultiStatusOrFrskyData:
      // Check len byte if it makes sense for multi
      if (data >= 5 && data <= 10) {
        multiTelemetryBufferState = ReceivingMultiStatus;
        telemetryRxBufferCount = 0;
      }
      else {
        multiTelemetryBufferState = FrskyTelemetryFallbackNextBytes;
        processMultiTelemetryData('M');
      }
      processMultiTelemetryData(data);
      break;

    case ReceivingMultiStatus:
      telemetryRxBuffer[telemetryRxBufferCount++] = data;
      if (telemetryRxBufferCount > 5 && telemetryRxBuffer[0] == telemetryRxBufferCount-1) {
        processMultiStatusPacket(telemetryRxBuffer+1);
        telemetryRxBufferCount = 0;
        multiTelemetryBufferState = NoProtocolDetected;
      }
      if (telemetryRxBufferCount > 10) {
        // too long ignore
        TRACE("Overlong multi status packet detected ignoring, wanted %d", telemetryRxBuffer[0]);
        telemetryRxBufferCount =0;
        multiTelemetryBufferState = NoProtocolDetected;
      }


  }

}

