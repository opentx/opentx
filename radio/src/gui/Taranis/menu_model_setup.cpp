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

uint8_t g_moduleIdx;
void menuModelFailsafe(uint8_t event);

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
#if defined(REV9E)
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
#if defined(REV9E)
  ITEM_MODEL_SWITCHES_WARNING2,
  ITEM_MODEL_SWITCHES_WARNING3,
#endif
  ITEM_MODEL_POTS_WARNING,
#if defined(REV9E)
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
  ITEM_MODEL_EXTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_EXTERNAL_MODULE_BIND,
  ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE,
  ITEM_MODEL_TRAINER_LABEL,
  ITEM_MODEL_TRAINER_MODE,
  ITEM_MODEL_TRAINER_CHANNELS,
  ITEM_MODEL_TRAINER_SETTINGS,
  ITEM_MODEL_SETUP_MAX
};

#define FIELD_PROTOCOL_MAX 1

#define MODEL_SETUP_2ND_COLUMN        (LCD_W-17*FW-MENUS_SCROLLBAR_WIDTH-1)
#define MODEL_SETUP_BIND_OFS          3*FW-2
#define MODEL_SETUP_RANGE_OFS         7*FW
#define MODEL_SETUP_SET_FAILSAFE_OFS  10*FW-2

void copySelection(char *dst, const char *src, uint8_t size)
{
  if (memcmp(src, "---", 3) == 0)
    memset(dst, 0, size);
  else
    memcpy(dst, src, size);
}

void onModelSetupBitmapMenu(const char *result)
{
  if (result == STR_UPDATE_LIST) {
    if (!listSdFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap), NULL)) {
      POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
    }
  }
  else {
    // The user choosed a bmp file in the list
    copySelection(g_model.header.bitmap, result, sizeof(g_model.header.bitmap));
    memcpy(modelHeaders[g_eeGeneral.currModel].bitmap, g_model.header.bitmap, sizeof(g_model.header.bitmap));
    eeDirty(EE_MODEL);
  }
}

void editTimerMode(int timerIdx, coord_t y, LcdFlags attr, uint8_t event)
{
  TimerData * timer = &g_model.timers[timerIdx];
  putsStrIdx(0*FW, y, STR_TIMER, timerIdx+1);
  putsTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer->mode, menuHorizontalPosition==0 ? attr : 0);
  putsTimer(MODEL_SETUP_2ND_COLUMN+5*FW-2+5*FWNUM+1, y, timer->start, menuHorizontalPosition==1 ? attr|TIMEHOUR : TIMEHOUR, menuHorizontalPosition==2 ? attr|TIMEHOUR : TIMEHOUR);
  if (attr && menuHorizontalPosition < 0) drawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, 11*FW, FH+1);
  if (attr && s_editMode>0) {
    div_t qr = div(timer->start, 60);
    switch (menuHorizontalPosition) {
      case 0:
      {
        swsrc_t timerMode = timer->mode;
        if (timerMode < 0) timerMode -= TMRMODE_COUNT-1;
        CHECK_INCDEC_MODELVAR_CHECK(event, timerMode, -TMRMODE_COUNT-SWSRC_LAST+1, TMRMODE_COUNT+SWSRC_LAST-1, isSwitchAvailableInTimers);
        if (timerMode < 0) timerMode += TMRMODE_COUNT-1;
        timer->mode = timerMode;
#if defined(AUTOSWITCH)
        if (s_editMode>0) {
          swsrc_t val = timer->mode - (TMRMODE_COUNT-1);
          swsrc_t switchVal = checkIncDecMovedSwitch(val);
          if (val != switchVal) {
            timer->mode = switchVal + (TMRMODE_COUNT-1);
            eeDirty(EE_MODEL);
          }
        }
#endif
        break;
      }
      case 1:
        qr.quot = checkIncDec(event, qr.quot, 0, 1439, EE_MODEL | NO_INCDEC_MARKS); // 23h59
        timer->start = qr.rem + qr.quot*60;
        break;
      case 2:
        qr.rem -= checkIncDecModel(event, qr.rem+2, 1, 62)-2;
        timer->start -= qr.rem ;
        if ((int16_t)timer->start < 0) timer->start=0;
        if ((int32_t)timer->start > 86399) timer->start=86399; // 23h59:59
        break;
    }
  }
}

#define CURRENT_MODULE_EDITED(k)         (k>=ITEM_MODEL_TRAINER_LABEL ? TRAINER_MODULE : (k>=ITEM_MODEL_EXTERNAL_MODULE_LABEL ? EXTERNAL_MODULE : INTERNAL_MODULE))

#if defined(PCBTARANIS)
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
#endif

#if !defined(TARANIS_INTERNAL_PPM)
  #define INTERNAL_MODULE_MODE_ROWS       0 // (OFF / RF protocols)
#else
  #define INTERNAL_MODULE_MODE_ROWS       (IS_MODULE_XJT(INTERNAL_MODULE) ? (uint8_t)1 : (uint8_t)0) // Module type + RF protocols
#endif
#if defined(TARANIS_INTERNAL_PPM)
  #define IF_INTERNAL_MODULE_ON(x)        (g_model.moduleData[INTERNAL_MODULE].type == MODULE_TYPE_NONE ? HIDDEN_ROW : (uint8_t)(x))
#else
  #define IF_INTERNAL_MODULE_ON(x)        (g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF ? HIDDEN_ROW : (uint8_t)(x))
#endif
#define IF_EXTERNAL_MODULE_ON(x)          (g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_NONE ? HIDDEN_ROW : (uint8_t)(x))
#define IF_TRAINER_ON(x)                  (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)(x) : HIDDEN_ROW)
#define IS_D8_RX(x)                       (g_model.moduleData[x].rfProtocol == RF_PROTO_D8)
#define IF_EXTERNAL_MODULE_XJT(x)         (IS_MODULE_XJT(EXTERNAL_MODULE) ? (uint8_t)x : HIDDEN_ROW)
#define INTERNAL_MODULE_CHANNELS_ROWS     IF_INTERNAL_MODULE_ON(1)
#define EXTERNAL_MODULE_CHANNELS_ROWS     IF_EXTERNAL_MODULE_ON((IS_MODULE_DSM2(EXTERNAL_MODULE) || IS_MODULE_CROSSFIRE(EXTERNAL_MODULE) || IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) ? (uint8_t)0 : (uint8_t)1)
#define TRAINER_CHANNELS_ROWS()           IF_TRAINER_ON(1)
#define PORT_CHANNELS_ROWS(x)             (x==INTERNAL_MODULE ? INTERNAL_MODULE_CHANNELS_ROWS : (x==EXTERNAL_MODULE ? EXTERNAL_MODULE_CHANNELS_ROWS : TRAINER_CHANNELS_ROWS()))
#if defined MULTIMODULE
#define MULTIMODULE_HASOPTIONS(x)         (x == MM_RF_PROTO_HUBSAN || x == MM_RF_PROTO_FRSKY || x == MM_RF_PROTO_DSM2)
#define MULTIMODULE_FAILSAFEROWS(x)       (IS_MODULE_MULTIMODULE(x) && (MULTIMODULE_HASOPTIONS(g_model.moduleData[x].multi.rfProtocol))) ? (uint8_t) 1: HIDDEN_ROW
#else
#define MULTIMODULE_FAILSAFEROWS(x)       HIDDEN_ROW
#endif
#define FAILSAFE_ROWS(x)                  (IS_MODULE_XJT(x) && HAS_RF_PROTOCOL_FAILSAFE(g_model.moduleData[x].rfProtocol) ? (g_model.moduleData[x].failsafeMode==FAILSAFE_CUSTOM ? (uint8_t)1 : (uint8_t)0) : MULTIMODULE_FAILSAFEROWS(x))
#define TIMER_ROWS                        2|NAVIGATION_LINE_BY_LINE, 0, CASE_PERSISTENT_TIMERS(0) 0, 0
#if defined(MULTIMODULE)
#define MULTIMODULE_HAS_SUBTYPE(x)       (x == MM_RF_PROTO_FLYSKY || x == MM_RF_PROTO_FRSKY || x == MM_RF_PROTO_HISKY || x == MM_RF_PROTO_DSM2 || x == MM_RF_PROTO_YD717 || x == MM_RF_PROTO_KN || x == MM_RF_PROTO_SYMAX || x == MM_RF_PROTO_CX10 || x == MM_RF_PROTO_CG023 || x == MM_RF_PROTO_MT99XX || x == MM_RF_PROTO_MJXQ)
#define MULTIMODULE_MODE_ROWS(x)          MULTIMODULE_HAS_SUBTYPE(g_model.moduleData[x].multi.rfProtocol) ? (uint8_t)2 : (uint8_t)1
#define MULTIMODULE_MODULE_ROWS           IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? (uint8_t) 1 : HIDDEN_ROW, IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? (uint8_t) 1 : HIDDEN_ROW,
#else
#define MULTIMODULE_MODE_ROWS(x) (uint8_t)0
#endif
#define EXTERNAL_MODULE_MODE_ROWS       (IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE)) ? (uint8_t)1 :  IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? MULTIMODULE_MODE_ROWS(EXTERNAL_MODULE) : (uint8_t)0
#if TIMERS == 1
  #define TIMERS_ROWS                     TIMER_ROWS
#elif TIMERS == 2
  #define TIMERS_ROWS                     TIMER_ROWS, TIMER_ROWS
#elif TIMERS == 3
  #define TIMERS_ROWS                     TIMER_ROWS, TIMER_ROWS, TIMER_ROWS
#endif
#if defined(REV9E)
  #define SW_WARN_ITEMS()                 uint8_t(NAVIGATION_LINE_BY_LINE|(getSwitchWarningsCount()-1)), uint8_t(getSwitchWarningsCount() > 8 ? TITLE_ROW : HIDDEN_ROW), uint8_t(getSwitchWarningsCount() > 16 ? TITLE_ROW : HIDDEN_ROW)
  #define POT_WARN_ITEMS()                uint8_t(g_model.potsWarnMode ? NAVIGATION_LINE_BY_LINE|NUM_POTS : 0), uint8_t(g_model.potsWarnMode ? TITLE_ROW : HIDDEN_ROW)
  #define TOPLCD_ROWS                     0,
#else
  #define SW_WARN_ITEMS()                 uint8_t(NAVIGATION_LINE_BY_LINE|getSwitchWarningsCount())
  #define POT_WARN_ITEMS()                uint8_t(g_model.potsWarnMode ? NAVIGATION_LINE_BY_LINE|NUM_POTS : 0)
  #define TOPLCD_ROWS
#endif

void menuModelSetup(uint8_t event)
{
  horzpos_t l_posHorz = menuHorizontalPosition;
  bool CURSOR_ON_CELL = (menuHorizontalPosition >= 0);
#if defined(TARANIS_INTERNAL_PPM)
  MENU_TAB({ 0, 0, TIMERS_ROWS, TOPLCD_ROWS 0, 1, 0, 0, LABEL(Throttle), 0, 0, 0, LABEL(PreflightCheck), 0, 0, SW_WARN_ITEMS(), POT_WARN_ITEMS(),
    NAVIGATION_LINE_BY_LINE|(NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1), 0, 
    LABEL(InternalModule), 
    INTERNAL_MODULE_MODE_ROWS, 
    INTERNAL_MODULE_CHANNELS_ROWS,
    IF_INTERNAL_MODULE_ON(IS_MODULE_XJT(INTERNAL_MODULE) ? (HAS_RF_PROTOCOL_MODELINDEX(g_model.moduleData[INTERNAL_MODULE].rfProtocol) ? (uint8_t)2 : (uint8_t)1) : (IS_MODULE_PPM(INTERNAL_MODULE) ? (uint8_t)1 : HIDDEN_ROW)),
    IF_INTERNAL_MODULE_ON((IS_MODULE_XJT(INTERNAL_MODULE)) ? FAILSAFE_ROWS(INTERNAL_MODULE) : HIDDEN_ROW),
    LABEL(ExternalModule),
    (IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE) || IS_MODULE_MULTIMODULE(EXTERNALE_MODULE)) ? (uint8_t)1 : (uint8_t)0,
    EXTERNAL_MODULE_CHANNELS_ROWS,
    (IS_MODULE_XJT(EXTERNAL_MODULE) && !HAS_RF_PROTOCOL_MODELINDEX(g_model.moduleData[EXTERNAL_MODULE].rfProtocol)) ? (uint8_t)1 : (IS_MODULE_PPM(EXTERNAL_MODULE) || IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE) || IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW,
    IF_EXTERNAL_MODULE_XJT(FAILSAFE_ROWS(EXTERNAL_MODULE)),
    LABEL(Trainer), 0, TRAINER_CHANNELS_ROWS(), IF_TRAINER_ON(2)});
#else
  MENU_TAB({ 0, 0, TIMERS_ROWS, TOPLCD_ROWS 0, 1, 0, 0, LABEL(Throttle), 0, 0, 0, LABEL(PreflightCheck), 0, 0, SW_WARN_ITEMS(), POT_WARN_ITEMS(),
    NAVIGATION_LINE_BY_LINE|(NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1), 0,
    LABEL(InternalModule),
    INTERNAL_MODULE_MODE_ROWS,
    INTERNAL_MODULE_CHANNELS_ROWS,
    IF_INTERNAL_MODULE_ON(HAS_RF_PROTOCOL_MODELINDEX(g_model.moduleData[INTERNAL_MODULE].rfProtocol) ? (uint8_t)2 : (uint8_t)1),
    IF_INTERNAL_MODULE_ON(FAILSAFE_ROWS(INTERNAL_MODULE)),
    LABEL(ExternalModule),
    EXTERNAL_MODULE_MODE_ROWS,
    EXTERNAL_MODULE_CHANNELS_ROWS,
    (IS_MODULE_XJT(EXTERNAL_MODULE) && !HAS_RF_PROTOCOL_FAILSAFE(g_model.moduleData[EXTERNAL_MODULE].rfProtocol)) ? (uint8_t)1 : (IS_MODULE_PPM(EXTERNAL_MODULE) || IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE) || IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW,
    FAILSAFE_ROWS(EXTERNAL_MODULE),
    LABEL(Trainer), 0, TRAINER_CHANNELS_ROWS(), IF_TRAINER_ON(2)});
#endif
  MENU_CHECK(STR_MENUSETUP, menuTabModel, e_ModelSetup, ITEM_MODEL_SETUP_MAX);

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
        lcd_putsLeft(y, STR_BITMAP);
        if (ZEXIST(g_model.header.bitmap))
          lcd_putsnAtt(MODEL_SETUP_2ND_COLUMN, y, g_model.header.bitmap, sizeof(g_model.header.bitmap), attr);
        else
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_VCSWFUNC, 0, attr);
        if (attr && event==EVT_KEY_BREAK(KEY_ENTER) && READ_ONLY_UNLOCKED()) {
          s_editMode = 0;
          if (listSdFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap), g_model.header.bitmap, LIST_NONE_SD_FILE)) {
            popupMenuHandler = onModelSetupBitmapMenu;
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
        g_model.timers[0].minuteBeep = onoffMenuItem(g_model.timers[0].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
        break;

      case ITEM_MODEL_TIMER1_COUNTDOWN_BEEP:
        g_model.timers[0].countdownBeep = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, STR_VBEEPCOUNTDOWN, g_model.timers[0].countdownBeep, COUNTDOWN_SILENT, COUNTDOWN_COUNT-1, attr, event);
        break;

      case ITEM_MODEL_TIMER1_PERSISTENT:
        g_model.timers[0].persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, g_model.timers[0].persistent, 0, 2, attr, event);
        break;

#if TIMERS > 1
      case ITEM_MODEL_TIMER2:
        editTimerMode(1, y, attr, event);
        break;

      case ITEM_MODEL_TIMER2_NAME:
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_TIMER_NAME, g_model.timers[1].name, LEN_TIMER_NAME, event, attr);
        break;

      case ITEM_MODEL_TIMER2_MINUTE_BEEP:
        g_model.timers[1].minuteBeep = onoffMenuItem(g_model.timers[1].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
        break;

      case ITEM_MODEL_TIMER2_COUNTDOWN_BEEP:
        g_model.timers[1].countdownBeep = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, STR_VBEEPCOUNTDOWN, g_model.timers[1].countdownBeep, COUNTDOWN_SILENT, COUNTDOWN_COUNT-1, attr, event);
        break;

      case ITEM_MODEL_TIMER2_PERSISTENT:
        g_model.timers[1].persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, g_model.timers[1].persistent, 0, 2, attr, event);
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
        g_model.timers[2].minuteBeep = onoffMenuItem(g_model.timers[2].minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
        break;

      case ITEM_MODEL_TIMER3_COUNTDOWN_BEEP:
        g_model.timers[2].countdownBeep = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, STR_VBEEPCOUNTDOWN, g_model.timers[2].countdownBeep, COUNTDOWN_SILENT, COUNTDOWN_COUNT-1, attr, event);
        break;

      case ITEM_MODEL_TIMER3_PERSISTENT:
        g_model.timers[2].persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, g_model.timers[2].persistent, 0, 2, attr, event);
        break;
#endif

#if defined(REV9E)
      case ITEM_MODEL_TOP_LCD_TIMER:
        lcd_putsLeft(y, STR_TOPLCDTIMER);
        putsStrIdx(MODEL_SETUP_2ND_COLUMN, y, STR_TIMER, g_model.topLcdTimer+1, attr);
        if (attr) {
          g_model.topLcdTimer = checkIncDec(event, g_model.topLcdTimer, 0, TIMERS-1, EE_MODEL);
        }
        break;
#endif

      case ITEM_MODEL_EXTENDED_LIMITS:
        ON_OFF_MENU_ITEM(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, STR_ELIMITS, attr, event);
        break;

      case ITEM_MODEL_EXTENDED_TRIMS:
        ON_OFF_MENU_ITEM(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, STR_ETRIMS, menuHorizontalPosition<=0 ? attr : 0, event==EVT_KEY_BREAK(KEY_ENTER) ? event : 0);
        lcd_putsAtt(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_RESET_BTN, (menuHorizontalPosition>0  && !NO_HIGHLIGHT()) ? attr : 0);
        if (attr && menuHorizontalPosition>0) {
          s_editMode = 0;
          if (event==EVT_KEY_LONG(KEY_ENTER)) {
            START_NO_HIGHLIGHT();
            for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
              memclear(&g_model.flightModeData[i], TRIMS_ARRAY_SIZE);
            }
            eeDirty(EE_MODEL);
            AUDIO_WARNING1();
          }
        }
        break;

      case ITEM_MODEL_DISPLAY_TRIMS:
        g_model.displayTrims = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_DISPLAY_TRIMS, STR_VDISPLAYTRIMS, g_model.displayTrims, 0, 2, attr, event);
        break;

      case ITEM_MODEL_TRIM_INC:
        g_model.trimInc = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_TRIMINC, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_LABEL:
        lcd_putsLeft(y, STR_THROTTLE_LABEL);
        break;

      case ITEM_MODEL_THROTTLE_REVERSED:
        ON_OFF_MENU_ITEM(g_model.throttleReversed, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEREVERSE, attr, event ) ;
        break;

      case ITEM_MODEL_THROTTLE_TRACE:
      {
        lcd_putsLeft(y, STR_TTRACE);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, g_model.thrTraceSrc, NUM_POTS+NUM_CHNOUT, isThrottleSourceAvailable);
        uint8_t idx = g_model.thrTraceSrc + MIXSRC_Thr;
        if (idx > MIXSRC_Thr)
          idx += 1;
        if (idx >= MIXSRC_FIRST_POT+NUM_POTS)
          idx += MIXSRC_CH1 - MIXSRC_FIRST_POT - NUM_POTS;
        putsMixerSource(MODEL_SETUP_2ND_COLUMN, y, idx, attr);
        break;
      }

      case ITEM_MODEL_THROTTLE_TRIM:
        ON_OFF_MENU_ITEM(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, STR_TTRIM, attr, event);
        break;

      case ITEM_MODEL_PREFLIGHT_LABEL:
        lcd_putsLeft(y, STR_PREFLIGHT);
        break;

      case ITEM_MODEL_CHECKLIST_DISPLAY:
        ON_OFF_MENU_ITEM(g_model.displayChecklist, MODEL_SETUP_2ND_COLUMN, y, STR_CHECKLIST, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_WARNING:
        g_model.disableThrottleWarning = !onoffMenuItem(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEWARNING, attr, event);
        break;

#if defined(REV9E)
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
#if defined(REV9E)
        if (i>=NUM_BODY_LINES-2 && getSwitchWarningsCount() > 8*(NUM_BODY_LINES-i)) {
          if (CURSOR_MOVED_LEFT(event))
            menuVerticalOffset--;
          else
            menuVerticalOffset++;
          break;
        }
#endif
        lcd_putsLeft(y, STR_SWITCHWARNING);
        swarnstate_t states = g_model.switchWarningState;
        char c;
        if (attr) {
          s_editMode = 0;
          if (!READ_ONLY()) {
            switch (event) {
              CASE_EVT_ROTARY_BREAK
              case EVT_KEY_BREAK(KEY_ENTER):
                break;

              case EVT_KEY_LONG(KEY_ENTER):
                if (menuHorizontalPosition < 0) {
                  START_NO_HIGHLIGHT();
                  getMovedSwitch();
                  g_model.switchWarningState = switches_states;
                  AUDIO_WARNING1();
                  eeDirty(EE_MODEL);
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
              eeDirty(EE_MODEL);
            }
            uint8_t swactive = !(g_model.switchWarningEnable & (1<<i));
            c = "\300-\301"[states & 0x03];
            lcd_putcAtt(MODEL_SETUP_2ND_COLUMN+qr.rem*(2*FW+1), y+FH*qr.quot, 'A'+i, line && (menuHorizontalPosition==current) ? INVERS : 0);
            if (swactive) lcd_putc(lcdNextPos, y+FH*qr.quot, c);
            ++current;
          }
          states >>= 2;
        }
        if (attr && menuHorizontalPosition < 0) {
#if defined(REV9E)
          drawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, 8*(2*FW+1), 1+FH*((current+7)/8));
#else
          drawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, current*(2*FW+1), FH+1);
#endif
        }
        break;
      }

      case ITEM_MODEL_POTS_WARNING:
#if defined(REV9E)
        if (i==NUM_BODY_LINES-1 && g_model.potsWarnMode) {
          if (CURSOR_MOVED_LEFT(event))
            menuVerticalOffset--;
          else
            menuVerticalOffset++;
          break;
        }
#endif

        lcd_putsLeft(y, STR_POTWARNING);
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, PSTR("\004""OFF\0""Man\0""Auto"), g_model.potsWarnMode, (menuHorizontalPosition == 0) ? attr : 0);
        if (attr && (menuHorizontalPosition == 0)) {
          CHECK_INCDEC_MODELVAR(event, g_model.potsWarnMode, POTS_WARN_OFF, POTS_WARN_AUTO);
          eeDirty(EE_MODEL);
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
                  eeDirty(EE_MODEL);
                }
                break;
              case EVT_KEY_BREAK(KEY_ENTER):
                g_model.potsWarnEnabled ^= (1 << (menuHorizontalPosition-1));
                eeDirty(EE_MODEL);
                break;
            }
          }
        }
        if (g_model.potsWarnMode) {
          coord_t x = MODEL_SETUP_2ND_COLUMN+28;
          for (int i=0; i<NUM_POTS; ++i) {
            if (i<NUM_XPOTS && !IS_POT_AVAILABLE(POT1+i)) {
              if (attr && (menuHorizontalPosition==i+1)) REPEAT_LAST_CURSOR_MOVE();
            }
            else {
#if defined(REV9E)
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
              lcd_putsnAtt(x, y, STR_VSRCRAW+2+STR_VSRCRAW[0]*(NUM_STICKS+1+i), STR_VSRCRAW[0]-1, flags & ~ZCHAR);
              x = lcdNextPos+3;
            }
          }
        }
        if (attr && menuHorizontalPosition < 0) {
#if defined(REV9E)
          drawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-FH-1, LCD_W-MODEL_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH+1, 2*FH+1);
#else
          drawFilledRect(MODEL_SETUP_2ND_COLUMN-1, y-1, LCD_W-MODEL_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH+1, FH+1);
#endif
        }
        break;

      case ITEM_MODEL_BEEP_CENTER:
      {
        lcd_putsLeft(y, STR_BEEPCTR);
        coord_t x = MODEL_SETUP_2ND_COLUMN;
        for (int i=0; i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS; i++) {
          if (i>=POT1 && i<POT1+NUM_XPOTS && !IS_POT_AVAILABLE(i)) {
            if (attr && menuHorizontalPosition == i) REPEAT_LAST_CURSOR_MOVE();
            continue;
          }
          lcd_putsiAtt(x, y, STR_RETA123, i, ((menuHorizontalPosition==i) && attr) ? BLINK|INVERS : (((g_model.beepANACenter & ((BeepANACenter)1<<i)) || (attr && CURSOR_ON_LINE())) ? INVERS : 0 ) );
          x += FW;
        }
        if (attr && CURSOR_ON_CELL) {
          if (event==EVT_KEY_BREAK(KEY_ENTER)) {
            if (READ_ONLY_UNLOCKED()) {
              s_editMode = 0;
              g_model.beepANACenter ^= ((BeepANACenter)1<<menuHorizontalPosition);
              eeDirty(EE_MODEL);
            }
          }
        }
        break;
      }

      case ITEM_MODEL_USE_GLOBAL_FUNCTIONS:
        lcd_putsLeft(y, STR_USE_GLOBAL_FUNCS);
        menu_lcd_onoff(MODEL_SETUP_2ND_COLUMN, y, !g_model.noGlobalFunctions, attr);
        if (attr) g_model.noGlobalFunctions = !checkIncDecModel(event, !g_model.noGlobalFunctions, 0, 1);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_LABEL:
        lcd_putsLeft(y, TR_INTERNALRF);
        break;
#if defined(TARANIS_INTERNAL_PPM)
      case ITEM_MODEL_INTERNAL_MODULE_MODE:
        lcd_putsLeft(y, STR_MODE);
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_TARANIS_PROTOCOLS, g_model.moduleData[INTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (IS_MODULE_XJT(INTERNAL_MODULE)) 
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[INTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        if (attr && s_editMode>0) {
          switch (menuHorizontalPosition) {
            case 0:
              g_model.moduleData[INTERNAL_MODULE].type = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].type, MODULE_TYPE_NONE, MODULE_TYPE_COUNT-2, EE_MODEL, isModuleAvailable);
              if (checkIncDec_Ret) {
                 g_model.moduleData[INTERNAL_MODULE].rfProtocol = 0;
                 g_model.moduleData[INTERNAL_MODULE].channelsStart = 0;
                 g_model.moduleData[INTERNAL_MODULE].channelsCount = 0;
              }
              break;
            case 1:              
              g_model.moduleData[INTERNAL_MODULE].rfProtocol = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].rfProtocol, RF_PROTO_X16, RF_PROTO_LAST, EE_MODEL, isRfProtocolAvailable);
              if (checkIncDec_Ret) {
                g_model.moduleData[INTERNAL_MODULE].channelsStart = 0;
                g_model.moduleData[INTERNAL_MODULE].channelsCount = 0;
              }
            }
          }
        break;
#else
      case ITEM_MODEL_INTERNAL_MODULE_MODE:
        lcd_putsLeft(y, STR_MODE);
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[0].rfProtocol, attr);
        if (attr) {
          g_model.moduleData[INTERNAL_MODULE].rfProtocol = checkIncDec(event, g_model.moduleData[INTERNAL_MODULE].rfProtocol, RF_PROTO_OFF, RF_PROTO_LAST, EE_MODEL, isRfProtocolAvailable);
          if (checkIncDec_Ret) {
            g_model.moduleData[0].type = MODULE_TYPE_XJT;
            g_model.moduleData[0].channelsStart = 0;
            g_model.moduleData[0].channelsCount = 0;
          }
        }
        break;
#endif
      case ITEM_MODEL_TRAINER_MODE:
        g_model.trainerMode = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_MODE, STR_VTRAINERMODES, g_model.trainerMode, 0, HAS_WIRELESS_TRAINER_HARDWARE() ? TRAINER_MODE_MASTER_BATTERY_COMPARTMENT : TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE, attr, event);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_LABEL:
        lcd_putsLeft(y, TR_EXTERNALRF);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_MODE:
        lcd_putsLeft(y, STR_MODE);
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_TARANIS_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].type, menuHorizontalPosition==0 ? attr : 0);
        if (IS_MODULE_XJT(EXTERNAL_MODULE))
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[EXTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
        else if (IS_MODULE_DSM2(EXTERNAL_MODULE))
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_DSM_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, menuHorizontalPosition==1 ? attr : 0);
#if defined(MULTIMODULE)
        else if (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) {
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_MULTI_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].multi.rfProtocol, menuHorizontalPosition==1 ? attr : 0);
          switch(g_model.moduleData[EXTERNAL_MODULE].multi.rfProtocol) {
            case MM_RF_PROTO_FLYSKY:
              lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+11*FW, y, STR_SUBTYPE_FLYSKY, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_FRSKY:
              lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+11*FW, y, STR_XJT_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].subType+1, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_HISKY:
              lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, STR_SUBTYPE_HISKY, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_DSM2:
              lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+11*FW, y, STR_DSM_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].subType+1, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_YD717:
              lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, STR_SUBTYPE_YD717, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_KN:
              lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+11*FW, y, STR_SUBTYPE_KN, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_SYMAX:
              lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+11*FW, y, STR_SUBTYPE_SYMAX, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_CX10:
              lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, STR_SUBTYPE_CX10, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_CG023:
              lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, STR_SUBTYPE_CG023, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_MT99XX:
              lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+11*FW, y, STR_SUBTYPE_MT99, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
              break;
            case MM_RF_PROTO_MJXQ:
              lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+11*FW, y, STR_SUBTYPE_MJXQ, g_model.moduleData[EXTERNAL_MODULE].subType, menuHorizontalPosition==2 ? attr : 0);
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
                g_model.moduleData[EXTERNAL_MODULE].channelsCount = 0;
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
                g_model.moduleData[EXTERNAL_MODULE].rfProtocol = checkIncDec(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, RF_PROTO_X16, RF_PROTO_LAST, EE_MODEL, isRfProtocolAvailable);
              if (checkIncDec_Ret) {
                g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsCount = 0;
              }
              break;
#if defined(MULTIMODULE)
            case 2:
              switch (g_model.moduleData[EXTERNAL_MODULE].multi.rfProtocol) {
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
              }
              break;
#endif
          }
        }
        break;

      case ITEM_MODEL_TRAINER_LABEL:
        lcd_putsLeft(y, STR_TRAINER);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_CHANNELS:
      case ITEM_MODEL_EXTERNAL_MODULE_CHANNELS:
      case ITEM_MODEL_TRAINER_CHANNELS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcd_putsLeft(y, STR_CHANNELRANGE);
        if ((int8_t)PORT_CHANNELS_ROWS(moduleIdx) >= 0) {
          lcd_putsAtt(MODEL_SETUP_2ND_COLUMN, y, STR_CH, menuHorizontalPosition==0 ? attr : 0);
          lcd_outdezAtt(lcdLastPos, y, moduleData.channelsStart+1, LEFT | (menuHorizontalPosition==0 ? attr : 0));
          lcd_putc(lcdLastPos, y, '-');
          lcd_outdezAtt(lcdLastPos + FW+1, y, moduleData.channelsStart+NUM_CHANNELS(moduleIdx), LEFT | (menuHorizontalPosition==1 ? attr : 0));
          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.channelsStart, 32-8-moduleData.channelsCount);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, moduleData.channelsCount, -4, min<int8_t>(MAX_CHANNELS(moduleIdx), 32-8-moduleData.channelsStart));
                
#if defined(TARANIS_INTERNAL_PPM)
                if ((k == ITEM_MODEL_EXTERNAL_MODULE_CHANNELS && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM) || (k == ITEM_MODEL_INTERNAL_MODULE_CHANNELS && g_model.moduleData[INTERNAL_MODULE].type == MODULE_TYPE_PPM) || (k == ITEM_MODEL_TRAINER_CHANNELS)) {
                  SET_DEFAULT_PPM_FRAME_LENGTH(moduleIdx);
                }
#else
                if ((k == ITEM_MODEL_EXTERNAL_MODULE_CHANNELS && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM) || (k == ITEM_MODEL_TRAINER_CHANNELS)) {
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
      case ITEM_MODEL_TRAINER_SETTINGS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        if (IS_MODULE_PPM(moduleIdx)) {
          lcd_putsLeft(y, STR_PPMFRAME);
          lcd_puts(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
          lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppmFrameLength*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
          lcd_putc(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
          lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (moduleData.ppmDelay*50)+300, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
          lcd_putcAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, moduleData.ppmPulsePol ? '+' : '-', (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);

          if (attr && s_editMode>0) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR(event, moduleData.ppmFrameLength, -20, 35);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, moduleData.ppmDelay, -4, 10);
                break;
              case 2:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.ppmPulsePol, 1);
                break;
            }
          }
        }
        else {
          horzpos_t l_posHorz = menuHorizontalPosition;
          coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
          if (IS_MODULE_XJT(moduleIdx) && g_model.moduleData[moduleIdx].rfProtocol == RF_PROTO_D8) {
            xOffsetBind = 0;
            lcd_putsLeft(y, INDENT "Receiver");
            if (attr) l_posHorz += 1;
          }
          else {
            lcd_putsLeft(y, STR_RXNUM);
          }
          if (IS_MODULE_XJT(moduleIdx) || IS_MODULE_DSM2(moduleIdx) || IS_MODULE_MULTIMODULE(moduleIdx)) {
            if (xOffsetBind) lcd_outdezNAtt(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[moduleIdx], (l_posHorz==0 ? attr : 0) | LEADING0|LEFT, 2);
            if (attr && l_posHorz==0) {
              if (s_editMode>0) {
                CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[moduleIdx],
                                           IS_MODULE_DSM2(moduleIdx) ? 20 :
                                           IS_MODULE_MULTIMODULE(moduleIdx) ? 15 : 63);
                if (checkIncDec_Ret) {
                  modelHeaders[g_eeGeneral.currModel].modelId[moduleIdx] = g_model.header.modelId[moduleIdx];
                }
              }
              if (s_editMode==0 && event==EVT_KEY_BREAK(KEY_ENTER)) {
                checkModelIdUnique(g_eeGeneral.currModel, moduleIdx);
              }
            }
            lcd_putsAtt(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
            lcd_putsAtt(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);
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
        lcd_putsLeft(y, TR_FAILSAFE);
        if (IS_MODULE_XJT(moduleIdx)) {
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_VFAILSAFE, moduleData.failsafeMode, menuHorizontalPosition==0 ? attr : 0);
          if (moduleData.failsafeMode == FAILSAFE_CUSTOM) lcd_putsAtt(MODEL_SETUP_2ND_COLUMN + MODEL_SETUP_SET_FAILSAFE_OFS, y, STR_SET, menuHorizontalPosition==1 ? attr : 0);
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
              if (moduleData.failsafeMode==FAILSAFE_CUSTOM && event==EVT_KEY_FIRST(KEY_ENTER)) {
                g_moduleIdx = moduleIdx;
                pushMenu(menuModelFailsafe);
              }
            }
            else {
              drawFilledRect(MODEL_SETUP_2ND_COLUMN, y, LCD_W-MODEL_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH, 8);
            }
          }
        }
#if defined(MULTIMODULE)
        else if (IS_MODULE_MULTIMODULE(moduleIdx)) {
            switch (g_model.moduleData[moduleIdx].multi.rfProtocol) {
              case MM_RF_PROTO_FRSKY:
                lcd_putsLeft(y, TR_MULTI_RFTUNE);
                break;
              case MM_RF_PROTO_HUBSAN:
                lcd_putsLeft(y, TR_MULTI_VIDFREQ);
                break;
            }
            lcd_outdezNAtt(MODEL_SETUP_2ND_COLUMN, y, g_model.moduleData[moduleIdx].multi.optionValue, LEFT|attr);
            if (attr)
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[moduleIdx].multi.optionValue, -128, 127);
        }
#endif
        break;
      }
    }
  }

#if defined(PXX)
  if (IS_RANGECHECK_ENABLE()) {
    displayPopup("RSSI: ");
    lcd_outdezAtt(16+4*FW, 5*FH, TELEMETRY_RSSI(), BOLD);
  }
#endif
}

void menuModelFailsafe(uint8_t event)
{
  static bool longNames = false;
  bool newLongNames = false;
  uint8_t ch = 0;
  uint8_t channelStart = g_model.moduleData[g_moduleIdx].channelsStart;

  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    event = 0;
    if (s_editMode) {
      g_model.moduleData[g_moduleIdx].failsafeChannels[menuVerticalPosition] = channelOutputs[menuVerticalPosition+channelStart];
      eeDirty(EE_MODEL);
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
      eeDirty(EE_MODEL);
      AUDIO_WARNING1();
      SEND_FAILSAFE_NOW(g_moduleIdx);
    }
  }

  SIMPLE_SUBMENU_NOTITLE(NUM_CHANNELS(g_moduleIdx));

  SET_SCROLLBAR_X(0);

  #define COL_W   (LCD_W/2)
  const uint8_t SLIDER_W = 64;
  // Column separator
  lcd_vline(LCD_W/2, FH, LCD_H-FH);

  lcd_putsCenter(0*FH, FAILSAFESET);
  lcd_invert_line(0);

  unsigned int lim = g_model.extendedLimits ? 640*2 : 512*2;

  for (uint8_t col=0; col<2; col++) {
    coord_t x = col*COL_W+1;

    // Channels
    for (uint8_t line=0; line<8; line++) {
      coord_t y = 9+line*7;
      int32_t channelValue = channelOutputs[ch+channelStart];
      int32_t failsafeValue = 0;
      bool failsafeEditable = false;
      uint8_t ofs = (col ? 0 : 1);

      if (ch < NUM_CHANNELS(g_moduleIdx)) {
        failsafeValue = g_model.moduleData[g_moduleIdx].failsafeChannels[8*col+line];
        failsafeEditable = true;
      }

      if (failsafeEditable) {
        // Channel name if present, number if not
        uint8_t lenLabel = ZLEN(g_model.limitData[ch+channelStart].name);
        if (lenLabel > 4) {
          newLongNames = longNames = true;
        }
  
        if (lenLabel > 0)
          lcd_putsnAtt(x+1-ofs, y, g_model.limitData[ch+channelStart].name, sizeof(g_model.limitData[ch+channelStart].name), ZCHAR | SMLSIZE);
        else
          putsChn(x+1-ofs, y, ch+1, SMLSIZE);
  
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

#if defined(PPM_UNIT_PERCENT_PREC1)
        uint8_t wbar = (longNames ? SLIDER_W-16 : SLIDER_W-6);
#else
        uint8_t wbar = (longNames ? SLIDER_W-10 : SLIDER_W);
#endif
  
        if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
          lcd_putsAtt(x+COL_W-4-wbar-ofs-16, y, "HOLD", flags);
          failsafeValue = 0;
        }
        else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
          lcd_putsAtt(x+COL_W-4-wbar-ofs-16, y, "NONE", flags);
          failsafeValue = 0;
        }
        else {
#if defined(PPM_UNIT_US)
          lcd_outdezAtt(x+COL_W-4-wbar-ofs, y, PPM_CH_CENTER(ch)+failsafeValue/2, flags);
#elif defined(PPM_UNIT_PERCENT_PREC1)
          lcd_outdezAtt(x+COL_W-4-wbar-ofs, y, calcRESXto1000(failsafeValue), PREC1|flags);
#else
          lcd_outdezAtt(x+COL_W-4-wbar-ofs, y, calcRESXto1000(failsafeValue)/10, flags);
#endif
        }
  
        // Gauge
        lcd_rect(x+COL_W-3-wbar-ofs, y, wbar+1, 6);
        unsigned int lenChannel = limit((uint8_t)1, uint8_t((abs(channelValue) * wbar/2 + lim/2) / lim), uint8_t(wbar/2));
        unsigned int lenFailsafe = limit((uint8_t)1, uint8_t((abs(failsafeValue) * wbar/2 + lim/2) / lim), uint8_t(wbar/2));
        coord_t xChannel = (channelValue>0) ? x+COL_W-ofs-3-wbar/2 : x+COL_W-ofs-2-wbar/2-lenChannel;
        coord_t xFailsafe = (failsafeValue>0) ? x+COL_W-ofs-3-wbar/2 : x+COL_W-ofs-2-wbar/2-lenFailsafe;
        lcd_hlineStip(xChannel, y+1, lenChannel, DOTTED, 0);
        lcd_hlineStip(xChannel, y+2, lenChannel, DOTTED, 0);
        lcd_hline(xFailsafe, y+3, lenFailsafe);
        lcd_hline(xFailsafe, y+4, lenFailsafe);
      }
      ch++;
    }
  }

  longNames = newLongNames;
}
