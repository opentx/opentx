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

enum LogicalSwitchFields {
  LS_FIELD_FUNCTION,
  LS_FIELD_V1,
  LS_FIELD_V2,
  LS_FIELD_ANDSW,
#if defined(CPUARM)
  LS_FIELD_DURATION,
  LS_FIELD_DELAY,
#endif
  LS_FIELD_COUNT,
  LS_FIELD_LAST = LS_FIELD_COUNT-1
};

#define CSW_1ST_COLUMN  (4*FW-3)
#define CSW_2ND_COLUMN  (8*FW-3)
#define CSW_3RD_COLUMN  (13*FW-6)
#define CSW_4TH_COLUMN  (18*FW+2)

#if defined(CPUARM)
void putsEdgeDelayParam(coord_t x, coord_t y, LogicalSwitchData *cs, uint8_t lattr, uint8_t rattr)
{
  lcdDrawChar(x-4, y, '[');
  lcdDrawNumber(x, y, lswTimerValue(cs->v2), LEFT|PREC1|lattr);
  lcdDrawChar(lcdLastPos, y, ':');
  if (cs->v3 < 0)
    lcdDrawText(lcdLastPos+3, y, "<<", rattr);
  else if (cs->v3 == 0)
    lcdDrawText(lcdLastPos+3, y, "--", rattr);
  else
    lcdDrawNumber(lcdLastPos+3, y, lswTimerValue(cs->v2+cs->v3), LEFT|PREC1|rattr);
  lcdDrawChar(lcdLastPos, y, ']');
}
#endif

#if defined(CPUARM)

#define CSWONE_2ND_COLUMN (11*FW)

void menuModelLogicalSwitchOne(event_t event)
{
  TITLE(STR_MENULOGICALSWITCH);

  LogicalSwitchData * cs = lswAddress(s_currIdx);
  
  uint8_t sw = SWSRC_SW1+s_currIdx;
  uint8_t cstate = lswFamily(cs->func);
  
  drawSwitch(14*FW, 0, sw, (getSwitch(sw) ? BOLD : 0));

  SUBMENU_NOTITLE(LS_FIELD_COUNT, { 0, 0, uint8_t(cstate == LS_FAMILY_EDGE ? 1 : 0), 0 /*, 0...*/ });

  int8_t sub = menuVerticalPosition;

  int v1_val = cs->v1;

  for (uint8_t k=0; k<LCD_LINES-1; k++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
    uint8_t i = k + menuVerticalOffset;
    uint8_t attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    
    switch (i) {
      case LS_FIELD_FUNCTION:
        lcdDrawTextAlignedLeft(y, STR_FUNC);
        lcdDrawTextAtIndex(CSWONE_2ND_COLUMN, y, STR_VCSWFUNC, cs->func, attr);
        if (attr) {
          cs->func = checkIncDec(event, cs->func, 0, LS_FUNC_MAX, EE_MODEL, isLogicalSwitchFunctionAvailable);
          uint8_t new_cstate = lswFamily(cs->func);
          if (cstate != new_cstate) {
            if (new_cstate == LS_FAMILY_TIMER) {
              cs->v1 = cs->v2 = 0;
            }
            else if (new_cstate == LS_FAMILY_EDGE) {
              cs->v1 = 0; cs->v2 = -129; cs->v3 = 0;
            }
            else {
              cs->v1 = cs->v2 = 0;
            }
          }
        }
        break;
        
      case LS_FIELD_V1:
      {
        INCDEC_DECLARE_VARS(EE_MODEL);
        lcdDrawTextAlignedLeft(y, STR_V1);
        int v1_min=0, v1_max=MIXSRC_LAST_TELEM;
        if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY || cstate == LS_FAMILY_EDGE) {
          drawSwitch(CSWONE_2ND_COLUMN, y, v1_val, attr);
          v1_min = SWSRC_OFF+1; v1_max = SWSRC_ON-1;
          INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
          INCDEC_ENABLE_CHECK(isSwitchAvailableInLogicalSwitches);
        }
        else if (cstate == LS_FAMILY_TIMER) {
          lcdDrawNumber(CSWONE_2ND_COLUMN, y, lswTimerValue(cs->v1), LEFT|PREC1|attr);
          v1_min = -128;
          v1_max = 122;
        }
        else {
          v1_val = (uint8_t)cs->v1;
          drawSource(CSWONE_2ND_COLUMN, y, v1_val, attr);
          INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
          INCDEC_ENABLE_CHECK(isSourceAvailable);
        }
        if (attr) {
          cs->v1 = CHECK_INCDEC_PARAM(event, v1_val, v1_min, v1_max);
        }
        break;
      }
      
      case LS_FIELD_V2:
      {
        INCDEC_DECLARE_VARS(EE_MODEL);
        lcdDrawTextAlignedLeft(y, STR_V2);
        int v2_min=0, v2_max=MIXSRC_LAST_TELEM;
        if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
          drawSwitch(CSWONE_2ND_COLUMN, y, cs->v2, attr);
          v2_min = SWSRC_OFF+1; v2_max = SWSRC_ON-1;
          INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
          INCDEC_ENABLE_CHECK(isSwitchAvailableInLogicalSwitches);
        }
        else if (cstate == LS_FAMILY_TIMER) {
          lcdDrawNumber(CSWONE_2ND_COLUMN, y, lswTimerValue(cs->v2), LEFT|PREC1|attr);
          v2_min = -128;
          v2_max = 122;
        }
        else if (cstate == LS_FAMILY_EDGE) {
          putsEdgeDelayParam(CSWONE_2ND_COLUMN+5, y, cs, menuHorizontalPosition==0 ? attr : 0, menuHorizontalPosition==1 ? attr : 0);
          if (s_editMode <= 0) continue;
          if (attr && menuHorizontalPosition==1) {
            CHECK_INCDEC_MODELVAR(event, cs->v3, -1, 222 - cs->v2);
            break;
          }
          v2_min = -129; v2_max = 122;
        }
        else if (cstate == LS_FAMILY_COMP) {
          drawSource(CSWONE_2ND_COLUMN, y, cs->v2, attr);
          INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
          INCDEC_ENABLE_CHECK(isSourceAvailable);
        }
        else {
#if defined(TELEMETRY_FRSKY)
          if (v1_val >= MIXSRC_FIRST_TELEM) {
#if defined(CPUARM)
            drawSourceCustomValue(CSWONE_2ND_COLUMN, y, v1_val, convertLswTelemValue(cs), attr|LEFT);
#else
            drawTelemetryValue(CSWONE_2ND_COLUMN, y, v1_val - MIXSRC_FIRST_TELEM, convertLswTelemValue(cs), attr|LEFT);
#endif
            v2_max = maxTelemValue(v1_val - MIXSRC_FIRST_TELEM + 1);
            if (cs->func == LS_FUNC_DIFFEGREATER)
              v2_min = -v2_max;
            else if (cs->func == LS_FUNC_ADIFFEGREATER)
              v2_min = 0;
            else
              v2_min = minTelemValue(v1_val - MIXSRC_FIRST_TELEM + 1);
            INCDEC_SET_FLAG(EE_MODEL | INCDEC_REP10 | NO_INCDEC_MARKS);
            if (cs->v2 < v2_min || cs->v2 > v2_max) {
              cs->v2 = 0;
              storageDirty(EE_MODEL);
            }
          }
          else
#endif
          {
            v2_min = -LIMIT_EXT_PERCENT; v2_max = +LIMIT_EXT_PERCENT;
            lcdDrawNumber(CSWONE_2ND_COLUMN, y, cs->v2, LEFT|attr);
          }
        }

        if (attr) {
          cs->v2 = CHECK_INCDEC_PARAM(event, cs->v2, v2_min, v2_max);
        }
        break;
      }
      
      case LS_FIELD_ANDSW:
        lcdDrawTextAlignedLeft(y, STR_AND_SWITCH);
        drawSwitch(CSWONE_2ND_COLUMN, y, cs->andsw, attr);
        if (attr) CHECK_INCDEC_MODELVAR_CHECK(event, cs->andsw, -MAX_LS_ANDSW, MAX_LS_ANDSW, isSwitchAvailableInLogicalSwitches);
        break;
        
      case LS_FIELD_DURATION:
        lcdDrawTextAlignedLeft(y, STR_DURATION);
        if (cs->duration > 0)
          lcdDrawNumber(CSWONE_2ND_COLUMN, y, cs->duration, attr|PREC1|LEFT);
        else
          lcdDrawTextAtIndex(CSWONE_2ND_COLUMN, y, STR_MMMINV, 0, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, cs->duration, MAX_LS_DURATION);
        break;
        
      case LS_FIELD_DELAY:
        lcdDrawTextAlignedLeft(y, STR_DELAY);
        if (cs->delay > 0)
          lcdDrawNumber(CSWONE_2ND_COLUMN, y, cs->delay, attr|PREC1|LEFT);
        else
          lcdDrawTextAtIndex(CSWONE_2ND_COLUMN, y, STR_MMMINV, 0, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, cs->delay, MAX_LS_DELAY);
        break;
    }
  }
}

void menuModelLogicalSwitches(event_t event)
{
  SIMPLE_MENU(STR_MENULOGICALSWITCHES, menuTabModel, MENU_MODEL_LOGICAL_SWITCHES, HEADER_LINE+MAX_LOGICAL_SWITCHES);

  coord_t y = 0;
  uint8_t k = 0;
  int8_t sub = menuVerticalPosition - HEADER_LINE;

  switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_BREAK:
#endif
#if !defined(PCBX7D)
    case EVT_KEY_FIRST(KEY_RIGHT):
#endif
    case EVT_KEY_FIRST(KEY_ENTER):
      if (sub >= 0) {
        s_currIdx = sub;
        pushMenu(menuModelLogicalSwitchOne);
      }
      break;
  }

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    y = 1 + (i+1)*FH;
    k = i + menuVerticalOffset;
    LogicalSwitchData * cs = lswAddress(k);

    // CSW name
    uint8_t sw = SWSRC_SW1+k;

    drawSwitch(0, y, sw, (sub==k ? INVERS : 0) | (getSwitch(sw) ? BOLD : 0));

    if (cs->func > 0) {
      // CSW func
      lcdDrawTextAtIndex(CSW_1ST_COLUMN, y, STR_VCSWFUNC, cs->func, 0);

      // CSW params
      uint8_t cstate = lswFamily(cs->func);

      if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
        drawSwitch(CSW_2ND_COLUMN, y, cs->v1, 0);
        drawSwitch(CSW_3RD_COLUMN, y, cs->v2, 0);
      }
      else if (cstate == LS_FAMILY_COMP) {
        drawSource(CSW_2ND_COLUMN, y, cs->v1, 0);
        drawSource(CSW_3RD_COLUMN, y, cs->v2, 0);
      }
      else if (cstate == LS_FAMILY_EDGE) {
        drawSwitch(CSW_2ND_COLUMN, y, cs->v1, 0);
        putsEdgeDelayParam(CSW_3RD_COLUMN, y, cs, 0, 0);
      }
      else if (cstate == LS_FAMILY_TIMER) {
        lcdDrawNumber(CSW_2ND_COLUMN, y, lswTimerValue(cs->v1), LEFT|PREC1);
        lcdDrawNumber(CSW_3RD_COLUMN, y, lswTimerValue(cs->v2), LEFT|PREC1);
      }
      else {
        uint8_t v1 = cs->v1;
        drawSource(CSW_2ND_COLUMN, y, v1, 0);
        if (v1 >= MIXSRC_FIRST_TELEM) {
#if defined(CPUARM)
          drawSourceCustomValue(CSW_3RD_COLUMN, y, v1, convertLswTelemValue(cs), LEFT);
#else
          drawTelemetryValue(CSW_3RD_COLUMN, y, v1 - MIXSRC_FIRST_TELEM, convertLswTelemValue(cs), LEFT);
#endif
        }
        else {
          lcdDrawNumber(CSW_3RD_COLUMN, y, cs->v2, LEFT);
        }
      }

      // CSW and switch
      drawSwitch(CSW_4TH_COLUMN, y, cs->andsw, 0);
    }
  }
}

#else

void menuModelLogicalSwitches(event_t event)
{
  INCDEC_DECLARE_VARS(EE_MODEL);

  MENU(STR_MENULOGICALSWITCHES, menuTabModel, MENU_MODEL_LOGICAL_SWITCHES, HEADER_LINE+MAX_LOGICAL_SWITCHES, { HEADER_LINE_COLUMNS NAVIGATION_LINE_BY_LINE|LS_FIELD_LAST/*repeated...*/});

  uint8_t   k = 0;
  int8_t    sub = menuVerticalPosition - HEADER_LINE;
  horzpos_t horz = menuHorizontalPosition;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    k = i+menuVerticalOffset;
    uint8_t attr = (sub==k ? ((s_editMode>0) ? BLINK|INVERS : INVERS)  : 0);
    uint8_t attr1 = (horz==1 ? attr : 0);
    uint8_t attr2 = (horz==2 ? attr : 0);
    LogicalSwitchData * cs = lswAddress(k);

    // CSW name
    uint8_t sw = SWSRC_SW1+k;
    drawSwitch(0, y, sw, (getSwitch(sw) ? BOLD : 0) | ((sub==k && CURSOR_ON_LINE()) ? INVERS : 0));

    // CSW func
    lcdDrawTextAtIndex(CSW_1ST_COLUMN, y, STR_VCSWFUNC, cs->func, horz==0 ? attr : 0);

    // CSW params
    uint8_t cstate = lswFamily(cs->func);
#if defined(CPUARM)
    int16_t v1_val=cs->v1, v1_min=0, v1_max=MIXSRC_LAST_TELEM, v2_min=0, v2_max=MIXSRC_LAST_TELEM;
    int16_t v3_min=-1, v3_max=100;
#else
    int8_t v1_min=0, v1_max=MIXSRC_LAST_TELEM, v2_min=0, v2_max=MIXSRC_LAST_TELEM;
    #define v1_val cs->v1
#endif

    if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
      drawSwitch(CSW_2ND_COLUMN, y, cs->v1, attr1);
      drawSwitch(CSW_3RD_COLUMN, y, cs->v2, attr2);
      v1_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES; v1_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      v2_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES; v2_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
      INCDEC_ENABLE_CHECK(isSwitchAvailableInLogicalSwitches);
    }
#if defined(CPUARM)
    else if (cstate == LS_FAMILY_EDGE) {
      drawSwitch(CSW_2ND_COLUMN, y, cs->v1, attr1);
      putsEdgeDelayParam(CSW_3RD_COLUMN, y, cs, attr2, horz==LS_FIELD_V3 ? attr : 0);
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
#endif
    else if (cstate == LS_FAMILY_COMP) {
#if defined(CPUARM)
      v1_val = (uint8_t)cs->v1;
#endif
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
#if defined(CPUARM)
      v1_val = (uint8_t)cs->v1;
#endif
      drawSource(CSW_2ND_COLUMN, y, v1_val, attr1);
      if (horz == 1) {
        INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
        INCDEC_ENABLE_CHECK(isSourceAvailableInCustomSwitches);
      }
      else {
        INCDEC_SET_FLAG(EE_MODEL);
        INCDEC_ENABLE_CHECK(NULL);
      }
#if defined(CPUARM)
      drawSourceCustomValue(CSW_3RD_COLUMN, y, v1_val, calc100toRESX(cs->v2), LEFT|attr2);
      v2_min = -30000;
      v2_max = 30000;
#elif defined(TELEMETRY_FRSKY)
      if (v1_val >= MIXSRC_FIRST_TELEM) {
        drawTelemetryValue(CSW_3RD_COLUMN, y, v1_val - MIXSRC_FIRST_TELEM, convertLswTelemValue(cs), LEFT|attr2);
        v2_max = maxTelemValue(v1_val - MIXSRC_FIRST_TELEM + 1);
        if (cstate == LS_FAMILY_OFS) {
          v2_min = -128;
          v2_max -= 128;
        }
        else {
          v2_max = min((uint8_t)127, (uint8_t)v2_max);
          v2_min = -v2_max;
        }
        if (cs->v2 > v2_max) {
          cs->v2 = v2_max;
          storageDirty(EE_MODEL);
        }
      }
      else {
        lcdDrawNumber(CSW_3RD_COLUMN, y, cs->v2, LEFT|attr2);
#if defined(GVARS)
        if (v1_val >= MIXSRC_GVAR1) {
          v2_min = -1024; v2_max = +1024;
        }
        else
#endif
        {
          v2_min = -LIMIT_EXT_PERCENT; v2_max = +LIMIT_EXT_PERCENT;
        }
      }
#else
      if (v1_val >= MIXSRC_FIRST_TELEM) {
        drawTelemetryValue(CSW_3RD_COLUMN, y, v1_val - MIXSRC_FIRST_TELEM, convertLswTelemValue(cs), LEFT|attr2);
        v2_min = -128; v2_max = 127;
      }
      else {
        lcdDrawNumber(CSW_3RD_COLUMN, y, cs->v2, LEFT|attr2);
        v2_min = -LIMIT_EXT_PERCENT; v2_max = +LIMIT_EXT_PERCENT;
      }
#endif
    }

    // CSW AND switch
#if defined(CPUARM)
    drawSwitch(CSW_4TH_COLUMN, y, cs->andsw, horz==LS_FIELD_ANDSW ? attr : 0);
#else
    uint8_t andsw = cs->andsw;
    if (andsw > SWSRC_LAST_SWITCH) {
      andsw += SWSRC_SW1-SWSRC_LAST_SWITCH-1;
    }
    drawSwitch(CSW_4TH_COLUMN, y, andsw, horz==LS_FIELD_ANDSW ? attr : 0);
#endif

#if defined(CPUARM)
    // CSW duration
    if (cs->duration > 0)
      lcdDrawNumber(CSW_5TH_COLUMN, y, cs->duration, (horz==LS_FIELD_DURATION ? attr : 0)|PREC1|LEFT);
    else
      lcdDrawTextAtIndex(CSW_5TH_COLUMN, y, STR_MMMINV, 0, horz==LS_FIELD_DURATION ? attr : 0);

    // CSW delay
    if (cstate == LS_FAMILY_EDGE) {
      lcdDrawText(CSW_6TH_COLUMN, y, STR_NA);
      if (attr && horz == LS_FIELD_DELAY) {
        REPEAT_LAST_CURSOR_MOVE();
      }
    }
    else if (cs->delay > 0) {
      lcdDrawNumber(CSW_6TH_COLUMN, y, cs->delay, (horz==LS_FIELD_DELAY ? attr : 0)|PREC1|LEFT);
    }
    else {
      lcdDrawTextAtIndex(CSW_6TH_COLUMN, y, STR_MMMINV, 0, horz==LS_FIELD_DELAY ? attr : 0);
    }

    if (attr && horz == LS_FIELD_V3 && cstate != LS_FAMILY_EDGE) {
      REPEAT_LAST_CURSOR_MOVE();
    }
#endif

    if ((s_editMode>0 || p1valdiff) && attr) {
      switch (horz) {
        case LS_FIELD_FUNCTION:
        {
#if defined(CPUARM)
          cs->func = checkIncDec(event, cs->func, 0, LS_FUNC_MAX, EE_MODEL, isLogicalSwitchFunctionAvailable);
#else
          CHECK_INCDEC_MODELVAR_ZERO(event, cs->func, LS_FUNC_MAX);
#endif
          uint8_t new_cstate = lswFamily(cs->func);
          if (cstate != new_cstate) {
#if defined(CPUARM)
            if (new_cstate == LS_FAMILY_TIMER) {
              cs->v1 = cs->v2 = -119;
            }
            else if (new_cstate == LS_FAMILY_EDGE) {
              cs->v1 = 0; cs->v2 = -129; cs->v3 = 0;
            }
            else {
              cs->v1 = cs->v2 = 0;
            }
#else
            cs->v1 = cs->v2 = (new_cstate==LS_FAMILY_TIMER ? -119/*1.0*/ : 0);
#endif
          }
          break;
        }
        case LS_FIELD_V1:
          cs->v1 = CHECK_INCDEC_PARAM(event, v1_val, v1_min, v1_max);
          break;
        case LS_FIELD_V2:
          cs->v2 = CHECK_INCDEC_PARAM(event, cs->v2, v2_min, v2_max);
          break;
#if defined(CPUARM)
        case LS_FIELD_V3:
          cs->v3 = CHECK_INCDEC_PARAM(event, cs->v3, v3_min, v3_max);
          break;
#endif
        case LS_FIELD_ANDSW:
#if defined(CPUARM)
          INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
          INCDEC_ENABLE_CHECK(isSwitchAvailableInLogicalSwitches);
          cs->andsw = CHECK_INCDEC_PARAM(event, cs->andsw, -MAX_LS_ANDSW, MAX_LS_ANDSW);
#else
          CHECK_INCDEC_MODELVAR_ZERO(event, cs->andsw, MAX_LS_ANDSW);
#endif
          break;
#if defined(CPUARM)
        case LS_FIELD_DURATION:
          CHECK_INCDEC_MODELVAR_ZERO(event, cs->duration, MAX_LS_DURATION);
          break;
        case LS_FIELD_DELAY:
          CHECK_INCDEC_MODELVAR_ZERO(event, cs->delay, MAX_LS_DELAY);
          break;
#endif
      }
    }
  }
}
#endif
