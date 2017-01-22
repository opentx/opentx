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

bool isThrottleOutput(uint8_t ch)
{
  for (uint8_t i=0; i<MAX_MIXERS; i++) {
    MixData *mix = mixAddress(i);
    if (mix->destCh==ch && mix->srcRaw==MIXSRC_Thr)
      return true;
  }
  return false;
}

enum MenuModelOutputsItems {
  ITEM_OUTPUTS_OFFSET,
  ITEM_OUTPUTS_MIN,
  ITEM_OUTPUTS_MAX,
  ITEM_OUTPUTS_DIRECTION,
#if defined(CPUARM)
  ITEM_OUTPUTS_CURVE,
#elif defined(PPM_CENTER_ADJUSTABLE)
  ITEM_OUTPUTS_PPM_CENTER,
#endif
#if defined(PPM_LIMITS_SYMETRICAL)
  ITEM_OUTPUTS_SYMETRICAL,
#endif
  ITEM_OUTPUTS_COUNT,
  ITEM_OUTPUTS_MAXROW = ITEM_OUTPUTS_COUNT-1
};

#if defined(PPM_UNIT_US)
  #define LIMITS_MIN_POS          12*FW+1
#else
  #define LIMITS_MIN_POS          12*FW
#endif
#if defined(CPUARM)
#define LIMITS_OFFSET_POS         7*FW+3
#else
#define LIMITS_OFFSET_POS         8*FW
#endif
#if defined(PPM_LIMITS_SYMETRICAL)
  #if defined(PPM_CENTER_ADJUSTABLE)
    #define LIMITS_MAX_POS        15*FW
    #define LIMITS_REVERT_POS     16*FW-3
    #define LIMITS_PPM_CENTER_POS 20*FW+1
  #else
    #define LIMITS_DIRECTION_POS  12*FW+4
    #define LIMITS_MAX_POS        16*FW+4
    #define LIMITS_REVERT_POS     17*FW
  #endif
#else
  #if defined(PPM_CENTER_ADJUSTABLE)
    #define LIMITS_MAX_POS        16*FW
    #define LIMITS_REVERT_POS     17*FW-2
    #define LIMITS_PPM_CENTER_POS 21*FW+2
  #else
    #define LIMITS_MAX_POS        17*FW
    #define LIMITS_REVERT_POS     18*FW
    #define LIMITS_DIRECTION_POS  12*FW+5
  #endif
#endif
#define LIMITS_CURVE_POS          17*FW+1
#define LIMITS_MIN_MAX_OFFSET 100
#define CONVERT_US_MIN_MAX(x) ((int16_t(x)*128)/25)
#define MIN_MAX_ATTR          attr

#if defined(PPM_UNIT_US)
  #define SET_MIN_MAX(x, val)   x = ((val)*250)/128
  #define MIN_MAX_DISPLAY(x)    CONVERT_US_MIN_MAX(x)
#elif defined(CPUARM)
  #define MIN_MAX_DISPLAY(x)    (x)
  #define SET_MIN_MAX(x, val)   x = (val)
#else
  #define MIN_MAX_DISPLAY(x)    ((int8_t)(x))
#endif

#if !defined(CPUARM)
void menuModelLimits(event_t event)
{
  uint8_t sub = menuVerticalPosition - HEADER_LINE;

  if (sub < MAX_OUTPUT_CHANNELS) {
#if defined(PPM_CENTER_ADJUSTABLE) || defined(PPM_UNIT_US)
    lcdDrawNumber(13*FW, 0, PPM_CH_CENTER(sub)+channelOutputs[sub]/2, RIGHT);
    lcdDrawText(13*FW, 0, STR_US);
#else
    lcdDrawNumber(13*FW, 0, calcRESXto1000(channelOutputs[sub]), RIGHT|PREC1);
#endif
  }

  MENU(STR_MENULIMITS, menuTabModel, MENU_MODEL_OUTPUTS, HEADER_LINE+MAX_OUTPUT_CHANNELS+1, { HEADER_LINE_COLUMNS ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, 0});

  if (warningResult) {
    warningResult = 0;
    LimitData * ld = limitAddress(sub);
    ld->revert = !ld->revert;
    storageDirty(EE_MODEL);
  }

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;

    if (k == MAX_OUTPUT_CHANNELS) {
      // last line available - add the "copy trim menu" line
      LcdFlags attr = (sub==MAX_OUTPUT_CHANNELS) ? INVERS : 0;
      lcdDrawText(CENTER_OFS, y, STR_TRIMS2OFFSETS, NO_HIGHLIGHT() ? 0 : attr);
      if (attr) {
        s_editMode = 0;
        if (event == EVT_KEY_LONG(KEY_ENTER)) {
          START_NO_HIGHLIGHT();
          killEvents(event);
          moveTrimsToOffsets(); // if highlighted and menu pressed - move trims to offsets
        }
      }
      return;
    }

    LimitData * ld = limitAddress(k);

#if !defined(PPM_CENTER_ADJUSTABLE)
    int16_t v = (ld->revert) ? -LIMIT_OFS(ld) : LIMIT_OFS(ld);
    char swVal = '-';  // '-', '<', '>'
    if ((channelOutputs[k] - v) > 50) swVal = (ld->revert ? 127 : 126); // Switch to raw inputs?  - remove trim!
    if ((channelOutputs[k] - v) < -50) swVal = (ld->revert ? 126 : 127);
    putsChn(0, y, k+1, 0);
    lcdDrawChar(LIMITS_DIRECTION_POS, y, swVal);
#endif

    limit_min_max_t limit = (g_model.extendedLimits ? LIMIT_EXT_MAX : 100);

    putsChn(0, y, k+1, IS_LINE_SELECTED(sub, k) ? INVERS : 0);

    for (uint8_t j=0; j<ITEM_OUTPUTS_COUNT; j++) {
      uint8_t attr = ((sub==k && menuHorizontalPosition==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      uint8_t active = (attr && (s_editMode>0 || p1valdiff)) ;
      if (active) STICK_SCROLL_DISABLE();
      switch (j) {
        case ITEM_OUTPUTS_OFFSET:
#if defined(PPM_UNIT_US)
          lcdDrawNumber(LIMITS_OFFSET_POS, y, ((int32_t)ld->offset*128) / 25, attr|PREC1|RIGHT);
#else
          lcdDrawNumber(LIMITS_OFFSET_POS, y, ld->offset, attr|PREC1|RIGHT);
#endif
          if (active) {
            ld->offset = checkIncDec(event, ld->offset, -1000, 1000, EE_MODEL|NO_INCDEC_MARKS);
          }
          else if (attr && event==EVT_KEY_LONG(KEY_MENU)) {
            copySticksToOffset(k);
            s_editMode = 0;
          }
          break;

        case ITEM_OUTPUTS_MIN:
          lcdDrawNumber(LIMITS_MIN_POS, y, MIN_MAX_DISPLAY(ld->min-LIMITS_MIN_MAX_OFFSET), MIN_MAX_ATTR|RIGHT);
          if (active) ld->min = LIMITS_MIN_MAX_OFFSET + checkIncDec(event, ld->min-LIMITS_MIN_MAX_OFFSET, -limit, 0, EE_MODEL);
          break;

        case ITEM_OUTPUTS_MAX:
          lcdDrawNumber(LIMITS_MAX_POS, y, MIN_MAX_DISPLAY(ld->max+LIMITS_MIN_MAX_OFFSET), MIN_MAX_ATTR|RIGHT);
          if (active) ld->max = -LIMITS_MIN_MAX_OFFSET + checkIncDec(event, ld->max+LIMITS_MIN_MAX_OFFSET, 0, +limit, EE_MODEL);
          break;

        case ITEM_OUTPUTS_DIRECTION:
        {
          uint8_t revert = ld->revert;
#if defined(PPM_CENTER_ADJUSTABLE)
          lcdDrawChar(LIMITS_REVERT_POS, y, revert ? 127 : 126, attr);
#else
          lcdDrawTextAtIndex(LIMITS_REVERT_POS, y, STR_MMMINV, revert, attr);
#endif
          if (active) {
            uint8_t revert_new = checkIncDecModel(event, revert, 0, 1);
            if (checkIncDec_Ret && isThrottleOutput(k)) {
              POPUP_CONFIRMATION(STR_INVERT_THR);
            }
            else {
              ld->revert = revert_new;
            }
          }
          break;
        }

#if defined(PPM_CENTER_ADJUSTABLE)
        case ITEM_OUTPUTS_PPM_CENTER:
          lcdDrawNumber(LIMITS_PPM_CENTER_POS, y, PPM_CENTER+ld->ppmCenter, RIGHT|attr);
          if (active) {
            CHECK_INCDEC_MODELVAR(event, ld->ppmCenter, -PPM_CENTER_MAX, +PPM_CENTER_MAX);
          }
          break;
#endif

#if defined(PPM_LIMITS_SYMETRICAL)
        case ITEM_OUTPUTS_SYMETRICAL:
          lcdDrawChar(LCD_W-FW, y, ld->symetrical ? '=' : '^', attr);
          if (active) {
            CHECK_INCDEC_MODELVAR_ZERO(event, ld->symetrical, 1);
          }
          break;
#endif
      }
    }
  }
}
#else // !CPUARM

enum MenuModelOutputsOneItems {
  ITEM_OUTPUTONE_CH_NAME,
  ITEM_OUTPUTONE_OFFSET,
  ITEM_OUTPUTONE_MIN,
  ITEM_OUTPUTONE_MAX,
  ITEM_OUTPUTONE_DIR,
  ITEM_OUTPUTONE_CURVE,
  ITEM_OUTPUTONE_PPM_CENTER,
  ITEM_OUTPUTONE_SYMETRICAL,
  ITEM_OUTPUTONE_MAXROW
};

#define LIMITS_ONE_2ND_COLUMN (13*FW)

void menuModelLimitsOne(event_t event)
{
  TITLE(STR_MENULIMITS);
  LimitData * ld = limitAddress(s_currIdx);

  putsChn(14*FW, 0, s_currIdx+1, 0);

  SUBMENU_NOTITLE(ITEM_OUTPUTONE_MAXROW, { 0, 0, 0, 0, 0, 0 , 0  /*, 0...*/ });

  int8_t sub = menuVerticalPosition;

  for (uint8_t k=0; k<LCD_LINES-1; k++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
    uint8_t i = k + menuVerticalOffset;
    uint8_t attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    uint8_t active = (attr && (s_editMode>0 || p1valdiff)) ;

    switch (i) {
      case ITEM_OUTPUTONE_CH_NAME:
        editSingleName(LIMITS_ONE_2ND_COLUMN, y, STR_NAME, ld->name, sizeof(ld->name), event, attr);
        break;

      case ITEM_OUTPUTONE_OFFSET:
        lcdDrawTextAlignedLeft(y, TR_LIMITS_HEADERS_SUBTRIM);
        ld->offset = GVAR_MENU_ITEM(LIMITS_ONE_2ND_COLUMN, y, ld->offset, -1000, 1000, PREC1 | attr, 0, event);
        break;

      case ITEM_OUTPUTONE_MIN:
        lcdDrawTextAlignedLeft(y, TR_MIN);
        ld->min = GVAR_MENU_ITEM(LIMITS_ONE_2ND_COLUMN, y, ld->min-LIMITS_MIN_MAX_OFFSET, -1000, 1000, PREC1 | attr, 0, event) + LIMITS_MIN_MAX_OFFSET;
        break;

      case ITEM_OUTPUTONE_MAX:
        lcdDrawTextAlignedLeft(y, TR_MAX);
        ld->max = GVAR_MENU_ITEM(LIMITS_ONE_2ND_COLUMN, y, ld->max+LIMITS_MIN_MAX_OFFSET, -1000, 1000, PREC1 | attr, 0, event) - LIMITS_MIN_MAX_OFFSET;
        break;

      case ITEM_OUTPUTONE_DIR:
      {
        uint8_t revert = ld->revert;
        lcdDrawTextAlignedLeft(y, TR_LIMITS_HEADERS_DIRECTION);
        lcdDrawTextAtIndex(LIMITS_ONE_2ND_COLUMN, y, STR_MMMINV, revert, attr);
        if (active) {
          uint8_t revert_new = checkIncDecModel(event, revert, 0, 1);
          if (checkIncDec_Ret && isThrottleOutput(k)) {
            POPUP_CONFIRMATION(STR_INVERT_THR);
          }
          else {
            ld->revert = revert_new;
          }
        }
        break;
      }

      case ITEM_OUTPUTONE_CURVE:
        lcdDrawTextAlignedLeft(y, TR_CURVE);
        drawCurveName(LIMITS_ONE_2ND_COLUMN, y, ld->curve, attr);
        if (active) {
            CHECK_INCDEC_MODELVAR(event, ld->curve, -MAX_CURVES, +MAX_CURVES);
          }
        break;

      case ITEM_OUTPUTONE_PPM_CENTER:
        lcdDrawTextAlignedLeft(y, TR_LIMITS_HEADERS_PPMCENTER);
        ld->ppmCenter = GVAR_MENU_ITEM(LIMITS_ONE_2ND_COLUMN, y, ld->ppmCenter, -PPM_CENTER_MAX, PPM_CENTER_MAX, PREC1 | attr, 0, event);
        break;

      case ITEM_OUTPUTONE_SYMETRICAL:
        lcdDrawTextAlignedLeft(y, TR_LIMITS_HEADERS_SUBTRIMMODE);
        lcdDrawChar(LIMITS_ONE_2ND_COLUMN, y, ld->symetrical ? '=' : '\306', attr);
        if (active) {
          CHECK_INCDEC_MODELVAR_ZERO(event, ld->symetrical, 1);
        }
        break;
    }
  }
}

void onLimitsMenu(const char *result)
{
  s_currIdx = menuVerticalPosition;

  if (result == STR_RESET) {
    LimitData *ld = limitAddress(s_currIdx);
    ld->min = 0;
    ld->max = 0;
    ld->offset = 0;
    ld->ppmCenter = 0;
    ld->revert = false;
    ld->curve = 0;
  }
  else if (result == STR_COPY_STICKS_TO_OFS) {
    copySticksToOffset(s_currIdx);
  }
  else if (result == STR_COPY_TRIMS_TO_OFS) {
    copyTrimsToOffset(s_currIdx);
  }
  else if (result == STR_EDIT) {
    pushMenu(menuModelLimitsOne);
  }
}

void menuModelLimits(event_t event)
{
  uint8_t sub = menuVerticalPosition - HEADER_LINE;

  if (sub < MAX_OUTPUT_CHANNELS) {
    lcdDrawNumber(13*FW, 0, PPM_CH_CENTER(sub)+channelOutputs[sub]/2, RIGHT);
    lcdDrawText(13*FW, 0, STR_US);
  }

  SIMPLE_MENU(STR_MENULIMITS, menuTabModel, MENU_MODEL_OUTPUTS, HEADER_LINE+MAX_OUTPUT_CHANNELS+1);

  if (warningResult) {
    warningResult = 0;
    LimitData * ld = limitAddress(sub);
    ld->revert = !ld->revert;
    storageDirty(EE_MODEL);
  }

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    LcdFlags attr = (sub==MAX_OUTPUT_CHANNELS) ? INVERS : 0;

    if (sub==k && event==EVT_KEY_LONG(KEY_ENTER) && !READ_ONLY()) {
      killEvents(event);
      POPUP_MENU_ADD_ITEM(STR_EDIT);
      POPUP_MENU_ADD_ITEM(STR_RESET);
      POPUP_MENU_ADD_ITEM(STR_COPY_TRIMS_TO_OFS);
      POPUP_MENU_ADD_ITEM(STR_COPY_STICKS_TO_OFS);
      POPUP_MENU_START(onLimitsMenu);
    }

    if (k == MAX_OUTPUT_CHANNELS) {
      // last line available - add the "copy trim menu" line
      lcdDrawText(CENTER_OFS, y, STR_TRIMS2OFFSETS, NO_HIGHLIGHT() ? 0 : attr);
      if (attr) {
        s_editMode = 0;
        if (event == EVT_KEY_LONG(KEY_ENTER)) {
          START_NO_HIGHLIGHT();
          killEvents(event);
          moveTrimsToOffsets(); // if highlighted and menu pressed - move trims to offsets
        }
      }
      return;
    }

    LimitData * ld = limitAddress(k);

    if (ld->name[0] == 0) {
      putsChn(0, y, k+1, (sub==k) ? INVERS : 0);
    }
    else {
      lcdDrawSizedText(0, y, ld->name, sizeof(ld->name), ((sub==k) ? INVERS : 0) | ZCHAR | LEFT);
    }

    for (uint8_t j=0; j<ITEM_OUTPUTS_COUNT; j++) {
      if (attr && (s_editMode>0 || p1valdiff)) STICK_SCROLL_DISABLE();
      switch (j) {
        case ITEM_OUTPUTS_OFFSET:
#if defined(PPM_UNIT_US)
          lcdDrawNumber(LIMITS_OFFSET_POS, y, ((int32_t)ld->offset*128) / 25, PREC1|RIGHT);
#else
          if (abs(ld->offset) >= 1000) {
            lcdDrawNumber(LIMITS_OFFSET_POS, y, ld->offset/10, RIGHT);
          }
          else {
            lcdDrawNumber(LIMITS_OFFSET_POS, y, ld->offset, PREC1|RIGHT);
          }
#endif
          break;

        case ITEM_OUTPUTS_MIN:
          lcdDrawNumber(LIMITS_MIN_POS, y, MIN_MAX_DISPLAY(ld->min-LIMITS_MIN_MAX_OFFSET), PREC1 | RIGHT);
          break;

        case ITEM_OUTPUTS_MAX:
          lcdDrawNumber(LIMITS_MAX_POS, y, MIN_MAX_DISPLAY(ld->max+LIMITS_MIN_MAX_OFFSET), PREC1 | RIGHT);
          break;

        case ITEM_OUTPUTS_DIRECTION:
        {
          uint8_t revert = ld->revert;

          lcdDrawChar(LIMITS_REVERT_POS, y, revert ? 127 : 126, 0);
          break;
        }

        case ITEM_OUTPUTS_CURVE:
          drawCurveName(LIMITS_CURVE_POS, y, ld->curve, 0);
          break;

        case ITEM_OUTPUTS_SYMETRICAL:
          lcdDrawChar(LCD_W-FW, y, ld->symetrical ? '=' : '\306', 0);
          break;
      }
    }
  }
}
#endif // !CPUARM
