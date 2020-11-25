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

#pragma once

#include <inttypes.h>
#include "dataconstants.h"

// Device (destination) address
#define GHST_ADDR_RADIO                 0x80    // phase 1
#define GHST_ADDR_MODULE_SYM            0x81    // symmetrical, 400k pulses, 400k telemetry
#define GHST_ADDR_MODULE_ASYM           0x88    // asymmetrical, 400k pulses, 115k telemetry
#define GHST_ADDR_FC                    0x82
#define GHST_ADDR_GOGGLES               0x83    // phase 2
#define GHST_ADDR_5G_TXCTRL             0x84	// phase 3
#define GHST_ADDR_5G_TWRSCAN            0x85
#define GHST_ADDR_5G_RLY                0x86

#define GHST_UL_RC_CHANS_HS4_5TO8	0x10	// High Speed 4 channel (12 bits), plus CH5-8 (8 bits)
#define GHST_UL_RC_CHANS_HS4_9TO12	0x11	// High Speed 4 channel (12 bits), plus CH9-12 (8 bits)
#define GHST_UL_RC_CHANS_HS4_13TO16	0x12	// High Speed 4 channel (12 bits), plus CH13-16 (8 bits)
#define GHST_UL_RC_CHANS_SIZE           12      // 1 (type) + 10 (data) + 1 (crc)
#define GHST_UL_MENU_CTRL               0x13

#define GHST_DL_OPENTX_SYNC		0x20
#define GHST_DL_LINK_STAT               0x21
#define GHST_DL_VTX_STAT                0x22
#define GHST_DL_PACK_STAT               0x23
#define GHST_DL_MENU_DESC               0x24

#define GHST_RC_CTR_VAL_12BIT		0x7C0   // 0x3e0 << 1
#define GHST_RC_CTR_VAL_8BIT		0x7C

#define GHST_CH_BITS_12                 12
#define GHST_CH_BITS_8                  8

enum GhstPowerMode
{
  GHST_PWR_16UW =   0,
  GHST_PWR_100UW =  1,
  GHST_PWR_1MW =    2,
  GHST_PWR_25MW =   3,
  GHST_PWR_100MW =  4,
  GHST_PWR_200MW =  5,
  GHST_PWR_350MW =  6,
  GHST_PWR_500MW =  7,
  GHST_PWR_600MW =  8,
  GHST_PWR_1W =     9,
  GHST_PWR_1_5W =   10,
  GHST_PWR_2W =     11,
  GHST_PWR_3W =     12,
  GHST_PWR_4W =     13,
  GHST_PWR_COUNT
};

enum GhstRFProfile
{
  GHST_RF_PROFILE_AUTO        = 0,
  GHST_RF_PROFILE_NORMAL      = 1,
  GHST_RF_PROFILE_RACE        = 2,
  GHST_RF_PROFILE_PURERACE    = 3,
  GHST_RF_PROFILE_LONGRANGE   = 4,
  GHST_RF_PROFILE_RESERVED    = 5,
  GHST_RF_PROFILE_RACE2       = 6,
  GHST_RF_PROFILE_PURERACE2   = 7,
  GHST_RF_PROFILE_MAX = GHST_RF_PROFILE_PURERACE2,
  GHST_RF_PROFILE_COUNT
};

enum GhstVtxBand
{
  GHST_VTX_BAND_UNKNOWN     = 0,
  GHST_VTX_BAND_IRC         = 1,
  GHST_VTX_BAND_RACE        = 2,
  GHST_VTX_BAND_BANDE       = 3,
  GHST_VTX_BAND_BANDB       = 4,
  GHST_VTX_BAND_BANDA       = 5,
  GHST_VTX_BAND_MAX = GHST_VTX_BAND_BANDA,
  GHST_VTX_BAND_COUNT
};

void processGhostTelemetryData(uint8_t data);
void ghostSetDefault(int index, uint8_t id, uint8_t subId);

#if SPORT_MAX_BAUDRATE < 400000
// For radios which can't support telemetry at high rates, offer baud rate choices
// (modified vs. unmodified radios)

constexpr uint16_t BRR_400K = PERI1_FREQUENCY / 400000;
constexpr uint16_t BRR_115K = PERI1_FREQUENCY / 115200;

enum GhostTelemetryBaudrates
{
  GHST_TELEMETRY_RATE_400K,
  GHST_TELEMETRY_RATE_115K
};
#endif
#define GHOST_BAUDRATE       400000
#define GHOST_PERIOD         4500        /* us; 222.22 Hz */

typedef enum
{
  GHST_LINE_FLAGS_None = 0x00,
  GHST_LINE_FLAGS_LabelSelect = 0x01,
  GHST_LINE_FLAGS_ValueSelect = 0x02,
  GHST_LINE_FLAGS_ValueEdit = 0x04,
} GHST_LINE_FLAGS;

typedef enum
{
  GHST_BTN_None = 0x00,
  GHST_BTN_JoyPress = 0x01,
  GHST_BTN_JoyUp = 0x02,
  GHST_BTN_JoyDown = 0x04,
  GHST_BTN_JoyLeft = 0x08,
  GHST_BTN_JoyRight = 0x10,
  GHST_BTN_Bind = 0x20			// future, for no-UI Ghost
} GHST_BTN;

typedef enum
{
  GHST_MENU_CTRL_None = 0x00,
  GHST_MENU_CTRL_Open = 0x01,
  GHST_MENU_CTRL_Close = 0x02,
  GHST_MENU_CTRL_Redraw = 0x04,
} GHST_MENU_CTRL;

enum GhostMenuStatus
{
  GHST_MENU_STATUS_UNOPENED = 0x00,
  GHST_MENU_STATUS_OPENED = 0x01,
  GHST_MENU_STATUS_CLOSING = 0x02,
};

enum GhostFrames{
  GHST_FRAME_CHANNEL,
  GHST_MENU_CONTROL
};

constexpr uint8_t GHST_MENU_LINES = 6;
constexpr uint8_t GHST_MENU_CHARS = 20;

// GHST_DL_MENU_DESC (27 bytes)
typedef struct
{
  uint8_t address;
  uint8_t length ;
  uint8_t packetId;
  uint8_t menuStatus;     // GHST_MENU_CTRL
  uint8_t lineFlags;     // Carat states, Inverse, Bold for each of Menu Label, and Value
  uint8_t lineIndex;     // 0 = first line
  unsigned char menuText[GHST_MENU_CHARS];
  uint8_t crc;
}  ghst_menu_frame;

typedef struct
{
  uint8_t menuFlags;     // Update Line, Clear Menu, etc.
  uint8_t lineFlags;     // Carat states, Inverse, Bold for each of Menu Label, and Value
  uint8_t splitLine;     // Store beginning of Value substring
  char menuText[GHST_MENU_CHARS + 1];
}  GhostMenuData;

