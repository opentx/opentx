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

#define EXPO_ONE_2ND_COLUMN (7*FW+3*FW+2)
#define EXPO_ONE_FM_WIDTH   (5*FW)

int expoFn(int x)
{
  ExpoData *ed = expoAddress(s_currIdx);
  int16_t anas[NUM_INPUTS] = {0};
  anas[ed->chn] = x;
  applyExpos(anas, e_perout_mode_inactive_flight_mode);
  return anas[ed->chn];
}

void drawFunction(FnFuncP fn, uint8_t offset)
{
  lcdDrawVerticalLine(CURVE_CENTER_X-offset, 0, LCD_H, 0xee);
  lcdDrawHorizontalLine(CURVE_CENTER_X-CURVE_SIDE_WIDTH-offset, CURVE_CENTER_Y, CURVE_SIDE_WIDTH*2, 0xee);

  coord_t prev_yv = (coord_t)-1;

  for (int8_t xv=-CURVE_SIDE_WIDTH; xv<=CURVE_SIDE_WIDTH; xv++) {
    coord_t yv = (LCD_H-1) - (((uint16_t)RESX + fn(xv * (RESX/CURVE_SIDE_WIDTH))) / 2 * (LCD_H-1) / RESX);
    if (prev_yv != (coord_t)-1) {
      if (abs((int8_t)yv-prev_yv) <= 1) {
        lcdDrawPoint(CURVE_CENTER_X+xv-offset-1, prev_yv, FORCE);
      }
      else {
        uint8_t tmp = (prev_yv < yv ? 0 : 1);
        lcdDrawSolidVerticalLine(CURVE_CENTER_X+xv-offset-1, yv+tmp, prev_yv-yv);
      }
    }
    prev_yv = yv;
  }
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
    memmove(expo, expo+1, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
    memclear(&g_model.expoData[MAX_EXPOS-1], sizeof(ExpoData));
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
    expo->mode = 3; // pos&neg
    expo->chn = s_currCh - 1;
    expo->weight = 100;
  }
  else {
    MixData *mix = mixAddress(idx);
    memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
    memclear(mix, sizeof(MixData));
    mix->destCh = s_currCh-1;
    mix->srcRaw = (s_currCh > 4 ? MIXSRC_Rud - 1 + s_currCh : MIXSRC_Rud - 1 + channel_order(s_currCh));
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
      if (((MixData *)x)->destCh == MAX_OUTPUT_CHANNELS-1)
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
        if (destCh<MAX_OUTPUT_CHANNELS-1) ((MixData *)x)->destCh++;
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
  CASE_CPUARM(EXPO_FIELD_NAME)
  EXPO_FIELD_WEIGHT,
  EXPO_FIELD_EXPO,
  CASE_CURVES(EXPO_FIELD_CURVE)
  CASE_FLIGHT_MODES(EXPO_FIELD_FLIGHT_MODES)
  EXPO_FIELD_SWITCH,
  EXPO_FIELD_SIDE,
  EXPO_FIELD_MAX
};

#define CURVE_ROWS 0

void menuModelExpoOne(event_t event)
{
  ExpoData * ed = expoAddress(s_currIdx);
  drawSource(7*FW+FW/2, 0, MIXSRC_Rud+ed->chn, 0);

  SUBMENU(STR_MENUINPUTS, EXPO_FIELD_MAX, {CASE_CPUARM(0) 0, 0, CASE_CURVES(CURVE_ROWS) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0 /*, ...*/});

  int8_t sub = menuVerticalPosition;

  coord_t y = MENU_HEADER_HEIGHT + 1;

  for (uint8_t i=0; i<EXPO_FIELD_MAX+1; i++) {
    uint8_t attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch (i) {
#if defined(CPUARM)
      case EXPO_FIELD_NAME:
        editSingleName(EXPO_ONE_2ND_COLUMN-sizeof(ed->name)*FW, y, STR_EXPONAME, ed->name, sizeof(ed->name), event, attr);
        break;
#endif

      case EXPO_FIELD_WEIGHT:
        lcdDrawTextAlignedLeft(y, STR_WEIGHT);
        ed->weight = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr, 0, event);
        break;

      case EXPO_FIELD_EXPO:
        lcdDrawTextAlignedLeft(y, STR_EXPO);
        if (ed->curveMode==MODE_EXPO || ed->curveParam==0) {
          ed->curveMode = MODE_EXPO;
          ed->curveParam = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->curveParam, -100, 100, attr, 0, event);
        }
        else {
          lcdDrawText(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_NA, attr);
        }
        break;

#if defined(CURVES)
      case EXPO_FIELD_CURVE:
        lcdDrawTextAlignedLeft(y, STR_CURVE);
        if (ed->curveMode!=MODE_EXPO || ed->curveParam==0) {
          drawCurveName(EXPO_ONE_2ND_COLUMN-3*FW, y, ed->curveParam, attr);
          if (attr) {
            CHECK_INCDEC_MODELVAR_ZERO(event, ed->curveParam, CURVE_BASE+MAX_CURVES-1);
            if (ed->curveParam) ed->curveMode = MODE_CURVE;
            if (ed->curveParam>=CURVE_BASE && event==EVT_KEY_LONG(KEY_ENTER)) {
              s_curveChan = ed->curveParam - CURVE_BASE;
              pushMenu(menuModelCurveOne);
            }
          }
        }
        else {
          lcdDrawText(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_NA, attr);
        }
        break;
#endif

#if defined(FLIGHT_MODES)
      case EXPO_FIELD_FLIGHT_MODES:
        ed->flightModes = editFlightModes(EXPO_ONE_2ND_COLUMN-EXPO_ONE_FM_WIDTH, y, event, ed->flightModes, attr);
        break;
#endif

      case EXPO_FIELD_SWITCH:
        ed->swtch = editSwitch(EXPO_ONE_2ND_COLUMN-3*FW, y, ed->swtch, attr, event);
        break;

      case EXPO_FIELD_SIDE:
        ed->mode = 4 - editChoice(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_SIDE, STR_VSIDE, 4-ed->mode, 1, 3, attr, event);
        break;
    }
    y += FH;
  }

  drawFunction(expoFn);

  int16_t x512 = calibratedAnalogs[ed->chn];
  lcdDrawNumber(LCD_W-8, 6*FH, calcRESXto100(x512), 0);
  int16_t y512 = expoFn(x512);
  lcdDrawNumber(LCD_W-8-6*FW, 1*FH, calcRESXto100(y512), 0);

#if defined(CPUARM)
  x512 = CURVE_CENTER_X+x512/(RESX/CURVE_SIDE_WIDTH);
  y512 = (LCD_H-1) - ((y512+RESX)/2) * (LCD_H-1) / RESX;
#else
  x512 = CURVE_CENTER_X+x512/(RESXu/CURVE_SIDE_WIDTH);
  y512 = (LCD_H-1) - (uint16_t)((y512+RESX)/2) * (LCD_H-1) / RESX;
#endif

  lcdDrawSolidVerticalLine(x512, y512-3, 3*2+1);
  lcdDrawSolidHorizontalLine(x512-3, y512, 3*2+1);
}

enum MixFields {
  CASE_CPUARM(MIX_FIELD_NAME)
  MIX_FIELD_SOURCE,
  MIX_FIELD_WEIGHT,
  MIX_FIELD_OFFSET,
  MIX_FIELD_TRIM,
  CASE_CURVES(MIX_FIELD_CURVE)
  CASE_FLIGHT_MODES(MIX_FIELD_FLIGHT_PHASE)
  MIX_FIELD_SWITCH,
  MIX_FIELD_WARNING,
  MIX_FIELD_MLTPX,
  MIX_FIELD_DELAY_UP,
  MIX_FIELD_DELAY_DOWN,
  MIX_FIELD_SLOW_UP,
  MIX_FIELD_SLOW_DOWN,
  MIX_FIELD_COUNT
};

#if !defined(CPUM64) || !defined(TELEMETRY_FRSKY)
#define GAUGE_WIDTH  33
#define GAUGE_HEIGHT 6
void drawOffsetBar(uint8_t x, uint8_t y, MixData * md)
{
  int offset = GET_GVAR(MD_OFFSET(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
  int weight = GET_GVAR(MD_WEIGHT(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
  int barMin = offset - weight;
  int barMax = offset + weight;
  if (y > 15) {
#if defined(CPUARM)
    lcdDrawNumber(x-((barMin >= 0) ? 2 : 3), y-6, barMin, TINSIZE|LEFT);
    lcdDrawNumber(x+GAUGE_WIDTH+1, y-6, barMax, TINSIZE);
#else
    lcdDrawNumber(x-((barMin >= 0) ? 2 : 3), y-8, barMin, LEFT);
    lcdDrawNumber(x+GAUGE_WIDTH+1, y-8, barMax);
#endif
  }
  if (weight < 0) {
    barMin = -barMin;
    barMax = -barMax;
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

void menuModelMixOne(event_t event)
{
  TITLE(STR_MIXER);
  MixData * md2 = mixAddress(s_currIdx) ;
  putsChn(lcdLastRightPos+1*FW, 0, md2->destCh+1,0);

#if defined(ROTARY_ENCODERS)
#if defined(CURVES)
  if ((menuVerticalPosition == MIX_FIELD_TRIM && md2->srcRaw > NUM_STICKS) || (menuVerticalPosition == MIX_FIELD_CURVE && md2->curveMode == MODE_CURVE))
#else
  if (menuVerticalPosition == MIX_FIELD_TRIM && md2->srcRaw > NUM_STICKS)
#endif
    SUBMENU_NOTITLE(MIX_FIELD_COUNT, {CASE_CPUARM(0) 0, 0, 0, 0, CASE_CURVES(0) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/})
  else
    SUBMENU_NOTITLE(MIX_FIELD_COUNT, {CASE_CPUARM(0) 0, 0, 0, 1, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/});
#else
  SUBMENU_NOTITLE(MIX_FIELD_COUNT, {CASE_CPUARM(0) 0, 0, 0, 1, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/});
#endif

  int8_t sub = menuVerticalPosition;
  int8_t editMode = s_editMode;

  for (uint8_t k=0; k<LCD_LINES-1; k++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
    int8_t i = k + menuVerticalOffset;

    uint8_t attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch (i) {
#if defined(CPUARM)
      case MIX_FIELD_NAME:
        editSingleName(COLUMN_X+MIXES_2ND_COLUMN, y, STR_MIXNAME, md2->name, sizeof(md2->name), event, attr);
        break;
#endif
      case MIX_FIELD_SOURCE:
        drawFieldLabel(COLUMN_X, y, NO_INDENT(STR_SOURCE));
        drawSource(COLUMN_X+MIXES_2ND_COLUMN, y, md2->srcRaw, STREXPANDED|attr);
        if (attr) CHECK_INCDEC_MODELSOURCE(event, md2->srcRaw, 1, MIXSRC_LAST);
        break;
      case MIX_FIELD_WEIGHT:
        drawFieldLabel(COLUMN_X, y, STR_WEIGHT);
        gvarWeightItem(COLUMN_X+MIXES_2ND_COLUMN, y, md2, attr|LEFT, event);
        break;
      case MIX_FIELD_OFFSET:
      {
        drawFieldLabel(COLUMN_X, y, NO_INDENT(STR_OFFSET));
        u_int8int16_t offset;
        MD_OFFSET_TO_UNION(md2, offset);
        offset.word = GVAR_MENU_ITEM(COLUMN_X+MIXES_2ND_COLUMN, y, offset.word, GV_RANGELARGE_OFFSET_NEG, GV_RANGELARGE_OFFSET, attr|LEFT, 0, event);
        MD_UNION_TO_OFFSET(offset, md2);
#if !defined(CPUM64) || !defined(TELEMETRY_FRSKY)
        drawOffsetBar(COLUMN_X+MIXES_2ND_COLUMN+22, y, md2);
#endif
        break;
      }

      case MIX_FIELD_TRIM:
      {
        uint8_t not_stick = (md2->srcRaw > NUM_STICKS);
        int8_t carryTrim = -md2->carryTrim;
        drawFieldLabel(COLUMN_X, y, STR_TRIM);
        lcdDrawTextAtIndex((not_stick ? COLUMN_X+MIXES_2ND_COLUMN : COLUMN_X+6*FW-3), y, STR_VMIXTRIMS, (not_stick && carryTrim == 0) ? 0 : carryTrim+1, menuHorizontalPosition==0 ? attr : 0);
        if (attr && menuHorizontalPosition==0 && (not_stick || editMode>0)) md2->carryTrim = -checkIncDecModel(event, carryTrim, not_stick ? TRIM_ON : -TRIM_OFF, -TRIM_AIL);
        if (!not_stick) {
          lcdDrawText(COLUMN_X+MIXES_2ND_COLUMN, y, STR_DREX);
          drawCheckBox(COLUMN_X+MIXES_2ND_COLUMN+DREX_CHBOX_OFFSET, y, !md2->noExpo, menuHorizontalPosition==1 ? attr : 0);
          if (attr && menuHorizontalPosition==1 && editMode>0) md2->noExpo = !checkIncDecModel(event, !md2->noExpo, 0, 1);
        }
        else if (attr) {
          REPEAT_LAST_CURSOR_MOVE();
        }
        break;
      }

#if defined(CURVES)
      case MIX_FIELD_CURVE:
      {
        drawFieldLabel(COLUMN_X, y, STR_CURVE);
        int8_t curveParam = md2->curveParam;
        if (md2->curveMode == MODE_CURVE) {
          drawCurveName(COLUMN_X+MIXES_2ND_COLUMN, y, curveParam, attr);
          if (attr) {
            if (event==EVT_KEY_LONG(KEY_ENTER) && (curveParam<0 || curveParam>=CURVE_BASE)){
              s_curveChan = (curveParam<0 ? -curveParam-1 : curveParam-CURVE_BASE);
              pushMenu(menuModelCurveOne);
            }
            else {
              CHECK_INCDEC_MODELVAR(event, md2->curveParam, -MAX_CURVES, CURVE_BASE+MAX_CURVES-1);
              if (md2->curveParam == 0)
                md2->curveMode = MODE_DIFFERENTIAL;
            }
          }
        }
        else {
          lcdDrawText(COLUMN_X+MIXES_2ND_COLUMN, y, PSTR("Diff"), menuHorizontalPosition==0 ? attr : 0);
          md2->curveParam = GVAR_MENU_ITEM(COLUMN_X+MIXES_2ND_COLUMN+5*FW, y, curveParam, -100, 100, LEFT|(menuHorizontalPosition==1 ? attr : 0), 0, editMode>0 ? event : 0);
          if (attr && editMode>0 && menuHorizontalPosition==0) {
            int8_t tmp = 0;
            CHECK_INCDEC_MODELVAR(event, tmp, -1, 1);
            if (tmp != 0) {
              md2->curveMode = MODE_CURVE;
              md2->curveParam = tmp;
            }
          }
        }
        break;
      }
#endif
#if defined(FLIGHT_MODES)
      case MIX_FIELD_FLIGHT_PHASE:
        md2->flightModes = editFlightModes(COLUMN_X+MIXES_2ND_COLUMN, y, event, md2->flightModes, attr);
        break;
#endif
      case MIX_FIELD_SWITCH:
        md2->swtch = editSwitch(COLUMN_X+MIXES_2ND_COLUMN, y, md2->swtch, attr, event);
        break;
      case MIX_FIELD_WARNING:
        drawFieldLabel(COLUMN_X+MIXES_2ND_COLUMN, y, STR_MIXWARNING);
        if (md2->mixWarn)
          lcdDrawNumber(COLUMN_X+MIXES_2ND_COLUMN, y, md2->mixWarn, attr|LEFT);
        else
          lcdDrawText(COLUMN_X+MIXES_2ND_COLUMN, y, STR_OFF, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, md2->mixWarn, 3);
        break;
      case MIX_FIELD_MLTPX:
        md2->mltpx = editChoice(COLUMN_X+MIXES_2ND_COLUMN, y, STR_MULTPX, STR_VMLTPX, md2->mltpx, 0, 2, attr, event);
        break;
      case MIX_FIELD_DELAY_UP:
        md2->delayUp = EDIT_DELAY(COLUMN_X, y, event, attr, STR_DELAYUP, md2->delayUp);
        break;
      case MIX_FIELD_DELAY_DOWN:
        md2->delayDown = EDIT_DELAY(COLUMN_X, y, event, attr, STR_DELAYDOWN, md2->delayDown);
        break;
      case MIX_FIELD_SLOW_UP:
        md2->speedUp = EDIT_DELAY(COLUMN_X, y, event, attr, STR_SLOWUP, md2->speedUp);
        break;
      case MIX_FIELD_SLOW_DOWN:
        md2->speedDown = EDIT_DELAY(COLUMN_X, y, event, attr, STR_SLOWDOWN, md2->speedDown);
        break;
    }
  }
}

#define _STR_MAX(x) PSTR("/" #x)
#define STR_MAX(x) _STR_MAX(x)

#if defined(CPUARM)
  #define EXPO_LINE_WEIGHT_POS 7*FW+1
  #define EXPO_LINE_EXPO_POS   10*FW+5
  #define EXPO_LINE_SWITCH_POS 11*FW+2
  #define EXPO_LINE_SIDE_POS   14*FW+2
  #define EXPO_LINE_SELECT_POS 24
  #define EXPO_LINE_FM_POS
  #define EXPO_LINE_NAME_POS   LCD_W-LEN_EXPOMIX_NAME*FW
  #define MIX_LINE_SRC_POS     4*FW-1
  #define MIX_LINE_WEIGHT_POS  11*FW+3
  #define MIX_LINE_CURVE_POS   12*FW+2
  #define MIX_LINE_SWITCH_POS  16*FW
  #define MIX_LINE_DELAY_POS   19*FW+7
#else
  #define EXPO_LINE_WEIGHT_POS 7*FW+1
  #define EXPO_LINE_EXPO_POS   11*FW
  #define EXPO_LINE_SWITCH_POS 11*FW+4
  #if MAX_FLIGHT_MODES == 6
    #define EXPO_LINE_SIDE_POS 15*FW
  #else
    #define EXPO_LINE_SIDE_POS 15*FW+2
  #endif
  #define EXPO_LINE_FM_POS     LCD_W-FW
  #define EXPO_LINE_SELECT_POS 24
  #define MIX_LINE_SRC_POS     4*FW-1
  #define MIX_LINE_WEIGHT_POS  11*FW+3
  #define MIX_LINE_CURVE_POS   12*FW+2
  #define MIX_LINE_SWITCH_POS  16*FW
  #define MIX_LINE_DELAY_POS   19*FW+7
#endif

#if defined(NAVIGATION_MENUS)
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
#endif

void displayMixInfos(coord_t y, MixData *md)
{
  if (md->curveParam) {
    if (md->curveMode == MODE_CURVE)
      drawCurveName(MIX_LINE_CURVE_POS, y, md->curveParam);
    else
      displayGVar(MIX_LINE_CURVE_POS+3*FW, y, md->curveParam, -100, 100);
  }

  if (md->swtch) {
    drawSwitch(MIX_LINE_SWITCH_POS, y, md->swtch);
  }
}

#if defined(CPUARM)
void displayMixLine(coord_t y, MixData * md)
{
  if (md->name[0]) {
    lcdDrawSizedText(EXPO_LINE_NAME_POS, y, md->name, sizeof(md->name), ZCHAR);
  }
  else {
    displayMixInfos(y, md);
  }
}
#else
#define displayMixLine(y, md) displayMixInfos(y, md)
#endif

void displayExpoInfos(coord_t y, ExpoData *ed)
{
  if (ed->curveMode == MODE_CURVE)
    drawCurveName(EXPO_LINE_EXPO_POS-3*FW, y, ed->curveParam);
  else
    displayGVar(EXPO_LINE_EXPO_POS, y, ed->curveParam, -100, 100);

  drawSwitch(EXPO_LINE_SWITCH_POS, y, ed->swtch, 0);
}

#if defined(CPUARM)
void displayExpoLine(coord_t y, ExpoData *ed)
{
  displayExpoInfos(y, ed);

  if (ed->name[0]) {
    lcdDrawSizedText(EXPO_LINE_NAME_POS, y, ed->name, sizeof(ed->name), ZCHAR);
  }
}
#else
#define displayExpoLine(y, ed) \
  displayExpoInfos(y, ed); \
  displayFlightModes(EXPO_LINE_FM_POS, y, ed->flightModes)
#endif

void menuModelExpoMix(uint8_t expo, event_t event)
{
  uint8_t sub = menuVerticalPosition;

  if (s_editMode > 0)
    s_editMode = 0;

  uint8_t chn = (expo ? expoAddress(s_currIdx)->chn+1 : mixAddress(s_currIdx)->destCh+1);

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
#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_LONG:
      if (s_copyMode) {
        killEvents(event);
      }
#endif
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
      if (sub != 0 && (!s_currCh || (s_copyMode && !s_copyTgtOfs)) && !READ_ONLY()) {
        s_copyMode = (s_copyMode == COPY_MODE ? MOVE_MODE : COPY_MODE);
        s_copySrcIdx = s_currIdx;
        s_copySrcCh = chn;
        s_copySrcRow = sub;
        break;
      }
      // no break

    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(event);
      if (s_copyTgtOfs) {
        s_copyMode = 0;
        s_copyTgtOfs = 0;
      }
      else if (sub != 0) {
        if (READ_ONLY()) {
          if (!s_currCh) {
            pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
            return;
          }
        }
        else {
          if (s_copyMode) s_currCh = 0;
#if defined(NAVIGATION_MENUS)
          if (s_currCh) {
            if (reachExpoMixCountLimit(expo)) break;
            insertExpoMix(expo, s_currIdx);
            pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
            s_copyMode = 0;
            return;
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
            POPUP_MENU_START(onExpoMixMenu);
          }
#else
          if (s_currCh) {
            if (reachExpoMixCountLimit(expo)) break;
            insertExpoMix(expo, s_currIdx);
          }
          pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
          s_copyMode = 0;
          return;
#endif
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
        return;
      }
      break;
#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_LEFT:
    case EVT_ROTARY_RIGHT:
#endif
    case EVT_KEY_FIRST(KEY_UP):
    case EVT_KEY_REPT(KEY_UP):
    case EVT_KEY_FIRST(KEY_DOWN):
    case EVT_KEY_REPT(KEY_DOWN):
      if (s_copyMode) {
        uint8_t key = (event & 0x1f);
        uint8_t next_ofs = ((IS_ROTARY_LEFT(event) || key==KEY_UP) ? s_copyTgtOfs - 1 : s_copyTgtOfs + 1);

        if (s_copyTgtOfs==0 && s_copyMode==COPY_MODE) {
          // insert a mix on the same channel (just above / just below)
          if (reachExpoMixCountLimit(expo)) break;
          copyExpoMix(expo, s_currIdx);
          if (IS_ROTARY_RIGHT(event) || key==KEY_DOWN) s_currIdx++;
          else if (sub-menuVerticalOffset >= 6) menuVerticalOffset++;
        }
        else if (next_ofs==0 && s_copyMode==COPY_MODE) {
          // delete the mix
          deleteExpoMix(expo, s_currIdx);
          if (IS_ROTARY_LEFT(event) || key==KEY_UP) s_currIdx--;
        }
        else {
          // only swap the mix with its neighbor
          if (!swapExpoMix(expo, s_currIdx, IS_ROTARY_LEFT(event) || key==KEY_UP)) break;
          storageDirty(EE_MODEL);
        }

        s_copyTgtOfs = next_ofs;
      }
      break;
  }

  lcdDrawNumber(FW*max(sizeof(TR_MENUINPUTS), sizeof(TR_MIXER))+FW+FW/2, 0, getExpoMixCount(expo));
  lcdDrawText(FW*max(sizeof(TR_MENUINPUTS), sizeof(TR_MIXER))+FW+FW/2, 0, expo ? STR_MAX(MAX_EXPOS) : STR_MAX(MAX_MIXERS));

  SIMPLE_MENU(expo ? STR_MENUINPUTS : STR_MIXER, menuTabModel, expo ? MENU_MODEL_INPUTS : MENU_MODEL_MIXES, s_maxLines);

  sub = menuVerticalPosition;
  s_currCh = 0;
  uint8_t cur = 1;
  uint8_t i = 0;

  for (uint8_t ch=1; ch<=(expo ? NUM_INPUTS : MAX_OUTPUT_CHANNELS); ch++) {
    void *pointer = NULL; MixData * &md = (MixData * &)pointer; ExpoData * &ed = (ExpoData * &)pointer;
    coord_t y = MENU_HEADER_HEIGHT-FH+1+(cur-menuVerticalOffset)*FH;
    if (expo ? (i<MAX_EXPOS && (ed=expoAddress(i))->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && (md=mixAddress(i))->srcRaw && md->destCh+1 == ch)) {
      if (menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES) {
        if (expo) {
          drawSource(0, y, MIXSRC_Rud+ch-1, 0);
        }
        else {
          putsChn(0, y, ch, 0); // show CHx
        }
      }
      uint8_t mixCnt = 0;
      do {
        if (s_copyMode) {
          if (s_copyMode == MOVE_MODE && menuVerticalOffset < cur && cur-menuVerticalOffset < 8 && s_copySrcCh == ch && s_copyTgtOfs != 0 && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
            lcdDrawRect(expo ? 18 : 22, y-1, expo ? LCD_W-18 : LCD_W-22, 9, DOTTED);
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
        if (menuVerticalOffset < cur && cur-menuVerticalOffset < 8) {
          uint8_t attr = ((s_copyMode || sub != cur) ? 0 : INVERS);
          if (expo) {
            ed->weight = GVAR_MENU_ITEM(EXPO_LINE_WEIGHT_POS, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr | (isExpoActive(i) ? BOLD : 0), 0, event);
            displayExpoLine(y, ed);
            if (ed->mode!=3) {
              lcdDrawChar(EXPO_LINE_SIDE_POS, y, ed->mode == 2 ? 126 : 127);
            }
          }
          else {
            if (mixCnt > 0) lcdDrawTextAtIndex(FW, y, STR_VMLTPX2, md->mltpx, 0);

            drawSource(MIX_LINE_SRC_POS, y, md->srcRaw, 0);

            gvarWeightItem(MIX_LINE_WEIGHT_POS, y, md, attr | (isMixActive(i) ? BOLD : 0), event);

            displayMixLine(y, md);

            char cs = ' ';
            if (md->speedDown || md->speedUp)
              cs = 'S';
            if (md->delayUp || md->delayDown)
              cs = (cs =='S' ? '*' : 'D');
            lcdDrawChar(MIX_LINE_DELAY_POS, y, cs);
          }
          if (s_copyMode) {
            if ((s_copyMode==COPY_MODE || s_copyTgtOfs == 0) && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
              /* draw a border around the raw on selection mode (copy/move) */
              lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? (LCD_W-EXPO_LINE_SELECT_POS) : (LCD_W-22), 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
            }
            if (cur == sub) {
              /* invert the raw when it's the current one */
              lcdDrawSolidFilledRect(expo ? EXPO_LINE_SELECT_POS+1 : 23, y, expo ? (LCD_W-EXPO_LINE_SELECT_POS-2) : (LCD_W-24), 7);
            }
          }
        }
        cur++; y+=FH; mixCnt++; i++; if (expo) ed++; else md++;
      } while (expo ? (i<MAX_EXPOS && ed->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && md->srcRaw && md->destCh+1 == ch));
      if (s_copyMode == MOVE_MODE && menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
        lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? LCD_W-EXPO_LINE_SELECT_POS : LCD_W-22, 9, DOTTED);
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
      if (menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES) {
        if (expo) {
          drawSource(0, y, MIXSRC_Rud+ch-1, attr);
        }
        else {
          putsChn(0, y, ch, attr); // show CHx
        }
        if (s_copyMode == MOVE_MODE && s_copySrcCh == ch) {
          lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? (LCD_W-EXPO_LINE_SELECT_POS) : (LCD_W-22), 9, DOTTED);
        }
      }
      cur++; y+=FH;
    }
  }
  s_maxLines = cur;
  if (sub >= s_maxLines-1) menuVerticalPosition = s_maxLines-1;
}

void menuModelExposAll(event_t event)
{
  return menuModelExpoMix(1, event);
}

void menuModelMixAll(event_t event)
{
  return menuModelExpoMix(0, event);
}
