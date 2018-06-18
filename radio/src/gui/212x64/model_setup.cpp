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

enum MenuModelSetupItems {
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
#if defined(PCBX9E)
  ITEM_MODEL_TOP_LCD_TIMER,
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
#if defined(PCBX9E)
  ITEM_MODEL_SWITCHES_WARNING2,
  ITEM_MODEL_SWITCHES_WARNING3,
#endif
  ITEM_MODEL_POTS_WARNING,
#if defined(PCBX9E)
  ITEM_MODEL_POTS_WARNING2,
#endif
  ITEM_MODEL_BEEP_CENTER,
  ITEM_MODEL_USE_GLOBAL_FUNCTIONS,
  ITEM_MODEL_INTERNAL_MODULE_LABEL,
  ITEM_MODEL_INTERNAL_MODULE_MODE,
  ITEM_MODEL_INTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_INTERNAL_MODULE_BIND,
  ITEM_MODEL_INTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_EXTERNAL_MODULE_LABEL,
  ITEM_MODEL_EXTERNAL_MODULE_MODE,
#if defined (MULTIMODULE)
  ITEM_MODEL_EXTERNAL_MODULE_STATUS,
  ITEM_MODEL_EXTERNAL_MODULE_SYNCSTATUS,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_EXTERNAL_MODULE_BIND,
  ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_EXTERNAL_MODULE_OPTIONS,
#if defined(MULTIMODULE)
  ITEM_MODEL_EXTERNAL_MODULE_AUTOBIND,
#endif
  ITEM_MODEL_EXTERNAL_MODULE_POWER,
  ITEM_MODEL_TRAINER_LABEL,
  ITEM_MODEL_TRAINER_MODE,
  ITEM_MODEL_TRAINER_LINE1,
  ITEM_MODEL_TRAINER_LINE2,
  ITEM_MODEL_SETUP_MAX
};

#define FIELD_PROTOCOL_MAX 1

#define MODEL_SETUP_2ND_COLUMN        (LCD_W-17*FW-MENUS_SCROLLBAR_WIDTH-1)
#define MODEL_SETUP_3RD_COLUMN        (MODEL_SETUP_2ND_COLUMN+6*FW)
#define MODEL_SETUP_BIND_OFS          3*FW-2
#define MODEL_SETUP_RANGE_OFS         7*FW
#define MODEL_SETUP_SET_FAILSAFE_OFS  10*FW-2

#define CURRENT_MODULE_EDITED(k)         (k>=ITEM_MODEL_TRAINER_LABEL ? TRAINER_MODULE : (k>=ITEM_MODEL_EXTERNAL_MODULE_LABEL ? EXTERNAL_MODULE : INTERNAL_MODULE))

void onBindMenu(const char * result)
{
  uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition);

  if (result == STR_BINDING_1_8_TELEM_ON) {
    g_model.moduleData[moduleIdx].pxx.receiver_telem_off = false;
    g_model.moduleData[moduleIdx].pxx.receiver_channel_9_16 = false;
  }
  else if (result == STR_BINDING_1_8_TELEM_OFF) {
    g_model.moduleData[moduleIdx].pxx.receiver_telem_off = true;
    g_model.moduleData[moduleIdx].pxx.receiver_channel_9_16 = false;
  }
  else if (result == STR_BINDING_9_16_TELEM_ON) {
    g_model.moduleData[moduleIdx].pxx.receiver_telem_off = false;
    g_model.moduleData[moduleIdx].pxx.receiver_channel_9_16 = true;
  }
  else if (result == STR_BINDING_9_16_TELEM_OFF) {
    g_model.moduleData[moduleIdx].pxx.receiver_telem_off = true;
    g_model.moduleData[moduleIdx].pxx.receiver_channel_9_16 = true;
  }
  else {
    return;
  }

  moduleFlag[moduleIdx] = MODULE_BIND;
}

void copySelection(char * dst, const char * src, uint8_t size)
{
  if (memcmp(src, "---", 3) == 0)
    memset(dst, 0, size);
  else
    memcpy(dst, src, size);
}

void onModelSetupBitmapMenu(const char * result)
{
  if (result == STR_UPDATE_LIST) {
    if (!sdListFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap), NULL)) {
      POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
    }
  }
  else {
    // The user choosed a bmp file in the list
    copySelection(g_model.header.bitmap, result, sizeof(g_model.header.bitmap));
    memcpy(modelHeaders[g_eeGeneral.currModel].bitmap, g_model.header.bitmap, sizeof(g_model.header.bitmap));
    storageDirty(EE_MODEL);
  }
}

void editTimerMode(int timerIdx, coord_t y, LcdFlags attr, event_t event)
{
  TimerData & timer = g_model.timers[timerIdx];
  drawStringWithIndex(0*FW, y, STR_TIMER, timerIdx+1);
  drawTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer.mode, menuHorizontalPosition==0 ? attr : 0);
  drawTimer(MODEL_SETUP_3RD_COLUMN, y, timer.start, menuHorizontalPosition==1 ? attr|TIMEHOUR : TIMEHOUR, menuHorizontalPosition==2 ? attr|TIMEHOUR : TIMEHOUR);
  if (attr && menuHorizontalPosition < 0) {
    lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, 13*FW-3, FH+1);
  }
  if (attr && s_editMode>0) {
    div_t qr = div(timer.start, 60);
    switch (menuHorizontalPosition) {
      case 0:
      {
        swsrc_t timerMode = timer.mode;
        if (timerMode < 0) timerMode -= TMRMODE_COUNT-1;
        CHECK_INCDEC_MODELVAR_CHECK(event, timerMode, -TMRMODE_COUNT-SWSRC_LAST+1, TMRMODE_COUNT+SWSRC_LAST-1, isSwitchAvailableInTimers);
        if (timerMode < 0) timerMode += TMRMODE_COUNT-1;
        timer.mode = timerMode;
#if defined(AUTOSWITCH)
        if (s_editMode>0) {
          swsrc_t val = timer.mode - (TMRMODE_COUNT-1);
          swsrc_t switchVal = checkIncDecMovedSwitch(val);
          if (val != switchVal) {
            timer.mode = switchVal + (TMRMODE_COUNT-1);
            storageDirty(EE_MODEL);
          }
        }
#endif
        break;
      }
      case 1:
        qr.quot = checkIncDec(event, qr.quot, 0, 1439, EE_MODEL | NO_INCDEC_MARKS); // 23h59
        timer.start = qr.rem + qr.quot*60;
        break;
      case 2:
        qr.rem -= checkIncDecModel(event, qr.rem+2, 1, 62)-2;
        timer.start -= qr.rem ;
        if ((int16_t)timer.start < 0) timer.start=0;
        if ((int32_t)timer.start > 86399) timer.start=86399; // 23h59:59
        break;
    }
  }
}

void editTimerCountdown(int timerIdx, coord_t y, LcdFlags attr, event_t event)
{
  TimerData & timer = g_model.timers[timerIdx];
  lcdDrawTextAlignedLeft(y, STR_BEEPCOUNTDOWN);
  lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VBEEPCOUNTDOWN, timer.countdownBeep, (menuHorizontalPosition==0 ? attr : 0));
  if (timer.countdownBeep != COUNTDOWN_SILENT) {
    lcdDrawNumber(MODEL_SETUP_3RD_COLUMN, y, TIMER_COUNTDOWN_START(timerIdx), (menuHorizontalPosition == 1 ? attr : 0) | LEFT);
    lcdDrawChar(lcdLastRightPos, y, 's');
  }
  if (attr && s_editMode>0) {
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

#define IF_INTERNAL_MODULE_ON(x)        (IS_INTERNAL_MODULE_ENABLED() ? (uint8_t)(x) : HIDDEN_ROW)
#if defined(TARANIS_INTERNAL_PPM)
  #define INTERNAL_MODULE_MODE_ROWS       (IS_MODULE_XJT(INTERNAL_MODULE) ? (uint8_t)1 : (uint8_t)0) // Module type + RF protocols
#else
  #define INTERNAL_MODULE_MODE_ROWS       0 // (OFF / RF protocols)
#endif
#define IF_EXTERNAL_MODULE_ON(x)          (IS_EXTERNAL_MODULE_ENABLED() ? (uint8_t)(x) : HIDDEN_ROW)
#define INTERNAL_MODULE_CHANNELS_ROWS     IF_INTERNAL_MODULE_ON(1)
#define PORT_CHANNELS_ROWS(x)             (x==INTERNAL_MODULE ? INTERNAL_MODULE_CHANNELS_ROWS : (x==EXTERNAL_MODULE ? EXTERNAL_MODULE_CHANNELS_ROWS : 1))

#if defined(BLUETOOTH) && defined(USEHORUSBT)
  #define TRAINER_LINE1_BLUETOOTH_M_ROWS    ((bluetoothDistantAddr[0] == 0 || bluetoothState == BLUETOOTH_STATE_CONNECTED) ? (uint8_t)0 : (uint8_t)1)
  #define TRAINER_LINE1_ROWS                (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)1 : (g_model.trainerMode == TRAINER_MODE_MASTER_BLUETOOTH ? TRAINER_LINE1_BLUETOOTH_M_ROWS : (g_model.trainerMode == TRAINER_MODE_SLAVE_BLUETOOTH ? (uint8_t)1 : HIDDEN_ROW)))
  #define TRAINER_LINE2_ROWS                (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)2 : HIDDEN_ROW)
#else
  #define TRAINER_LINE1_ROWS                (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)1 : HIDDEN_ROW)
  #define TRAINER_LINE2_ROWS                (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)2 : HIDDEN_ROW)
#endif

#define TIMER_ROWS(x)                     2|NAVIGATION_LINE_BY_LINE, 0, 0, 0, g_model.timers[x].countdownBeep != COUNTDOWN_SILENT ? (uint8_t) 1 : (uint8_t)0

#define EXTERNAL_MODULE_MODE_ROWS         (IS_MODULE_PXX(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE)) ? (uint8_t)1 : IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? MULTIMODULE_MODE_ROWS(EXTERNAL_MODULE) : (uint8_t)0

#if TIMERS == 1
  #define TIMERS_ROWS                     TIMER_ROWS(0)
#elif TIMERS == 2
  #define TIMERS_ROWS                     TIMER_ROWS(0), TIMER_ROWS(1)
#elif TIMERS == 3
  #define TIMERS_ROWS                     TIMER_ROWS(0), TIMER_ROWS(1), TIMER_ROWS(2)
#endif
#if defined(PCBX9E)
  #define SW_WARN_ITEMS()                 uint8_t(NAVIGATION_LINE_BY_LINE|(getSwitchWarningsCount()-1)), uint8_t(getSwitchWarningsCount() > 8 ? TITLE_ROW : HIDDEN_ROW), uint8_t(getSwitchWarningsCount() > 16 ? TITLE_ROW : HIDDEN_ROW)
  #define POT_WARN_ITEMS()                uint8_t(g_model.potsWarnMode ? NAVIGATION_LINE_BY_LINE|(NUM_POTS+NUM_SLIDERS) : 0), uint8_t(g_model.potsWarnMode ? TITLE_ROW : HIDDEN_ROW)
  #define TOPLCD_ROWS                     0,
#else
  #define SW_WARN_ITEMS()                 uint8_t(NAVIGATION_LINE_BY_LINE|getSwitchWarningsCount())
  #define POT_WARN_ITEMS()                uint8_t(g_model.potsWarnMode ? NAVIGATION_LINE_BY_LINE|(NUM_POTS+NUM_SLIDERS) : 0)
  #define TOPLCD_ROWS
#endif


void menuModelSetup(event_t event)
{
  horzpos_t l_posHorz = menuHorizontalPosition;
  bool CURSOR_ON_CELL = (menuHorizontalPosition >= 0);

  int8_t old_editMode = s_editMode;
  MENU_TAB({ 0, 0, TIMERS_ROWS, TOPLCD_ROWS 0, 1, 0, 0,
    LABEL(Throttle), 0, 0, 0,
    LABEL(PreflightCheck), 0, 0, SW_WARN_ITEMS(), POT_WARN_ITEMS(), NAVIGATION_LINE_BY_LINE|(NUM_STICKS+NUM_POTS+NUM_SLIDERS+NUM_ROTARY_ENCODERS-1), 0,
    LABEL(InternalModule),
    INTERNAL_MODULE_MODE_ROWS,
    INTERNAL_MODULE_CHANNELS_ROWS,
    IF_INTERNAL_MODULE_ON(HAS_RF_PROTOCOL_MODELINDEX(g_model.moduleData[INTERNAL_MODULE].rfProtocol) ? (uint8_t)2 : (uint8_t)1),
    IF_INTERNAL_MODULE_ON(FAILSAFE_ROWS(INTERNAL_MODULE)),
    LABEL(ExternalModule),
    EXTERNAL_MODULE_MODE_ROWS,
    MULTIMODULE_STATUS_ROWS
    EXTERNAL_MODULE_CHANNELS_ROWS,
    ((IS_MODULE_XJT(EXTERNAL_MODULE) && !HAS_RF_PROTOCOL_FAILSAFE(g_model.moduleData[EXTERNAL_MODULE].rfProtocol)) || IS_MODULE_SBUS(EXTERNAL_MODULE)) ? (uint8_t)1 : (IS_MODULE_PPM(EXTERNAL_MODULE) || IS_MODULE_PXX(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE) || IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW,
    FAILSAFE_ROWS(EXTERNAL_MODULE),
    EXTERNAL_MODULE_OPTION_ROW,
    MULTIMODULE_MODULE_ROWS
    EXTERNAL_MODULE_POWER_ROW,
    LABEL(Trainer), 0, TRAINER_LINE1_ROWS, TRAINER_LINE2_ROWS});

  MENU_CHECK(STR_MENUSETUP, menuTabModel, MENU_MODEL_SETUP, ITEM_MODEL_SETUP_MAX);

  if (event == EVT_ENTRY) {
    reusableBuffer.modelsetup.r9mPower = g_model.moduleData[EXTERNAL_MODULE].pxx.power;
  }

#if (defined(DSM2) || defined(PXX))
  if (menuEvent) {
    moduleFlag[0] = 0;
    moduleFlag[1] = 0;
  }
#endif

  int sub = menuVerticalPosition;

  for (int i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j] == HIDDEN_ROW)
        k++;
    }

    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_MODEL_NAME:
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_MODELNAME, g_model.header.name, sizeof(g_model.header.name), event, attr);
        memcpy(modelHeaders[g_eeGeneral.currModel].name, g_model.header.name, sizeof(g_model.header.name));
        break;

      case ITEM_MODEL_BITMAP:
        lcdDrawTextAlignedLeft(y, STR_BITMAP);
        if (ZEXIST(g_model.header.bitmap))
          lcdDrawSizedText(MODEL_SETUP_2ND_COLUMN, y, g_model.header.bitmap, sizeof(g_model.header.bitmap), attr);
        else
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VCSWFUNC, 0, attr);
        if (attr && event==EVT_KEY_BREAK(KEY_ENTER) && READ_ONLY_UNLOCKED()) {
          s_editMode = 0;
          if (sdListFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap), g_model.header.bitmap, LIST_NONE_SD_FILE)) {
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
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_TIMER_NAME, g_model.timers[0].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_TIMER1_MINUTE_BEEP:
        g_model.timers[0].minuteBeep = editCheckBox(g_model.timers[0].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
        break;

      case ITEM_MODEL_TIMER1_COUNTDOWN_BEEP:
        editTimerCountdown(0, y, attr, event);
        break;

      case ITEM_MODEL_TIMER1_PERSISTENT:
        g_model.timers[0].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, g_model.timers[0].persistent, 0, 2, attr, event);
        break;

#if TIMERS > 1
      case ITEM_MODEL_TIMER2:
        editTimerMode(1, y, attr, event);
        break;

      case ITEM_MODEL_TIMER2_NAME:
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_TIMER_NAME, g_model.timers[1].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_TIMER2_MINUTE_BEEP:
        g_model.timers[1].minuteBeep = editCheckBox(g_model.timers[1].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
        break;

      case ITEM_MODEL_TIMER2_COUNTDOWN_BEEP:
        editTimerCountdown(1, y, attr, event);
        break;

      case ITEM_MODEL_TIMER2_PERSISTENT:
        g_model.timers[1].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, g_model.timers[1].persistent, 0, 2, attr, event);
        break;
#endif

#if TIMERS > 2
      case ITEM_MODEL_TIMER3:
        editTimerMode(2, y, attr, event);
        break;

      case ITEM_MODEL_TIMER3_NAME:
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_TIMER_NAME, g_model.timers[2].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_TIMER3_MINUTE_BEEP:
        g_model.timers[2].minuteBeep = editCheckBox(g_model.timers[2].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
        break;

      case ITEM_MODEL_TIMER3_COUNTDOWN_BEEP:
        editTimerCountdown(2, y, attr, event);
        break;

      case ITEM_MODEL_TIMER3_PERSISTENT:
        g_model.timers[2].persistent = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, g_model.timers[2].persistent, 0, 2, attr, event);
        break;
#endif

#if defined(PCBX9E)
      case ITEM_MODEL_TOP_LCD_TIMER:
        lcdDrawTextAlignedLeft(y, STR_TOPLCDTIMER);
        drawStringWithIndex(MODEL_SETUP_2ND_COLUMN, y, STR_TIMER, g_model.toplcdTimer+1, attr);
        if (attr) {
          g_model.toplcdTimer = checkIncDec(event, g_model.toplcdTimer, 0, TIMERS-1, EE_MODEL);
        }
        break;
#endif

      case ITEM_MODEL_EXTENDED_LIMITS:
        ON_OFF_MENU_ITEM(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, STR_ELIMITS, attr, event);
        break;

      case ITEM_MODEL_EXTENDED_TRIMS:
        ON_OFF_MENU_ITEM(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, STR_ETRIMS, menuHorizontalPosition<=0 ? attr : 0, event==EVT_KEY_BREAK(KEY_ENTER) ? event : 0);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_RESET_BTN, (menuHorizontalPosition>0  && !NO_HIGHLIGHT()) ? attr : 0);
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
        g_model.displayTrims = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_DISPLAY_TRIMS, STR_VDISPLAYTRIMS, g_model.displayTrims, 0, 2, attr, event);
        break;

      case ITEM_MODEL_TRIM_INC:
        g_model.trimInc = editChoice(MODEL_SETUP_2ND_COLUMN, y, STR_TRIMINC, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_LABEL:
        lcdDrawTextAlignedLeft(y, STR_THROTTLE_LABEL);
        break;

      case ITEM_MODEL_THROTTLE_REVERSED:
        ON_OFF_MENU_ITEM(g_model.throttleReversed, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEREVERSE, attr, event ) ;
        break;

      case ITEM_MODEL_THROTTLE_TRACE:
      {
        lcdDrawTextAlignedLeft(y, STR_TTRACE);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, g_model.thrTraceSrc, NUM_POTS+NUM_SLIDERS+MAX_OUTPUT_CHANNELS, isThrottleSourceAvailable);
        uint8_t idx = g_model.thrTraceSrc + MIXSRC_Thr;
        if (idx > MIXSRC_Thr)
          idx += 1;
        if (idx >= MIXSRC_FIRST_POT+NUM_POTS+NUM_SLIDERS)
          idx += MIXSRC_CH1 - MIXSRC_FIRST_POT - NUM_POTS - NUM_SLIDERS;
        drawSource(MODEL_SETUP_2ND_COLUMN, y, idx, attr);
        break;
      }

      case ITEM_MODEL_THROTTLE_TRIM:
        ON_OFF_MENU_ITEM(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, STR_TTRIM, attr, event);
        break;

      case ITEM_MODEL_PREFLIGHT_LABEL:
        lcdDrawTextAlignedLeft(y, STR_PREFLIGHT);
        break;

      case ITEM_MODEL_CHECKLIST_DISPLAY:
        ON_OFF_MENU_ITEM(g_model.displayChecklist, MODEL_SETUP_2ND_COLUMN, y, STR_CHECKLIST, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_WARNING:
        g_model.disableThrottleWarning = !editCheckBox(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEWARNING, attr, event);
        break;

#if defined(PCBX9E)
      case ITEM_MODEL_SWITCHES_WARNING2:
      case ITEM_MODEL_SWITCHES_WARNING3:
      case ITEM_MODEL_POTS_WARNING2:
        if (i==0) {
          if (CURSOR_MOVED_LEFT(event))
            menuVerticalOffset--;
          else
            menuVerticalOffset++;
        }
        break;
#endif

      case ITEM_MODEL_SWITCHES_WARNING:
      {
#if defined(PCBX9E)
        if (i>=NUM_BODY_LINES-2 && getSwitchWarningsCount() > 8*(NUM_BODY_LINES-i)) {
          if (CURSOR_MOVED_LEFT(event))
            menuVerticalOffset--;
          else
            menuVerticalOffset++;
          break;
        }
#endif
        lcdDrawTextAlignedLeft(y, STR_SWITCHWARNING);
        swarnstate_t states = g_model.switchWarningState;
        char c;
        if (attr) {
          s_editMode = 0;
          if (!READ_ONLY()) {
            switch (event) {
              case EVT_KEY_BREAK(KEY_ENTER):
                break;

              case EVT_KEY_LONG(KEY_ENTER):
                if (menuHorizontalPosition < 0) {
                  START_NO_HIGHLIGHT();
                  getMovedSwitch();
                  g_model.switchWarningState = switches_states;
                  AUDIO_WARNING1();
                  storageDirty(EE_MODEL);
                }
                killEvents(event);
                break;
            }
          }
        }

        LcdFlags line = attr;

        int current = 0;
        for (int i=0; i<NUM_SWITCHES; i++) {
          if (SWITCH_WARNING_ALLOWED(i)) {
            div_t qr = div(current, 8);
            if (!READ_ONLY() && event==EVT_KEY_BREAK(KEY_ENTER) && line && l_posHorz==current) {
              g_model.switchWarningEnable ^= (1 << i);
              storageDirty(EE_MODEL);
            }
            uint8_t swactive = !(g_model.switchWarningEnable & (1<<i));
            c = "\300-\301"[states & 0x03];
            lcdDrawChar(MODEL_SETUP_2ND_COLUMN+qr.rem*(2*FW+1), y+FH*qr.quot, 'A'+i, line && (menuHorizontalPosition==current) ? INVERS : 0);
            if (swactive) lcdDrawChar(lcdNextPos, y+FH*qr.quot, c);
            ++current;
          }
          states >>= 2;
        }
        if (attr && menuHorizontalPosition < 0) {
#if defined(PCBX9E)
          lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, 8*(2*FW+1), 1+FH*((current+7)/8));
#else
          lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, current*(2*FW+1), FH+1);
#endif
        }
        break;
      }

      case ITEM_MODEL_POTS_WARNING:
#if defined(PCBX9E)
        if (i==NUM_BODY_LINES-1 && g_model.potsWarnMode) {
          if (CURSOR_MOVED_LEFT(event))
            menuVerticalOffset--;
          else
            menuVerticalOffset++;
          break;
        }
#endif

        lcdDrawTextAlignedLeft(y, STR_POTWARNING);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, PSTR("\004""OFF\0""Man\0""Auto"), g_model.potsWarnMode, (menuHorizontalPosition == 0) ? attr : 0);
        if (attr && (menuHorizontalPosition == 0)) {
          CHECK_INCDEC_MODELVAR(event, g_model.potsWarnMode, POTS_WARN_OFF, POTS_WARN_AUTO);
          storageDirty(EE_MODEL);
        }

        if (attr) {
          if (menuHorizontalPosition > 0) s_editMode = 0;
          if (!READ_ONLY() && menuHorizontalPosition > 0) {
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
        if (g_model.potsWarnMode) {
          coord_t x = MODEL_SETUP_2ND_COLUMN+28;
          for (int i=0; i<NUM_POTS+NUM_SLIDERS; ++i) {
            if (i<NUM_XPOTS && !IS_POT_SLIDER_AVAILABLE(POT1+i)) {
              if (attr && (menuHorizontalPosition==i+1)) REPEAT_LAST_CURSOR_MOVE();
            }
            else {
#if defined(PCBX9E)
              if (i == NUM_XPOTS) {
                y += FH;
                x = MODEL_SETUP_2ND_COLUMN;
              }
#endif
              LcdFlags flags = ((menuHorizontalPosition==i+1) && attr) ? BLINK : 0;
              if ((!attr || menuHorizontalPosition >= 0) && !(g_model.potsWarnEnabled & (1 << i))) {
                flags |= INVERS;
              }

              // TODO add a new function
              lcdDrawSizedText(x, y, STR_VSRCRAW+2+STR_VSRCRAW[0]*(NUM_STICKS+1+i), STR_VSRCRAW[0]-1, flags & ~ZCHAR);
              x = lcdNextPos+3;
            }
          }
        }
        if (attr && menuHorizontalPosition < 0) {
#if defined(PCBX9E)
          lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-FH-1, LCD_W-MODEL_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH+1, 2*FH+1);
#else
          lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, LCD_W-MODEL_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH+1, FH+1);
#endif
        }
        break;

      case ITEM_MODEL_BEEP_CENTER:
      {
        lcdDrawTextAlignedLeft(y, STR_BEEPCTR);
        coord_t x = MODEL_SETUP_2ND_COLUMN;
        for (int i=0; i<NUM_STICKS+NUM_POTS+NUM_SLIDERS+NUM_ROTARY_ENCODERS; i++) {
          if (i>=POT1 && i<POT1+NUM_XPOTS && !IS_POT_SLIDER_AVAILABLE(i)) {
            if (attr && menuHorizontalPosition == i) REPEAT_LAST_CURSOR_MOVE();
            continue;
          }
          lcdDrawTextAtIndex(x, y, STR_RETA123, i, ((menuHorizontalPosition==i) && attr) ? BLINK|INVERS : (((g_model.beepANACenter & ((BeepANACenter)1<<i)) || (attr && CURSOR_ON_LINE())) ? INVERS : 0 ) );
          x += FW;
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
      }

      case ITEM_MODEL_USE_GLOBAL_FUNCTIONS:
        lcdDrawTextAlignedLeft(y, STR_USE_GLOBAL_FUNCS);
        drawCheckBox(MODEL_SETUP_2ND_COLUMN, y, !g_model.noGlobalFunctions, attr);
        if (attr) g_model.noGlobalFunctions = !checkIncDecModel(event, !g_model.noGlobalFunctions, 0, 1);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_LABEL:
        lcdDrawTextAlignedLeft(y, TR_INTERNALRF);
        break;

#if defined(TARANIS_INTERNAL_PPM)
      case ITEM_MODEL_INTERNAL_MODULE_MODE:
        lcdDrawTextAlignedLeft(y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_TARANIS_PROTOCOLS, g_model.moduleData[INTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (IS_MODULE_XJT(INTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[INTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        if (attr && s_editMode>0) {
          switch (menuHorizontalPosition) {
            case 0:
              g_model.moduleData[INTERNAL_MODULE].type = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].type, MODULE_TYPE_NONE, MODULE_TYPE_XJT, EE_MODEL, isModuleAvailable);
              if (checkIncDec_Ret) {
                 g_model.moduleData[INTERNAL_MODULE].rfProtocol = 0;
                 g_model.moduleData[INTERNAL_MODULE].channelsStart = 0;
                 g_model.moduleData[INTERNAL_MODULE].channelsCount = DEFAULT_CHANNELS(INTERNAL_MODULE);;
              }
              break;
            case 1:
              g_model.moduleData[INTERNAL_MODULE].rfProtocol = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].rfProtocol, RF_PROTO_X16, RF_PROTO_LAST, EE_MODEL, isRfProtocolAvailable);
              if (checkIncDec_Ret) {
                g_model.moduleData[INTERNAL_MODULE].channelsStart = 0;
                g_model.moduleData[INTERNAL_MODULE].channelsCount = DEFAULT_CHANNELS(INTERNAL_MODULE);
              }
            }
          }
        break;
#else
      case ITEM_MODEL_INTERNAL_MODULE_MODE:
        lcdDrawTextAlignedLeft(y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[0].rfProtocol, attr);
        if (attr) {
          g_model.moduleData[INTERNAL_MODULE].rfProtocol = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].rfProtocol, -1, RF_PROTO_LAST, EE_MODEL, isRfProtocolAvailable);

          if (checkIncDec_Ret) {
            g_model.moduleData[INTERNAL_MODULE].type = MODULE_TYPE_XJT;
            g_model.moduleData[INTERNAL_MODULE].channelsStart = 0;
            g_model.moduleData[INTERNAL_MODULE].channelsCount = DEFAULT_CHANNELS(INTERNAL_MODULE);
            if (g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF)
              g_model.moduleData[INTERNAL_MODULE].type = MODULE_TYPE_NONE;
          }
        }
        break;
#endif
      case ITEM_MODEL_TRAINER_MODE:
        lcdDrawTextAlignedLeft(y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VTRAINERMODES, g_model.trainerMode, attr);
        if (attr) {
          g_model.trainerMode = checkIncDec(event, g_model.trainerMode, 0, TRAINER_MODE_MAX(), EE_MODEL, isTrainerModeAvailable);
        }
#if defined(BLUETOOTH) && defined(USEHORUSBT)
        if (attr && checkIncDec_Ret) {
          bluetoothState = BLUETOOTH_STATE_OFF;
          bluetoothDistantAddr[0] = 0;
        }
#endif
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_LABEL:
        lcdDrawTextAlignedLeft(y, TR_EXTERNALRF);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_MODE:
        lcdDrawTextAlignedLeft(y, STR_MODE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_TARANIS_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (IS_MODULE_XJT(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[EXTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        else if (IS_MODULE_DSM2(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_DSM_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        else if (IS_MODULE_R9M(EXTERNAL_MODULE))
          lcdDrawTextAtIndex(MODEL_SETUP_3RD_COLUMN, y, STR_R9M_REGION, g_model.moduleData[EXTERNAL_MODULE].subType, (menuHorizontalPosition==1 ? attr : 0));
#if defined(MULTIMODULE)
        else if (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) {
          uint8_t multi_rfProto = g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false);

          // Do not use MODEL_SETUP_3RD_COLUMN here since some the protocol string are so long that we cannot afford the 2 spaces (+6) here
          if (g_model.moduleData[EXTERNAL_MODULE].multi.customProto) {
            lcdDrawText(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, STR_MULTI_CUSTOM, menuHorizontalPosition == 1 ? attr : 0);
            lcdDrawNumber(MODEL_SETUP_2ND_COLUMN + 14 * FW, y, g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false), menuHorizontalPosition == 2 ? attr : 0, 2);
            lcdDrawNumber(MODEL_SETUP_2ND_COLUMN + 16 * FW, y, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition == 3 ? attr : 0, 2);
          }
          else {
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_MULTI_PROTOCOLS, multi_rfProto, menuHorizontalPosition==1 ? attr : 0);

            const mm_protocol_definition *pdef = getMultiProtocolDefinition(multi_rfProto);
            if (pdef->subTypeString != nullptr)
              lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+11*FW, y, pdef->subTypeString, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
          }
        }
#endif
        if (attr && s_editMode>0) {
          switch (menuHorizontalPosition) {
            case 0:
              g_model.moduleData[EXTERNAL_MODULE].type = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].type, MODULE_TYPE_NONE, IS_TRAINER_EXTERNAL_MODULE() ? MODULE_TYPE_NONE : MODULE_TYPE_COUNT-1, EE_MODEL, isModuleAvailable);
              if (checkIncDec_Ret) {
                g_model.moduleData[EXTERNAL_MODULE].rfProtocol = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsCount = DEFAULT_CHANNELS(EXTERNAL_MODULE);
                if (IS_MODULE_SBUS(EXTERNAL_MODULE))
                  g_model.moduleData[EXTERNAL_MODULE].sbus.refreshRate = -31;
                if(IS_MODULE_PPM(EXTERNAL_MODULE))
                  SET_DEFAULT_PPM_FRAME_LENGTH(EXTERNAL_MODULE);
              }
              break;
            case 1:
              if (IS_MODULE_DSM2(EXTERNAL_MODULE))
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, DSM2_PROTO_LP45, DSM2_PROTO_DSMX);
              else if (IS_MODULE_R9M(EXTERNAL_MODULE))
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, MODULE_SUBTYPE_R9M_FCC, MODULE_SUBTYPE_R9M_LBT);
#if defined(MULTIMODULE)
              else if (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) {
                int multiRfProto = g_model.moduleData[EXTERNAL_MODULE].multi.customProto == 1 ? MM_RF_PROTO_CUSTOM : g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false);
                CHECK_INCDEC_MODELVAR(event, multiRfProto, MM_RF_PROTO_FIRST, MM_RF_PROTO_LAST);
                if (checkIncDec_Ret) {
                  g_model.moduleData[EXTERNAL_MODULE].multi.customProto = (multiRfProto == MM_RF_PROTO_CUSTOM);
                  if (!g_model.moduleData[EXTERNAL_MODULE].multi.customProto)
                    g_model.moduleData[EXTERNAL_MODULE].setMultiProtocol(multiRfProto);
                  g_model.moduleData[EXTERNAL_MODULE].subType = 0;
                  // Sensible default for DSM2 (same as for ppm): 7ch@22ms + Autodetect settings enabled
                  if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) == MM_RF_PROTO_DSM2) {
                    g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = 1;
                  }
                  else {
                    g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = 0;
                  }
                  g_model.moduleData[EXTERNAL_MODULE].multi.optionValue = 0;
                }
              }
#endif
              else {
                g_model.moduleData[EXTERNAL_MODULE].rfProtocol = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, RF_PROTO_X16, RF_PROTO_LAST, EE_MODEL, isRfProtocolAvailable);
              }
              if (checkIncDec_Ret) {
                g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsCount = DEFAULT_CHANNELS(EXTERNAL_MODULE);
              }
              break;
#if defined(MULTIMODULE)
            case 2:
              if (g_model.moduleData[EXTERNAL_MODULE].multi.customProto) {
                g_model.moduleData[EXTERNAL_MODULE].setMultiProtocol(checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false), 0, 63, EE_MODEL));
                break;
              }
              else {
                const mm_protocol_definition * pdef = getMultiProtocolDefinition(g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false));
                if (pdef->maxSubtype > 0)
                  CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].subType, 0, pdef->maxSubtype);
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
      lcdDrawTextAlignedLeft(y, STR_TRAINER);
      break;

#if defined(BLUETOOTH) && defined(USEHORUSBT)
    case ITEM_MODEL_TRAINER_LINE1:
      if (g_model.trainerMode == TRAINER_MODE_MASTER_BLUETOOTH) {
        if (attr) {
          s_editMode = 0;
        }
        if (bluetoothDistantAddr[0]) {
          lcdDrawText(INDENT_WIDTH, y+1, bluetoothDistantAddr, TINSIZE);
          if (bluetoothState != BLUETOOTH_STATE_CONNECTED) {
            lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, BUTTON("Bind"), menuHorizontalPosition == 0 ? attr : 0);
            lcdDrawText(MODEL_SETUP_2ND_COLUMN+5*FW, y, BUTTON("Clear"), menuHorizontalPosition == 1 ? attr : 0);
          }
          else {
            lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, BUTTON("Clear"), attr);
          }
          if (attr && event == EVT_KEY_FIRST(KEY_ENTER)) {
            if (bluetoothState == BLUETOOTH_STATE_CONNECTED || menuHorizontalPosition == 1) {
              bluetoothState = BLUETOOTH_STATE_OFF;
              bluetoothDistantAddr[0] = 0;
            }
            else {
              bluetoothState = BLUETOOTH_STATE_BIND_REQUESTED;
            }
          }
        }
        else {
          lcdDrawText(INDENT_WIDTH, y, "---");
          if (bluetoothState < BLUETOOTH_STATE_IDLE)
            lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, BUTTON("Init"), attr);
          else
            lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, BUTTON("Discover"), attr);
          if (attr && event == EVT_KEY_FIRST(KEY_ENTER)) {
            if (bluetoothState < BLUETOOTH_STATE_IDLE)
              bluetoothState = BLUETOOTH_STATE_OFF;
            else
              bluetoothState = BLUETOOTH_STATE_DISCOVER_REQUESTED;
          }
        }
        break;
      }
      // no break
#else
      case ITEM_MODEL_TRAINER_LINE1:
#endif
      case ITEM_MODEL_INTERNAL_MODULE_CHANNELS:
      case ITEM_MODEL_EXTERNAL_MODULE_CHANNELS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcdDrawTextAlignedLeft(y, STR_CHANNELRANGE);
        if ((int8_t)PORT_CHANNELS_ROWS(moduleIdx) >= 0) {
          lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_CH, menuHorizontalPosition==0 ? attr : 0);
          lcdDrawNumber(lcdLastRightPos, y, moduleData.channelsStart+1, LEFT | (menuHorizontalPosition==0 ? attr : 0));
          lcdDrawChar(lcdLastRightPos, y, '-');
          lcdDrawNumber(lcdLastRightPos + FW+1, y, moduleData.channelsStart+NUM_CHANNELS(moduleIdx), LEFT | (menuHorizontalPosition==1 ? attr : 0));
          if (IS_R9M_OR_XJTD16(moduleIdx)) {
            if (NUM_CHANNELS(moduleIdx) > 8)
              lcdDrawText(lcdLastRightPos+5, y, "(18ms)");
            else
              lcdDrawText(lcdLastRightPos+5, y, "(9ms)");
          }
          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.channelsStart, 32-8-moduleData.channelsCount);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, moduleData.channelsCount, -4, min<int8_t>(MAX_CHANNELS(moduleIdx), 32-8-moduleData.channelsStart));
#if defined(TARANIS_INTERNAL_PPM)
                if ((k == ITEM_MODEL_EXTERNAL_MODULE_CHANNELS && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM) || (k == ITEM_MODEL_INTERNAL_MODULE_CHANNELS && g_model.moduleData[INTERNAL_MODULE].type == MODULE_TYPE_PPM) || (k == ITEM_MODEL_TRAINER_LINE1)) {
                  SET_DEFAULT_PPM_FRAME_LENGTH(moduleIdx);
                }
#else
                if ((k == ITEM_MODEL_EXTERNAL_MODULE_CHANNELS && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM) || (k == ITEM_MODEL_TRAINER_LINE1)) {
                  SET_DEFAULT_PPM_FRAME_LENGTH(moduleIdx);
                }
#endif
                break;
            }
          }
        }
        break;
      }

      case ITEM_MODEL_INTERNAL_MODULE_BIND:
      case ITEM_MODEL_EXTERNAL_MODULE_BIND:
      case ITEM_MODEL_TRAINER_LINE2:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        if (IS_MODULE_PPM(moduleIdx)) {
          lcdDrawTextAlignedLeft(y, STR_PPMFRAME);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppm.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
          lcdDrawText(lcdLastRightPos, y, STR_MS);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN+6*FW, y, (moduleData.ppm.delay*50)+300, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
          lcdDrawChar(lcdLastRightPos, y, 'u');
          lcdDrawChar(MODEL_SETUP_2ND_COLUMN+12*FW, y, moduleData.ppm.pulsePol ? '+' : '-', (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);

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
        else if (IS_MODULE_SBUS(moduleIdx)) {
          lcdDrawTextAlignedLeft(y, STR_REFRESHRATE);
          lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppm.frameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
          lcdDrawText(lcdLastRightPos, y, STR_MS);
          lcdDrawText(MODEL_SETUP_3RD_COLUMN, y, moduleData.sbus.noninverted ? "not inverted" : "normal", (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);

          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR(event, moduleData.ppm.frameLength, -33, 35);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.sbus.noninverted, 1);
                break;
            }
          }
        }
        else {
          horzpos_t l_posHorz = menuHorizontalPosition;
          coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
          if (IS_MODULE_XJT(moduleIdx) && g_model.moduleData[moduleIdx].rfProtocol == RF_PROTO_D8) {
            xOffsetBind = 0;
            lcdDrawTextAlignedLeft(y, STR_RECEIVER);
            if (attr) l_posHorz += 1;
          }
          else {
            lcdDrawTextAlignedLeft(y, STR_RECEIVER_NUM);
          }
          if (IS_MODULE_PXX(moduleIdx) || IS_MODULE_DSM2(moduleIdx) || IS_MODULE_MULTIMODULE(moduleIdx)) {
            if (xOffsetBind)
              lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[moduleIdx], (l_posHorz==0 ? attr : 0) | LEADING0|LEFT, 2);
            if (attr && l_posHorz==0) {
              if (s_editMode>0) {
                CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[moduleIdx], MAX_RX_NUM(moduleIdx));
                if (checkIncDec_Ret) {
                  modelHeaders[g_eeGeneral.currModel].modelId[moduleIdx] = g_model.header.modelId[moduleIdx];
                }
                else if (event == EVT_KEY_LONG(KEY_ENTER)) {
                  killEvents(event);
                  uint8_t newVal = findNextUnusedModelId(g_eeGeneral.currModel, moduleIdx);
                  if (newVal != g_model.header.modelId[moduleIdx]) {
                    modelHeaders[g_eeGeneral.currModel].modelId[moduleIdx] = g_model.header.modelId[moduleIdx] = newVal;
                    storageDirty(EE_MODEL);
                  }
                }
              }
            }
            lcdDrawText(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
            lcdDrawText(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);
            uint8_t newFlag = 0;
#if defined(MULTIMODULE)
            if (multiBindStatus == MULTI_BIND_FINISHED) {
              multiBindStatus = MULTI_NORMAL_OPERATION;
              s_editMode = 0;
            }
#endif
            if (attr && l_posHorz>0) {
              if (s_editMode>0) {
                if (l_posHorz == 1) {
                  if (IS_MODULE_R9M(moduleIdx) || (IS_MODULE_XJT(moduleIdx) && g_model.moduleData[moduleIdx].rfProtocol== RF_PROTO_X16)) {
                    if (event == EVT_KEY_BREAK(KEY_ENTER)) {
                      uint8_t default_selection = 0; // R9M_LBT should default to 0 as available options are variables
                      if (IS_MODULE_R9M_LBT(moduleIdx)) {
                        if (BIND_TELEM_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_ON);
                        POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_OFF);
                        if (BIND_TELEM_ALLOWED(moduleIdx) && BIND_CH9TO16_ALLOWED(moduleIdx))
                         POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_ON);
                        if (BIND_CH9TO16_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_OFF);
                      }
                      else {
                        if (BIND_TELEM_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_ON);
                        POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_OFF);
                        if (BIND_TELEM_ALLOWED(moduleIdx))
                          POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_ON);
                        POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_OFF);
                        default_selection = g_model.moduleData[moduleIdx].pxx.receiver_telem_off + (g_model.moduleData[moduleIdx].pxx.receiver_channel_9_16 << 1);
                      }
                      POPUP_MENU_SELECT_ITEM(default_selection);
                      POPUP_MENU_START(onBindMenu);
                      continue;
                    }
                    if (moduleFlag[moduleIdx] == MODULE_BIND) {
                      newFlag = MODULE_BIND;
                    }
                    else {
                      if (!popupMenuNoItems) {
                        s_editMode = 0;  // this is when popup is exited before a choice is made
                      }
                    }
                  }
                  else {
                    newFlag = MODULE_BIND;
                  }
                }
                else if (l_posHorz == 2) {
                  newFlag = MODULE_RANGECHECK;
                }
              }
            }
            moduleFlag[moduleIdx] = newFlag;
#if defined(MULTIMODULE)
            if (newFlag == MODULE_BIND)
              multiBindStatus = MULTI_BIND_INITIATED;
#endif
          }
        }
        break;
      }

      case ITEM_MODEL_INTERNAL_MODULE_FAILSAFE:
      case ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcdDrawTextAlignedLeft(y, STR_FAILSAFE);
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_VFAILSAFE, moduleData.failsafeMode, menuHorizontalPosition==0 ? attr : 0);
        if (moduleData.failsafeMode == FAILSAFE_CUSTOM) lcdDrawText(MODEL_SETUP_2ND_COLUMN + MODEL_SETUP_SET_FAILSAFE_OFS, y, STR_SET, menuHorizontalPosition==1 ? attr : 0);
        if (attr) {
          if (moduleData.failsafeMode != FAILSAFE_CUSTOM) {
            menuHorizontalPosition = 0;
          }
          if (menuHorizontalPosition == 0) {
            if (s_editMode > 0) {
              CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.failsafeMode, FAILSAFE_LAST);
              if (checkIncDec_Ret) SEND_FAILSAFE_NOW(moduleIdx);
            }
          }
          else if (menuHorizontalPosition == 1) {
            s_editMode = 0;
            if (moduleData.failsafeMode == FAILSAFE_CUSTOM) {
              if (event == EVT_KEY_LONG(KEY_ENTER)) {
                killEvents(event);
                setCustomFailsafe(moduleIdx);
                storageDirty(EE_MODEL);
                AUDIO_WARNING1();
                SEND_FAILSAFE_NOW(moduleIdx);
              }
              else if (event == EVT_KEY_BREAK(KEY_ENTER)) {
                g_moduleIdx = moduleIdx;
                pushMenu(menuModelFailsafe);
              }
            }
          }
          else {
            lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN, y, LCD_W - MODEL_SETUP_2ND_COLUMN - MENUS_SCROLLBAR_WIDTH, 8);
          }
        }
        break;
      }
      break;

     case ITEM_MODEL_EXTERNAL_MODULE_OPTIONS:
     {
       uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
#if defined(MULTIMODULE)

       if (IS_MODULE_MULTIMODULE(moduleIdx)) {
         int optionValue = g_model.moduleData[moduleIdx].multi.optionValue;

         const uint8_t multi_proto = g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true);
         const mm_protocol_definition *pdef = getMultiProtocolDefinition(multi_proto);
         if (pdef->optionsstr)
           lcdDrawText(INDENT_WIDTH, y, pdef->optionsstr);

         if (multi_proto == MM_RF_PROTO_FS_AFHDS2A)
           optionValue = 50 + 5 * optionValue;

         lcdDrawNumber(MODEL_SETUP_2ND_COLUMN, y, optionValue, LEFT | attr);
         if (attr) {
           if (multi_proto == MM_RF_PROTO_FS_AFHDS2A) {
             CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].multi.optionValue, 0, 70);
           }
           else if (multi_proto == MM_RF_PROTO_OLRS) {
             CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].multi.optionValue, -1, 7);
           }
           else {
             CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].multi.optionValue, -128, 127);
           }
         }
       }
#endif
       if (IS_MODULE_R9M(moduleIdx)) {
         lcdDrawTextAlignedLeft(y, STR_MODULE_TELEMETRY);
         if (IS_TELEMETRY_INTERNAL_MODULE()) {
           lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_DISABLE_INTERNAL);
         }
         else {
           lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, STR_MODULE_TELEM_ON);
         }
       }
       else if (IS_MODULE_SBUS(moduleIdx)) {
         lcdDrawTextAlignedLeft(y, STR_WARN_BATTVOLTAGE);
         putsVolts(lcdLastRightPos, y, getBatteryVoltage(), attr | PREC2 | LEFT);
       }
     }
     break;

    case  ITEM_MODEL_EXTERNAL_MODULE_POWER:
    {
      uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
      if (IS_MODULE_R9M(moduleIdx)) {
        lcdDrawTextAlignedLeft(y, TR_MULTI_RFPOWER);
        if(IS_MODULE_R9M_FCC(moduleIdx)) {
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_FCC_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
          if (attr)
            CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].pxx.power, 0, R9M_FCC_POWER_MAX);
        }
        else {
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN, y, STR_R9M_LBT_POWER_VALUES, g_model.moduleData[moduleIdx].pxx.power, LEFT | attr);
          if (attr)
            CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].pxx.power, 0, R9M_LBT_POWER_MAX);
          if (attr && s_editMode == 0 && reusableBuffer.modelsetup.r9mPower != g_model.moduleData[moduleIdx].pxx.power) {
            if((reusableBuffer.modelsetup.r9mPower + g_model.moduleData[moduleIdx].pxx.power) < 5)  //switching between mode 2 and 3 does not require rebind
              POPUP_WARNING(STR_REBIND);
            reusableBuffer.modelsetup.r9mPower = g_model.moduleData[moduleIdx].pxx.power;
          }
        }
      }
#if defined (MULTIMODULE)
      else if (IS_MODULE_MULTIMODULE(moduleIdx)) {
        g_model.moduleData[EXTERNAL_MODULE].multi.lowPowerMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.lowPowerMode, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_LOWPOWER, attr, event);
      }
#endif
      break;
    }

#if defined (MULTIMODULE)
    case ITEM_MODEL_EXTERNAL_MODULE_AUTOBIND:
      if (g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) == MM_RF_PROTO_DSM2)
        g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_DSM_AUTODTECT, attr, event);
      else
        g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode = editCheckBox(g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_AUTOBIND, attr, event);
      break;
    case ITEM_MODEL_EXTERNAL_MODULE_STATUS:
    {
      lcdDrawTextAlignedLeft(y, STR_MODULE_STATUS);

      char statusText[64];
      multiModuleStatus.getStatusString(statusText);
      lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, statusText);
      break;
    }
    case ITEM_MODEL_EXTERNAL_MODULE_SYNCSTATUS: {
      lcdDrawTextAlignedLeft(y, STR_MODULE_SYNC);

      char statusText[64];
      multiSyncStatus.getRefreshString(statusText);
      lcdDrawText(MODEL_SETUP_2ND_COLUMN, y, statusText);
      break;
    }
#endif
    }
  }

#if defined(PXX)
  if (IS_RANGECHECK_ENABLE()) {
    showMessageBox("RSSI: ");
    lcdDrawNumber(16+4*FW, 5*FH, TELEMETRY_RSSI(), BOLD);
  }
#endif

  if (old_editMode > 0 && s_editMode == 0) {
    switch(menuVerticalPosition) {
    case ITEM_MODEL_INTERNAL_MODULE_BIND:
      if (menuHorizontalPosition == 0)
        checkModelIdUnique(g_eeGeneral.currModel, INTERNAL_MODULE);
      break;

    case ITEM_MODEL_EXTERNAL_MODULE_BIND:
      if (menuHorizontalPosition == 0)
        checkModelIdUnique(g_eeGeneral.currModel, EXTERNAL_MODULE);
      break;
    }
  }
}

void menuModelFailsafe(event_t event)
{
  static uint8_t maxNameLen = 4;
  static int8_t lastModel = g_eeGeneral.currModel;
  const coord_t barH = (LCD_H - FH) / 8 - 1;
  const int lim = (g_model.extendedLimits ? (512 * LIMIT_EXT_PERCENT / 100) : 512) * 2;
  const uint8_t channelStart = g_model.moduleData[g_moduleIdx].channelsStart;
  uint8_t cols = 1;
  uint8_t colW = LCD_W;

  if (lastModel != g_eeGeneral.currModel) {
    lastModel = g_eeGeneral.currModel;
    maxNameLen = 4;
  }

  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    event = 0;

    if (menuVerticalPosition < NUM_CHANNELS(g_moduleIdx)) {
      if (s_editMode) {
        g_model.moduleData[g_moduleIdx].failsafeChannels[menuVerticalPosition] = channelOutputs[menuVerticalPosition+channelStart];
        s_editMode = 0;
      }
      else {
        int16_t & failsafe = g_model.moduleData[g_moduleIdx].failsafeChannels[menuVerticalPosition];
        if (failsafe < FAILSAFE_CHANNEL_HOLD)
          failsafe = FAILSAFE_CHANNEL_HOLD;
        else if (failsafe == FAILSAFE_CHANNEL_HOLD)
          failsafe = FAILSAFE_CHANNEL_NOPULSE;
        else
          failsafe = 0;
      }
    }
    else {
      // "Outputs => Failsafe" menu item
      setCustomFailsafe(g_moduleIdx);
    }

    storageDirty(EE_MODEL);
    AUDIO_WARNING1();
    SEND_FAILSAFE_NOW(g_moduleIdx);
  }

  SIMPLE_SUBMENU_NOTITLE(NUM_CHANNELS(g_moduleIdx)+1);
  SET_SCROLLBAR_X(0);

  if (NUM_CHANNELS(g_moduleIdx) > 8) {
    cols = 2;
    colW = LCD_W / cols - 1;
    // Column separator
    if (menuVerticalPosition >= NUM_CHANNELS(g_moduleIdx)) {
      lcdDrawSolidVerticalLine(colW, FH, LCD_H - (FH*2 + FH/2 + 2));
    }
    else {
      lcdDrawSolidVerticalLine(colW, FH, LCD_H - FH);
    }
  }

  lcdDrawTextAlignedCenter(0, FAILSAFESET);
  lcdInvertLine(0);

  coord_t x = colW;
  for (uint8_t col = 0; col < cols; col++) {

    coord_t y = FH + 1;
    uint8_t line = (menuVerticalPosition >= NUM_CHANNELS(g_moduleIdx) ? 2 : 0);
    uint8_t ch = line + col*8;

    for (; line < 8; line++) {
      const int32_t channelValue = channelOutputs[ch+channelStart];
      int32_t failsafeValue = g_model.moduleData[g_moduleIdx].failsafeChannels[8*col+line];
      uint8_t lenLabel = ZLEN(g_model.limitData[ch+channelStart].name);
      uint8_t barW = colW - FW * maxNameLen - FWNUM * 3;  // default bar width

#if defined(PPM_UNIT_PERCENT_PREC1)
      barW -= FWNUM + 1;
#endif
      barW += (barW % 2);

      // Channel name if present, number if not
      if (lenLabel > 0) {
        if (lenLabel > maxNameLen)
          maxNameLen = lenLabel;
        lcdDrawSizedText(x - colW, y, g_model.limitData[ch+channelStart].name, sizeof(g_model.limitData[ch+channelStart].name), ZCHAR | SMLSIZE);
      }
      else {
        putsChn(x - colW, y, ch+1, SMLSIZE);
      }

      // Value
      LcdFlags flags = TINSIZE;
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

      const coord_t xValue = x - barW;
      if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
        lcdDrawText(xValue, y, STR_HOLD, RIGHT|flags);
        failsafeValue = 0;
      }
      else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
        lcdDrawText(xValue, y, STR_NONE, RIGHT|flags);
        failsafeValue = 0;
      }
      else {
#if defined(PPM_UNIT_US)
        lcdDrawNumber(xValue, y, PPM_CH_CENTER(ch)+failsafeValue/2, RIGHT|flags);
#elif defined(PPM_UNIT_PERCENT_PREC1)
        lcdDrawNumber(xValue, y, calcRESXto1000(failsafeValue), RIGHT|PREC1|flags);
#else
        lcdDrawNumber(xValue, y, calcRESXto1000(failsafeValue)/10, RIGHT|flags);
#endif
      }

      // Gauge
      lcdDrawRect(x - barW, y, barW - 1, barH);
      barW = barW / 2 - 1;
      const coord_t lenChannel  = limit<uint8_t>(1, (abs(channelValue)  * barW + lim / 2) / lim, barW);
      const coord_t lenFailsafe = limit<uint8_t>(1, (abs(failsafeValue) * barW + lim / 2) / lim, barW);
      const coord_t barX = x - barW - 2;
      const coord_t xChannel  = (channelValue >= 0) ? barX : barX - lenChannel + 1;
      const coord_t xFailsafe = (failsafeValue > 0) ? barX : barX - lenFailsafe + 1;
      lcdDrawHorizontalLine(xChannel, y+1, lenChannel, DOTTED, 0);
      lcdDrawHorizontalLine(xChannel, y+2, lenChannel, DOTTED, 0);
      lcdDrawSolidHorizontalLine(xFailsafe, y+3, lenFailsafe);
      lcdDrawSolidHorizontalLine(xFailsafe, y+4, lenFailsafe);

      if (++ch >= NUM_CHANNELS(g_moduleIdx))
        break;

      y += barH + 1;

    }  // channels

    x += colW + 2;

  }  // columns

  if (menuVerticalPosition >= NUM_CHANNELS(g_moduleIdx)) {
    // Outputs => Failsafe
    lcdDrawText(CENTER_OFS, LCD_H - (FH + 1), STR_OUTPUTS2FAILSAFE, INVERS);
  }
}
