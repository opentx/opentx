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
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
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

const pm_uchar sticks[] PROGMEM = {
#include "../bitmaps/sticks.lbm"
};

#if defined(CPUARM)
extern LP_CONST LanguagePack czLanguagePack;
extern LP_CONST LanguagePack enLanguagePack;
extern LP_CONST LanguagePack esLanguagePack;
extern LP_CONST LanguagePack frLanguagePack;
extern LP_CONST LanguagePack deLanguagePack;
extern LP_CONST LanguagePack itLanguagePack;
extern LP_CONST LanguagePack plLanguagePack;
extern LP_CONST LanguagePack ptLanguagePack;
extern LP_CONST LanguagePack skLanguagePack;
extern LP_CONST LanguagePack seLanguagePack;
extern LP_CONST LanguagePack huLanguagePack;

const LanguagePack * LP_CONST languagePacks[] = {
  // alphabetical order
  &czLanguagePack,
  &deLanguagePack,
  &enLanguagePack,
  &esLanguagePack,
  &frLanguagePack,
  &huLanguagePack,
  &itLanguagePack,
  &plLanguagePack,
  &ptLanguagePack,
  &seLanguagePack,
  &skLanguagePack,
  NULL
};
#endif

enum EnumTabDiag {
  e_Setup,
  CASE_SDCARD(e_Sd)
  CASE_CPUARM(e_GeneralCustomFunctions)
  e_Trainer,
  e_Vers,
  e_Keys,
  e_Ana,
  CASE_CPUARM(e_Hardware)
  e_Calib
};

void menuGeneralSetup(uint8_t event);
void menuGeneralSdManager(uint8_t event);
void menuGeneralCustomFunctions(uint8_t event);
void menuGeneralTrainer(uint8_t event);
void menuGeneralVersion(uint8_t event);
void menuGeneralDiagKeys(uint8_t event);
void menuGeneralDiagAna(uint8_t event);
void menuGeneralHardware(uint8_t event);
void menuGeneralCalib(uint8_t event);

const MenuFuncP_PROGMEM menuTabGeneral[] PROGMEM = {
  menuGeneralSetup,
  CASE_SDCARD(menuGeneralSdManager)
  CASE_CPUARM(menuGeneralCustomFunctions)
  menuGeneralTrainer,
  menuGeneralVersion,
  menuGeneralDiagKeys,
  menuGeneralDiagAna,
  CASE_CPUARM(menuGeneralHardware)
  menuGeneralCalib
};

#if LCD_W >= 212
  #define RADIO_SETUP_2ND_COLUMN  (LCD_W-10*FW-MENUS_SCROLLBAR_WIDTH)
  #define RADIO_SETUP_DATE_COLUMN RADIO_SETUP_2ND_COLUMN + 4*FWNUM
  #define RADIO_SETUP_TIME_COLUMN RADIO_SETUP_2ND_COLUMN + 2*FWNUM
#else
  #define RADIO_SETUP_2ND_COLUMN  (LCD_W-6*FW-3-MENUS_SCROLLBAR_WIDTH)
  #define RADIO_SETUP_TIME_COLUMN (FW*15+9)
  #define RADIO_SETUP_DATE_COLUMN (FW*15+7)
#endif

#if !defined(CPUM64)
  #define SLIDER_5POS(y, value, label, event, attr) { \
    int8_t tmp = value; \
    displaySlider(RADIO_SETUP_2ND_COLUMN, y, 2+tmp, 4, attr); \
    value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, NULL, tmp, -2, +2, attr, event); \
  }
#elif defined(GRAPHICS)
  #define SLIDER_5POS(y, value, label, event, attr) { \
    int8_t tmp = value; \
    display5posSlider(RADIO_SETUP_2ND_COLUMN, y, tmp, attr); \
    value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, NULL, tmp, -2, +2, attr, event); \
  }
#else
  #define SLIDER_5POS(y, value, label, event, attr) value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, STR_VBEEPLEN, value, -2, +2, attr, event)
#endif

#if defined(SPLASH) && !defined(FSPLASH)
  #define CASE_SPLASH_PARAM(x) x,
#else
  #define CASE_SPLASH_PARAM(x)
#endif

enum menuGeneralSetupItems {
  CASE_RTCLOCK(ITEM_SETUP_DATE)
  CASE_RTCLOCK(ITEM_SETUP_TIME)
  CASE_BATTGRAPH(ITEM_SETUP_BATT_RANGE)
  ITEM_SETUP_SOUND_LABEL,
  CASE_AUDIO(ITEM_SETUP_BEEP_MODE)
  CASE_BUZZER(ITEM_SETUP_BUZZER_MODE)
  CASE_VOICE(ITEM_SETUP_SPEAKER_VOLUME)
  CASE_CPUARM(ITEM_SETUP_BEEP_VOLUME)
  ITEM_SETUP_BEEP_LENGTH,
  CASE_AUDIO(ITEM_SETUP_SPEAKER_PITCH)
  CASE_CPUARM(ITEM_SETUP_WAV_VOLUME)
  CASE_CPUARM(ITEM_SETUP_BACKGROUND_VOLUME)
  CASE_VARIO_CPUARM(ITEM_SETUP_VARIO_LABEL)
  CASE_VARIO_CPUARM(ITEM_SETUP_VARIO_VOLUME)
  CASE_VARIO_CPUARM(ITEM_SETUP_VARIO_PITCH)
  CASE_VARIO_CPUARM(ITEM_SETUP_VARIO_RANGE)
  CASE_VARIO_CPUARM(ITEM_SETUP_VARIO_REPEAT)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_LABEL)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_MODE)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_LENGTH)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_STRENGTH)
  ITEM_SETUP_CONTRAST,
  ITEM_SETUP_ALARMS_LABEL,
  ITEM_SETUP_BATTERY_WARNING,
  CASE_PCBSKY9X(ITEM_SETUP_CAPACITY_WARNING)
  CASE_PCBSKY9X(ITEM_SETUP_TEMPERATURE_WARNING)
  ITEM_SETUP_INACTIVITY_ALARM,
  ITEM_SETUP_MEMORY_WARNING,
  ITEM_SETUP_ALARM_WARNING,
  IF_ROTARY_ENCODERS(ITEM_SETUP_RE_NAVIGATION)
  ITEM_SETUP_BACKLIGHT_LABEL,
  ITEM_SETUP_BACKLIGHT_MODE,
  ITEM_SETUP_BACKLIGHT_DELAY,
  CASE_CPUARM(ITEM_SETUP_BRIGHTNESS)
  CASE_REVPLUS(ITEM_SETUP_BACKLIGHT_COLOR)
  CASE_PWM_BACKLIGHT(ITEM_SETUP_BACKLIGHT_BRIGHTNESS_OFF)
  CASE_PWM_BACKLIGHT(ITEM_SETUP_BACKLIGHT_BRIGHTNESS_ON)
  ITEM_SETUP_FLASH_BEEP,
  CASE_SPLASH_PARAM(ITEM_SETUP_DISABLE_SPLASH)
  CASE_GPS(ITEM_SETUP_TIMEZONE)
  CASE_GPS(ITEM_SETUP_GPSFORMAT)
  CASE_PXX(ITEM_SETUP_COUNTRYCODE)
  CASE_CPUARM(ITEM_SETUP_LANGUAGE)
  CASE_CPUARM(ITEM_SETUP_IMPERIAL)
  IF_FAI_CHOICE(ITEM_SETUP_FAI)
  CASE_MAVLINK(ITEM_MAVLINK_BAUD)
  CASE_CPUARM(ITEM_SETUP_SWITCHES_DELAY)
  ITEM_SETUP_RX_CHANNEL_ORD,
  ITEM_SETUP_STICK_MODE_LABELS,
  ITEM_SETUP_STICK_MODE,
  ITEM_SETUP_MAX
};

#if defined(FRSKY_STICKS)
  #define COL_TX_MODE 0
#else
  #define COL_TX_MODE LABEL(TX_MODE)
#endif

void menuGeneralSetup(uint8_t event)
{
#if defined(RTCLOCK)
  struct gtm t;
  gettime(&t);

  if ((m_posVert==ITEM_SETUP_DATE+1 || m_posVert==ITEM_SETUP_TIME+1) &&
      (s_editMode>0) &&
      (event==EVT_KEY_FIRST(KEY_ENTER) || event==EVT_KEY_FIRST(KEY_EXIT) || IS_ROTARY_BREAK(event) || IS_ROTARY_LONG(event))) {
    // set the date and time into RTC chip
    rtcSetTime(&t);
  }
#endif

#if defined(FAI_CHOICE)
  if (s_warning_result) {
    s_warning_result = 0;
    g_eeGeneral.fai = true;
    eeDirty(EE_GENERAL);
  }
#endif

  MENU(STR_MENURADIOSETUP, menuTabGeneral, e_Setup, ITEM_SETUP_MAX+1, {0, CASE_RTCLOCK(2) CASE_RTCLOCK(2) CASE_BATTGRAPH(1) LABEL(SOUND), CASE_AUDIO(0) CASE_BUZZER(0) CASE_VOICE(0) CASE_CPUARM(0) CASE_CPUARM(0) CASE_CPUARM(0) 0, CASE_AUDIO(0) CASE_VARIO_CPUARM(LABEL(VARIO)) CASE_VARIO_CPUARM(0) CASE_VARIO_CPUARM(0) CASE_VARIO_CPUARM(0) CASE_VARIO_CPUARM(0) CASE_HAPTIC(LABEL(HAPTIC)) CASE_HAPTIC(0) CASE_HAPTIC(0) CASE_HAPTIC(0) 0, LABEL(ALARMS), 0, CASE_PCBSKY9X(0) CASE_PCBSKY9X(0) 0, 0, 0, IF_ROTARY_ENCODERS(0) LABEL(BACKLIGHT), 0, 0, CASE_CPUARM(0) CASE_REVPLUS(0) CASE_PWM_BACKLIGHT(0) CASE_PWM_BACKLIGHT(0) 0, CASE_SPLASH_PARAM(0) CASE_GPS(0) CASE_GPS(0) CASE_PXX(0) CASE_CPUARM(0) CASE_CPUARM(0) IF_FAI_CHOICE(0) CASE_MAVLINK(0) CASE_CPUARM(0) 0, COL_TX_MODE, CASE_PCBTARANIS(0) 1/*to force edit mode*/});

  uint8_t sub = m_posVert - 1;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_TITLE_HEIGHT + 1 + i*FH;
    uint8_t k = i+s_pgOfs;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
#if defined(RTCLOCK)
      case ITEM_SETUP_DATE:
        lcd_putsLeft(y, STR_DATE);
        lcd_putc(RADIO_SETUP_DATE_COLUMN, y, '-'); lcd_putc(RADIO_SETUP_DATE_COLUMN+3*FW-2, y, '-');
        for (uint8_t j=0; j<3; j++) {
          uint8_t rowattr = (m_posHorz==j ? attr : 0);
          switch (j) {
            case 0:
              lcd_outdezAtt(RADIO_SETUP_DATE_COLUMN, y, t.tm_year+1900, rowattr);
              if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_year = checkIncDec(event, t.tm_year, 112, 200, 0);
              break;
            case 1:
              lcd_outdezNAtt(RADIO_SETUP_DATE_COLUMN+3*FW-2, y, t.tm_mon+1, rowattr|LEADING0, 2);
              if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_mon = checkIncDec(event, t.tm_mon, 0, 11, 0);
              break;
            case 2:
            {
              int16_t year = 1900 + t.tm_year;
              int8_t dlim = (((((year%4==0) && (year%100!=0)) || (year%400==0)) && (t.tm_mon==1)) ? 1 : 0);
              static const pm_uint8_t dmon[] PROGMEM = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
              dlim += pgm_read_byte(&dmon[t.tm_mon]);
              lcd_outdezNAtt(RADIO_SETUP_DATE_COLUMN+6*FW-4, y, t.tm_mday, rowattr|LEADING0, 2);
              if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_mday = checkIncDec(event, t.tm_mday, 1, dlim, 0);
              break;
            }
          }
        }
#if defined(PCBTARANIS)
        if (attr && m_posHorz < 0) lcd_filled_rect(RADIO_SETUP_2ND_COLUMN, y, LCD_W-RADIO_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH, 8);
#endif
        if (attr && checkIncDec_Ret) {
          g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
        }
        break;

      case ITEM_SETUP_TIME:
        lcd_putsLeft(y, STR_TIME);
        lcd_putc(RADIO_SETUP_TIME_COLUMN+1, y, ':'); lcd_putc(RADIO_SETUP_TIME_COLUMN+3*FW-2, y, ':');
        for (uint8_t j=0; j<3; j++) {
          uint8_t rowattr = (m_posHorz==j ? attr : 0);
          switch (j) {
            case 0:
              lcd_outdezNAtt(RADIO_SETUP_TIME_COLUMN, y, t.tm_hour, rowattr|LEADING0, 2);
              if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_hour = checkIncDec(event, t.tm_hour, 0, 23, 0);
              break;
            case 1:
              lcd_outdezNAtt(RADIO_SETUP_TIME_COLUMN+3*FWNUM, y, t.tm_min, rowattr|LEADING0, 2);
              if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_min = checkIncDec(event, t.tm_min, 0, 59, 0);
              break;
            case 2:
              lcd_outdezNAtt(RADIO_SETUP_TIME_COLUMN+6*FWNUM, y, t.tm_sec, rowattr|LEADING0, 2);
              if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_sec = checkIncDec(event, t.tm_sec, 0, 59, 0);
              break;
          }
        }
#if defined(PCBTARANIS)
        if (attr && m_posHorz < 0) lcd_filled_rect(RADIO_SETUP_2ND_COLUMN, y, LCD_W-RADIO_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH, 8);
#endif
        if (attr && checkIncDec_Ret)
          g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
        break;
#endif

#if defined(BATTGRAPH) || defined(PCBTARANIS)
      case ITEM_SETUP_BATT_RANGE:
        lcd_putsLeft(y, STR_BATTERY_RANGE);
        putsVolts(RADIO_SETUP_2ND_COLUMN, y,  90+g_eeGeneral.vBatMin, (m_posHorz==0 ? attr : 0)|LEFT|NO_UNIT);
        lcd_putc(lcdLastPos, y, '-');
        putsVolts(lcdLastPos+FW, y, 120+g_eeGeneral.vBatMax, (m_posHorz>0 ? attr : 0)|LEFT|NO_UNIT);
#if defined(PCBTARANIS)
        if (attr && m_posHorz < 0) lcd_filled_rect(RADIO_SETUP_2ND_COLUMN, y, LCD_W-RADIO_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH, 8);
#endif
        if (attr && s_editMode>0) {
          if (m_posHorz==0)
            CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMin, -50, g_eeGeneral.vBatMax+29); // min=4.0V
          else
            CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMax, g_eeGeneral.vBatMin-29, +40); // max=16.0V
        }
        break;
#endif

      case ITEM_SETUP_SOUND_LABEL:
        lcd_putsLeft(y, STR_SOUND_LABEL);
        break;

#if defined(AUDIO)
      case ITEM_SETUP_BEEP_MODE:
        g_eeGeneral.beepMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_SPEAKER, STR_VBEEPMODE, g_eeGeneral.beepMode, -2, 1, attr, event);
#if defined(FRSKY)
        if (attr && checkIncDec_Ret) frskySendAlarms();
#endif
        break;

#if defined(BUZZER)
      case ITEM_SETUP_BUZZER_MODE:
        g_eeGeneral.buzzerMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_BUZZER, STR_VBEEPMODE, g_eeGeneral.buzzerMode, -2, 1, attr, event);
#if defined(FRSKY)
        if (attr && checkIncDec_Ret) frskySendAlarms();
#endif
        break;
#endif
#else
      case ITEM_SETUP_BUZZER_MODE:
        g_eeGeneral.beepMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_SPEAKER, STR_VBEEPMODE, g_eeGeneral.beepMode, -2, 1, attr, event);
#if defined(FRSKY)
        if (attr && checkIncDec_Ret) frskySendAlarms();
#endif
        break;
#endif

#if defined(VOICE)
      case ITEM_SETUP_SPEAKER_VOLUME:
      {
        lcd_putsLeft(y, STR_SPEAKER_VOLUME);
        uint8_t b = g_eeGeneral.speakerVolume+VOLUME_LEVEL_DEF;
        displaySlider(RADIO_SETUP_2ND_COLUMN, y, b, VOLUME_LEVEL_MAX, attr);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, b, 0, VOLUME_LEVEL_MAX);
          if (checkIncDec_Ret) {
            g_eeGeneral.speakerVolume = (int8_t)b-VOLUME_LEVEL_DEF;
#if !defined(CPUARM)
            setVolume(b);
#endif
          }
        }
        break;
      }
#endif

#if defined(CPUARM)
      case ITEM_SETUP_BEEP_VOLUME:
        SLIDER_5POS(y, g_eeGeneral.beepVolume, STR_BEEP_VOLUME, event, attr);
        break;
      case ITEM_SETUP_WAV_VOLUME:
        SLIDER_5POS(y, g_eeGeneral.wavVolume, STR_WAV_VOLUME, event, attr);
        break;
      case ITEM_SETUP_BACKGROUND_VOLUME:
        SLIDER_5POS(y, g_eeGeneral.backgroundVolume, STR_BG_VOLUME, event, attr);
        break;
#endif

      case ITEM_SETUP_BEEP_LENGTH:
        SLIDER_5POS(y, g_eeGeneral.beepLength, STR_BEEP_LENGTH, event, attr);
        break;

#if defined(AUDIO)
      case ITEM_SETUP_SPEAKER_PITCH:
        lcd_putsLeft( y, STR_SPKRPITCH);
#if defined(CPUARM)
        lcd_putcAtt(RADIO_SETUP_2ND_COLUMN, y, '+', attr);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN+FW, y, g_eeGeneral.speakerPitch*15, attr|LEFT);
        lcd_putsAtt(lcdLastPos, y, "Hz", attr);
#else
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.speakerPitch, attr|LEFT);
#endif
        if (attr) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.speakerPitch, 0, 20);
        }
        break;
#endif

#if defined(CPUARM) && defined(VARIO)
      case ITEM_SETUP_VARIO_LABEL:
        lcd_putsLeft(y, STR_VARIO);
        break;
      case ITEM_SETUP_VARIO_VOLUME:
        SLIDER_5POS(y, g_eeGeneral.varioVolume, TR_SPEAKER_VOLUME, event, attr);
        break;
      case ITEM_SETUP_VARIO_PITCH:
        lcd_putsLeft(y, STR_PITCH_AT_ZERO);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, VARIO_FREQUENCY_ZERO+(g_eeGeneral.varioPitch*10), attr|LEFT);
        lcd_putsAtt(lcdLastPos, y, "Hz", attr);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.varioPitch, -40, 40);
        break;
      case ITEM_SETUP_VARIO_RANGE:
        lcd_putsLeft(y, STR_PITCH_AT_MAX);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, VARIO_FREQUENCY_ZERO+(g_eeGeneral.varioPitch*10)+VARIO_FREQUENCY_RANGE+(g_eeGeneral.varioRange*10), attr|LEFT);
        lcd_putsAtt(lcdLastPos, y, "Hz", attr);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.varioRange, -80, 80);
        break;
      case ITEM_SETUP_VARIO_REPEAT:
        lcd_putsLeft(y, STR_REPEAT_AT_ZERO);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, VARIO_REPEAT_ZERO+(g_eeGeneral.varioRepeat*10), attr|LEFT);
        lcd_putsAtt(lcdLastPos, y, STR_MS, attr);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.varioRepeat, -30, 50);
        break;
#endif

#if defined(HAPTIC)
      case ITEM_SETUP_HAPTIC_LABEL:
        lcd_putsLeft(y, STR_HAPTIC_LABEL);
        break;

      case ITEM_SETUP_HAPTIC_MODE:
        g_eeGeneral.hapticMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MODE, STR_VBEEPMODE, g_eeGeneral.hapticMode, -2, 1, attr, event);
        break;

      case ITEM_SETUP_HAPTIC_LENGTH:
        SLIDER_5POS(y, g_eeGeneral.hapticLength, STR_LENGTH, event, attr);
        break;

      case ITEM_SETUP_HAPTIC_STRENGTH:
        SLIDER_5POS(y, g_eeGeneral.hapticStrength, STR_HAPTICSTRENGTH, event, attr);
        break;
#endif

      case ITEM_SETUP_CONTRAST:
        lcd_putsLeft(y, STR_CONTRAST);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.contrast, attr|LEFT);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.contrast, CONTRAST_MIN, CONTRAST_MAX);
          lcdSetContrast();
        }
        break;

      case ITEM_SETUP_ALARMS_LABEL:
        lcd_putsLeft(y, STR_ALARMS_LABEL);
        break;

      case ITEM_SETUP_BATTERY_WARNING:
        lcd_putsLeft(y, STR_BATTERYWARNING);
        putsVolts(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.vBatWarn, attr|LEFT);
        if(attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatWarn, 40, 120); //4-12V
        break;

      case ITEM_SETUP_MEMORY_WARNING:
      {
        uint8_t b = 1-g_eeGeneral.disableMemoryWarning;
        g_eeGeneral.disableMemoryWarning = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_MEMORYWARNING, attr, event);
        break;
      }

      case ITEM_SETUP_ALARM_WARNING:
      {
        uint8_t b = 1-g_eeGeneral.disableAlarmWarning;
        g_eeGeneral.disableAlarmWarning = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_ALARMWARNING, attr, event);
        break;
      }

#if defined(PCBSKY9X)
      case ITEM_SETUP_CAPACITY_WARNING:
        lcd_putsLeft(y, STR_CAPAWARNING);
        putsValueWithUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.mAhWarn*50, UNIT_MAH, attr|LEFT) ;
        if(attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.mAhWarn, 0, 100);
        break;
#endif

#if defined(PCBSKY9X)
      case ITEM_SETUP_TEMPERATURE_WARNING:
        lcd_putsLeft(y, STR_TEMPWARNING);
        putsValueWithUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.temperatureWarn, UNIT_TEMPERATURE, attr|LEFT) ;
        if(attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.temperatureWarn, 0, 120); // 0 means no alarm
        break;
#endif

      case ITEM_SETUP_INACTIVITY_ALARM:
        lcd_putsLeft( y,STR_INACTIVITYALARM);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.inactivityTimer, attr|LEFT);
        lcd_putc(lcdLastPos, y, 'm');
        if(attr) g_eeGeneral.inactivityTimer = checkIncDec(event, g_eeGeneral.inactivityTimer, 0, 250, EE_GENERAL); //0..250minutes
        break;

#if ROTARY_ENCODERS > 0
      case ITEM_SETUP_RE_NAVIGATION:
        g_eeGeneral.reNavigation = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_RENAVIG, STR_VRENAVIG, g_eeGeneral.reNavigation, 0, NUM_ROTARY_ENCODERS, attr, event);
        if (attr && checkIncDec_Ret) {
          g_rotenc[NAVIGATION_RE_IDX()] = 0;
        }
        break;
#endif

      case ITEM_SETUP_BACKLIGHT_LABEL:
        lcd_putsLeft(y, STR_BACKLIGHT_LABEL);
        break;

      case ITEM_SETUP_BACKLIGHT_MODE:
        g_eeGeneral.backlightMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MODE, STR_VBLMODE, g_eeGeneral.backlightMode, e_backlight_mode_off, e_backlight_mode_on, attr, event);
        break;

      case ITEM_SETUP_FLASH_BEEP:
        g_eeGeneral.alarmsFlash = onoffMenuItem(g_eeGeneral.alarmsFlash, RADIO_SETUP_2ND_COLUMN, y, STR_ALARM, attr, event ) ;
        break;

      case ITEM_SETUP_BACKLIGHT_DELAY:
        lcd_putsLeft(y, STR_BLDELAY);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.lightAutoOff*5, attr|LEFT);
        lcd_putc(lcdLastPos, y, 's');
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.lightAutoOff, 0, 600/5);
        break;

#if defined(CPUARM)
      case ITEM_SETUP_BRIGHTNESS:
        lcd_putsLeft(y, STR_BRIGHTNESS);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, 100-g_eeGeneral.backlightBright, attr|LEFT) ;
        if (attr) {
          uint8_t b = 100 - g_eeGeneral.backlightBright;
          CHECK_INCDEC_GENVAR(event, b, 0, 100);
          g_eeGeneral.backlightBright = 100 - b;
        }
        break;
#endif

#if defined(PCBTARANIS) && defined(REVPLUS)
      case ITEM_SETUP_BACKLIGHT_COLOR:
        lcd_putsLeft(y, STR_BLCOLOR);
        displaySlider(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.backlightColor, 20, attr);
        if (attr) g_eeGeneral.backlightColor = checkIncDec(event, g_eeGeneral.backlightColor, 0, 20, EE_GENERAL | NO_INCDEC_MARKS);
        break;
#endif

#if defined(PWM_BACKLIGHT)
      case ITEM_SETUP_BACKLIGHT_BRIGHTNESS_OFF:
        lcd_putsLeft(y, STR_BLOFFBRIGHTNESS);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.blOffBright, attr|LEFT);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.blOffBright, 0, 15);
        break;

      case ITEM_SETUP_BACKLIGHT_BRIGHTNESS_ON:
        lcd_putsLeft(y, STR_BLONBRIGHTNESS);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, 15-g_eeGeneral.blOnBright, attr|LEFT);
        if (attr) g_eeGeneral.blOnBright = 15 - checkIncDecGen(event, 15-g_eeGeneral.blOnBright, 0, 15);
        break;
#endif

#if defined(SPLASH) && !defined(FSPLASH)
      case ITEM_SETUP_DISABLE_SPLASH:
      {
#if defined(PCBTARANIS)
        lcd_putsLeft(y, STR_SPLASHSCREEN);
        if (SPLASH_NEEDED()) {
          lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, SPLASH_TIMEOUT/100, attr|LEFT);
          lcd_putc(lcdLastPos, y, 's');
        }
        else {
          lcd_putsiAtt(RADIO_SETUP_2ND_COLUMN, y, STR_MMMINV, 0, attr); // TODO define
        }
        if (attr) g_eeGeneral.splashMode = -checkIncDecGen(event, -g_eeGeneral.splashMode, -3, 4);
#else
        uint8_t b = 1-g_eeGeneral.splashMode;
        g_eeGeneral.splashMode = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_SPLASHSCREEN, attr, event);
#endif
        break;
      }
#endif

#if defined(FRSKY) && defined(FRSKY_HUB) && defined(GPS)
      case ITEM_SETUP_TIMEZONE:
        lcd_putsLeft(y, STR_TIMEZONE);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.timezone, attr|LEFT);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.timezone, -12, 12);
        break;

      case ITEM_SETUP_GPSFORMAT:
        g_eeGeneral.gpsFormat = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_GPSCOORD, STR_GPSFORMAT, g_eeGeneral.gpsFormat, 0, 1, attr, event);
        break;
#endif

#if defined(PXX)
      case ITEM_SETUP_COUNTRYCODE:
        g_eeGeneral.countryCode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_COUNTRYCODE, STR_COUNTRYCODES, g_eeGeneral.countryCode, 0, 2, attr, event);
        break;
#endif

#if defined(CPUARM)
      case ITEM_SETUP_LANGUAGE:
        lcd_putsLeft(y, STR_VOICELANG);
        lcd_putsAtt(RADIO_SETUP_2ND_COLUMN, y, currentLanguagePack->name, attr);
        if (attr) {
          currentLanguagePackIdx = checkIncDec(event, currentLanguagePackIdx, 0, DIM(languagePacks)-2, EE_GENERAL);
          if (checkIncDec_Ret) {
            currentLanguagePack = languagePacks[currentLanguagePackIdx];
            strncpy(g_eeGeneral.ttsLanguage, currentLanguagePack->id, 2);
          }
        }
        break;

      case ITEM_SETUP_IMPERIAL:
        g_eeGeneral.imperial = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_UNITSSYSTEM, STR_VUNITSSYSTEM, g_eeGeneral.imperial, 0, 1, attr, event);
        break;
#endif

#if defined(FAI_CHOICE)
      case ITEM_SETUP_FAI:
        onoffMenuItem(g_eeGeneral.fai, RADIO_SETUP_2ND_COLUMN, y, PSTR("FAI Mode"), attr, event);
        if (attr && checkIncDec_Ret) {
          if (g_eeGeneral.fai)
            POPUP_WARNING(PSTR("FAI\001mode blocked!"));
          else
            POPUP_CONFIRMATION(PSTR("FAI mode?"));
        }
        break;
#endif

#if defined(MAVLINK)
      case ITEM_MAVLINK_BAUD:
        g_eeGeneral.mavbaud = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MAVLINK_BAUD_LABEL, STR_MAVLINK_BAUDS, g_eeGeneral.mavbaud, 0, 7, attr, event);
        break;
#endif

#if defined(CPUARM)
      case ITEM_SETUP_SWITCHES_DELAY:
        lcd_putsLeft(y, STR_SWITCHES_DELAY);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, 10*SWITCHES_DELAY(), attr|LEFT);
        lcd_putsAtt(lcdLastPos, y, STR_MS, attr);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.switchesDelay, -15, +15);
        break;
#endif

      case ITEM_SETUP_RX_CHANNEL_ORD:
        lcd_putsLeft(y, STR_RXCHANNELORD); // RAET->AETR
        for (uint8_t i=1; i<=4; i++) {
          putsChnLetter(RADIO_SETUP_2ND_COLUMN - FW + i*FW, y, channel_order(i), attr);
        }
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.templateSetup, 0, 23);
        break;

      case ITEM_SETUP_STICK_MODE_LABELS:
        lcd_putsLeft(y, NO_INDENT(STR_MODE));
        for (uint8_t i=0; i<4; i++) {
          lcd_img((6+4*i)*FW, y, sticks, i, 0);
#if defined(FRSKY_STICKS)
          if (g_eeGeneral.stickReverse & (1<<i)) {
            lcd_filled_rect((6+4*i)*FW, y, 3*FW, FH-1);
          }
#endif
        }
#if defined(FRSKY_STICKS)
	if (attr) {
	  s_editMode = 0;
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.stickReverse, 0, 15);
          lcd_rect(6*FW-1, y-1, 15*FW+2, 9);
        }
#endif
        break;

      case ITEM_SETUP_STICK_MODE:
        lcd_putcAtt(2*FW, y, '1'+g_eeGeneral.stickMode, attr);
        for (uint8_t i=0; i<4; i++) {
          putsMixerSource((6+4*i)*FW, y, MIXSRC_Rud + pgm_read_byte(modn12x3 + 4*g_eeGeneral.stickMode + i), 0);
        }
        if (attr && s_editMode>0) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.stickMode, 0, 3);
        }
        else if (stickMode != g_eeGeneral.stickMode) {
          pausePulses();
          stickMode = g_eeGeneral.stickMode;
          checkTHR();
          resumePulses();
          clearKeyEvents();
        }
#if defined(ROTARY_ENCODER_NAVIGATION)
        MOVE_CURSOR_FROM_HERE();
#endif
        break;
    }
  }
}

#if defined(SDCARD)
void menuGeneralSdManagerInfo(uint8_t event)
{
  SIMPLE_SUBMENU(STR_SD_INFO_TITLE, 1);

  lcd_putsLeft(2*FH, STR_SD_TYPE);
  lcd_puts(10*FW, 2*FH, SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD);

  lcd_putsLeft(3*FH, STR_SD_SIZE);
  lcd_outdezAtt(10*FW, 3*FH, SD_GET_SIZE_MB(), LEFT);
  lcd_putc(lcdLastPos, 3*FH, 'M');

  lcd_putsLeft(4*FH, STR_SD_SECTORS);
  lcd_outdezAtt(10*FW, 4*FH, SD_GET_BLOCKNR()/1000, LEFT);
  lcd_putc(lcdLastPos, 4*FH, 'k');

  lcd_putsLeft(5*FH, STR_SD_SPEED);
  lcd_outdezAtt(10*FW, 5*FH, SD_GET_SPEED()/1000, LEFT);
  lcd_puts(lcdLastPos, 5*FH, "kb/s");
}

inline bool isFilenameGreater(bool isfile, const char * fn, const char * line)
{
  return (isfile && !line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) > 0);
}

inline bool isFilenameLower(bool isfile, const char * fn, const char * line)
{
  return (!isfile && line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) < 0);
}

#if defined(PCBTARANIS)
void backupEeprom()
{
  char filename[60];
  uint8_t buffer[1024];
  FIL file;

  lcd_clear();
  lcd_putsLeft(4*FH, STR_WRITING);
  lcd_rect(3, 6*FH+4, 204, 7);
  lcdRefresh();

  //reset unexpectedShutdown to prevent warning when user restores EEPROM backup
  g_eeGeneral.unexpectedShutdown = 0;
  eeDirty(EE_GENERAL);
  eeCheck(true);

  // create the directory if needed...
  DIR folder;
  FRESULT result = f_opendir(&folder, EEPROMS_PATH);
  if (result != FR_OK) {
    if (result == FR_NO_PATH)
      result = f_mkdir(EEPROMS_PATH);
    if (result != FR_OK) {
      POPUP_WARNING(SDCARD_ERROR(result));
      return;
    }
  }

  // prepare the filename...
  char * tmp = strAppend(filename, EEPROMS_PATH "/eeprom");
  tmp = strAppendDate(tmp, true);
  strAppend(tmp, EEPROM_EXT);

  // open the file for writing...
  f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);

  for (int i=0; i<EESIZE; i+=1024) {
    UINT count;
    eeprom_read_block(buffer, i, 1024);
    f_write(&file, buffer, 1024, &count);
    lcd_hline(5, 6*FH+6, (200*i)/EESIZE, FORCE);
    lcd_hline(5, 6*FH+7, (200*i)/EESIZE, FORCE);
    lcd_hline(5, 6*FH+8, (200*i)/EESIZE, FORCE);
    lcdRefresh();
    SIMU_SLEEP(100/*ms*/);
  }

  f_close(&file);

  //set back unexpectedShutdown
  g_eeGeneral.unexpectedShutdown = 1;
  eeDirty(EE_GENERAL);
  eeCheck(true);
}
#endif

#if defined(PCBTARANIS)
void flashBootloader(const char * filename)
{
  FIL file;
  f_open(&file, filename, FA_READ);
  uint8_t buffer[1024];
  UINT count;

  lcd_clear();
  lcd_putsLeft(4*FH, STR_WRITING);
  lcd_rect(3, 6*FH+4, 204, 7);
  lcdRefresh();

  static uint8_t unlocked = 0;
  if (!unlocked) {
    unlocked = 1;
    unlockFlash();
  }

  for (int i=0; i<BOOTLOADER_SIZE; i+=1024) {
    watchdogSetTimeout(100/*1s*/);
    if (f_read(&file, buffer, 1024, &count) != FR_OK || count != 1024) {
      POPUP_WARNING(STR_SDCARD_ERROR);
      break;
    }
    if (i==0 && !isBootloaderStart((uint32_t *)buffer)) {
      POPUP_WARNING(STR_INCOMPATIBLE);
      break;
    }
    for (int j=0; j<1024; j+=FLASH_PAGESIZE) {
      writeFlash(CONVERT_UINT_PTR(FIRMWARE_ADDRESS+i+j), (uint32_t *)(buffer+j));
      lcd_hline(5, 6*FH+6, (200*i)/BOOTLOADER_SIZE, FORCE);
      lcd_hline(5, 6*FH+7, (200*i)/BOOTLOADER_SIZE, FORCE);
      lcd_hline(5, 6*FH+8, (200*i)/BOOTLOADER_SIZE, FORCE);
      lcdRefresh();
      SIMU_SLEEP(30/*ms*/);
    }
  }

  if (unlocked) {
    lockFlash();
    unlocked = 0;
  }

  f_close(&file);
}
#endif

void onSdManagerMenu(const char *result)
{
  TCHAR lfn[_MAX_LFN+1];

  uint8_t index = m_posVert-1-s_pgOfs;
  if (result == STR_SD_INFO) {
    pushMenu(menuGeneralSdManagerInfo);
  }
  else if (result == STR_SD_FORMAT) {
    POPUP_CONFIRMATION(STR_CONFIRM_FORMAT);
  }
  else if (result == STR_DELETE_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat_P(lfn, PSTR("/"));
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    f_unlink(lfn);
    strncpy(statusLineMsg, reusableBuffer.sdmanager.lines[index], 13);
    strcpy_P(statusLineMsg+min((uint8_t)strlen(statusLineMsg), (uint8_t)13), STR_REMOVED);
    showStatusLine();
    if ((uint16_t)m_posVert == reusableBuffer.sdmanager.count) m_posVert--;
    reusableBuffer.sdmanager.offset = s_pgOfs-1;
  }
#if defined(CPUARM)
  /* TODO else if (result == STR_LOAD_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    POPUP_WARNING(eeLoadModelSD(lfn));
  } */
  else if (result == STR_PLAY_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    audioQueue.stopAll();
    audioQueue.playFile(lfn, 0, ID_PLAY_FROM_SD_MANAGER);
  }
#endif
#if defined(PCBTARANIS)
  else if (result == STR_ASSIGN_BITMAP) {
    strAppendFilename(g_model.header.bitmap, reusableBuffer.sdmanager.lines[index], sizeof(g_model.header.bitmap));
    LOAD_MODEL_BITMAP();
    memcpy(modelHeaders[g_eeGeneral.currModel].bitmap, g_model.header.bitmap, sizeof(g_model.header.bitmap));
    eeDirty(EE_MODEL);
  }
  else if (result == STR_VIEW_TEXT) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    pushMenuTextView(lfn);
  }
  else if (result == STR_FLASH_BOOTLOADER) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    flashBootloader(lfn);
  }
#endif
#if defined(LUA)
  else if (result == STR_EXECUTE_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    luaExec(lfn);
  }
#endif
}

void menuGeneralSdManager(uint8_t _event)
{
  FILINFO fno;
  DIR dir;
  char *fn;   /* This function is assuming non-Unicode cfg. */
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);

#if defined(SDCARD)
  if (s_warning_result) {
    s_warning_result = 0;
    displayPopup(STR_FORMATTING);
    closeLogs();
#if defined(PCBSKY9X)
    Card_state = SD_ST_DATA;
#endif
#if defined(CPUARM)
    audioQueue.stopSD();
#endif
    if (f_mkfs(0, 1, 0) == FR_OK) {
      f_chdir("/");
      reusableBuffer.sdmanager.offset = -1;
    }
    else {
      POPUP_WARNING(STR_SDCARD_ERROR);
    }
  }
#endif

  uint8_t event = ((READ_ONLY() && EVT_KEY_MASK(_event) == KEY_ENTER) ? 0 : _event);
  SIMPLE_MENU(SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD, menuTabGeneral, e_Sd, 1+reusableBuffer.sdmanager.count);

  if (s_editMode > 0)
    s_editMode = 0;

  switch(_event) {
    case EVT_ENTRY:
      f_chdir(ROOT_PATH);
      reusableBuffer.sdmanager.offset = 65535;
      break;

#if defined(PCBTARANIS)
    case EVT_KEY_LONG(KEY_MENU):
      if (!READ_ONLY()) {
        killEvents(_event);
        // MENU_ADD_ITEM(STR_SD_INFO);  TODO: Implement
        MENU_ADD_ITEM(STR_SD_FORMAT);
        menuHandler = onSdManagerMenu;
      }
      break;
#endif

#if defined(PCBTARANIS)
    case EVT_KEY_BREAK(KEY_ENTER):
#else
    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_ENTER):
#endif
    {
      if (m_posVert > 0) {
        vertpos_t index = m_posVert-1-s_pgOfs;
        if (!reusableBuffer.sdmanager.lines[index][SD_SCREEN_FILE_LENGTH+1]) {
          f_chdir(reusableBuffer.sdmanager.lines[index]);
          s_pgOfs = 0;
          m_posVert = 1;
          reusableBuffer.sdmanager.offset = 65535;
          killEvents(_event);
          break;
        }
      }
      if (!IS_ROTARY_BREAK(_event) || m_posVert==0)
        break;
      // no break;
    }

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(_event);
#if !defined(PCBTARANIS)
      if (m_posVert == 0) {
        MENU_ADD_ITEM(STR_SD_INFO);
        MENU_ADD_ITEM(STR_SD_FORMAT);
      }
      else
#endif
      {
#if defined(CPUARM)
        uint8_t index = m_posVert-1-s_pgOfs;
        // TODO duplicated code for finding extension
        char * ext = reusableBuffer.sdmanager.lines[index];
        int len = strlen(ext) - 4;
        ext += len;
        /* TODO if (!strcasecmp(ext, MODELS_EXT)) {
          s_menu[s_menu_count++] = STR_LOAD_FILE;
        }
        else */ if (!strcasecmp(ext, SOUNDS_EXT)) {
          MENU_ADD_ITEM(STR_PLAY_FILE);
        }
#endif
#if defined(PCBTARANIS)
        else if (!strcasecmp(ext, BITMAPS_EXT) && !READ_ONLY() && len <= (int)sizeof(g_model.header.bitmap)) {
          MENU_ADD_ITEM(STR_ASSIGN_BITMAP);
        }
        else if (!strcasecmp(ext, TEXT_EXT)) {
          MENU_ADD_ITEM(STR_VIEW_TEXT);
        }
        else if (!strcasecmp(ext, FIRMWARE_EXT) && !READ_ONLY()) {
          MENU_ADD_ITEM(STR_FLASH_BOOTLOADER);
        }
#endif
#if defined(LUA)
        else if (!strcasecmp(ext, SCRIPTS_EXT)) {
          MENU_ADD_ITEM(STR_EXECUTE_FILE);
        }
#endif
        if (!READ_ONLY()) {
          MENU_ADD_ITEM(STR_DELETE_FILE);
          // MENU_ADD_ITEM(STR_RENAME_FILE);  TODO: Implement
          // MENU_ADD_ITEM(STR_COPY_FILE);    TODO: Implement
        }
      }
      menuHandler = onSdManagerMenu;
      break;
  }

  if (reusableBuffer.sdmanager.offset != s_pgOfs) {
    if (s_pgOfs == 0) {
      reusableBuffer.sdmanager.offset = 0;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    }
    else if (s_pgOfs == reusableBuffer.sdmanager.count-7) {
      reusableBuffer.sdmanager.offset = s_pgOfs;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    }
    else if (s_pgOfs > reusableBuffer.sdmanager.offset) {
      memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], 6*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[6], 0xff, SD_SCREEN_FILE_LENGTH);
      reusableBuffer.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = 1;
    }
    else {
      memmove(reusableBuffer.sdmanager.lines[1], reusableBuffer.sdmanager.lines[0], 6*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
    }

    reusableBuffer.sdmanager.count = 0;

    FRESULT res = f_opendir(&dir, ".");        /* Open the directory */
    if (res == FR_OK) {
      for (;;) {
        res = f_readdir(&dir, &fno);                   /* Read a directory item */
        if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        if (fno.fname[0] == '.' && fno.fname[1] == '\0') continue;             /* Ignore dot entry */
#if _USE_LFN
        fn = *fno.lfname ? fno.lfname : fno.fname;
#else
        fn = fno.fname;
#endif
        if (strlen(fn) > SD_SCREEN_FILE_LENGTH) continue;

        reusableBuffer.sdmanager.count++;

        bool isfile = !(fno.fattrib & AM_DIR);

        if (s_pgOfs == 0) {
          for (uint8_t i=0; i<LCD_LINES-1; i++) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameLower(isfile, fn, line)) {
              if (i < 6) memmove(reusableBuffer.sdmanager.lines[i+1], line, sizeof(reusableBuffer.sdmanager.lines[i]) * (6-i));
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[i]));
              strcpy(line, fn);
              line[SD_SCREEN_FILE_LENGTH+1] = isfile;
              break;
            }
          }
        }
        else if (reusableBuffer.sdmanager.offset == s_pgOfs) {
          for (int8_t i=6; i>=0; i--) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameGreater(isfile, fn, line)) {
              if (i > 0) memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], sizeof(reusableBuffer.sdmanager.lines[0]) * i);
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[i]));
              strcpy(line, fn);
              line[SD_SCREEN_FILE_LENGTH+1] = isfile;
              break;
            }
          }
        }
        else if (s_pgOfs > reusableBuffer.sdmanager.offset) {
          if (isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[5]) && isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[6])) {
            memset(reusableBuffer.sdmanager.lines[6], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[6], fn);
            reusableBuffer.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = isfile;
          }
        }
        else {
          if (isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[1]) && isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[0])) {
            memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[0], fn);
            reusableBuffer.sdmanager.lines[0][SD_SCREEN_FILE_LENGTH+1] = isfile;
          }
        }
      }
    }
  }

  reusableBuffer.sdmanager.offset = s_pgOfs;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_TITLE_HEIGHT + 1 + i*FH;
    lcdNextPos = 0;
    uint8_t attr = (m_posVert-1-s_pgOfs == i ? BSS|INVERS : BSS);
    if (reusableBuffer.sdmanager.lines[i][0]) {
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) { lcd_putcAtt(0, y, '[', attr); }
      lcd_putsAtt(lcdNextPos, y, reusableBuffer.sdmanager.lines[i], attr);
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) { lcd_putcAtt(lcdNextPos, y, ']', attr); }
    }
  }

#if defined(PCBTARANIS)
  static vertpos_t sdBitmapIdx = 0xFFFF;
  static uint8_t sdBitmap[MODEL_BITMAP_SIZE];
  vertpos_t index = m_posVert-1-s_pgOfs;
  if (m_posVert > 0) {
    char * ext = reusableBuffer.sdmanager.lines[index];
    ext += strlen(ext) - 4;
    if (!strcasecmp(ext, BITMAPS_EXT)) {
      if (sdBitmapIdx != m_posVert) {
        sdBitmapIdx = m_posVert;
        uint8_t *dest = sdBitmap;
        if (bmpLoad(dest, reusableBuffer.sdmanager.lines[index], MODEL_BITMAP_WIDTH, MODEL_BITMAP_HEIGHT)) {
          memcpy(sdBitmap, logo_taranis, MODEL_BITMAP_SIZE);
        }
      }
      lcd_bmp(22*FW+2, 2*FH+FH/2, sdBitmap);
    }
  }
#endif
}
#endif

#if defined(CPUARM)
void menuGeneralCustomFunctions(uint8_t event)
{
  MENU(STR_MENUGLOBALFUNCS, menuTabGeneral, e_GeneralCustomFunctions, NUM_CFN+1, {0, NAVIGATION_LINE_BY_LINE|4/*repeated*/});
  return menuCustomFunctions(event, g_eeGeneral.customFn, globalFunctionsContext);
}
#endif

#if LCD_W >= 212
  #define TRAINER_CALIB_POS 12
#else
  #define TRAINER_CALIB_POS 8
#endif

void menuGeneralTrainer(uint8_t event)
{
  uint8_t y;
  bool slave = SLAVE_MODE();

  MENU(STR_MENUTRAINER, menuTabGeneral, e_Trainer, (slave ? 1 : 7), {0, 2, 2, 2, 2, 0/*, 0*/});

  if (slave) {
    lcd_puts(7*FW, 4*FH, STR_SLAVE);
  }
  else {
    uint8_t attr;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);

    lcd_puts(3*FW, MENU_TITLE_HEIGHT+1, STR_MODESRC);

    y = MENU_TITLE_HEIGHT + 1 + FH;

    for (uint8_t i=1; i<=NUM_STICKS; i++) {
      uint8_t chan = channel_order(i);
      volatile TrainerMix *td = &g_eeGeneral.trainer.mix[chan-1];

      putsMixerSource(0, y, MIXSRC_Rud-1+chan, (m_posVert==i && m_posHorz<0) ? INVERS : 0);

      for (uint8_t j=0; j<3; j++) {

        attr = ((m_posVert==i && m_posHorz==j) ? blink : 0);

        switch(j) {
          case 0:
            lcd_putsiAtt(4*FW, y, STR_TRNMODE, td->mode, attr);
            if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->mode, 0, 2);
            break;

          case 1:
            lcd_outdezAtt(11*FW, y, td->studWeight, attr);
            if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->studWeight, -125, 125);
            break;

          case 2:
            lcd_putsiAtt(12*FW, y, STR_TRNCHN, td->srcChn, attr);
            if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->srcChn, 0, 3);
            break;
        }
      }
      y += FH;
    }

    attr = (m_posVert==5) ? blink : 0;
    lcd_putsLeft(MENU_TITLE_HEIGHT+1+5*FH, STR_MULTIPLIER);
    lcd_outdezAtt(LEN_MULTIPLIER*FW+3*FW, MENU_TITLE_HEIGHT+1+5*FH, g_eeGeneral.PPM_Multiplier+10, attr|PREC1);
    if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.PPM_Multiplier, -10, 40);

    attr = (m_posVert==6) ? INVERS : 0;
    if (attr) s_editMode = 0;
    lcd_putsAtt(0*FW, MENU_TITLE_HEIGHT+1+6*FH, STR_CAL, attr);
    for (uint8_t i=0; i<4; i++) {
      uint8_t x = (i*TRAINER_CALIB_POS+16)*FW/2;
#if defined (PPM_UNIT_PERCENT_PREC1)
      lcd_outdezAtt(x, MENU_TITLE_HEIGHT+1+6*FH, (g_ppmIns[i]-g_eeGeneral.trainer.calib[i])*2, PREC1);
#else
      lcd_outdezAtt(x, MENU_TITLE_HEIGHT+1+6*FH, (g_ppmIns[i]-g_eeGeneral.trainer.calib[i])/5, 0);
#endif
    }

    if (attr) {
      if (event==EVT_KEY_LONG(KEY_ENTER)){
        memcpy(g_eeGeneral.trainer.calib, g_ppmIns, sizeof(g_eeGeneral.trainer.calib));
        eeDirty(EE_GENERAL);
        AUDIO_WARNING1();
      }
    }
  }
}

void menuGeneralVersion(uint8_t event)
{
  SIMPLE_MENU(STR_MENUVERSION, menuTabGeneral, e_Vers, 1);

  lcd_putsLeft(MENU_TITLE_HEIGHT+FH, vers_stamp);
  
#if defined(COPROCESSOR)
  if (Coproc_valid == 1) {
     lcd_putsLeft(6*FH, PSTR("CoPr:"));
     lcd_outdez8(10*FW, 6*FH, Coproc_read);
  }
  else {
     lcd_putsLeft(6*FH, PSTR("CoPr: ---"));
  }
#endif

#if defined(PCBTARANIS)
  lcd_putsLeft(MENU_TITLE_HEIGHT+6*FH, STR_EEBACKUP);
  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    backupEeprom();
  }
#endif
}

void displayKeyState(uint8_t x, uint8_t y, EnumKeys key)
{
  uint8_t t = switchState(key);
  lcd_putcAtt(x, y, t+'0', t ? INVERS : 0);
}

void menuGeneralDiagKeys(uint8_t event)
{
  SIMPLE_MENU(STR_MENUDIAG, menuTabGeneral, e_Keys, 1);

  lcd_puts(14*FW, MENU_TITLE_HEIGHT+2*FH, STR_VTRIM);

  for(uint8_t i=0; i<9; i++) {
    coord_t y;

    if (i<8) {
      y = MENU_TITLE_HEIGHT + FH*3 + FH*(i/2);
      if (i&1) lcd_img(14*FW, y, sticks, i/2, 0);
      displayKeyState(i&1? 20*FW : 18*FW, y, (EnumKeys)(TRM_BASE+i));
    }

    if (i<6) {
      y = (5-i)*FH+MENU_TITLE_HEIGHT+FH;
      lcd_putsiAtt(0, y, STR_VKEYS, i, 0);
      displayKeyState(5*FW+2, y, (EnumKeys)(KEY_MENU+i));
    }

#if !defined(PCBTARANIS)
    if (i != SW_ID0-SW_BASE) {
      y = MENU_TITLE_HEIGHT+i*FH-2*FH;
      putsSwitches(8*FW, y, i+1, 0); //ohne off,on
      displayKeyState(11*FW+2, y, (EnumKeys)(SW_BASE+i));
    }
#endif
  }

#if defined(ROTARY_ENCODERS) || defined(ROTARY_ENCODER_NAVIGATION)
  for(uint8_t i=0; i<DIM(g_rotenc); i++) {
    coord_t y = MENU_TITLE_HEIGHT /* ??? + 1 ??? */ + i*FH;
    lcd_putsiAtt(14*FW, y, STR_VRENCODERS, i, 0);
    lcd_outdezNAtt(18*FW, y, g_rotenc[i], LEFT|(switchState((EnumKeys)(BTN_REa+i)) ? INVERS : 0));
  }
#endif

}

void menuGeneralDiagAna(uint8_t event)
{
#if defined(PCBSKY9X) && !defined(REVA)
  #define ANAS_ITEMS_COUNT 4
#elif defined(PCBSKY9X)
  #define ANAS_ITEMS_COUNT 3
#else
  #define ANAS_ITEMS_COUNT 2
#endif

  SIMPLE_MENU(STR_MENUANA, menuTabGeneral, e_Ana, ANAS_ITEMS_COUNT);

  STICK_SCROLL_DISABLE();

  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
    coord_t y = MENU_TITLE_HEIGHT + 1 + (i/2)*FH;
    uint8_t x = i&1 ? 64+5 : 0;
    putsStrIdx(x, y, PSTR("A"), i+1);
    lcd_putc(lcdNextPos, y, ':');
    lcd_outhex4(x+3*FW-1, y, anaIn(i));
    lcd_outdez8(x+10*FW-1, y, (int16_t)calibratedStick[CONVERT_MODE(i)]*25/256);
  }

#if !defined(CPUARM)
  // Display raw BandGap result (debug)
  lcd_puts(64+5, MENU_TITLE_HEIGHT+1+3*FH, STR_BG);
  lcd_outdezAtt(64+5+6*FW-3, 1+4*FH, BandGap, 0);
#endif

#if defined(PCBTARANIS)
  lcd_putsLeft(MENU_TITLE_HEIGHT+1+5*FH, STR_BATT_CALIB);
  static int32_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(TX_VOLTAGE)) / 8;
  uint32_t batCalV = (adcBatt + (adcBatt*g_eeGeneral.vBatCalib)/128) * BATT_SCALE;
  batCalV >>= 11;
  batCalV += 2; // because of the diode
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, MENU_TITLE_HEIGHT+1+5*FH, batCalV, (m_posVert==1 ? INVERS : 0));
#elif defined(PCBSKY9X)
  lcd_putsLeft(MENU_TITLE_HEIGHT+1+4*FH, STR_BATT_CALIB);
  static int32_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(TX_VOLTAGE)) / 8;
  uint32_t batCalV = (adcBatt + adcBatt*(g_eeGeneral.vBatCalib)/128) * 4191;
  batCalV /= 55296;
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, MENU_TITLE_HEIGHT+1+4*FH, batCalV, (m_posVert==1 ? INVERS : 0));
#elif defined(PCBGRUVIN9X)
  lcd_putsLeft(6*FH-2, STR_BATT_CALIB);
  // Gruvin wants 2 decimal places and instant update of volts calib field when button pressed
  static uint16_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(TX_VOLTAGE)) / 8; // running average, sourced directly (to avoid unending debate :P)
  uint32_t batCalV = ((uint32_t)adcBatt*1390 + (10*(int32_t)adcBatt*g_eeGeneral.vBatCalib)/8) / BandGap;
  lcd_outdezNAtt(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, batCalV, PREC2|(m_posVert==1 ? INVERS : 0));
#else
  lcd_putsLeft(6*FH-2, STR_BATT_CALIB);
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, g_vbat100mV, (m_posVert==1 ? INVERS : 0));
#endif
  if (m_posVert==1) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatCalib, -127, 127);

#if defined(PCBSKY9X) && !defined(REVA)
  lcd_putsLeft(6*FH+1, STR_CURRENT_CALIB);
  putsValueWithUnit(LEN_CALIB_FIELDS*FW+4*FW, 6*FH+1, getCurrent(), UNIT_MILLIAMPS, (m_posVert==2 ? INVERS : 0)) ;
  if (m_posVert==2) CHECK_INCDEC_GENVAR(event, g_eeGeneral.currentCalib, -49, 49);
#endif

#if defined(PCBSKY9X)
  lcd_putsLeft(7*FH+1, STR_TEMP_CALIB);
  putsValueWithUnit(LEN_CALIB_FIELDS*FW+4*FW, 7*FH+1, getTemperature(), UNIT_TEMPERATURE, (m_posVert==3 ? INVERS : 0)) ;
  if (m_posVert==3) CHECK_INCDEC_GENVAR(event, g_eeGeneral.temperatureCalib, -100, 100);
#endif
}

#if defined(PCBTARANIS)
enum menuGeneralHwItems {
  ITEM_SETUP_HW_LABEL_STICKS,
  ITEM_SETUP_HW_STICK1,
  ITEM_SETUP_HW_STICK2,
  ITEM_SETUP_HW_STICK3,
  ITEM_SETUP_HW_STICK4,
  ITEM_SETUP_HW_LABEL_POTS,
  ITEM_SETUP_HW_POT1,
  ITEM_SETUP_HW_POT2,
  CASE_REVPLUS(ITEM_SETUP_HW_POT3)
  ITEM_SETUP_HW_LS,
  ITEM_SETUP_HW_RS,
  ITEM_SETUP_HW_LABEL_SWITCHES,
  ITEM_SETUP_HW_SA,
  ITEM_SETUP_HW_SB,
  ITEM_SETUP_HW_SC,
  ITEM_SETUP_HW_SD,
  ITEM_SETUP_HW_SE,
  ITEM_SETUP_HW_SF,
  ITEM_SETUP_HW_SG,
  ITEM_SETUP_HW_SH,
  ITEM_SETUP_HW_SI,
  ITEM_SETUP_HW_SJ,
  ITEM_SETUP_HW_SK,
  ITEM_SETUP_HW_SL,
  ITEM_SETUP_HW_SM,
  ITEM_SETUP_HW_SN,
  ITEM_SETUP_HW_UART3_MODE,
  ITEM_SETUP_HW_MAX
};

#define HW_SETTINGS_COLUMN 15*FW

#if defined(REVPLUS)
  #define POTS_ROWS NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, 0, 0
#else
  #define POTS_ROWS NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, 0, 0
#endif

#define SWITCH_ROWS(x) uint8_t(IS_2x2POS(x) ? 0 : HIDDEN_ROW)

void menuGeneralHardware(uint8_t event)
{
  MENU(STR_HARDWARE, menuTabGeneral, e_Hardware, ITEM_SETUP_HW_MAX+1, {0, LABEL(Sticks), 0, 0, 0, 0, LABEL(Pots), POTS_ROWS, LABEL(Switches), NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, SWITCH_ROWS(0), SWITCH_ROWS(1), SWITCH_ROWS(2), SWITCH_ROWS(3), SWITCH_ROWS(4), SWITCH_ROWS(6), 0});

  uint8_t sub = m_posVert - 1;

  for (uint8_t i=0; i<LCD_LINES-1; ++i) {
    coord_t y = MENU_TITLE_HEIGHT + 1 + i*FH;
    uint8_t k = i + s_pgOfs;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j+1] == HIDDEN_ROW)
        k++;
    }
    uint8_t attr = (sub == k ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
    switch (k) {
      case ITEM_SETUP_HW_LABEL_STICKS:
        lcd_putsLeft(y, "Sticks");
        break;
      case ITEM_SETUP_HW_STICK1:
      case ITEM_SETUP_HW_STICK2:
      case ITEM_SETUP_HW_STICK3:
      case ITEM_SETUP_HW_STICK4:
      case ITEM_SETUP_HW_LS:
      case ITEM_SETUP_HW_RS:
      {
        int idx = (k<=ITEM_SETUP_HW_STICK4 ? k-ITEM_SETUP_HW_STICK1 : k-ITEM_SETUP_HW_LS+7);
        lcd_putsiAtt(INDENT_WIDTH, y, STR_VSRCRAW, idx+1, 0);
        if (ZEXIST(g_eeGeneral.anaNames[idx]) || attr)
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.anaNames[idx], LEN_ANA_NAME, event, attr);
        else
          lcd_putsiAtt(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
        break;
      }
      case ITEM_SETUP_HW_LABEL_POTS:
        lcd_putsLeft(y, "Pots");
        break;
      case ITEM_SETUP_HW_POT1:
      case ITEM_SETUP_HW_POT2:
#if defined(REVPLUS)
      case ITEM_SETUP_HW_POT3:
#endif
      {
        int idx = k - ITEM_SETUP_HW_POT1;
        uint8_t shift = (2*idx);
        uint8_t mask = (0x03 << shift);
        lcd_putsiAtt(INDENT_WIDTH, y, STR_VSRCRAW, NUM_STICKS+idx+1, m_posHorz < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.anaNames[NUM_STICKS+idx]) || (attr && m_posHorz == 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.anaNames[NUM_STICKS+idx], LEN_ANA_NAME, event, attr && m_posHorz == 0);
        else
          lcd_putsiAtt(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);

        uint8_t potType = (g_eeGeneral.potsType & mask) >> shift;
        if (potType == POT_TYPE_NONE && k <= ITEM_SETUP_HW_POT2)
          potType = POT_TYPE_DETENT;
        potType = selectMenuItem(HW_SETTINGS_COLUMN+5*FW, y, "", STR_POTTYPES, potType, 0, POT_TYPE_MAX, m_posHorz == 1 ? attr : 0, event);
        if (potType == POT_TYPE_DETENT && k <= ITEM_SETUP_HW_POT2)
          potType = POT_TYPE_NONE;
        g_eeGeneral.potsType &= ~mask;
        g_eeGeneral.potsType |= (potType << shift);
        break;
      }
      case ITEM_SETUP_HW_LABEL_SWITCHES:
        lcd_putsLeft(y, "Switches");
        break;
      case ITEM_SETUP_HW_SA:
      case ITEM_SETUP_HW_SB:
      case ITEM_SETUP_HW_SC:
      case ITEM_SETUP_HW_SD:
      case ITEM_SETUP_HW_SE:
      case ITEM_SETUP_HW_SF:
      case ITEM_SETUP_HW_SG:
      case ITEM_SETUP_HW_SH:
      case ITEM_SETUP_HW_SI:
      case ITEM_SETUP_HW_SJ:
      case ITEM_SETUP_HW_SK:
      case ITEM_SETUP_HW_SL:
      case ITEM_SETUP_HW_SM:
      case ITEM_SETUP_HW_SN:
      {      
        int index = k-ITEM_SETUP_HW_SA;
        char label[] = INDENT "S*";
        label[2] = 'A' + index;
        uint32_t config = SWITCH_CONFIG(index);
        lcd_putsAtt(0, y, label, m_posHorz < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.switchNames[index]) || (attr && m_posHorz == 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.switchNames[index], LEN_SWITCH_NAME, event, m_posHorz == 0 ? attr : 0);
        else
          lcd_putsiAtt(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
        if (k <= ITEM_SETUP_HW_SH) {
          config = selectMenuItem(HW_SETTINGS_COLUMN+5*FW, y, "", "\007DefaultToggle\0""2POS\0  3POS\0  2x2POS\0", config, 0, 4, m_posHorz == 1 ? attr : 0, event);
          if (attr && checkIncDec_Ret) {
            uint32_t mask = 0x0f << (4*index);
            g_eeGeneral.switchConfig = (g_eeGeneral.switchConfig & ~mask) | (config << (4*index));
          }
        }
        break;
      }
      case ITEM_SETUP_HW_UART3_MODE:
        g_eeGeneral.uart3Mode = selectMenuItem(HW_SETTINGS_COLUMN, y, STR_UART3MODE, STR_UART3MODES, g_eeGeneral.uart3Mode, 0, UART_MODE_MAX, attr, event);
        if (attr && checkIncDec_Ret) {
          uart3Init(g_eeGeneral.uart3Mode, MODEL_TELEMETRY_PROTOCOL());
        }
        break;
    }
  }
}

#elif defined(PCBSKY9X)

enum menuGeneralHwItems {
  ITEM_SETUP_HW_OPTREX_DISPLAY,
  ITEM_SETUP_HW_STICKS_GAINS_LABELS,
  ITEM_SETUP_HW_STICK_LV_GAIN,
  ITEM_SETUP_HW_STICK_LH_GAIN,
  ITEM_SETUP_HW_STICK_RV_GAIN,
  ITEM_SETUP_HW_STICK_RH_GAIN,
  IF_ROTARY_ENCODERS(ITEM_SETUP_HW_ROTARY_ENCODER)
  CASE_BLUETOOTH(ITEM_SETUP_HW_BT_BAUDRATE)
  ITEM_SETUP_HW_MAX
};

#define GENERAL_HW_PARAM_OFS (2+(15*FW))
void menuGeneralHardware(uint8_t event)
{
  MENU(STR_HARDWARE, menuTabGeneral, e_Hardware, ITEM_SETUP_HW_MAX+1, {0, 0, (uint8_t)-1, 0, 0, 0, IF_ROTARY_ENCODERS(0) CASE_BLUETOOTH(0)});

  uint8_t sub = m_posVert - 1;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_TITLE_HEIGHT + 1 + i*FH;
    uint8_t k = i+s_pgOfs;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_SETUP_HW_OPTREX_DISPLAY:
        g_eeGeneral.optrexDisplay = selectMenuItem(GENERAL_HW_PARAM_OFS, y, STR_LCD, STR_VLCD, g_eeGeneral.optrexDisplay, 0, 1, attr, event);
        break;

      case ITEM_SETUP_HW_STICKS_GAINS_LABELS:
        lcd_putsLeft(y, PSTR("Sticks"));
        break;

      case ITEM_SETUP_HW_STICK_LV_GAIN:
      case ITEM_SETUP_HW_STICK_LH_GAIN:
      case ITEM_SETUP_HW_STICK_RV_GAIN:
      case ITEM_SETUP_HW_STICK_RH_GAIN:
      {
        lcd_putsiAtt(INDENT_WIDTH, y, PSTR("\002LVLHRVRH"), k-ITEM_SETUP_HW_STICK_LV_GAIN, 0);
        lcd_puts(INDENT_WIDTH+3*FW, y, PSTR("Gain"));
        uint8_t mask = (1<<(k-ITEM_SETUP_HW_STICK_LV_GAIN));
        uint8_t val = (g_eeGeneral.sticksGain & mask ? 1 : 0);
        lcd_putcAtt(GENERAL_HW_PARAM_OFS, y, val ? '2' : '1', attr);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, val, 0, 1);
          if (checkIncDec_Ret) {
            g_eeGeneral.sticksGain ^= mask;
            setSticksGain(g_eeGeneral.sticksGain);
          }
        }
        break;
      }

#if defined(ROTARY_ENCODERS)
      case ITEM_SETUP_HW_ROTARY_ENCODER:
        g_eeGeneral.rotarySteps = selectMenuItem(GENERAL_HW_PARAM_OFS, y, PSTR("Rotary Encoder"), PSTR("\0062steps4steps"), g_eeGeneral.rotarySteps, 0, 1, attr, event);
        break;
#endif

#if defined(BLUETOOTH)
      case ITEM_SETUP_HW_BT_BAUDRATE:
        g_eeGeneral.btBaudrate = selectMenuItem(GENERAL_HW_PARAM_OFS, y, STR_BAUDRATE, PSTR("\005115k 9600 19200"), g_eeGeneral.btBaudrate, 0, 2, attr, event);
        if (attr && checkIncDec_Ret) {
          btInit();
        }
        break;
#endif

    }
  }
}
#endif

#define XPOT_DELTA 10
#define XPOT_DELAY 10 /* cycles */

void menuCommonCalib(uint8_t event)
{
  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) { // get low and high vals for sticks and trims
    int16_t vt = anaIn(i);
    reusableBuffer.calib.loVals[i] = min(vt, reusableBuffer.calib.loVals[i]);
    reusableBuffer.calib.hiVals[i] = max(vt, reusableBuffer.calib.hiVals[i]);
    if (i >= POT1 && i <= POT_LAST) {
      if (IS_POT_WITHOUT_DETENT(i)) {
        reusableBuffer.calib.midVals[i] = (reusableBuffer.calib.hiVals[i] + reusableBuffer.calib.loVals[i]) / 2;
      }
#if defined(PCBTARANIS)
      uint8_t idx = i - POT1;
      int count = reusableBuffer.calib.xpotsCalib[idx].stepsCount;
      if (IS_POT_MULTIPOS(i) && count <= XPOTS_MULTIPOS_COUNT) {
        if (reusableBuffer.calib.xpotsCalib[idx].lastCount == 0 || vt < reusableBuffer.calib.xpotsCalib[idx].lastPosition - XPOT_DELTA || vt > reusableBuffer.calib.xpotsCalib[idx].lastPosition + XPOT_DELTA) {
          reusableBuffer.calib.xpotsCalib[idx].lastPosition = vt;
          reusableBuffer.calib.xpotsCalib[idx].lastCount = 1;
        }
        else {
          if (reusableBuffer.calib.xpotsCalib[idx].lastCount < 255) reusableBuffer.calib.xpotsCalib[idx].lastCount++;
        }
        if (reusableBuffer.calib.xpotsCalib[idx].lastCount == XPOT_DELAY) {
          int16_t position = reusableBuffer.calib.xpotsCalib[idx].lastPosition;
          bool found = false;
          for (int j=0; j<count; j++) {
            int16_t step = reusableBuffer.calib.xpotsCalib[idx].steps[j];
            if (position >= step-XPOT_DELTA && position <= step+XPOT_DELTA) {
              found = true;
              break;
            }
          }
          if (!found) {
            if (count < XPOTS_MULTIPOS_COUNT) {
              reusableBuffer.calib.xpotsCalib[idx].steps[count] = position;
            }
            reusableBuffer.calib.xpotsCalib[idx].stepsCount += 1;
          }
        }
      }
#endif
    }
  }

  calibrationState = reusableBuffer.calib.state; // make sure we don't scroll while calibrating

  switch (event)
  {
    case EVT_ENTRY:
      reusableBuffer.calib.state = 0;
      break;

    case EVT_KEY_BREAK(KEY_ENTER):
      reusableBuffer.calib.state++;
      break;
  }

  switch (reusableBuffer.calib.state) {
    case 0:
      // START CALIBRATION
      if (!READ_ONLY()) {
        lcd_putsLeft(MENU_TITLE_HEIGHT+2*FH, STR_MENUTOSTART);
      }
      break;

    case 1:
      // SET MIDPOINT
      lcd_putsAtt(0*FW, MENU_TITLE_HEIGHT+FH, STR_SETMIDPOINT, INVERS);
      lcd_putsLeft(MENU_TITLE_HEIGHT+2*FH, STR_MENUWHENDONE);

      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
        reusableBuffer.calib.loVals[i] = 15000;
        reusableBuffer.calib.hiVals[i] = -15000;
        reusableBuffer.calib.midVals[i] = anaIn(i);
#if defined(PCBTARANIS)
        if (i<NUM_XPOTS) {
          reusableBuffer.calib.xpotsCalib[i].stepsCount = 0;
          reusableBuffer.calib.xpotsCalib[i].lastCount = 0;
        }
#endif
      }
      break;

    case 2:
      // MOVE STICKS/POTS
      STICK_SCROLL_DISABLE();
      lcd_putsAtt(0*FW, MENU_TITLE_HEIGHT+FH, STR_MOVESTICKSPOTS, INVERS);
      lcd_putsLeft(MENU_TITLE_HEIGHT+2*FH, STR_MENUWHENDONE);

      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
        if (abs(reusableBuffer.calib.loVals[i]-reusableBuffer.calib.hiVals[i]) > 50) {
          g_eeGeneral.calib[i].mid = reusableBuffer.calib.midVals[i];
          int16_t v = reusableBuffer.calib.midVals[i] - reusableBuffer.calib.loVals[i];
          g_eeGeneral.calib[i].spanNeg = v - v/STICK_TOLERANCE;
          v = reusableBuffer.calib.hiVals[i] - reusableBuffer.calib.midVals[i];
          g_eeGeneral.calib[i].spanPos = v - v/STICK_TOLERANCE;
        }
      }
      break;

    case 3:
#if defined(PCBTARANIS)
      for (uint8_t i=POT1; i<=POT_LAST; i++) {
        int idx = i - POT1;
        int count = reusableBuffer.calib.xpotsCalib[idx].stepsCount;
        if (IS_POT_MULTIPOS(i)) {
          if (count > 1 && count <= XPOTS_MULTIPOS_COUNT) {
            for (int j=0; j<count; j++) {
              for (int k=j+1; k<count; k++) {
                if (reusableBuffer.calib.xpotsCalib[idx].steps[k] < reusableBuffer.calib.xpotsCalib[idx].steps[j]) {
                  swap(reusableBuffer.calib.xpotsCalib[idx].steps[j], reusableBuffer.calib.xpotsCalib[idx].steps[k]);
                }
              }
            }
            StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[i];
            calib->count = count - 1;
            for (int j=0; j<calib->count; j++) {
              calib->steps[j] = (reusableBuffer.calib.xpotsCalib[idx].steps[j+1] + reusableBuffer.calib.xpotsCalib[idx].steps[j]) >> 5;
            }
          }
          else {
            g_eeGeneral.potsType &= ~(0x03<<(2*idx));
          }
        }
      }
#endif
      g_eeGeneral.chkSum = evalChkSum();
      eeDirty(EE_GENERAL);
      reusableBuffer.calib.state = 4;
      break;

    default:
      reusableBuffer.calib.state = 0;
      break;
  }

  doMainScreenGraphics();

#if defined(PCBTARANIS)
  drawPotsBars();
  for (int i=POT1; i<=POT_LAST; i++) {
    uint8_t steps = 0;
    if (reusableBuffer.calib.state == 2) {
      steps = reusableBuffer.calib.xpotsCalib[i-POT1].stepsCount;
    }
    else if (IS_POT_MULTIPOS(i)) {
      StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[i];
      steps = calib->count + 1;
    }
    if (steps > 0 && steps <= XPOTS_MULTIPOS_COUNT) {
      lcd_outdezAtt(LCD_W/2-2+(i-POT1)*5, LCD_H-6, steps, TINSIZE);
    }
  }
#endif
}

void menuGeneralCalib(uint8_t event)
{
  check_simple(event, e_Calib, menuTabGeneral, DIM(menuTabGeneral), 0);

  if (menuEvent) {
    calibrationState = 0;
  }

  TITLE(STR_MENUCALIBRATION);
  menuCommonCalib(READ_ONLY() ? 0 : event);
}

void menuFirstCalib(uint8_t event)
{
  if (event == EVT_KEY_BREAK(KEY_EXIT) || reusableBuffer.calib.state == 4) {
    calibrationState = 0;
    chainMenu(menuMainView);
  }
  else {
    lcd_putsCenter(0*FH, MENUCALIBRATION);
    lcd_invert_line(0);
    menuCommonCalib(event);
  }
}
