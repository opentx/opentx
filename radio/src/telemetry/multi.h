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

#ifndef OPENTX_MULTI_H
#define OPENTX_MULTI_H

/*
  Multiprotocol telemetry definition

  Serial: 100000 Baud 8e2 (same as input)

  TLV Protocol (type, length, value), allows a TX to ignore unknown messages

  Format: header (4 byte) + data (variable)

   [0] = 'M' (0x4d)
   [1] = 'P' (0x50)


   The first byte is deliberatly chosen to be different from other telemetry protocols
   (e.g. 0xAA for DSM/Multi, 0xAA for FlySky and 0x7e for Frsky) to allow a TX to detect
   the telemetry format of older versions

   [2] Type (see below)
   [3] Length (excluding the 4 header bytes)

   [4-xx] data


Type = 0x01 Multimodule Status:
   [4] Flags
   0x01 = Input signal detected
   0x02 = Serial mode enabled
   0x04 = protocol is valid
   0x08 = module is in binding mode
   [5] major
   [6] minor
   [7] revision
   [8] patchlevel,
   version of multi code, should be displayed as major.minor.revision.patchlevel

   more information can be added by specifying a longer length of the type, the TX will just ignore these bytes


Type 0x02 Frksy S.port telemetry
Type 0x03 Frsky Hub telemetry

	*No* usual frsky byte stuffing and without start/stop byte (0x7e)


Type 0x04 Spektrum telemetry data
   data[0] RSSI
   data[1-15] telemetry data

Type 0x05 DSM bind data
	data[0-16] DSM bind data

    technically DSM bind data is only 10 bytes but multi send 16
    like with telemtry, check length field)

Type 0x06 Flysky AFHDS2 telemetry data
   length: 29
   data[0] = RSSI value
   data[1-28] telemetry data

*/



void processMultiTelemetryData(uint8_t data);

// This should be put into the Module definition if other modules gain this functionality
struct MultiModuleSyncStatus {
  uint32_t adjustedRefreshRate;    // in ps
  tmr10ms_t lastUpdate;
  uint16_t refreshRate;
  uint16_t inputLag;
  uint8_t interval;
  uint8_t target;

  inline bool isValid() {return (get_tmr10ms()  - lastUpdate < 100);}
  void getRefreshString(char* refreshText);
  uint16_t getAdjustedRefreshRate();
  void calcAdjustedRefreshRate(uint16_t newRefreshRate, uint16_t newInputLag);

  MultiModuleSyncStatus() {
    // Initialise to a valid value
    adjustedRefreshRate=9000 * 1000;
  }

};

extern MultiModuleSyncStatus multiSyncStatus;


struct MultiModuleStatus {

  uint8_t major;
  uint8_t minor;
  uint8_t revision;
  uint8_t patch;

  uint8_t flags;
  tmr10ms_t lastUpdate;

  void getStatusString(char* statusText);

  inline bool isValid() { return (bool)(get_tmr10ms() - lastUpdate < 200); }
  inline bool supportsFailsafe() { return (bool) (flags & 0x20); }
  inline bool isWaitingforBind() { return (bool) (flags & 0x10); }
  inline bool isBinding() { return (bool) (flags & 0x08); }
  inline bool protocolValid() { return (bool) (flags & 0x04); }
  inline bool serialMode() { return (bool) (flags & 0x02); }
  inline bool inputDetected() { return (bool) (flags & 0x01); }
};

extern MultiModuleStatus multiModuleStatus;
enum MultiBindStatus : uint8_t {
  MULTI_NORMAL_OPERATION,
  MULTI_BIND_INITIATED,
  MULTI_BIND_FINISHED,
};

extern uint8_t multiBindStatus;

#endif //OPENTX_MULTI_H