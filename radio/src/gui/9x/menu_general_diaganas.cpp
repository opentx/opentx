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

#include "../../opentx.h"

void menuGeneralDiagAna(uint8_t event)
{
#if defined(TX_CAPACITY_MEASUREMENT)
  #define ANAS_ITEMS_COUNT 4
#elif defined(PCBSKY9X)
  #define ANAS_ITEMS_COUNT 3
#else
  #define ANAS_ITEMS_COUNT 2
#endif

  SIMPLE_MENU(STR_MENUANA, menuTabGeneral, e_Ana, ANAS_ITEMS_COUNT);

  STICK_SCROLL_DISABLE();

  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
#if (NUM_STICKS+NUM_POTS) > 9
    coord_t y = MENU_HEADER_HEIGHT + 1 + (i/3)*FH;
    const uint8_t x_coord[] = {0, 70, 154};
    uint8_t x = x_coord[i%3];
    lcdDrawNumber(x, y, i+1, LEADING0|LEFT, 2);
    lcdDrawChar(x+2*FW-2, y, ':');
#else
    coord_t y = MENU_HEADER_HEIGHT + 1 + (i/2)*FH;
    uint8_t x = i&1 ? 64+5 : 0;
    putsStrIdx(x, y, PSTR("A"), i+1);
    lcdDrawChar(lcdNextPos, y, ':');
#endif
    lcdDrawHexNumber(x+3*FW-1, y, anaIn(i));
    lcdDraw8bitsNumber(x+10*FW-1, y, (int16_t)calibratedStick[CONVERT_MODE(i)]*25/256);
  }

#if !defined(CPUARM)
  // Display raw BandGap result (debug)
  lcdDrawText(64+5, MENU_HEADER_HEIGHT+1+3*FH, STR_BG);
  lcdDrawNumber(64+5+6*FW-3, 1+4*FH, BandGap, 0);
#endif

#if defined(PCBSKY9X)
  lcd_putsLeft(MENU_HEADER_HEIGHT+1+4*FH, STR_BATT_CALIB);
  static int32_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(TX_VOLTAGE)) / 8;
  uint32_t batCalV = (adcBatt + adcBatt*(g_eeGeneral.txVoltageCalibration)/128) * 4191;
  batCalV /= 55296;
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, MENU_HEADER_HEIGHT+1+4*FH, batCalV, (menuVerticalPosition==1 ? INVERS : 0));
#elif defined(PCBGRUVIN9X)
  lcd_putsLeft(6*FH-2, STR_BATT_CALIB);
  // Gruvin wants 2 decimal places and instant update of volts calib field when button pressed
  static uint16_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(TX_VOLTAGE)) / 8; // running average, sourced directly (to avoid unending debate :P)
  uint32_t batCalV = ((uint32_t)adcBatt*1390 + (10*(int32_t)adcBatt*g_eeGeneral.txVoltageCalibration)/8) / BandGap;
  lcdDrawNumber(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, batCalV, PREC2|(menuVerticalPosition==1 ? INVERS : 0));
#else
  lcd_putsLeft(6*FH-2, STR_BATT_CALIB);
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, g_vbat100mV, (menuVerticalPosition==1 ? INVERS : 0));
#endif
  if (menuVerticalPosition==1) CHECK_INCDEC_GENVAR(event, g_eeGeneral.txVoltageCalibration, -127, 127);

#if defined(TX_CAPACITY_MEASUREMENT)
  lcd_putsLeft(6*FH+1, STR_CURRENT_CALIB);
  putsValueWithUnit(LEN_CALIB_FIELDS*FW+4*FW, 6*FH+1, getCurrent(), UNIT_MILLIAMPS, (menuVerticalPosition==2 ? INVERS : 0)) ;
  if (menuVerticalPosition==2) CHECK_INCDEC_GENVAR(event, g_eeGeneral.txCurrentCalibration, -49, 49);
#endif

#if defined(PCBSKY9X)
  #if defined(TX_CAPACITY_MEASUREMENT)
    #define TEMP_CALIB_POS 7*FH+1
    #define TEMP_CALIB_MENU_POS 3
  #else
    #define TEMP_CALIB_POS 6*FH+1
    #define TEMP_CALIB_MENU_POS 2
  #endif

  lcd_putsLeft(TEMP_CALIB_POS, STR_TEMP_CALIB);
  putsValueWithUnit(LEN_CALIB_FIELDS*FW+4*FW, TEMP_CALIB_POS, getTemperature(), UNIT_TEMPERATURE, (menuVerticalPosition==TEMP_CALIB_MENU_POS ? INVERS : 0)) ;
  if (menuVerticalPosition==TEMP_CALIB_MENU_POS) CHECK_INCDEC_GENVAR(event, g_eeGeneral.temperatureCalib, -100, 100);
#endif
}
