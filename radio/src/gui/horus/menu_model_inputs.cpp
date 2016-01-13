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
#include "../../opentx.h"

#define EXPO_ONE_2ND_COLUMN 110

int expoFn(int x)
{
  ExpoData * ed = expoAddress(s_currIdx);
  int16_t anas[NUM_INPUTS] = {0};
  applyExpos(anas, e_perout_mode_inactive_flight_mode, ed->srcRaw, x);
  return anas[ed->chn];
}

int getExposLinesCount()
{
  int lastch = -1;
  uint8_t count = MAX_INPUTS;
  for (int i=0; i<MAX_EXPOS; i++) {
    bool valid = EXPO_VALID(expoAddress(i));
    if (!valid)
      break;
    int ch = expoAddress(i)->chn;
    if (ch == lastch) {
      count++;
    }
    else {
      lastch = ch;
    }
  }
  return count;
}

uint8_t getExposCount()
{
  uint8_t count = 0;
  uint8_t ch ;

  for (int i=MAX_EXPOS-1 ; i>=0; i--) {
    ch = EXPO_VALID(expoAddress(i));
    if (ch != 0) {
      count++;
    }
  }
  return count;
}

bool reachExposLimit()
{
  if (getExposCount() >= MAX_EXPOS) {
    POPUP_WARNING(STR_NOFREEEXPO);
    return true;
  }
  return false;
}

void deleteExpo(uint8_t idx)
{
  pauseMixerCalculations();
  ExpoData * expo = expoAddress(idx);
  int input = expo->chn;
  memmove(expo, expo+1, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
  memclear(&g_model.expoData[MAX_EXPOS-1], sizeof(ExpoData));
  if (!isInputAvailable(input)) {
    memclear(&g_model.inputNames[input], LEN_INPUT_NAME);
  }
  resumeMixerCalculations();
  storageDirty(EE_MODEL);
}

// TODO avoid this global s_currCh on ARM boards ...
int8_t s_currCh;
void insertExpo(uint8_t idx)
{
  pauseMixerCalculations();
  ExpoData * expo = expoAddress(idx);
  memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
  memclear(expo, sizeof(ExpoData));
  expo->srcRaw = (s_currCh > 4 ? MIXSRC_Rud - 1 + s_currCh : MIXSRC_Rud - 1 + channel_order(s_currCh));
  expo->curve.type = CURVE_REF_EXPO;
  expo->mode = 3; // pos+neg
  expo->chn = s_currCh - 1;
  expo->weight = 100;
  resumeMixerCalculations();
  storageDirty(EE_MODEL);
}

void copyExpo(uint8_t idx)
{
  pauseMixerCalculations();
  ExpoData * expo = expoAddress(idx);
  memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
  resumeMixerCalculations();
  storageDirty(EE_MODEL);
}

bool swapExpos(uint8_t & idx, uint8_t up)
{
  ExpoData * x, * y;
  int8_t tgt_idx = (up ? idx-1 : idx+1);

  x = expoAddress(idx);

  if (tgt_idx < 0) {
    if (x->chn == 0)
      return false;
    x->chn--;
    return true;
  }

  if (tgt_idx == MAX_EXPOS) {
    if (x->chn == NUM_INPUTS-1)
      return false;
    x->chn++;
    return true;
  }

  y = expoAddress(tgt_idx);
  if (x->chn != y->chn || !EXPO_VALID(y)) {
    if (up) {
      if (x->chn>0) x->chn--;
      else return false;
    }
    else {
      if (x->chn<NUM_INPUTS-1) x->chn++;
      else return false;
    }
    return true;
  }

  pauseMixerCalculations();
  memswap(x, y, sizeof(ExpoData));
  resumeMixerCalculations();

  idx = tgt_idx;
  return true;
}

enum ExposFields {
  EXPO_FIELD_INPUT_NAME,
  EXPO_FIELD_NAME,
  EXPO_FIELD_SOURCE,
  EXPO_FIELD_WEIGHT,
  EXPO_FIELD_OFFSET,
  CASE_CURVES(EXPO_FIELD_CURVE)
  CASE_FLIGHT_MODES(EXPO_FIELD_FLIGHT_MODES)
  EXPO_FIELD_SWITCH,
  EXPO_FIELD_SIDE,
  EXPO_FIELD_TRIM,
  EXPO_FIELD_MAX
};

#define CURVE_ROWS 1

bool menuModelExpoOne(evt_t event)
{
  ExpoData * ed = expoAddress(s_currIdx);

  SUBMENU(STR_MENUINPUTS, EXPO_FIELD_MAX, 0, { 0, 0, (ed->srcRaw >= MIXSRC_FIRST_TELEM ? (uint8_t)1 : (uint8_t)0), 0, 0, CASE_CURVES(CURVE_ROWS) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0 /*, ...*/});

  int sub = menuVerticalPosition;

  coord_t y = MENU_CONTENT_TOP;

  drawFunction(expoFn, CURVE_CENTER_X, CURVE_CENTER_Y, CURVE_SIDE_WIDTH);
  drawCurveHorizontalScale();
  drawCurveVerticalScale(CURVE_CENTER_X-CURVE_SIDE_WIDTH-15);

  {
    char textx[5];
    char texty[5];
    int x = getValue(ed->srcRaw);
    if (ed->srcRaw >= MIXSRC_FIRST_TELEM) {
      sprintf(textx, "%d", calcRESXto100(x));
      // TODO putsTelemetryChannelValue(LCD_W-8, 6*FH, ed->srcRaw - MIXSRC_FIRST_TELEM, x);
      if (ed->scale > 0) x = (x * 1024) / convertTelemValue(ed->srcRaw - MIXSRC_FIRST_TELEM + 1, ed->scale);
    }
    else {
      sprintf(textx, "%d", calcRESXto100(x));
    }

    x = limit(-1024, x, 1024);
    int y = limit<int>(-1024, expoFn(x), 1024);
    sprintf(texty, "%d", calcRESXto100(y));

    x = divRoundClosest(x*CURVE_SIDE_WIDTH, RESX);
    y = CURVE_CENTER_Y + getCurveYCoord(expoFn, x, CURVE_SIDE_WIDTH);

    lcdDrawSolidFilledRect(CURVE_CENTER_X+x, CURVE_CENTER_Y-CURVE_SIDE_WIDTH, 2, 2*CURVE_SIDE_WIDTH+2, CURVE_CURSOR_COLOR);
    lcdDrawSolidFilledRect(CURVE_CENTER_X-CURVE_SIDE_WIDTH-2, y-1, 2*CURVE_SIDE_WIDTH+2, 2, CURVE_CURSOR_COLOR);
    lcdDrawBitmapPattern(CURVE_CENTER_X+x-4, y-4, LBM_CURVE_POINT, CURVE_CURSOR_COLOR);
    lcdDrawBitmapPattern(CURVE_CENTER_X+x-4, y-4, LBM_CURVE_POINT_CENTER, TEXT_BGCOLOR);

    int left = limit(CURVE_CENTER_X-CURVE_SIDE_WIDTH, CURVE_CENTER_X-CURVE_COORD_WIDTH/2+x, CURVE_CENTER_X+CURVE_SIDE_WIDTH-CURVE_COORD_WIDTH+2);
    drawCurveCoord(left, CURVE_CENTER_Y+CURVE_SIDE_WIDTH+2, textx);
    int top = limit(CURVE_CENTER_Y-CURVE_SIDE_WIDTH-1, -CURVE_COORD_HEIGHT/2+y, CURVE_CENTER_Y+CURVE_SIDE_WIDTH-CURVE_COORD_HEIGHT+1);
    drawCurveCoord(CURVE_CENTER_X-CURVE_SIDE_WIDTH-37, top, texty);
  }

  for (int i=0; i<NUM_BODY_LINES+1; i++) {
    LcdFlags attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch (i) {
      case EXPO_FIELD_INPUT_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_INPUTNAME);
        editName(EXPO_ONE_2ND_COLUMN, y, g_model.inputNames[ed->chn], sizeof(g_model.inputNames[ed->chn]), event, attr);
        break;

      case EXPO_FIELD_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_EXPONAME);
        editName(EXPO_ONE_2ND_COLUMN, y, ed->name, sizeof(ed->name), event, attr);
        break;

      case EXPO_FIELD_SOURCE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, NO_INDENT(STR_SOURCE));
        putsMixerSource(EXPO_ONE_2ND_COLUMN, y, ed->srcRaw, STREXPANDED|(menuHorizontalPosition==0?attr:0));
        if (attr && menuHorizontalPosition==0) ed->srcRaw = checkIncDec(event, ed->srcRaw, INPUTSRC_FIRST, INPUTSRC_LAST, EE_MODEL|INCDEC_SOURCE|NO_INCDEC_MARKS, isInputSourceAvailable);
        if (ed->srcRaw >= MIXSRC_FIRST_TELEM) {
          putsTelemetryChannelValue(EXPO_ONE_2ND_COLUMN+60, y, (ed->srcRaw - MIXSRC_FIRST_TELEM)/3, convertTelemValue(ed->srcRaw - MIXSRC_FIRST_TELEM + 1, ed->scale), LEFT|(menuHorizontalPosition==1?attr:0));
          if (attr && menuHorizontalPosition == 1) ed->scale = checkIncDec(event, ed->scale, 0, maxTelemValue(ed->srcRaw - MIXSRC_FIRST_TELEM + 1), EE_MODEL);
        }
        else if (attr) {
          menuHorizontalPosition = 0;
        }
        break;

      case EXPO_FIELD_WEIGHT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_WEIGHT);
        ed->weight = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->weight, MIN_EXPO_WEIGHT, 100, LEFT|attr, 0, event);
        break;

      case EXPO_FIELD_OFFSET:
        lcdDrawText(MENUS_MARGIN_LEFT, y, NO_INDENT(STR_OFFSET));
        ed->offset = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->offset, -100, 100, LEFT|attr, 0, event);
        break;

#if defined(CURVES)
      case EXPO_FIELD_CURVE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CURVE);
        editCurveRef(EXPO_ONE_2ND_COLUMN, y, ed->curve, event, attr);
        break;
#endif

#if defined(FLIGHT_MODES)
      case EXPO_FIELD_FLIGHT_MODES:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_FLMODE);
        ed->flightModes = editFlightModes(EXPO_ONE_2ND_COLUMN, y, event, ed->flightModes, attr);
        break;
#endif

      case EXPO_FIELD_SWITCH:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SWITCH);
        ed->swtch = switchMenuItem(EXPO_ONE_2ND_COLUMN, y, ed->swtch, attr, event);
        break;

      case EXPO_FIELD_SIDE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SIDE);
        ed->mode = 4 - selectMenuItem(EXPO_ONE_2ND_COLUMN, y, STR_VSIDE, 4-ed->mode, 1, 3, attr, event);
        break;

      case EXPO_FIELD_TRIM:
        uint8_t not_stick = (ed->srcRaw > MIXSRC_Ail);
        int8_t carryTrim = -ed->carryTrim;
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TRIM);
        lcdDrawTextAtIndex(EXPO_ONE_2ND_COLUMN, y, STR_VMIXTRIMS, (not_stick && carryTrim == 0) ? 0 : carryTrim+1, menuHorizontalPosition==0 ? attr : 0);
        if (attr) ed->carryTrim = -checkIncDecModel(event, carryTrim, not_stick ? TRIM_ON : -TRIM_OFF, -TRIM_AIL);
        break;
    }
    y += FH;
  }

  return true;
}

#define _STR_MAX(x) PSTR("/" #x)
#define STR_MAX(x) _STR_MAX(x)

#define EXPO_LINE_WEIGHT_POS    92
#define EXPO_LINE_SRC_POS       115
#define EXPO_LINE_CURVE_POS     162
#define EXPO_LINE_SWITCH_POS    210
#define EXPO_LINE_SIDE_POS      250
#define EXPO_LINE_FM_POS        270
#define EXPO_LINE_NAME_POS      384
#define EXPO_LINE_SELECT_POS    50
#define EXPO_LINE_SELECT_WIDTH  (LCD_W-EXPO_LINE_SELECT_POS-15)

void lineExpoSurround(coord_t y, LcdFlags flags=CURVE_AXIS_COLOR)
{
  lcdDrawRect(EXPO_LINE_SELECT_POS, y-INVERT_VERT_MARGIN+1, EXPO_LINE_SELECT_WIDTH, INVERT_LINE_HEIGHT, s_copyMode == COPY_MODE ? SOLID : DOTTED, flags);
}

void onExposMenu(const char * result)
{
  uint8_t chn = expoAddress(s_currIdx)->chn + 1;

  if (result == STR_EDIT) {
    pushMenu(menuModelExpoOne);
  }
  else if (result == STR_INSERT_BEFORE || result == STR_INSERT_AFTER) {
    if (!reachExposLimit()) {
      s_currCh = chn;
      if (result == STR_INSERT_AFTER) { s_currIdx++; menuVerticalPosition++; }
      insertExpo(s_currIdx);
      pushMenu(menuModelExpoOne);
    }
  }
  else if (result == STR_COPY || result == STR_MOVE) {
    s_copyMode = (result == STR_COPY ? COPY_MODE : MOVE_MODE);
    s_copySrcIdx = s_currIdx;
    s_copySrcCh = chn;
    s_copySrcRow = menuVerticalPosition;
  }
  else if (result == STR_DELETE) {
    deleteExpo(s_currIdx);
  }
}

void displayExpoInfos(coord_t y, ExpoData *ed)
{
  putsCurveRef(EXPO_LINE_CURVE_POS, y, ed->curve);
  if (ed->swtch) {
    putsSwitches(EXPO_LINE_SWITCH_POS, y, ed->swtch);
  }
  if (ed->mode != 3) {
    lcdDrawText(EXPO_LINE_SIDE_POS, y, ed->mode == 2 ? "\176" : "\177");
  }
}

void displayExpoLine(coord_t y, ExpoData *ed)
{
  putsMixerSource(EXPO_LINE_SRC_POS, y, ed->srcRaw);

  displayExpoInfos(y, ed);
  displayFlightModes(EXPO_LINE_FM_POS, y, ed->flightModes, 0);

  if (ed->name[0]) {
    lcdDrawSizedText(EXPO_LINE_NAME_POS, y+2, ed->name, sizeof(ed->name), ZCHAR|SMLSIZE);
  }
}

bool menuModelExposAll(evt_t event)
{
  int sub = menuVerticalPosition;

  if (s_editMode > 0) {
    s_editMode = 0;
  }

  uint8_t chn = expoAddress(s_currIdx)->chn + 1;

  int linesCount = getExposLinesCount();
  SIMPLE_MENU(STR_MENUINPUTS, menuTabModel, e_InputsAll, linesCount, DEFAULT_SCROLLBAR_X);

  switch (event) {
    case EVT_ENTRY:
    case EVT_ENTRY_UP:
      s_copyMode = 0;
      s_copyTgtOfs = 0;
      break;
    case EVT_KEY_LONG(KEY_EXIT):
      if (s_copyMode && s_copyTgtOfs == 0) {
        deleteExpo(s_currIdx);
        killEvents(event);
        event = 0;
      }
      // no break
    case EVT_KEY_BREAK(KEY_EXIT):
      if (s_copyMode) {
        if (s_copyTgtOfs) {
          // cancel the current copy / move operation
          if (s_copyMode == COPY_MODE) {
            deleteExpo(s_currIdx);
          }
          else {
            do {
              swapExpos(s_currIdx, s_copyTgtOfs > 0);
              s_copyTgtOfs += (s_copyTgtOfs < 0 ? +1 : -1);
            } while (s_copyTgtOfs != 0);
            storageDirty(EE_MODEL);
          }
          menuVerticalPosition = s_copySrcRow;
          s_copyTgtOfs = 0;
        }
        s_copyMode = 0;
        event = 0;
      }
      break;
    case EVT_KEY_BREAK(KEY_ENTER):
      if ((!s_currCh || (s_copyMode && !s_copyTgtOfs)) && !READ_ONLY()) {
        s_copyMode = (s_copyMode == COPY_MODE ? MOVE_MODE : COPY_MODE);
        s_copySrcIdx = s_currIdx;
        s_copySrcCh = chn;
        s_copySrcRow = sub;
        break;
      }
      // no break

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(event);
      if (s_copyTgtOfs) {
        s_copyMode = 0;
        s_copyTgtOfs = 0;
      }
      else {
        if (READ_ONLY()) {
          if (!s_currCh) {
            pushMenu(menuModelExpoOne);
          }
        }
        else {
          if (s_copyMode) s_currCh = 0;
          if (s_currCh) {
            if (reachExposLimit()) break;
            insertExpo(s_currIdx);
            pushMenu(menuModelExpoOne);
            s_copyMode = 0;
          }
          else {
            event = 0;
            s_copyMode = 0;
            POPUP_MENU_ADD_ITEM(STR_EDIT);
            POPUP_MENU_ADD_ITEM(STR_INSERT_BEFORE);
            POPUP_MENU_ADD_ITEM(STR_INSERT_AFTER);
            POPUP_MENU_ADD_ITEM(STR_COPY);
            POPUP_MENU_ADD_ITEM(STR_MOVE);
            POPUP_MENU_ADD_ITEM(STR_DELETE);
            popupMenuHandler = onExposMenu;
          }
        }
      }
      break;
    case EVT_KEY_LONG(KEY_LEFT):
    case EVT_KEY_LONG(KEY_RIGHT):
      if (s_copyMode && !s_copyTgtOfs) {
        if (reachExposLimit()) break;
        s_currCh = chn;
        if (event == EVT_KEY_LONG(KEY_RIGHT)) { s_currIdx++; menuVerticalPosition++; }
        insertExpo(s_currIdx);
        pushMenu(menuModelExpoOne);
        s_copyMode = 0;
        killEvents(event);
      }
      break;
    case EVT_KEY_FIRST(KEY_UP):
    case EVT_KEY_REPT(KEY_UP):
    case EVT_KEY_FIRST(KEY_DOWN):
    case EVT_KEY_REPT(KEY_DOWN):
      if (s_copyMode) {
        uint8_t key = (event & 0x1f);
        uint8_t next_ofs = ((event==EVT_ROTARY_LEFT || key==KEY_UP) ? s_copyTgtOfs - 1 : s_copyTgtOfs + 1);

        if (s_copyTgtOfs==0 && s_copyMode==COPY_MODE) {
          // insert a mix on the same channel (just above / just below)
          if (reachExposLimit()) break;
          copyExpo(s_currIdx);
          if (event==EVT_ROTARY_RIGHT || key==KEY_DOWN) s_currIdx++;
          else if (sub-menuVerticalOffset >= 6) menuVerticalOffset++;
        }
        else if (next_ofs==0 && s_copyMode==COPY_MODE) {
          // delete the mix
          deleteExpo(s_currIdx);
          if (event==EVT_ROTARY_LEFT || key==KEY_UP) s_currIdx--;
        }
        else {
          // only swap the mix with its neighbor
          if (!swapExpos(s_currIdx, event==EVT_ROTARY_LEFT || key==KEY_UP)) break;
          storageDirty(EE_MODEL);
        }

        s_copyTgtOfs = next_ofs;
      }
      break;
  }

  char str[6];
  sprintf(str, "%d/%d", getExposCount(), MAX_EXPOS);
  lcdDrawText(MENU_TITLE_NEXT_POS, MENU_TITLE_TOP+2, str, HEADER_COLOR);

  sub = menuVerticalPosition;
  s_currCh = 0;
  int cur = 0;
  int i = 0;

  for (int ch=1; ch<=NUM_INPUTS; ch++) {
    ExpoData * ed;
    coord_t y = MENU_CONTENT_TOP + (cur-menuVerticalOffset)*FH;
    if ((i<MAX_EXPOS && (ed=expoAddress(i))->chn+1 == ch && EXPO_VALID(ed))) {
      if (cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES) {
        putsMixerSource(MENUS_MARGIN_LEFT, y, ch);
      }
      uint8_t mixCnt = 0;
      do {
        if (s_copyMode) {
          if (s_copyMode == MOVE_MODE && cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES && s_copySrcCh == ch && s_copyTgtOfs != 0 && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
            lineExpoSurround(y);
            cur++; y+=FH;
          }
          if (s_currIdx == i) {
            sub = menuVerticalPosition = cur;
            s_currCh = ch;
          }
        }
        else if (sub == cur) {
          s_currIdx = i;
        }
        if (cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES) {
          LcdFlags attr = ((s_copyMode || sub != cur) ? 0 : INVERS);

          GVAR_MENU_ITEM(EXPO_LINE_WEIGHT_POS, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr | (isExpoActive(i) ? BOLD : 0), 0, 0);
          displayExpoLine(y, ed);

          if (s_copyMode) {
            if ((s_copyMode==COPY_MODE || s_copyTgtOfs == 0) && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
              /* draw a border around the raw on selection mode (copy/move) */
              lineExpoSurround(y);
            }
            if (cur == sub) {
              /* invert the raw when it's the current one */
              lineExpoSurround(y, ALARM_COLOR);
            }
          }
        }
        cur++; y+=FH; mixCnt++; i++; ed++;
      } while (i<MAX_EXPOS && ed->chn+1 == ch && EXPO_VALID(ed));
      if (s_copyMode == MOVE_MODE && cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
        lineExpoSurround(y);
        cur++; y+=FH;
      }
    }
    else {
      uint8_t attr = 0;
      if (sub == cur) {
        s_currIdx = i;
        s_currCh = ch;
        if (!s_copyMode) {
          attr = INVERS;
        }
      }
      if (cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES) {
        putsMixerSource(MENUS_MARGIN_LEFT, y, ch, attr);
        if (s_copyMode == MOVE_MODE && s_copySrcCh == ch) {
          lineExpoSurround(y);
        }
      }
      cur++; y+=FH;
    }
  }

  if (sub >= linesCount-1) menuVerticalPosition = linesCount-1;

  return true;
}

