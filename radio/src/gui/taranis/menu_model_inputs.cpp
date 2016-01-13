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

#include "../../opentx.h"

#define EXPO_ONE_2ND_COLUMN (LCD_W-8*FW-90)
#define EXPO_ONE_FM_WIDTH   (9*FW)

int16_t expoFn(int16_t x)
{
  ExpoData * ed = expoAddress(s_currIdx);
  int16_t anas[NUM_INPUTS] = {0};
  applyExpos(anas, e_perout_mode_inactive_flight_mode, ed->srcRaw, x);
  return anas[ed->chn];
}

void drawFunction(FnFuncP fn, uint8_t offset)
{
  lcdDrawVerticalLine(X0-offset, 0/*TODO Y0-WCHART*/, WCHART*2, 0xee);
  lcdDrawHorizontalLine(X0-WCHART-offset, Y0, WCHART*2, 0xee);

  coord_t prev_yv = (coord_t)-1;

  for (int xv=-WCHART; xv<=WCHART; xv++) {
    coord_t yv = (LCD_H-1) - (((uint16_t)RESX + fn(xv * (RESX/WCHART))) / 2 * (LCD_H-1) / RESX);
    if (prev_yv != (coord_t)-1) {
      if (abs((int8_t)yv-prev_yv) <= 1) {
        lcdDrawPoint(X0+xv-offset-1, prev_yv, FORCE);
      }
      else {
        uint8_t tmp = (prev_yv < yv ? 0 : 1);
        lcdDrawSolidVerticalLine(X0+xv-offset-1, yv+tmp, prev_yv-yv);
      }
    }
    prev_yv = yv;
  }
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
  EXPO_FIELD_SCALE,
  EXPO_FIELD_WEIGHT,
  EXPO_FIELD_OFFSET,
  CASE_CURVES(EXPO_FIELD_CURVE)
  CASE_FLIGHT_MODES(EXPO_FIELD_FLIGHT_MODES)
  EXPO_FIELD_SWITCH,
  EXPO_FIELD_SIDE,
  EXPO_FIELD_TRIM,
  EXPO_FIELD_MAX
};

#define CURVE_ROWS                     1

void menuModelExpoOne(uint8_t event)
{
  if (event == EVT_KEY_LONG(KEY_MENU)) {
    pushMenu(menuChannelsView);
    killEvents(event);
  }

  ExpoData * ed = expoAddress(s_currIdx);
  putsMixerSource(PSIZE(TR_MENUINPUTS)*FW+FW, 0, MIXSRC_FIRST_INPUT+ed->chn, 0);
  lcdDrawFilledRect(0, 0, LCD_W, FH, SOLID, FILL_WHITE|GREY_DEFAULT);

  SUBMENU(STR_MENUINPUTS, EXPO_FIELD_MAX, {0, 0, 0, ed->srcRaw >= MIXSRC_FIRST_TELEM ? (uint8_t)0 : (uint8_t)HIDDEN_ROW, 0, 0, CASE_CURVES(CURVE_ROWS) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0 /*, ...*/});

  SET_SCROLLBAR_X(EXPO_ONE_2ND_COLUMN+10*FW);

  int8_t sub = menuVerticalPosition;

  coord_t y = MENU_HEADER_HEIGHT + 1;

  for (unsigned int k=0; k<NUM_BODY_LINES; k++) {
    int i = k + menuVerticalOffset;
    for (int j=0; j<=i; ++j) {
      if (j<(int)DIM(mstate_tab) && mstate_tab[j] == HIDDEN_ROW) {
        ++i;
      }
    }
    LcdFlags attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);

    switch(i) {
      case EXPO_FIELD_INPUT_NAME:
        editSingleName(EXPO_ONE_2ND_COLUMN, y, STR_INPUTNAME, g_model.inputNames[ed->chn], sizeof(g_model.inputNames[ed->chn]), event, attr);
        break;

      case EXPO_FIELD_NAME:
        editSingleName(EXPO_ONE_2ND_COLUMN, y, STR_EXPONAME, ed->name, sizeof(ed->name), event, attr);
        break;

      case EXPO_FIELD_SOURCE:
        lcd_putsLeft(y, NO_INDENT(STR_SOURCE));
        putsMixerSource(EXPO_ONE_2ND_COLUMN, y, ed->srcRaw, STREXPANDED|attr);
        if (attr) ed->srcRaw = checkIncDec(event, ed->srcRaw, INPUTSRC_FIRST, INPUTSRC_LAST, EE_MODEL|INCDEC_SOURCE|NO_INCDEC_MARKS, isInputSourceAvailable);
        break;

      case EXPO_FIELD_SCALE:
        lcd_putsLeft(y, STR_SCALE);
        putsTelemetryChannelValue(EXPO_ONE_2ND_COLUMN, y, (ed->srcRaw - MIXSRC_FIRST_TELEM)/3, convertTelemValue(ed->srcRaw - MIXSRC_FIRST_TELEM + 1, ed->scale), LEFT|attr);
        if (attr) ed->scale = checkIncDec(event, ed->scale, 0, maxTelemValue(ed->srcRaw - MIXSRC_FIRST_TELEM + 1), EE_MODEL);
        break;

      case EXPO_FIELD_WEIGHT:
        lcd_putsLeft(y, STR_WEIGHT);
        ed->weight = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->weight, MIN_EXPO_WEIGHT, 100, LEFT|attr, 0, event);
        break;

      case EXPO_FIELD_OFFSET:
        lcd_putsLeft(y, NO_INDENT(STR_OFFSET));
        ed->offset = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->offset, -100, 100, LEFT|attr, 0, event);
        break;

#if defined(CURVES)
      case EXPO_FIELD_CURVE:
        lcd_putsLeft(y, STR_CURVE);
        editCurveRef(EXPO_ONE_2ND_COLUMN, y, ed->curve, event, attr);
        break;
#endif

#if defined(FLIGHT_MODES)
      case EXPO_FIELD_FLIGHT_MODES:
        ed->flightModes = editFlightModes(EXPO_ONE_2ND_COLUMN, y, event, ed->flightModes, attr);
        break;
#endif

      case EXPO_FIELD_SWITCH:
        ed->swtch = switchMenuItem(EXPO_ONE_2ND_COLUMN, y, ed->swtch, attr, event);
        break;

      case EXPO_FIELD_SIDE:
        ed->mode = 4 - selectMenuItem(EXPO_ONE_2ND_COLUMN, y, STR_SIDE, STR_VSIDE, 4-ed->mode, 1, 3, attr, event);
        break;

      case EXPO_FIELD_TRIM:
        uint8_t not_stick = (ed->srcRaw > MIXSRC_Ail);
        int8_t carryTrim = -ed->carryTrim;
        lcd_putsLeft(y, STR_TRIM);
        lcdDrawTextAtIndex(EXPO_ONE_2ND_COLUMN, y, STR_VMIXTRIMS, (not_stick && carryTrim == 0) ? 0 : carryTrim+1, menuHorizontalPosition==0 ? attr : 0);
        if (attr) ed->carryTrim = -checkIncDecModel(event, carryTrim, not_stick ? TRIM_ON : -TRIM_OFF, -TRIM_AIL);
        break;
    }
    y += FH;
  }

  drawFunction(expoFn);

  int x512 = getValue(ed->srcRaw);
  if (ed->srcRaw >= MIXSRC_FIRST_TELEM) {
    putsTelemetryChannelValue(LCD_W-8, 6*FH, (ed->srcRaw - MIXSRC_FIRST_TELEM) / 3, x512, 0);
    if (ed->scale > 0) x512 = (x512 * 1024) / convertTelemValue(ed->srcRaw - MIXSRC_FIRST_TELEM + 1, ed->scale);
  }
  else {
    lcdDrawNumber(LCD_W-8, 6*FH, calcRESXto1000(x512), PREC1);
  }
  x512 = limit(-1024, x512, 1024);
  int y512 = expoFn(x512);
  y512 = limit(-1024, y512, 1024);
  lcdDrawNumber(LCD_W-8-6*FW, 1*FH, calcRESXto1000(y512), PREC1);

  x512 = X0+x512/(RESX/WCHART);
  y512 = (LCD_H-1) - ((y512+RESX)/2) * (LCD_H-1) / RESX;

  lcdDrawSolidVerticalLine(x512, y512-3, 3*2+1);
  lcdDrawSolidHorizontalLine(x512-3, y512, 3*2+1);
}

#define _STR_MAX(x)                     PSTR("/" #x)
#define STR_MAX(x)                     _STR_MAX(x)

#define EXPO_LINE_WEIGHT_POS           8*FW+8
#define EXPO_LINE_SRC_POS              9*FW+3
#define EXPO_LINE_CURVE_POS            12*FW+11
#define EXPO_LINE_TRIM_POS             19*FW-4
#define EXPO_LINE_SWITCH_POS           20*FW-1
#define EXPO_LINE_SIDE_POS             25*FW
#define EXPO_LINE_FM_POS               12*FW+11
#define EXPO_LINE_SELECT_POS           5*FW+2
#define EXPO_LINE_NAME_POS             LCD_W-LEN_EXPOMIX_NAME*FW-MENUS_SCROLLBAR_WIDTH

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
  putsCurveRef(EXPO_LINE_CURVE_POS, y, ed->curve, 0);
  putsSwitches(EXPO_LINE_SWITCH_POS, y, ed->swtch, 0);
}

void displayExpoLine(coord_t y, ExpoData *ed)
{
  putsMixerSource(EXPO_LINE_SRC_POS, y, ed->srcRaw, 0);

  if (ed->carryTrim != TRIM_ON) {
    lcdDrawChar(EXPO_LINE_TRIM_POS, y, ed->carryTrim > 0 ? '-' : STR_RETA123[-ed->carryTrim]);
  }

  if (!ed->flightModes || ((ed->curve.value || ed->swtch) && ((get_tmr10ms() / 200) & 1)))
    displayExpoInfos(y, ed);
  else
    displayFlightModes(EXPO_LINE_FM_POS, y, ed->flightModes);

  if (ed->name[0]) {
    lcdDrawSizedText(EXPO_LINE_NAME_POS, y, ed->name, sizeof(ed->name), ZCHAR);
  }
}

void menuModelExposAll(uint8_t event)
{
  uint8_t sub = menuVerticalPosition;

  if (s_editMode > 0) {
    s_editMode = 0;
  }

  uint8_t chn = expoAddress(s_currIdx)->chn + 1;

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
        uint8_t next_ofs = (key==KEY_UP ? s_copyTgtOfs - 1 : s_copyTgtOfs + 1);

        if (s_copyTgtOfs==0 && s_copyMode==COPY_MODE) {
          // insert a mix on the same channel (just above / just below)
          if (reachExposLimit()) break;
          copyExpo(s_currIdx);
          if (key==KEY_DOWN) s_currIdx++;
          else if (sub-menuVerticalOffset >= 6) menuVerticalOffset++;
        }
        else if (next_ofs==0 && s_copyMode==COPY_MODE) {
          // delete the mix
          deleteExpo(s_currIdx);
          if (key==KEY_UP) s_currIdx--;
        }
        else {
          // only swap the mix with its neighbor
          if (!swapExpos(s_currIdx, key==KEY_UP)) break;
          storageDirty(EE_MODEL);
        }

        s_copyTgtOfs = next_ofs;
      }
      break;
  }

  lcdDrawNumber(FW*sizeof(TR_MENUINPUTS)+FW+FW/2, 0, getExposCount());
  lcdDrawText(FW*sizeof(TR_MENUINPUTS)+FW+FW/2, 0, STR_MAX(MAX_EXPOS));

  // Value
  uint8_t index = expoAddress(s_currIdx)->chn;
  if (!s_currCh) {
    lcdDrawNumber(127, 2, calcRESXto1000(anas[index]), PREC1|TINSIZE);
  }

  SIMPLE_MENU(STR_MENUINPUTS, menuTabModel, e_InputsAll, s_maxLines);

  // Gauge
  if (!s_currCh) {
    drawGauge(127, 1, 58, 6, anas[index], 1024);
  }

  sub = menuVerticalPosition;
  s_currCh = 0;
  int cur = 0;
  int i = 0;

  for (int ch=1; ch<=NUM_INPUTS; ch++) {
    ExpoData * ed;
    coord_t y = MENU_HEADER_HEIGHT+1+(cur-menuVerticalOffset)*FH;
    if (i<MAX_EXPOS && (ed=expoAddress(i))->chn+1 == ch && EXPO_VALID(ed)) {
      if (cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES) {
        putsMixerSource(0, y, ch, 0);
      }
      uint8_t mixCnt = 0;
      do {
        if (s_copyMode) {
          if (s_copyMode == MOVE_MODE && cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES && s_copySrcCh == ch && s_copyTgtOfs != 0 && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
            lcdDrawRect(18, y-1, LCD_W-18, 9, DOTTED);
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
          uint8_t attr = ((s_copyMode || sub != cur) ? 0 : INVERS);

          GVAR_MENU_ITEM(EXPO_LINE_WEIGHT_POS, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr | (isExpoActive(i) ? BOLD : 0), 0, 0);
          displayExpoLine(y, ed);
          if (ed->mode!=3) {
            lcdDrawChar(EXPO_LINE_SIDE_POS, y, ed->mode == 2 ? 126 : 127);
          }

          if (s_copyMode) {
            if ((s_copyMode==COPY_MODE || s_copyTgtOfs == 0) && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
              /* draw a border around the raw on selection mode (copy/move) */
              lcdDrawRect(EXPO_LINE_SELECT_POS, y-1, LCD_W-EXPO_LINE_SELECT_POS, 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
            }
            if (cur == sub) {
              /* invert the raw when it's the current one */
              lcdDrawFilledRect(EXPO_LINE_SELECT_POS+1, y, LCD_W-EXPO_LINE_SELECT_POS-2, 7);
            }
          }
        }
        cur++; y+=FH; mixCnt++; i++; ed++;
      } while (i<MAX_EXPOS && ed->chn+1 == ch && EXPO_VALID(ed));
      if (s_copyMode == MOVE_MODE && cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
        lcdDrawRect(EXPO_LINE_SELECT_POS, y-1, LCD_W-EXPO_LINE_SELECT_POS, 9, DOTTED);
        cur++;
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
        putsMixerSource(0, y, ch, attr);
        if (s_copyMode == MOVE_MODE && s_copySrcCh == ch) {
          lcdDrawRect(EXPO_LINE_SELECT_POS, y-1, LCD_W-EXPO_LINE_SELECT_POS, 9, DOTTED);
        }
      }
      cur++;
    }
  }
  s_maxLines = cur;
  if (sub >= s_maxLines-1) menuVerticalPosition = s_maxLines-1;
}
