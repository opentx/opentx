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
#include <stdio.h>

int getMixesLinesCount()
{
  int lastch = -1;
  uint8_t count = NUM_CHNOUT;
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

bool menuModelMixOne(evt_t event)
{
  MixData * md2 = mixAddress(s_currIdx) ;

  SUBMENU_WITH_OPTIONS(STR_MIXER, LBM_MIXER_ICON, MIX_FIELD_COUNT, OPTION_MENU_NO_SCROLLBAR, { 0, 0, 0, 0, 0, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0 /*, ...*/ });
  putsChn(50, 3+FH, md2->destCh+1, MENU_TITLE_COLOR);

  // The separation line between 2 columns
  lcdDrawSolidVerticalLine(MENU_COLUMN2_X-20, DEFAULT_SCROLLBAR_Y-FH, DEFAULT_SCROLLBAR_H+5, TEXT_COLOR);

  int8_t sub = menuVerticalPosition;
  int8_t editMode = s_editMode;

  for (int k=0; k<2*NUM_BODY_LINES; k++) {
    coord_t y;
    if (k >= NUM_BODY_LINES) {
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

#define _STR_MAX(x) PSTR("/" #x)
#define STR_MAX(x) _STR_MAX(x)

#define MIX_LINE_WEIGHT_POS     92
#define MIX_LINE_SRC_POS        115
#define MIX_LINE_CURVE_POS      162
#define MIX_LINE_SWITCH_POS     210
#define MIX_LINE_DELAY_POS      255
#define MIX_LINE_FM_POS         270
#define MIX_LINE_NAME_POS       384
#define MIX_LINE_SELECT_POS     50
#define MIX_LINE_SELECT_POS     50
#define MIX_LINE_SELECT_WIDTH   (LCD_W-MIX_LINE_SELECT_POS-15)

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
  putsCurveRef(MIX_LINE_CURVE_POS, y, md->curve);

  if (md->swtch) {
    putsSwitches(MIX_LINE_SWITCH_POS, y, md->swtch);
  }
}

void displayMixLine(coord_t y, MixData *md)
{
  if (md->name[0]) {
    lcdDrawSizedText(MIX_LINE_NAME_POS, y+2, md->name, sizeof(md->name), ZCHAR | SMLSIZE);
  }
  displayMixInfos(y, md);
  displayFlightModes(MIX_LINE_FM_POS, y, md->flightModes, 0);
}

bool menuModelMixAll(evt_t event)
{
  int sub = menuVerticalPosition;

  if (s_editMode > 0) {
    s_editMode = 0;
  }

  uint8_t chn = mixAddress(s_currIdx)->destCh + 1;

  int linesCount = getMixesLinesCount();
  SIMPLE_MENU(STR_MIXER, LBM_MODEL_ICONS, menuTabModel, e_MixAll, linesCount);

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
          else if (menuVerticalPosition >= 0) {
            event = 0;
            s_copyMode = 0;
            POPUP_MENU_ADD_ITEM(STR_EDIT);
            POPUP_MENU_ADD_ITEM(STR_INSERT_BEFORE);
            POPUP_MENU_ADD_ITEM(STR_INSERT_AFTER);
            POPUP_MENU_ADD_ITEM(STR_COPY);
            POPUP_MENU_ADD_ITEM(STR_MOVE);
            POPUP_MENU_ADD_ITEM(STR_DELETE);
            popupMenuHandler = onMixesMenu;
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
        uint8_t next_ofs = ((event==EVT_ROTARY_LEFT || key==KEY_UP) ? s_copyTgtOfs - 1 : s_copyTgtOfs + 1);

        if (s_copyTgtOfs==0 && s_copyMode==COPY_MODE) {
          // insert a mix on the same channel (just above / just below)
          if (reachMixesLimit()) break;
          copyMix(s_currIdx);
          if (event==EVT_ROTARY_RIGHT || key==KEY_DOWN) s_currIdx++;
          else if (sub-menuVerticalOffset >= 6) menuVerticalOffset++;
        }
        else if (next_ofs==0 && s_copyMode==COPY_MODE) {
          // delete the mix
          deleteMix(s_currIdx);
          if (event==EVT_ROTARY_LEFT || key==KEY_UP) s_currIdx--;
        }
        else {
          // only swap the mix with its neighbor
          if (!swapMixes(s_currIdx, event==EVT_ROTARY_LEFT || key==KEY_UP)) break;
          storageDirty(EE_MODEL);
        }

        s_copyTgtOfs = next_ofs;
      }
      break;
  }

  char str[6];
  sprintf(str, "%d/%d", getMixesCount(), MAX_MIXERS);
  lcdDrawText(MENU_TITLE_NEXT_POS, MENU_TITLE_TOP+2, str, HEADER_COLOR);

  sub = menuVerticalPosition;
  s_currCh = 0;
  int cur = 0;
  int i = 0;

  for (int ch=1; ch<=NUM_CHNOUT; ch++) {
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
        }
        if (cur-menuVerticalOffset >= 0 && cur-menuVerticalOffset < NUM_BODY_LINES) {
          LcdFlags attr = ((s_copyMode || sub != cur) ? 0 : INVERS);

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
