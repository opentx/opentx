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

#include "../../opentx.h"

#define RADIO_SETUP_2ND_COLUMN  220

#define SLIDER_5POS(y, value, label, event, attr) { \
  int8_t tmp = value; \
  displaySlider(RADIO_SETUP_2ND_COLUMN, y, 2+tmp, 4, attr); \
  value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, NULL, tmp, -2, +2, attr, event); \
}

#if defined(SPLASH) && !defined(FSPLASH)
  #define CASE_SPLASH_PARAM(x) x,
#else
  #define CASE_SPLASH_PARAM(x)
#endif

enum menuGeneralSetupItems {
  CASE_RTCLOCK(ITEM_SETUP_DATE)
  CASE_RTCLOCK(ITEM_SETUP_TIME)
  // ITEM_SETUP_BATT_RANGE,
  ITEM_SETUP_SOUND_LABEL,
  ITEM_SETUP_BEEP_MODE,
  ITEM_SETUP_SPEAKER_VOLUME,
  ITEM_SETUP_BEEP_VOLUME,
  ITEM_SETUP_BEEP_LENGTH,
  ITEM_SETUP_SPEAKER_PITCH,
  ITEM_SETUP_WAV_VOLUME,
  ITEM_SETUP_BACKGROUND_VOLUME,
  CASE_VARIO_CPUARM(ITEM_SETUP_VARIO_LABEL)
  CASE_VARIO_CPUARM(ITEM_SETUP_VARIO_VOLUME)
  CASE_VARIO_CPUARM(ITEM_SETUP_VARIO_PITCH)
  CASE_VARIO_CPUARM(ITEM_SETUP_VARIO_RANGE)
  CASE_VARIO_CPUARM(ITEM_SETUP_VARIO_REPEAT)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_LABEL)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_MODE)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_LENGTH)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_STRENGTH)
  // ITEM_SETUP_CONTRAST,
  ITEM_SETUP_ALARMS_LABEL,
  ITEM_SETUP_BATTERY_WARNING,
  ITEM_SETUP_INACTIVITY_ALARM,
  // ITEM_SETUP_MEMORY_WARNING,
  ITEM_SETUP_ALARM_WARNING,
  // ITEM_SETUP_BACKLIGHT_LABEL,
  // ITEM_SETUP_BACKLIGHT_MODE,
  // ITEM_SETUP_BACKLIGHT_DELAY,
  // ITEM_SETUP_BRIGHTNESS,
  // ITEM_SETUP_FLASH_BEEP,
  // CASE_SPLASH_PARAM(ITEM_SETUP_DISABLE_SPLASH)
  CASE_GPS(ITEM_SETUP_TIMEZONE)
  CASE_GPS(ITEM_SETUP_GPSFORMAT)
  CASE_PXX(ITEM_SETUP_COUNTRYCODE)
  ITEM_SETUP_LANGUAGE,
  ITEM_SETUP_IMPERIAL,
  // IF_FAI_CHOICE(ITEM_SETUP_FAI)
  // CASE_MAVLINK(ITEM_MAVLINK_BAUD)
  ITEM_SETUP_SWITCHES_DELAY,
  ITEM_SETUP_RX_CHANNEL_ORD,
  ITEM_SETUP_STICK_MODE_LABELS,
  ITEM_SETUP_STICK_MODE,
  ITEM_SETUP_MAX
};

void menuGeneralSetup(evt_t event)
{
#if defined(RTCLOCK)
  struct gtm t;
  gettime(&t);

  if ((m_posVert==ITEM_SETUP_DATE || m_posVert==ITEM_SETUP_TIME) &&
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

  MENU(STR_MENURADIOSETUP, menuTabGeneral, e_Setup, ITEM_SETUP_MAX, DEFAULT_SCROLLBAR_X, { 2|NAVIGATION_LINE_BY_LINE, 2|NAVIGATION_LINE_BY_LINE, LABEL(SOUND), 0, 0, 0, 0, 0, 0, 0, CASE_VARIO_CPUARM(LABEL(VARIO)) CASE_VARIO_CPUARM(0) CASE_VARIO_CPUARM(0) CASE_VARIO_CPUARM(0) CASE_VARIO_CPUARM(0) CASE_HAPTIC(LABEL(HAPTIC)) CASE_HAPTIC(0) CASE_HAPTIC(0) CASE_HAPTIC(0) LABEL(ALARMS), 0, 0, 0, CASE_GPS(0) CASE_GPS(0) CASE_PXX(0) 0, 0, CASE_MAVLINK(0) 0, 0, LABEL(TX_MODE), 0, 1/*to force edit mode*/ });

  int sub = m_posVert;

  for (int i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    int k = i+s_pgOfs;
    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_SETUP_DATE:
      {
        lcd_putsLeft(y, STR_DATE);
        LcdFlags flags = 0;
        if (attr && m_posHorz < 0) {
          flags |= INVERS;
          lcdDrawFilledRect(RADIO_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN, 85, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }
        lcd_putsAtt(RADIO_SETUP_2ND_COLUMN+28, y, "-", flags);
        lcd_putsAtt(RADIO_SETUP_2ND_COLUMN+53, y, "-", flags);
        for (uint8_t j=0; j<3; j++) {
          uint8_t rowattr = (m_posHorz==j ? attr : 0);
          switch (j) {
            case 0:
              lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, t.tm_year+1900, LEFT|flags|rowattr);
              if (rowattr && s_editMode>0) t.tm_year = checkIncDec(event, t.tm_year, 112, 200, 0);
              break;
            case 1:
              lcd_outdezNAtt(RADIO_SETUP_2ND_COLUMN+36, y, t.tm_mon+1, LEFT|flags|rowattr|LEADING0, 2);
              if (rowattr && s_editMode>0) t.tm_mon = checkIncDec(event, t.tm_mon, 0, 11, 0);
              break;
            case 2:
            {
              int16_t year = 1900 + t.tm_year;
              int8_t dlim = (((((year%4==0) && (year%100!=0)) || (year%400==0)) && (t.tm_mon==1)) ? 1 : 0);
              static const pm_uint8_t dmon[] PROGMEM = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
              dlim += pgm_read_byte(&dmon[t.tm_mon]);
              lcd_outdezNAtt(RADIO_SETUP_2ND_COLUMN+61, y, t.tm_mday, LEFT|flags|rowattr|LEADING0, 2);
              if (rowattr && s_editMode>0) t.tm_mday = checkIncDec(event, t.tm_mday, 1, dlim, 0);
              break;
            }
          }
        }
        if (attr && checkIncDec_Ret) {
          g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
        }
        break;
      }

      case ITEM_SETUP_TIME:
      {
        lcd_putsLeft(y, STR_TIME);
        LcdFlags flags = 0;
        if (attr && m_posHorz < 0) {
          flags |= INVERS;
          lcdDrawFilledRect(RADIO_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN, 85, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }
        lcd_putsAtt(RADIO_SETUP_2ND_COLUMN+15, y, ":", flags);
        lcd_putsAtt(RADIO_SETUP_2ND_COLUMN+35, y, ":", flags);
        for (uint8_t j=0; j<3; j++) {
          uint8_t rowattr = (m_posHorz==j ? attr : 0);
          switch (j) {
            case 0:
              lcd_outdezNAtt(RADIO_SETUP_2ND_COLUMN, y, t.tm_hour, flags|rowattr|LEFT|LEADING0, 2);
              if (rowattr && s_editMode>0) t.tm_hour = checkIncDec(event, t.tm_hour, 0, 23, 0);
              break;
            case 1:
              lcd_outdezNAtt(RADIO_SETUP_2ND_COLUMN+20, y, t.tm_min, flags|rowattr|LEFT|LEADING0, 2);
              if (rowattr && s_editMode>0) t.tm_min = checkIncDec(event, t.tm_min, 0, 59, 0);
              break;
            case 2:
              lcd_outdezNAtt(RADIO_SETUP_2ND_COLUMN+40, y, t.tm_sec, flags|rowattr|LEFT|LEADING0, 2);
              if (rowattr && s_editMode>0) t.tm_sec = checkIncDec(event, t.tm_sec, 0, 59, 0);
              break;
          }
        }
        if (attr && checkIncDec_Ret)
          g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
        break;
      }

#if 0
      case ITEM_SETUP_BATT_RANGE:
      {
        lcd_putsLeft(y, STR_BATTERY_RANGE);
        LcdFlags color = WHITE;
        if (attr && m_posHorz < 0) {
          color = BLACK;
          lcdDrawFilledRect(RADIO_SETUP_2ND_COLUMN-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN, 90, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
        }
        putsVolts(RADIO_SETUP_2ND_COLUMN, y, 90+g_eeGeneral.vBatMin, color|(m_posHorz==0 ? attr : 0)|LEFT|NO_UNIT);
        lcd_puts(RADIO_SETUP_2ND_COLUMN+20, y, "-");
        putsVolts(RADIO_SETUP_2ND_COLUMN+28, y, 120+g_eeGeneral.vBatMax, color|(m_posHorz>0 ? attr : 0)|LEFT|NO_UNIT);
        if (attr && s_editMode>0) {
          if (m_posHorz==0)
            CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMin, -50, g_eeGeneral.vBatMax+29); // min=4.0V
          else
            CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMax, g_eeGeneral.vBatMin-29, +40); // max=16.0V
        }
        break;
      }
#endif

      case ITEM_SETUP_SOUND_LABEL:
        lcd_putsLeft(y, STR_SOUND_LABEL);
        break;

      case ITEM_SETUP_BEEP_MODE:
        g_eeGeneral.beepMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_SPEAKER, STR_VBEEPMODE, g_eeGeneral.beepMode, -2, 1, attr, event);
#if defined(FRSKY)
        if (attr && checkIncDec_Ret) frskySendAlarms();
#endif
        break;

      case ITEM_SETUP_SPEAKER_VOLUME:
      {
        lcd_putsLeft(y, STR_SPEAKER_VOLUME);
        uint8_t b = g_eeGeneral.speakerVolume+VOLUME_LEVEL_DEF;
        displaySlider(RADIO_SETUP_2ND_COLUMN, y, b, VOLUME_LEVEL_MAX, attr);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, b, 0, VOLUME_LEVEL_MAX);
          if (checkIncDec_Ret) {
            g_eeGeneral.speakerVolume = (int8_t)b-VOLUME_LEVEL_DEF;
          }
        }
        break;
      }

      case ITEM_SETUP_BEEP_VOLUME:
        SLIDER_5POS(y, g_eeGeneral.beepVolume, STR_BEEP_VOLUME, event, attr);
        break;
      case ITEM_SETUP_WAV_VOLUME:
        SLIDER_5POS(y, g_eeGeneral.wavVolume, STR_WAV_VOLUME, event, attr);
        break;
      case ITEM_SETUP_BACKGROUND_VOLUME:
        SLIDER_5POS(y, g_eeGeneral.backgroundVolume, STR_BG_VOLUME, event, attr);
        break;

      case ITEM_SETUP_BEEP_LENGTH:
        SLIDER_5POS(y, g_eeGeneral.beepLength, STR_BEEP_LENGTH, event, attr);
        break;

      case ITEM_SETUP_SPEAKER_PITCH:
        lcd_putsLeft( y, STR_SPKRPITCH);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.speakerPitch*15, attr|LEFT, "Hz", "+");
        if (attr) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.speakerPitch, 0, 20);
        }
        break;

#if defined(VARIO)
      case ITEM_SETUP_VARIO_LABEL:
        lcd_putsLeft(y, STR_VARIO);
        break;
      case ITEM_SETUP_VARIO_VOLUME:
        SLIDER_5POS(y, g_eeGeneral.varioVolume, TR_SPEAKER_VOLUME, event, attr);
        break;
      case ITEM_SETUP_VARIO_PITCH:
        lcd_putsLeft(y, STR_PITCH_AT_ZERO);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, VARIO_FREQUENCY_ZERO+(g_eeGeneral.varioPitch*10), attr|LEFT, "Hz");
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.varioPitch, -40, 40);
        break;
      case ITEM_SETUP_VARIO_RANGE:
        lcd_putsLeft(y, STR_PITCH_AT_MAX);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, VARIO_FREQUENCY_ZERO+(g_eeGeneral.varioPitch*10)+VARIO_FREQUENCY_RANGE+(g_eeGeneral.varioRange*10), attr|LEFT, "Hz");
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.varioRange, -80, 80);
        break;
      case ITEM_SETUP_VARIO_REPEAT:
        lcd_putsLeft(y, STR_REPEAT_AT_ZERO);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, VARIO_REPEAT_ZERO+(g_eeGeneral.varioRepeat*10), attr|LEFT, STR_MS);
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

#if 0
      case ITEM_SETUP_CONTRAST:
        lcd_putsLeft(y, STR_CONTRAST);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.contrast, attr|LEFT);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.contrast, CONTRAST_MIN, CONTRAST_MAX);
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
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatWarn, 27, 42); // 2.7-4.2V
        break;

#if 0
      case ITEM_SETUP_MEMORY_WARNING:
      {
        uint8_t b = 1-g_eeGeneral.disableMemoryWarning;
        g_eeGeneral.disableMemoryWarning = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_MEMORYWARNING, attr, event);
        break;
      }
#endif

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
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.inactivityTimer, attr|LEFT, "m");
        if (attr) g_eeGeneral.inactivityTimer = checkIncDec(event, g_eeGeneral.inactivityTimer, 0, 250, EE_GENERAL); //0..250minutes
        break;

#if 0
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
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.lightAutoOff*5, attr|LEFT, "s");
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.lightAutoOff, 0, 600/5);
        break;

      case ITEM_SETUP_BRIGHTNESS:
        lcd_putsLeft(y, STR_BRIGHTNESS);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, 100-g_eeGeneral.backlightBright, attr|LEFT) ;
        if (attr) {
          uint8_t b = 100 - g_eeGeneral.backlightBright;
          CHECK_INCDEC_GENVAR(event, b, 0, 100);
          g_eeGeneral.backlightBright = 100 - b;
        }
        break;

      case ITEM_SETUP_BACKLIGHT_COLOR:
        lcd_putsLeft(y, STR_BLCOLOR);
        displaySlider(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.backlightColor, 20, attr);
        if (attr) g_eeGeneral.backlightColor = checkIncDec(event, g_eeGeneral.backlightColor, 0, 20, EE_GENERAL | NO_INCDEC_MARKS);
        break;
#endif

#if 0
      case ITEM_SETUP_DISABLE_SPLASH:
      {
        lcd_putsLeft(y, STR_SPLASHSCREEN);
        if (SPLASH_NEEDED()) {
          lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, SPLASH_TIMEOUT/100, attr|LEFT, "s");
        }
        else {
          lcd_putsiAtt(RADIO_SETUP_2ND_COLUMN, y, STR_MMMINV, 0, attr); // TODO define
        }
        if (attr) g_eeGeneral.splashMode = -checkIncDecGen(event, -g_eeGeneral.splashMode, -3, 4);
        break;
      }
#endif

      case ITEM_SETUP_TIMEZONE:
        lcd_putsLeft(y, STR_TIMEZONE);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.timezone, attr|LEFT);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.timezone, -12, 12);
        break;

      case ITEM_SETUP_GPSFORMAT:
        g_eeGeneral.gpsFormat = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_GPSCOORD, STR_GPSFORMAT, g_eeGeneral.gpsFormat, 0, 1, attr, event);
        break;

      case ITEM_SETUP_COUNTRYCODE:
        g_eeGeneral.countryCode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_COUNTRYCODE, STR_COUNTRYCODES, g_eeGeneral.countryCode, 0, 2, attr, event);
        break;

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

#if 0
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

      case ITEM_SETUP_SWITCHES_DELAY:
        lcd_putsLeft(y, STR_SWITCHES_DELAY);
        lcd_outdezAtt(RADIO_SETUP_2ND_COLUMN, y, 10*SWITCHES_DELAY(), attr|LEFT, STR_MS);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.switchesDelay, -15, +15);
        break;

      case ITEM_SETUP_RX_CHANNEL_ORD:
      {
        lcd_putsLeft(y, STR_RXCHANNELORD); // RAET->AETR
        char s[5];
        for (uint8_t i=0; i<4; i++) {
          s[i] = STR_RETA123[channel_order(i+1)];
        }
        s[4] = '\0';
        lcd_putsAtt(RADIO_SETUP_2ND_COLUMN, y, s, attr);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.templateSetup, 0, 23);
        break;
      }

      case ITEM_SETUP_STICK_MODE_LABELS:
        lcd_putsLeft(y, NO_INDENT(STR_MODE));
        for (int i=0; i<4; i++) {
          // lcdDrawBitmap((6+4*i)*12, y, ...);
        }
        break;

      case ITEM_SETUP_STICK_MODE:
      {
        char s[2] = " ";
        s[0] = '1'+g_eeGeneral.stickMode;
        lcd_putsAtt(INDENT_WIDTH, y, s, attr);
        for (uint8_t i=0; i<4; i++) {
          putsMixerSource((6+4*i)*12, y, MIXSRC_Rud + pgm_read_byte(modn12x3 + 4*g_eeGeneral.stickMode + i));
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
        break;
      }
    }
  }
}
