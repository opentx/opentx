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

enum EnumTabDiag {
  e_Setup,
  IF_SDCARD(e_Sd)
  e_Trainer,
  e_Vers,
  e_Keys,
  e_Ana,
  IF_PCBSKY9X(e_Hardware)
  e_Calib
};

void menuGeneralSetup(uint8_t event);
void menuGeneralSdManager(uint8_t event);
void menuGeneralTrainer(uint8_t event);
void menuGeneralVersion(uint8_t event);
void menuGeneralDiagKeys(uint8_t event);
void menuGeneralDiagAna(uint8_t event);
void menuGeneralHardware(uint8_t event);
void menuGeneralCalib(uint8_t event);

const MenuFuncP_PROGMEM menuTabDiag[] PROGMEM = {
  menuGeneralSetup,
  IF_SDCARD(menuGeneralSdManager)
  menuGeneralTrainer,
  menuGeneralVersion,
  menuGeneralDiagKeys,
  menuGeneralDiagAna,
  IF_PCBSKY9X(menuGeneralHardware)
  menuGeneralCalib
};

#if LCD_W >= 212
#define RADIO_SETUP_2ND_COLUMN  (LCD_W-10*FW-MENUS_SCROLLBAR_WIDTH)
#define RADIO_SETUP_DATE_COLUMN RADIO_SETUP_2ND_COLUMN + 4*FWNUM
#define RADIO_SETUP_TIME_COLUMN RADIO_SETUP_2ND_COLUMN + 2*FWNUM
#else
#define RADIO_SETUP_2ND_COLUMN  (LCD_W-6*FW-3-MENUS_SCROLLBAR_WIDTH)
#define RADIO_SETUP_TIME_COLUMN (FW*15+9)
#define RADIO_SETUP_DATE_COLUMN (FW*15+5)
#endif

#if !defined(CPUM64)
  void displaySlider(uint8_t x, uint8_t y, uint8_t value, uint8_t max, uint8_t attr)
  {
    lcd_putc(RADIO_SETUP_2ND_COLUMN+(value*4*FW)/max, y, '$');
    lcd_hline(RADIO_SETUP_2ND_COLUMN, y+3, 5*FW-1, SOLID);
    if (attr && (!(attr & BLINK) || !BLINK_ON_PHASE)) lcd_filled_rect(RADIO_SETUP_2ND_COLUMN, y, 5*FW-1, FH-1);
  }
  #define SLIDER_5POS(y, value, label, event, attr) { \
    int8_t tmp = value; \
    displaySlider(RADIO_SETUP_2ND_COLUMN, y, 2+tmp, 4, attr); \
    value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, NULL, tmp, -2, +2, attr, event); \
  }
#elif defined(GRAPHICS)
  void display5posSlider(uint8_t x, uint8_t y, uint8_t value, uint8_t attr)
  {
    lcd_putc(RADIO_SETUP_2ND_COLUMN+2*FW+(value*FW), y, '$');
    lcd_hline(RADIO_SETUP_2ND_COLUMN, y+3, 5*FW-1, SOLID);
    if (attr && (!(attr & BLINK) || !BLINK_ON_PHASE)) lcd_filled_rect(RADIO_SETUP_2ND_COLUMN, y, 5*FW-1, FH-1);
  }
  #define SLIDER_5POS(y, value, label, event, attr) { \
    int8_t tmp = value; \
    display5posSlider(RADIO_SETUP_2ND_COLUMN, y, tmp, attr); \
    value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, NULL, tmp, -2, +2, attr, event); \
  }
  #define displaySlider(x, y, value, max, attr) lcd_outdezAtt(x, y, value, attr|LEFT)
#else
  #define SLIDER_5POS(y, value, label, event, attr) value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, STR_VBEEPLEN, value, -2, +2, attr, event)
  #define displaySlider(x, y, value, max, attr) lcd_outdezAtt(x, y, value, attr|LEFT)
#endif

enum menuGeneralSetupItems {
  IF_RTCLOCK(ITEM_SETUP_DATE)
  IF_RTCLOCK(ITEM_SETUP_TIME)
  IF_BATTGRAPH(ITEM_SETUP_BATT_RANGE)
  ITEM_SETUP_SOUND_LABEL,
  IF_AUDIO(ITEM_SETUP_BEEP_MODE)
  IF_BUZZER(ITEM_SETUP_BUZZER_MODE)
  ITEM_SETUP_BEEP_LENGTH,
  IF_AUDIO(ITEM_SETUP_SPEAKER_PITCH)
  IF_VOICE(ITEM_SETUP_SPEAKER_VOLUME)
  IF_HAPTIC(ITEM_SETUP_HAPTIC_LABEL)
  IF_HAPTIC(ITEM_SETUP_HAPTIC_MODE)
  IF_HAPTIC(ITEM_SETUP_HAPTIC_LENGTH)
  IF_HAPTIC(ITEM_SETUP_HAPTIC_STRENGTH)
  IF_PCBSKY9X(ITEM_SETUP_BRIGHTNESS)
  IF_9X(ITEM_SETUP_CONTRAST)
  ITEM_SETUP_ALARMS_LABEL,
  ITEM_SETUP_BATTERY_WARNING,
  IF_PCBSKY9X(ITEM_SETUP_CAPACITY_WARNING)
  IF_PCBSKY9X(ITEM_SETUP_TEMPERATURE_WARNING)
  ITEM_SETUP_INACTIVITY_ALARM,
  ITEM_SETUP_MEMORY_WARNING,
  ITEM_SETUP_ALARM_WARNING,
  IF_ROTARY_ENCODERS(ITEM_SETUP_RE_NAVIGATION)
  ITEM_SETUP_BACKLIGHT_LABEL,
  ITEM_SETUP_BACKLIGHT_MODE,
  ITEM_SETUP_BACKLIGHT_DELAY,
  CASE_PWM_BACKLIGHT(ITEM_SETUP_BACKLIGHT_BRIGHTNESS_OFF)
  CASE_PWM_BACKLIGHT(ITEM_SETUP_BACKLIGHT_BRIGHTNESS_ON)
  ITEM_SETUP_FLASH_BEEP,
#if defined(SPLASH) && !defined(FSPLASH)
  ITEM_SETUP_DISABLE_SPLASH,
#endif
  IF_FRSKY(ITEM_SETUP_TIMEZONE)
  IF_FRSKY(ITEM_SETUP_GPSFORMAT)
  IF_PXX(ITEM_SETUP_COUNTRYCODE)
  IF_CPUARM(ITEM_SETUP_IMPERIAL)
  ITEM_SETUP_RX_CHANNEL_ORD,
  ITEM_SETUP_STICK_MODE_LABELS,
  ITEM_SETUP_STICK_MODE,
  ITEM_SETUP_MAX
};

void menuGeneralSetup(uint8_t event)
{
#if defined(RTCLOCK)
  struct gtm t;
  gettime(&t);

  if ((m_posVert==ITEM_SETUP_DATE+1 || m_posVert==ITEM_SETUP_TIME+1) && s_editMode>0 && event == EVT_KEY_FIRST(KEY_ENTER)) {
    // set the date and time into RTC chip
    rtcSetTime(&t);
  }
#endif

  MENU(STR_MENURADIOSETUP, menuTabDiag, e_Setup, ITEM_SETUP_MAX+1, {0, IF_RTCLOCK(2) IF_RTCLOCK(2) IF_BATTGRAPH(1) LABEL(SOUND), IF_AUDIO(0) IF_BUZZER(0) 0, IF_AUDIO(0) IF_VOICE(0) IF_HAPTIC(LABEL(HAPTIC)) IF_HAPTIC(0) IF_HAPTIC(0) IF_HAPTIC(0) IF_PCBSKY9X(0) IF_9X(0) LABEL(ALARMS), 0, IF_PCBSKY9X(0) IF_PCBSKY9X(0) 0, 0, 0, IF_ROTARY_ENCODERS(0) LABEL(BACKLIGHT), 0, 0, CASE_PWM_BACKLIGHT(0) CASE_PWM_BACKLIGHT(0) 0, IF_SPLASH(0) IF_FRSKY(0) IF_FRSKY(0) IF_PXX(0) IF_CPUARM(0) 0, LABEL(TX_MODE), CASE_PCBTARANIS(0) 1/*to force edit mode*/});

  uint8_t sub = m_posVert - 1;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    uint8_t y = 1 + 1*FH + i*FH;
    uint8_t k = i+s_pgOfs;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
#if defined(RTCLOCK)
      case ITEM_SETUP_DATE:
        lcd_putsLeft(y, STR_DATE);
        lcd_putc(RADIO_SETUP_DATE_COLUMN, y, '-'); lcd_putc(RADIO_SETUP_DATE_COLUMN+3*FW-1, y, '-');
        for (uint8_t j=0; j<3; j++) {
          uint8_t rowattr = (m_posHorz==j ? attr : 0);
          switch (j) {
            case 0:
              lcd_outdezAtt(RADIO_SETUP_DATE_COLUMN, y, t.tm_year+1900, rowattr);
              if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_year = checkIncDec(event, t.tm_year, 112, 200, 0);
              break;
            case 1:
              lcd_outdezNAtt(RADIO_SETUP_DATE_COLUMN+3*FW-1, y, t.tm_mon+1, rowattr|LEADING0, 2);
              if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_mon = checkIncDec(event, t.tm_mon, 0, 11, 0);
              break;
            case 2:
            {
              int16_t year = 1900 + t.tm_year;
              int8_t dlim = (((((year%4==0) && (year%100!=0)) || (year%400==0)) && (t.tm_mon==1)) ? 1 : 0);
              int8_t dmon[] = {31,28,31,30,31,30,31,31,30,31,30,31}; // TODO in flash
              dlim += dmon[t.tm_mon];
              lcd_outdezNAtt(RADIO_SETUP_DATE_COLUMN+6*FW-2, y, t.tm_mday, rowattr|LEADING0, 2);
              if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_mday = checkIncDec(event, t.tm_mday, 1, dlim, 0);
              break;
            }
          }
        }
#if defined(PCBTARANIS)
        if (attr && m_posHorz < 0) lcd_filled_rect(RADIO_SETUP_2ND_COLUMN, y, LCD_W-RADIO_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH, 8);
#endif
        if (attr && checkIncDec_Ret)
          g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
        break;

      case ITEM_SETUP_TIME:
        lcd_putsLeft(y, STR_TIME);
        lcd_putc(RADIO_SETUP_TIME_COLUMN-1, y, ':'); lcd_putc(RADIO_SETUP_TIME_COLUMN+3*FW-4, y, ':');
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
        lcd_putc(g_eeGeneral.vBatMin >= 10 ? RADIO_SETUP_2ND_COLUMN+2*FW+FWNUM-1 : RADIO_SETUP_2ND_COLUMN+2*FW+FWNUM-FW/2, y, '-');
        putsVolts(RADIO_SETUP_2ND_COLUMN, y,  90+g_eeGeneral.vBatMin, (m_posHorz==0 ? attr : 0)|LEFT|NO_UNIT);
        putsVolts(RADIO_SETUP_2ND_COLUMN+4*FW-2, y, 120+g_eeGeneral.vBatMax, (m_posHorz>0 ? attr : 0)|LEFT|NO_UNIT);
#if defined(PCBTARANIS)
        if (attr && m_posHorz < 0) lcd_filled_rect(RADIO_SETUP_2ND_COLUMN, y, LCD_W-RADIO_SETUP_2ND_COLUMN-MENUS_SCROLLBAR_WIDTH, 8);
#endif
        if (attr && s_editMode>0) {
          if (m_posHorz==0)
            CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMin, -50, g_eeGeneral.vBatMax+20); // min=4.0V
          else
            CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMax, g_eeGeneral.vBatMin-20, +40); // max=16.0V
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

      case ITEM_SETUP_BEEP_LENGTH:
        SLIDER_5POS(y, g_eeGeneral.beepLength, STR_LENGTH, event, attr);
        break;

#if defined(AUDIO)
      case ITEM_SETUP_SPEAKER_PITCH:
        lcd_putsLeft( y, STR_SPKRPITCH);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.speakerPitch, attr|LEFT);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.speakerPitch, 0, 20);
        }
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
        lcd_putsLeft( y, STR_HAPTICSTRENGTH);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.hapticStrength, attr|LEFT);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.hapticStrength, 0, 5);
        }
        break;
#endif

#if defined(PCBSKY9X)
      case ITEM_SETUP_BRIGHTNESS:
        lcd_putsLeft(y, STR_BRIGHTNESS);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, 100-g_eeGeneral.backlightBright, attr|LEFT) ;
        if(attr) {
          uint8_t b = 100 - g_eeGeneral.backlightBright;
          CHECK_INCDEC_GENVAR(event, b, 0, 100);
          g_eeGeneral.backlightBright = 100 - b;
        }
        break;
#endif

#if !defined(PCBTARANIS) && !defined(PCBACT)
      case ITEM_SETUP_CONTRAST:
        lcd_putsLeft(y, STR_CONTRAST);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.contrast, attr|LEFT);
        if(attr) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.contrast, 10, 45);
          lcdSetContrast();
        }
        break;
#endif

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
        putsTelemetryValue(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.mAhWarn*50, UNIT_MAH, attr|LEFT) ;
        if(attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.mAhWarn, 0, 100);
        break;
#endif

#if defined(PCBSKY9X)
      case ITEM_SETUP_TEMPERATURE_WARNING:
        lcd_putsLeft(y, STR_TEMPWARNING);
        putsTelemetryValue(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.temperatureWarn, UNIT_DEGREES, attr|LEFT) ;
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
        uint8_t b = 1-g_eeGeneral.splashMode;
        g_eeGeneral.splashMode = 1 - onoffMenuItem( b, RADIO_SETUP_2ND_COLUMN, y, STR_SPLASHSCREEN, attr, event ) ;
        break;
      }
#endif

#if defined(FRSKY)
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
      case ITEM_SETUP_IMPERIAL:
        g_eeGeneral.imperial = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_UNITSSYSTEM, STR_VUNITSSYSTEM, g_eeGeneral.imperial, 0, 1, attr, event);
        break;
#endif

      case ITEM_SETUP_RX_CHANNEL_ORD:
        lcd_putsLeft(y, STR_RXCHANNELORD); // RAET->AETR
        for (uint8_t i=1; i<=4; i++)
          putsChnLetter(RADIO_SETUP_2ND_COLUMN - FW + i*FW, y, channel_order(i), attr);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.templateSetup, 0, 23);
        break;

      case ITEM_SETUP_STICK_MODE_LABELS:
        lcd_putsLeft(y, NO_INDENT(STR_MODE));
        for (uint8_t i=0; i<4; i++) lcd_img((6+4*i)*FW, y, sticks, i, 0);
        break;

      case ITEM_SETUP_STICK_MODE:
        lcd_putcAtt(2*FW, y, '1'+g_eeGeneral.stickMode, attr);
        for (uint8_t i=0; i<4; i++) putsMixerSource((6+4*i)*FW, y, pgm_read_byte(modn12x3 + 4*g_eeGeneral.stickMode + i), 0);
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
        if (m_posHorz > 0) {
          REPEAT_LAST_CURSOR_MOVE();
        }
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
  return (isfile && !line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcmp(fn, line) > 0);
}

inline bool isFilenameLower(bool isfile, const char * fn, const char * line)
{
  return (!isfile && line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcmp(fn, line) < 0);
}

void onSdManagerMenu(const char *result)
{
  char lfn[SD_SCREEN_FILE_LENGTH];

  uint8_t index = m_posVert-1-s_pgOfs;
  if (result == STR_SD_INFO) {
    pushMenu(menuGeneralSdManagerInfo);
  }
  else if (result == STR_SD_FORMAT) {
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
  else if (result == STR_DELETE_FILE) {
    f_getcwd(lfn, SD_SCREEN_FILE_LENGTH);
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
    f_getcwd(lfn, SD_SCREEN_FILE_LENGTH);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    POPUP_WARNING(eeLoadModelSD(lfn));
  } */
  else if (result == STR_PLAY_FILE) {
    f_getcwd(lfn, SD_SCREEN_FILE_LENGTH);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    audioQueue.playFile(lfn, PLAY_NOW);
  }
#endif
}

void menuGeneralSdManager(uint8_t event)
{
  FILINFO fno;
  DIR dir;
  char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#else
  char lfn[SD_SCREEN_FILE_LENGTH];
#endif

  SIMPLE_MENU(SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD, menuTabDiag, e_Sd, 1+reusableBuffer.sdmanager.count);

  if (s_editMode > 0)
    s_editMode = 0;

  switch(event) {
    case EVT_ENTRY:
      f_chdir(ROOT_PATH);
      reusableBuffer.sdmanager.offset = 65535;
      break;

#if defined(PCBTARANIS)
    case EVT_KEY_BREAK(KEY_ENTER):
#else
    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_ENTER):
#endif
    {
      if (m_posVert > 0) {
        uint8_t index = m_posVert-1-s_pgOfs;
        if (!reusableBuffer.sdmanager.lines[index][SD_SCREEN_FILE_LENGTH+1]) {
          f_chdir(reusableBuffer.sdmanager.lines[index]);
          s_pgOfs = 0;
          m_posVert = 1;
          reusableBuffer.sdmanager.offset = 65535;
          break;
        }
      }
      if (!IS_ROTARY_BREAK(event) || m_posVert==0)
        break;
      // no break;
    }

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(event);
      if (m_posVert == 0) {
        MENU_ADD_ITEM(STR_SD_INFO);
        MENU_ADD_ITEM(STR_SD_FORMAT);
      }
      else {
#if defined(CPUARM)
        uint8_t index = m_posVert-1-s_pgOfs;
        char * ext = reusableBuffer.sdmanager.lines[index];
        ext += strlen(ext) - 4;
        /* TODO if (!strcmp(ext, MODELS_EXT)) {
          s_menu[s_menu_count++] = STR_LOAD_FILE;
        }
        else */ if (!strcmp(ext, SOUNDS_EXT)) {
          MENU_ADD_ITEM(STR_PLAY_FILE);
        }
#endif
        MENU_ADD_ITEM(STR_DELETE_FILE);
        MENU_ADD_ITEM(STR_RENAME_FILE);
        MENU_ADD_ITEM(STR_COPY_FILE);
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
    uint8_t y = 1 + FH + i*FH;
    uint8_t x = 0;
    uint8_t attr = (m_posVert-1-s_pgOfs == i ? BSS|INVERS : BSS);
    if (reusableBuffer.sdmanager.lines[i][0]) {
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) { lcd_putcAtt(0, y, '[', attr); x += FW; }
      lcd_putsAtt(x, y, reusableBuffer.sdmanager.lines[i], attr);
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) { lcd_putcAtt(lcdLastPos, y, ']', attr); }
    }
  }
}
#endif

void menuGeneralTrainer(uint8_t event)
{
  uint8_t y;
  bool slave = SLAVE_MODE();

  MENU(STR_MENUTRAINER, menuTabDiag, e_Trainer, slave ? 1 : 7, {0, 2, 2, 2, 2, 0/*, 0*/});

  if (slave) { // i am the slave
    lcd_puts(7*FW, 3*FH, STR_SLAVE);
  }
  else {
    uint8_t attr;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);

    lcd_puts(3*FW, 1 + 1*FH, STR_MODESRC);

    y = 1 + 2*FH;

    for (uint8_t i=1; i<=NUM_STICKS; i++) {
      uint8_t chan = channel_order(i);
      volatile TrainerMix *td = &g_eeGeneral.trainer.mix[chan-1];

      putsMixerSource(0, y, chan, (m_posVert==i && m_posHorz<0) ? INVERS : 0);

      for (uint8_t j=0; j<3; j++) {

        attr = ((m_posVert==i && m_posHorz==j) ? blink : 0);

        switch(j) {
          case 0:
            lcd_putsiAtt(4*FW, y, STR_TRNMODE, td->mode, attr);
            if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->mode, 0, 2);
            break;

          case 1:
            lcd_outdezAtt(11*FW, y, td->studWeight, attr);
            if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->studWeight, -100, 100);
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
    lcd_putsLeft(6*FH+1, STR_MULTIPLIER);
    lcd_outdezAtt(LEN_MULTIPLIER*FW+3*FW, 6*FH+1, g_eeGeneral.PPM_Multiplier+10, attr|PREC1);
    if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.PPM_Multiplier, -10, 40);

    attr = (m_posVert==6) ? blink : 0;
    lcd_putsAtt(0*FW, 1+7*FH, STR_CAL, attr);
    for (uint8_t i=0; i<4; i++) {
      uint8_t x = (i*8+16)*FW/2;
#if defined (PPM_UNIT_PERCENT_PREC1)
      lcd_outdezAtt(x, 1+7*FH, (g_ppmIns[i]-g_eeGeneral.trainer.calib[i])*2, PREC1);
#else
      lcd_outdezAtt(x, 1+7*FH, (g_ppmIns[i]-g_eeGeneral.trainer.calib[i])/5, 0);
#endif
    }

    if (attr) {
      if (event==EVT_KEY_FIRST(KEY_MENU)){
        s_editMode = -1;
        memcpy(g_eeGeneral.trainer.calib, g_ppmIns, sizeof(g_eeGeneral.trainer.calib));
        eeDirty(EE_GENERAL);
        AUDIO_KEYPAD_UP();
      }
    }
  }
}

void menuGeneralVersion(uint8_t event)
{
  SIMPLE_MENU(STR_MENUVERSION, menuTabDiag, e_Vers, 1);

  lcd_putsLeft(2*FH, stamp1);
  lcd_putsLeft(3*FH, stamp2);
  lcd_putsLeft(4*FH, stamp3);
#if defined(PCBSKY9X) && !defined(REVA)
  if (Coproc_valid == 1) {
     lcd_putsLeft(5*FH, PSTR("CoPr:"));
     lcd_outdez8(10*FW, 5*FH, Coproc_read);
  }
  else {
     lcd_putsLeft(5*FH, PSTR("CoPr: ---"));
  }
#endif  
  lcd_putsLeft(7*FH, eeprom_stamp);
}

void displayKeyState(uint8_t x, uint8_t y, EnumKeys key)
{
  uint8_t t = switchState(key);
  lcd_putcAtt(x, y, t+'0', t ? INVERS : 0);
}

void menuGeneralDiagKeys(uint8_t event)
{
  SIMPLE_MENU(STR_MENUDIAG, menuTabDiag, e_Keys, 1);

  lcd_puts(14*FW, 3*FH, STR_VTRIM);

  for(uint8_t i=0; i<9; i++) {
    uint8_t y = i*FH; //+FH;
#if !defined(PCBTARANIS)
    if(i>(SW_ID0-SW_BASE)) y-=FH; //overwrite ID0
    putsSwitches(8*FW, y, i+1, 0); //ohne off,on
    displayKeyState(11*FW+2, y, (EnumKeys)(SW_BASE+i));
#endif

    if (i<8) {
      y = i/2*FH+FH*4;
      lcd_img(14*FW, y, sticks, i/2, 0);
      displayKeyState(i&1? 20*FW : 18*FW, y, (EnumKeys)(TRM_BASE+i));
    }

    if (i<6) {
      y = (5-i)*FH+2*FH;
      lcd_putsiAtt(0, y, STR_VKEYS, i, 0);
      displayKeyState(5*FW+2, y, (EnumKeys)(KEY_MENU+i));
    }
  }

#if defined(ROTARY_ENCODERS) || defined(ROTARY_ENCODER_NAVIGATION)
  for(uint8_t i=0; i<DIM(g_rotenc); i++) {
    uint8_t y = i*FH + FH;
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

  SIMPLE_MENU(STR_MENUANA, menuTabDiag, e_Ana, ANAS_ITEMS_COUNT);

  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
    uint8_t y = 1+FH+(i/2)*FH;
    uint8_t x = i&1 ? 64+5 : 0;
    putsStrIdx(x, y, PSTR("A"), i+1);
    lcd_putc(x+2*FWNUM, y, ':');
    lcd_outhex4(x+3*FW-1, y, anaIn(i));
    lcd_outdez8(x+10*FW-1, y, (int16_t)calibratedStick[CONVERT_MODE(i+1)-1]*25/256);
  }

#if !defined(CPUARM)
  // Display raw BandGap result (debug)
  lcd_puts(64+5, 1+4*FH, STR_BG);
  lcd_outdezAtt(64+5+6*FW-3, 1+4*FH, BandGap, 0);
#endif

#if defined(PCBSKY9X)
  lcd_putsLeft(5*FH+1, STR_BATT_CALIB);
  static uint32_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(7)) / 8; // running average, sourced directly (to avoid unending debate :P)
  uint32_t batCalV = ( adcBatt + adcBatt*(g_eeGeneral.vBatCalib)/128 ) * 4191 ;
  batCalV /= 55296  ;
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, 5*FH+1, batCalV, (m_posVert==1 ? INVERS : 0));
#elif defined(PCBGRUVIN9X)
  lcd_putsLeft(6*FH-2, STR_BATT_CALIB);
  // Gruvin wants 2 decimal places and instant update of volts calib field when button pressed
  static uint16_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(7)) / 8; // running average, sourced directly (to avoid unending debate :P)
  uint32_t batCalV = ((uint32_t)adcBatt*1390 + (10*(int32_t)adcBatt*g_eeGeneral.vBatCalib)/8) / BandGap;
  lcd_outdezNAtt(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, batCalV, PREC2|(m_posVert==1 ? INVERS : 0));
#elif defined(PCBTARANIS)
  lcd_putsLeft(6*FH+1, STR_BATT_CALIB);
  static uint32_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(8)) / 8; // running average, sourced directly (to avoid unending debate :P)
  uint32_t batCalV = ( adcBatt + adcBatt*(g_eeGeneral.vBatCalib)/128 ) * BATT_SCALE;
  batCalV >>= 11;
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, 6*FH+1, batCalV, (m_posVert==1 ? INVERS : 0));
#else
  lcd_putsLeft(6*FH-2, STR_BATT_CALIB);
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, g_vbat100mV, (m_posVert==1 ? INVERS : 0));
#endif
  if (m_posVert==1) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatCalib, -127, 127);

#if defined(PCBSKY9X) && !defined(REVA)
  lcd_putsLeft(6*FH+1, STR_CURRENT_CALIB);
  putsTelemetryValue(LEN_CALIB_FIELDS*FW+4*FW, 6*FH+1, getCurrent(), UNIT_MILLIAMPS, (m_posVert==2 ? INVERS : 0)) ;
  if (m_posVert==2) CHECK_INCDEC_GENVAR(event, g_eeGeneral.currentCalib, -49, 49);
#endif

#if defined(PCBSKY9X)
  lcd_putsLeft(7*FH+1, STR_TEMP_CALIB);
  putsTelemetryValue(LEN_CALIB_FIELDS*FW+4*FW, 7*FH+1, getTemperature(), UNIT_DEGREES, (m_posVert==3 ? INVERS : 0)) ;
  if (m_posVert==3) CHECK_INCDEC_GENVAR(event, g_eeGeneral.temperatureCalib, -100, 100);
#endif
}

#if defined(PCBSKY9X)
enum menuGeneralHwItems {
  ITEM_SETUP_HW_OPTREX_DISPLAY,
  ITEM_SETUP_HW_STICKS_GAINS_LABELS,
  ITEM_SETUP_HW_STICK_LV_GAIN,
  ITEM_SETUP_HW_STICK_LH_GAIN,
  ITEM_SETUP_HW_STICK_RV_GAIN,
  ITEM_SETUP_HW_STICK_RH_GAIN,
  ITEM_SETUP_HW_ROTARY_ENCODER,
  IF_BLUETOOTH(ITEM_SETUP_HW_BT_BAUDRATE)
  ITEM_SETUP_HW_MAX
};

#define GENERAL_HW_PARAM_OFS (2+(15*FW))
void menuGeneralHardware(uint8_t event)
{
  MENU(STR_HARDWARE, menuTabDiag, e_Hardware, ITEM_SETUP_HW_MAX+1, {0, 0, (uint8_t)-1, 0, 0, 0, 0, IF_BLUETOOTH(0)});

  uint8_t sub = m_posVert - 1;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    uint8_t y = 1 + 1*FH + i*FH;
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

      case ITEM_SETUP_HW_ROTARY_ENCODER:
        g_eeGeneral.rotarySteps = selectMenuItem(GENERAL_HW_PARAM_OFS, y, PSTR("Rotary Encoder"), PSTR("\0062steps4steps"), g_eeGeneral.rotarySteps, 0, 1, attr, event);
        break;

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


void menuCommonCalib(uint8_t event)
{
  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) { //get low and high vals for sticks and trims
    int16_t vt = anaIn(i);
    reusableBuffer.calib.loVals[i] = min(vt, reusableBuffer.calib.loVals[i]);
    reusableBuffer.calib.hiVals[i] = max(vt, reusableBuffer.calib.hiVals[i]);
    if (i >= NUM_STICKS) {
      reusableBuffer.calib.midVals[i] = (reusableBuffer.calib.hiVals[i] + reusableBuffer.calib.loVals[i]) / 2;
    }
  }

  s_noScroll = reusableBuffer.calib.state; // make sure we don't scroll while calibrating

  switch(event)
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
      lcd_putsLeft(3*FH, STR_MENUTOSTART);
      break;

    case 1:
      // SET MIDPOINT
      lcd_putsAtt(0*FW, 2*FH, STR_SETMIDPOINT, s_noScroll ? INVERS : 0);
      lcd_putsLeft(3*FH, STR_MENUWHENDONE);

      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
        reusableBuffer.calib.loVals[i] = 15000;
        reusableBuffer.calib.hiVals[i] = -15000;
        reusableBuffer.calib.midVals[i] = anaIn(i);
      }
      break;

    case 2:
      // MOVE STICKS/POTS
      lcd_putsAtt(0*FW, 2*FH, STR_MOVESTICKSPOTS, s_noScroll ? INVERS : 0);
      lcd_putsLeft(3*FH, STR_MENUWHENDONE);

      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
        if (abs(reusableBuffer.calib.loVals[i]-reusableBuffer.calib.hiVals[i])>50) {
          g_eeGeneral.calibMid[i] = reusableBuffer.calib.midVals[i];
          int16_t v = reusableBuffer.calib.midVals[i] - reusableBuffer.calib.loVals[i];
          g_eeGeneral.calibSpanNeg[i] = v - v/STICK_TOLERANCE;
          v = reusableBuffer.calib.hiVals[i] - reusableBuffer.calib.midVals[i];
          g_eeGeneral.calibSpanPos[i] = v - v/STICK_TOLERANCE;
        }
      }
      break;

    case 3:
      g_eeGeneral.chkSum = evalChkSum();
      STORE_GENERALVARS;
      reusableBuffer.calib.state = 4;
      break;

    default:
      reusableBuffer.calib.state = 0;
      break;
  }

  doMainScreenGraphics();
#if defined(PCBTARANIS)
  drawPotsBars();
#endif
}

void menuGeneralCalib(uint8_t event)
{
  SIMPLE_MENU(STR_MENUCALIBRATION, menuTabDiag, e_Calib, 1);

  menuCommonCalib(event);
}

void menuFirstCalib(uint8_t event)
{
  if (event == EVT_KEY_BREAK(KEY_EXIT) || reusableBuffer.calib.state == 4) {
    chainMenu(menuMainView);
  }
  else {
    lcd_putsCenter(0*FH, MENUCALIBRATION);
    lcd_invert_line(0);
    menuCommonCalib(event);
  }
}
