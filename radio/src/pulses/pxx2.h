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

#define PXX2_TYPE_C_MODULE                  0x01
  #define PXX2_TYPE_ID_REGISTER             0x01
  #define PXX2_TYPE_ID_BIND                 0x02
  #define PXX2_TYPE_ID_CHANNELS             0x03
  #define PXX2_TYPE_ID_TX_SETTINGS          0x04
  #define PXX2_TYPE_ID_RX_SETTINGS          0x05
  #define PXX2_TYPE_ID_HW_INFO              0x06
  #define PXX2_TYPE_ID_SHARE                0x07
  #define PXX2_TYPE_ID_RESET                0x08
  #define PXX2_TYPE_ID_AUTHENTICATION       0x09
  #define PXX2_TYPE_ID_TELEMETRY            0xFE

#define PXX2_TYPE_C_POWER_METER     0x02
  #define PXX2_TYPE_ID_POWER_METER  0x01
  #define PXX2_TYPE_ID_SPECTRUM     0x02

#define PXX2_TYPE_C_OTA             0xFE
  #define PXX2_TYPE_ID_OTA          0x02

#define PXX2_CHANNELS_FLAG0_FAILSAFE         (1 << 6)
#define PXX2_CHANNELS_FLAG0_RANGECHECK       (1 << 7)
#define PXX2_CHANNELS_FLAG1_RACING_MODE      (1 << 3)

#define PXX2_RX_SETTINGS_FLAG0_WRITE               (1 << 6)

#define PXX2_RX_SETTINGS_FLAG1_TELEMETRY_DISABLED  (1 << 7)
#define PXX2_RX_SETTINGS_FLAG1_READONLY            (1 << 6)
#define PXX2_RX_SETTINGS_FLAG1_SBUS24              (1 << 5)
#define PXX2_RX_SETTINGS_FLAG1_FASTPWM             (1 << 4)
#define PXX2_RX_SETTINGS_FLAG1_FPORT               (1 << 3)
#define PXX2_RX_SETTINGS_FLAG1_TELEMETRY_25MW      (1 << 2)
#define PXX2_RX_SETTINGS_FLAG1_ENABLE_PWM_CH5_CH6  (1 << 1)
#define PXX2_RX_SETTINGS_FLAG1_FPORT2              (1 << 0)

#define PXX2_TX_SETTINGS_FLAG0_WRITE               (1 << 6)
#define PXX2_TX_SETTINGS_FLAG1_EXTERNAL_ANTENNA    (1 << 3)

#define PXX2_HW_INFO_TX_ID                         0xFF

#if defined(COLORLCD)
  #define SPECTRUM_ANALYSER_POWER_FLOOR   -120 /*dBm*/
#else
  #define SPECTRUM_ANALYSER_POWER_FLOOR   -120 /*dBm*/
#endif

#define PXX2_AUTH_REFUSED_FLAG          0xA5

#define PXX2_CHMAP_SPORT      0b01000000
#define PXX2_CHMAP_FBUS       0b11000000
#define PXX2_CHMAP_SBUS_IN    0b10100000
#define PXX2_CHMAP_SBUS_OUT   0b10000000

enum PXX2ModuleModelID {
  PXX2_MODULE_NONE,
  PXX2_MODULE_XJT,
  PXX2_MODULE_ISRM,
  PXX2_MODULE_ISRM_PRO,
  PXX2_MODULE_ISRM_S,
  PXX2_MODULE_R9M,
  PXX2_MODULE_R9M_LITE,
  PXX2_MODULE_R9M_LITE_PRO,
  PXX2_MODULE_ISRM_N,
  PXX2_MODULE_ISRM_S_X9,
  PXX2_MODULE_ISRM_S_X10E,
  PXX2_MODULE_XJT_LITE,
  PXX2_MODULE_ISRM_S_X10S,
  PXX2_MODULE_ISRM_X9LITES,
};

static const char * const PXX2ModulesNames[] = {
  "---",
  "XJT",
  "ISRM",
  "ISRM-PRO",
  "ISRM-S",
  "R9M",
  "R9M Lite",
  "R9M Lite Pro",
  "ISRM-N",
  "ISRM-S-X9",
  "ISRM-S-X10E",
  "XJT Lite",
  "ISRM-S-X10S",
  "ISRM-X9LiteS"
};

inline const char * getPXX2ModuleName(uint8_t modelId)
{
  if (modelId < DIM(PXX2ModulesNames))
    return PXX2ModulesNames[modelId];
  else
    return PXX2ModulesNames[0];
}

enum {
  MODULE_OPTION_EXTERNAL_ANTENNA,
  MODULE_OPTION_POWER,
  MODULE_OPTION_SPECTRUM_ANALYSER,
  MODULE_OPTION_POWER_METER,
};

/* Module options order:
 * - External antenna (0x01)
 * - Power (0x02)
 * - Spektrum analyser (0x04)
 * - Power meter (0x08)
 */
static const uint8_t PXX2ModuleOptions[] = {
#if defined(SIMU)
  0b11111111, // None = display all options on SIMU
#else
  0b00000000, // None = no option available on unknown modules
#endif
  0b00000001, // XJT
  0b00000001, // ISRM
  0b00001101, // ISRM-PRO
  0b00000101, // ISRM-S
  0b00000010, // R9M
  0b00000010, // R9MLite
  0b00000110, // R9MLite-PRO
  0b00000100, // ISRM-N
  0b00000100, // ISRM-S-X9
  0b00000101, // ISRM-S-X10E
  0b00000001, // XJT_LITE
  0b00000101, // ISRM-S-X10S
  0b00000100, // ISRM-X9LITES
};

inline uint8_t getPXX2ModuleOptions(uint8_t modelId)
{
  if (modelId < DIM(PXX2ModuleOptions))
    return PXX2ModuleOptions[modelId];
  else
    return PXX2ModuleOptions[0];
}

inline bool isPXX2ModuleOptionAvailable(uint8_t modelId, uint8_t option)
{
  return getPXX2ModuleOptions(modelId) & (1 << option);
}

enum ModuleCapabilities {
  MODULE_CAPABILITY_COUNT
};

static const char * const PXX2ReceiversNames[] = {
  "---",
  "X8R",
  "RX8R",
  "RX8R Pro",
  "RX6R",
  "RX4R",
  "G-RX8",
  "G-RX6",
  "X6R",
  "X4R",
  "X4R SB",
  "XSR",
  "XSR M",
  "RXSR",
  "S6R",
  "S8R",
  "XM",
  "XM+",
  "XMR",
  "R9",
  "R9 SLIM",
  "R9 SLIM+",
  "R9 MINI",
  "R9 MM",
  "R9 Stab",
  "R9 Mini OTA",
  "R9 MM OTA",
  "R9 SLIM+ OTA",
  "Archer X",
  "R9MX",
  "R9SX",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "---",
  "SR10 Plus", // 0x40
  "R10 Plus",
  "GR8 Plus",
  "R8 Plus",
  "SR8 Plus",
  "GR6 Plus",
  "R6 Plus",
  "R6M (ESC DC)",
  "Rs Plus",
  "RS Mini",
  "R6FB",
  "GR6FB",
  "SR12 Plus",
  "R12 Plus",
  "R6 Mini ESC",
  "SR6 Mini",
  "SR6 Mini ESC", // 0x50
};

inline const char * getPXX2ReceiverName(uint8_t modelId)
{
  if (modelId < DIM(PXX2ReceiversNames))
    return PXX2ReceiversNames[modelId];
  else
    return PXX2ReceiversNames[0];
}

enum {
  RECEIVER_OPTION_24G,
  RECEIVER_OPTION_900M,
  RECEIVER_OPTION_OTA_TO_UPDATE_SELF,
  RECEIVER_OPTION_OTA_TO_UPDATE_OTHER,
  RECEIVER_OPTION_TANDEM,
  RECEIVER_OPTION_TWIN,
  RECEIVER_OPTION_D_TELE_PORT,
};

/* Receiver options order:
 * - 2.4G (0x01)
 * - 900M (0x02)
 * - OTA to update self (0x04)
 * - OTA to update sensors (0x08)
 * - Tandem Receiver (0x10)
 * - Twin Receiver (0x20)
 * - Dynamic Telemetry Port (0x40)
 */
static const uint8_t PXX2ReceiverOptions[] = {
#if defined(SIMU)
  0b11111111, // None = display all options on SIMU
#else
  0b00000000, // None
#endif
  0b00000001, // X8R
  0b00000001, // RX8R
  0b00000001, // RX8R-PRO
  0b00001001, // RX6R, flash sensors by OTA
  0b00001001, // RX4R, flash sensors by OTA
  0b00001001, // G-RX8, flash sensors by OTA
  0b00001001, // G-RX6, flash sensors by OTA
  0b00000001, // X6R
  0b00000001, // X4R
  0b00000001, // X4R-SB
  0b00000001, // XSR
  0b00000001, // XSR-M
  0b00001001, // RXSR, flash sensors by OTA
  0b00000001, // S6R
  0b00000001, // S8R
  0b00000001, // XM
  0b00000001, // XM+
  0b00000001, // XMR
  0b00000010, // R9
  0b00000010, // R9-SLIM
  0b00000010, // R9-SLIM+
  0b00000010, // R9-MINI
  0b00000010, // R9-MM
  0b00001110, // R9-STAB+OTA, flash self + sensors by OTA
  0b00001110, // R9-MINI+OTA, flash self + sensors by OTA
  0b00001110, // R9-MM+OTA, flash self + sensors by OTA
  0b00001110, // R9-SLIM+OTA, flash self + sensors by OTA
  0b00001101, // ARCHER-X, flash self + sensors by OTA
  0b00001110, // R9MX, flash self + sensors by OTA
  0b00001110, // R9SX, flash self + sensors by OTA
  0b00010100, // TDMX, flash self by OTA
  0b01010100, // TDR18, flash self by OTA
  0b01010100, // TDR10, flash self by OTA
  0b00010100, // TDR6, flash self by OTA
  0b01010100, // TDR12
  0b01010100, // TDSR12
  0b01010100, // TDSR18
  0b00010100, // 0x26
  0b00010100,
  0b00010100,
  0b00010100,
  0b00010100, // 0x2A
  0b00010100,
  0b00010100,
  0b00010100,
  0b00010100,
  0b00010100, // 0x2F
  0b00010100, // reserve 0x23 ~ 0x30 for TD Receivers
  0b00100100, // TWMX, flash self by OTA
  0b01100100, // TWSR12, flash self by OTA
  0b01100100, // TWR12, flash self by OTA
  0b00100100, // TWGR6, flash self by OTA
  0b00100100, // TWGR8
  0b00100100, // TWR6
  0b00100100, // TWR8
  0b00100100, // TWGR6FB
  0b00100100, // TWR6FB
  0b00100100, // 0x3A
  0b00100100,
  0b00100100,
  0b00100100,
  0b00100100,
  0b00100100, // reserve 0x38 ~ 0x3F for TW Receivers
  0b01001101, // SR10-plus
  0b01001101, // R10-plus
  0b00001101, // GR8-plus
  0b00001101, // R8-plus
  0b00001101, // SR8-plus
  0b00001101, // GR6-plus
  0b00001101, // R6-plus
  0b00001101, // R6M (ESC DC)
  0b00001101, // RS-plus
  0b00001101, // RS Mini
  0b00001101, // R6FB
  0b00001101, // GR6FB
  0b01001101, // SR12-plus
  0b01001101, // R12-plus
  0b00001101, // R6 Mini E
  0b00001101, // SR6 Mini
  0b00001101, // SR6 Mini E
};

inline uint8_t getPXX2ReceiverOptions(uint8_t modelId)
{
  if (modelId < DIM(PXX2ReceiverOptions))
    return PXX2ReceiverOptions[modelId];
  else
    return PXX2ReceiverOptions[0];
}

inline bool isPXX2ReceiverOptionAvailable(uint8_t modelId, uint8_t option)
{
  return getPXX2ReceiverOptions(modelId) & (1 << option);
}

enum ReceiverCapabilities {
  RECEIVER_CAPABILITY_FPORT,
  RECEIVER_CAPABILITY_TELEMETRY_25MW,
  RECEIVER_CAPABILITY_ENABLE_PWM_CH5_CH6,
  RECEIVER_CAPABILITY_FPORT2,
  RECEIVER_CAPABILITY_RACING_MODE,
  RECEIVER_CAPABILITY_SBUS24,
  RECEIVER_CAPABILITY_COUNT
};

enum PXX2Variant {
  PXX2_VARIANT_NONE,
  PXX2_VARIANT_FCC,
  PXX2_VARIANT_EU,
  PXX2_VARIANT_FLEX
};

enum PXX2RegisterSteps {
  REGISTER_INIT,
  REGISTER_RX_NAME_RECEIVED,
  REGISTER_RX_NAME_SELECTED,
  REGISTER_OK
};

enum PXX2BindSteps {
  BIND_MODULE_TX_INFORMATION_REQUEST = -2,
  BIND_MODULE_TX_SETTINGS_REQUEST = -1,
  BIND_INIT,
  BIND_RX_NAME_SELECTED,
  BIND_INFO_REQUEST,
  BIND_START,
  BIND_WAIT,
  BIND_OK
};

enum PXX2OtaUpdateSteps {
  OTA_UPDATE_START = BIND_OK + 1,
  OTA_UPDATE_START_ACK,
  OTA_UPDATE_TRANSFER,
  OTA_UPDATE_TRANSFER_ACK,
  OTA_UPDATE_EOF,
  OTA_UPDATE_EOF_ACK
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

class Pxx2Pulses: public Pxx2Transport {
  friend class Pxx2OtaUpdate;

  public:
    bool setupFrame(uint8_t module);
    void setupAuthenticationFrame(uint8_t module, uint8_t mode, const uint8_t * outputMessage);

  protected:
    void setupHardwareInfoFrame(uint8_t module);

    void setupRegisterFrame(uint8_t module);

    void setupAccstBindFrame(uint8_t module);

    void setupAccessBindFrame(uint8_t module);

    void setupResetFrame(uint8_t module);

    void setupShareMode(uint8_t module);

    void setupModuleSettingsFrame(uint8_t module);

    void setupReceiverSettingsFrame(uint8_t module);

    void setupChannelsFrame(uint8_t module);

    void setupTelemetryFrame(uint8_t module);

    void setupSpectrumAnalyser(uint8_t module);

    void setupPowerMeter(uint8_t module);

    void sendOtaUpdate(uint8_t module, const char * rxName, uint32_t address, const char * data);

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
      Pxx2Transport::addByte(type_c);
      Pxx2Transport::addByte(type_id);
    }

    uint8_t addFlag0(uint8_t module);

    void addFlag1(uint8_t module);

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

class Pxx2OtaUpdate {
  public:
    Pxx2OtaUpdate(uint8_t module, const char * rxName):
      module(module),
      rxName(rxName)
    {
    }

    void flashFirmware(const char * filename);

  protected:
    uint8_t module;
    const char * rxName;

    const char * doFlashFirmware(const char * filename);
    bool waitStep(uint8_t step, uint8_t timeout);
    const char * nextStep(uint8_t step, const char * rxName, uint32_t address, const uint8_t * buffer);
};

#endif
