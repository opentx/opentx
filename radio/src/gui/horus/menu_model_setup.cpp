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

uint8_t g_moduleIdx;
bool menuModelFailsafe(evt_t event);

enum menuModelSetupItems {
  ITEM_MODEL_NAME,
  ITEM_MODEL_BITMAP,
  ITEM_MODEL_TIMER1,
  ITEM_MODEL_TIMER1_NAME,
  ITEM_MODEL_TIMER1_PERSISTENT,
  ITEM_MODEL_TIMER1_MINUTE_BEEP,
  ITEM_MODEL_TIMER1_COUNTDOWN_BEEP,
#if TIMERS > 1
  ITEM_MODEL_TIMER2,
  ITEM_MODEL_TIMER2_NAME,
  ITEM_MODEL_TIMER2_PERSISTENT,
  ITEM_MODEL_TIMER2_MINUTE_BEEP,
  ITEM_MODEL_TIMER2_COUNTDOWN_BEEP,
#endif
#if TIMERS > 2
  ITEM_MODEL_TIMER3,
  ITEM_MODEL_TIMER3_NAME,
  ITEM_MODEL_TIMER3_PERSISTENT,
  ITEM_MODEL_TIMER3_MINUTE_BEEP,
  ITEM_MODEL_TIMER3_COUNTDOWN_BEEP,
#endif
  ITEM_MODEL_EXTENDED_LIMITS,
  ITEM_MODEL_EXTENDED_TRIMS,
  ITEM_MODEL_DISPLAY_TRIMS,
  ITEM_MODEL_TRIM_INC,
  ITEM_MODEL_THROTTLE_LABEL,
  ITEM_MODEL_THROTTLE_REVERSED,
  ITEM_MODEL_THROTTLE_TRACE,
  ITEM_MODEL_THROTTLE_TRIM,
  ITEM_MODEL_PREFLIGHT_LABEL,
  ITEM_MODEL_CHECKLIST_DISPLAY,
  ITEM_MODEL_THROTTLE_WARNING,
  ITEM_MODEL_SWITCHES_WARNING,
  ITEM_MODEL_POT_WARNING,
  ITEM_MODEL_BEEP_CENTER,
  ITEM_MODEL_USE_GLOBAL_FUNCTIONS,
  ITEM_MODEL_INTERNAL_MODULE_LABEL,
  ITEM_MODEL_INTERNAL_MODULE_MODE,
  ITEM_MODEL_INTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_INTERNAL_MODULE_BIND,
  ITEM_MODEL_INTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_INTERNAL_MODULE_ANTENNA,
  ITEM_MODEL_EXTERNAL_MODULE_LABEL,
  ITEM_MODEL_EXTERNAL_MODULE_MODE,
  ITEM_MODEL_EXTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_EXTERNAL_MODULE_BIND,
  ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE,
#if defined(MULTIMODULE)
  ITEM_MODEL_EXTERNAL_MODULE_AUTOBIND,
  ITEM_MODEL_EXTERNAL_MODULE_LOWPOWER,
#endif
  ITEM_MODEL_TRAINER_LABEL,
  ITEM_MODEL_TRAINER_MODE,
  ITEM_MODEL_TRAINER_CHANNELS,
  ITEM_MODEL_TRAINER_SETTINGS,
  ITEM_MODEL_SETUP_MAX
};

#define MODEL_SETUP_2ND_COLUMN        200
#define MODEL_SETUP_3RD_COLUMN        270
#define MODEL_SETUP_4TH_COLUMN        350
#define MODEL_SETUP_BIND_OFS          40
#define MODEL_SETUP_RANGE_OFS         80
#define MODEL_SETUP_SET_FAILSAFE_OFS  100

void onModelSetupBitmapMenu(const char *result)
{
  if (result == STR_UPDATE_LIST) {
    if (!sdListFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap), NULL)) {
      POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
    }
  }
  else {
    // The user choosed a bmp file in the list
    copySelection(g_model.header.bitmap, result, sizeof(g_model.header.bitmap));
    storageDirty(EE_MODEL);
  }
}

void editTimerMode(int timerIdx, coord_t y, LcdFlags attr, evt_t event)
{
  TimerData & timer = g_model.timers[timerIdx];
  if (attr && menuHorizontalPosition < 0) {
    lcdDrawSolidFilledRect(MODEL_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN+1, 115+2*INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
  }
  drawStringWithIndex(MENUS_MARGIN_LEFT, y, STR_TIMER, timerIdx+1);
  putsTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer.mode, (menuHorizontalPosition<=0 ? attr : 0));
  putsTimer(MODEL_SETUP_2ND_COLUMN+50, y, timer.start, (menuHorizontalPosition!=0 ? attr|TIMEHOUR : TIMEHOUR));
  if (attr && s_editMode>0) {
    switch (menuHorizontalPosition) {
      case 0:
      {
        int8_t timerMode = timer.mode;
        if (timerMode < 0) timerMode -= TMRMODE_COUNT-1;
        CHECK_INCDEC_MODELVAR_CHECK(event, timerMode, -TMRMODE_COUNT-SWSRC_LAST+1, TMRMODE_COUNT+SWSRC_LAST-1, isSwitchAvailableInTimers);
        if (timerMode < 0) timerMode += TMRMODE_COUNT-1;
        timer.mode = timerMode;
#if defined(AUTOSWITCH)
        if (s_editMode>0) {
          int8_t val = timer.mode - (TMRMODE_COUNT-1);
          int8_t switchVal = checkIncDecMovedSwitch(val);
          if (val != switchVal) {
            timer.mode = switchVal + (TMRMODE_COUNT-1);
            storageDirty(EE_MODEL);
          }
        }
#endif
        break;
      }
      case 1:
      {
        const int stopsMinutes[] = { 8, 60, 120, 180, 240, 300, 600, 900, 1200 };
        timer.start = checkIncDec(event, timer.start, 0, 60*60, EE_MODEL, NULL, (const CheckIncDecStops&)stopsMinutes);
        break;
      }
    }
  }
}

void editTimerCountdown(int timerIdx, coord_t y, LcdFlags attr, evt_t event)
{
  TimerData & timer = g_model.timers[timerIdx];
  lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BEEPCOUNTDOWN);
  lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VBEEPCOUNTDOWN, timer.countdownBeep, (menuHorizontalPosition == 0 ? attr : 0));
  if (timer.countdownBeep != COUNTDOWN_SILENT) {
    lcdDrawNumber(MODEL_SETUP_3RD_COLUMN, y, TIMER_COUNTDOWN_START(timerIdx), (menuHorizontalPosition == 1 ? attr : 0) | LEFT, 0, NULL, "s");
  }
  if (attr && s_editMode > 0) {
    switch (menuHorizontalPosition) {
      case 0:
        CHECK_INCDEC_MODELVAR(event, timer.countdownBeep, COUNTDOWN_SILENT, COUNTDOWN_COUNT - 1);
        break;
      case 1:
        timer.countdownStart = -checkIncDecModel(event, -timer.countdownStart, -1, +2);
        break;
    }
  }
}

#define CURRENT_MODULE_EDITED(k)         (k>=ITEM_MODEL_TRAINER_LABEL ? TRAINER_MODULE : (k>=ITEM_MODEL_EXTERNAL_MODULE_LABEL ? EXTERNAL_MODULE : INTERNAL_MODULE))

int getSwitchWarningsCount()
{
  int count = 0;
  for (int i=0; i<NUM_SWITCHES; ++i) {
    if (SWITCH_WARNING_ALLOWED(i)) {
      ++count;
    }
  }
  return count;
}

#define IF_EXTERNAL_MODULE_XJT(x)         (IS_MODULE_XJT(EXTERNAL_MODULE) ? (uint8_t)x : HIDDEN_ROW)
#define IS_D8_RX(x)                       (g_model.moduleData[x].rfProtocol == RF_PROTO_D8)
#define IF_INTERNAL_MODULE_ON(x)          (g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF ? HIDDEN_ROW : (uint8_t)(x))
#define IF_EXTERNAL_MODULE_ON(x)          (g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_NONE ? HIDDEN_ROW : (uint8_t)(x))
#define IF_TRAINER_ON(x)                  (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)(x) : HIDDEN_ROW)

#define INTERNAL_MODULE_MODE_ROWS         (uint8_t)0
#define INTERNAL_MODULE_CHANNELS_ROWS     IF_INTERNAL_MODULE_ON(1)
#define EXTERNAL_MODULE_CHANNELS_ROWS     IF_EXTERNAL_MODULE_ON((IS_MODULE_DSM2(EXTERNAL_MODULE) || IS_MODULE_CROSSFIRE(EXTERNAL_MODULE) || IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) ? (uint8_t)0 : (uint8_t)1)
#define TRAINER_CHANNELS_ROWS()           IF_TRAINER_ON(1)
#define PORT_CHANNELS_ROWS(x)             (x==INTERNAL_MODULE ? INTERNAL_MODULE_CHANNELS_ROWS : (x==EXTERNAL_MODULE ? EXTERNAL_MODULE_CHANNELS_ROWS : TRAINER_CHANNELS_ROWS()))
#if defined MULTIMODULE
  #define MULTIMODULE_HASOPTIONS(x)       (x == MM_RF_PROTO_HUBSAN || x == MM_RF_PROTO_FRSKY || x == MM_RF_PROTO_DSM2 || x >= MM_RF_PROTO_CUSTOM)
  #define MULTIMODULE_FAILSAFEROWS(x)     (IS_MODULE_MULTIMODULE(x) && (MULTIMODULE_HASOPTIONS(g_model.moduleData[x].multi.rfProtocol))) ? (uint8_t) 1: HIDDEN_ROW
#else
  #define MULTIMODULE_FAILSAFEROWS(x)     HIDDEN_ROW
#endif

#define FAILSAFE_ROWS(x)                  (IS_MODULE_XJT(x) && HAS_RF_PROTOCOL_FAILSAFE(g_model.moduleData[x].rfProtocol) ? (g_model.moduleData[x].failsafeMode==FAILSAFE_CUSTOM ? (uint8_t)1 : (uint8_t)0) : MULTIMODULE_FAILSAFEROWS(x))
#define TIMER_ROWS(x)                     NAVIGATION_LINE_BY_LINE|1, 0, 0, 0, g_model.timers[x].countdownBeep != COUNTDOWN_SILENT ? (uint8_t)1 : (uint8_t)0

#if defined(MULTIMODULE)
  #define MULTIMODULE_HAS_SUBTYPE(x)      (x == MM_RF_PROTO_FLYSKY || x == MM_RF_PROTO_FRSKY || x == MM_RF_PROTO_HISKY || x == MM_RF_PROTO_DSM2 || x == MM_RF_PROTO_YD717 || x == MM_RF_PROTO_KN || x == MM_RF_PROTO_SYMAX || x == MM_RF_PROTO_CX10 || x == MM_RF_PROTO_CG023 || x == MM_RF_PROTO_MT99XX || x == MM_RF_PROTO_MJXQ)
  #define MULTIMODULE_MODE_ROWS(x)        (g_model.moduleData[x].multi.rfProtocol >= MM_RF_PROTO_CUSTOM ) ? (uint8_t) 3 : MULTIMODULE_HAS_SUBTYPE(g_model.moduleData[x].multi.rfProtocol) ? (uint8_t)2 : (uint8_t)1
  #define MULTIMODULE_MODULE_ROWS         IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? (uint8_t) 1 : HIDDEN_ROW, IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? (uint8_t) 1 : HIDDEN_ROW,
#else
  #define MULTIMODULE_MODE_ROWS(x)        (uint8_t)0
  #define MULTIMODULE_MODULE_ROWS
#endif

#define EXTERNAL_MODULE_MODE_ROWS         (IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE)) ? (uint8_t)1 : IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? MULTIMODULE_MODE_ROWS(EXTERNAL_MODULE) : (uint8_t)0

#if TIMERS == 1
  #define TIMERS_ROWS                     TIMER_ROWS(0)
#elif TIMERS == 2
  #define TIMERS_ROWS                     TIMER_ROWS(0), TIMER_ROWS(1)
#elif TIMERS == 3
  #define TIMERS_ROWS                     TIMER_ROWS(0), TIMER_ROWS(1), TIMER_ROWS(2)
#endif

#define SW_WARN_ITEMS()                   uint8_t(NAVIGATION_LINE_BY_LINE|(getSwitchWarningsCount()-1))
#define POT_WARN_ITEMS()                  ((g_model.potsWarnMode >> 6) ? (uint8_t)NUM_POTS : (uint8_t)0)

bool menuModelSetup(evt_t event)
{
  int l_posHorz = menuHorizontalPosition;
  bool CURSOR_ON_CELL = (menuHorizontalPosition >= 0);

  // Switch to external antenna confirmation
  bool newAntennaSel;
  if (warningResult) {
    warningResult = 0;
    g_model.moduleData[INTERNAL_MODULE].ppm.pulsePol = XJT_EXTERNAL_ANTENNA;
  }

  MENU(STR_MENUSETUP, MODEL_ICONS, menuTabModel, e_ModelSetup, ITEM_MODEL_SETUP_MAX,
       { 0, 0, TIMERS_ROWS, 0, 1, 0, 0,
         LABEL(Throttle), 0, 0, 0,
         LABEL(PreflightCheck), 0, 0, SW_WARN_ITEMS(), POT_WARN_ITEMS(), NAVIGATION_LINE_BY_LINE|(NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1), 0,
         LABEL(InternalModule),
         INTERNAL_MODULE_MODE_ROWS,
         INTERNAL_MODULE_CHANNELS_ROWS,
         IF_INTERNAL_MODULE_ON(IS_MODULE_XJT(INTERNAL_MODULE) ? (HAS_RF_PROTOCOL_MODELINDEX(g_model.moduleData[INTERNAL_MODULE].rfProtocol) ? (uint8_t)2 : (uint8_t)1) : (IS_MODULE_PPM(INTERNAL_MODULE) ? (uint8_t)1 : HIDDEN_ROW)),
         IF_INTERNAL_MODULE_ON((IS_MODULE_XJT(INTERNAL_MODULE)) ? FAILSAFE_ROWS(INTERNAL_MODULE) : HIDDEN_ROW),
         IF_INTERNAL_MODULE_ON(0),
         LABEL(ExternalModule),
         EXTERNAL_MODULE_MODE_ROWS
         EXTERNAL_MODULE_CHANNELS_ROWS,
         (IS_MODULE_XJT(EXTERNAL_MODULE) && !HAS_RF_PROTOCOL_MODELINDEX(g_model.moduleData[EXTERNAL_MODULE].rfProtocol)) ? (uint8_t)1 : (IS_MODULE_PPM(EXTERNAL_MODULE) || IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE) || IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW,
         FAILSAFE_ROWS(EXTERNAL_MODULE), MULTIMODULE_MODULE_ROWS
         LABEL(Trainer), 0, TRAINER_CHANNELS_ROWS(), IF_TRAINER_ON(2) });

  if (menuEvent) {
    moduleFlag[0] = 0;
    moduleFlag[1] = 0;
  }

  int sub = menuVerticalPosition;

  for (int i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    int k = i + menuVerticalOffset;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j] == HIDDEN_ROW)
        k++;
    }

    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_MODEL_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODELNAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.header.name, sizeof(g_model.header.name), event, attr);
        break;

      case ITEM_MODEL_BITMAP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BITMAP);
        if (ZEXIST(g_model.header.bitmap))
          lcdDrawSizedText(MODEL_SETUP_2ND_COLUMN, y, g_model.header.bitmap, sizeof(g_model.header.bitmap), attr);
        else
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VCSWFUNC, 0, attr);
        if (attr && event==EVT_KEY_BREAK(KEY_ENTER) && READ_ONLY_UNLOCKED()) {
          s_editMode = 0;
          if (sdListFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap), g_model.header.bitmap, LIST_NONE_SD_FILE | LIST_SD_FILE_EXT)) {
            POPUP_MENU_START(onModelSetupBitmapMenu);
          }
          else {
            POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
          }
        }
        break;

      case ITEM_MODEL_TIMER1:
        editTimerMode(0, y, attr, event);
        break;

      case ITEM_MODEL_TIMER1_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TIMER_NAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.timers[0].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_TIMER1_MINUTE_BEEP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MINUTEBEEP);
        g_model.timers[0].minuteBeep = editCheckBox(g_model.timers[0].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_TIMER1_COUNTDOWN_BEEP:
        editTimerCountdown(0, y, attr, event);
        break;

      case ITEM_MODEL_TIMER1_PERSISTENT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PERSISTENT);
        g_model.timers[0].persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_VPERSISTENT, g_model.timers[0].persistent, 0, 2, attr, event);
        break;

#if TIMERS > 1
      case ITEM_MODEL_TIMER2:
        editTimerMode(1, y, attr, event);
        break;

      case ITEM_MODEL_TIMER2_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TIMER_NAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.timers[1].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_TIMER2_MINUTE_BEEP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MINUTEBEEP);
        g_model.timers[1].minuteBeep = editCheckBox(g_model.timers[1].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_TIMER2_COUNTDOWN_BEEP:
        editTimerCountdown(1, y, attr, event);
        break;

      case ITEM_MODEL_TIMER2_PERSISTENT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PERSISTENT);
        g_model.timers[1].persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_VPERSISTENT, g_model.timers[1].persistent, 0, 2, attr, event);
        break;
#endif

#if TIMERS > 2
      case ITEM_MODEL_TIMER3:
        editTimerMode(2, y, attr, event);
        break;

      case ITEM_MODEL_TIMER3_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TIMER_NAME);
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.timers[2].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_TIMER3_MINUTE_BEEP:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MINUTEBEEP);
        g_model.timers[2].minuteBeep = editCheckBox(g_model.timers[2].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_TIMER3_COUNTDOWN_BEEP:
        editTimerCountdown(2, y, attr, event);
        break;

      case ITEM_MODEL_TIMER3_PERSISTENT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PERSISTENT);
        g_model.timers[2].persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_VPERSISTENT, g_model.timers[2].persistent, 0, 2, attr, event);
        break;
#endif

      case ITEM_MODEL_EXTENDED_LIMITS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ELIMITS);
        g_model.extendedLimits = editCheckBox(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_EXTENDED_TRIMS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ETRIMS);
        g_model.extendedTrims = editCheckBox(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, menuHorizontalPosition<=0 ? attr : 0, event==EVT_KEY_BREAK(KEY_ENTER) ? event : 0);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN+18, y, STR_RESET_BTN, menuHorizontalPosition>0  && !NO_HIGHLIGHT() ? attr : 0);
        if (attr && menuHorizontalPosition>0) {
          s_editMode = 0;
          if (event==EVT_KEY_LONG(KEY_ENTER)) {
            START_NO_HIGHLIGHT();
            for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
              memclear(&g_model.flightModeData[i], TRIMS_ARRAY_SIZE);
            }
            storageDirty(EE_MODEL);
            AUDIO_WARNING1();
          }
        }
        break;

      case ITEM_MODEL_DISPLAY_TRIMS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_DISPLAY_TRIMS);
        g_model.displayTrims = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, "\006No\0   ChangeYes", g_model.displayTrims, 0, 2, attr, event);
        break;

      case ITEM_MODEL_TRIM_INC:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TRIMINC);
        g_model.trimInc = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_THROTTLE_LABEL);
        break;

      case ITEM_MODEL_THROTTLE_REVERSED:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_THROTTLEREVERSE);
        g_model.throttleReversed = editCheckBox(g_model.throttleReversed, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_TRACE:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TTRACE);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.thrTraceSrc, NUM_POTS+NUM_CHNOUT);
        uint8_t idx = g_model.thrTraceSrc + MIXSRC_Thr;
        if (idx > MIXSRC_Thr)
          idx += 1;
        if (idx >= MIXSRC_FIRST_POT+NUM_POTS)
          idx += MIXSRC_CH1 - MIXSRC_FIRST_POT - NUM_POTS;
        putsMixerSource(MODEL_SETUP_2ND_COLUMN, y, idx, attr);
        break;
      }

      case ITEM_MODEL_THROTTLE_TRIM:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TTRIM);
        g_model.thrTrim = editCheckBox(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_PREFLIGHT_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PREFLIGHT);
        break;

      case ITEM_MODEL_CHECKLIST_DISPLAY:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CHECKLIST);
        g_model.displayChecklist = editCheckBox(g_model.displayChecklist, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_WARNING:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_THROTTLEWARNING);
        g_model.disableThrottleWarning = !editCheckBox(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, attr, event);
        break;

      case ITEM_MODEL_SWITCHES_WARNING:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SWITCHWARNING);
        if (!READ_ONLY() && attr && menuHorizontalPosition<0 && event==EVT_KEY_LONG(KEY_ENTER)) {
          killEvents(event);
          START_NO_HIGHLIGHT();
          getMovedSwitch();
          for (int i=0; i<NUM_SWITCHES; i++) {
            bool enabled = ((g_model.switchWarningState >> (3*i)) & 0x07);
            if (enabled) {
              g_model.switchWarningState &= ~(0x07 << (3*i));
              unsigned int newState = (switches_states >> (2*i) & 0x03) + 1;
              g_model.switchWarningState |= (newState << (3*i));
            }
          }
          AUDIO_WARNING1();
          storageDirty(EE_MODEL);
        }

        if (attr && menuHorizontalPosition < 0) {
          lcdDrawSolidFilledRect(MODEL_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN+1, (NUM_SWITCHES-1)*25+INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }

        unsigned int newStates = 0;
        for (int i=0, current=0; i<NUM_SWITCHES; i++) {
          if (SWITCH_WARNING_ALLOWED(i)) {
            if (!READ_ONLY() && attr && l_posHorz==current) {
              storageDirty(EE_MODEL);
            }
            unsigned int state = ((g_model.switchWarningState >> (3*i)) & 0x07);
            LcdFlags color = (state > 0 ? TEXT_COLOR : TEXT_DISABLE_COLOR);
            if (attr && menuHorizontalPosition < 0) {
              color |= INVERS;
            }
            char s[3];
            s[0] = 'A' + i;
            s[1] = "x\300-\301"[state];
            s[2] = '\0';
            lcdDrawText(MODEL_SETUP_2ND_COLUMN+i*25, y, s, color|(menuHorizontalPosition==current ? attr : 0));
            if (attr && menuHorizontalPosition==current) CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, state, 3, IS_3POS(i) ? 0 : isSwitchWarningStateAvailable);
            newStates |= (state << (3*i));
            ++current;
          }
        }
        g_model.switchWarningState = newStates;
        break;
      }

      case ITEM_MODEL_POT_WARNING:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_POTWARNING);
        if (attr) {
          if (menuHorizontalPosition) s_editMode = 0;
          if (!READ_ONLY() && menuHorizontalPosition) {
            switch (event) {
              case EVT_KEY_LONG(KEY_ENTER):
                killEvents(event);
                if (g_model.potsWarnMode == POTS_WARN_MANUAL) {
                  SAVE_POT_POSITION(menuHorizontalPosition-1);
                  AUDIO_WARNING1();
                  storageDirty(EE_MODEL);
                }
                break;
              case EVT_KEY_BREAK(KEY_ENTER):
                g_model.potsWarnEnabled ^= (1 << (menuHorizontalPosition-1));
                storageDirty(EE_MODEL);
                break;
            }
          }
        }
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, PSTR("\004""OFF\0""Man\0""Auto"), g_model.potsWarnMode, (menuHorizontalPosition == 0) ? attr : 0);
        if (g_model.potsWarnMode) {
          coord_t x = MODEL_SETUP_2ND_COLUMN+30;
          for (int i=0; i<NUM_POTS; ++i) {
            LcdFlags flags = (((menuHorizontalPosition==i+1) && attr) ? INVERS : 0);
            flags |= (g_model.potsWarnEnabled & (1 << i)) ? TEXT_DISABLE_COLOR : TEXT_COLOR;
            lcdDrawTextAtIndex(x, y, STR_VSRCRAW, NUM_STICKS+1+i, flags);
            x += 20;
          }
        }

        if (attr && (menuHorizontalPosition == 0)) {
          CHECK_INCDEC_MODELVAR(event, g_model.potsWarnMode, POTS_WARN_OFF, POTS_WARN_AUTO);
          storageDirty(EE_MODEL);
        }
        break;
      }

      case ITEM_MODEL_BEEP_CENTER:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BEEPCTR);
        lcdNextPos = MODEL_SETUP_2ND_COLUMN - 3;
        for (int i=0; i<NUM_STICKS+NUM_POTS; i++) {
          LcdFlags flags = ((menuHorizontalPosition==i && attr) ? INVERS : 0);
          flags |= (g_model.beepANACenter & ((BeepANACenter)1<<i)) ? TEXT_COLOR : (TEXT_DISABLE_COLOR | NO_FONTCACHE);
          if (attr && menuHorizontalPosition < 0) flags |= INVERS;
          lcdDrawTextAtIndex(lcdNextPos+3, y, STR_RETA123, i, flags);
        }
        if (attr && CURSOR_ON_CELL) {
          if (event==EVT_KEY_BREAK(KEY_ENTER)) {
            if (READ_ONLY_UNLOCKED()) {
              s_editMode = 0;
              g_model.beepANACenter ^= ((BeepANACenter)1<<menuHorizontalPosition);
              storageDirty(EE_MODEL);
            }
          }
        }
        break;

      case ITEM_MODEL_USE_GLOBAL_FUNCTIONS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_USE_GLOBAL_FUNCS);
        drawCheckBox(MODEL_SETUP_2ND_COLUMN, y, !g_model.noGlobalFunctions, attr);
        if (attr) g_model.noGlobalFunctions = !checkIncDecModel(event, !g_model.noGlobalFunctions, 0, 1);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, TR_INTERNALRF);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[0].rfProtocol, attr);
        if (attr) {
          g_model.moduleData[INTERNAL_MODULE].rfProtocol = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].rfProtocol, RF_PROTO_OFF, RF_PROTO_LAST, EE_MODEL, isRfProtocolAvailable);
          if (checkIncDec_Ret) {
            g_model.moduleData[0].type = MODULE_TYPE_XJT;
            g_model.moduleData[0].channelsStart = 0;
            g_model.moduleData[0].channelsCount = 0;
          }
        }
        break;

      case ITEM_MODEL_INTERNAL_MODULE_ANTENNA:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ANTENNASELECTION);
        newAntennaSel = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_VANTENNATYPES, g_model.moduleData[INTERNAL_MODULE].ppm.pulsePol, 0, 1, attr, event);
        if (newAntennaSel != g_model.moduleData[INTERNAL_MODULE].ppm.pulsePol && newAntennaSel == XJT_EXTERNAL_ANTENNA) {
          POPUP_CONFIRMATION(STR_ANTENNACONFIRM1);
          const char * w = STR_ANTENNACONFIRM2;
          SET_WARNING_INFO(w, strlen(w), 0);
        }
        else {
          g_model.moduleData[INTERNAL_MODULE].ppm.pulsePol = newAntennaSel;
        }
        break;

      case ITEM_MODEL_TRAINER_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODE);
        g_model.trainerMode = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_VTRAINERMODES, g_model.trainerMode, 0, HAS_WIRELESS_TRAINER_HARDWARE() ? TRAINER_MODE_MASTER_BATTERY_COMPARTMENT : TRAINER_MODE_SLAVE, attr, event);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, TR_EXTERNALRF);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_TARANIS_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].type, (menuHorizontalPosition==0 ? attr : 0));
        if (IS_MODULE_XJT(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[EXTERNAL_MODULE].rfProtocol, (menuHorizontalPosition==1 ? attr : 0));
        else if (IS_MODULE_DSM2(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_DSM_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, (menuHorizontalPosition==1 ? attr : 0));
#if defined(MULTIMODULE)
        else if (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) {
          int8_t multi_rfProto = min(g_model.moduleData[EXTERNAL_MODULE].multi.rfProtocol, (uint8_t) MM_RF_PROTO_CUSTOM);
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_MULTI_PROTOCOLS, multi_rfProto, menuHorizontalPosition==1 ? attr : 0);

          switch(multi_rfProto) {
            case MM_RF_PROTO_FLYSKY:
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, STR_SUBTYPE_FLYSKY, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_FRSKY:
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, STR_XJT_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].subType+1, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_HISKY:
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, STR_SUBTYPE_HISKY, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_DSM2:
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, STR_DSM_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].subType+1, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_YD717:
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, STR_SUBTYPE_YD717, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_KN:
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, STR_SUBTYPE_KN, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_SYMAX:
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, STR_SUBTYPE_SYMAX, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_CX10:
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, STR_SUBTYPE_CX10, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_CG023:
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, STR_SUBTYPE_CG023, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_MT99XX:
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, STR_SUBTYPE_MT99, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_MJXQ:
              lcdDrawTextAtIndex(MODEL_SETUP_4TH_COLUMN, y, STR_SUBTYPE_MJXQ, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_CUSTOM:
              lcdDrawNumber(MODEL_SETUP_4TH_COLUMN, y, g_model.moduleData[EXTERNAL_MODULE].multi.rfProtocol & 0x1f, menuHorizontalPosition==2 ? attr : 0, 2);
              lcdDrawNumber(MODEL_SETUP_4TH_COLUMN, y, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==3 ? attr : 0, 2);
              break;
          }
        }
#endif
        if (attr && s_editMode>0) {
          switch (menuHorizontalPosition) {
            case 0:
              g_model.moduleData[EXTERNAL_MODULE].type = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].type, MODULE_TYPE_NONE, MODULE_TYPE_COUNT-1, EE_MODEL, isModuleAvailable);
              if (checkIncDec_Ret) {
                g_model.moduleData[EXTERNAL_MODULE].rfProtocol = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                if (g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM)
                  g_model.moduleData[EXTERNAL_MODULE].channelsCount = 0;
                else
                  g_model.moduleData[EXTERNAL_MODULE].channelsCount = MAX_EXTERNAL_MODULE_CHANNELS();
              }
              break;
            case 1:
              if (IS_MODULE_DSM2(EXTERNAL_MODULE))
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, DSM2_PROTO_LP45, DSM2_PROTO_DSMX);
              else if (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) {
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].multi.rfProtocol, MM_RF_PROTO_FIRST, MM_RF_PROTO_LAST);
                if (checkIncDec_Ret)
                  g_model.moduleData[EXTERNAL_MODULE].subType=0;
              }
              else
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, RF_PROTO_X16, RF_PROTO_LAST);
              if (checkIncDec_Ret) {
                g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsCount = MAX_EXTERNAL_MODULE_CHANNELS();
              }
              break;
#if defined(MULTIMODULE)
            case 2:
              switch (min(g_model.moduleData[EXTERNAL_MODULE].multi.rfProtocol, (uint8_t) MM_RF_PROTO_CUSTOM)) {
                case MM_RF_PROTO_HISKY:
                case MM_RF_PROTO_DSM2:
                case MM_RF_PROTO_SYMAX:
                case MM_RF_PROTO_KN:
                case MM_RF_PROTO_FRSKY:
                  CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, 1);
                  break;
                case MM_RF_PROTO_CG023:
                case MM_RF_PROTO_MT99XX:
                  CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, 2);
                  break;
                case MM_RF_PROTO_FLYSKY:
                case MM_RF_PROTO_MJXQ:
                  CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, 3);
                  break;
                case MM_RF_PROTO_YD717:
                  CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, 4);
                  break;
                case MM_RF_PROTO_CX10:
                  CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, 7);
                  break;
                case MM_RF_PROTO_CUSTOM:
                  g_model.moduleData[EXTERNAL_MODULE].multi.rfProtocol = 0x20 | checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].multi.rfProtocol & 0x1f, 1, 31, EE_MODEL);
                  break;
              }
              break;
            case 3:
              // Custom protocol, third column is subtype
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, 7);
              break;
#endif
          }
        }
        break;

      case ITEM_MODEL_TRAINER_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TRAINER);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_CHANNELS:
      case ITEM_MODEL_EXTERNAL_MODULE_CHANNELS:
      case ITEM_MODEL_TRAINER_CHANNELS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CHANNELRANGE);
        if ((int8_t)PORT_CHANNELS_ROWS(moduleIdx) >= 0) {
          drawStringWithIndex(MODEL_SETUP_2ND_COLUMN, y, STR_CH, moduleData.channelsStart+1, menuHorizontalPosition==0 ? attr : 0);
          lcdDrawText(lcdNextPos+5, y, "-");
          drawStringWithIndex(lcdNextPos+5, y, STR_CH, moduleData.channelsStart+NUM_CHANNELS(moduleIdx), menuHorizontalPosition==1 ? attr : 0);
          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.channelsStart, 32-8-moduleData.channelsCount);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, moduleData.channelsCount, -4, min<int8_t>(MAX_CHANNELS(moduleIdx), 32-8-moduleData.channelsStart));
                if ((k == ITEM_MODEL_EXTERNAL_MODULE_CHANNELS && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM)
                    || (k == ITEM_MODEL_TRAINER_CHANNELS)
                    )
                  SET_DEFAULT_PPM_FRAME_LENGTH(moduleIdx);
                break;
            }
          }
        }
        break;
      }

      case ITEM_MODEL_INTERNAL_MODULE_BIND:
      case ITEM_MODEL_EXTERNAL_MODULE_BIND:
      case ITEM_MODEL_TRAINER_SETTINGS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        if (IS_MODULE_PPM(moduleIdx)) {
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PPMFRAME);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppm.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT, 0, NULL, STR_MS);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN+80, y, (moduleData.ppm.delay*50)+300, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr|LEFT : LEFT, 0, NULL, "us");
          lcdDrawText(MODEL_SETUP_2ND_COLUMN+160, y, moduleData.ppm.pulsePol ? "+" : "-", (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);
          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR(event, moduleData.ppm.frameLength, -20, 35);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, moduleData.ppm.delay, -4, 10);
                break;
              case 2:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.ppm.pulsePol, 1);
                break;
            }
          }
        }
        else {
          int l_posHorz = menuHorizontalPosition;
          coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
          if (IS_MODULE_XJT(moduleIdx) && IS_D8_RX(moduleIdx)) {
            xOffsetBind = 0;
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_RECEIVER);
            if (attr) l_posHorz += 1;
          }
          else {
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_RECEIVER_NUM);
          }
          if (IS_MODULE_XJT(moduleIdx) || IS_MODULE_DSM2(moduleIdx) || IS_MODULE_MULTIMODULE(moduleIdx)) {
            if (xOffsetBind) lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[moduleIdx], (l_posHorz==0 ? attr : 0) | LEADING0 | LEFT, 2);
            if (attr && l_posHorz==0 && s_editMode>0) {
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[moduleIdx],
                                         IS_MODULE_DSM2(moduleIdx) ? 20 :
                                         IS_MODULE_MULTIMODULE(moduleIdx) ? 15 : 63);
            }
            drawButton(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, (moduleFlag[moduleIdx] == MODULE_BIND ? BUTTON_ON : BUTTON_OFF) | (l_posHorz==1 ? attr : 0));
            drawButton(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, (moduleFlag[moduleIdx] == MODULE_RANGECHECK ? BUTTON_ON : BUTTON_OFF) | (l_posHorz==2 ? attr : 0));
            uint8_t newFlag = 0;
            if (attr && l_posHorz>0 && s_editMode>0) {
              if (l_posHorz == 1)
                newFlag = MODULE_BIND;
              else if (l_posHorz == 2) {
                newFlag = MODULE_RANGECHECK;
              }
            }
            moduleFlag[moduleIdx] = newFlag;
          }
        }
        break;
      }

      case ITEM_MODEL_INTERNAL_MODULE_FAILSAFE:
      case ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcdDrawText(MENUS_MARGIN_LEFT, y, TR_FAILSAFE);
        if (IS_MODULE_XJT(moduleIdx)) {
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VFAILSAFE, moduleData.failsafeMode, menuHorizontalPosition==0 ? attr : 0);
          if (moduleData.failsafeMode == FAILSAFE_CUSTOM) {
            drawButton(MODEL_SETUP_2ND_COLUMN + MODEL_SETUP_SET_FAILSAFE_OFS, y, STR_SET, menuHorizontalPosition==1 ? attr : 0);
          }
          if (attr) {
            if (moduleData.failsafeMode != FAILSAFE_CUSTOM)
              menuHorizontalPosition = 0;
            if (menuHorizontalPosition==0) {
              if (s_editMode>0) {
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.failsafeMode, FAILSAFE_LAST);
                if (checkIncDec_Ret) SEND_FAILSAFE_NOW(moduleIdx);
              }
            }
            else if (menuHorizontalPosition==1) {
              s_editMode = 0;
              if (moduleData.failsafeMode==FAILSAFE_CUSTOM && event==EVT_KEY_FIRST(KEY_ENTER)) {
                g_moduleIdx = moduleIdx;
                pushMenu(menuModelFailsafe);
              }
            }
            else {
              lcdDrawSolidFilledRect(MODEL_SETUP_2ND_COLUMN, y, LCD_W-MODEL_SETUP_2ND_COLUMN-2, 8, TEXT_COLOR);
            }
          }
        }
#if defined(MULTIMODULE)
        else if (IS_MODULE_MULTIMODULE(moduleIdx)) {
          switch (g_model.moduleData[moduleIdx].multi.rfProtocol) {
            case MM_RF_PROTO_FRSKY:
              lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTI_RFTUNE);
              break;
            case MM_RF_PROTO_HUBSAN:
              lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTI_VIDFREQ);
              break;
            case MM_RF_PROTO_DSM2:
              lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTI_DSMFRAME);
              g_model.moduleData[moduleIdx].multi.optionValue = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_OPTIONS_DSM, g_model.moduleData[moduleIdx].multi.optionValue, 0, 12, attr, event);
              break;
            default:
              lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTI_OPTION);
              break;
          }
          if (g_model.moduleData[moduleIdx].multi.rfProtocol != MM_RF_PROTO_DSM2) {
            lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.moduleData[moduleIdx].multi.optionValue, LEFT | attr);
            if (attr)
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].multi.optionValue, -128, 127);
          }
        }
#endif
        break;
      }
#if defined(MULTIMODULE)
    case ITEM_MODEL_EXTERNAL_MODULE_AUTOBIND:
      lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTI_AUTOBIND);
      g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode, MODEL_SETUP_2ND_COLUMN, y, attr, event);
      break;
    case  ITEM_MODEL_EXTERNAL_MODULE_LOWPOWER:
      lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTI_LOWPOWER);
      g_model.moduleData[EXTERNAL_MODULE].multi.lowPowerMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.lowPowerMode, MODEL_SETUP_2ND_COLUMN, y, attr, event);
      break;
#endif
    }
  }

  if (IS_RANGECHECK_ENABLE()) {
    theme->drawMessageBox("RSSI :", NULL, NULL, WARNING_TYPE_INFO);
    lcdDrawNumber(WARNING_LINE_X, WARNING_INFOLINE_Y, TELEMETRY_RSSI(), DBLSIZE|LEFT);
  }

  return true;
}

bool menuModelFailsafe(evt_t event)
{
  uint8_t ch = 0;
  uint8_t channelStart = g_model.moduleData[g_moduleIdx].channelsStart;

  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    event = 0;
    if (s_editMode) {
      g_model.moduleData[g_moduleIdx].failsafeChannels[menuVerticalPosition] = channelOutputs[menuVerticalPosition+channelStart];
      storageDirty(EE_MODEL);
      AUDIO_WARNING1();
      s_editMode = 0;
      SEND_FAILSAFE_NOW(g_moduleIdx);
    }
    else {
      int16_t & failsafe = g_model.moduleData[g_moduleIdx].failsafeChannels[menuVerticalPosition];
      if (failsafe < FAILSAFE_CHANNEL_HOLD)
        failsafe = FAILSAFE_CHANNEL_HOLD;
      else if (failsafe == FAILSAFE_CHANNEL_HOLD)
        failsafe = FAILSAFE_CHANNEL_NOPULSE;
      else
        failsafe = 0;
      storageDirty(EE_MODEL);
      AUDIO_WARNING1();
      SEND_FAILSAFE_NOW(g_moduleIdx);
    }
  }

  SIMPLE_SUBMENU_WITH_OPTIONS("FAILSAFE", ICON_STATS_ANALOGS, NUM_CHANNELS(g_moduleIdx), OPTION_MENU_NO_SCROLLBAR);
  drawStringWithIndex(50, 3+FH, "Module", g_moduleIdx+1, MENU_TITLE_COLOR);

  #define COL_W   (LCD_W/2)
  const uint8_t SLIDER_W = 128;

  unsigned int lim = g_model.extendedLimits ? 640*2 : 512*2;

  for (uint8_t col=0; col<2; col++) {
    for (uint8_t line=0; line<8; line++) {
      coord_t x = col*(LCD_W/2);
      coord_t y = MENU_CONTENT_TOP - FH + line*(FH+4);
      int32_t channelValue = channelOutputs[ch+channelStart];
      int32_t failsafeValue = 0;
      bool failsafeEditable = false;

      if (ch < NUM_CHANNELS(g_moduleIdx)) {
        failsafeValue = g_model.moduleData[g_moduleIdx].failsafeChannels[8*col+line];
        failsafeEditable = true;
      }

      if (failsafeEditable) {
        // Channel name if present, number if not
        uint8_t lenLabel = ZLEN(g_model.limitData[ch+channelStart].name);
        if (lenLabel > 0) {
          putsChn(x+MENUS_MARGIN_LEFT, y-3, ch+1, TINSIZE);
          lcdDrawSizedText(x+MENUS_MARGIN_LEFT, y+5, g_model.limitData[ch+channelStart].name, sizeof(g_model.limitData[ch+channelStart].name), ZCHAR|SMLSIZE);
        }
        else {
          putsChn(x+MENUS_MARGIN_LEFT, y, ch+1, 0);
        }

        // Value
        LcdFlags flags = RIGHT;
        if (menuVerticalPosition == ch) {
          flags |= INVERS;
          if (s_editMode) {
            if (failsafeValue == FAILSAFE_CHANNEL_HOLD || failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
              s_editMode = 0;
            }
            else {
              flags |= BLINK;
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[g_moduleIdx].failsafeChannels[8*col+line], -lim, +lim);
            }
          }
        }

        x += COL_W-4-MENUS_MARGIN_LEFT-SLIDER_W;

        if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
          lcdDrawText(x, y+2, "HOLD", flags|SMLSIZE);
          failsafeValue = 0;
        }
        else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
          lcdDrawText(x, y+2, "NONE", flags|SMLSIZE);
          failsafeValue = 0;
        }
        else {
#if defined(PPM_UNIT_US)
          lcdDrawNumber(x, y, PPM_CH_CENTER(ch)+failsafeValue/2, flags);
#elif defined(PPM_UNIT_PERCENT_PREC1)
          lcdDrawNumber(x, y, calcRESXto1000(failsafeValue), PREC1|flags);
#else
          lcdDrawNumber(x, y, calcRESXto1000(failsafeValue)/10, flags);
#endif
        }

        // Gauge
        x += 4;
        lcdDrawRect(x, y+3, SLIDER_W+1, 12);
        unsigned int lenChannel = limit((uint8_t)1, uint8_t((abs(channelValue) * SLIDER_W/2 + lim/2) / lim), uint8_t(SLIDER_W/2));
        unsigned int lenFailsafe = limit((uint8_t)1, uint8_t((abs(failsafeValue) * SLIDER_W/2 + lim/2) / lim), uint8_t(SLIDER_W/2));
        x += SLIDER_W/2;
        coord_t xChannel = (channelValue>0) ? x : x+1-lenChannel;
        coord_t xFailsafe = (failsafeValue>0) ? x : x+1-lenFailsafe;
        lcdDrawSolidFilledRect(xChannel, y+4, lenChannel, 5, TEXT_COLOR);
        lcdDrawSolidFilledRect(xFailsafe, y+9, lenFailsafe, 5, ALARM_COLOR);
      }
      ch++;
    }
  }

  return true;
}
