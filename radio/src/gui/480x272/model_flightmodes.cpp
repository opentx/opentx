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

#include <stdio.h>
#include "opentx.h"

void displayFlightModes(coord_t x, coord_t y, FlightModesType value, uint8_t attr)
{
  for (int i=0; i<MAX_FLIGHT_MODES; i++) {
    LcdFlags flags = ((menuHorizontalPosition==i && attr) ? INVERS : 0);
    flags |= ((value & (1<<i))) ? TEXT_DISABLE_COLOR : TEXT_COLOR;
    if (attr && menuHorizontalPosition < 0) flags |= INVERS;
    char s[] = " ";
    s[0] = '0' + i;
    lcdDrawText(x, y, s, flags);
    x += 12;
  }
}

FlightModesType editFlightModes(coord_t x, coord_t y, event_t event, FlightModesType value, uint8_t attr)
{
  displayFlightModes(x, y, value, attr);

  if (attr) {
    if (s_editMode && event==EVT_KEY_BREAK(KEY_ENTER)) {
      s_editMode = 0;
      value ^= (1<<menuHorizontalPosition);
      storageDirty(EE_MODEL);
    }
  }

  return value;
}

enum FlightModesItems {
  ITEM_FLIGHT_MODES_NAME,
  ITEM_FLIGHT_MODES_SWITCH,
  ITEM_FLIGHT_MODES_TRIM_RUD,
  ITEM_FLIGHT_MODES_TRIM_ELE,
  ITEM_FLIGHT_MODES_TRIM_THR,
  ITEM_FLIGHT_MODES_TRIM_AIL,
  ITEM_FLIGHT_MODES_TRIM_T5,
  ITEM_FLIGHT_MODES_TRIM_T6,
  ITEM_FLIGHT_MODES_FADE_IN,
  ITEM_FLIGHT_MODES_FADE_OUT,
  ITEM_FLIGHT_MODES_COUNT,
  ITEM_FLIGHT_MODES_LAST = ITEM_FLIGHT_MODES_COUNT-1
};

bool isTrimModeAvailable(int mode)
{
  return (mode < 0 || (mode%2) == 0 || (mode/2) != menuVerticalPosition);
}

#define FLIGHT_MODES_NAME_COLUMN      50
#define FLIGHT_MODES_SWITCH_COLUMN    180
#define FLIGHT_MODES_TRIMS_COLUMN     175
#define FLIGHT_MODES_TRIM_WIDTH       26
#define FLIGHT_MODES_FADEIN_COLUMN    420
#define FLIGHT_MODES_FADEOUT_COLUMN   460

bool menuModelFlightModesAll(event_t event)
{
  MENU(STR_MENUFLIGHTMODES, MODEL_ICONS, menuTabModel, MENU_MODEL_FLIGHT_MODES, MAX_FLIGHT_MODES+1, { NAVIGATION_LINE_BY_LINE|ITEM_FLIGHT_MODES_LAST, NAVIGATION_LINE_BY_LINE|ITEM_FLIGHT_MODES_LAST, NAVIGATION_LINE_BY_LINE|ITEM_FLIGHT_MODES_LAST, NAVIGATION_LINE_BY_LINE|NAVIGATION_LINE_BY_LINE|ITEM_FLIGHT_MODES_LAST, NAVIGATION_LINE_BY_LINE|ITEM_FLIGHT_MODES_LAST, NAVIGATION_LINE_BY_LINE|ITEM_FLIGHT_MODES_LAST, NAVIGATION_LINE_BY_LINE|ITEM_FLIGHT_MODES_LAST, NAVIGATION_LINE_BY_LINE|ITEM_FLIGHT_MODES_LAST, NAVIGATION_LINE_BY_LINE|ITEM_FLIGHT_MODES_LAST, 0});

  if (menuVerticalPosition==0 && menuHorizontalPosition==ITEM_FLIGHT_MODES_SWITCH) {
    menuHorizontalPosition += CURSOR_MOVED_LEFT(event) ? -1 : +1;
  }

  if (menuVerticalPosition<MAX_FLIGHT_MODES && menuHorizontalPosition>=0) {
    drawColumnHeader(STR_PHASES_HEADERS, NULL, menuHorizontalPosition);
  }

  for (uint8_t i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    int k = i+menuVerticalOffset;

    if (k==MAX_FLIGHT_MODES) {
      // last line available - add the "check trims" line
      LcdFlags attr = 0;
      if (menuVerticalPosition==MAX_FLIGHT_MODES) {
        if (!trimsCheckTimer) {
          if (event == EVT_KEY_FIRST(KEY_ENTER)) {
            trimsCheckTimer = 200; // 2 seconds trims cancelled
            s_editMode = 1;
            killEvents(event);
          }
          else {
            attr |= INVERS;
            s_editMode = 0;
          }
        }
        else {
          if (event == EVT_KEY_FIRST(KEY_EXIT)) {
            trimsCheckTimer = 0;
            s_editMode = 0;
            killEvents(event);
          }
        }
      }
      char s[32];
      sprintf(s, "Check FM%d Trims", mixerCurrentFlightMode);
      lcdDrawText(LCD_W/2, y, s, CENTERED|attr);
      return true;
    }

    FlightModeData * p = flightModeAddress(k);

    drawFlightMode(MENUS_MARGIN_LEFT, y, k+1, (getFlightMode()==k ? BOLD : 0) | ((menuVerticalPosition==k && menuHorizontalPosition<0) ? INVERS : 0));

    for (uint8_t j=0; j<ITEM_FLIGHT_MODES_COUNT; j++) {
      LcdFlags attr = ((menuVerticalPosition==k && menuHorizontalPosition==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      LcdFlags active = (attr && s_editMode>0) ;
      switch (j) {
        case ITEM_FLIGHT_MODES_NAME:
          editName(FLIGHT_MODES_NAME_COLUMN, y, p->name, sizeof(p->name), event, attr);
          break;

        case ITEM_FLIGHT_MODES_SWITCH:
          if (active) CHECK_INCDEC_MODELSWITCH(event, p->swtch, SWSRC_FIRST_IN_MIXES, SWSRC_LAST_IN_MIXES, isSwitchAvailableInMixes);
          if (k == 0)
            lcdDrawText(FLIGHT_MODES_SWITCH_COLUMN, y, "N/A");
          else
            drawSwitch(FLIGHT_MODES_SWITCH_COLUMN, y, p->swtch, attr);
          break;

        case ITEM_FLIGHT_MODES_TRIM_RUD:
        case ITEM_FLIGHT_MODES_TRIM_ELE:
        case ITEM_FLIGHT_MODES_TRIM_THR:
        case ITEM_FLIGHT_MODES_TRIM_AIL:
        case ITEM_FLIGHT_MODES_TRIM_T5:
        case ITEM_FLIGHT_MODES_TRIM_T6:
        {
          uint8_t t = j-ITEM_FLIGHT_MODES_TRIM_RUD;
          if (active) {
            trim_t & v = p->trim[t];
            v.mode = checkIncDec(event, v.mode==TRIM_MODE_NONE ? -1 : v.mode, -1, k==0 ? 0 : 2*MAX_FLIGHT_MODES-1, EE_MODEL, isTrimModeAvailable);
          }
          drawTrimMode(FLIGHT_MODES_TRIMS_COLUMN+j*FLIGHT_MODES_TRIM_WIDTH, y, k, t, attr);
          break;
        }

        case ITEM_FLIGHT_MODES_FADE_IN:
          if (active) p->fadeIn = checkIncDec(event, p->fadeIn, 0, DELAY_MAX, EE_MODEL|NO_INCDEC_MARKS);
          lcdDrawNumber(FLIGHT_MODES_FADEIN_COLUMN, y, (10/DELAY_STEP)*p->fadeIn, attr|PREC1|RIGHT);
          break;

        case ITEM_FLIGHT_MODES_FADE_OUT:
          if (active) p->fadeOut = checkIncDec(event, p->fadeOut, 0, DELAY_MAX, EE_MODEL|NO_INCDEC_MARKS);
          lcdDrawNumber(FLIGHT_MODES_FADEOUT_COLUMN, y, (10/DELAY_STEP)*p->fadeOut, attr|PREC1|RIGHT);
          break;

      }
    }
  }

  return true;
}
