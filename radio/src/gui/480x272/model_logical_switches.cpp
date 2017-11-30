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

enum LogicalSwitchFields {
  LS_FIELD_FUNCTION,
  LS_FIELD_V1,
  LS_FIELD_V2,
  LS_FIELD_V3,
  LS_FIELD_ANDSW,
  LS_FIELD_DURATION,
  LS_FIELD_DELAY,
  LS_FIELD_COUNT,
  LS_FIELD_LAST = LS_FIELD_COUNT-1
};

#define CSW_1ST_COLUMN  50
#define CSW_2ND_COLUMN  120
#define CSW_3RD_COLUMN  200
#define CSW_4TH_COLUMN  315
#define CSW_5TH_COLUMN  375
#define CSW_6TH_COLUMN  425

void putsEdgeDelayParam(coord_t x, coord_t y, LogicalSwitchData *cs, uint8_t lattr, uint8_t rattr)
{
  lcdDrawChar(x, y, '[');
  lcdDrawNumber(lcdNextPos+2, y, lswTimerValue(cs->v2), LEFT|PREC1|lattr);
  lcdDrawChar(lcdNextPos, y, ':');
  if (cs->v3 < 0)
    lcdDrawText(lcdNextPos+3, y, "<<", rattr);
  else if (cs->v3 == 0)
    lcdDrawText(lcdNextPos+3, y, "--", rattr);
  else
    lcdDrawNumber(lcdNextPos+3, y, lswTimerValue(cs->v2+cs->v3), LEFT|PREC1|rattr);
  lcdDrawChar(lcdNextPos, y, ']');
}

void onLogicalSwitchesMenu(const char * result)
{
  LogicalSwitchData * cs = lswAddress(menuVerticalPosition);

  if (result == STR_COPY) {
    clipboard.type = CLIPBOARD_TYPE_CUSTOM_SWITCH;
    clipboard.data.csw = *cs;
  }
  else if (result == STR_PASTE) {
    *cs = clipboard.data.csw;
    storageDirty(EE_MODEL);
  }
  else if (result == STR_CLEAR) {
    memset(cs, 0, sizeof(LogicalSwitchData));
    storageDirty(EE_MODEL);
  }
}

bool menuModelLogicalSwitches(event_t event)
{
  INCDEC_DECLARE_VARS(EE_MODEL);

  MENU(STR_MENULOGICALSWITCHES, MODEL_ICONS, menuTabModel, MENU_MODEL_LOGICAL_SWITCHES, MAX_LOGICAL_SWITCHES, { NAVIGATION_LINE_BY_LINE|LS_FIELD_LAST/*repeated...*/} );

  LogicalSwitchData * cs = lswAddress(menuVerticalPosition);
  uint8_t cstate = lswFamily(cs->func);
  if ((cstate == LS_FAMILY_EDGE && menuHorizontalPosition == LS_FIELD_DELAY) ||
      (cstate != LS_FAMILY_EDGE && menuHorizontalPosition == LS_FIELD_V3)) {
    REPEAT_LAST_CURSOR_MOVE(LS_FIELD_LAST, false);
  }

#if defined(MENU_TOOLTIPS)
  if (menuVerticalPosition>= 0 && menuHorizontalPosition>=0) {
    drawColumnHeader(STR_LSW_HEADERS, STR_LSW_DESCRIPTIONS, menuHorizontalPosition);
  }
#endif

  if (menuHorizontalPosition<0 && event==EVT_KEY_LONG(KEY_ENTER) && !READ_ONLY()) {
    killEvents(event);
    if (cs->func)
      POPUP_MENU_ADD_ITEM(STR_COPY);
    if (clipboard.type == CLIPBOARD_TYPE_CUSTOM_SWITCH)
      POPUP_MENU_ADD_ITEM(STR_PASTE);
    if (cs->func || cs->v1 || cs->v2 || cs->delay || cs->duration || cs->andsw)
      POPUP_MENU_ADD_ITEM(STR_CLEAR);
    POPUP_MENU_START(onLogicalSwitchesMenu);
  }

  for (uint8_t i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    int k = i+menuVerticalOffset;
    LcdFlags attr = (menuVerticalPosition==k ? ((s_editMode>0) ? BLINK|INVERS : INVERS)  : 0);
    LcdFlags attr1 = (menuHorizontalPosition==1 ? attr : 0);
    LcdFlags attr2 = (menuHorizontalPosition==2 ? attr : 0);
    cs = lswAddress(k);

    // CSW name
    unsigned int sw = SWSRC_SW1+k;
    drawSwitch(MENUS_MARGIN_LEFT, y, sw, (getSwitch(sw) ? BOLD : 0) | ((menuVerticalPosition==k && CURSOR_ON_LINE()) ? INVERS : 0));

    // CSW func
    lcdDrawTextAtIndex(CSW_1ST_COLUMN, y, STR_VCSWFUNC, cs->func, menuHorizontalPosition==0 ? attr : 0);

    // CSW params
    cstate = lswFamily(cs->func);
    int v1_val = cs->v1;
    int16_t v1_min = 0, v1_max = MIXSRC_LAST_TELEM;
    int16_t v2_min = 0, v2_max = MIXSRC_LAST_TELEM;
    int16_t v3_min =-1, v3_max = 100;

    if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
      drawSwitch(CSW_2ND_COLUMN, y, cs->v1, attr1);
      drawSwitch(CSW_3RD_COLUMN, y, cs->v2, attr2);
      v1_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES; v1_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      v2_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES; v2_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
      INCDEC_ENABLE_CHECK(isSwitchAvailableInLogicalSwitches);
    }
    else if (cstate == LS_FAMILY_EDGE) {
      drawSwitch(CSW_2ND_COLUMN, y, cs->v1, attr1);
      putsEdgeDelayParam(CSW_3RD_COLUMN, y, cs, attr2, (menuHorizontalPosition==LS_FIELD_V3 ? attr : 0));
      v1_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES; v1_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      v2_min=-129; v2_max = 122;
      v3_max = 222 - cs->v2;
      if (menuHorizontalPosition == 1) {
        INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
        INCDEC_ENABLE_CHECK(isSwitchAvailableInLogicalSwitches);
      }
      else {
        INCDEC_SET_FLAG(EE_MODEL);
        INCDEC_ENABLE_CHECK(NULL);
      }
    }
    else if (cstate == LS_FAMILY_COMP) {
      v1_val = cs->v1;
      drawSource(CSW_2ND_COLUMN, y, v1_val, attr1);
      drawSource(CSW_3RD_COLUMN, y, cs->v2, attr2);
      INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
      INCDEC_ENABLE_CHECK(isSourceAvailable);
    }
    else if (cstate == LS_FAMILY_TIMER) {
      lcdDrawNumber(CSW_2ND_COLUMN, y, lswTimerValue(cs->v1), LEFT|PREC1|attr1);
      lcdDrawNumber(CSW_3RD_COLUMN, y, lswTimerValue(cs->v2), LEFT|PREC1|attr2);
      v1_min = v2_min = -128;
      v1_max = v2_max = 122;
      INCDEC_SET_FLAG(EE_MODEL);
      INCDEC_ENABLE_CHECK(NULL);
    }
    else {
      v1_val = cs->v1;
      drawSource(CSW_2ND_COLUMN, y, v1_val, attr1);
      if (menuHorizontalPosition == 1) {
        INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
        INCDEC_ENABLE_CHECK(isSourceAvailableInCustomSwitches);
      }
      else {
        INCDEC_SET_FLAG(EE_MODEL);
        INCDEC_ENABLE_CHECK(NULL);
      }
      LcdFlags lf = attr2 | LEFT;
      getMixSrcRange(v1_val, v2_min, v2_max, &lf);
      drawSourceCustomValue(CSW_3RD_COLUMN, y, v1_val, (v1_val <= MIXSRC_LAST_CH ? calc100toRESX(cs->v2) : cs->v2), lf);
    }

    // CSW AND switch
    drawSwitch(CSW_4TH_COLUMN, y, cs->andsw, menuHorizontalPosition==LS_FIELD_ANDSW ? attr : 0);

    // CSW duration
    if (cs->duration > 0)
      lcdDrawNumber(CSW_5TH_COLUMN, y, cs->duration, (menuHorizontalPosition==LS_FIELD_DURATION ? attr : 0)|PREC1|LEFT);
    else
      lcdDrawMMM(CSW_5TH_COLUMN, y, menuHorizontalPosition==LS_FIELD_DURATION ? attr : 0);

    // CSW delay
    if (cstate == LS_FAMILY_EDGE) {
      lcdDrawText(CSW_6TH_COLUMN, y, STR_NA);
    }
    else if (cs->delay > 0) {
      lcdDrawNumber(CSW_6TH_COLUMN, y, cs->delay, (menuHorizontalPosition==LS_FIELD_DELAY ? attr : 0)|PREC1|LEFT);
    }
    else {
      lcdDrawMMM(CSW_6TH_COLUMN, y, menuHorizontalPosition==LS_FIELD_DELAY ? attr : 0);
    }

    if (s_editMode>0 && attr) {
      switch (menuHorizontalPosition) {
        case LS_FIELD_FUNCTION:
        {
          cs->func = checkIncDec(event, cs->func, 0, LS_FUNC_MAX, EE_MODEL, isLogicalSwitchFunctionAvailable);
          uint8_t new_cstate = lswFamily(cs->func);
          if (cstate != new_cstate) {
            unsigned int save_func = cs->func;
            memset(cs, 0, sizeof(LogicalSwitchData));
            cs->func = save_func;
            if (new_cstate == LS_FAMILY_TIMER) {
              cs->v1 = cs->v2 = -119;
            }
            else if (new_cstate == LS_FAMILY_EDGE) {
              cs->v2 = -129;
            }
          }
          break;
        }
        case LS_FIELD_V1:
          cs->v1 = CHECK_INCDEC_PARAM(event, v1_val, v1_min, v1_max);
          break;
        case LS_FIELD_V2:
          cs->v2 = CHECK_INCDEC_PARAM(event, cs->v2, v2_min, v2_max);
          if (cstate==LS_FAMILY_OFS && cs->v1!=0 && event==EVT_KEY_LONG(KEY_ENTER)) {
            killEvents(event);
            getvalue_t x = getValue(v1_val);
            if (v1_val <= MIXSRC_LAST_CH)
              cs->v2 = calcRESXto100(x);
            storageDirty(EE_MODEL);
          }
          break;
        case LS_FIELD_V3:
          cs->v3 = CHECK_INCDEC_PARAM(event, cs->v3, v3_min, v3_max);
          break;
        case LS_FIELD_ANDSW:
          INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
          INCDEC_ENABLE_CHECK(isSwitchAvailableInLogicalSwitches);
          cs->andsw = CHECK_INCDEC_PARAM(event, cs->andsw, -MAX_LS_ANDSW, MAX_LS_ANDSW);
          break;
        case LS_FIELD_DURATION:
          CHECK_INCDEC_MODELVAR_ZERO(event, cs->duration, MAX_LS_DURATION);
          break;
        case LS_FIELD_DELAY:
          CHECK_INCDEC_MODELVAR_ZERO(event, cs->delay, MAX_LS_DELAY);
          break;
      }
    }
  }

  return true;
}
