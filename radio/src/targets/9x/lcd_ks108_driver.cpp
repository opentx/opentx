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

#define DISPLAY_SET_COLUMN       0x40
#define DISPLAY_SET_PAGE         0xB8
#define DISPLAY_SET_START        0XC0 
#define DISPLAY_ON_CMD	         0x3F
#define CS1_on                   PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_CS1)
#define CS1_off                  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_CS1)
#define CS2_on                   PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_CS2)
#define CS2_off                  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_CS2)
#define A0_on                    PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_A0)
#define A0_off                   PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_A0)
#define E_on                     PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_E)
#define E_off                    PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_E)

void lcdPulseEnable(void)
{
  E_on;
  _delay_us(4);
  E_off;
}

void lcdSendCtl(uint8_t val)
{
  PORTA_LCD_DAT = val;
  A0_off;
  lcdPulseEnable();
  A0_on;
}

void lcdInit()
{
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_RES);  //LCD reset
  _delay_us(20);
  PORTC_LCD_CTRL |= (1<<OUT_C_LCD_RES);  //LCD normal operation
  CS1_on;
  lcdSendCtl(DISPLAY_ON_CMD);
  lcdSendCtl(DISPLAY_SET_START);
  CS1_off;
  CS2_on;
  lcdSendCtl(DISPLAY_ON_CMD);
  lcdSendCtl(DISPLAY_SET_START);
  CS2_off;
}

void lcdSetRefVolt(uint8_t val)
{
}

void lcdRefreshSide()
{
  static uint8_t change = 0; // toggle left or right lcd writing
  uint8_t *p;
  if (change == 0){ CS2_off; CS1_on; p = displayBuf; change = 1;} // Right
  else{ CS1_off; CS2_on; p = displayBuf + 64; change = 0;} // Left
  
  for (uint8_t page=0; page < 8; page++) {
    lcdSendCtl(DISPLAY_SET_COLUMN); // Column addr 0
    lcdSendCtl( page | DISPLAY_SET_PAGE); //Page addr
    A0_on;
    for (coord_t x=64; x>0; --x) {
      PORTA_LCD_DAT = *p++;
      lcdPulseEnable();
    }
    p += 64;
  }
  A0_off;
}

void lcdRefresh()
{
  lcdRefreshSide();
  lcdRefreshSide();
}
