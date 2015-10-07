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

bool isThrottleOutput(uint8_t ch)
{
  for (uint8_t i=0; i<MAX_MIXERS; i++) {
    MixData *mix = mixAddress(i);
    if (mix->destCh==ch && mix->srcRaw==MIXSRC_Thr)
      return true;
  }
  return false;
}

enum LimitsItems {
  ITEM_LIMITS_CH_NAME,
  ITEM_LIMITS_OFFSET,
  ITEM_LIMITS_MIN,
  ITEM_LIMITS_MAX,
  ITEM_LIMITS_DIRECTION,
#if defined(CURVES)
  ITEM_LIMITS_CURVE,
#endif
#if defined(PPM_CENTER_ADJUSTABLE)
  ITEM_LIMITS_PPM_CENTER,
#endif
#if defined(PPM_LIMITS_SYMETRICAL)
  ITEM_LIMITS_SYMETRICAL,
#endif
  ITEM_LIMITS_COUNT,
  ITEM_LIMITS_MAXROW = ITEM_LIMITS_COUNT-1
};

#if defined(PPM_CENTER_ADJUSTABLE)
  #define LIMITS_NAME_POS         44
  #define LIMITS_OFFSET_POS       136
  #define LIMITS_MIN_POS          175
  #define LIMITS_DIRECTION_POS    177
  #define LIMITS_MAX_POS          214
  #define LIMITS_REVERT_POS       218
  #define LIMITS_CURVE_POS        232
  #define LIMITS_PPM_CENTER_POS   292
  #define LIMITS_SYMETRICAL_POS   298
#else
  #define LIMITS_NAME_POS         44
  #define LIMITS_OFFSET_POS       136
  #define LIMITS_MIN_POS          178
  #define LIMITS_DIRECTION_POS    181
  #define LIMITS_MAX_POS          218
  #define LIMITS_REVERT_POS       228
  #define LIMITS_CURVE_POS        258
  #define LIMITS_SYMETRICAL_POS   294
#endif

#define LIMITS_MIN_MAX_OFFSET 1000
#define CONVERT_US_MIN_MAX(x) (((x)*1280)/250)

#if defined(PPM_UNIT_US)
  #define SET_MIN_MAX(x, val)   x = ((val)*250)/128
  #define MIN_MAX_DISPLAY(x)    CONVERT_US_MIN_MAX(x)
#else
  #define MIN_MAX_DISPLAY(x)    (x)
  #define SET_MIN_MAX(x, val)   x = (val)
#endif

void onLimitsMenu(const char *result)
{
  uint8_t ch = m_posVert - 1;

  if (result == STR_RESET) {
    LimitData *ld = limitAddress(ch);
    ld->min = 0;
    ld->max = 0;
    ld->offset = 0;
    ld->ppmCenter = 0;
    ld->revert = false;
    ld->curve = 0;
  }
  else if (result == STR_COPY_STICKS_TO_OFS) {
    copySticksToOffset(ch);
  }
  else if (result == STR_COPY_TRIMS_TO_OFS) {
    copyTrimsToOffset(ch);
  }
}

void menuModelLimits(evt_t event)
{
  MENU(STR_MENULIMITS, menuTabModel, e_Limits, NUM_CHNOUT+1, DEFAULT_SCROLLBAR_X,
      { NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW,
        NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW,
        NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW,
        NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW, NAVIGATION_LINE_BY_LINE|ITEM_LIMITS_MAXROW,
        0 });

  uint32_t sub = m_posVert;

  if (sub < NUM_CHNOUT) {
#if defined(PPM_CENTER_ADJUSTABLE) || defined(PPM_UNIT_US)
    lcd_outdezAtt(100, MENU_FOOTER_TOP, PPM_CH_CENTER(sub)+channelOutputs[sub]/2, HEADER_COLOR, STR_US);
#else
    lcd_outdezAtt(100, MENU_FOOTER_TOP, calcRESXto1000(channelOutputs[sub]), HEADER_COLOR|PREC1);
#endif
  }

  if (sub<NUM_CHNOUT && m_posHorz>=0) {
    displayColumnHeader(STR_LIMITS_HEADERS, m_posHorz);
  }

  if (s_warning_result) {
    s_warning_result = 0;
    LimitData *ld = limitAddress(sub);
    ld->revert = !ld->revert;
    eeDirty(EE_MODEL);
  }

  for (int i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    uint8_t k = i+s_pgOfs;

    if (k==NUM_CHNOUT) {
      // last line available - add the "copy trim menu" line
      uint8_t attr = (sub==NUM_CHNOUT) ? INVERS : 0;
      // TODO CENTER attribute
      lcd_putsAtt(100, y, STR_TRIMS2OFFSETS, s_noHi ? 0 : attr);
      if (attr) {
        s_editMode = 0;
        if (event==EVT_KEY_LONG(KEY_ENTER)) {
          s_noHi = NO_HI_LEN;
          killEvents(event);
          moveTrimsToOffsets(); // if highlighted and menu pressed - move trims to offsets
        }
      }
      return;
    }

    LimitData *ld = limitAddress(k);

    int16_t v = (ld->revert) ? -LIMIT_OFS(ld) : LIMIT_OFS(ld);
    char swVal[2] = "-";  // '-', '<', '>'
    if ((channelOutputs[k] - v) > 50) swVal[0] = (ld->revert ? 127 : 126); // Switch to raw inputs?  - remove trim!
    if ((channelOutputs[k] - v) < -50) swVal[0] = (ld->revert ? 126 : 127);
    lcd_puts(LIMITS_DIRECTION_POS, y, swVal);

    int limit = (g_model.extendedLimits ? LIMIT_EXT_MAX : 1000);

    putsChn(MENU_TITLE_LEFT, y, k+1, (sub==k && m_posHorz < 0) ? INVERS : 0);
    if (sub==k && m_posHorz < 0 && event==EVT_KEY_LONG(KEY_ENTER) && !READ_ONLY()) {
      killEvents(event);
      MENU_ADD_ITEM(STR_RESET);
      MENU_ADD_ITEM(STR_COPY_TRIMS_TO_OFS);
      MENU_ADD_ITEM(STR_COPY_STICKS_TO_OFS);
      menuHandler = onLimitsMenu;
    }

    for (uint8_t j=0; j<ITEM_LIMITS_COUNT; j++) {
      uint8_t attr = ((sub==k && m_posHorz==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      uint8_t active = (attr && s_editMode>0) ;
      if (active) STICK_SCROLL_DISABLE();
      switch(j)
      {
        case ITEM_LIMITS_CH_NAME:
          editName(LIMITS_NAME_POS, y, ld->name, sizeof(ld->name), event, attr);
          break;

        case ITEM_LIMITS_OFFSET:
          if (GV_IS_GV_VALUE(ld->offset, -1000, 1000) || (attr && event == EVT_KEY_LONG(KEY_ENTER))) {
            ld->offset = GVAR_MENU_ITEM(LIMITS_OFFSET_POS, y, ld->offset, -1000, 1000, attr|PREC1, 0, event);
            break;
          }

#if defined(PPM_UNIT_US)
          lcd_outdezAtt(LIMITS_OFFSET_POS, y, ((int32_t)ld->offset*128) / 25, attr|PREC1);
#else
          lcd_outdezAtt(LIMITS_OFFSET_POS, y, ld->offset, attr|PREC1);
#endif
          if (active) {
            ld->offset = checkIncDec(event, ld->offset, -1000, 1000, EE_MODEL, NULL, stops1000);
          }
          else if (attr && event==EVT_KEY_LONG(KEY_MENU)) {
            copySticksToOffset(k);
            s_editMode = 0;
          }
          break;

        case ITEM_LIMITS_MIN:
          if (GV_IS_GV_VALUE(ld->min, -GV_RANGELARGE, GV_RANGELARGE) || (attr && event == EVT_KEY_LONG(KEY_ENTER))) {
            ld->min = GVAR_MENU_ITEM(LIMITS_MIN_POS, y, ld->min, -LIMIT_EXT_MAX, LIMIT_EXT_MAX, attr|PREC1, 0, event);
            break;
          }
          lcd_outdezAtt(LIMITS_MIN_POS, y, MIN_MAX_DISPLAY(ld->min-LIMITS_MIN_MAX_OFFSET), attr|PREC1);
          if (active) ld->min = LIMITS_MIN_MAX_OFFSET + checkIncDec(event, ld->min-LIMITS_MIN_MAX_OFFSET, -limit, 0, EE_MODEL, NULL, stops1000);
          break;

        case ITEM_LIMITS_MAX:
          if (GV_IS_GV_VALUE(ld->max, -GV_RANGELARGE, GV_RANGELARGE) || (attr && event == EVT_KEY_LONG(KEY_ENTER))) {
            ld->max = GVAR_MENU_ITEM(LIMITS_MAX_POS, y, ld->max, -LIMIT_EXT_MAX, LIMIT_EXT_MAX, attr|PREC1, 0, event);
            break;
          }
          lcd_outdezAtt(LIMITS_MAX_POS, y, MIN_MAX_DISPLAY(ld->max+LIMITS_MIN_MAX_OFFSET), attr|PREC1);
          if (active) ld->max = -LIMITS_MIN_MAX_OFFSET + checkIncDec(event, ld->max+LIMITS_MIN_MAX_OFFSET, 0, +limit, EE_MODEL, NULL, stops1000);
          break;

        case ITEM_LIMITS_DIRECTION:
        {
          uint8_t revert = ld->revert;
#if defined(PPM_CENTER_ADJUSTABLE)
          lcd_putsAtt(LIMITS_REVERT_POS, y, revert ? "\177" : "\176", attr);
#else
          lcd_putsiAtt(LIMITS_REVERT_POS, y, STR_MMMINV, revert, attr);
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

#if defined(CURVES)
        case ITEM_LIMITS_CURVE:
          putsCurve(LIMITS_CURVE_POS, y, ld->curve, attr);
          if (attr && event==EVT_KEY_LONG(KEY_ENTER) && ld->curve>0) {
            s_curveChan = (ld->curve<0 ? -ld->curve-1 : ld->curve-1);
            pushMenu(menuModelCurveOne);
          }
          if (active) {
            CHECK_INCDEC_MODELVAR(event, ld->curve, -MAX_CURVES, +MAX_CURVES);
          }
          break;
#endif

#if defined(PPM_CENTER_ADJUSTABLE)
        case ITEM_LIMITS_PPM_CENTER:
          lcd_outdezAtt(LIMITS_PPM_CENTER_POS, y, PPM_CENTER+ld->ppmCenter, attr);
          if (active) {
            CHECK_INCDEC_MODELVAR(event, ld->ppmCenter, -PPM_CENTER_MAX, +PPM_CENTER_MAX);
          }
          break;
#endif

#if defined(PPM_LIMITS_SYMETRICAL)
        case ITEM_LIMITS_SYMETRICAL:
          lcd_putsAtt(LIMITS_SYMETRICAL_POS, y, ld->symetrical ? "=" : "\306", attr);
          if (active) {
            CHECK_INCDEC_MODELVAR_ZERO(event, ld->symetrical, 1);
          }
          break;
#endif
      }
    }
  }
}
