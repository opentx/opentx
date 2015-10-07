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

#define CSW_1ST_COLUMN  40
#define CSW_2ND_COLUMN  85
#define CSW_3RD_COLUMN  130
#define CSW_4TH_COLUMN  185
#define CSW_5TH_COLUMN  225
#define CSW_6TH_COLUMN  270

#define INCDEC_DECLARE_VARS(f)  uint8_t incdecFlag = (f); IsValueAvailable isValueAvailable = NULL
#define INCDEC_SET_FLAG(f)      incdecFlag = (f)
#define INCDEC_ENABLE_CHECK(fn) isValueAvailable = fn
#define CHECK_INCDEC_PARAM(event, var, min, max) checkIncDec(event, var, min, max, incdecFlag, isValueAvailable)

void putsEdgeDelayParam(coord_t x, coord_t y, LogicalSwitchData *cs, uint8_t lattr, uint8_t rattr)
{
  char s[32];
  div_t left = div(lswTimerValue(cs->v2), 10);
  char sleft[10];
  sprintf(sleft, "%d.%d", left.quot, left.rem);
  div_t right = div(lswTimerValue(cs->v2+cs->v3), 10);
  char sright[10];
  sprintf(sright, "%d.%d", right.quot, right.rem);
  sprintf(s, "[%s:%s]", sleft, sright);
  lcd_puts(x-4, y, s);
  /* if (cs->v3 < 0)
    lcd_putsAtt(lcdLastPos+3, y, "<<", rattr);
  else if (cs->v3 == 0)
    lcd_putsAtt(lcdLastPos+3, y, "--", rattr); */
}

void onLogicalSwitchesMenu(const char *result)
{
  int8_t sub = m_posVert;
  LogicalSwitchData * cs = lswAddress(sub);

  if (result == STR_COPY) {
    clipboard.type = CLIPBOARD_TYPE_CUSTOM_SWITCH;
    clipboard.data.csw = *cs;
  }
  else if (result == STR_PASTE) {
    *cs = clipboard.data.csw;
    eeDirty(EE_MODEL);
  }
  else if (result == STR_CLEAR) {
    memset(cs, 0, sizeof(LogicalSwitchData));
    eeDirty(EE_MODEL);
  }
}

void menuModelLogicalSwitches(evt_t event)
{
  INCDEC_DECLARE_VARS(EE_MODEL);

  MENU(STR_MENULOGICALSWITCHES, menuTabModel, e_LogicalSwitches, NUM_LOGICAL_SWITCH, DEFAULT_SCROLLBAR_X, { NAVIGATION_LINE_BY_LINE|LS_FIELD_LAST/*repeated...*/} );

  int k = 0;
  int sub = m_posVert;
  horzpos_t horz = m_posHorz;

  if (horz>=0) {
    displayColumnHeader(STR_CSW_HEADERS, horz);
  }

  if (horz<0 && event==EVT_KEY_LONG(KEY_ENTER) && !READ_ONLY()) {
    killEvents(event);
    LogicalSwitchData * cs = lswAddress(sub);
    if (cs->func)
      MENU_ADD_ITEM(STR_COPY);
    if (clipboard.type == CLIPBOARD_TYPE_CUSTOM_SWITCH)
      MENU_ADD_ITEM(STR_PASTE);
    if (cs->func || cs->v1 || cs->v2 || cs->delay || cs->duration || cs->andsw)
      MENU_ADD_ITEM(STR_CLEAR);
    menuHandler = onLogicalSwitchesMenu;
  }

  for (int i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    k = i+s_pgOfs;
    LcdFlags attr = (sub==k ? ((s_editMode>0) ? BLINK|INVERS : INVERS)  : 0);
    LcdFlags attr1 = (horz==1 ? attr : 0);
    LcdFlags attr2 = (horz==2 ? attr : 0);
    LogicalSwitchData * cs = lswAddress(k);

    // CSW name
    unsigned int sw = SWSRC_SW1+k;
    putsSwitches(MENU_TITLE_LEFT, y, sw, (getSwitch(sw) ? 0/*BOLD*/ : 0) | ((sub==k && CURSOR_ON_LINE()) ? INVERS : 0));

    // CSW func
    lcd_putsiAtt(CSW_1ST_COLUMN, y, STR_VCSWFUNC, cs->func, horz==0 ? attr : 0);

    // CSW params
    unsigned int cstate = lswFamily(cs->func);
    int v1_val=cs->v1, v1_min=0, v1_max=MIXSRC_LAST_TELEM;
    int v2_min=0, v2_max=MIXSRC_LAST_TELEM;
    int v3_min=-1, v3_max=100;

    if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
      putsSwitches(CSW_2ND_COLUMN, y, cs->v1, attr1);
      putsSwitches(CSW_3RD_COLUMN, y, cs->v2, attr2);
      v1_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES; v1_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      v2_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES; v2_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
      INCDEC_ENABLE_CHECK(isSwitchAvailableInLogicalSwitches);
    }
    else if (cstate == LS_FAMILY_EDGE) {
      putsSwitches(CSW_2ND_COLUMN, y, cs->v1, attr1);
      putsEdgeDelayParam(CSW_3RD_COLUMN, y, cs, attr2, (horz==LS_FIELD_V3 ? attr : 0));
      v1_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES; v1_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      v2_min=-129; v2_max = 122;
      v3_max = 222 - cs->v2;
      if (horz == 1) {
        INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
        INCDEC_ENABLE_CHECK(isSwitchAvailableInLogicalSwitches);
      }
      else {
        INCDEC_SET_FLAG(EE_MODEL);
        INCDEC_ENABLE_CHECK(NULL);
      }
    }
    else if (cstate == LS_FAMILY_COMP) {
      v1_val = (uint8_t)cs->v1;
      putsMixerSource(CSW_2ND_COLUMN, y, v1_val, attr1);
      putsMixerSource(CSW_3RD_COLUMN, y, cs->v2, attr2);
      INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
      INCDEC_ENABLE_CHECK(isSourceAvailable);
    }
    else if (cstate == LS_FAMILY_TIMER) {
      lcd_outdezAtt(CSW_2ND_COLUMN, y, lswTimerValue(cs->v1), LEFT|PREC1|attr1);
      lcd_outdezAtt(CSW_3RD_COLUMN, y, lswTimerValue(cs->v2), LEFT|PREC1|attr2);
      v1_min = v2_min = -128;
      v1_max = v2_max = 122;
      INCDEC_SET_FLAG(EE_MODEL);
      INCDEC_ENABLE_CHECK(NULL);
    }
    else {
      v1_val = (uint8_t)cs->v1;
      putsMixerSource(CSW_2ND_COLUMN, y, v1_val, attr1);
      if (horz == 1) {
        INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
        INCDEC_ENABLE_CHECK(isSourceAvailableInCustomSwitches);
      }
      else {
        INCDEC_SET_FLAG(EE_MODEL);
        INCDEC_ENABLE_CHECK(NULL);
      }
      v2_max = getMaximumValue(v1_val);
      v2_min = - v2_min;
      putsChannelValue(CSW_3RD_COLUMN, y, v1_val, v1_val <= MIXSRC_LAST_CH ? calc100toRESX(cs->v2) : cs->v2, LEFT|attr2);
    }

    // CSW AND switch
    putsSwitches(CSW_4TH_COLUMN, y, cs->andsw, horz==LS_FIELD_ANDSW ? attr : 0);

    // CSW duration
    if (cs->duration > 0)
      lcd_outdezAtt(CSW_5TH_COLUMN, y, cs->duration, (horz==LS_FIELD_DURATION ? attr : 0)|PREC1|LEFT);
    else
      lcd_putsiAtt(CSW_5TH_COLUMN, y, STR_MMMINV, 0, horz==LS_FIELD_DURATION ? attr : 0);

    // CSW delay
    if (cstate == LS_FAMILY_EDGE) {
      lcd_puts(CSW_6TH_COLUMN, y, STR_NA);
      if (attr && horz == LS_FIELD_DELAY) {
        REPEAT_LAST_CURSOR_MOVE();
      }
    }
    else if (cs->delay > 0) {
      lcd_outdezAtt(CSW_6TH_COLUMN, y, cs->delay, (horz==LS_FIELD_DELAY ? attr : 0)|PREC1|LEFT);
    }
    else {
      lcd_putsiAtt(CSW_6TH_COLUMN, y, STR_MMMINV, 0, horz==LS_FIELD_DELAY ? attr : 0);
    }

    if (attr && horz == LS_FIELD_V3 && cstate != LS_FAMILY_EDGE) {
      REPEAT_LAST_CURSOR_MOVE();
    }

    if (s_editMode>0 && attr) {
      switch (horz) {
        case LS_FIELD_FUNCTION:
        {
          cs->func = checkIncDec(event, cs->func, 0, LS_FUNC_MAX, EE_MODEL, isLogicalSwitchFunctionAvailable);
          uint8_t new_cstate = lswFamily(cs->func);
          if (cstate != new_cstate) {
            if (new_cstate == LS_FAMILY_TIMER) {
              cs->v1 = cs->v2 = -119;
            }
            else if (new_cstate == LS_FAMILY_EDGE) {
              cs->v1 = 0; cs->v2 = -129; cs->v3 = 0;
            }
            else {
              cs->v1 = cs->v2 = 0;
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
            eeDirty(EE_MODEL);
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
}
