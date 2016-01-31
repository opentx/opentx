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

#define SETUP_SCREEN_COLUMN2           200
#define SETUP_SCREEN_COLUMN3           275
#define SETUP_SCREEN_COLUMN4           350

enum menuSetupScreensViewItems
{
  ITEM_SETUP_TOP_BAR_LABEL,
  ITEM_SETUP_TOP_BAR_VOLTAGE,
  ITEM_SETUP_TOP_BAR_ALTITUDE,
  ITEM_SETUP_SCREEN_LABEL1,
  ITEM_SETUP_SCREEN_LINE1,
  ITEM_SETUP_SCREEN_LINE2,
  ITEM_SETUP_SCREEN_LINE3,
  ITEM_SETUP_SCREEN_LINE4,
  ITEM_SETUP_SCREEN_LABEL2,
  ITEM_SETUP_SCREEN_LINE5,
  ITEM_SETUP_SCREEN_LINE6,
  ITEM_SETUP_SCREEN_LINE7,
  ITEM_SETUP_SCREEN_LINE8,
  ITEM_SETUP_SCREEN_LABEL3,
  ITEM_SETUP_SCREEN_LINE9,
  ITEM_SETUP_SCREEN_LINE10,
  ITEM_SETUP_SCREEN_LINE11,
  ITEM_SETUP_SCREEN_LINE12,
  ITEM_SETUP_SCREEN_LABEL4,
  ITEM_SETUP_SCREEN_LINE13,
  ITEM_SETUP_SCREEN_LINE14,
  ITEM_SETUP_SCREEN_LINE15,
  ITEM_SETUP_SCREEN_LINE16,
  ITEM_SETUP_SCREEN_MAX
};

#if defined(LUA)
  #define SCREEN_TYPE_ROWS             1
  #define SETUP_SCREEN_LINE(x)         ((TELEMETRY_SCREEN_TYPE(x) == TELEMETRY_SCREEN_TYPE_NONE || TELEMETRY_SCREEN_TYPE(x) == TELEMETRY_SCREEN_TYPE_SCRIPT) ? HIDDEN_ROW : (uint8_t)2)
#else
  #define SCREEN_TYPE_ROWS             0
  #define SETUP_SCREEN_LINE(x)         (TELEMETRY_SCREEN_TYPE(x) == TELEMETRY_SCREEN_TYPE_NONE ? HIDDEN_ROW : (uint8_t)2)
#endif

#define SETUP_SCREEN_ROWS(x)           SCREEN_TYPE_ROWS, SETUP_SCREEN_LINE(x), SETUP_SCREEN_LINE(x), SETUP_SCREEN_LINE(x), SETUP_SCREEN_LINE(x)
#define TELEMETRY_CURRENT_SCREEN(k)    (k < ITEM_SETUP_SCREEN_LABEL2 ? 0 : (k < ITEM_SETUP_SCREEN_LABEL3 ? 1 : (k < ITEM_SETUP_SCREEN_LABEL4 ? 2 : 3)))

#if defined(LUA)
void onSetupScriptFileSelectionMenu(const char * result)
{
  int screenIndex = TELEMETRY_CURRENT_SCREEN(menuVerticalPosition);

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

bool menuSetupScreensView(evt_t event)
{
  MENU_ONE_PAGE("Setup screens", ITEM_SETUP_SCREEN_MAX, DEFAULT_SCROLLBAR_X, { LABEL(TopBar), 0, 0, SETUP_SCREEN_ROWS(0), SETUP_SCREEN_ROWS(1), SETUP_SCREEN_ROWS(2), SETUP_SCREEN_ROWS(3) });

  for (int i=0; i<NUM_BODY_LINES+1; i++) {
    coord_t y = MENU_CONTENT_TOP + (i - 1) * FH - 5;
    int k = i + menuVerticalOffset;

    for (int j=0; j<k; j++) {
      if (mstate_tab[j + 1] == HIDDEN_ROW) {
        if (++k >= (int) DIM(mstate_tab)) {
          return true;
        }
      }
    }

    LcdFlags blink = ((s_editMode > 0) ? BLINK | INVERS : INVERS);
    LcdFlags attr = (menuVerticalPosition == k ? blink : 0);

    switch (k) {
      case ITEM_SETUP_TOP_BAR_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TOP_BAR);
        break;

      case ITEM_SETUP_TOP_BAR_VOLTAGE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_VOLTAGE);
        putsMixerSource(SETUP_SCREEN_COLUMN2, y, g_model.frsky.voltsSource ? MIXSRC_FIRST_TELEM+3*(g_model.frsky.voltsSource-1) : 0, attr);
        if (attr) {
          g_model.frsky.voltsSource = checkIncDec(event, g_model.frsky.voltsSource, 0, MAX_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isVoltsSensor);
        }
        break;

      case ITEM_SETUP_TOP_BAR_ALTITUDE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ALTITUDE);
        putsMixerSource(SETUP_SCREEN_COLUMN2, y, g_model.frsky.altitudeSource ? MIXSRC_FIRST_TELEM+3*(g_model.frsky.altitudeSource-1) : 0, attr);
        if (attr) {
          g_model.frsky.altitudeSource = checkIncDec(event, g_model.frsky.altitudeSource, 0, MAX_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isAltSensor);
        }
        break;

      case ITEM_SETUP_SCREEN_LABEL1:
      case ITEM_SETUP_SCREEN_LABEL2:
      case ITEM_SETUP_SCREEN_LABEL3:
      case ITEM_SETUP_SCREEN_LABEL4:
      {
        uint8_t screenIndex = TELEMETRY_CURRENT_SCREEN(k);
        drawStringWithIndex(MENUS_MARGIN_LEFT, y, STR_SCREEN, screenIndex+1);
        TelemetryScreenType oldScreenType = TELEMETRY_SCREEN_TYPE(screenIndex);
        TelemetryScreenType newScreenType = (TelemetryScreenType)selectMenuItem(SETUP_SCREEN_COLUMN2, y, STR_VTELEMSCREENTYPE, oldScreenType, 0, TELEMETRY_SCREEN_TYPE_MAX, (menuHorizontalPosition==0 ? attr : 0), event);
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
            lcdDrawSizedText(SETUP_SCREEN_COLUMN3, y, scriptData.file, sizeof(scriptData.file), (menuHorizontalPosition==1 ? attr : 0));
          else
            lcdDrawTextAtIndex(SETUP_SCREEN_COLUMN3, y, STR_VCSWFUNC, 0, (menuHorizontalPosition==1 ? attr : 0));

          if (menuHorizontalPosition==1 && attr && event==EVT_KEY_BREAK(KEY_ENTER) && READ_ONLY_UNLOCKED()) {
            s_editMode = 0;
            if (sdListFiles(SCRIPTS_TELEM_PATH, SCRIPTS_EXT, sizeof(g_model.frsky.screens[screenIndex].script.file), g_model.frsky.screens[screenIndex].script.file)) {
              popupMenuHandler = onSetupScriptFileSelectionMenu;
            }
            else {
              POPUP_WARNING(STR_NO_SCRIPTS_ON_SD);
            }
          }
        }
        else if (attr) {
          MOVE_CURSOR_FROM_HERE();
        }
#endif
        break;
      }

      case ITEM_SETUP_SCREEN_LINE1:
      case ITEM_SETUP_SCREEN_LINE2:
      case ITEM_SETUP_SCREEN_LINE3:
      case ITEM_SETUP_SCREEN_LINE4:
      case ITEM_SETUP_SCREEN_LINE5:
      case ITEM_SETUP_SCREEN_LINE6:
      case ITEM_SETUP_SCREEN_LINE7:
      case ITEM_SETUP_SCREEN_LINE8:
      case ITEM_SETUP_SCREEN_LINE9:
      case ITEM_SETUP_SCREEN_LINE10:
      case ITEM_SETUP_SCREEN_LINE11:
      case ITEM_SETUP_SCREEN_LINE12:
      case ITEM_SETUP_SCREEN_LINE13:
      case ITEM_SETUP_SCREEN_LINE14:
      case ITEM_SETUP_SCREEN_LINE15:
      case ITEM_SETUP_SCREEN_LINE16: {
        uint8_t screenIndex, lineIndex;
        if (k < ITEM_SETUP_SCREEN_LABEL2) {
          screenIndex = 0;
          lineIndex = k - ITEM_SETUP_SCREEN_LINE1;
        }
        else if (k >= ITEM_SETUP_SCREEN_LABEL4) {
          screenIndex = 3;
          lineIndex = k - ITEM_SETUP_SCREEN_LINE13;
        }
        else if (k >= ITEM_SETUP_SCREEN_LABEL3) {
          screenIndex = 2;
          lineIndex = k - ITEM_SETUP_SCREEN_LINE9;
        }
        else {
          screenIndex = 1;
          lineIndex = k - ITEM_SETUP_SCREEN_LINE5;
        }

#if defined(GAUGES)
        if (IS_BARS_SCREEN(screenIndex)) {
          FrSkyBarData & bar = g_model.frsky.screens[screenIndex].bars[lineIndex];
          source_t barSource = bar.source;
          putsMixerSource(MENUS_MARGIN_LEFT+INDENT_WIDTH, y, barSource, (menuHorizontalPosition==0 ? attr : 0));
          int barMax = getMaximumValue(barSource);
          int barMin = -barMax;
          if (barSource) {
            if (barSource <= MIXSRC_LAST_CH) {
              putsChannelValue(SETUP_SCREEN_COLUMN2, y, barSource, calc100toRESX(bar.barMin), (menuHorizontalPosition==1 ? attr : 0) | LEFT);
              putsChannelValue(SETUP_SCREEN_COLUMN4, y, barSource, calc100toRESX(bar.barMax), (menuHorizontalPosition==2 ? attr : 0) | LEFT);
            }
            else {
              putsChannelValue(SETUP_SCREEN_COLUMN2, y, barSource, bar.barMin, (menuHorizontalPosition==1 ? attr : 0) | LEFT);
              putsChannelValue(SETUP_SCREEN_COLUMN4, y, barSource, bar.barMax, (menuHorizontalPosition==2 ? attr : 0) | LEFT);
            }
          }
          else if (attr) {
            MOVE_CURSOR_FROM_HERE();
          }
          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                bar.source = checkIncDec(event, barSource, 0, MIXSRC_LAST_TELEM, EE_MODEL|INCDEC_SOURCE|NO_INCDEC_MARKS, isSourceAvailable);
                if (checkIncDec_Ret) {
                  if (barSource <= MIXSRC_LAST_CH) {
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
        else
#endif
        {
          for (int c = 0; c < NUM_LINE_ITEMS; c++) {
            LcdFlags cellAttr = (menuHorizontalPosition == c ? attr : 0);
            source_t &value = g_model.frsky.screens[screenIndex].lines[lineIndex].sources[c];
            const coord_t pos[] = {MENUS_MARGIN_LEFT + INDENT_WIDTH, SETUP_SCREEN_COLUMN2, SETUP_SCREEN_COLUMN4};
            putsMixerSource(pos[c], y, value, cellAttr);
            if (cellAttr && s_editMode > 0) {
              value = checkIncDec(event, value, 0, MIXSRC_LAST_TELEM, EE_MODEL | INCDEC_SOURCE | NO_INCDEC_MARKS,
                                  isSourceAvailable);
            }
          }
          if (attr && menuHorizontalPosition == NUM_LINE_ITEMS) {
            REPEAT_LAST_CURSOR_MOVE(0);
          }
        }
        break;
      }
    }
  }
  return true;
}
