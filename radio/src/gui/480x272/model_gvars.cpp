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

void onGVARSMenu(const char * result)
{
  int8_t sub = menuVerticalPosition;

  if (result == STR_ENABLE_POPUP) {
    g_model.gvars[sub].popup = true;
    storageDirty(EE_MODEL);
  }
  else if (result == STR_DISABLE_POPUP) {
    g_model.gvars[sub].popup = false;
    storageDirty(EE_MODEL);
  }
  else if (result == STR_CLEAR) {
    for (int i=0; i<MAX_FLIGHT_MODES; i++) {
      g_model.flightModeData[i].gvars[sub] = 0;
    }
    storageDirty(EE_MODEL);
  }
}

#define GVARS_FM_COLUMN(p) (150 + (p)*41)

bool menuModelGVars(event_t event)
{
  MENU(STR_MENUGLOBALVARS, MODEL_ICONS, menuTabModel, MENU_MODEL_GVARS/* TODO, first2seconds ? CHECK_FLAG_NO_SCREEN_INDEX : 0*/, MAX_GVARS, { NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES, NAVIGATION_LINE_BY_LINE|MAX_FLIGHT_MODES});

  int sub = menuVerticalPosition;
  int curfm = getFlightMode();

#if defined(MENU_TOOLTIPS)
   if (menuVerticalPosition>= 0 && menuHorizontalPosition>=0) {
     drawColumnHeader(STR_GVAR_HEADERS, NULL, menuHorizontalPosition);
   }
#endif

  for (uint8_t l=0; l<NUM_BODY_LINES; l++) {
    int i = l+menuVerticalOffset;
    coord_t y = MENU_CONTENT_TOP + l*FH;
    if (g_model.gvars[i].popup) {
      lcdDrawText(MENUS_MARGIN_LEFT+25, y, "!");
    }
    drawStringWithIndex(MENUS_MARGIN_LEFT, y, STR_GV, i+1, ((sub==i && menuHorizontalPosition<0) ? INVERS : 0));

    for (uint8_t j=0; j<1+MAX_FLIGHT_MODES; j++) {
      LcdFlags attr = ((sub==i && menuHorizontalPosition==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      coord_t x = GVARS_FM_COLUMN(j-1);
      switch (j) {
        case 0:
          editName(MENUS_MARGIN_LEFT+50, y, g_model.gvars[i].name, LEN_GVAR_NAME, event, attr);
          break;

        default:
        {
          uint16_t flightMode = j-1;
          FlightModeData * fm = &g_model.flightModeData[flightMode];
          int16_t & v = fm->gvars[i];
          int16_t vmin, vmax;
          if (v > GVAR_MAX) {
            uint8_t p = v - GVAR_MAX - 1;
            if (p >= flightMode) p++;
            drawFlightMode(x, y, p+1, attr | RIGHT | ((flightMode == curfm) ? BOLD : 0));
            vmin = GVAR_MAX+1; vmax = GVAR_MAX+MAX_FLIGHT_MODES-1;
          }
          else {
            if (abs(v) >= 1000)
              lcdDrawNumber(x, y+1, v, TINSIZE|attr|RIGHT);
            else
              lcdDrawNumber(x, y, v, attr | RIGHT | ((flightMode == curfm) ? BOLD : 0));
            vmin = -GVAR_MAX; vmax = GVAR_MAX;
          }
          if (attr) {
            if (event == EVT_KEY_LONG(KEY_ENTER) && flightMode > 0) {
              v = (v > GVAR_MAX ? 0 : GVAR_MAX+1);
              storageDirty(EE_MODEL);
            }
            else if (s_editMode>0) {
              v = checkIncDec(event, v, vmin, vmax, EE_MODEL);
            }
          }
          break;
        }
      }
    }
  }

  if (menuHorizontalPosition < 0 && event==EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    if (g_model.gvars[sub].popup)
      POPUP_MENU_ADD_ITEM(STR_DISABLE_POPUP);
    else
      POPUP_MENU_ADD_ITEM(STR_ENABLE_POPUP);
    POPUP_MENU_ADD_ITEM(STR_CLEAR);
    POPUP_MENU_START(onGVARSMenu);
  }

  return true;
}
