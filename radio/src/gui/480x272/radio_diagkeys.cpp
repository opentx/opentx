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

constexpr coord_t TRIM_COLUMN = LCD_W - 120;
constexpr coord_t TRIM_MINUS_COLUMN = TRIM_COLUMN + 60;
constexpr coord_t TRIM_PLUS_COLUMN = TRIM_MINUS_COLUMN + 20;
constexpr coord_t SWITCHES_COLUMN = LCD_W / 2 - 20;

#if defined(KEYS_GPIO_PIN_PGUP)
  constexpr uint8_t KEY_START = 0;
#else
  constexpr uint8_t KEY_START = 1;
#endif

void displayKeyState(coord_t x, coord_t y, uint8_t key)
{
  uint8_t t = keys[key].state();
  lcdDrawChar(x, y, t + '0', t ? INVERS : 0);
}

bool menuRadioDiagKeys(event_t event)
{
  SIMPLE_SUBMENU(STR_MENU_RADIO_SWITCHES, ICON_MODEL_SETUP, 1);

  lcdDrawText(TRIM_COLUMN, MENU_HEADER_HEIGHT + 1, "Trims");
  lcdDrawText(TRIM_MINUS_COLUMN, MENU_HEADER_HEIGHT + 1, "-");
  lcdDrawText(TRIM_PLUS_COLUMN, MENU_HEADER_HEIGHT + 1, "+");

  for (uint8_t i = 0; i < NUM_TRIMS_KEYS; i++) {
    const uint8_t trimMap[NUM_TRIMS_KEYS] = {6, 7, 4 , 5, 2, 3, 0, 1, 8, 9, 10, 11};
    coord_t y = MENU_HEADER_HEIGHT + 1 + FH + FH * (i / 2);
    if (i & 1) {
      lcdDrawText(TRIM_COLUMN, y, "T", 0);
      lcdDrawNumber(lcdNextPos, y, i / 2 + 1, 0);
    }
    displayKeyState(i & 1 ? TRIM_PLUS_COLUMN : TRIM_MINUS_COLUMN, y, TRM_BASE + trimMap[i]);
  }

  for (uint8_t i = KEY_START; i <= 6; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + FH * (i - KEY_START);
    lcdDrawTextAtIndex(MENUS_MARGIN_LEFT, y, STR_VKEYS, (i), 0);
    displayKeyState(70, y, i);
  }

  for (uint8_t i = 0; i < NUM_SWITCHES; i++) {
    if (SWITCH_EXISTS(i)) {
      coord_t y = MENU_HEADER_HEIGHT + 1 + FH * i;
      getvalue_t val = getValue(MIXSRC_FIRST_SWITCH + i);
      getvalue_t sw = ((val < 0) ? 3 * i + 1 : ((val == 0) ? 3 * i + 2 : 3 * i + 3));
      drawSwitch(SWITCHES_COLUMN, y, sw, 0, false);
    }
  }

#if defined(ROTARY_ENCODER_NAVIGATION)
  coord_t y = MENU_HEADER_HEIGHT + FH * (8 - KEY_START);
  lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ROTARY_ENCODER);
  lcdDrawNumber(70, y, rotencValue / ROTARY_ENCODER_GRANULARITY, 0);
#endif

  return true;
}
