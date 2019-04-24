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

#ifndef _PULSES_PXX2_H_
#define _PULSES_PXX2_H_

#include "fifo.h"
#include "io/frsky_pxx2.h"
#include "./pxx.h"

#define PXX2_TYPE_C_MODULE          0x01
  #define PXX2_TYPE_ID_REGISTER     0x01
  #define PXX2_TYPE_ID_BIND         0x02
  #define PXX2_TYPE_ID_CHANNELS     0x03
  #define PXX2_TYPE_ID_TX_SETTINGS  0x04
  #define PXX2_TYPE_ID_RX_SETTINGS  0x05
  #define PXX2_TYPE_ID_HW_INFO      0x06
  #define PXX2_TYPE_ID_SHARE        0x07
  #define PXX2_TYPE_ID_RESET        0x08
  #define PXX2_TYPE_ID_TELEMETRY    0xFE

#define PXX2_TYPE_C_POWER_METER     0x02
  #define PXX2_TYPE_ID_POWER_METER  0x01
  #define PXX2_TYPE_ID_SPECTRUM     0x02

#define PXX2_TYPE_C_OTA             0xFE

#define PXX2_CHANNELS_FLAG0_FAILSAFE         (1 << 6)
#define PXX2_CHANNELS_FLAG0_RANGECHECK       (1 << 7)

#define PXX2_RX_SETTINGS_FLAG0_WRITE               (1 << 6)

#define PXX2_RX_SETTINGS_FLAG1_TELEMETRY_DISABLED  (1 << 7)
#define PXX2_RX_SETTINGS_FLAG1_READONLY            (1 << 6)
#define PXX2_RX_SETTINGS_FLAG1_FASTPWM             (1 << 4)

#define PXX2_TX_SETTINGS_FLAG0_WRITE               (1 << 6)
#define PXX2_TX_SETTINGS_FLAG1_EXTERNAL_ANTENNA    (1 << 3)

#define PXX2_HW_INFO_TX_ID                         0xFF

static const char * const PXX2modulesModels[] = {
  "---",
  "XJT",
  "ISRM",
  "ISRM-PRO",
  "ISRM-S",
  "R9M",
  "R9MLite",
  "R9MLite-PRO",
  "ISRM-N"
};

static const char * const PXX2receiversModels[] = {
  "---",
  "X8R",
  "RX8R",
  "RX8R-PRO",
  "RX6R",
  "RX4R",
  "G-RX8",
  "G-RX6",
  "X6R",
  "X4R",
  "X4R-SB",
  "XSR",
  "XSR-M",
  "RXSR",
  "S6R",
  "S8R",
  "XM",
  "XM+",
  "XMR",
  "R9",
  "R9-SLIM",
  "R9-SLIM+",
  "R9-MINI",
  "R9-MM",
  "R9-STAB",
};

enum PXX2ModuleModelID {
  PXX2_MODULE_NONE,
  PXX2_MODULE_XJT,
  PXX2_MODULE_IXJT,
  PXX2_MODULE_IXJT_PRO,
  PXX2_MODULE_IXJT_S,
  PXX2_MODULE_R9M,
  PXX2_MODULE_R9M_LITE,
  PXX2_MODULE_R9M_LITE_PRO,
};

enum PXX2Variant {
  PXX2_VARIANT_NONE,
  PXX2_VARIANT_FCC,
  PXX2_VARIANT_EU,
  PXX2_VARIANT_FLEX
};

enum PXX2RegisterSteps {
  REGISTER_START,
  REGISTER_RX_NAME_RECEIVED,
  REGISTER_RX_NAME_SELECTED,
  REGISTER_OK
};

enum PXX2BindSteps {
  BIND_START,
  BIND_RX_NAME_SELECTED,
  BIND_OPTIONS_SELECTED,
  BIND_WAIT,
  BIND_OK
};

enum PXX2ResetSteps {
  RESET_START,
  RESET_OK
};

enum PXX2ReceiverStatus {
  PXX2_HARDWARE_INFO,
  PXX2_SETTINGS_READ,
  PXX2_SETTINGS_WRITE,
  PXX2_SETTINGS_OK
};

extern ModuleFifo intmoduleFifo;
extern ModuleFifo extmoduleFifo;

class Pxx2CrcMixin {
  protected:
    void initCrc()
    {
      crc = 0xFFFF;
    }

    void addToCrc(uint8_t byte)
    {
      crc -= byte;
    }

    uint16_t crc;
};

class Pxx2Transport: public DataBuffer<uint8_t, 64>, public Pxx2CrcMixin {
  protected:
    void addWord(uint32_t word)
    {
      addByte(word);
      addByte(word >> 8);
      addByte(word >> 16);
      addByte(word >> 24);
    }

    void addByte(uint8_t byte)
    {
      Pxx2CrcMixin::addToCrc(byte);
      addByteWithoutCrc(byte);
    };

    void addByteWithoutCrc(uint8_t byte)
    {
      *ptr++ = byte;
    }
};

class Pxx2Pulses: public PxxPulses<Pxx2Transport> {
  public:
    void setupFrame(uint8_t module);

  protected:
    void setupHardwareInfoFrame(uint8_t module);

    void setupRegisterFrame(uint8_t module);

    void setupBindFrame(uint8_t module);

    void setupResetFrame(uint8_t module);

    void setupShareMode(uint8_t module);

    void setupModuleSettingsFrame(uint8_t module);

    void setupReceiverSettingsFrame(uint8_t module);

    void setupChannelsFrame(uint8_t module);

    void setupTelemetryFrame(uint8_t module);

    void setupSpectrumAnalyser(uint8_t module);

    void setupPowerMeter(uint8_t module);

    void addHead()
    {
      // send 7E, do not CRC
      Pxx2Transport::addByteWithoutCrc(0x7E);

      // reserve 1 byte for LEN
      Pxx2Transport::addByteWithoutCrc(0x00);
    }

    void addFrameType(uint8_t type_c, uint8_t type_id)
    {
      // TYPE_C + TYPE_ID
      // TODO optimization ? Pxx2Transport::addByte(0x26); // This one is CRC-ed on purpose

      Pxx2Transport::addByte(type_c);
      Pxx2Transport::addByte(type_id);
    }

    uint8_t addFlag0(uint8_t module);

    void addFlag1();

    void addPulsesValues(uint16_t low, uint16_t high);

    void addChannels(uint8_t module);

    void addFailsafe(uint8_t module);

    void addCrc()
    {
      Pxx2Transport::addByteWithoutCrc(Pxx2CrcMixin::crc >> 8);
      Pxx2Transport::addByteWithoutCrc(Pxx2CrcMixin::crc);
    }

    void initFrame()
    {
      // init the CRC counter
      initCrc();

      // reset the frame pointer
      Pxx2Transport::initBuffer();

      // add the frame head
      addHead();
    }

    void endFrame()
    {
      uint8_t size = getSize() - 2;

      if (size > 0) {
        // update the frame LEN = frame length minus the 2 first bytes
        data[1] = getSize() - 2;

        // now add the CRC
        addCrc();
      }
      else {
        Pxx2Transport::initBuffer();
      }
    }
};

#endif
