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

#if defined(FLIGHT_MODES)
void displayFlightModes(coord_t x, coord_t y, FlightModesType value, uint8_t attr)
{
  for (int i=0; i<MAX_FLIGHT_MODES; i++) {
    LcdFlags flags = ((menuHorizontalPosition==i && attr) ? INVERS : 0);
    flags |= ((value & (1<<i))) ? TEXT_DISABLE_COLOR : TEXT_COLOR;
    if (attr && menuHorizontalPosition < 0) flags |= INVERS;
    char s[] = " ";
    s[0] = '0' + i;
    lcdDrawText(x, y, s, flags);
    x += 12;
  }
}

FlightModesType editFlightModes(coord_t x, coord_t y, evt_t event, FlightModesType value, uint8_t attr)
{
  int posHorz = menuHorizontalPosition;

  displayFlightModes(x, y, value, attr);

  if (attr) {
    if (s_editMode && event==EVT_KEY_BREAK(KEY_ENTER)) {
      s_editMode = 0;
      value ^= (1<<posHorz);
      storageDirty(EE_MODEL);
    }
  }

  return value;
}
#else
  #define displayFlightModes(...)
#endif

int expoFn(int x)
{
  ExpoData *ed = expoAddress(s_currIdx);
  int16_t anas[NUM_INPUTS] = {0};
  applyExpos(anas, e_perout_mode_inactive_flight_mode, ed->srcRaw, x);
  return anas[ed->chn];
}

int getLinesCount(uint8_t expo)
{
  int lastch = -1;
  uint8_t count = (expo ? MAX_INPUTS : NUM_CHNOUT);
  for (int i=0; i<(expo ? MAX_EXPOS : MAX_MIXERS); i++) {
    bool valid = (expo ? EXPO_VALID(expoAddress(i)) : mixAddress(i)->srcRaw);
    if (!valid)
      break;
    int ch = (expo ? expoAddress(i)->chn : mixAddress(i)->destCh);
    if (ch == lastch) {
      count++;
    }
    else {
      lastch = ch;
    }
  }
  return count;
}

uint8_t getExpoMixCount(uint8_t expo)
{
  uint8_t count = 0;
  uint8_t ch ;

  for(int8_t i=(expo ? MAX_EXPOS-1 : MAX_MIXERS-1); i>=0; i--) {
    ch = (expo ? EXPO_VALID(expoAddress(i)) : mixAddress(i)->srcRaw);
    if (ch != 0) {
      count++;
    }
  }
  return count;
}

bool reachExpoMixCountLimit(uint8_t expo)
{
  // check mixers count limit
  if (getExpoMixCount(expo) >= (expo ? MAX_EXPOS : MAX_MIXERS)) {
    POPUP_WARNING(expo ? STR_NOFREEEXPO : STR_NOFREEMIXER);
    return true;
  }
  return false;
}

void deleteExpoMix(uint8_t expo, uint8_t idx)
{
  pauseMixerCalculations();
  if (expo) {
    ExpoData *expo = expoAddress(idx);
    int input = expo->chn;
    memmove(expo, expo+1, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
    memclear(&g_model.expoData[MAX_EXPOS-1], sizeof(ExpoData));
    if (!isInputAvailable(input)) {
      memclear(&g_model.inputNames[input], LEN_INPUT_NAME);
    }
  }
  else {
    MixData *mix = mixAddress(idx);
    memmove(mix, mix+1, (MAX_MIXERS-(idx+1))*sizeof(MixData));
    memclear(&g_model.mixData[MAX_MIXERS-1], sizeof(MixData));
  }
  resumeMixerCalculations();
  storageDirty(EE_MODEL);
}

// TODO avoid this global s_currCh on ARM boards ...
int8_t s_currCh;
void insertExpoMix(uint8_t expo, uint8_t idx)
{
  pauseMixerCalculations();
  if (expo) {
    ExpoData *expo = expoAddress(idx);
    memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
    memclear(expo, sizeof(ExpoData));
    expo->srcRaw = (s_currCh > 4 ? MIXSRC_Rud - 1 + s_currCh : MIXSRC_Rud - 1 + channel_order(s_currCh));
    expo->curve.type = CURVE_REF_EXPO;
    expo->mode = 3; // pos&neg
    expo->chn = s_currCh - 1;
    expo->weight = 100;
  }
  else {
    MixData *mix = mixAddress(idx);
    memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
    memclear(mix, sizeof(MixData));
    mix->destCh = s_currCh-1;
    mix->srcRaw = s_currCh;
    if (!isSourceAvailable(mix->srcRaw)) {
      mix->srcRaw = (s_currCh > 4 ? MIXSRC_Rud - 1 + s_currCh : MIXSRC_Rud - 1 + channel_order(s_currCh));
      while (!isSourceAvailable(mix->srcRaw)) {
        mix->srcRaw += 1;
      }
    }
    mix->weight = 100;
  }
  resumeMixerCalculations();
  storageDirty(EE_MODEL);
}

void copyExpoMix(uint8_t expo, uint8_t idx)
{
  pauseMixerCalculations();
  if (expo) {
    ExpoData *expo = expoAddress(idx);
    memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
  }
  else {
    MixData *mix = mixAddress(idx);
    memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
  }
  resumeMixerCalculations();
  storageDirty(EE_MODEL);
}

void memswap(void *a, void *b, uint8_t size)
{
  uint8_t *x = (uint8_t*)a;
  uint8_t *y = (uint8_t*)b;
  uint8_t temp ;

  while (size--) {
    temp = *x;
    *x++ = *y;
    *y++ = temp;
  }
}

bool swapExpoMix(uint8_t expo, uint8_t &idx, uint8_t up)
{
  void *x, *y;
  uint8_t size;
  int8_t tgt_idx = (up ? idx-1 : idx+1);

  if (expo) {
    x = (ExpoData *)expoAddress(idx);

    if (tgt_idx < 0) {
      if (((ExpoData *)x)->chn == 0)
        return false;
      ((ExpoData *)x)->chn--;
      return true;
    }

    if (tgt_idx == MAX_EXPOS) {
      if (((ExpoData *)x)->chn == NUM_INPUTS-1)
        return false;
      ((ExpoData *)x)->chn++;
      return true;
    }

    y = (ExpoData *)expoAddress(tgt_idx);
    if(((ExpoData *)x)->chn != ((ExpoData *)y)->chn || !EXPO_VALID((ExpoData *)y)) {
      if (up) {
        if (((ExpoData *)x)->chn>0) ((ExpoData *)x)->chn--;
        else return false;
      }
      else {
        if (((ExpoData *)x)->chn<NUM_INPUTS-1) ((ExpoData *)x)->chn++;
        else return false;
      }
      return true;
    }

    size = sizeof(ExpoData);
  }
  else {
    x = (MixData *)mixAddress(idx);

    if (tgt_idx < 0) {
      if (((MixData *)x)->destCh == 0)
        return false;
      ((MixData *)x)->destCh--;
      return true;
    }

    if (tgt_idx == MAX_MIXERS) {
      if (((MixData *)x)->destCh == NUM_CHNOUT-1)
        return false;
      ((MixData *)x)->destCh++;
      return true;
    }

    y = (MixData *)mixAddress(tgt_idx);
    uint8_t destCh = ((MixData *)x)->destCh;
    if(!((MixData *)y)->srcRaw || destCh != ((MixData *)y)->destCh) {
      if (up) {
        if (destCh>0) ((MixData *)x)->destCh--;
        else return false;
      }
      else {
        if (destCh<NUM_CHNOUT-1) ((MixData *)x)->destCh++;
        else return false;
      }
      return true;
    }

    size = sizeof(MixData);
  }

  pauseMixerCalculations();
  memswap(x, y, size);
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
  ExpoData *ed = expoAddress(s_currIdx);

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

enum MixFields {
  MIX_FIELD_NAME,
  MIX_FIELD_SOURCE,
  MIX_FIELD_WEIGHT,
  MIX_FIELD_OFFSET,
  MIX_FIELD_TRIM,
  CASE_CURVES(MIX_FIELD_CURVE)
  CASE_FLIGHT_MODES(MIX_FIELD_FLIGHT_PHASE)
  MIX_FIELD_SWITCH,
  // MIX_FIELD_WARNING,
  MIX_FIELD_MLTPX,
  MIX_FIELD_DELAY_UP,
  MIX_FIELD_DELAY_DOWN,
  MIX_FIELD_SLOW_UP,
  MIX_FIELD_SLOW_DOWN,
  MIX_FIELD_COUNT
};

void gvarWeightItem(coord_t x, coord_t y, MixData *md, uint8_t attr, evt_t event)
{
  u_int8int16_t weight;
  MD_WEIGHT_TO_UNION(md, weight);
  weight.word = GVAR_MENU_ITEM(x, y, weight.word, GV_RANGELARGE_WEIGHT_NEG, GV_RANGELARGE_WEIGHT, attr, 0, event);
  MD_UNION_TO_WEIGHT(weight, md);
}

#if 0
#define GAUGE_WIDTH  33
#define GAUGE_HEIGHT 6
void drawOffsetBar(uint8_t x, uint8_t y, MixData * md)
{
  int offset = GET_GVAR(MD_OFFSET(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
  int weight = abs(GET_GVAR(MD_WEIGHT(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode));
  int barMin = offset - weight;
  int barMax = offset + weight;
  if (y > 15) {
    lcdDrawNumber(x-((barMin >= 0) ? 2 : 3), y-6, barMin, TINSIZE|LEFT);
    lcdDrawNumber(x+GAUGE_WIDTH+1, y-6, barMax, TINSIZE);
  }
  if (barMin < -101)
    barMin = -101;
  if (barMax > 101)
    barMax = 101;
  lcdDrawHorizontalLine(x-2, y, GAUGE_WIDTH+2, DOTTED);
  lcdDrawHorizontalLine(x-2, y+GAUGE_HEIGHT, GAUGE_WIDTH+2, DOTTED);
  lcdDrawSolidVerticalLine(x-2, y+1, GAUGE_HEIGHT-1);
  lcdDrawSolidVerticalLine(x+GAUGE_WIDTH-1, y+1, GAUGE_HEIGHT-1);
  if (barMin <= barMax) {
    int8_t right = (barMax * GAUGE_WIDTH) / 200;
    int8_t left = ((barMin * GAUGE_WIDTH) / 200)-1;
    lcdDrawSolidFilledRect(x+GAUGE_WIDTH/2+left, y+2, right-left, GAUGE_HEIGHT-3);
  }
  lcdDrawSolidVerticalLine(x+GAUGE_WIDTH/2-1, y, GAUGE_HEIGHT+1);
  if (barMin == -101) {
    for (uint8_t i=0; i<3; ++i) {
      lcdDrawPoint(x+i, y+4-i);
      lcdDrawPoint(x+3+i, y+4-i);
    }
  }
  if (barMax == 101) {
    for (uint8_t i=0; i<3; ++i) {
      lcdDrawPoint(x+GAUGE_WIDTH-8+i, y+4-i);
      lcdDrawPoint(x+GAUGE_WIDTH-5+i, y+4-i);
    }
  }
}
#undef GAUGE_WIDTH
#undef GAUGE_HEIGHT
#endif

bool menuModelMixOne(evt_t event)
{
  MixData *md2 = mixAddress(s_currIdx) ;

  SUBMENU(s_currCh ? STR_INSERTMIX : STR_EDITMIX, MIX_FIELD_COUNT, 0, { 0, 0, 0, 0, 0, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0 /*, ...*/ });

  putsChn(MENU_TITLE_NEXT_POS, MENU_TITLE_TOP+1, md2->destCh+1, TEXT_COLOR);

  // The separation line between 2 columns
  lcdDrawSolidVerticalLine(MENU_COLUMN2_X-20, DEFAULT_SCROLLBAR_Y, DEFAULT_SCROLLBAR_H+5, TEXT_COLOR);

  int8_t sub = menuVerticalPosition;
  int8_t editMode = s_editMode;

  for (int k=0; k<2*NUM_BODY_LINES; k++) {
    coord_t y;
    if (k >= NUM_BODY_LINES) {
      y = MENU_CONTENT_TOP + (k-NUM_BODY_LINES)*FH + 2;
    }
    else {
      y = MENU_CONTENT_TOP + k*FH + 2;
    }
    int8_t i = k;

    LcdFlags attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i) {
      case MIX_FIELD_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MIXNAME);
        editName(MIXES_2ND_COLUMN, y, md2->name, sizeof(md2->name), event, attr);
        break;
      case MIX_FIELD_SOURCE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, NO_INDENT(STR_SOURCE));
        putsMixerSource(MIXES_2ND_COLUMN, y, md2->srcRaw, STREXPANDED|attr);
        if (attr) CHECK_INCDEC_MODELSOURCE(event, md2->srcRaw, 1, MIXSRC_LAST);
        break;
      case MIX_FIELD_WEIGHT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_WEIGHT);
        gvarWeightItem(MIXES_2ND_COLUMN, y, md2, attr|LEFT, event);
        break;
      case MIX_FIELD_OFFSET:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, NO_INDENT(STR_OFFSET));
        u_int8int16_t offset;
        MD_OFFSET_TO_UNION(md2, offset);
        offset.word = GVAR_MENU_ITEM(MIXES_2ND_COLUMN, y, offset.word, GV_RANGELARGE_OFFSET_NEG, GV_RANGELARGE_OFFSET, attr|LEFT, 0, event);
        MD_UNION_TO_OFFSET(offset, md2);
#if 0
        drawOffsetBar(x+MIXES_2ND_COLUMN+22, y, md2);
#endif
        break;
      }
      case MIX_FIELD_TRIM:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TRIM);
        drawCheckBox(MIXES_2ND_COLUMN, y, !md2->carryTrim, attr);
        if (attr) md2->carryTrim = !checkIncDecModel(event, !md2->carryTrim, 0, 1);
        break;
#if defined(CURVES)
      case MIX_FIELD_CURVE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CURVE);
        editCurveRef(MIXES_2ND_COLUMN, y, md2->curve, event, attr);
        break;
#endif
#if defined(FLIGHT_MODES)
      case MIX_FIELD_FLIGHT_PHASE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_FLMODE);
        md2->flightModes = editFlightModes(MIXES_2ND_COLUMN, y, event, md2->flightModes, attr);
        break;
#endif
      case MIX_FIELD_SWITCH:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SWITCH);
        md2->swtch = switchMenuItem(MIXES_2ND_COLUMN, y, md2->swtch, attr, event);
        break;
      case MIX_FIELD_MLTPX:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTPX);
        md2->mltpx = selectMenuItem(MIXES_2ND_COLUMN, y, STR_VMLTPX, md2->mltpx, 0, 2, attr, event);
        break;
      case MIX_FIELD_DELAY_UP:
        lcdDrawText(MENU_COLUMN2_X+MENUS_MARGIN_LEFT, y, STR_DELAYUP);
        md2->delayUp = editDelay(MENU_COLUMN2_X, y, event, attr, md2->delayUp);
        break;
      case MIX_FIELD_DELAY_DOWN:
        lcdDrawText(MENU_COLUMN2_X+MENUS_MARGIN_LEFT, y, STR_DELAYDOWN);
        md2->delayDown = editDelay(MENU_COLUMN2_X, y, event, attr, md2->delayDown);
        break;
      case MIX_FIELD_SLOW_UP:
        lcdDrawText(MENU_COLUMN2_X+MENUS_MARGIN_LEFT, y, STR_SLOWUP);
        md2->speedUp = editDelay(MENU_COLUMN2_X, y, event, attr, md2->speedUp);
        break;
      case MIX_FIELD_SLOW_DOWN:
        lcdDrawText(MENU_COLUMN2_X+MENUS_MARGIN_LEFT, y, STR_SLOWDOWN);
        md2->speedDown = editDelay(MENU_COLUMN2_X, y, event, attr, md2->speedDown);
        break;
    }
  }

  return true;
}

static uint8_t s_copySrcIdx;
static uint8_t s_copySrcCh;

#define _STR_MAX(x) PSTR("/" #x)
#define STR_MAX(x) _STR_MAX(x)

#define EXPO_LINE_WEIGHT_POS    92
#define EXPO_LINE_SRC_POS       115
#define EXPO_LINE_CURVE_POS     162
#define EXPO_LINE_SWITCH_POS    210
#define EXPO_LINE_SIDE_POS      250
#define EXPO_LINE_FM_POS        270
#define EXPO_LINE_NAME_POS      380
#define EXPO_LINE_SELECT_POS    50
#define EXPO_LINE_SELECT_WIDTH  (LCD_W-EXPO_LINE_SELECT_POS-5)

#define MIX_LINE_WEIGHT_POS     92
#define MIX_LINE_SRC_POS        115
#define MIX_LINE_CURVE_POS      162
#define MIX_LINE_SWITCH_POS     210
#define MIX_LINE_DELAY_POS      255
#define MIX_LINE_FM_POS         270
#define MIX_LINE_NAME_POS       380
#define MIX_LINE_SELECT_POS     50
#define MIX_LINE_SELECT_WIDTH   (LCD_W-EXPO_LINE_SELECT_POS-5)

void lineSurround(bool expo, coord_t y, LcdFlags flags=CURVE_AXIS_COLOR)
{
  lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : MIX_LINE_SELECT_POS, y-INVERT_VERT_MARGIN, expo ? EXPO_LINE_SELECT_WIDTH : MIX_LINE_SELECT_WIDTH, INVERT_LINE_HEIGHT, (s_copyMode == COPY_MODE ? SOLID : DOTTED), flags);
}

void onExpoMixMenu(const char *result)
{
  bool expo = (menuHandlers[menuLevel] == menuModelExposAll);
  uint8_t chn = (expo ? expoAddress(s_currIdx)->chn+1 : mixAddress(s_currIdx)->destCh+1);

  if (result == STR_EDIT) {
    pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
  }
  else if (result == STR_INSERT_BEFORE || result == STR_INSERT_AFTER) {
    if (!reachExpoMixCountLimit(expo)) {
      s_currCh = chn;
      if (result == STR_INSERT_AFTER) { s_currIdx++; menuVerticalPosition++; }
      insertExpoMix(expo, s_currIdx);
      pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
    }
  }
  else if (result == STR_COPY || result == STR_MOVE) {
    s_copyMode = (result == STR_COPY ? COPY_MODE : MOVE_MODE);
    s_copySrcIdx = s_currIdx;
    s_copySrcCh = chn;
    s_copySrcRow = menuVerticalPosition;
  }
  else if (result == STR_DELETE) {
    deleteExpoMix(expo, s_currIdx);
  }
}

void displayHeaderChannelName(uint8_t ch)
{
  uint8_t len = zlen(g_model.limitData[ch-1].name, sizeof(g_model.limitData[ch-1].name));
  if (len) {
    lcdDrawSizedText(COLUMN_HEADER_X, MENU_FOOTER_TOP, g_model.limitData[ch-1].name, len, HEADER_COLOR|ZCHAR);
  }
}

void displayMixInfos(coord_t y, MixData *md)
{
  putsCurveRef(MIX_LINE_CURVE_POS, y, md->curve);

  if (md->swtch) {
    putsSwitches(MIX_LINE_SWITCH_POS, y, md->swtch);
  }
}

void displayMixLine(coord_t y, MixData *md)
{
  if (md->name[0])
    lcdDrawSizedText(EXPO_LINE_NAME_POS, y, md->name, sizeof(md->name), ZCHAR);
  displayMixInfos(y, md);
  displayFlightModes(MIX_LINE_FM_POS, y, md->flightModes, 0);
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
    lcdDrawSizedText(EXPO_LINE_NAME_POS, y, ed->name, sizeof(ed->name), ZCHAR);
  }
}

bool menuModelExpoMix(uint8_t expo, evt_t event)
{
  int sub = menuVerticalPosition;

  if (s_editMode > 0)
    s_editMode = 0;

  uint8_t chn = (expo ? expoAddress(s_currIdx)->chn+1 : mixAddress(s_currIdx)->destCh+1);

  int linesCount = getLinesCount(expo);
  SIMPLE_MENU(expo ? STR_MENUINPUTS : STR_MIXER, menuTabModel, expo ? e_InputsAll : e_MixAll, linesCount, DEFAULT_SCROLLBAR_X);

  switch (event)
  {
    case EVT_ENTRY:
    case EVT_ENTRY_UP:
      s_copyMode = 0;
      s_copyTgtOfs = 0;
      break;
    case EVT_KEY_LONG(KEY_EXIT):
      if (s_copyMode && s_copyTgtOfs == 0) {
        deleteExpoMix(expo, s_currIdx);
        killEvents(event);
        event = 0;
      }
      // no break
    case EVT_KEY_BREAK(KEY_EXIT):
      if (s_copyMode) {
        if (s_copyTgtOfs) {
          // cancel the current copy / move operation
          if (s_copyMode == COPY_MODE) {
            deleteExpoMix(expo, s_currIdx);
          }
          else {
            do {
              swapExpoMix(expo, s_currIdx, s_copyTgtOfs > 0);
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
            pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
          }
        }
        else {
          if (s_copyMode) s_currCh = 0;
          if (s_currCh) {
            if (reachExpoMixCountLimit(expo)) break;
            insertExpoMix(expo, s_currIdx);
            pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
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
            popupMenuHandler = onExpoMixMenu;
          }
        }
      }
      break;
    case EVT_KEY_LONG(KEY_LEFT):
    case EVT_KEY_LONG(KEY_RIGHT):
      if (s_copyMode && !s_copyTgtOfs) {
        if (reachExpoMixCountLimit(expo)) break;
        s_currCh = chn;
        if (event == EVT_KEY_LONG(KEY_RIGHT)) { s_currIdx++; menuVerticalPosition++; }
        insertExpoMix(expo, s_currIdx);
        pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
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
          if (reachExpoMixCountLimit(expo)) break;
          copyExpoMix(expo, s_currIdx);
          if (event==EVT_ROTARY_RIGHT || key==KEY_DOWN) s_currIdx++;
          else if (sub-menuVerticalOffset >= 6) menuVerticalOffset++;
        }
        else if (next_ofs==0 && s_copyMode==COPY_MODE) {
          // delete the mix
          deleteExpoMix(expo, s_currIdx);
          if (event==EVT_ROTARY_LEFT || key==KEY_UP) s_currIdx--;
        }
        else {
          // only swap the mix with its neighbor
          if (!swapExpoMix(expo, s_currIdx, event==EVT_ROTARY_LEFT || key==KEY_UP)) break;
          storageDirty(EE_MODEL);
        }

        s_copyTgtOfs = next_ofs;
      }
      break;
  }

  char str[6];
  sprintf(str, "%d/%d", getExpoMixCount(expo), expo ? MAX_EXPOS : MAX_MIXERS);
  lcdDrawText(MENU_TITLE_NEXT_POS, MENU_TITLE_TOP+2, str, HEADER_COLOR);

  sub = menuVerticalPosition;
  s_currCh = 0;
  int cur = 0;
  int i = 0;

  for (int ch=1; ch<=(expo ? NUM_INPUTS : NUM_CHNOUT); ch++) {
    void *pointer = NULL; MixData * &md = (MixData * &)pointer; ExpoData * &ed = (ExpoData * &)pointer;
    coord_t y = MENU_CONTENT_TOP + (cur-menuVerticalOffset)*FH;
    if (expo ? (i<MAX_EXPOS && (ed=expoAddress(i))->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && (md=mixAddress(i))->srcRaw && md->destCh+1 == ch)) {
      if (cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES) {
        if (expo) {
          putsMixerSource(MENUS_MARGIN_LEFT, y, ch);
        }
        else {
          putsChn(MENUS_MARGIN_LEFT, y, ch, 0); // show CHx
        }
      }
      uint8_t mixCnt = 0;
      do {
        if (s_copyMode) {
          if (s_copyMode == MOVE_MODE && cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES && s_copySrcCh == ch && s_copyTgtOfs != 0 && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
            lineSurround(expo, y);
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
          if (expo) {
            GVAR_MENU_ITEM(EXPO_LINE_WEIGHT_POS, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr | (isExpoActive(i) ? BOLD : 0), 0, 0);
            displayExpoLine(y, ed);
          }
          else {
            if (attr) {
              displayHeaderChannelName(ch);
            }

            if (mixCnt > 0) lcdDrawTextAtIndex(6, y, STR_VMLTPX2, md->mltpx, 0);

            putsMixerSource(MIX_LINE_SRC_POS, y, md->srcRaw);

            gvarWeightItem(MIX_LINE_WEIGHT_POS, y, md, attr | (isMixActive(i) ? BOLD : 0), event);

            displayMixLine(y, md);

            char cs[] = " ";
            if (md->speedDown || md->speedUp)
              cs[0] = 'S';
            if (md->delayUp || md->delayDown)
              cs[0] = (cs[0] =='S' ? '*' : 'D');
            lcdDrawText(MIX_LINE_DELAY_POS, y, cs);
          }
          if (s_copyMode) {
            if ((s_copyMode==COPY_MODE || s_copyTgtOfs == 0) && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
              /* draw a border around the raw on selection mode (copy/move) */
              lineSurround(expo, y);
            }
            if (cur == sub) {
              /* invert the raw when it's the current one */
              lineSurround(expo, y, ALARM_COLOR);
            }
          }
        }
        cur++; y+=FH; mixCnt++; i++; if (expo) ed++; else md++;
      } while (expo ? (i<MAX_EXPOS && ed->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && md->srcRaw && md->destCh+1 == ch));
      if (s_copyMode == MOVE_MODE && cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
        lineSurround(expo, y);
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
        if (expo) {
          putsMixerSource(MENUS_MARGIN_LEFT, y, ch, attr);
        }
        else {
          putsChn(MENUS_MARGIN_LEFT, y, ch, attr); // show CHx
          if (attr) {
            displayHeaderChannelName(ch);
          }
        }
        if (s_copyMode == MOVE_MODE && s_copySrcCh == ch) {
          lineSurround(expo, y);
        }
      }
      cur++; y+=FH;
    }
  }

  if (sub >= linesCount-1) menuVerticalPosition = linesCount-1;

  return true;
}

bool menuModelExposAll(evt_t event)
{
  return menuModelExpoMix(1, event);
}

bool menuModelMixAll(evt_t event)
{
  return menuModelExpoMix(0, event);
}
