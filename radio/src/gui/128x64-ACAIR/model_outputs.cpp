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

enum MenuModelOutputsItems {
  ITEM_OUTPUTS_OFFSET,
  ITEM_OUTPUTS_MIN,
  ITEM_OUTPUTS_MAX,
  ITEM_OUTPUTS_DIRECTION,
  ITEM_OUTPUTS_COUNT,
  ITEM_OUTPUTS_MAXROW = ITEM_OUTPUTS_COUNT-1
};

#define LIMITS_VALUE_POS          8*FW
#define LIMITS_OFFSET_POS         11*FW
#define LIMITS_MIN_POS            15*FW
#define LIMITS_MAX_POS            18*FW
#define LIMITS_REVERT_POS         19*FW-1

#define LIMITS_MIN_MAX_OFFSET     1000
#define CONVERT_US_MIN_MAX(x)     ((int16_t(x)*128)/25)
#define MIN_MAX_ATTR              attr

#if defined(PPM_UNIT_US)
  #define SET_MIN_MAX(x, val)     x = ((val)*250)/128
  #define MIN_MAX_DISPLAY(x)      CONVERT_US_MIN_MAX(x)
#else
  #define MIN_MAX_DISPLAY(x)      ((int8_t)(x))
#endif

void menuModelLimits(event_t event)
{
  uint8_t sub = menuVerticalPosition - HEADER_LINE;

  MENU(STR_MENULIMITS, menuTabModel, MENU_MODEL_OUTPUTS, HEADER_LINE+8, { HEADER_LINE_COLUMNS ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW, ITEM_OUTPUTS_MAXROW});

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    LimitData * ld = limitAddress(k);
    putsChn(0, y, k+1, 0);

    limit_min_max_t limit = (g_model.extendedLimits ? LIMIT_EXT_MAX : 100);

    putsChn(0, y, k+1, IS_LINE_SELECTED(sub, k) ? INVERS : 0);

#if defined(PPM_UNIT_US)
    lcdDrawNumber(LIMITS_VALUE_POS, 0, PPM_CH_CENTER(k)+channelOutputs[k]/2, RIGHT);
    // lcdDrawText(13*FW, 0, STR_US);
#else
    lcdDrawNumber(LIMITS_VALUE_POS, y, calcRESXto1000(channelOutputs[k]), RIGHT|PREC1);
#endif

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
          lcdDrawNumber(LIMITS_MIN_POS, y, MIN_MAX_DISPLAY(ld->min-LIMITS_MIN_MAX_OFFSET), MIN_MAX_ATTR|RIGHT|PREC1);
          if (active) {
            ld->min = LIMITS_MIN_MAX_OFFSET + checkIncDec(event, ld->min-LIMITS_MIN_MAX_OFFSET, -limit, 0, EE_MODEL);
          }
          break;

        case ITEM_OUTPUTS_MAX:
          lcdDrawNumber(LIMITS_MAX_POS, y, MIN_MAX_DISPLAY(ld->max+LIMITS_MIN_MAX_OFFSET), MIN_MAX_ATTR|RIGHT|PREC1);
          if (active) {
            ld->max = -LIMITS_MIN_MAX_OFFSET + checkIncDec(event, ld->max+LIMITS_MIN_MAX_OFFSET, 0, +limit, EE_MODEL);
          }
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
            ld->revert = checkIncDecModel(event, revert, 0, 1);
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
