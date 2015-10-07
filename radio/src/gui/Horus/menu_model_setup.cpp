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
void menuModelFailsafe(evt_t event);

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
  // ITEM_MODEL_THROTTLE_REVERSED,
  ITEM_MODEL_THROTTLE_TRACE,
  ITEM_MODEL_THROTTLE_TRIM,
  ITEM_MODEL_PREFLIGHT_LABEL,
  ITEM_MODEL_CHECKLIST_DISPLAY,
  ITEM_MODEL_THROTTLE_WARNING,
  ITEM_MODEL_SWITCHES_WARNING,
  ITEM_MODEL_POT_WARNING,
  ITEM_MODEL_BEEP_CENTER,
  ITEM_MODEL_USE_GLOBAL_FUNCTIONS,
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

#define MODEL_SETUP_2ND_COLUMN        200
#define MODEL_SETUP_BIND_OFS          20
#define MODEL_SETUP_RANGE_OFS         40
#define MODEL_SETUP_SET_FAILSAFE_OFS  60

void copySelection(char *dst, const char *src, uint8_t size)
{
  if (memcmp(src, "---", 3) == 0)
    memset(dst, 0, size);
  else
    memcpy(dst, src, size);
}

void editTimerMode(int timerIdx, coord_t y, LcdFlags attr, evt_t event)
{
  TimerData * timer = &g_model.timers[timerIdx];
  if (attr && m_posHorz < 0) {
	lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN, 80+2*INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
  }
  putsStrIdx(MENU_TITLE_LEFT, y, STR_TIMER, timerIdx+1);
  putsTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer->mode, (m_posHorz<=0 ? attr : 0));
  putsTimer(MODEL_SETUP_2ND_COLUMN+50, y, timer->start, (m_posHorz!=0 ? attr : 0));
  if (attr && s_editMode>0) {
    switch (m_posHorz) {
      case 0:
      {
        int8_t timerMode = timer->mode;
        if (timerMode < 0) timerMode -= TMRMODE_COUNT-1;
        CHECK_INCDEC_MODELVAR_CHECK(event, timerMode, -TMRMODE_COUNT-SWSRC_LAST+1, TMRMODE_COUNT+SWSRC_LAST-1, isSwitchAvailableInTimers);
        if (timerMode < 0) timerMode += TMRMODE_COUNT-1;
        timer->mode = timerMode;
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
      }
      case 1:
      {
        const int stopsMinutes[] = { 8, 60, 120, 180, 240, 300, 600, 900, 1200 };
        timer->start = checkIncDec(event, timer->start, 0, 60*60, EE_MODEL, NULL, (const CheckIncDecStops&)stopsMinutes);
        break;
      }
    }
  }
}

#define CURRENT_MODULE_EDITED(k)            (k>=ITEM_MODEL_TRAINER_LABEL ? TRAINER_MODULE : EXTERNAL_MODULE)

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

void menuModelSetup(evt_t event)
{
  horzpos_t l_posHorz = m_posHorz;
  #define IF_EXTERNAL_MODULE_ON(x)          (g_model.externalModule == MODULE_TYPE_NONE ? HIDDEN_ROW : (uint8_t)(x))
  #define IF_TRAINER_ON(x)                  (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)(x) : HIDDEN_ROW)
  #define IF_EXTERNAL_MODULE_XJT(x)         (IS_MODULE_XJT(EXTERNAL_MODULE) ? (uint8_t)x : HIDDEN_ROW)
  #define IS_D8_RX(x)                       (g_model.moduleData[x].rfProtocol == RF_PROTO_D8)
  #define EXTERNAL_MODULE_CHANNELS_ROWS()   IF_EXTERNAL_MODULE_ON(IS_MODULE_DSM2(EXTERNAL_MODULE) ? (uint8_t)0 : (uint8_t)1)
  #define TRAINER_CHANNELS_ROWS()           IF_TRAINER_ON(1)
  #define PORT_CHANNELS_ROWS(x)             (x==EXTERNAL_MODULE ? EXTERNAL_MODULE_CHANNELS_ROWS() : TRAINER_CHANNELS_ROWS())
  #define FAILSAFE_ROWS(x)                  ((g_model.moduleData[x].rfProtocol==RF_PROTO_X16 || g_model.moduleData[x].rfProtocol==RF_PROTO_LR12) ? (g_model.moduleData[x].failsafeMode==FAILSAFE_CUSTOM ? (uint8_t)1 : (uint8_t)0) : HIDDEN_ROW)
  #define POT_WARN_ITEMS()                  ((g_model.potsWarnMode >> 6) ? (uint8_t)NUM_POTS : (uint8_t)0)
  #define TIMER_ROWS                        NAVIGATION_LINE_BY_LINE|1, 0, CASE_PERSISTENT_TIMERS(0) 0, 0
  #if TIMERS == 1
    #define TIMERS_ROWS TIMER_ROWS
  #elif TIMERS == 2
    #define TIMERS_ROWS TIMER_ROWS, TIMER_ROWS
  #elif TIMERS == 3
    #define TIMERS_ROWS TIMER_ROWS, TIMER_ROWS, TIMER_ROWS
  #endif
  bool CURSOR_ON_CELL = (m_posHorz >= 0);

  MENU(STR_MENUSETUP, menuTabModel, e_ModelSetup, ITEM_MODEL_SETUP_MAX, DEFAULT_SCROLLBAR_X, { 0, 0, TIMERS_ROWS, 0, 1, 0, 0, LABEL(Throttle), 0, 0, LABEL(PreflightCheck), 0, 0, uint8_t(NAVIGATION_LINE_BY_LINE|(getSwitchWarningsCount()-1)), POT_WARN_ITEMS(), NAVIGATION_LINE_BY_LINE|(NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1), 0, LABEL(ExternalModule), (IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE)) ? (uint8_t)1 : (uint8_t)0, EXTERNAL_MODULE_CHANNELS_ROWS(), (IS_MODULE_XJT(EXTERNAL_MODULE) && IS_D8_RX(EXTERNAL_MODULE)) ? (uint8_t)1 : (IS_MODULE_PPM(EXTERNAL_MODULE) || IS_MODULE_XJT(EXTERNAL_MODULE) || IS_MODULE_DSM2(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW, IF_EXTERNAL_MODULE_XJT(FAILSAFE_ROWS(EXTERNAL_MODULE)), LABEL(Trainer), 0, TRAINER_CHANNELS_ROWS(), IF_TRAINER_ON(2) });

#if (defined(DSM2) || defined(PXX))
  if (menuEvent) {
    moduleFlag[0] = 0;
#if NUM_MODULES > 1
    moduleFlag[1] = 0;
#endif
  }
#endif

  int sub = m_posVert;

  for (int i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    int k = i+s_pgOfs;
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
        lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN, y, g_model.header.bitmap, LEFT|attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.bitmap, 3);
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
        g_model.timers[0].countdownBeep = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, STR_VBEEPCOUNTDOWN, g_model.timers[0].countdownBeep, 0, 2, attr, event);
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
        g_model.timers[1].countdownBeep = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, STR_VBEEPCOUNTDOWN, g_model.timers[1].countdownBeep, 0, 2, attr, event);
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
        g_model.timers[2].countdownBeep = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, STR_VBEEPCOUNTDOWN, g_model.timers[2].countdownBeep, 0, 2, attr, event);
        break;

      case ITEM_MODEL_TIMER3_PERSISTENT:
        g_model.timers[2].persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, g_model.timers[2].persistent, 0, 2, attr, event);
        break;
#endif

      case ITEM_MODEL_EXTENDED_LIMITS:
        ON_OFF_MENU_ITEM(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, STR_ELIMITS, attr, event);
        break;

      case ITEM_MODEL_EXTENDED_TRIMS:
        ON_OFF_MENU_ITEM(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, STR_ETRIMS, m_posHorz<=0 ? attr : 0, event==EVT_KEY_BREAK(KEY_ENTER) ? event : 0);
        lcd_putsAtt(MODEL_SETUP_2ND_COLUMN+18, y, STR_RESET_BTN, m_posHorz>0  && !s_noHi ? attr : 0);
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
        break;

      case ITEM_MODEL_DISPLAY_TRIMS:
        g_model.displayTrims = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, "Display Trims", "\006No\0   ChangeYes", g_model.displayTrims, 0, 2, attr, event);
        break;

      case ITEM_MODEL_TRIM_INC:
        g_model.trimInc = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_TRIMINC, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_LABEL:
        lcd_putsLeft(y, STR_THROTTLE_LABEL);
        break;

#if 0
      case ITEM_MODEL_THROTTLE_REVERSED:
        ON_OFF_MENU_ITEM(g_model.throttleReversed, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEREVERSE, attr, event ) ;
        break;
#endif

      case ITEM_MODEL_THROTTLE_TRACE:
      {
        lcd_putsLeft(y, STR_TTRACE);
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

      case ITEM_MODEL_SWITCHES_WARNING:
      {
        lcd_putsLeft(y, STR_SWITCHWARNING);
        if (!READ_ONLY() && attr && m_posHorz<0 && event==EVT_KEY_LONG(KEY_ENTER)) {
          killEvents(event);
          s_noHi = NO_HI_LEN;
          getMovedSwitch();
          g_model.switchWarningState = switches_states;
          AUDIO_WARNING1();
          eeDirty(EE_MODEL);
        }

        if (attr && m_posHorz < 0) {
          lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN, NUM_SWITCHES*18+INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }

        unsigned int newStates = 0;
        for (int i=0, current=0; i<NUM_SWITCHES; i++) {
          if (SWITCH_WARNING_ALLOWED(i)) {
            if (!READ_ONLY() && attr && l_posHorz==current) {
              // g_model.switchWarningEnable ^= (1 << i);
              eeDirty(EE_MODEL);
            }
            unsigned int state = ((g_model.switchWarningState >> (3*i)) & 0x07);
            LcdFlags color = (state > 0 ? TEXT_COLOR : TEXT_DISABLE_COLOR);
            if (attr && m_posHorz < 0) {
              color |= INVERS;
            }
            char s[3];
            s[0] = 'A' + i + (i >= 3 ? 1 : 0 /* skip SD which is a POT*/);
            int max;
            if (state == 0) {
              s[1] = 'x';
              max = 2;
            }
            else if (i == 2) {
              s[1] = '0' + state;
              max = 6;
            }
            else if (IS_3POS(i)) {
              s[1] = "\300-\301"[state-1];
              max = 3;
            }
            else {
              s[1] = "\300\301"[state-1];
              max = 2;
            }
            s[2] = '\0';
            lcd_putsAtt(MODEL_SETUP_2ND_COLUMN+i*18, y, s, color|(m_posHorz==current ? attr : 0));
            if (attr && m_posHorz==current) CHECK_INCDEC_MODELVAR_ZERO(event, state, max);
            newStates |= (state << (3*i));
            ++current;
          }
        }
        g_model.switchWarningState = newStates;
        break;
      }

      case ITEM_MODEL_POT_WARNING:
      {
        lcd_putsLeft(y, STR_POTWARNING);
        if (attr) {
          if (m_posHorz) s_editMode = 0;
          if (!READ_ONLY() && m_posHorz) {
            switch (event) {
              case EVT_KEY_LONG(KEY_ENTER):
                killEvents(event);
                if (g_model.potsWarnMode == POTS_WARN_MANUAL) {
                  SAVE_POT_POSITION(m_posHorz-1);
                  AUDIO_WARNING1();
                  eeDirty(EE_MODEL);
                }
                break;
              case EVT_KEY_BREAK(KEY_ENTER):
                g_model.potsWarnEnabled ^= (1 << (m_posHorz-1));
                eeDirty(EE_MODEL);
                break;
            }
          }
        }
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, PSTR("\004""OFF\0""Man\0""Auto"), g_model.potsWarnMode, (m_posHorz == 0) ? attr : 0);
        if (g_model.potsWarnMode) {
          coord_t x = MODEL_SETUP_2ND_COLUMN+30;
          for (int i=0; i<NUM_POTS; ++i) {
            LcdFlags flags = (((m_posHorz==i+1) && attr) ? INVERS : 0);
            flags |= (g_model.potsWarnEnabled & (1 << i)) ? TEXT_DISABLE_COLOR : TEXT_COLOR;
            lcd_putsiAtt(x, y, STR_VSRCRAW, NUM_STICKS+1+i, flags);
            x += 20;
          }
        }

        if (attr && (m_posHorz == 0)) {
          CHECK_INCDEC_MODELVAR(event, g_model.potsWarnMode, POTS_WARN_OFF, POTS_WARN_AUTO);
          eeDirty(EE_MODEL);
        }
        break;
      }

      case ITEM_MODEL_BEEP_CENTER:
        lcd_putsLeft(y, STR_BEEPCTR);
        if (attr && m_posHorz < 0) {
          lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN, (NUM_STICKS+NUM_POTS)*11+2*INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }
        for (int i=0; i<NUM_STICKS+NUM_POTS; i++) {
          // TODO flash saving, \001 not needed in STR_RETA123
          coord_t x = MODEL_SETUP_2ND_COLUMN+i*11;
          LcdFlags flags = ((m_posHorz==i && attr) ? INVERS : 0);
          flags |= (g_model.beepANACenter & ((BeepANACenter)1<<i)) ? TEXT_COLOR : TEXT_DISABLE_COLOR;
          if (attr && m_posHorz < 0) flags |= INVERS;
          lcd_putsiAtt(x, y, STR_RETA123, i, flags);
        }
        if (attr && CURSOR_ON_CELL) {
          if (event==EVT_KEY_BREAK(KEY_ENTER)) {
            if (READ_ONLY_UNLOCKED()) {
              s_editMode = 0;
              g_model.beepANACenter ^= ((BeepANACenter)1<<m_posHorz);
              eeDirty(EE_MODEL);
            }
          }
        }
        break;

      case ITEM_MODEL_USE_GLOBAL_FUNCTIONS:
        lcd_putsLeft(y, "Use Global Funcs");
        lcdDrawCheckBox(MODEL_SETUP_2ND_COLUMN, y, !g_model.noGlobalFunctions, attr);
        if (attr) g_model.noGlobalFunctions = !checkIncDecModel(event, !g_model.noGlobalFunctions, 0, 1);
        break;

      case ITEM_MODEL_TRAINER_MODE:
        g_model.trainerMode = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_MODE, STR_VTRAINERMODES, g_model.trainerMode, TRAINER_MODE_MASTER, TRAINER_MODE_SLAVE, attr, event);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_LABEL:
        lcd_putsLeft(y, TR_EXTERNALRF);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_MODE:
        lcd_putsLeft(y, STR_MODE);
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_TARANIS_PROTOCOLS, g_model.externalModule, (m_posHorz==0 ? attr : 0));
        if (IS_MODULE_XJT(EXTERNAL_MODULE))
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+40, y, STR_XJT_PROTOCOLS, 1+g_model.moduleData[EXTERNAL_MODULE].rfProtocol, (m_posHorz==1 ? attr : 0));
        else if (IS_MODULE_DSM2(EXTERNAL_MODULE))
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+40, y, STR_DSM_PROTOCOLS, g_model.moduleData[EXTERNAL_MODULE].rfProtocol, (m_posHorz==1 ? attr : 0));
        if (attr && s_editMode>0) {
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

      case ITEM_MODEL_TRAINER_LABEL:
        lcd_putsLeft(y, STR_TRAINER);
        break;

      case ITEM_MODEL_EXTERNAL_MODULE_CHANNELS:
      case ITEM_MODEL_TRAINER_CHANNELS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        lcd_putsLeft(y, STR_CHANNELRANGE);
        if ((int8_t)PORT_CHANNELS_ROWS(moduleIdx) >= 0) {
          putsStrIdx(MODEL_SETUP_2ND_COLUMN, y, STR_CH, moduleData.channelsStart+1, m_posHorz==0 ? attr : 0);
          lcd_puts(MODEL_SETUP_2ND_COLUMN+30, y, "-");
          putsStrIdx(MODEL_SETUP_2ND_COLUMN+40, y, STR_CH, moduleData.channelsStart+NUM_CHANNELS(moduleIdx), m_posHorz==1 ? attr : 0);
          if (attr && s_editMode>0) {
            switch (m_posHorz) {
              case 0:
                CHECK_INCDEC_MODELVAR_ZERO(event, moduleData.channelsStart, 32-8-moduleData.channelsCount);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, moduleData.channelsCount, -4, min<int8_t>(MAX_CHANNELS(moduleIdx), 32-8-moduleData.channelsStart));
                if ((k == ITEM_MODEL_EXTERNAL_MODULE_CHANNELS && g_model.externalModule == MODULE_TYPE_PPM)
                    || (k == ITEM_MODEL_TRAINER_CHANNELS)
                    )
                  SET_DEFAULT_PPM_FRAME_LENGTH(moduleIdx);
                break;
            }
          }
        }
        break;
      }

      case ITEM_MODEL_EXTERNAL_MODULE_BIND:
      case ITEM_MODEL_TRAINER_SETTINGS:
      {
        uint8_t moduleIdx = CURRENT_MODULE_EDITED(k);
        ModuleData & moduleData = g_model.moduleData[moduleIdx];
        if (IS_MODULE_PPM(moduleIdx)) {
          lcd_putsLeft(y, STR_PPMFRAME);
          lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN, y, (int16_t)moduleData.ppmFrameLength*5 + 225, (m_posHorz<=0 ? attr : 0) | PREC1|LEFT, STR_MS);
          lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN+70, y, (moduleData.ppmDelay*50)+300, (CURSOR_ON_LINE() || m_posHorz==1) ? attr : 0, "us");
          lcd_putsAtt(MODEL_SETUP_2ND_COLUMN+90, y, moduleData.ppmPulsePol ? "+" : "-", (CURSOR_ON_LINE() || m_posHorz==2) ? attr : 0);

          if (attr && s_editMode>0) {
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
            if (xOffsetBind) lcd_outdezNAtt(MODEL_SETUP_2ND_COLUMN, y, g_model.header.modelId[moduleIdx], (l_posHorz==0 ? attr : 0) | LEADING0|LEFT, 2);
            if (attr && l_posHorz==0) {
              if (s_editMode>0) {
                CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[moduleIdx], IS_MODULE_DSM2(moduleIdx) ? 20 : 63);
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
              if (s_editMode>0) {
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
              lcdDrawFilledRect(MODEL_SETUP_2ND_COLUMN, y, LCD_W-MODEL_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH, 8, TEXT_COLOR);
            }
          }
        }
        break;
      }
    }
  }

  if (IS_RANGECHECK_ENABLE()) {
    displayPopup("RSSI :");
    lcd_outdezAtt(WARNING_LINE_X, WARNING_INFOLINE_Y, TELEMETRY_RSSI(), DBLSIZE|LEFT);
  }
}

void menuModelFailsafe(evt_t event)
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

  SIMPLE_SUBMENU_NOTITLE(NUM_CHNOUT, 0);

  #define COL_W   (LCD_W/2)
  const uint8_t SLIDER_W = 64;
  // Column separator
  lcd_vline(LCD_W/2, FH, LCD_H-FH);

  if (m_posVert >= 16) {
    ch = 16;
  }

  // TODO lcd_putsCenter(0, FAILSAFESET);
  // TODO lcd_invert_line(0);

  for (int col=0; col<2; col++) {
    coord_t x = col*COL_W+1;

    // Channels
    for (int line=0; line<8; line++) {
      coord_t y = 9+line*7;
      int32_t val;
      int ofs = (col ? 0 : 1);

      if (ch < g_model.moduleData[g_moduleIdx].channelsStart || ch >= NUM_CHANNELS(g_moduleIdx) + g_model.moduleData[g_moduleIdx].channelsStart)
        val = 0;
      else if (s_editMode && m_posVert == ch)
        val = channelOutputs[ch];
      else
        val = g_model.moduleData[g_moduleIdx].failsafeChannels[8*col+line];

      // Channel name if present, number if not
      uint8_t lenLabel = ZLEN(g_model.limitData[ch].name);
      if (lenLabel > 4) {
        newLongNames = longNames = true;
      }

      if (lenLabel > 0)
        lcd_putsnAtt(x+1-ofs, y, g_model.limitData[ch].name, sizeof(g_model.limitData[ch].name), ZCHAR | SMLSIZE);
      else
        putsChn(x+1-ofs, y, ch+1, SMLSIZE);

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
