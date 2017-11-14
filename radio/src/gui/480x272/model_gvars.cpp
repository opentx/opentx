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

void editGVarValue(coord_t x, coord_t y, event_t event, uint8_t gvar, uint8_t flightMode, LcdFlags flags)
{
  FlightModeData * fm = &g_model.flightModeData[flightMode];
  gvar_t & v = fm->gvars[gvar];
  int16_t vmin, vmax;
  if (v > GVAR_MAX) {
    uint8_t fm = v - GVAR_MAX - 1;
    if (fm >= flightMode) fm++;
    drawFlightMode(x, y, fm + 1, flags&(~LEFT));
    vmin = GVAR_MAX + 1;
    vmax = GVAR_MAX + MAX_FLIGHT_MODES - 1;
  }
  else {
    drawGVarValue(x, y, gvar, v, flags);
    vmin = GVAR_MIN + g_model.gvars[gvar].min;
    vmax = GVAR_MAX - g_model.gvars[gvar].max;
  }

  if (flags & INVERS) {
    if (event == EVT_KEY_LONG(KEY_ENTER) && flightMode > 0) {
      v = (v > GVAR_MAX ? 0 : GVAR_MAX+1);
      storageDirty(EE_MODEL);
    }
    else if (s_editMode > 0) {
      v = checkIncDec(event, v, vmin, vmax, EE_MODEL);
    }
  }
}

enum GVarFields {
  GVAR_FIELD_NAME,
  GVAR_FIELD_UNIT,
  GVAR_FIELD_PREC,
  GVAR_FIELD_MIN,
  GVAR_FIELD_MAX,
  GVAR_FIELD_FM0,
  GVAR_FIELD_LAST = GVAR_FIELD_FM0 + MAX_FLIGHT_MODES
};

bool menuModelGVarOne(event_t event)
{
  SIMPLE_SUBMENU(STR_GVARS, ICON_MODEL_GVARS, GVAR_FIELD_LAST);

  GVarData * gvar = &g_model.gvars[s_currIdx];
  drawStringWithIndex(50, 3+FH, STR_GV, s_currIdx+1, MENU_TITLE_COLOR, NULL, "=");
  drawGVarValue(lcdNextPos + 2, 3+FH, s_currIdx, getGVarValue(s_currIdx, getFlightMode()), LEFT | MENU_TITLE_COLOR);

  for (int i=0; i<NUM_BODY_LINES+1; i++) {
    coord_t y = MENU_CONTENT_TOP - FH - 2 + i*FH;
    int k = i + menuVerticalOffset;
    LcdFlags attr = (menuVerticalPosition == k ? (s_editMode > 0 ? BLINK | INVERS : INVERS) : 0);

    switch (k) {
      case GVAR_FIELD_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_NAME);
        editName(MIXES_2ND_COLUMN, y, gvar->name, LEN_GVAR_NAME, event, attr);
        break;

      case GVAR_FIELD_UNIT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_UNIT);
        gvar->unit = editChoice(MIXES_2ND_COLUMN, y, "\001-%", gvar->unit, 0, 1, attr, event);
        break;

      case GVAR_FIELD_PREC:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PRECISION);
        gvar->prec = editChoice(MIXES_2ND_COLUMN, y, STR_VPREC, gvar->prec, 0, 1, attr, event);
        break;

      case GVAR_FIELD_MIN:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MIN);
        drawGVarValue(MIXES_2ND_COLUMN, y, s_currIdx, GVAR_MIN+gvar->min, LEFT|attr);
        if (attr) gvar->min = checkIncDec(event, GVAR_MIN+gvar->min, GVAR_MIN, GVAR_MAX-gvar->max, EE_MODEL) - GVAR_MIN;
        break;

      case GVAR_FIELD_MAX:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MAX);
        drawGVarValue(MIXES_2ND_COLUMN, y, s_currIdx, GVAR_MAX-gvar->max, LEFT|attr);
        if (attr) gvar->max = GVAR_MAX - checkIncDec(event, GVAR_MAX-gvar->max, GVAR_MIN+gvar->min, GVAR_MAX, EE_MODEL);
        break;

      default:
        drawStringWithIndex(MENUS_MARGIN_LEFT, y, STR_FP, k-GVAR_FIELD_FM0);
        editGVarValue(MIXES_2ND_COLUMN, y, event, s_currIdx, k-GVAR_FIELD_FM0, LEFT|attr);
        break;
    }
  }

  return true;
}

void onGVARSMenu(const char * result)
{
  int sub = menuVerticalPosition;

  if (result == STR_EDIT) {
    s_currIdx = sub;
    pushMenu(menuModelGVarOne);
  }
  else if (result == STR_CLEAR) {
    for (int i=0; i<MAX_FLIGHT_MODES; i++) {
      g_model.flightModeData[i].gvars[sub] = 0;
    }
    storageDirty(EE_MODEL);
  }
}

#define GVARS_FM_COLUMN(p)             (127 + (p)*44)
#define GVAR_MAX_COLUMNS               (NAVIGATION_LINE_BY_LINE|(MAX_FLIGHT_MODES-1))

bool menuModelGVars(event_t event)
{
  MENU(STR_MENUGLOBALVARS, MODEL_ICONS, menuTabModel, MENU_MODEL_GVARS/* TODO, first2seconds ? CHECK_FLAG_NO_SCREEN_INDEX : 0*/, MAX_GVARS, { GVAR_MAX_COLUMNS, GVAR_MAX_COLUMNS, GVAR_MAX_COLUMNS, GVAR_MAX_COLUMNS, GVAR_MAX_COLUMNS, GVAR_MAX_COLUMNS, GVAR_MAX_COLUMNS, GVAR_MAX_COLUMNS, GVAR_MAX_COLUMNS});

  int sub = menuVerticalPosition;
  int curfm = getFlightMode();

#if defined(MENU_TOOLTIPS)
   if (menuVerticalPosition>= 0 && menuHorizontalPosition>=0) {
     drawColumnHeader(STR_GVAR_HEADERS, NULL, menuHorizontalPosition);
   }
#endif

  for (uint8_t l=0; l<NUM_BODY_LINES; l++) {
    int i = l + menuVerticalOffset;
    coord_t y = MENU_CONTENT_TOP + l*FH;
    if (g_model.gvars[i].popup) {
      lcdDrawText(MENUS_MARGIN_LEFT+25, y, "!");
    }

    drawStringWithIndex(MENUS_MARGIN_LEFT-3, y, STR_GV, i+1, ((sub==i && menuHorizontalPosition<0) ? INVERS : 0));
    lcdDrawSizedText(MENUS_MARGIN_LEFT+35, y, g_model.gvars[i].name, LEN_GVAR_NAME, ZCHAR);

    for (int j=0; j<MAX_FLIGHT_MODES; j++) {
      FlightModeData * fm = &g_model.flightModeData[j];
      gvar_t v = fm->gvars[i];

      LcdFlags attr = RIGHT | ((sub == i && menuHorizontalPosition == j) ? (s_editMode > 0 ? BLINK | INVERS : INVERS) : 0);
      if (j == curfm)
        attr |= BOLD;
      coord_t x = GVARS_FM_COLUMN(j);
      coord_t yval = y;
      if (v <= GVAR_MAX && (g_model.gvars[i].prec > 0 || abs(v) >= 1000 || ( abs(v) >= 100 && g_model.gvars[i].unit > 0))) {
        attr |= SMLSIZE;
        yval += 3;
      }
      if (v <= GVAR_MAX && g_model.gvars[i].unit > 0) {
        x -= 9;
        lcdDrawText(GVARS_FM_COLUMN(j) - 9, y+5, "%", TINSIZE);
      }
      editGVarValue(x, yval, event, i, j, attr | NO_UNIT);
    }
  }

  if (menuHorizontalPosition < 0 && event==EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    POPUP_MENU_ADD_ITEM(STR_EDIT);
    POPUP_MENU_ADD_ITEM(STR_CLEAR);
    POPUP_MENU_START(onGVARSMenu);
  }

  return true;
}
