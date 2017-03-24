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

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define CPN_MAX_MODELS                 60
#define CPN_MAX_TIMERS                 3
#define CPN_MAX_FLIGHT_MODES           9
#define CPN_MAX_MIXERS                 64
#define CPN_MAX_INPUTS                 32
#define CPN_MAX_EXPOS                  64
#define CPN_MAX_CURVES                 32
#define CPN_MAX_POINTS                 17
#define CPN_MAX_GVARS                  9
#define CPN_MAX_ENCODERS               2   // rotary encoders
#define CPN_MAX_CHNOUT                 32  // number of real output channels
#define CPN_MAX_CSW                    64  // number of custom switches
#define CPN_MAX_CUSTOM_FUNCTIONS       64  // number of functions assigned to switches
#define CPN_MAX_MODULES                2
#define CPN_MAX_STICKS                 Board::STICK_AXIS_COUNT
#define CPN_MAX_TRIMS                  Board::TRIM_AXIS_COUNT
#define CPN_MAX_TRIM_SW                Board::TRIM_SW_COUNT
#define CPN_MAX_KNOBS                  4
#define CPN_MAX_SLIDERS                4
#define CPN_MAX_POTS                   (CPN_MAX_KNOBS + CPN_MAX_SLIDERS)
#define CPN_MAX_CYC                    3
#define CPN_MAX_SWITCHES               32
#define CPN_MAX_KEYS                   32
#define CPN_MAX_MOUSE_ANALOGS          2
#define CPN_MAX_ANALOGS                (CPN_MAX_STICKS + CPN_MAX_POTS + CPN_MAX_MOUSE_ANALOGS)

#define CPN_STR_FILES                  QT_TRANSLATE_NOOP("CompanionMacros", "files")
#define CPN_STR_RAD_MOD_SETTINGS       QT_TRANSLATE_NOOP("CompanionMacros", "Radio and Models settings")
#define HEX_FILES_FILTER               "HEX " CPN_STR_FILES " (*.hex);;"
#define BIN_FILES_FILTER               "BIN " CPN_STR_FILES " (*.bin);;"
#define DFU_FILES_FILTER               "DFU " CPN_STR_FILES " (*.dfu);;"
#define EEPE_FILES_FILTER              "EEPE " CPN_STR_FILES " (*.eepe);;"
#define OTX_FILES_FILTER               "OpenTX " CPN_STR_FILES " (*.otx);;"
#define EEPROM_FILES_FILTER            CPN_STR_RAD_MOD_SETTINGS " " CPN_STR_FILES " (*.otx *.eepe *.bin *.hex);;" OTX_FILES_FILTER EEPE_FILES_FILTER BIN_FILES_FILTER HEX_FILES_FILTER
#define FLASH_FILES_FILTER             "FLASH " CPN_STR_FILES " (*.bin *.hex *.dfu);;" BIN_FILES_FILTER HEX_FILES_FILTER DFU_FILES_FILTER
#define EXTERNAL_EEPROM_FILES_FILTER   "EEPROM " CPN_STR_FILES " (*.bin *.hex);;" BIN_FILES_FILTER HEX_FILES_FILTER
#define ER9X_EEPROM_FILE_TYPE          "ER9X_EEPROM_FILE"
#define EEPE_EEPROM_FILE_HEADER        "EEPE EEPROM FILE"
#define EEPE_MODEL_FILE_HEADER         "EEPE MODEL FILE"

const char * const ARROW_LEFT  = "\xE2\x86\x90";
const char * const ARROW_UP    = "\xE2\x86\x91";
const char * const ARROW_RIGHT = "\xE2\x86\x92";
const char * const ARROW_DOWN  = "\xE2\x86\x93";

#endif // _CONSTANTS_H_
