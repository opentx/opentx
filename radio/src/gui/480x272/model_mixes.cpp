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
#include <stdio.h>

void displayMixStatus(uint8_t channel);

int getMixesLinesCount()
{
  int lastch = -1;
  uint8_t count = MAX_OUTPUT_CHANNELS;
  for (int i=0; i<MAX_MIXERS; i++) {
    bool valid = mixAddress(i)->srcRaw;
    if (!valid)
      break;
    int ch = mixAddress(i)->destCh;
    if (ch == lastch) {
      count++;
    }
    else {
      lastch = ch;
    }
  }
  return count;
}

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
    if (x->destCh == MAX_OUTPUT_CHANNELS-1)
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
      if (destCh<MAX_OUTPUT_CHANNELS-1) x->destCh++;
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

void gvarWeightItem(coord_t x, coord_t y, MixData *md, LcdFlags attr, event_t event)
{
  u_int8int16_t weight;
  MD_WEIGHT_TO_UNION(md, weight);
  weight.word = GVAR_MENU_ITEM(x, y, weight.word, GV_RANGELARGE_WEIGHT_NEG, GV_RANGELARGE_WEIGHT, attr, 0, event);
  MD_UNION_TO_WEIGHT(weight, md);
}

bool menuModelMixOne(event_t event)
{
  MixData * md2 = mixAddress(s_currIdx) ;

  SUBMENU_WITH_OPTIONS(STR_MIXER, ICON_MODEL_MIXER, MIX_FIELD_COUNT, OPTION_MENU_NO_SCROLLBAR, { 0, 0, 0, 0, 0, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0 /*, ...*/ });
  putsChn(50, 3+FH, md2->destCh+1, MENU_TITLE_COLOR);
  displayMixStatus(md2->destCh);

  // The separation line between 2 columns
  lcdDrawSolidVerticalLine(MENU_COLUMN2_X-10, DEFAULT_SCROLLBAR_Y-FH, DEFAULT_SCROLLBAR_H+5, TEXT_COLOR);

  int8_t sub = menuVerticalPosition;
  int8_t editMode = s_editMode;

  for (int k=0; k<2*NUM_BODY_LINES; k++) {
    coord_t y;
    if (k > NUM_BODY_LINES) {
      y = MENU_CONTENT_TOP - FH + (k-NUM_BODY_LINES)*FH;
    }
    else {
      y = MENU_CONTENT_TOP - FH + k*FH;
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
        drawSource(MIXES_2ND_COLUMN, y, md2->srcRaw, attr);
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
      case MIX_FIELD_FLIGHT_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_FLMODE);
        md2->flightModes = editFlightModes(MIXES_2ND_COLUMN, y, event, md2->flightModes, attr);
        break;
#endif
      case MIX_FIELD_SWITCH:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SWITCH);
        md2->swtch = editSwitch(MIXES_2ND_COLUMN, y, md2->swtch, attr, event);
        break;
      case MIX_FIELD_WARNING:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MIXWARNING);
        if (md2->mixWarn) {
          lcdDrawNumber(MIXES_2ND_COLUMN, y, md2->mixWarn, attr|LEFT);
        }
        else {
          lcdDrawText(MIXES_2ND_COLUMN, y, STR_OFF, attr);
        }
        if (attr) {
          CHECK_INCDEC_MODELVAR_ZERO(event, md2->mixWarn, 3);
        }
        break;
      case MIX_FIELD_MLTPX:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTPX);
        md2->mltpx = editChoice(MIXES_2ND_COLUMN, y, STR_VMLTPX, md2->mltpx, 0, 2, attr, event);
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

#define _STR_MAX(x) PSTR("/" #x)
#define STR_MAX(x) _STR_MAX(x)

#define MIX_LINE_WEIGHT_POS     105
#define MIX_LINE_SRC_POS        120
#define MIX_LINE_CURVE_ICON     175
#define MIX_LINE_CURVE_POS      195
#define MIX_LINE_SWITCH_ICON    260
#define MIX_LINE_SWITCH_POS     280
#define MIX_LINE_DELAY_SLOW_POS 340
#define MIX_LINE_NAME_FM_ICON   370
#define MIX_LINE_NAME_FM_POS    390
#define MIX_LINE_SELECT_POS     50
#define MIX_LINE_SELECT_WIDTH   (LCD_W-MIX_LINE_SELECT_POS-15)
#define MIX_STATUS_BAR_W        130
#define MIX_STATUS_BAR_H        13
#define MIX_STATUS_CHAN_BAR     MENUS_MARGIN_LEFT + 45
#define MIX_STATUS_ICON_MIXER   MIX_STATUS_CHAN_BAR + 140
#define MIX_STATUS_ICON_TO      MIX_STATUS_ICON_MIXER + 20
#define MIX_STATUS_ICON_OUTPUT  MIX_STATUS_ICON_TO + 35
#define MIX_STATUS_OUT_NAME     MIX_STATUS_ICON_OUTPUT + 25
#define MIX_STATUS_OUT_BAR      LCD_W - MENUS_MARGIN_LEFT - MIX_STATUS_BAR_W

void lineMixSurround(coord_t y, LcdFlags flags=CURVE_AXIS_COLOR)
{
  lcdDrawRect(MIX_LINE_SELECT_POS, y-INVERT_VERT_MARGIN+1, MIX_LINE_SELECT_WIDTH, INVERT_LINE_HEIGHT, 1, s_copyMode == COPY_MODE ? SOLID : DOTTED, flags);
}

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

void displayMixInfos(coord_t y, MixData *md)
{
  if (md->curve.value != 0 ) lcd->drawBitmap(MIX_LINE_CURVE_ICON, y + 2, mixerSetupCurveBitmap);
  drawCurveRef(MIX_LINE_CURVE_POS, y, md->curve);

  if (md->swtch) {
    lcd->drawBitmap(MIX_LINE_SWITCH_ICON, y + 2, mixerSetupSwitchBitmap);
    drawSwitch(MIX_LINE_SWITCH_POS, y, md->swtch);
  }
}

void displayMixSmallFlightModes(coord_t x, coord_t y, FlightModesType value)
{
  for (int i=0; i<MAX_FLIGHT_MODES; i++) {
    char s[] = " ";
    s[0] = '0' + i;
    if (value & (1<<i)) lcd->drawFilledRect(x, y+2, 8, 12 , SOLID, CURVE_AXIS_COLOR);
    lcdDrawText(x, y, s, SMLSIZE);
    x += 8;
  }
}

void displayMixLine(coord_t y, MixData *md)
{
  if (md->name[0] && md->flightModes) {
    if (SLOW_BLINK_ON_PHASE) {
      lcd->drawBitmap(MIX_LINE_NAME_FM_ICON, y + 2, mixerSetupFlightmodeBitmap);
      displayMixSmallFlightModes(MIX_LINE_NAME_FM_POS, y + 2, md->flightModes);
    }
    else {
      lcd->drawBitmap(MIX_LINE_NAME_FM_ICON, y + 2, mixerSetupLabelBitmap);
      lcdDrawSizedText(MIX_LINE_NAME_FM_POS, y, md->name, sizeof(md->name), ZCHAR);
    }
  }
  else if (md->name[0]) {
    lcd->drawBitmap(MIX_LINE_NAME_FM_ICON, y + 2, mixerSetupLabelBitmap);
    lcdDrawSizedText(MIX_LINE_NAME_FM_POS, y, md->name, sizeof(md->name), ZCHAR);
  }
  else if (md->flightModes) {
    lcd->drawBitmap(MIX_LINE_NAME_FM_ICON, y + 2, mixerSetupFlightmodeBitmap);
    displayMixSmallFlightModes(MIX_LINE_NAME_FM_POS, y + 2, md->flightModes);
  }
  displayMixInfos(y, md);
}

void displayMixStatus(uint8_t channel)
{
  lcdDrawNumber(MENUS_MARGIN_LEFT, MENU_FOOTER_TOP, channel + 1, MENU_TITLE_COLOR, 0, "CH", NULL);
  drawSingleMixerBar(MIX_STATUS_CHAN_BAR, MENU_FOOTER_TOP + 4, MIX_STATUS_BAR_W, MIX_STATUS_BAR_H, channel);

  lcd->drawBitmap(MIX_STATUS_ICON_MIXER, MENU_FOOTER_TOP, mixerSetupMixerBitmap);
  lcd->drawBitmap(MIX_STATUS_ICON_TO, MENU_FOOTER_TOP, mixerSetupToBitmap);
  lcd->drawBitmap(MIX_STATUS_ICON_OUTPUT, MENU_FOOTER_TOP, mixerSetupOutputBitmap);

  if (g_model.limitData[channel].name[0] == '\0')
    lcdDrawNumber(MIX_STATUS_OUT_NAME, MENU_FOOTER_TOP, channel + 1, MENU_TITLE_COLOR, 0, "CH", NULL);
  else
    lcdDrawSizedText(MIX_STATUS_OUT_NAME, MENU_FOOTER_TOP, g_model.limitData[channel].name, sizeof(g_model.limitData[channel].name), MENU_TITLE_COLOR | LEFT | ZCHAR);
  drawSingleOutputBar(MIX_STATUS_OUT_BAR, MENU_FOOTER_TOP + 4, MIX_STATUS_BAR_W, MIX_STATUS_BAR_H, channel);
}

bool menuModelMixAll(event_t event)
{
  const BitmapBuffer * mpx_mode[] = {
    mixerSetupAddBitmap,
    mixerSetupMultiBitmap,
    mixerSetupReplaceBitmap
  };


  uint8_t sub = menuVerticalPosition;

  if (s_editMode > 0) {
    s_editMode = 0;
  }

  uint8_t chn = mixAddress(s_currIdx)->destCh + 1;

  int linesCount = getMixesLinesCount();
  SIMPLE_MENU(STR_MIXER, MODEL_ICONS, menuTabModel, MENU_MODEL_MIXES, linesCount);

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
        return true;
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
            return true;
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

    case EVT_ROTARY_RIGHT:
    case EVT_ROTARY_LEFT:
      if (s_copyMode) {
        uint8_t next_ofs = (event==EVT_ROTARY_LEFT ? s_copyTgtOfs - 1 : s_copyTgtOfs + 1);

        if (s_copyTgtOfs==0 && s_copyMode==COPY_MODE) {
          // insert a mix on the same channel (just above / just below)
          if (reachMixesLimit()) break;
          copyMix(s_currIdx);
          if (event==EVT_ROTARY_RIGHT) s_currIdx++;
          else if (sub-menuVerticalOffset >= 6) menuVerticalOffset++;
        }
        else if (next_ofs==0 && s_copyMode==COPY_MODE) {
          // delete the mix
          deleteMix(s_currIdx);
          if (event==EVT_ROTARY_LEFT) s_currIdx--;
        }
        else {
          // only swap the mix with its neighbor
          if (!swapMixes(s_currIdx, event==EVT_ROTARY_LEFT)) break;
          storageDirty(EE_MODEL);
        }

        s_copyTgtOfs = next_ofs;
      }
      break;
  }

  char str[6];
  strAppendUnsigned(strAppend(strAppendUnsigned(str, getMixesCount()), "/"), MAX_MIXERS, 2);
  lcdDrawText(MENU_TITLE_NEXT_POS, MENU_TITLE_TOP+1, str, HEADER_COLOR);

  sub = menuVerticalPosition;
  s_currCh = 0;
  int cur = 0;
  int i = 0;

  for (int ch=1; ch<=MAX_OUTPUT_CHANNELS; ch++) {
    MixData * md;
    coord_t y = MENU_CONTENT_TOP + (cur-menuVerticalOffset)*FH;
    if (i<MAX_MIXERS && (md=mixAddress(i))->srcRaw && md->destCh+1 == ch) {
      if (cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES) {
        putsChn(MENUS_MARGIN_LEFT, y, ch, 0); // show CHx
      }
      uint8_t mixCnt = 0;
      do {
        if (s_copyMode) {
          if (s_copyMode == MOVE_MODE && cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES && s_copySrcCh == ch && s_copyTgtOfs != 0 && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
            lineMixSurround(y);
            cur++; y+=FH;
          }
          if (s_currIdx == i) {
            sub = menuVerticalPosition = cur;
            s_currCh = ch;
          }
        }
        else if (sub == cur) {
          s_currIdx = i;
          displayMixStatus(ch - 1);
        }
        if (cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES) {
          LcdFlags attr = ((s_copyMode || sub != cur) ? 0 : INVERS);

          if (mixCnt > 0) {
            lcd->drawBitmap(10, y, mpx_mode[md->mltpx]);
          }

          drawSource(MIX_LINE_SRC_POS, y, md->srcRaw);

          if (mixCnt == 0 && md->mltpx == 1)
            lcdDrawText(MIX_LINE_WEIGHT_POS, y, "MULT!", RIGHT | attr | (isMixActive(i) ? BOLD : 0));
          else
            gvarWeightItem(MIX_LINE_WEIGHT_POS, y, md, RIGHT | attr | (isMixActive(i) ? BOLD : 0), event);

          displayMixLine(y, md);

          BitmapBuffer *delayslowbmp[] = {mixerSetupSlowBitmap, mixerSetupDelayBitmap, mixerSetupDelaySlowBitmap};
          uint8_t delayslow = 0;
          if (md->speedDown || md->speedUp)
            delayslow = 1;
          if (md->delayUp || md->delayDown)
            delayslow += 2;
          if (delayslow)
            lcd->drawBitmap(MIX_LINE_DELAY_SLOW_POS, y + 2, delayslowbmp[delayslow - 1]);

          if (s_copyMode) {
            if ((s_copyMode==COPY_MODE || s_copyTgtOfs == 0) && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
              /* draw a border around the raw on selection mode (copy/move) */
              lineMixSurround(y);
            }
            if (cur == sub) {
              /* invert the raw when it's the current one */
              lineMixSurround(y, ALARM_COLOR);
            }
          }
        }
        cur++; y+=FH; mixCnt++; i++; md++;
      } while (i<MAX_MIXERS && md->srcRaw && md->destCh+1 == ch);
      if (s_copyMode == MOVE_MODE && cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
        lineMixSurround(y);
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
        putsChn(MENUS_MARGIN_LEFT, y, ch, attr); // show CHx
        if (s_copyMode == MOVE_MODE && s_copySrcCh == ch) {
          lineMixSurround(y);
        }
      }
      cur++; y+=FH;
    }
  }

  if (sub >= linesCount-1) menuVerticalPosition = linesCount-1;

  return true;
}
