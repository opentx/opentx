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

uint8_t getMixesCount()
{
  uint8_t count = 0;
  uint8_t ch ;

  for (int i=MAX_MIXERS-1; i>=0; i--) {
    ch = mixAddress(i)->srcRaw;
    if (ch != 0) {
      count++;
    }
  }
  return count;
}

bool reachMixesLimit()
{
  if (getMixesCount() >= MAX_MIXERS) {
    POPUP_WARNING(STR_NOFREEMIXER);
    return true;
  }
  return false;
}

void deleteMix(uint8_t idx)
{
  pauseMixerCalculations();
  MixData * mix = mixAddress(idx);
  memmove(mix, mix+1, (MAX_MIXERS-(idx+1))*sizeof(MixData));
  memclear(&g_model.mixData[MAX_MIXERS-1], sizeof(MixData));
  resumeMixerCalculations();
  storageDirty(EE_MODEL);
}

void insertMix(uint8_t idx)
{
  pauseMixerCalculations();
  MixData * mix = mixAddress(idx);
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
  resumeMixerCalculations();
  storageDirty(EE_MODEL);
}

void copyMix(uint8_t idx)
{
  pauseMixerCalculations();
  MixData * mix = mixAddress(idx);
  memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
  resumeMixerCalculations();
  storageDirty(EE_MODEL);
}

bool swapMixes(uint8_t & idx, uint8_t up)
{
  MixData * x, * y;
  int8_t tgt_idx = (up ? idx-1 : idx+1);

  x = mixAddress(idx);

  if (tgt_idx < 0) {
    if (x->destCh == 0)
      return false;
    x->destCh--;
    return true;
  }

  if (tgt_idx == MAX_MIXERS) {
    if (x->destCh == NUM_CHNOUT-1)
      return false;
    x->destCh++;
    return true;
  }

  y = mixAddress(tgt_idx);
  uint8_t destCh = x->destCh;
  if(!y->srcRaw || destCh != y->destCh) {
    if (up) {
      if (destCh>0) x->destCh--;
      else return false;
    }
    else {
      if (destCh<NUM_CHNOUT-1) x->destCh++;
      else return false;
    }
    return true;
  }

  pauseMixerCalculations();
  memswap(x, y, sizeof(MixData));
  resumeMixerCalculations();

  idx = tgt_idx;
  return true;
}

enum MixFields {
  MIX_FIELD_NAME,
  MIX_FIELD_SOURCE,
  MIX_FIELD_WEIGHT,
  MIX_FIELD_OFFSET,
  MIX_FIELD_TRIM,
  CASE_CURVES(MIX_FIELD_CURVE)
  CASE_FLIGHT_MODES(MIX_FIELD_FLIGHT_MODE)
  MIX_FIELD_SWITCH,
  MIX_FIELD_WARNING,
  MIX_FIELD_MLTPX,
  MIX_FIELD_DELAY_UP,
  MIX_FIELD_DELAY_DOWN,
  MIX_FIELD_SLOW_UP,
  MIX_FIELD_SLOW_DOWN,
  MIX_FIELD_COUNT
};

void gvarWeightItem(coord_t x, coord_t y, MixData *md, uint8_t attr, uint8_t event)
{
  u_int8int16_t weight;
  MD_WEIGHT_TO_UNION(md, weight);
  weight.word = GVAR_MENU_ITEM(x, y, weight.word, GV_RANGELARGE_WEIGHT_NEG, GV_RANGELARGE_WEIGHT, attr, 0, event);
  MD_UNION_TO_WEIGHT(weight, md);
}

void drawOffsetBar(uint8_t x, uint8_t y, MixData * md)
{
  const int gaugeWidth = 33;
  const int gaugeHeight = 6;

  int offset = GET_GVAR(MD_OFFSET(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
  int weight = GET_GVAR(MD_WEIGHT(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
  int barMin = offset - weight;
  int barMax = offset + weight;
  if (y > 15) {
    lcdDrawNumber(x-((barMin >= 0) ? 2 : 3), y-6, barMin, TINSIZE|LEFT);
    lcdDrawNumber(x+gaugeWidth+1, y-6, barMax, TINSIZE|RIGHT);
  }
  if (weight < 0) {
    barMin = -barMin;
    barMax = -barMax;
  }
  if (barMin < -101)
    barMin = -101;
  if (barMax > 101)
    barMax = 101;
  lcdDrawHorizontalLine(x-2, y, gaugeWidth+2, DOTTED);
  lcdDrawHorizontalLine(x-2, y+gaugeHeight, gaugeWidth+2, DOTTED);
  lcdDrawSolidVerticalLine(x-2, y+1, gaugeHeight-1);
  lcdDrawSolidVerticalLine(x+gaugeWidth-1, y+1, gaugeHeight-1);
  if (barMin <= barMax) {
    int8_t right = (barMax * gaugeWidth) / 200;
    int8_t left = ((barMin * gaugeWidth) / 200)-1;
    lcdDrawFilledRect(x+gaugeWidth/2+left, y+2, right-left, gaugeHeight-3);
  }
  lcdDrawSolidVerticalLine(x+gaugeWidth/2-1, y, gaugeHeight+1);
  if (barMin == -101) {
    for (uint8_t i=0; i<3; ++i) {
      lcdDrawPoint(x+i, y+4-i);
      lcdDrawPoint(x+3+i, y+4-i);
    }
  }
  if (barMax == 101) {
    for (uint8_t i=0; i<3; ++i) {
      lcdDrawPoint(x+gaugeWidth-8+i, y+4-i);
      lcdDrawPoint(x+gaugeWidth-5+i, y+4-i);
    }
  }
}

void menuModelMixOne(uint8_t event)
{
  if (event == EVT_KEY_LONG(KEY_MENU)) {
    pushMenu(menuChannelsView);
    killEvents(event);
  }

  MixData * md2 = mixAddress(s_currIdx) ;
  putsChn(PSIZE(TR_MIXER)*FW+FW, 0, md2->destCh+1,0);
  lcdDrawFilledRect(0, 0, LCD_W, FH, SOLID, FILL_WHITE|GREY_DEFAULT);

  SUBMENU(STR_MIXER, MIX_FIELD_COUNT, {0, 0, 0, 0, 0, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/});

#if MENU_COLUMNS > 1
  SET_SCROLLBAR_X(0);
#endif

  int8_t sub = menuVerticalPosition;
  int8_t editMode = s_editMode;

  for (int k=0; k<MENU_COLUMNS*(LCD_LINES-1); k++) {
    coord_t y;
    if (k >= LCD_LINES-1)
      y = 1 + (k-LCD_LINES+2)*FH;
    else
      y = 1 + (k+1)*FH;
    int8_t i = k;

#if MENU_COLUMNS < 2
    i = i + menuVerticalOffset;
#endif

    LcdFlags attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i) {
      case MIX_FIELD_NAME:
        editSingleName(MIXES_2ND_COLUMN, y, STR_MIXNAME, md2->name, sizeof(md2->name), event, attr);
        break;

      case MIX_FIELD_SOURCE:
        lcd_putsLeft(y, NO_INDENT(STR_SOURCE));
        putsMixerSource(MIXES_2ND_COLUMN, y, md2->srcRaw, STREXPANDED|attr);
        if (attr) CHECK_INCDEC_MODELSOURCE(event, md2->srcRaw, 1, MIXSRC_LAST);
        break;

      case MIX_FIELD_WEIGHT:
        lcd_putsLeft(y, STR_WEIGHT);
        gvarWeightItem(MIXES_2ND_COLUMN, y, md2, attr|LEFT, event);
        break;

      case MIX_FIELD_OFFSET:
      {
        lcd_putsLeft(y, NO_INDENT(STR_OFFSET));
        u_int8int16_t offset;
        MD_OFFSET_TO_UNION(md2, offset);
        offset.word = GVAR_MENU_ITEM(MIXES_2ND_COLUMN, y, offset.word, GV_RANGELARGE_OFFSET_NEG, GV_RANGELARGE_OFFSET, attr|LEFT, 0, event);
        MD_UNION_TO_OFFSET(offset, md2);
        drawOffsetBar(MIXES_2ND_COLUMN+22, y, md2);
        break;
      }

      case MIX_FIELD_TRIM:
        lcd_putsLeft(y, STR_TRIM);
        drawCheckBox(MIXES_2ND_COLUMN, y, !md2->carryTrim, attr);
        if (attr) md2->carryTrim = !checkIncDecModel(event, !md2->carryTrim, 0, 1);
        break;

#if defined(CURVES)
      case MIX_FIELD_CURVE:
        lcd_putsLeft(y, STR_CURVE);
        editCurveRef(MIXES_2ND_COLUMN, y, md2->curve, event, attr);
        break;
#endif

#if defined(FLIGHT_MODES)
      case MIX_FIELD_FLIGHT_MODE:
        md2->flightModes = editFlightModes(MIXES_2ND_COLUMN, y, event, md2->flightModes, attr);
        break;
#endif

      case MIX_FIELD_SWITCH:
        md2->swtch = switchMenuItem(MENU_COLUMN2_X+MIXES_2ND_COLUMN, y, md2->swtch, attr, event);
        break;

      case MIX_FIELD_WARNING:
        lcd_putsColumnLeft(MENU_COLUMN2_X+MIXES_2ND_COLUMN, y, STR_MIXWARNING);
        if (md2->mixWarn)
          lcdDrawNumber(MENU_COLUMN2_X+MIXES_2ND_COLUMN, y, md2->mixWarn, attr|LEFT);
        else
          lcdDrawText(MENU_COLUMN2_X+MIXES_2ND_COLUMN, y, STR_OFF, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, md2->mixWarn, 3);
        break;

      case MIX_FIELD_MLTPX:
        md2->mltpx = selectMenuItem(MENU_COLUMN2_X+MIXES_2ND_COLUMN, y, STR_MULTPX, STR_VMLTPX, md2->mltpx, 0, 2, attr, event);
        break;

      case MIX_FIELD_DELAY_UP:
        md2->delayUp = EDIT_DELAY(MENU_COLUMN2_X, y, event, attr, STR_DELAYUP, md2->delayUp);
        break;

      case MIX_FIELD_DELAY_DOWN:
        md2->delayDown = EDIT_DELAY(MENU_COLUMN2_X, y, event, attr, STR_DELAYDOWN, md2->delayDown);
        break;

      case MIX_FIELD_SLOW_UP:
        md2->speedUp = EDIT_DELAY(MENU_COLUMN2_X, y, event, attr, STR_SLOWUP, md2->speedUp);
        break;

      case MIX_FIELD_SLOW_DOWN:
        md2->speedDown = EDIT_DELAY(MENU_COLUMN2_X, y, event, attr, STR_SLOWDOWN, md2->speedDown);
        break;
    }
  }
}

#define _STR_MAX(x)                     PSTR("/" #x)
#define STR_MAX(x)                     _STR_MAX(x)

#define MIX_LINE_WEIGHT_POS            6*FW+10
#define MIX_LINE_SRC_POS               7*FW+5
#define MIX_LINE_CURVE_POS             13*FW+3
#define MIX_LINE_SWITCH_POS            19*FW+1
#define MIX_LINE_FM_POS                13*FW+3
#define MIX_LINE_DELAY_POS             24*FW+3
#define MIX_LINE_NAME_POS              LCD_W-LEN_EXPOMIX_NAME*FW-MENUS_SCROLLBAR_WIDTH

void onMixesMenu(const char * result)
{
  uint8_t chn = mixAddress(s_currIdx)->destCh + 1;

  if (result == STR_EDIT) {
    pushMenu(menuModelMixOne);
  }
  else if (result == STR_INSERT_BEFORE || result == STR_INSERT_AFTER) {
    if (!reachMixesLimit()) {
      s_currCh = chn;
      if (result == STR_INSERT_AFTER) { s_currIdx++; menuVerticalPosition++; }
      insertMix(s_currIdx);
      pushMenu(menuModelMixOne);
    }
  }
  else if (result == STR_COPY || result == STR_MOVE) {
    s_copyMode = (result == STR_COPY ? COPY_MODE : MOVE_MODE);
    s_copySrcIdx = s_currIdx;
    s_copySrcCh = chn;
    s_copySrcRow = menuVerticalPosition;
  }
  else if (result == STR_DELETE) {
    deleteMix(s_currIdx);
  }
}

void displayHeaderChannelName(uint8_t ch)
{
  uint8_t len = zlen(g_model.limitData[ch].name, sizeof(g_model.limitData[ch].name));
  if (len) {
    lcdDrawSizedText(80, 1, g_model.limitData[ch].name, len, ZCHAR|SMLSIZE);
  }
}

void displayMixInfos(coord_t y, MixData *md)
{
  putsCurveRef(MIX_LINE_CURVE_POS, y, md->curve, 0);

  if (md->swtch) {
    putsSwitches(MIX_LINE_SWITCH_POS, y, md->swtch);
  }
}

void displayMixLine(coord_t y, MixData *md)
{
  if (md->name[0])
    lcdDrawSizedText(MIX_LINE_NAME_POS, y, md->name, sizeof(md->name), ZCHAR);
  if (!md->flightModes || ((md->curve.value || md->swtch) && ((get_tmr10ms() / 200) & 1)))
    displayMixInfos(y, md);
  else
    displayFlightModes(MIX_LINE_FM_POS, y, md->flightModes);
}

void menuModelMixAll(uint8_t event)
{
  uint8_t sub = menuVerticalPosition;

  if (s_editMode > 0) {
    s_editMode = 0;
  }

  uint8_t chn = mixAddress(s_currIdx)->destCh + 1;

  switch (event) {
    case EVT_ENTRY:
    case EVT_ENTRY_UP:
      s_copyMode = 0;
      s_copyTgtOfs = 0;
      break;
    case EVT_KEY_LONG(KEY_EXIT):
      if (s_copyMode && s_copyTgtOfs == 0) {
        deleteMix(s_currIdx);
        killEvents(event);
        event = 0;
      }
      // no break
    case EVT_KEY_BREAK(KEY_EXIT):
      if (s_copyMode) {
        if (s_copyTgtOfs) {
          // cancel the current copy / move operation
          if (s_copyMode == COPY_MODE) {
            deleteMix(s_currIdx);
          }
          else {
            do {
              swapMixes(s_currIdx, s_copyTgtOfs > 0);
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
            pushMenu(menuModelMixOne);
          }
        }
        else {
          if (s_copyMode) s_currCh = 0;
          if (s_currCh) {
            if (reachMixesLimit()) break;
            insertMix(s_currIdx);
            pushMenu(menuModelMixOne);
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
            POPUP_MENU_START(onMixesMenu);
          }
        }
      }
      break;
    case EVT_KEY_LONG(KEY_LEFT):
    case EVT_KEY_LONG(KEY_RIGHT):
      if (s_copyMode && !s_copyTgtOfs) {
        if (reachMixesLimit()) break;
        s_currCh = chn;
        if (event == EVT_KEY_LONG(KEY_RIGHT)) { s_currIdx++; menuVerticalPosition++; }
        insertMix(s_currIdx);
        pushMenu(menuModelMixOne);
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
          if (reachMixesLimit()) break;
          copyMix(s_currIdx);
          if (key==KEY_DOWN) s_currIdx++;
          else if (sub-menuVerticalOffset >= 6) menuVerticalOffset++;
        }
        else if (next_ofs==0 && s_copyMode==COPY_MODE) {
          // delete the mix
          deleteMix(s_currIdx);
          if (key==KEY_UP) s_currIdx--;
        }
        else {
          // only swap the mix with its neighbor
          if (!swapMixes(s_currIdx, key==KEY_UP)) break;
          storageDirty(EE_MODEL);
        }

        s_copyTgtOfs = next_ofs;
      }
      break;
  }

  lcdDrawNumber(FW*sizeof(TR_MIXER)+FW+FW/2, 0, getMixesCount(), RIGHT);
  lcdDrawText(FW*sizeof(TR_MIXER)+FW+FW/2, 0, STR_MAX(MAX_MIXERS));

  // Value
  uint8_t index = mixAddress(s_currIdx)->destCh;
  if (!s_currCh) {
    displayHeaderChannelName(index);
    lcdDrawNumber(127, 2, calcRESXto1000(ex_chans[index]), PREC1|TINSIZE|RIGHT);
  }

  SIMPLE_MENU(STR_MIXER, menuTabModel, e_MixAll, s_maxLines);

  // Gauge
  if (!s_currCh) {
    drawGauge(127, 1, 58, 6, ex_chans[index], 1024);
  }

  sub = menuVerticalPosition;
  s_currCh = 0;
  int cur = 0;
  int i = 0;

  for (int ch=1; ch<=NUM_CHNOUT; ch++) {
    MixData * md;
    coord_t y = MENU_HEADER_HEIGHT+1+(cur-menuVerticalOffset)*FH;
    if (i<MAX_MIXERS && (md=mixAddress(i))->srcRaw && md->destCh+1 == ch) {
      if (cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES) {
        putsChn(0, y, ch, 0); // show CHx
      }
      uint8_t mixCnt = 0;
      do {
        if (s_copyMode) {
          if (s_copyMode == MOVE_MODE && cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES && s_copySrcCh == ch && s_copyTgtOfs != 0 && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
            lcdDrawRect(22, y-1, LCD_W-22, 9, DOTTED);
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

          if (mixCnt > 0) lcdDrawTextAtIndex(FW, y, STR_VMLTPX2, md->mltpx, 0);

          putsMixerSource(MIX_LINE_SRC_POS, y, md->srcRaw, 0);

          gvarWeightItem(MIX_LINE_WEIGHT_POS, y, md, RIGHT | attr | (isMixActive(i) ? BOLD : 0), 0);

          displayMixLine(y, md);

          char cs = ' ';
          if (md->speedDown || md->speedUp)
            cs = 'S';
          if (md->delayUp || md->delayDown)
            cs = (cs =='S' ? '*' : 'D');
          lcdDrawChar(MIX_LINE_DELAY_POS, y, cs);

          if (s_copyMode) {
            if ((s_copyMode==COPY_MODE || s_copyTgtOfs == 0) && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
              /* draw a border around the raw on selection mode (copy/move) */
              lcdDrawRect(22, y-1, LCD_W-22, 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
            }
            if (cur == sub) {
              /* invert the raw when it's the current one */
              lcdDrawFilledRect(23, y, LCD_W-24, 7);
            }
          }
        }
        cur++; y+=FH; mixCnt++; i++; md++;
      } while (i<MAX_MIXERS && md->srcRaw && md->destCh+1 == ch);
      if (s_copyMode == MOVE_MODE && cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
        lcdDrawRect(22, y-1, LCD_W-22, 9, DOTTED);
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
        putsChn(0, y, ch, attr); // show CHx
        if (s_copyMode == MOVE_MODE && s_copySrcCh == ch) {
          lcdDrawRect(22, y-1, LCD_W-22, 9, DOTTED);
        }
      }
      cur++;
    }
  }
  s_maxLines = cur;
  if (sub >= s_maxLines-1) menuVerticalPosition = s_maxLines-1;
}
