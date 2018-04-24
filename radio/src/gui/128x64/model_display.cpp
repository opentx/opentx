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

enum MenuModelDisplayItems {
  ITEM_DISPLAY_SCREEN_LABEL1,
  ITEM_DISPLAY_SCREEN_LINE1,
  ITEM_DISPLAY_SCREEN_LINE2,
  ITEM_DISPLAY_SCREEN_LINE3,
  ITEM_DISPLAY_SCREEN_LINE4,
  ITEM_DISPLAY_SCREEN_LABEL2,
  ITEM_DISPLAY_SCREEN_LINE5,
  ITEM_DISPLAY_SCREEN_LINE6,
  ITEM_DISPLAY_SCREEN_LINE7,
  ITEM_DISPLAY_SCREEN_LINE8,
  ITEM_DISPLAY_SCREEN_LABEL3,
  ITEM_DISPLAY_SCREEN_LINE9,
  ITEM_DISPLAY_SCREEN_LINE10,
  ITEM_DISPLAY_SCREEN_LINE11,
  ITEM_DISPLAY_SCREEN_LINE12,
  ITEM_DISPLAY_SCREEN_LABEL4,
  ITEM_DISPLAY_SCREEN_LINE13,
  ITEM_DISPLAY_SCREEN_LINE14,
  ITEM_DISPLAY_SCREEN_LINE15,
  ITEM_DISPLAY_SCREEN_LINE16,
  ITEM_DISPLAY_MAX
};

#define DISPLAY_COL1                  (1*FW)
#if defined(TRANSLATIONS_CZ)
  #define DISPLAY_COL2                (9*FW)
#else
  #define DISPLAY_COL2                (8*FW)
#endif
#define DISPLAY_COL3                  (17*FW+2)

#if defined(LUA)
  #define SCREEN_TYPE_ROWS            1
  #define DISPLAY_LINE_ROWS(x)        ((TELEMETRY_SCREEN_TYPE(x) == TELEMETRY_SCREEN_TYPE_NONE || TELEMETRY_SCREEN_TYPE(x) == TELEMETRY_SCREEN_TYPE_SCRIPT) ? HIDDEN_ROW : (TELEMETRY_SCREEN_TYPE(x) == TELEMETRY_SCREEN_TYPE_GAUGES ? (uint8_t)2 : (uint8_t)1))
#else
  #define SCREEN_TYPE_ROWS            0
  #define DISPLAY_LINE_ROWS(x)        (TELEMETRY_SCREEN_TYPE(x) == TELEMETRY_SCREEN_TYPE_NONE ? HIDDEN_ROW : (TELEMETRY_SCREEN_TYPE(x) == TELEMETRY_SCREEN_TYPE_GAUGES ? (uint8_t)2 : (uint8_t)1))
#endif

#define TELEMETRY_SCREEN_ROWS(x)      SCREEN_TYPE_ROWS, DISPLAY_LINE_ROWS(x), DISPLAY_LINE_ROWS(x), DISPLAY_LINE_ROWS(x), DISPLAY_LINE_ROWS(x)
#define TELEMETRY_CURRENT_SCREEN(k)   (k < ITEM_DISPLAY_SCREEN_LABEL2 ? 0 : (k < ITEM_DISPLAY_SCREEN_LABEL3 ? 1 : (k < ITEM_DISPLAY_SCREEN_LABEL4 ? 2 : 3)))

#if defined(LUA)
void onTelemetryScriptFileSelectionMenu(const char *result)
{
  int screenIndex = TELEMETRY_CURRENT_SCREEN(menuVerticalPosition - HEADER_LINE);

  if (result == STR_UPDATE_LIST) {
    if (!sdListFiles(SCRIPTS_TELEM_PATH, SCRIPTS_EXT, sizeof(g_model.frsky.screens[screenIndex].script.file), NULL)) {
      POPUP_WARNING(STR_NO_SCRIPTS_ON_SD);
    }
  }
  else {
    // The user choosed a file in the list
    memcpy(g_model.frsky.screens[screenIndex].script.file, result, sizeof(g_model.frsky.screens[screenIndex].script.file));
    storageDirty(EE_MODEL);
    LUA_LOAD_MODEL_SCRIPTS();
  }
}
#endif


int skipHiddenLines(int noRows, const uint8_t * mstate_tab, int row)
{
  for(int i=0; i<noRows; ++i) {
    if (mstate_tab[HEADER_LINE + i] != HIDDEN_ROW) {
      if (row == 0) {
        // TRACE("%d -> %d", Row, i);
        return i;
      }
      --row;
    }
  }
  return -1;
}

// #define SKIP_HIDDEN_MENU_ROWS(row)    row = skipHiddenLines((int)DIM(mstate_tab), mstate_tab, row); if (row < 0) return;
#define SKIP_HIDDEN_MENU_ROWS(row)    if ((row = skipHiddenLines((int)DIM(mstate_tab), mstate_tab, row)) < 0) return;

void menuModelDisplay(event_t event)
{
  MENU(STR_MENU_DISPLAY, menuTabModel, MENU_MODEL_DISPLAY, HEADER_LINE + ITEM_DISPLAY_MAX, { HEADER_LINE_COLUMNS TELEMETRY_SCREEN_ROWS(0), TELEMETRY_SCREEN_ROWS(1), TELEMETRY_SCREEN_ROWS(2), TELEMETRY_SCREEN_ROWS(3) });

  int8_t sub = menuVerticalPosition - HEADER_LINE;

  for (uint8_t i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    int k = i + menuVerticalOffset;
    SKIP_HIDDEN_MENU_ROWS(k);

    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch (k) {
      case ITEM_DISPLAY_SCREEN_LABEL1:
      case ITEM_DISPLAY_SCREEN_LABEL2:
      case ITEM_DISPLAY_SCREEN_LABEL3:
      case ITEM_DISPLAY_SCREEN_LABEL4:
      {
        uint8_t screenIndex = TELEMETRY_CURRENT_SCREEN(k);
        drawStringWithIndex(0*FW, y, STR_SCREEN, screenIndex+1);
        TelemetryScreenType oldScreenType = TELEMETRY_SCREEN_TYPE(screenIndex);
        TelemetryScreenType newScreenType = (TelemetryScreenType)editChoice(DISPLAY_COL2, y, PSTR(""), STR_VTELEMSCREENTYPE, oldScreenType, 0, TELEMETRY_SCREEN_TYPE_MAX, (menuHorizontalPosition==0 ? attr : 0), event);
        if (newScreenType != oldScreenType) {
          g_model.frsky.screensType = (g_model.frsky.screensType & (~(0x03 << (2*screenIndex)))) | (newScreenType << (2*screenIndex));
          memset(&g_model.frsky.screens[screenIndex], 0, sizeof(g_model.frsky.screens[screenIndex]));
        }
#if defined(LUA)
        if (newScreenType == TELEMETRY_SCREEN_TYPE_SCRIPT) {
          TelemetryScriptData & scriptData = g_model.frsky.screens[screenIndex].script;

          // TODO better function name for ---
          // TODO function for these lines
          if (ZEXIST(scriptData.file))
            lcdDrawSizedText(DISPLAY_COL2+7*FW, y, scriptData.file, sizeof(scriptData.file), (menuHorizontalPosition==1 ? attr : 0));
          else
            lcdDrawTextAtIndex(DISPLAY_COL2+7*FW, y, STR_VCSWFUNC, 0, (menuHorizontalPosition==1 ? attr : 0));

          if (menuHorizontalPosition==1 && attr && event==EVT_KEY_BREAK(KEY_ENTER) && READ_ONLY_UNLOCKED()) {
            s_editMode = 0;
            if (sdListFiles(SCRIPTS_TELEM_PATH, SCRIPTS_EXT, sizeof(g_model.frsky.screens[screenIndex].script.file), g_model.frsky.screens[screenIndex].script.file)) {
              POPUP_MENU_START(onTelemetryScriptFileSelectionMenu);
            }
            else {
              POPUP_WARNING(STR_NO_SCRIPTS_ON_SD);
            }
          }
        }
#if !defined(PCBXLITE)
        else if (attr) {
          MOVE_CURSOR_FROM_HERE();
        }
#endif
#endif
        break;
      }

      case ITEM_DISPLAY_SCREEN_LINE1:
      case ITEM_DISPLAY_SCREEN_LINE2:
      case ITEM_DISPLAY_SCREEN_LINE3:
      case ITEM_DISPLAY_SCREEN_LINE4:
      case ITEM_DISPLAY_SCREEN_LINE5:
      case ITEM_DISPLAY_SCREEN_LINE6:
      case ITEM_DISPLAY_SCREEN_LINE7:
      case ITEM_DISPLAY_SCREEN_LINE8:
      case ITEM_DISPLAY_SCREEN_LINE9:
      case ITEM_DISPLAY_SCREEN_LINE10:
      case ITEM_DISPLAY_SCREEN_LINE11:
      case ITEM_DISPLAY_SCREEN_LINE12:
      case ITEM_DISPLAY_SCREEN_LINE13:
      case ITEM_DISPLAY_SCREEN_LINE14:
      case ITEM_DISPLAY_SCREEN_LINE15:
      case ITEM_DISPLAY_SCREEN_LINE16:
      {
        uint8_t screenIndex, lineIndex;
        if (k < ITEM_DISPLAY_SCREEN_LABEL2) {
          screenIndex = 0;
          lineIndex = k-ITEM_DISPLAY_SCREEN_LINE1;
        }
        else if (k >= ITEM_DISPLAY_SCREEN_LABEL4) {
          screenIndex = 3;
          lineIndex = k-ITEM_DISPLAY_SCREEN_LINE13;
        }
        else if (k >= ITEM_DISPLAY_SCREEN_LABEL3) {
          screenIndex = 2;
          lineIndex = k-ITEM_DISPLAY_SCREEN_LINE9;
        }
        else {
          screenIndex = 1;
          lineIndex = k-ITEM_DISPLAY_SCREEN_LINE5;
        }

        if (IS_BARS_SCREEN(screenIndex)) {
          FrSkyBarData & bar = g_model.frsky.screens[screenIndex].bars[lineIndex];
          drawSource(DISPLAY_COL1, y, bar.source, menuHorizontalPosition==0 ? attr : 0);
          int16_t barMax, barMin;
          LcdFlags lf = LEFT;
          getMixSrcRange(bar.source, barMin, barMax, &lf);
          if (bar.source) {
            if (bar.source <= MIXSRC_LAST_CH) {
              drawSourceCustomValue(DISPLAY_COL2, y, bar.source, calc100toRESX(bar.barMin), (menuHorizontalPosition==1 ? attr : 0) | lf);
              drawSourceCustomValue(DISPLAY_COL3, y, bar.source, calc100toRESX(bar.barMax), (menuHorizontalPosition==2 ? attr : 0) | lf);
            }
            else {
              drawSourceCustomValue(DISPLAY_COL2, y, bar.source, bar.barMin, (menuHorizontalPosition==1 ? attr : 0) | lf);
              drawSourceCustomValue(DISPLAY_COL3, y, bar.source, bar.barMax, (menuHorizontalPosition==2 ? attr : 0) | lf);
            }
          }
          else if (attr) {
            MOVE_CURSOR_FROM_HERE();
          }
          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                bar.source = checkIncDec(event, bar.source, 0, MIXSRC_LAST_TELEM, EE_MODEL|INCDEC_SOURCE|NO_INCDEC_MARKS, isSourceAvailable);
                if (checkIncDec_Ret) {
                  if (bar.source <= MIXSRC_LAST_CH) {
                    bar.barMin = -100;
                    bar.barMax = 100;
                  }
                  else {
                    bar.barMin = 0;
                    bar.barMax = 0;
                  }
                }
                break;
              case 1:
                bar.barMin = checkIncDec(event, bar.barMin, barMin, bar.barMax, EE_MODEL|NO_INCDEC_MARKS);
                break;
              case 2:
                bar.barMax = checkIncDec(event, bar.barMax, bar.barMin, barMax, EE_MODEL|NO_INCDEC_MARKS);
                break;
            }
          }
        }
        else {
          for (int c=0; c<NUM_LINE_ITEMS; c++) {
            LcdFlags cellAttr = (menuHorizontalPosition==c ? attr : 0);
            source_t & value = g_model.frsky.screens[screenIndex].lines[lineIndex].sources[c];
            const coord_t pos[] = {DISPLAY_COL1, DISPLAY_COL2, DISPLAY_COL3};
            drawSource(pos[c], y, value, cellAttr);
            if (cellAttr && s_editMode>0) {
              value = checkIncDec(event, value, 0, MIXSRC_LAST_TELEM, EE_MODEL|INCDEC_SOURCE|NO_INCDEC_MARKS, isSourceAvailable);
            }
          }
          if (attr && menuHorizontalPosition == NUM_LINE_ITEMS) {
            REPEAT_LAST_CURSOR_MOVE();
          }
        }
        break;
      }
    }
  }
}
