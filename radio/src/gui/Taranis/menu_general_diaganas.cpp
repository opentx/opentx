/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "../../opentx.h"

void menuGeneralDiagAna(uint8_t event)
{
  SIMPLE_MENU(STR_MENUANA, menuTabGeneral, e_Ana, 1);

  STICK_SCROLL_DISABLE();

  for (int i=0; i<NUM_STICKS+NUM_POTS; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + (i/2)*FH;
    uint8_t x = i&1 ? LCD_W/2 + FW : 0;
    lcd_outdezNAtt(x, y, i+1, LEADING0|LEFT, 2);
    lcd_putc(x+2*FW-2, y, ':');
    lcd_outhex4(x+3*FW-1, y, anaIn(i));
#if defined(JITTER_MEASURE)
    lcd_outdezAtt(x+10*FW-1, y, rawJitter[i].get());
    lcd_outdezAtt(x+13*FW-1, y, avgJitter[i].get());
    lcd_outdez8(x+17*FW-1, y, (int16_t)calibratedStick[CONVERT_MODE(i)]*25/256);
#else
    lcd_outdez8(x+10*FW-1, y, (int16_t)calibratedStick[CONVERT_MODE(i)]*25/256);
#endif
  }

  lcd_putsLeft(MENU_HEADER_HEIGHT+1+6*FH, STR_BATT_CALIB);
  static int32_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(TX_VOLTAGE)) / 8;
  uint32_t batCalV = (adcBatt + (adcBatt*g_eeGeneral.txVoltageCalibration)/128) * BATT_SCALE;
  batCalV >>= 11;
  batCalV += 2; // because of the diode
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, MENU_HEADER_HEIGHT+1+6*FH, batCalV, s_editMode > 0 ? BLINK|INVERS : INVERS);
  if (s_editMode > 0) CHECK_INCDEC_GENVAR(event, g_eeGeneral.txVoltageCalibration, -127, 127);
}
