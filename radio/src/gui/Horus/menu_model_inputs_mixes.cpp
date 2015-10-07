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

#define EXPO_ONE_2ND_COLUMN 80

#if defined(FLIGHT_MODES)
void displayFlightModes(coord_t x, coord_t y, FlightModesType value, uint8_t attr)
{
  for (int i=0; i<MAX_FLIGHT_MODES; i++) {
    LcdFlags flags = ((m_posHorz==i && attr) ? INVERS : 0);
    flags |= ((value & (1<<i))) ? TEXT_DISABLE_COLOR : TEXT_COLOR;
    if (attr && m_posHorz < 0) flags |= INVERS;
    char s[] = " ";
    s[0] = '0' + i;
    lcd_putsAtt(x, y, s, flags);
    x += 9;
  }
}

FlightModesType editFlightModes(coord_t x, coord_t y, evt_t event, FlightModesType value, uint8_t attr)
{
  lcd_putsColumnLeft(x, y, STR_FLMODE);

  int posHorz = m_posHorz;

  displayFlightModes(x, y, value, attr);

  if (attr) {
    if (s_editMode && event==EVT_KEY_BREAK(KEY_ENTER)) {
      s_editMode = 0;
      value ^= (1<<posHorz);
      eeDirty(EE_MODEL);
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

coord_t getYCoord(FnFuncP fn, coord_t x)
{
  return limit(0, Y0 - (fn(x * (RESX/WCHART)) * WCHART / RESX), LCD_H-1);
}

void DrawFunction(FnFuncP fn, int offset)
{
  lcd_vlineStip(X0-offset, Y0-WCHART, WCHART*2, 0xee, CURVE_AXIS_COLOR);
  lcd_hlineStip(X0-WCHART-offset, Y0, WCHART*2, 0xee, CURVE_AXIS_COLOR);

  coord_t ymin=0, ymax=0, info=0;
  for (int xv=-WCHART; xv<=WCHART; xv++) {
    coord_t yv = getYCoord(fn, xv);

    if (ymin == 0 || yv < ymin)
      ymin = yv;
    if (ymax == 0 || yv > ymax)
      ymax = yv;
    info += yv*xv;
  }
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
  eeDirty(EE_MODEL);
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
  eeDirty(EE_MODEL);
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
  eeDirty(EE_MODEL);
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

#define CURVE_ROWS 1

void menuModelExpoOne(evt_t event)
{
  ExpoData *ed = expoAddress(s_currIdx);

  SUBMENU(STR_MENUINPUTS, EXPO_FIELD_MAX, EXPO_ONE_2ND_COLUMN+85, { 0, 0, 0, (ed->srcRaw >= MIXSRC_FIRST_TELEM ? (uint8_t)0 : (uint8_t)HIDDEN_ROW), 0, 0, CASE_CURVES(CURVE_ROWS) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0 /*, ...*/});

  putsMixerSource(60, MENU_FOOTER_TOP, MIXSRC_FIRST_INPUT+ed->chn, HEADER_COLOR);

  int8_t sub = m_posVert;

  coord_t y = MENU_CONTENT_TOP;

  DrawFunction(expoFn);

  {
    int x = getValue(ed->srcRaw);
    if (ed->srcRaw >= MIXSRC_FIRST_TELEM) {
      putsTelemetryChannelValue(LCD_W-8, 6*FH, ed->srcRaw - MIXSRC_FIRST_TELEM, x);
      if (ed->scale > 0) x = (x * 1024) / convertTelemValue(ed->srcRaw - MIXSRC_FIRST_TELEM + 1, ed->scale);
    }
    else {
      lcd_outdezAtt(X0+WCHART, MENU_HEADER_HEIGHT+6*FH, calcRESXto1000(x), PREC1);
    }

    x = limit(-1024, x, 1024);
    int y = limit<int>(-1024, expoFn(x), 1024);
    lcd_outdezAtt(X0-WCHART, MENU_HEADER_HEIGHT+1, calcRESXto1000(y), LEFT|PREC1);

    x = x / (RESX/WCHART);
    y = getYCoord(expoFn, x);

    lcd_vlineStip(X0+x, y-3, 7, SOLID, TEXT_INVERTED_BGCOLOR);
    lcd_hlineStip(X0+x-3, y, 7, SOLID, TEXT_INVERTED_BGCOLOR);
  }

  for (unsigned int k=0; k<NUM_BODY_LINES; k++) {
    int i = k + s_pgOfs;
    for (int j=0; j<=i; ++j) {
      if (j<(int)DIM(mstate_tab) && mstate_tab[j] == HIDDEN_ROW) {
        ++i;
      }
    }
    LcdFlags attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i)
    {
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
        lcd_putsiAtt(EXPO_ONE_2ND_COLUMN, y, STR_VMIXTRIMS, (not_stick && carryTrim == 0) ? 0 : carryTrim+1, m_posHorz==0 ? attr : 0);
        if (attr) ed->carryTrim = -checkIncDecModel(event, carryTrim, not_stick ? TRIM_ON : -TRIM_OFF, -TRIM_AIL);
        break;
    }
    y += FH;
  }
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

#define GAUGE_WIDTH  33
#define GAUGE_HEIGHT 6
void drawOffsetBar(uint8_t x, uint8_t y, MixData * md)
{
  int offset = GET_GVAR(MD_OFFSET(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
  int weight = abs(GET_GVAR(MD_WEIGHT(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode));
  int barMin = offset - weight;
  int barMax = offset + weight;
  if (y > 15) {
    lcd_outdezAtt(x-((barMin >= 0) ? 2 : 3), y-6, barMin, TINSIZE|LEFT);
    lcd_outdezAtt(x+GAUGE_WIDTH+1, y-6, barMax, TINSIZE);
  }
  if (barMin < -101)
    barMin = -101;
  if (barMax > 101)
    barMax = 101;
  lcd_hlineStip(x-2, y, GAUGE_WIDTH+2, DOTTED);
  lcd_hlineStip(x-2, y+GAUGE_HEIGHT, GAUGE_WIDTH+2, DOTTED);
  lcd_vline(x-2, y+1, GAUGE_HEIGHT-1);
  lcd_vline(x+GAUGE_WIDTH-1, y+1, GAUGE_HEIGHT-1);
  if (barMin <= barMax) {
    int8_t right = (barMax * GAUGE_WIDTH) / 200;
    int8_t left = ((barMin * GAUGE_WIDTH) / 200)-1;
    // TODO lcdDrawFilledRect(x+GAUGE_WIDTH/2+left, y+2, right-left, GAUGE_HEIGHT-3);
  }
  lcd_vline(x+GAUGE_WIDTH/2-1, y, GAUGE_HEIGHT+1);
#if 0 // TODO
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
#endif
}
#undef GAUGE_WIDTH
#undef GAUGE_HEIGHT

void menuModelMixOne(evt_t event)
{
  MixData *md2 = mixAddress(s_currIdx) ;

  SUBMENU(s_currCh ? STR_INSERTMIX : STR_EDITMIX, MIX_FIELD_COUNT, 0, { 0, 0, 0, 0, 0, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0 /*, ...*/ });

  putsChn(80, MENU_FOOTER_TOP, md2->destCh+1, HEADER_COLOR);

  // The separation line between 2 columns
  lcd_vlineStip(MENU_COLUMN2_X, DEFAULT_SCROLLBAR_Y, DEFAULT_SCROLLBAR_H, SOLID, HEADER_COLOR);

  int8_t sub = m_posVert;
  int8_t editMode = s_editMode;

  for (int k=0; k<2*NUM_BODY_LINES; k++) {
    coord_t y;
    coord_t x;
    if (k >= NUM_BODY_LINES) {
      y = MENU_CONTENT_TOP + (k-NUM_BODY_LINES)*FH;
      x = MENU_COLUMN2_X;
    }
    else {
      y = MENU_CONTENT_TOP + k*FH;
      x = 0;
    }
    int8_t i = k;

    LcdFlags attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i) {
      case MIX_FIELD_NAME:
        editSingleName(x+MIXES_2ND_COLUMN, y, STR_MIXNAME, md2->name, sizeof(md2->name), event, attr);
        break;
      case MIX_FIELD_SOURCE:
        lcd_putsLeft(y, NO_INDENT(STR_SOURCE));
        putsMixerSource(x+MIXES_2ND_COLUMN, y, md2->srcRaw, STREXPANDED|attr);
        if (attr) CHECK_INCDEC_MODELSOURCE(event, md2->srcRaw, 1, MIXSRC_LAST);
        break;
      case MIX_FIELD_WEIGHT:
        lcd_putsColumnLeft(x, y, STR_WEIGHT);
        gvarWeightItem(x+MIXES_2ND_COLUMN, y, md2, attr|LEFT, event);
        break;
      case MIX_FIELD_OFFSET:
      {
        lcd_putsColumnLeft(x, y, NO_INDENT(STR_OFFSET));
        u_int8int16_t offset;
        MD_OFFSET_TO_UNION(md2, offset);
        offset.word = GVAR_MENU_ITEM(x+MIXES_2ND_COLUMN, y, offset.word, GV_RANGELARGE_OFFSET_NEG, GV_RANGELARGE_OFFSET, attr|LEFT, 0, event);
        MD_UNION_TO_OFFSET(offset, md2);
#if 0
        drawOffsetBar(x+MIXES_2ND_COLUMN+22, y, md2);
#endif
        break;
      }
      case MIX_FIELD_TRIM:
        lcd_putsColumnLeft(x, y, STR_TRIM);
        lcdDrawCheckBox(x+MIXES_2ND_COLUMN, y, !md2->carryTrim, attr);
        if (attr) md2->carryTrim = !checkIncDecModel(event, !md2->carryTrim, 0, 1);
        break;
#if defined(CURVES)
      case MIX_FIELD_CURVE:
      {
        lcd_putsColumnLeft(x, y, STR_CURVE);
        editCurveRef(x+MIXES_2ND_COLUMN, y, md2->curve, event, attr);
        break;
      }
#endif
#if defined(FLIGHT_MODES)
      case MIX_FIELD_FLIGHT_PHASE:
        md2->flightModes = editFlightModes(x+MIXES_2ND_COLUMN, y, event, md2->flightModes, attr);
        break;
#endif
      case MIX_FIELD_SWITCH:
        md2->swtch = switchMenuItem(x+MIXES_2ND_COLUMN, y, md2->swtch, attr, event);
        break;
#if 0
      case MIX_FIELD_WARNING:
        lcd_putsColumnLeft(x+MIXES_2ND_COLUMN, y, STR_MIXWARNING);
        if (md2->mixWarn)
          lcd_outdezAtt(x+MIXES_2ND_COLUMN, y, md2->mixWarn, attr|LEFT);
        else
          lcd_putsAtt(x+MIXES_2ND_COLUMN, y, STR_OFF, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, md2->mixWarn, 3);
        break;
#endif
      case MIX_FIELD_MLTPX:
        md2->mltpx = selectMenuItem(x+MIXES_2ND_COLUMN, y, STR_MULTPX, STR_VMLTPX, md2->mltpx, 0, 2, attr, event);
        break;
      case MIX_FIELD_DELAY_UP:
        md2->delayUp = EDIT_DELAY(x, y, event, attr, STR_DELAYUP, md2->delayUp);
        break;
      case MIX_FIELD_DELAY_DOWN:
        md2->delayDown = EDIT_DELAY(x, y, event, attr, STR_DELAYDOWN, md2->delayDown);
        break;
      case MIX_FIELD_SLOW_UP:
        md2->speedUp = EDIT_DELAY(x, y, event, attr, STR_SLOWUP, md2->speedUp);
        break;
      case MIX_FIELD_SLOW_DOWN:
        md2->speedDown = EDIT_DELAY(x, y, event, attr, STR_SLOWDOWN, md2->speedDown);
        break;
    }
  }
}

static uint8_t s_copySrcIdx;
static uint8_t s_copySrcCh;

#define _STR_MAX(x) PSTR("/" #x)
#define STR_MAX(x) _STR_MAX(x)

#define EXPO_LINE_WEIGHT_POS    89
#define EXPO_LINE_SRC_POS       102
#define EXPO_LINE_FM_POS        138
#define EXPO_LINE_CURVE_POS     138
#define EXPO_LINE_SWITCH_POS    182
#define EXPO_LINE_SIDE_POS      215
#define EXPO_LINE_SELECT_POS    61
#define EXPO_LINE_SELECT_WIDTH  244
#define EXPO_LINE_NAME_POS      238

#define MIX_LINE_WEIGHT_POS     69
#define MIX_LINE_SRC_POS        83
#define MIX_LINE_FM_POS         142
#define MIX_LINE_CURVE_POS      142
#define MIX_LINE_SWITCH_POS     190
#define MIX_LINE_DELAY_POS      225
#define MIX_LINE_SELECT_POS     40
#define MIX_LINE_SELECT_WIDTH   265

void lineSurround(bool expo, coord_t y, LcdFlags flags=CURVE_AXIS_COLOR)
{
  lcd_rect(expo ? EXPO_LINE_SELECT_POS : MIX_LINE_SELECT_POS, y-INVERT_VERT_MARGIN, expo ? EXPO_LINE_SELECT_WIDTH : MIX_LINE_SELECT_WIDTH, INVERT_LINE_HEIGHT, WARNING_COLOR|(s_copyMode == COPY_MODE ? SOLID : DOTTED), flags);
}

void onExpoMixMenu(const char *result)
{
  bool expo = (g_menuStack[g_menuStackPtr] == menuModelExposAll);
  uint8_t chn = (expo ? expoAddress(s_currIdx)->chn+1 : mixAddress(s_currIdx)->destCh+1);

  if (result == STR_EDIT) {
    pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
  }
  else if (result == STR_INSERT_BEFORE || result == STR_INSERT_AFTER) {
    if (!reachExpoMixCountLimit(expo)) {
      s_currCh = chn;
      if (result == STR_INSERT_AFTER) { s_currIdx++; m_posVert++; }
      insertExpoMix(expo, s_currIdx);
      pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
    }
  }
  else if (result == STR_COPY || result == STR_MOVE) {
    s_copyMode = (result == STR_COPY ? COPY_MODE : MOVE_MODE);
    s_copySrcIdx = s_currIdx;
    s_copySrcCh = chn;
    s_copySrcRow = m_posVert;
  }
  else if (result == STR_DELETE) {
    deleteExpoMix(expo, s_currIdx);
  }
}

void displayHeaderChannelName(uint8_t ch)
{
  uint8_t len = zlen(g_model.limitData[ch-1].name, sizeof(g_model.limitData[ch-1].name));
  if (len) {
    lcd_putsnAtt(COLUMN_HEADER_X, MENU_FOOTER_TOP, g_model.limitData[ch-1].name, len, HEADER_COLOR|ZCHAR);
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
    lcd_putsnAtt(EXPO_LINE_NAME_POS, y, md->name, sizeof(md->name), ZCHAR);
  if (!md->flightModes || ((md->curve.value || md->swtch) && ((get_tmr10ms() / 200) & 1)))
    displayMixInfos(y, md);
  else
    displayFlightModes(MIX_LINE_FM_POS, y, md->flightModes, 0);
}

void displayExpoInfos(coord_t y, ExpoData *ed)
{
  putsCurveRef(EXPO_LINE_CURVE_POS, y, ed->curve);
  if (ed->swtch) {
    putsSwitches(EXPO_LINE_SWITCH_POS, y, ed->swtch);
  }
  if (ed->mode != 3) {
    lcd_puts(EXPO_LINE_SIDE_POS, y, ed->mode == 2 ? "\176" : "\177");
  }
}

void displayExpoLine(coord_t y, ExpoData *ed)
{
  putsMixerSource(EXPO_LINE_SRC_POS, y, ed->srcRaw);

  if (!ed->flightModes || ((ed->curve.value || ed->swtch) && ((get_tmr10ms() / 200) & 1)))
    displayExpoInfos(y, ed);
  else
    displayFlightModes(EXPO_LINE_FM_POS, y, ed->flightModes, 0);

  if (ed->name[0]) {
    lcd_putsnAtt(EXPO_LINE_NAME_POS, y, ed->name, sizeof(ed->name), ZCHAR);
  }
}

void menuModelExpoMix(uint8_t expo, evt_t event)
{
  int sub = m_posVert;

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
            eeDirty(EE_MODEL);
          }
          m_posVert = s_copySrcRow;
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

    CASE_EVT_ROTARY_BREAK
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
            MENU_ADD_ITEM(STR_EDIT);
            MENU_ADD_ITEM(STR_INSERT_BEFORE);
            MENU_ADD_ITEM(STR_INSERT_AFTER);
            MENU_ADD_ITEM(STR_COPY);
            MENU_ADD_ITEM(STR_MOVE);
            MENU_ADD_ITEM(STR_DELETE);
            menuHandler = onExpoMixMenu;
          }
        }
      }
      break;
    case EVT_KEY_LONG(KEY_LEFT):
    case EVT_KEY_LONG(KEY_RIGHT):
      if (s_copyMode && !s_copyTgtOfs) {
        if (reachExpoMixCountLimit(expo)) break;
        s_currCh = chn;
        if (event == EVT_KEY_LONG(KEY_RIGHT)) { s_currIdx++; m_posVert++; }
        insertExpoMix(expo, s_currIdx);
        pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
        s_copyMode = 0;
        killEvents(event);
      }
      break;
    case EVT_KEY_FIRST(KEY_MOVE_UP):
    case EVT_KEY_REPT(KEY_MOVE_UP):
    case EVT_KEY_FIRST(KEY_MOVE_DOWN):
    case EVT_KEY_REPT(KEY_MOVE_DOWN):
      if (s_copyMode) {
        uint8_t key = (event & 0x1f);
        uint8_t next_ofs = ((IS_ROTARY_UP(event) || key==KEY_MOVE_UP) ? s_copyTgtOfs - 1 : s_copyTgtOfs + 1);

        if (s_copyTgtOfs==0 && s_copyMode==COPY_MODE) {
          // insert a mix on the same channel (just above / just below)
          if (reachExpoMixCountLimit(expo)) break;
          copyExpoMix(expo, s_currIdx);
          if (IS_ROTARY_DOWN(event) || key==KEY_MOVE_DOWN) s_currIdx++;
          else if (sub-s_pgOfs >= 6) s_pgOfs++;
        }
        else if (next_ofs==0 && s_copyMode==COPY_MODE) {
          // delete the mix
          deleteExpoMix(expo, s_currIdx);
          if (IS_ROTARY_UP(event) || key==KEY_MOVE_UP) s_currIdx--;
        }
        else {
          // only swap the mix with its neighbor
          if (!swapExpoMix(expo, s_currIdx, IS_ROTARY_UP(event) || key==KEY_MOVE_UP)) break;
          eeDirty(EE_MODEL);
        }

        s_copyTgtOfs = next_ofs;
      }
      break;
  }

  int linesCount = getLinesCount(expo);
  SIMPLE_MENU(expo ? STR_MENUINPUTS : STR_MIXER, menuTabModel, expo ? e_InputsAll : e_MixAll, linesCount, DEFAULT_SCROLLBAR_X);

  char str[6];
  sprintf(str, "%d/%d", getExpoMixCount(expo), expo ? MAX_EXPOS : MAX_MIXERS);
  lcd_putsAtt(60, MENU_FOOTER_TOP, str, HEADER_COLOR);

  sub = m_posVert;
  s_currCh = 0;
  int cur = 0;
  int i = 0;

  for (int ch=1; ch<=(expo ? NUM_INPUTS : NUM_CHNOUT); ch++) {
    void *pointer = NULL; MixData * &md = (MixData * &)pointer; ExpoData * &ed = (ExpoData * &)pointer;
    coord_t y = MENU_CONTENT_TOP + (cur-s_pgOfs)*FH;
    if (expo ? (i<MAX_EXPOS && (ed=expoAddress(i))->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && (md=mixAddress(i))->srcRaw && md->destCh+1 == ch)) {
      if (cur-s_pgOfs >= 0 && cur-s_pgOfs < NUM_BODY_LINES) {
        if (expo) {
          putsMixerSource(MENU_TITLE_LEFT, y, ch);
        }
        else {
          putsChn(MENU_TITLE_LEFT, y, ch, 0); // show CHx
        }
      }
      uint8_t mixCnt = 0;
      do {
        if (s_copyMode) {
          if (s_copyMode == MOVE_MODE && cur-s_pgOfs >= 0 && cur-s_pgOfs < NUM_BODY_LINES && s_copySrcCh == ch && s_copyTgtOfs != 0 && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
            lineSurround(expo, y);
            cur++; y+=FH;
          }
          if (s_currIdx == i) {
            sub = m_posVert = cur;
            s_currCh = ch;
          }
        }
        else if (sub == cur) {
          s_currIdx = i;
        }
        if (cur-s_pgOfs >= 0 && cur-s_pgOfs < NUM_BODY_LINES) {
          LcdFlags attr = ((s_copyMode || sub != cur) ? 0 : INVERS);
          if (expo) {
            ed->weight = GVAR_MENU_ITEM(EXPO_LINE_WEIGHT_POS, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr | (isExpoActive(i) ? BOLD : 0), 0, 0);
            displayExpoLine(y, ed);
          }
          else {
            if (attr) {
              displayHeaderChannelName(ch);
            }

            if (mixCnt > 0) lcd_putsiAtt(6, y, STR_VMLTPX2, md->mltpx, 0);

            putsMixerSource(MIX_LINE_SRC_POS, y, md->srcRaw);

            gvarWeightItem(MIX_LINE_WEIGHT_POS, y, md, attr | (isMixActive(i) ? BOLD : 0), event);

            displayMixLine(y, md);

            char cs[] = " ";
            if (md->speedDown || md->speedUp)
              cs[0] = 'S';
            if (md->delayUp || md->delayDown)
              cs[0] = (cs[0] =='S' ? '*' : 'D');
            lcd_puts(MIX_LINE_DELAY_POS, y, cs);
          }
          if (s_copyMode) {
            if ((s_copyMode==COPY_MODE || s_copyTgtOfs == 0) && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
              /* draw a border around the raw on selection mode (copy/move) */
              lineSurround(expo, y);
            }
            if (cur == sub) {
              /* invert the raw when it's the current one */
              lineSurround(expo, y, WARNING_COLOR);
            }
          }
        }
        cur++; y+=FH; mixCnt++; i++; if (expo) ed++; else md++;
      } while (expo ? (i<MAX_EXPOS && ed->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && md->srcRaw && md->destCh+1 == ch));
      if (s_copyMode == MOVE_MODE && cur-s_pgOfs >= 0 && cur-s_pgOfs < NUM_BODY_LINES && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
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
      if (cur-s_pgOfs >= 0 && cur-s_pgOfs < NUM_BODY_LINES) {
        if (expo) {
          putsMixerSource(MENU_TITLE_LEFT, y, ch, attr);
        }
        else {
          putsChn(MENU_TITLE_LEFT, y, ch, attr); // show CHx
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

  if (sub >= linesCount-1) m_posVert = linesCount-1;
}

void menuModelExposAll(evt_t event)
{
  return menuModelExpoMix(1, event);
}

void menuModelMixAll(evt_t event)
{
  return menuModelExpoMix(0, event);
}
