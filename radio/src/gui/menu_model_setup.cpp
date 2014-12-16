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

#include "../opentx.h"

#if defined(CPUARM)
uint8_t g_moduleIdx;
void menuModelFailsafe(uint8_t event);
#endif

enum menuModelSetupItems {
  ITEM_MODEL_NAME,
  CASE_PCBTARANIS(ITEM_MODEL_BITMAP)
  ITEM_MODEL_TIMER1,
  CASE_CPUARM(ITEM_MODEL_TIMER1_NAME)
  CASE_PERSISTENT_TIMERS(ITEM_MODEL_TIMER1_PERSISTENT)
  ITEM_MODEL_TIMER1_MINUTE_BEEP,
  ITEM_MODEL_TIMER1_COUNTDOWN_BEEP,
  ITEM_MODEL_TIMER2,
  CASE_CPUARM(ITEM_MODEL_TIMER2_NAME)
  CASE_PERSISTENT_TIMERS(ITEM_MODEL_TIMER2_PERSISTENT)
  ITEM_MODEL_TIMER2_MINUTE_BEEP,
  ITEM_MODEL_TIMER2_COUNTDOWN_BEEP,
  CASE_CPUARM(ITEM_MODEL_TIMER3)
  CASE_CPUARM(ITEM_MODEL_TIMER3_NAME)
  CASE_CPUARM(ITEM_MODEL_TIMER3_PERSISTENT)
  CASE_CPUARM(ITEM_MODEL_TIMER3_MINUTE_BEEP)
  CASE_CPUARM(ITEM_MODEL_TIMER3_COUNTDOWN_BEEP)
  ITEM_MODEL_EXTENDED_LIMITS,
  ITEM_MODEL_EXTENDED_TRIMS,
  CASE_CPUARM(ITEM_MODEL_DISPLAY_TRIMS)
  ITEM_MODEL_TRIM_INC,
  CASE_PCBTARANIS(ITEM_MODEL_THROTTLE_LABEL)
  ITEM_MODEL_THROTTLE_REVERSED,
  ITEM_MODEL_THROTTLE_TRACE,
  ITEM_MODEL_THROTTLE_TRIM,
  CASE_CPUARM(ITEM_MODEL_PREFLIGHT_LABEL)
  CASE_CPUARM(ITEM_MODEL_CHECKLIST_DISPLAY)
  ITEM_MODEL_THROTTLE_WARNING,
  ITEM_MODEL_SWITCHES_WARNING,
  CASE_PCBTARANIS(ITEM_MODEL_SWITCHES_WARNING2)
  CASE_PCBTARANIS(ITEM_MODEL_POT_WARNING)
  ITEM_MODEL_BEEP_CENTER,
  CASE_CPUARM(ITEM_MODEL_USE_GLOBAL_FUNCTIONS)
#if defined(PCBTARANIS)
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
#elif defined(CPUARM)
  ITEM_MODEL_EXTERNAL_MODULE_LABEL,
  ITEM_MODEL_EXTERNAL_MODULE_MODE,
  ITEM_MODEL_EXTERNAL_MODULE_CHANNELS,
  ITEM_MODEL_EXTERNAL_MODULE_BIND,
  ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE,
#if defined(PCBSKY9X) && !defined(REVA) && !defined(REVX)
  ITEM_MODEL_EXTRA_MODULE_LABEL,
  ITEM_MODEL_EXTRA_MODULE_CHANNELS,
  ITEM_MODEL_EXTRA_MODULE_BIND,
#endif
#else
  ITEM_MODEL_PPM1_PROTOCOL,
  ITEM_MODEL_PPM1_PARAMS,
#endif
  ITEM_MODEL_SETUP_MAX
};

#if defined(PCBSKY9X)
  #define FIELD_PROTOCOL_MAX 2
#else
  #define FIELD_PROTOCOL_MAX 1
#endif

#if LCD_W >= 212
  #define MODEL_SETUP_2ND_COLUMN        (LCD_W-17*FW-MENUS_SCROLLBAR_WIDTH)
  #define MODEL_SETUP_BIND_OFS          3*FW-2
  #define MODEL_SETUP_RANGE_OFS         7*FW
  #define MODEL_SETUP_SET_FAILSAFE_OFS  10*FW
#else
  #define MODEL_SETUP_2ND_COLUMN        (LCD_W-11*FW-MENUS_SCROLLBAR_WIDTH)
  #define MODEL_SETUP_BIND_OFS          2*FW+1
  #define MODEL_SETUP_RANGE_OFS         4*FW+3
  #define MODEL_SETUP_SET_FAILSAFE_OFS  7*FW-2
#endif

#if defined(PCBTARANIS) && defined(SDCARD)
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
      s_menu_flags = 0;
    }
  }
  else {
    // The user choosed a bmp file in the list
    copySelection(g_model.header.bitmap, result, sizeof(g_model.header.bitmap));
    LOAD_MODEL_BITMAP();
    memcpy(modelHeaders[g_eeGeneral.currModel].bitmap, g_model.header.bitmap, sizeof(g_model.header.bitmap));
    eeDirty(EE_MODEL);
  }
}
#endif

#if defined(PCBTARANIS)
  #define CURRENT_MODULE_EDITED(k)         (k>=ITEM_MODEL_TRAINER_LABEL ? TRAINER_MODULE : (k>=ITEM_MODEL_EXTERNAL_MODULE_LABEL ? EXTERNAL_MODULE : INTERNAL_MODULE))
#elif defined(PCBSKY9X) && !defined(REVA) && !defined(REVX)
  #define CURRENT_MODULE_EDITED(k)         (k>=ITEM_MODEL_EXTRA_MODULE_LABEL ? EXTRA_MODULE : EXTERNAL_MODULE)
#else
  #define CURRENT_MODULE_EDITED(k)         (EXTERNAL_MODULE)
#endif

void menuModelSetup(uint8_t event)
{
#if defined(PCBTARANIS)
  horzpos_t l_posHorz = m_posHorz;
  #define IF_INTERNAL_MODULE_ON(x)          (g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF ? HIDDEN_ROW : (uint8_t)(x))
  #define IF_EXTERNAL_MODULE_ON(x)          (g_model.externalModule == MODULE_TYPE_NONE ? HIDDEN_ROW : (uint8_t)(x))
  #define IF_TRAINER_ON(x)                  (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)(x) : HIDDEN_ROW)
  #define IF_EXTERNAL_MODULE_XJT(x)         (IS_MODULE_XJT(EXTERNAL_MODULE) ? (uint8_t)x : HIDDEN_ROW)
  #define IS_D8_RX(x)                       (g_model.moduleData[x].rfProtocol == RF_PROTO_D8)
  #define INTERNAL_MODULE_CHANNELS_ROWS()   IF_INTERNAL_MODULE_ON(1)
  #define EXTERNAL_MODULE_CHANNELS_ROWS()   IF_EXTERNAL_MODULE_ON(IS_MODULE_DSM2(EXTERNAL_MODULE) ? (uint8_t)0 : (uint8_t)1)
  #define TRAINER_CHANNELS_ROWS()           IF_TRAINER_ON(1)
  #define PORT_CHANNELS_ROWS(x)             (x==INTERNAL_MODULE ? INTERNAL_MODULE_CHANNELS_ROWS() : (x==EXTERNAL_MODULE ? EXTERNAL_MODULE_CHANNELS_ROWS() : TRAINER_CHANNELS_ROWS()))
  #define FAILSAFE_ROWS(x)                  ((g_model.moduleData[x].rfProtocol==RF_PROTO_X16 || g_model.moduleData[x].rfProtocol==RF_PROTO_LR12) ? (g_model.moduleData[x].failsafeMode==FAILSAFE_CUSTOM ? (uint8_t)1 : (uint8_t)0) : HIDDEN_ROW)
  #define MODEL_SETUP_MAX_LINES             (1+ITEM_MODEL_SETUP_MAX)
  #define POT_WARN_ITEMS()                  ((g_model.nPotsToWarn >> 6) ? (uint8_t)NUM_POTS : (uint8_t)0)
  #define TIMER_ROWS                        2, 0, CASE_PERSISTENT_TIMERS(0) 0, 0
  bool CURSOR_ON_CELL = (m_posHorz >= 0);
  MENU_TAB({ 0, 0, CASE_PCBTARANIS(0) TIMER_ROWS, TIMER_ROWS, TIMER_ROWS, 0, 1, 0, 0, CASE_PCBTARANIS(LABEL(Throttle)) 0, 0, 0, CASE_CPUARM(LABEL(PreflightCheck)) CASE_CPUARM(0) 0, uint8_t(NAVIGATION_LINE_BY_LINE|getSwitchWarningsAllowed()), ONE_2x2POS_DEFINED() ? TITLE_ROW : HIDDEN_ROW, POT_WARN_ITEMS(), NAVIGATION_LINE_BY_LINE|(NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1), 0, LABEL(InternalModule), 0, IF_INTERNAL_MODULE_ON(1), IF_INTERNAL_MODULE_ON(IS_D8_RX(0) ? (uint8_t)1 : (uint8_t)2), IF_INTERNAL_MODULE_ON(FAILSAFE_ROWS(INTERNAL_MODULE)), LABEL(ExternalModule), (IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE)) ? (uint8_t)1 : (uint8_t)0, EXTERNAL_MODULE_CHANNELS_ROWS(), (IS_MODULE_XJT(EXTERNAL_MODULE) && IS_D8_RX(EXTERNAL_MODULE)) ? (uint8_t)1 : (IS_MODULE_PPM(EXTERNAL_MODULE) || IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW, IF_EXTERNAL_MODULE_XJT(FAILSAFE_ROWS(EXTERNAL_MODULE)), LABEL(Trainer), 0, TRAINER_CHANNELS_ROWS(), IF_TRAINER_ON(2)});
#elif defined(CPUARM)
  #define IF_EXTERNAL_MODULE_XJT(x)         (IS_MODULE_XJT(EXTERNAL_MODULE) ? (uint8_t)x : HIDDEN_ROW)
  #define IF_EXTERNAL_MODULE_ON(x)          (g_model.externalModule == MODULE_TYPE_NONE ? HIDDEN_ROW : (uint8_t)(x))
  #define IS_D8_RX(x)                       (g_model.moduleData[x].rfProtocol == RF_PROTO_D8)
  #define EXTERNAL_MODULE_CHANNELS_ROWS()   IF_EXTERNAL_MODULE_ON(IS_MODULE_DSM2(EXTERNAL_MODULE) ? (uint8_t)0 : (uint8_t)1)
  #define TRAINER_CHANNELS_ROWS()           (HIDDEN_ROW)
  #define PORT_CHANNELS_ROWS(x)             (x==EXTERNAL_MODULE ? EXTERNAL_MODULE_CHANNELS_ROWS() : 0)
  #define FAILSAFE_ROWS(x)                  ((g_model.moduleData[x].rfProtocol==RF_PROTO_X16 || g_model.moduleData[x].rfProtocol==RF_PROTO_LR12) ? (g_model.moduleData[x].failsafeMode==FAILSAFE_CUSTOM ? (uint8_t)1 : (uint8_t)0) : HIDDEN_ROW)
  #define CURSOR_ON_CELL                    (true)
  #define MODEL_SETUP_MAX_LINES             (1+ITEM_MODEL_SETUP_MAX)
  #define POT_WARN_ITEMS()                  ((g_model.nPotsToWarn >> 6) ? (uint8_t)NUM_POTS : (uint8_t)0)
  #define TIMER_ROWS                        2, 0, CASE_PERSISTENT_TIMERS(0) 0, 0
#if (defined(PCBSKY9X) && !defined(REVA) && !defined(REVX))
  #define EXTRA_MODULE_ROWS                 LABEL(ExtraModule), 1, 2,
#else
  #define EXTRA_MODULE_ROWS
#endif
  #define TRAINER_MODULE_ROWS
  MENU_TAB({ 0, 0, TIMER_ROWS, TIMER_ROWS, TIMER_ROWS, 0, 1, 0, 0, CASE_PCBTARANIS(LABEL(Throttle)) 0, 0, 0, CASE_CPUARM(LABEL(PreflightCheck)) CASE_CPUARM(0) 0, 6, NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1, 0, LABEL(ExternalModule), (IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE)) ? (uint8_t)1 : (uint8_t)0, EXTERNAL_MODULE_CHANNELS_ROWS(), (IS_MODULE_XJT(EXTERNAL_MODULE) && IS_D8_RX(EXTERNAL_MODULE)) ? (uint8_t)1 : (IS_MODULE_PPM(EXTERNAL_MODULE) || IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW, IF_EXTERNAL_MODULE_XJT(FAILSAFE_ROWS(EXTERNAL_MODULE)), EXTRA_MODULE_ROWS TRAINER_MODULE_ROWS });
#elif defined(CPUM64)
  #define CURSOR_ON_CELL                    (true)
  #define MODEL_SETUP_MAX_LINES             ((IS_PPM_PROTOCOL(protocol)||IS_DSM2_PROTOCOL(protocol)||IS_PXX_PROTOCOL(protocol)) ? 1+ITEM_MODEL_SETUP_MAX : ITEM_MODEL_SETUP_MAX)
  uint8_t protocol = g_model.protocol;
  MENU_TAB({ 0, 0, CASE_PCBTARANIS(0) 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1, FIELD_PROTOCOL_MAX, 2 });
#else
  #define CURSOR_ON_CELL                    (true)
  #define MODEL_SETUP_MAX_LINES             ((IS_PPM_PROTOCOL(protocol)||IS_DSM2_PROTOCOL(protocol)||IS_PXX_PROTOCOL(protocol)) ? 1+ITEM_MODEL_SETUP_MAX : ITEM_MODEL_SETUP_MAX)
  uint8_t protocol = g_model.protocol;
  MENU_TAB({ 0, 0, CASE_PCBTARANIS(0) 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 0, 1, 0, 0, 0, 0, 0, NUM_SWITCHES, NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1, FIELD_PROTOCOL_MAX, 2, CASE_PCBSKY9X(1) CASE_PCBSKY9X(2) });
#endif

  MENU_CHECK(menuTabModel, e_ModelSetup, MODEL_SETUP_MAX_LINES);

#if defined(CPUARM) && (defined(DSM2) || defined(PXX))
  if (menuEvent) {
    moduleFlag[0] = 0;
#if NUM_MODULES > 1
    moduleFlag[1] = 0;
#endif
  }
#endif

  TITLE(STR_MENUSETUP);

  uint8_t sub = m_posVert - 1;
  int8_t editMode = s_editMode;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_TITLE_HEIGHT + 1 + i*FH;
    uint8_t k = i+s_pgOfs;
#if defined(CPUARM)
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j+1] == HIDDEN_ROW)
        k++;
    }
#endif

    uint8_t blink = ((editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_MODEL_NAME:
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_MODELNAME, g_model.header.name, sizeof(g_model.header.name), event, attr);
#if defined(CPUARM)
        memcpy(modelHeaders[g_eeGeneral.currModel].name, g_model.header.name, sizeof(g_model.header.name));
#endif
        break;

#if defined(PCBTARANIS) && defined(SDCARD)
      case ITEM_MODEL_BITMAP:
        lcd_putsLeft(y, STR_BITMAP);
        if (ZEXIST(g_model.header.bitmap))
          lcd_putsnAtt(MODEL_SETUP_2ND_COLUMN, y, g_model.header.bitmap, sizeof(g_model.header.bitmap), attr);
        else
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_VCSWFUNC, 0, attr);
        if (attr && event==EVT_KEY_BREAK(KEY_ENTER) && READ_ONLY_UNLOCKED()) {
          s_editMode = 0;
          if (listSdFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(g_model.header.bitmap), g_model.header.bitmap, LIST_NONE_SD_FILE)) {
            menuHandler = onModelSetupBitmapMenu;
          }
          else {
            POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
            s_menu_flags = 0;
          }
        }
        break;
#endif

#if defined(CPUARM)
      case ITEM_MODEL_TIMER1:
      case ITEM_MODEL_TIMER2:
      case ITEM_MODEL_TIMER3:
      {
        unsigned int timerIdx = (k>=ITEM_MODEL_TIMER3 ? 2 : (k>=ITEM_MODEL_TIMER2 ? 1 : 0));
        TimerData * timer = &g_model.timers[timerIdx];
        putsStrIdx(0*FW, y, STR_TIMER, timerIdx+1);
        putsTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer->mode, m_posHorz==0 ? attr : 0);
        putsTimer(MODEL_SETUP_2ND_COLUMN+5*FW-2+5*FWNUM+1, y, timer->start, m_posHorz==1 ? attr : 0, m_posHorz==2 ? attr : 0);
#if defined(PCBTARANIS)
        if (attr && m_posHorz < 0) lcd_filled_rect(MODEL_SETUP_2ND_COLUMN, y, LCD_W-MODEL_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH, 8);
#endif
        if (attr && (editMode>0 || p1valdiff)) {
          div_t qr = div(timer->start, 60);
          switch (m_posHorz) {
            case 0:
              CHECK_INCDEC_MODELVAR_CHECK(event, timer->mode, SWSRC_FIRST, TMRMODE_COUNT+SWSRC_LAST-1/*SWSRC_None removed*/, isSwitchAvailableInTimers);
#if defined(AUTOSWITCH)
              if (s_editMode>0) {
                int8_t val = timer->mode - (TMRMODE_COUNT-1);
                int8_t switchVal = checkIncDecMovedSwitch(val);
                if (val != switchVal) {
                  timer->mode = switchVal + (TMRMODE_COUNT-1);
                  eeDirty(EE_MODEL);
                }
              }
#endif
              break;
            case 1:
              CHECK_INCDEC_MODELVAR_ZERO(event, qr.quot, 59);
              timer->start = qr.rem + qr.quot*60;
              break;
            case 2:
              qr.rem -= checkIncDecModel(event, qr.rem+2, 1, 62)-2;
              timer->start -= qr.rem ;
              if ((int16_t)timer->start < 0) timer->start=0;
              break;
          }
        }
        break;
      }

      case ITEM_MODEL_TIMER1_NAME:
      case ITEM_MODEL_TIMER2_NAME:
      case ITEM_MODEL_TIMER3_NAME:
      {
        TimerData * timer = &g_model.timers[k>=ITEM_MODEL_TIMER3 ? 2 : (k>=ITEM_MODEL_TIMER2 ? 1 : 0)];
        editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_TIMER_NAME, timer->name, sizeof(timer->name), event, attr);
        break;
      }

      case ITEM_MODEL_TIMER1_MINUTE_BEEP:
      case ITEM_MODEL_TIMER2_MINUTE_BEEP:
      case ITEM_MODEL_TIMER3_MINUTE_BEEP:
      {
        TimerData * timer = &g_model.timers[k>=ITEM_MODEL_TIMER3 ? 2 : (k>=ITEM_MODEL_TIMER2 ? 1 : 0)];
        timer->minuteBeep = onoffMenuItem(timer->minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
        break;
      }

      case ITEM_MODEL_TIMER1_COUNTDOWN_BEEP:
      case ITEM_MODEL_TIMER2_COUNTDOWN_BEEP:
      case ITEM_MODEL_TIMER3_COUNTDOWN_BEEP:
      {
        TimerData * timer = &g_model.timers[k>=ITEM_MODEL_TIMER3 ? 2 : (k>=ITEM_MODEL_TIMER2 ? 1 : 0)];
        timer->countdownBeep = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, STR_VBEEPCOUNTDOWN, timer->countdownBeep, 0, 2, attr, event);
        break;
      }

      case ITEM_MODEL_TIMER1_PERSISTENT:
      case ITEM_MODEL_TIMER2_PERSISTENT:
      case ITEM_MODEL_TIMER3_PERSISTENT:
      {
        TimerData * timer = &g_model.timers[k>=ITEM_MODEL_TIMER3 ? 2 : (k>=ITEM_MODEL_TIMER2 ? 1 : 0)];
        timer->persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, timer->persistent, 0, 2, attr, event);
        break;
      }
#else
      case ITEM_MODEL_TIMER1:
      case ITEM_MODEL_TIMER2:
      case ITEM_MODEL_TIMER1_MINUTE_BEEP:
      case ITEM_MODEL_TIMER2_MINUTE_BEEP:
      case ITEM_MODEL_TIMER1_COUNTDOWN_BEEP:
      case ITEM_MODEL_TIMER2_COUNTDOWN_BEEP:
      {
        TimerData *timer = &g_model.timers[k>=ITEM_MODEL_TIMER2 ? 1 : 0];
        if (k==ITEM_MODEL_TIMER1_MINUTE_BEEP || k==ITEM_MODEL_TIMER2_MINUTE_BEEP) {
          timer->minuteBeep = onoffMenuItem(timer->minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
        }
        else if (k==ITEM_MODEL_TIMER1_COUNTDOWN_BEEP || k==ITEM_MODEL_TIMER2_COUNTDOWN_BEEP) {
          timer->countdownBeep = onoffMenuItem(timer->countdownBeep, MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, attr, event);
        }
        else {
          putsStrIdx(0*FW, y, STR_TIMER, k>=ITEM_MODEL_TIMER2 ? 2 : 1);
          putsTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer->mode, m_posHorz==0 ? attr : 0);
          putsTimer(MODEL_SETUP_2ND_COLUMN+5*FW-2+5*FWNUM+1, y, timer->start, m_posHorz==1 ? attr : 0, m_posHorz==2 ? attr : 0);
          if (attr && (editMode>0 || p1valdiff)) {
            div_t qr = div(timer->start, 60);
            switch (m_posHorz) {
              case 0:
                CHECK_INCDEC_MODELVAR_CHECK(event, timer->mode, SWSRC_FIRST, TMRMODE_COUNT+SWSRC_LAST-1/*SWSRC_None removed*/, isSwitchAvailableInTimers);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR_ZERO(event, qr.quot, 59);
                timer->start = qr.rem + qr.quot*60;
                break;
              case 2:
                qr.rem -= checkIncDecModel(event, qr.rem+2, 1, 62)-2;
                timer->start -= qr.rem ;
                break;
            }
          }
        }
        break;
      }

#if defined(PCBGRUVIN9X)
      case ITEM_MODEL_TIMER1_PERSISTENT:
      case ITEM_MODEL_TIMER2_PERSISTENT:
      {
        TimerData &timer = g_model.timers[k==ITEM_MODEL_TIMER2_PERSISTENT];
        timer.persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, timer.persistent, 0, 2, attr, event);
        break;
      }
#endif
#endif

      case ITEM_MODEL_EXTENDED_LIMITS:
        ON_OFF_MENU_ITEM(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, STR_ELIMITS, attr, event);
        break;

      case ITEM_MODEL_EXTENDED_TRIMS:
#if defined(CPUM64)
        ON_OFF_MENU_ITEM(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, STR_ETRIMS, attr, event);
#else
        ON_OFF_MENU_ITEM(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, STR_ETRIMS, m_posHorz<=0 ? attr : 0, event==EVT_KEY_BREAK(KEY_ENTER) ? event : 0);
        lcd_putsAtt(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_RESET_BTN, m_posHorz>0  && !s_noHi ? attr : 0);
        if (attr && m_posHorz>0) {
          s_editMode = 0;
          if (event==EVT_KEY_LONG(KEY_ENTER)) {
            s_noHi = NO_HI_LEN;
            for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
              memclear(&g_model.flightModeData[i], TRIMS_ARRAY_SIZE);
            }
            eeDirty(EE_MODEL);
            AUDIO_WARNING1();
          }
        }
#endif
        break;

#if defined(CPUARM)
      case ITEM_MODEL_DISPLAY_TRIMS:
        g_model.displayTrims = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, "Display Trims", "\006No\0   ChangeYes", g_model.displayTrims, 0, 2, attr, event);
        break;
#endif

      case ITEM_MODEL_TRIM_INC:
        g_model.trimInc = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_TRIMINC, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
        break;

#if defined(PCBTARANIS)
      case ITEM_MODEL_THROTTLE_LABEL:
        lcd_putsLeft(y, STR_THROTTLE_LABEL);
        break;
#endif

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

#if defined(CPUARM)
      case ITEM_MODEL_PREFLIGHT_LABEL:
        lcd_putsLeft(y, STR_PREFLIGHT);
        break;

      case ITEM_MODEL_CHECKLIST_DISPLAY:
        ON_OFF_MENU_ITEM(g_model.displayChecklist, MODEL_SETUP_2ND_COLUMN, y, STR_CHECKLIST, attr, event);
        break;
#endif

      case ITEM_MODEL_THROTTLE_WARNING:
        g_model.disableThrottleWarning = !onoffMenuItem(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEWARNING, attr, event);
        break;

#if defined(PCBTARANIS)
      // TODO something more generic
      case ITEM_MODEL_SWITCHES_WARNING2:
        if (i==0) s_pgOfs++;
        break;
#endif

      case ITEM_MODEL_SWITCHES_WARNING:
#if defined(PCBTARANIS)
        // TODO something more generic
        if (i==LCD_LINES-2) {
          s_pgOfs++;
          break;
        }
#endif
      {
        lcd_putsLeft(y, STR_SWITCHWARNING);
        swarnstate_t states = g_model.switchWarningState;
        char c;
        if (attr) {
          s_editMode = 0;
          if (!READ_ONLY()) {
            switch (event) {
              CASE_EVT_ROTARY_BREAK
              case EVT_KEY_BREAK(KEY_ENTER):
#if defined(CPUM64)
                g_model.switchWarningEnable ^= (1 << m_posHorz);
                eeDirty(EE_MODEL);
#elif !defined(PCBTARANIS)
                if (m_posHorz < NUM_SWITCHES-1) {
                  g_model.switchWarningEnable ^= (1 << m_posHorz);
                  eeDirty(EE_MODEL);
                }
#endif
                break;

              case EVT_KEY_LONG(KEY_ENTER):
#if defined(CPUM64)
                getMovedSwitch();
                g_model.switchWarningState = switches_states;
                AUDIO_WARNING1();
                eeDirty(EE_MODEL);
#elif defined(PCBTARANIS)
                if (m_posHorz < 0) {
                  s_noHi = NO_HI_LEN;
                  getMovedSwitch();
                  g_model.switchWarningState = switches_states;
                  AUDIO_WARNING1();
                  eeDirty(EE_MODEL);
                }
#else
                if (m_posHorz == NUM_SWITCHES-1) {
                  s_noHi = NO_HI_LEN;
                  getMovedSwitch();
                  g_model.switchWarningState = switches_states;
                  AUDIO_WARNING1();
                  eeDirty(EE_MODEL);
                }
#endif
                killEvents(event);
                break;
            }
          }
        }

        LcdFlags line = attr;

#if defined(PCBTARANIS)
        for (int i=0, current=0; i<NUM_SWITCHES; i++) {
          div_t qr = div(i, 8);
          if (SWITCH_WARNING_ALLOWED(i)) {
            if (!READ_ONLY() && event==EVT_KEY_BREAK(KEY_ENTER) && line && l_posHorz==current) {
              g_model.switchWarningEnable ^= (1 << i);
              eeDirty(EE_MODEL);
            }
            uint8_t swactive = !(g_model.switchWarningEnable & (1<<i));
            c = "\300-\301"[states & 0x03];
            lcd_putcAtt(MODEL_SETUP_2ND_COLUMN+qr.rem*(2*FW+1), y+FH*qr.quot, 'A'+i, line && (m_posHorz==current) ? INVERS : 0);
            if (swactive) lcd_putc(lcdNextPos, y+FH*qr.quot, c);
            ++current;
          }
          states >>= 2;
        }
        if (attr && m_posHorz < 0) {
          lcd_filled_rect(MODEL_SETUP_2ND_COLUMN-1, y-1, 8*(2*FW+1), ONE_2x2POS_DEFINED() ? 2*FH+1 : FH+1);
        }
#else
        for (uint8_t i=0; i<NUM_SWITCHES-1/*not on TRN switch*/; i++) {
          uint8_t swactive = !(g_model.switchWarningEnable & 1 << i);
          attr = 0;

          if (IS_3POS(i)) {
            c = '0'+(states & 0x03);
            states >>= 2;
          }
          else {
            if ((states & 0x01) && swactive)
              attr = INVERS;
            c = pgm_read_byte(STR_VSWITCHES - 2 + 9 + (3*(i+1)));
            states >>= 1;
          }
          if (line && (m_posHorz == i)) {
            attr = BLINK;
            if (swactive)
              attr |= INVERS;
          }
          lcd_putcAtt(MODEL_SETUP_2ND_COLUMN+i*FW, y, (swactive || (attr & BLINK)) ? c : '-', attr);
#if !defined(CPUM64)
          lcd_putsAtt(MODEL_SETUP_2ND_COLUMN+(NUM_SWITCHES*FW), y, PSTR("<]"), (m_posHorz == NUM_SWITCHES-1 && !s_noHi) ? line : 0);
#endif
        }
#endif
        break;
      }

#if defined(PCBTARANIS)
      case ITEM_MODEL_POT_WARNING:
      {
        lcd_putsLeft(y, STR_POTWARNING);
        uint8_t potMode = g_model.nPotsToWarn >> 6;
        if (attr) {
          if (m_posHorz) s_editMode = 0;
          if (!READ_ONLY() && m_posHorz) {
            switch (event) {
              case EVT_KEY_LONG(KEY_ENTER):
                killEvents(event);
                if (potMode == 1) {
                  SAVE_POT_POSITION(m_posHorz-1);
                  AUDIO_WARNING1();
                  eeDirty(EE_MODEL);
                }
                break;
              case EVT_KEY_BREAK(KEY_ENTER):
                g_model.nPotsToWarn ^= (1 << (m_posHorz-1));
                eeDirty(EE_MODEL);
                break;
            }
          }
        }
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, PSTR("\004""OFF\0""Man\0""Auto"), potMode, attr & ((m_posHorz == 0) ? attr : !INVERS));
        if (potMode) {
          coord_t x = MODEL_SETUP_2ND_COLUMN+5*FW;
          for (uint8_t i=0; i<NUM_POTS ; i++) {
#if !defined(REVPLUS)
            if (i == POT3-POT1) {
              if (attr && (m_posHorz==i+1)) REPEAT_LAST_CURSOR_MOVE();
              continue;
            }
#endif
            LcdFlags flags = ((m_posHorz==i+1) && attr) ? BLINK : 0;
            flags |= (!(g_model.nPotsToWarn & (1 << i))) ? INVERS : 0;
            lcd_putsiAtt(x, y, STR_VSRCRAW, NUM_STICKS+1+i, flags);
            x += (2*FW+3);
          }
        }

        if (attr && (m_posHorz == 0)) {
          CHECK_INCDEC_MODELVAR(event, potMode, 0, 2);
          g_model.nPotsToWarn = (g_model.nPotsToWarn & 0x3F) | ((potMode << 6) & 0xC0);
          eeDirty(EE_MODEL);
        }
        break;
      }
#endif

      case ITEM_MODEL_BEEP_CENTER:
        lcd_putsLeft(y, STR_BEEPCTR);
        for (uint8_t i=0; i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS; i++) {
          // TODO flash saving, \001 not needed in STR_RETA123
          coord_t x = MODEL_SETUP_2ND_COLUMN+i*FW;
#if defined(PCBTARANIS) && !defined(REVPLUS)
          if (i == POT3) {
            if (attr && m_posHorz == POT3) REPEAT_LAST_CURSOR_MOVE();
            continue;
          }
          else if (i > POT3) {
            x -= FW;
          }
#endif
          lcd_putsiAtt(x, y, STR_RETA123, i, ((m_posHorz==i) && attr) ? BLINK|INVERS : (((g_model.beepANACenter & ((BeepANACenter)1<<i)) || (attr && CURSOR_ON_LINE())) ? INVERS : 0 ) );
        }
        if (attr && CURSOR_ON_CELL) {
          if (event==EVT_KEY_BREAK(KEY_ENTER) || p1valdiff) {
            if (READ_ONLY_UNLOCKED()) {
              s_editMode = 0;
              g_model.beepANACenter ^= ((BeepANACenter)1<<m_posHorz);
              eeDirty(EE_MODEL);
            }
          }
        }
        break;

#if defined(CPUARM)
      case ITEM_MODEL_USE_GLOBAL_FUNCTIONS:
        lcd_putsLeft(y, "Use Global Funcs");
        menu_lcd_onoff(MODEL_SETUP_2ND_COLUMN, y, !g_model.noGlobalFunctions, attr);
        if (attr) g_model.noGlobalFunctions = !checkIncDecModel(event, !g_model.noGlobalFunctions, 0, 1);
        break;
#endif

#if defined(PCBSKY9X) && !defined(REVX)
      case ITEM_MODEL_EXTRA_MODULE_LABEL:
        lcd_putsLeft(y, "RF Port 2 (PPM)");
        break;
#endif

#if defined(PCBTARANIS)
      case ITEM_MODEL_INTERNAL_MODULE_LABEL:
        lcd_putsLeft(y, TR_INTERNALRF);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_MODE:
        lcd_putsLeft(y, STR_MODE);
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[0].rfProtocol, attr);
        if (attr) {
          CHECK_INCDEC_MODELVAR(event, g_model.moduleData[0].rfProtocol, RF_PROTO_OFF, RF_PROTO_LAST);
          if (checkIncDec_Ret) {
            g_model.moduleData[0].channelsStart = 0;
            g_model.moduleData[0].channelsCount = 0;
          }
        }
        break;

      case ITEM_MODEL_TRAINER_MODE:
        g_model.trainerMode = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_MODE, STR_VTRAINERMODES, g_model.trainerMode, 0, HAS_WIRELESS_TRAINER_HARDWARE() ? TRAINER_MODE_MASTER_BATTERY_COMPARTMENT : TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE, attr, event);
        break;
#endif

#if defined(CPUARM)
      case ITEM_MODEL_EXTERNAL_MODULE_LABEL:
        lcd_putsLeft(y, TR_EXTERNALRF);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_MODE:
        lcd_putsLeft(y, STR_MODE);
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_TARANIS_PROTOCOLS, g_model.externalModule, m_posHorz==0 ? attr : 0);
        if (IS_MODULE_XJT(EXTERNAL_MODULE))
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[EXTERNAL_MODULE].rfProtocol, m_posHorz==1 ? attr : 0);
        else if (IS_MODULE_DSM2(EXTERNAL_MODULE))
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_DSM_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, m_posHorz==1 ? attr : 0);
        if (attr && (editMode>0 || p1valdiff)) {
          switch (m_posHorz) {
            case 0:
              g_model.externalModule = checkIncDec(event, g_model.externalModule, MODULE_TYPE_NONE, MODULE_TYPE_COUNT-1, EE_MODEL, isModuleAvailable);
              if (checkIncDec_Ret) {
                g_model.moduleData[EXTERNAL_MODULE].rfProtocol = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                if (g_model.externalModule == MODULE_TYPE_PPM)
                  g_model.moduleData[EXTERNAL_MODULE].channelsCount = 0;
                else
                  g_model.moduleData[EXTERNAL_MODULE].channelsCount = MAX_EXTERNAL_MODULE_CHANNELS();
              }
              break;
            case 1:
              if (IS_MODULE_DSM2(EXTERNAL_MODULE))
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, DSM2_PROTO_LP45, DSM2_PROTO_DSMX);
              else
                CHECK_INCDEC_MODELVAR(event, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, RF_PROTO_X16, RF_PROTO_LAST);
              if (checkIncDec_Ret) {
                g_model.moduleData[EXTERNAL_MODULE].channelsStart = 0;
                g_model.moduleData[EXTERNAL_MODULE].channelsCount = MAX_EXTERNAL_MODULE_CHANNELS();
              }
          }
        }
        break;
#endif

#if defined(PCBTARANIS)
      case ITEM_MODEL_TRAINER_LABEL:
        lcd_putsLeft(y, STR_TRAINER);
        break;

      case ITEM_MODEL_INTERNAL_MODULE_CHANNELS:
#elif defined(PCBSKY9X) && !defined(REVX)
      case ITEM_MODEL_EXTRA_MODULE_CHANNELS:
#endif
#if defined(CPUARM)
      case ITEM_MODEL_EXTERNAL_MODULE_CHANNELS:
#if defined(PCBTARANIS)
      case ITEM_MODEL_TRAINER_CHANNELS:
#endif
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcd_putsLeft(y, STR_CHANNELRANGE);
        if ((int8_t)PORT_CHANNELS_ROWS(moduleIdx) >= 0) {
          lcd_putsAtt(MODEL_SETUP_2ND_COLUMN, y, STR_CH, m_posHorz==0 ? attr : 0);
          lcd_outdezAtt(lcdLastPos, y, moduleData.channelsStart+1, LEFT | (m_posHorz==0 ? attr : 0));
          lcd_putc(lcdLastPos, y, '-');
          lcd_outdezAtt(lcdLastPos + FW+1, y, moduleData.channelsStart+NUM_CHANNELS(moduleIdx), LEFT | (m_posHorz==1 ? attr : 0));
          if (attr && (editMode>0 || p1valdiff)) {
            switch (m_posHorz) {
              case 0:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.channelsStart, 32-8-moduleData.channelsCount);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, moduleData.channelsCount, -4, min<int8_t>(MAX_CHANNELS(moduleIdx), 32-8-moduleData.channelsStart));
                if ((k == ITEM_MODEL_EXTERNAL_MODULE_CHANNELS && g_model.externalModule == MODULE_TYPE_PPM)
#if defined(PCBTARANIS)
                    || (k == ITEM_MODEL_TRAINER_CHANNELS)
#endif
                    )
                  SET_DEFAULT_PPM_FRAME_LENGTH(moduleIdx);
                break;
            }
          }
        }
        break;
      }
#endif

#if defined(PCBTARANIS)
      case ITEM_MODEL_INTERNAL_MODULE_BIND:
#elif defined(PCBSKY9X) && !defined(REVX)
      case ITEM_MODEL_EXTRA_MODULE_BIND:
#endif
#if defined(CPUARM)
      case ITEM_MODEL_EXTERNAL_MODULE_BIND:
#if defined(PCBTARANIS)
      case ITEM_MODEL_TRAINER_SETTINGS:
#endif
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        if (IS_MODULE_PPM(moduleIdx)) {
          lcd_putsLeft(y, STR_PPMFRAME);
          lcd_puts(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
          lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppmFrameLength*5 + 225, (m_posHorz<=0 ? attr : 0) | PREC1|LEFT);
          lcd_putc(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
          lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (moduleData.ppmDelay*50)+300, (CURSOR_ON_LINE() || m_posHorz==1) ? attr : 0);
          lcd_putcAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, moduleData.ppmPulsePol ? '+' : '-', (CURSOR_ON_LINE() || m_posHorz==2) ? attr : 0);

          if (attr && (editMode>0 || p1valdiff)) {
            switch (m_posHorz) {
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
          horzpos_t l_posHorz = m_posHorz;
          coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
          if (IS_MODULE_XJT(moduleIdx) && IS_D8_RX(moduleIdx)) {
            xOffsetBind = 0;
            lcd_putsLeft(y, INDENT "Receiver");
            if (attr) l_posHorz += 1;
          }
          else {
            lcd_putsLeft(y, STR_RXNUM);
          }
          if (IS_MODULE_XJT(moduleIdx) || IS_MODULE_DSM2(moduleIdx)) {
            if (xOffsetBind) lcd_outdezNAtt(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId, (l_posHorz==0 ? attr : 0) | LEADING0|LEFT, 2);
            if (attr && l_posHorz==0) {
              if (editMode>0 || p1valdiff) {
                CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId, IS_MODULE_DSM2(moduleIdx) ? 20 : 63);
                if (checkIncDec_Ret) {
                  modelHeaders[g_eeGeneral.currModel].modelId = g_model.header.modelId;
                }
              }
              if (editMode==0 && event==EVT_KEY_BREAK(KEY_ENTER)) {
                checkModelIdUnique(g_eeGeneral.currModel);
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
#endif

#if defined(PCBTARANIS)
      case ITEM_MODEL_INTERNAL_MODULE_FAILSAFE:
#endif
#if defined(CPUARM)
      case ITEM_MODEL_EXTERNAL_MODULE_FAILSAFE:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcd_putsLeft(y, TR_FAILSAFE);
        if (IS_MODULE_XJT(moduleIdx)) {
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_VFAILSAFE, moduleData.failsafeMode, m_posHorz==0 ? attr : 0);
          if (moduleData.failsafeMode == FAILSAFE_CUSTOM) lcd_putsAtt(MODEL_SETUP_2ND_COLUMN + MODEL_SETUP_SET_FAILSAFE_OFS, y, STR_SET, m_posHorz==1 ? attr : 0);
          if (attr) {
            if (moduleData.failsafeMode != FAILSAFE_CUSTOM)
              m_posHorz = 0;
            if (m_posHorz==0) {
              if (editMode>0 || p1valdiff) {
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.failsafeMode, FAILSAFE_LAST);
                if (checkIncDec_Ret) SEND_FAILSAFE_NOW(moduleIdx);
              }
            }
            else if (m_posHorz==1) {
              s_editMode = 0;
              if (moduleData.failsafeMode==FAILSAFE_CUSTOM && event==EVT_KEY_FIRST(KEY_ENTER)) {
                g_moduleIdx = moduleIdx;
                pushMenu(menuModelFailsafe);
              }
            }
            else {
              lcd_filled_rect(MODEL_SETUP_2ND_COLUMN, y, LCD_W-MODEL_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH, 8);
            }
          }
        }
        break;
      }
#endif

#if !defined(CPUARM)
      case ITEM_MODEL_PPM1_PROTOCOL:
        lcd_putsLeft(y, NO_INDENT(STR_PROTO));
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_VPROTOS, protocol, m_posHorz<=0 ? attr : 0);
        if (IS_PPM_PROTOCOL(protocol)) {
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+7*FW, y, STR_NCHANNELS, g_model.ppmNCH+2, m_posHorz!=0 ? attr : 0);
        }
        else if (m_posHorz>0 && attr) {
          MOVE_CURSOR_FROM_HERE();
        }
        if (attr && (editMode>0 || p1valdiff || (!IS_PPM_PROTOCOL(protocol) && !IS_DSM2_PROTOCOL(protocol)))) {
          switch (m_posHorz) {
            case 0:
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.protocol, PROTO_MAX-1);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.ppmNCH, -2, 4);
              g_model.ppmFrameLength = g_model.ppmNCH * 8;
              break;
          }
        }
        break;
#endif

#if 0
      case ITEM_MODEL_PPM2_PROTOCOL:
        lcd_putsLeft(y, PSTR("Port2"));
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_VPROTOS, 0, 0);
        lcd_putsAtt(MODEL_SETUP_2ND_COLUMN+4*FW+3, y, STR_CH, m_posHorz<=0 ? attr : 0);
        lcd_outdezAtt(lcdLastPos, y, g_model.moduleData[1].channelsStart+1, LEFT | (m_posHorz<=0 ? attr : 0));
        lcd_putc(lcdLastPos, y, '-');
        lcd_outdezAtt(lcdLastPos + FW+1, y, g_model.moduleData[1].channelsStart+8+g_model.moduleData[1].channelsCount, LEFT | (m_posHorz!=0 ? attr : 0));
        if (attr && (editMode>0 || p1valdiff)) {
          switch (m_posHorz) {
            case 0:
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.moduleData[1].channelsStart, 32-8-g_model.moduleData[1].channelsCount);
              SET_DEFAULT_PPM_FRAME_LENGTH(1);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[1].channelsCount, -4, min<int8_t>(8, 32-8-g_model.moduleData[1].channelsStart));
              SET_DEFAULT_PPM_FRAME_LENGTH(1);
              break;
          }
        }
        break;

      case ITEM_MODEL_PPM2_PARAMS:
        lcd_putsLeft(y, STR_PPMFRAME);
        lcd_puts(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
        lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN, y, (int16_t)g_model.moduleData[1].ppmFrameLength*5 + 225, (m_posHorz<=0 ? attr : 0) | PREC1|LEFT);
        lcd_putc(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
        lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (g_model.moduleData[1].ppmDelay*50)+300, (m_posHorz < 0 || m_posHorz==1) ? attr : 0);
        lcd_putcAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, g_model.moduleData[1].ppmPulsePol ? '+' : '-', (m_posHorz < 0 || m_posHorz==2) ? attr : 0);
        if (attr && (editMode>0 || p1valdiff)) {
          switch (m_posHorz) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[1].ppmFrameLength, -20, 35);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.moduleData[1].ppmDelay, -4, 10);
              break;
            case 2:
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.moduleData[1].ppmPulsePol, 1);
              break;
          }
        }
        break;
#endif

#if !defined(CPUARM)
      case ITEM_MODEL_PPM1_PARAMS:
        if (IS_PPM_PROTOCOL(protocol)) {
          lcd_putsLeft(y, STR_PPMFRAME);
          lcd_puts(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
          lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN, y, (int16_t)g_model.ppmFrameLength*5 + 225, (m_posHorz<=0 ? attr : 0) | PREC1|LEFT);
          lcd_putc(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
          lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (g_model.ppmDelay*50)+300, (CURSOR_ON_LINE() || m_posHorz==1) ? attr : 0);
          lcd_putcAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, g_model.pulsePol ? '+' : '-', (CURSOR_ON_LINE() || m_posHorz==2) ? attr : 0);
          if (attr && (editMode>0 || p1valdiff)) {
            switch (m_posHorz) {
              case 0:
                CHECK_INCDEC_MODELVAR(event, g_model.ppmFrameLength, -20, 35);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, g_model.ppmDelay, -4, 10);
                break;
              case 2:
                CHECK_INCDEC_MODELVAR_ZERO(event, g_model.pulsePol, 1);
                break;
            }
          }
        }
#if defined(DSM2) || defined(PXX)
        else if (IS_DSM2_PROTOCOL(protocol) || IS_PXX_PROTOCOL(protocol)) {
          if (attr && m_posHorz > 1) {
            REPEAT_LAST_CURSOR_MOVE(); // limit 3 column row to 2 colums (Rx_Num and RANGE fields)
          }

          lcd_putsLeft(y, STR_RXNUM);
          lcd_outdezNAtt(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId, (m_posHorz<=0 ? attr : 0) | LEADING0|LEFT, 2);
          if (attr && (m_posHorz==0 && (editMode>0 || p1valdiff))) {
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId, 99);
          }
#if defined(PXX)
          if (protocol == PROTO_PXX) {
            lcd_putsAtt(MODEL_SETUP_2ND_COLUMN+4*FW, y, STR_SYNCMENU, m_posHorz!=0 ? attr : 0);
            uint8_t newFlag = 0;
            if (attr && m_posHorz>0 && editMode>0) {
              // send reset code
              newFlag = MODULE_BIND;
            }
            moduleFlag[0] = newFlag;
          }
#endif
#if defined(DSM2)
          if (IS_DSM2_PROTOCOL(protocol)) {
            lcd_putsAtt(MODEL_SETUP_2ND_COLUMN+4*FW, y, STR_MODULE_RANGE, m_posHorz!=0 ? attr : 0);
            moduleFlag[0] = (attr && m_posHorz>0 && editMode>0) ? MODULE_RANGECHECK : 0; // [MENU] key toggles range check mode
          }
#endif
        }
#endif
        break;
#endif
    }
  }

#if defined(CPUARM) && defined(PXX)
  if (IS_RANGECHECK_ENABLE()) {
    displayPopup("RSSI: ");
    lcd_outdezAtt(16+4*FW, 5*FH, TELEMETRY_RSSI(), BOLD);
  }
#endif
}

#if defined(CPUARM)
void menuModelFailsafe(uint8_t event)
{
  static bool longNames = false;
  bool newLongNames = false;
  uint8_t ch = 0;

  if (event == EVT_KEY_LONG(KEY_ENTER) && s_editMode) {
    s_noHi = NO_HI_LEN;
    g_model.moduleData[g_moduleIdx].failsafeChannels[m_posVert] = channelOutputs[m_posVert];
    eeDirty(EE_MODEL);
    AUDIO_WARNING1();
    SEND_FAILSAFE_NOW(g_moduleIdx);
  }

  SIMPLE_SUBMENU_NOTITLE(NUM_CHNOUT);

  SET_SCROLLBAR_X(0);

#if LCD_W >= 212
  #define COL_W   (LCD_W/2)
  const uint8_t SLIDER_W = 64;
  // Column separator
  lcd_vline(LCD_W/2, FH, LCD_H-FH);

  if (m_posVert >= 16) {
    ch = 16;
  }
#else
  #define COL_W   (LCD_W)
  const uint8_t SLIDER_W = 90;
  ch = 8 * (m_posVert / 8);
#endif

  lcd_putsCenter(0*FH, FAILSAFESET);
  lcd_invert_line(0);

#if LCD_W >= 212
  for (uint8_t col=0; col<2; col++)
#else
  uint8_t col = 0;
#endif
  {
    coord_t x = col*COL_W+1;

    // Channels
    for (uint8_t line=0; line<8; line++) {
      coord_t y = 9+line*7;
      int32_t val;
      uint8_t ofs = (col ? 0 : 1);

      if (ch < g_model.moduleData[g_moduleIdx].channelsStart || ch >= NUM_CHANNELS(g_moduleIdx) + g_model.moduleData[g_moduleIdx].channelsStart)
        val = 0;
      else if (s_editMode && m_posVert == ch)
        val = channelOutputs[ch];
      else
        val = g_model.moduleData[g_moduleIdx].failsafeChannels[8*col+line];

#if defined(PCBTARANIS)
      // Channel name if present, number if not
      uint8_t lenLabel = ZLEN(g_model.limitData[ch].name);
      if (lenLabel > 4) {
        newLongNames = longNames = true;
      }

      if (lenLabel > 0)
        lcd_putsnAtt(x+1-ofs, y, g_model.limitData[ch].name, sizeof(g_model.limitData[ch].name), ZCHAR | SMLSIZE);
      else
        putsChn(x+1-ofs, y, ch+1, SMLSIZE);
#else
      putsChn(x+1-ofs, y, ch+1, SMLSIZE);
#endif

      // Value
      LcdFlags flags = TINSIZE;
      if (m_posVert == ch && !s_noHi) {
        flags |= INVERS;
        if (s_editMode)
          flags |= BLINK;
      }
#if defined(PPM_UNIT_US)
      uint8_t wbar = (longNames ? SLIDER_W-10 : SLIDER_W);
      lcd_outdezAtt(x+COL_W-4-wbar-ofs, y, PPM_CH_CENTER(ch)+val/2, flags);
#elif defined(PPM_UNIT_PERCENT_PREC1)
      uint8_t wbar = (longNames ? SLIDER_W-16 : SLIDER_W-6);
      lcd_outdezAtt(x+COL_W-4-wbar-ofs, y, calcRESXto1000(val), PREC1|flags);
#else
      uint8_t wbar = (longNames ? SLIDER_W-10 : SLIDER_W);
      lcd_outdezAtt(x+COL_W-4-wbar-ofs, y, calcRESXto1000(val)/10, flags);
#endif

      // Gauge
      lcd_rect(x+COL_W-3-wbar-ofs, y, wbar+1, 6);
      uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
      uint8_t len = limit((uint8_t)1, uint8_t((abs(val) * wbar/2 + lim/2) / lim), uint8_t(wbar/2));
      coord_t x0 = (val>0) ? x+COL_W-ofs-3-wbar/2 : x+COL_W-ofs-2-wbar/2-len;
      lcd_hline(x0, y+1, len);
      lcd_hline(x0, y+2, len);
      lcd_hline(x0, y+3, len);
      lcd_hline(x0, y+4, len);

      ch++;
    }
  }

  longNames = newLongNames;
}
#endif
