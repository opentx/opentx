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

#define ISTR(x) LEN_##x TR_##x

// The non-0-terminated-strings
const pm_char STR_OPEN9X[]  =
    ISTR(OFFON)
    ISTR(MMMINV)
    ISTR(NCHANNELS)
#if !defined(GRAPHICS)
    ISTR(VBEEPLEN)
#endif
    ISTR(VBEEPMODE)
#if defined(ROTARY_ENCODERS)
    ISTR(VRENAVIG)
#endif
#if defined(ROTARY_ENCODER_NAVIGATION)
    ISTR(VRENCODERS)
#endif
    ISTR(TRNMODE)
    ISTR(TRNCHN)
#if defined(PCBTARANIS) || defined(PCBHORUS)
    ISTR(UART3MODES)
    ISTR(SWTYPES)
    ISTR(POTTYPES)
    ISTR(SLIDERTYPES)
#endif
    ISTR(VTRIMINC)
    ISTR(VDISPLAYTRIMS)
    ISTR(RETA123)
    ISTR(VPROTOS)
    ISTR(POSNEG)
#if defined(PCBSKY9X) && defined(REVX)
    ISTR(VOUTPUT_TYPE)
#endif
    ISTR(VBLMODE)
    ISTR(VCURVEFUNC)
    ISTR(VMLTPX)
    ISTR(VMLTPX2)
    ISTR(VMIXTRIMS)
    ISTR(VCSWFUNC)
    ISTR(VFSWFUNC)
    ISTR(VFSWRESET)
    ISTR(FUNCSOUNDS)
    ISTR(VTELEMUNIT)
    ISTR(VALARM)
    ISTR(VALARMFN)
    ISTR(VTELPROTO)
    ISTR(GPSFORMAT)
    ISTR(AMPSRC)
    ISTR(VARIOSRC)
    ISTR(VTELEMSCREENTYPE)
#if defined(HELI)
    ISTR(VSWASHTYPE)
#endif
    ISTR(VKEYS)
    ISTR(VSWITCHES)
    ISTR(VSRCRAW)
#if defined(TRANSLATIONS_CZ)
    ISTR(INPUTNAMES)
#endif
    ISTR(VTMRMODES)
    ISTR(DATETIME)
    ISTR(VPERSISTENT)
    ISTR(VLCD)
    ISTR(VUNITSSYSTEM)
    ISTR(VBEEPCOUNTDOWN)
    ISTR(VVARIOCENTER)
    ISTR(COUNTRYCODES)
    ISTR(USBMODES)
    ISTR(VFAILSAFE)
    ISTR(VTRAINERMODES)
    ISTR(TARANIS_PROTOCOLS)
    ISTR(R9M_REGION)
    ISTR(R9M_FCC_POWER_VALUES)
    ISTR(R9M_LBT_POWER_VALUES)
    ISTR(TELEMETRY_PROTOCOLS)
    ISTR(XJT_PROTOCOLS)
    ISTR(DSM_PROTOCOLS)
#if defined(MULTIMODULE)
    ISTR(MULTI_PROTOCOLS)
#endif
    ISTR(VOLTSRC)
    ISTR(CURVE_TYPES)
    ISTR(VSENSORTYPES)
    ISTR(VFORMULAS)
    ISTR(VPREC)
    ISTR(VCELLINDEX)
#if defined(BLUETOOTH)
    ISTR(BLUETOOTH_MODES)
#endif
    ISTR(VANTENNATYPES)
    ;

// The 0-terminated-strings
const pm_char STR_POPUPS[]  = TR_POPUPS;
#if !defined(OFS_EXIT)
  const pm_char STR_EXIT[]  = TR_EXIT;
#endif
#if !defined(PCBHORUS)
  const pm_char STR_MENUWHENDONE[]  = TR_MENUWHENDONE;
#endif
const pm_char STR_FREE[]  = TR_FREE;
const pm_char STR_DELETEMODEL[]  = TR_DELETEMODEL;
const pm_char STR_COPYINGMODEL[]  = TR_COPYINGMODEL;
const pm_char STR_MOVINGMODEL[]  = TR_MOVINGMODEL;
const pm_char STR_LOADINGMODEL[]  = TR_LOADINGMODEL;
const pm_char STR_NAME[]  = TR_NAME;
const pm_char STR_BITMAP[]  = TR_BITMAP;
const pm_char STR_TIMER[]  = TR_TIMER;
const pm_char STR_ELIMITS[]  = TR_ELIMITS;
const pm_char STR_ETRIMS[]  = TR_ETRIMS;
const pm_char STR_TRIMINC[]  = TR_TRIMINC;
const pm_char STR_DISPLAY_TRIMS[]  = TR_DISPLAY_TRIMS;
const pm_char STR_TTRACE[]  = TR_TTRACE;
const pm_char STR_TTRIM[]  = TR_TTRIM;
const pm_char STR_BEEPCTR[]  = TR_BEEPCTR;
const pm_char STR_USE_GLOBAL_FUNCS[]  = TR_USE_GLOBAL_FUNCS;
#if defined(PCBSKY9X) && defined(REVX)
  const pm_char STR_OUTPUT_TYPE[]  = TR_OUTPUT_TYPE;
#endif
const pm_char STR_PROTO[]  = TR_PROTO;
const pm_char STR_PPMFRAME[]  = TR_PPMFRAME;
const pm_char STR_REFRESHRATE[]  = TR_REFRESHRATE;
const pm_char SSTR_WARN_BATTVOLTAGE[]  = STR_WARN_BATTVOLTAGE;
const pm_char STR_MS[]  = TR_MS;
const pm_char STR_SWITCH[]  = TR_SWITCH;
const pm_char STR_TRIMS[]  = TR_TRIMS;
const pm_char STR_FADEIN[]  = TR_FADEIN;
const pm_char STR_FADEOUT[]  = TR_FADEOUT;
const pm_char STR_DEFAULT[]  = TR_DEFAULT;
const pm_char STR_CHECKTRIMS[]  = TR_CHECKTRIMS;
#ifdef HELI
const pm_char STR_SWASHTYPE[]  = TR_SWASHTYPE;
const pm_char STR_COLLECTIVE[]  = TR_COLLECTIVE;
const pm_char STR_AILERON[]  = TR_AILERON;
const pm_char STR_ELEVATOR[]  = TR_ELEVATOR;
const pm_char STR_SWASHRING[]  = TR_SWASHRING;
const pm_char STR_ELEDIRECTION[]  = TR_ELEDIRECTION;
const pm_char STR_AILDIRECTION[]  = TR_AILDIRECTION;
const pm_char STR_COLDIRECTION[]  = TR_COLDIRECTION;
#endif
const pm_char STR_MODE[]  = TR_MODE;
#if defined(AUDIO) && defined(BUZZER)
const pm_char STR_SPEAKER[]  = TR_SPEAKER;
const pm_char STR_BUZZER[]  = TR_BUZZER;
#endif
const pm_char STR_NOFREEEXPO[]  = TR_NOFREEEXPO;
const pm_char STR_NOFREEMIXER[]  = TR_NOFREEMIXER;
const pm_char STR_SOURCE[]  = TR_SOURCE;
const pm_char STR_WEIGHT[]  = TR_WEIGHT;
const pm_char STR_EXPO[]  = TR_EXPO;
const pm_char STR_SIDE[]  = TR_SIDE;
const pm_char STR_DIFFERENTIAL[]  = TR_DIFFERENTIAL;
const pm_char STR_OFFSET[]  = TR_OFFSET;
const pm_char STR_TRIM[]  = TR_TRIM;
const pm_char STR_DREX[]  = TR_DREX;
const pm_char STR_CURVE[]  = TR_CURVE;
const pm_char STR_FLMODE[]  = TR_FLMODE;
const pm_char STR_MIXWARNING[]  = TR_MIXWARNING;
const pm_char STR_OFF[]  = TR_OFF;
const pm_char STR_MULTPX[]  = TR_MULTPX;
const pm_char STR_DELAYDOWN[]  = TR_DELAYDOWN;
const pm_char STR_DELAYUP[]  = TR_DELAYUP;
const pm_char STR_SLOWDOWN[]  = TR_SLOWDOWN;
const pm_char STR_SLOWUP[]  = TR_SLOWUP;
const pm_char STR_MIXER[]  = TR_MIXER;
const pm_char STR_CV[]  = TR_CV;
const pm_char STR_GV[]  = TR_GV;
const pm_char STR_ACHANNEL[]  = TR_ACHANNEL;
const pm_char STR_RANGE[]  = TR_RANGE;
const pm_char STR_CENTER[]  = TR_CENTER;
const pm_char STR_BAR[]  = TR_BAR;
const pm_char STR_ALARM[]  = TR_ALARM;
const pm_char STR_USRDATA[]  = TR_USRDATA;
const pm_char STR_BLADES[]  = TR_BLADES;
const pm_char STR_SCREEN[]  = TR_SCREEN;
const pm_char STR_SOUND_LABEL[]  = TR_SOUND_LABEL;
const pm_char STR_LENGTH[]  = TR_LENGTH;
const pm_char STR_BEEP_LENGTH[]  = TR_BEEP_LENGTH;
#if defined(AUDIO)
const pm_char STR_SPKRPITCH[]  = TR_SPKRPITCH;
#endif
#if defined(HAPTIC)
const pm_char STR_HAPTIC_LABEL[]  = TR_HAPTIC_LABEL;
const pm_char STR_HAPTICSTRENGTH[]  = TR_HAPTICSTRENGTH;
#endif
const pm_char STR_CONTRAST[]  = TR_CONTRAST;
const pm_char STR_ALARMS_LABEL[]  = TR_ALARMS_LABEL;
const pm_char STR_BATTERY_RANGE[]  = TR_BATTERY_RANGE;
const pm_char STR_BATTERYWARNING[]  = TR_BATTERYWARNING;
const pm_char STR_INACTIVITYALARM[]  = TR_INACTIVITYALARM;
const pm_char STR_MEMORYWARNING[]  = TR_MEMORYWARNING;
const pm_char STR_ALARMWARNING[]  = TR_ALARMWARNING;
const pm_char STR_RSSISHUTDOWNALARM[]  = TR_RSSISHUTDOWNALARM;
const pm_char STR_MODEL_STILL_POWERED[]  = TR_MODEL_STILL_POWERED;
const pm_char STR_MODEL_SHUTDOWN[]  = TR_MODEL_SHUTDOWN;
const pm_char STR_PRESS_ENTER_TO_CONFIRM[]  = TR_PRESS_ENTER_TO_CONFIRM;
#if defined(ROTARY_ENCODERS)
const pm_char STR_RENAVIG[]  = TR_RENAVIG;
#endif
const pm_char STR_THROTTLEREVERSE[]  = TR_THROTTLEREVERSE;
const pm_char STR_TIMER_NAME[]  = TR_TIMER_NAME;
const pm_char STR_MINUTEBEEP[]  = TR_MINUTEBEEP;
const pm_char STR_BEEPCOUNTDOWN[]  = TR_BEEPCOUNTDOWN;
const pm_char STR_PERSISTENT[]  = TR_PERSISTENT;
const pm_char STR_BACKLIGHT_LABEL[]  = TR_BACKLIGHT_LABEL;
const pm_char STR_BLDELAY[]  = TR_BLDELAY;

#if defined(PWM_BACKLIGHT) || defined(PCBHORUS)
const pm_char STR_BLONBRIGHTNESS[]  = TR_BLONBRIGHTNESS;
const pm_char STR_BLOFFBRIGHTNESS[]  = TR_BLOFFBRIGHTNESS;
#endif

const pm_char STR_SPLASHSCREEN[]  = TR_SPLASHSCREEN;
const pm_char STR_THROTTLEWARNING[]  = TR_THROTTLEWARNING;
const pm_char STR_SWITCHWARNING[]  = TR_SWITCHWARNING;
const pm_char STR_POTWARNINGSTATE[]  = TR_POTWARNINGSTATE;
const pm_char STR_POTWARNING[]  = TR_POTWARNING;
const pm_char STR_SLIDERWARNING[]  = TR_SLIDERWARNING;
#ifdef TELEMETRY_FRSKY
const pm_char STR_TIMEZONE[]  = TR_TIMEZONE;
const pm_char STR_ADJUST_RTC[]  = TR_ADJUST_RTC;
const pm_char STR_GPS[]  = TR_GPS;
const pm_char STR_GPSCOORD[]  = TR_GPSCOORD;
const pm_char STR_VARIO[]  = TR_VARIO;
const pm_char STR_PITCH_AT_ZERO[]  = TR_PITCH_AT_ZERO;
const pm_char STR_PITCH_AT_MAX[]  = TR_PITCH_AT_MAX;
const pm_char STR_REPEAT_AT_ZERO[]  = TR_REPEAT_AT_ZERO;
#endif
const pm_char STR_RXCHANNELORD[]  = TR_RXCHANNELORD;
const pm_char STR_STICKS[]  = TR_STICKS;
const pm_char STR_POTS[]  = TR_POTS;
const pm_char STR_SWITCHES[]  = TR_SWITCHES;
const pm_char STR_SWITCHES_DELAY[]  = TR_SWITCHES_DELAY;
const pm_char STR_SLAVE[]  = TR_SLAVE;
const pm_char STR_MODESRC[]  = TR_MODESRC;
const pm_char STR_MULTIPLIER[]  = TR_MULTIPLIER;
const pm_char STR_CAL[]  = TR_CAL;
const pm_char STR_VTRIM[]  = TR_VTRIM;
const pm_char STR_BG[]  = TR_BG;
const pm_char STR_MENUTOSTART[]  = TR_MENUTOSTART;
const pm_char STR_SETMIDPOINT[]  = TR_SETMIDPOINT;
const pm_char STR_MOVESTICKSPOTS[]  = TR_MOVESTICKSPOTS;
const pm_char STR_RXBATT[]  = TR_RXBATT;
const pm_char STR_TX[]  = TR_TXnRX;
const pm_char STR_ACCEL[]  = TR_ACCEL;
const pm_char STR_NODATA[]  = TR_NODATA;
const pm_char STR_TOTTM1TM2THRTHP[]  = TR_TOTTM1TM2THRTHP;
const pm_char STR_TMR1LATMAXUS[]  = TR_TMR1LATMAXUS;
const pm_char STR_TMR1LATMINUS[]  = TR_TMR1LATMINUS;
const pm_char STR_TMR1JITTERUS[]  = TR_TMR1JITTERUS;
const pm_char STR_TMIXMAXMS[]  = TR_TMIXMAXMS;
const pm_char STR_FREESTACKMINB[]  = TR_FREESTACKMINB;
const pm_char STR_MENUTORESET[]  = TR_MENUTORESET;
const pm_char STR_PPM_TRAINER[]  = TR_PPM_TRAINER;
const pm_char STR_CH[]  = TR_CH;
const pm_char STR_MODEL[]  = TR_MODEL;
const pm_char STR_FP[]  = TR_FP;
const pm_char STR_MIX[]  = TR_MIX;
const pm_char STR_ALERT[]  = TR_ALERT;
const pm_char STR_PRESSANYKEYTOSKIP[]  = TR_PRESSANYKEYTOSKIP;
const pm_char STR_THROTTLENOTIDLE[]  = TR_THROTTLENOTIDLE;
const pm_char STR_ALARMSDISABLED[]  = TR_ALARMSDISABLED;
const pm_char STR_PRESSANYKEY[]  = TR_PRESSANYKEY;
const pm_char STR_TRIMS2OFFSETS[]  = TR_TRIMS2OFFSETS;
const pm_char STR_OUTPUTS2FAILSAFE[]  = TR_OUTPUTS2FAILSAFE;
const pm_char STR_MENURADIOSETUP[]  = TR_MENURADIOSETUP;

#if defined(EEPROM)
const pm_char STR_BAD_RADIO_DATA[]  = TR_BADEEPROMDATA;
const pm_char STR_STORAGE_FORMAT[]  = TR_EEPROMFORMATTING;
const pm_char STR_STORAGE_WARNING[]  = TR_EEPROMWARN;
const pm_char STR_EEPROM_CONVERTING[]  = TR_EEPROM_CONVERTING;
const pm_char STR_EEPROMLOWMEM[]  = TR_EEPROMLOWMEM;
const pm_char STR_EEPROMOVERFLOW[]  = TR_EEPROMOVERFLOW;
#else
const pm_char STR_BAD_RADIO_DATA[]  = TR_BAD_RADIO_DATA;
const pm_char STR_STORAGE_WARNING[]  = TR_STORAGE_WARNING;
const pm_char STR_STORAGE_FORMAT[]  = TR_STORAGE_FORMAT;
#endif

const pm_char STR_FAS_OFFSET[]  = TR_FAS_OFFSET;

const pm_char STR_MENUDATEANDTIME[]  = TR_MENUDATEANDTIME;

const pm_char STR_MENUTRAINER[]  = TR_MENUTRAINER;
const pm_char STR_MENUSPECIALFUNCS[]  = TR_MENUSPECIALFUNCS;
const pm_char STR_MENUVERSION[]  = TR_MENUVERSION;
const pm_char STR_MENU_RADIO_SWITCHES[]  = TR_MENU_RADIO_SWITCHES;
const pm_char STR_MENU_RADIO_ANALOGS[]  = TR_MENU_RADIO_ANALOGS;
const pm_char STR_MENUCALIBRATION[]  = TR_MENUCALIBRATION;

const pm_char STR_MENUMODELSEL[]  = TR_MENUMODELSEL;
const pm_char STR_MENUSETUP[]  = TR_MENUSETUP;
const pm_char STR_MENUFLIGHTMODE[]  = TR_MENUFLIGHTMODE;
const pm_char STR_MENUFLIGHTMODES[]  = TR_MENUFLIGHTMODES;

#ifdef HELI
const pm_char STR_MENUHELISETUP[]  = TR_MENUHELISETUP;
#endif

const pm_char STR_MENUINPUTS[]  = TR_MENUINPUTS;
const pm_char STR_MENULIMITS[]  = TR_MENULIMITS;
const pm_char STR_MENUCURVES[]  = TR_MENUCURVES;
const pm_char STR_MENUCURVE[]  = TR_MENUCURVE;
const pm_char STR_MENULOGICALSWITCH[]  = TR_MENULOGICALSWITCH;
const pm_char STR_MENULOGICALSWITCHES[]  = TR_MENULOGICALSWITCHES;
const pm_char STR_MENUCUSTOMFUNC[]  = TR_MENUCUSTOMFUNC;

#if defined(LUA)
const pm_char STR_MENUCUSTOMSCRIPTS[]  = TR_MENUCUSTOMSCRIPTS;
#endif

#if defined(TELEMETRY_FRSKY)
const pm_char STR_MENUTELEMETRY[]  = TR_MENUTELEMETRY;
const pm_char STR_LIMIT[]  = TR_LIMIT;
#endif


const pm_char STR_MENUSTAT[]  = TR_MENUSTAT;
const pm_char STR_MENUDEBUG[]  = TR_MENUDEBUG;
const char * const STR_MONITOR_CHANNELS[] = { TR_MONITOR_CHANNELS1, TR_MONITOR_CHANNELS2, TR_MONITOR_CHANNELS3, TR_MONITOR_CHANNELS4 };
const pm_char STR_MONITOR_SWITCHES[]  = TR_MONITOR_SWITCHES;
const pm_char STR_MONITOR_OUTPUT_DESC[]  = TR_MONITOR_OUTPUT_DESC;
const pm_char STR_MONITOR_MIXER_DESC[]  = TR_MONITOR_MIXER_DESC;
const pm_char STR_MENUGLOBALVARS[]  = TR_MENUGLOBALVARS;

#if defined(DSM2) || defined(PXX)
const pm_char STR_RECEIVER_NUM[]  = TR_RECEIVER_NUM;
const pm_char STR_RECEIVER[]  = TR_RECEIVER;
const pm_char STR_REBIND[]  = TR_REBIND;
#endif

const pm_char STR_SYNCMENU[]  = TR_SYNCMENU;
const pm_char STR_INTERNALRF[]  = TR_INTERNALRF;
const pm_char STR_EXTERNALRF[]  = TR_EXTERNALRF;
const pm_char STR_MODULE_TELEMETRY[]  = TR_MODULE_TELEMETRY;
const pm_char STR_MODULE_TELEM_ON[]  = TR_MODULE_TELEM_ON;
const pm_char STR_COUNTRYCODE[]  = TR_COUNTRYCODE;
const pm_char STR_USBMODE[]  = TR_USBMODE;
const pm_char STR_FAILSAFE[]  = TR_FAILSAFE;
const pm_char STR_FAILSAFESET[]  = TR_FAILSAFESET;
const pm_char STR_HOLD[]  = TR_HOLD;
const pm_char STR_NONE[]  = TR_NONE;
const pm_char STR_MENUSENSOR[]  = TR_MENUSENSOR;
const pm_char STR_SENSOR[]  = TR_SENSOR;
const pm_char STR_DISABLE_INTERNAL[]  = TR_DISABLE_INTERNAL;

const pm_char STR_INVERT_THR[]  = TR_INVERT_THR;
const pm_char STR_AND_SWITCH[]  = TR_AND_SWITCH;
const pm_char STR_SF[]  = TR_SF;
const pm_char STR_GF[]  = TR_GF;

#if defined(FRSKY_HUB)
const pm_char STR_MINRSSI[]  = TR_MINRSSI;
const pm_char STR_LATITUDE[]  = TR_LATITUDE;
const pm_char STR_LONGITUDE[]  = TR_LONGITUDE;
#endif

const pm_char STR_SHUTDOWN[]  = TR_SHUTDOWN;
const pm_char STR_SAVEMODEL[]  = TR_SAVEMODEL;

#if defined(PCBX9E)
const pm_char STR_POWEROFF[]  = TR_POWEROFF;
#endif

const pm_char STR_BATT_CALIB[]  = TR_BATT_CALIB;

const pm_char STR_VOLTAGE[]  = TR_VOLTAGE;
const pm_char STR_CURRENT[]  = TR_CURRENT;

const pm_char STR_CURRENT_CALIB[]  = TR_CURRENT_CALIB;
const pm_char STR_UNITSSYSTEM[]    = TR_UNITSSYSTEM;
const pm_char STR_VOICELANG[]  = TR_VOICELANG;
const pm_char STR_MODELIDUSED[]  = TR_MODELIDUSED;
const pm_char STR_BEEP_VOLUME[]  = INDENT TR_BEEP_VOLUME;
const pm_char STR_WAV_VOLUME[]  = INDENT TR_WAV_VOLUME;
const pm_char STR_BG_VOLUME[]  = INDENT TR_BG_VOLUME;
const pm_char STR_PERSISTENT_MAH[]  = TR_PERSISTENT_MAH;

#if defined(NAVIGATION_MENUS)
const pm_char STR_SELECT_MODEL[]  = TR_SELECT_MODEL;
const pm_char STR_CREATE_CATEGORY[]  = TR_CREATE_CATEGORY;
const pm_char STR_RENAME_CATEGORY[]  = TR_RENAME_CATEGORY;
const pm_char STR_DELETE_CATEGORY[]  = TR_DELETE_CATEGORY;
const pm_char STR_CREATE_MODEL[]  = TR_CREATE_MODEL;
const pm_char STR_DUPLICATE_MODEL[]  = TR_DUPLICATE_MODEL;
const pm_char STR_COPY_MODEL[]  = TR_COPY_MODEL;
const pm_char STR_MOVE_MODEL[]  = TR_MOVE_MODEL;
const pm_char STR_DELETE_MODEL[]  = TR_DELETE_MODEL;
const pm_char STR_EDIT[]  = TR_EDIT;
const pm_char STR_INSERT_BEFORE[]  = TR_INSERT_BEFORE;
const pm_char STR_INSERT_AFTER[]  = TR_INSERT_AFTER;
const pm_char STR_COPY[]  = TR_COPY;
const pm_char STR_MOVE[]  = TR_MOVE;
const pm_char STR_PASTE[]  = TR_PASTE;
const pm_char STR_INSERT[]  = TR_INSERT;
const pm_char STR_DELETE[]  = TR_DELETE;
const pm_char STR_RESET_FLIGHT[]  = TR_RESET_FLIGHT;
const pm_char STR_RESET_TIMER1[]  = TR_RESET_TIMER1;
const pm_char STR_RESET_TIMER2[]  = TR_RESET_TIMER2;
const pm_char STR_RESET_TIMER3[]  = TR_RESET_TIMER3;
const pm_char STR_RESET_TELEMETRY[]  = TR_RESET_TELEMETRY;
const pm_char STR_STATISTICS[]  = TR_STATISTICS;
const pm_char STR_ABOUT_US[]  = TR_ABOUT_US;
const pm_char STR_USB_JOYSTICK[]  = TR_USB_JOYSTICK;
const pm_char STR_USB_MASS_STORAGE[]  = TR_USB_MASS_STORAGE;
const pm_char STR_USB_SERIAL[]  = TR_USB_SERIAL;
const pm_char STR_SETUP_SCREENS[]  = TR_SETUP_SCREENS;
const pm_char STR_MONITOR_SCREENS[]  = TR_MONITOR_SCREENS;
#endif

#if defined(MULTIMODULE)
const pm_char STR_MULTI_CUSTOM[]  = TR_MULTI_CUSTOM;
const pm_char STR_MULTI_RFTUNE[]  = TR_MULTI_RFTUNE;
const pm_char STR_MULTI_TELEMETRY[]  = TR_MULTI_TELEMETRY;
const pm_char STR_MULTI_VIDFREQ[]  = TR_MULTI_VIDFREQ;
const pm_char STR_MULTI_OPTION[]  = TR_MULTI_OPTION;
const pm_char STR_MULTI_AUTOBIND[]  = TR_MULTI_AUTOBIND;
const pm_char STR_MULTI_DSM_AUTODTECT[]  = TR_MULTI_DSM_AUTODTECT;
const pm_char STR_MULTI_LOWPOWER[]  = TR_MULTI_LOWPOWER;
const pm_char STR_MODULE_NO_SERIAL_MODE[]  = TR_MODULE_NO_SERIAL_MODE;
const pm_char STR_MODULE_NO_INPUT[]  = TR_MODULE_NO_INPUT;
const pm_char STR_MODULE_WAITFORBIND[]  = TR_MODULE_WAITFORBIND;
const pm_char STR_MODULE_NO_TELEMETRY[]  = TR_MODULE_NO_TELEMETRY;
const pm_char STR_MODULE_BINDING[]  = TR_MODULE_BINDING;
const pm_char STR_PROTOCOL_INVALID[]  = TR_PROTOCOL_INVALID;
const pm_char STR_MODULE_STATUS[]  = TR_MODULE_STATUS;
const pm_char STR_MODULE_SYNC[]  = TR_MODULE_SYNC;
const pm_char STR_MULTI_SERVOFREQ[]  = TR_MULTI_SERVOFREQ;
#if LCD_W < 212
const pm_char STR_SUBTYPE[]  = TR_SUBTYPE;
#endif
#endif

const pm_char STR_RESET_BTN[]  = TR_RESET_BTN;

#if defined(SDCARD)
const pm_char STR_BACKUP_MODEL[]  = TR_BACKUP_MODEL;
const pm_char STR_RESTORE_MODEL[]  = TR_RESTORE_MODEL;
const pm_char STR_DELETE_ERROR[]  = TR_DELETE_ERROR;
const pm_char STR_SDCARD_ERROR[]  = TR_SDCARD_ERROR;
const pm_char STR_NO_SDCARD[]  = TR_NO_SDCARD;
const pm_char STR_SDCARD_FULL[]  = TR_SDCARD_FULL;
const pm_char STR_INCOMPATIBLE[]  = TR_INCOMPATIBLE;
const pm_char STR_LOGS_PATH[]  = LOGS_PATH;
const pm_char STR_LOGS_EXT[]  = LOGS_EXT;
const pm_char STR_MODELS_PATH[]  = MODELS_PATH;
const pm_char STR_MODELS_EXT[]  = MODELS_EXT;
#endif

const pm_char STR_CAT_NOT_EMPTY[]  = TR_CAT_NOT_EMPTY;
const pm_char STR_WARNING[]  = TR_WARNING;
const pm_char STR_THROTTLEWARN[]  = TR_THROTTLEWARN;
const pm_char STR_ALARMSWARN[]  = TR_ALARMSWARN;
const pm_char STR_SWITCHWARN[]  = TR_SWITCHWARN;
const pm_char STR_FAILSAFEWARN[]  = TR_FAILSAFEWARN;
#if defined(NIGHTLY_BUILD_WARNING)
const pm_char STR_NIGHTLY_WARNING[]  = TR_NIGHTLY_WARNING;
const pm_char STR_NIGHTLY_NOTSAFE[]  = TR_NIGHTLY_NOTSAFE;
#endif
const pm_char STR_WRONG_SDCARDVERSION[]  = TR_WRONG_SDCARDVERSION;
const pm_char STR_WRONG_PCBREV[]  = TR_WRONG_PCBREV;
const pm_char STR_EMERGENCY_MODE[]  = TR_EMERGENCY_MODE;
const pm_char STR_PCBREV_ERROR[]  = TR_PCBREV_ERROR;
const pm_char STR_NO_FAILSAFE[]  = TR_NO_FAILSAFE;
const pm_char STR_KEYSTUCK[]  = TR_KEYSTUCK;

const pm_char STR_SPEAKER_VOLUME[]  = TR_SPEAKER_VOLUME;
const pm_char STR_LCD[]  = TR_LCD;
const pm_char STR_BRIGHTNESS[]  = TR_BRIGHTNESS;
const pm_char STR_CPU_TEMP[]  = TR_CPU_TEMP;
const pm_char STR_CPU_CURRENT[]  = TR_CPU_CURRENT;
const pm_char STR_CPU_MAH[]  = TR_CPU_MAH;
const pm_char STR_COPROC[]  = TR_COPROC;
const pm_char STR_COPROC_TEMP[]  = TR_COPROC_TEMP;
const pm_char STR_TEMPWARNING[]  = TR_TEMPWARNING;
const pm_char STR_CAPAWARNING[]  = TR_CAPAWARNING;
const pm_char STR_FUNC[]  = TR_FUNC;
const pm_char STR_V1[]  = TR_V1;
const pm_char STR_V2[]  = TR_V2;
const pm_char STR_DURATION[]  = TR_DURATION;
const pm_char STR_DELAY[]  = TR_DELAY;
const pm_char STR_SD_CARD[]  = TR_SD_CARD;
const pm_char STR_SDHC_CARD[]  = TR_SDHC_CARD;
const pm_char STR_NO_SOUNDS_ON_SD[]  = TR_NO_SOUNDS_ON_SD;
const pm_char STR_NO_MODELS_ON_SD[]  = TR_NO_MODELS_ON_SD;
const pm_char STR_NO_BITMAPS_ON_SD[]  = TR_NO_BITMAPS_ON_SD;
const pm_char STR_NO_SCRIPTS_ON_SD[]  = TR_NO_SCRIPTS_ON_SD;
const pm_char STR_SCRIPT_SYNTAX_ERROR[]  = TR_SCRIPT_SYNTAX_ERROR;
const pm_char STR_SCRIPT_PANIC[]  = TR_SCRIPT_PANIC;
const pm_char STR_SCRIPT_KILLED[]  = TR_SCRIPT_KILLED;
const pm_char STR_SCRIPT_ERROR[]  = TR_SCRIPT_ERROR;
const pm_char STR_PLAY_FILE[]  = TR_PLAY_FILE;
const pm_char STR_ASSIGN_BITMAP[]  = TR_ASSIGN_BITMAP;
#if defined(PCBHORUS)
const pm_char STR_ASSIGN_SPLASH[]  = TR_ASSIGN_SPLASH;
#endif
const pm_char STR_EXECUTE_FILE[]  = TR_EXECUTE_FILE;
const pm_char STR_DELETE_FILE[]  = TR_DELETE_FILE;
const pm_char STR_COPY_FILE[]  = TR_COPY_FILE;
const pm_char STR_RENAME_FILE[]  = TR_RENAME_FILE;
const pm_char STR_SD_INFO[]  = TR_SD_INFO;
const pm_char STR_SD_FORMAT[]  = TR_SD_FORMAT;
const pm_char STR_REMOVED[]  = TR_REMOVED;
const pm_char STR_NA[]  = TR_NA;
const pm_char STR_HARDWARE[]  = TR_HARDWARE;
const pm_char STR_FORMATTING[]  = TR_FORMATTING;
const pm_char STR_TEMP_CALIB[]  = TR_TEMP_CALIB;
const pm_char STR_TIME[]  = TR_TIME;
const pm_char STR_MAXBAUDRATE[]  = TR_MAXBAUDRATE;
const pm_char STR_BAUDRATE[]  = TR_BAUDRATE;
const pm_char STR_SD_INFO_TITLE[]  = TR_SD_INFO_TITLE;
const pm_char STR_SD_TYPE[]  = TR_SD_TYPE;
const pm_char STR_SD_SPEED[]  = TR_SD_SPEED;
const pm_char STR_SD_SECTORS[]  = TR_SD_SECTORS;
const pm_char STR_SD_SIZE[]  = TR_SD_SIZE;
const pm_char STR_TYPE[]  = TR_TYPE;
const pm_char STR_GLOBAL_VARS[]  = TR_GLOBAL_VARS;
const pm_char STR_GVARS[]  = TR_GVARS;
const pm_char STR_GLOBAL_VAR[]  = TR_GLOBAL_VAR;
const pm_char STR_OWN[]  = TR_OWN;
const pm_char STR_ROTARY_ENCODER[]  = TR_ROTARY_ENCODER;
const pm_char STR_DATE[]  = TR_DATE;
const pm_char STR_CHANNELS_MONITOR[]  = TR_CHANNELS_MONITOR;
const pm_char STR_MIXERS_MONITOR[]  = TR_MIXERS_MONITOR;
const pm_char STR_PATH_TOO_LONG[]  = TR_PATH_TOO_LONG;
const pm_char STR_VIEW_TEXT[]  = TR_VIEW_TEXT;
const pm_char STR_FLASH_BOOTLOADER[]  = TR_FLASH_BOOTLOADER;
const pm_char STR_FLASH_INTERNAL_MODULE[]  = TR_FLASH_INTERNAL_MODULE;
const pm_char STR_FLASH_EXTERNAL_MODULE[]  = TR_FLASH_EXTERNAL_MODULE;
const pm_char STR_FIRMWARE_UPDATE_ERROR[]  = TR_FIRMWARE_UPDATE_ERROR;
const pm_char STR_FLASH_EXTERNAL_DEVICE[]  = TR_FLASH_EXTERNAL_DEVICE;
const pm_char STR_WRITING[]  = TR_WRITING;
const pm_char STR_CONFIRM_FORMAT[]  = TR_CONFIRM_FORMAT;
const pm_char STR_EEBACKUP[]  = TR_EEBACKUP;
const pm_char STR_FACTORYRESET[]  = TR_FACTORYRESET;
const pm_char STR_CONFIRMRESET[]  = TR_CONFIRMRESET;
const pm_char STR_TOO_MANY_LUA_SCRIPTS[]  = TR_TO_MANY_LUA_SCRIPTS;
const pm_char STR_BLCOLOR[]   = TR_BLCOLOR;

  const pm_char STR_MODELNAME[]  = TR_MODELNAME;
  const pm_char STR_PHASENAME[]  = TR_PHASENAME;
  const pm_char STR_MIXNAME[]  = TR_MIXNAME;
  const pm_char STR_INPUTNAME[]  = TR_INPUTNAME;
  const pm_char STR_EXPONAME[]  = TR_EXPONAME;

#if LCD_W >= 212
  const char * const STR_PHASES_HEADERS[] = TR_PHASES_HEADERS;
  const char * const STR_LIMITS_HEADERS[] = TR_LIMITS_HEADERS;
  const char * const STR_LSW_HEADERS[] = TR_LSW_HEADERS;
  const char * const STR_LSW_DESCRIPTIONS[] = TR_LSW_DESCRIPTIONS;
  const char * const STR_GVAR_HEADERS[] = TR_GVAR_HEADERS;
#endif

  const pm_char STR_TRAINER[]  = TR_TRAINER;
  const pm_char STR_MODULE_BIND[]   = TR_MODULE_BIND;
  const pm_char STR_BINDING_1_8_TELEM_ON[]  = TR_BINDING_CH1_8_TELEM_ON;
  const pm_char STR_BINDING_1_8_TELEM_OFF[]  = TR_BINDING_CH1_8_TELEM_OFF;
  const pm_char STR_BINDING_9_16_TELEM_ON[]  = TR_BINDING_CH9_16_TELEM_ON;
  const pm_char STR_BINDING_9_16_TELEM_OFF[]  = TR_BINDING_CH9_16_TELEM_OFF;
  const pm_char STR_CHANNELRANGE[]  = TR_CHANNELRANGE;
  const pm_char STR_ANTENNASELECTION[]  = TR_ANTENNASELECTION;
  const pm_char STR_ANTENNACONFIRM1[]  = TR_ANTENNACONFIRM1;
  const pm_char STR_ANTENNACONFIRM2[]  = TR_ANTENNACONFIRM2;
  const pm_char STR_SET[]  = TR_SET;
  const pm_char STR_PREFLIGHT[]  = TR_PREFLIGHT;
  const pm_char STR_CHECKLIST[]  = TR_CHECKLIST;
  const pm_char STR_VIEW_NOTES[]  = TR_VIEW_NOTES;
  const pm_char STR_MODEL_SELECT[]  = TR_MODEL_SELECT;
  const pm_char STR_RESET_SUBMENU[]  = TR_RESET_SUBMENU;
  const pm_char STR_LOWALARM[]  = TR_LOWALARM;
  const pm_char STR_CRITICALALARM[]  = TR_CRITICALALARM;
  const pm_char STR_RSSIALARM_WARN[]  = TR_RSSIALARM_WARN;
  const pm_char STR_NO_RSSIALARM[]  = TR_NO_RSSIALARM;
  const pm_char STR_DISABLE_ALARM[]  = TR_DISABLE_ALARM;
  const pm_char STR_TELEMETRY_TYPE[]  = TR_TELEMETRY_TYPE;
  const pm_char STR_TELEMETRY_SENSORS[]  = TR_TELEMETRY_SENSORS;
  const pm_char STR_VALUE[]  = TR_VALUE;
  const pm_char STR_TOPLCDTIMER[]  = TR_TOPLCDTIMER;
  const pm_char STR_UNIT[]  = TR_UNIT;
  const pm_char STR_TELEMETRY_NEWSENSOR[]  = TR_TELEMETRY_NEWSENSOR;
  const pm_char STR_ID[]  = TR_ID;
  const pm_char STR_PRECISION[]  = TR_PRECISION;
  const pm_char STR_RATIO[]  = TR_RATIO;
  const pm_char STR_FORMULA[]  = TR_FORMULA;
  const pm_char STR_CELLINDEX[]  = TR_CELLINDEX;
  const pm_char STR_LOGS[]  = TR_LOGS;
  const pm_char STR_OPTIONS[]  = TR_OPTIONS;
  const pm_char STR_ALTSENSOR[]  = TR_ALTSENSOR;
  const pm_char STR_CELLSENSOR[]  = TR_CELLSENSOR;
  const pm_char STR_GPSSENSOR[]  = TR_GPSSENSOR;
  const pm_char STR_CURRENTSENSOR[]  = TR_CURRENTSENSOR;
  const pm_char STR_AUTOOFFSET[]  = TR_AUTOOFFSET;
  const pm_char STR_ONLYPOSITIVE[]  = TR_ONLYPOSITIVE;
  const pm_char STR_FILTER[]  = TR_FILTER;
  const pm_char STR_TELEMETRYFULL[]  = TR_TELEMETRYFULL;
  const pm_char STR_SERVOS_OK[]  = TR_SERVOS_OK;
  const pm_char STR_SERVOS_KO[]  = TR_SERVOS_KO;
  const pm_char STR_INVERTED_SERIAL[]  = TR_INVERTED_SERIAL;
  const pm_char STR_IGNORE_INSTANCE[]  = TR_IGNORE_INSTANCE;
  const pm_char STR_DISCOVER_SENSORS[]  = TR_DISCOVER_SENSORS;
  const pm_char STR_STOP_DISCOVER_SENSORS[]  = TR_STOP_DISCOVER_SENSORS;
  const pm_char STR_DELETE_ALL_SENSORS[]  = TR_DELETE_ALL_SENSORS;
  const pm_char STR_CONFIRMDELETE[]  = TR_CONFIRMDELETE;
  const pm_char STR_SELECT_WIDGET[]  = TR_SELECT_WIDGET;
  const pm_char STR_REMOVE_WIDGET[]  = TR_REMOVE_WIDGET;
  const pm_char STR_WIDGET_SETTINGS[]  = TR_WIDGET_SETTINGS;
  const pm_char STR_REMOVE_SCREEN[]  = TR_REMOVE_SCREEN;
  const pm_char STR_SETUP_WIDGETS[]  = TR_SETUP_WIDGETS;
  const pm_char STR_USER_INTERFACE[]  = TR_USER_INTERFACE;
  const pm_char STR_THEME[]  = TR_THEME;
  const pm_char STR_SETUP[]  = TR_SETUP;
  const pm_char STR_MAINVIEWX[]  = TR_MAINVIEWX;
  const pm_char STR_LAYOUT[]  = TR_LAYOUT;
  const pm_char STR_ADDMAINVIEW[]  = TR_ADDMAINVIEW;
  const pm_char STR_BACKGROUND_COLOR[]  = TR_BACKGROUND_COLOR;
  const pm_char STR_MAIN_COLOR[]  = TR_MAIN_COLOR;
  const pm_char STR_TEXT_VIEWER[]  = TR_TEXT_VIEWER;
  const pm_char STR_MULTI_RFPOWER[]  = TR_MULTI_RFPOWER;

  const pm_char STR_BYTES[]  = TR_BYTES;
  const pm_char STR_ANTENNAPROBLEM[]  = TR_ANTENNAPROBLEM;
  const pm_char STR_MODULE[]  = TR_MODULE;
  const pm_char STR_ENABLE_POPUP[]  = TR_ENABLE_POPUP;
  const pm_char STR_DISABLE_POPUP[]  = TR_DISABLE_POPUP;
  const pm_char STR_POPUP[]  = TR_POPUP;
  const pm_char STR_MIN[]  = TR_MIN;
  const pm_char STR_MAX[]  = TR_MAX;
  const pm_char STR_CURVE_PRESET[]  = TR_CURVE_PRESET;
  const pm_char STR_PRESET[]  = TR_PRESET;
  const pm_char STR_MIRROR[]  = TR_MIRROR;
  const pm_char STR_CLEAR[]  = TR_CLEAR;
  const pm_char STR_RESET[]  = TR_RESET;
  const pm_char STR_COUNT[]  = TR_COUNT;
  const pm_char STR_PT[]  = TR_PT;
  const pm_char STR_PTS[]  = TR_PTS;
  const pm_char STR_SMOOTH[]  = TR_SMOOTH;
  const pm_char STR_COPY_STICKS_TO_OFS[]  = TR_COPY_STICKS_TO_OFS;
  const pm_char STR_COPY_TRIMS_TO_OFS[]  = TR_COPY_TRIMS_TO_OFS;
  const pm_char STR_INCDEC[]  = TR_INCDEC;
  const pm_char STR_GLOBALVAR[]  = TR_GLOBALVAR;
  const pm_char STR_MIXSOURCE[]  = TR_MIXSOURCE;
  const pm_char STR_CONSTANT[]  = TR_CONSTANT;
  const pm_char STR_TOP_BAR[]  = TR_TOP_BAR;
  const pm_char STR_ALTITUDE[]  = TR_ALTITUDE;
  const pm_char STR_SCALE[]  = TR_SCALE;
  const pm_char STR_VIEW_CHANNELS[]  = TR_VIEW_CHANNELS;
  const pm_char STR_UART3MODE[]  = TR_UART3MODE;
  const pm_char STR_THROTTLE_LABEL[]  = TR_THROTTLE_LABEL;
  const pm_char STR_SCRIPT[]  = TR_SCRIPT;
  const pm_char STR_INPUTS[]  = TR_INPUTS;
  const pm_char STR_OUTPUTS[]  = TR_OUTPUTS;
  const pm_char STR_MENU_INPUTS[]  = TR_MENU_INPUTS;
  const pm_char STR_MENU_LUA[]  = TR_MENU_LUA;
  const pm_char STR_MENU_STICKS[]  = TR_MENU_STICKS;
  const pm_char STR_MENU_POTS[]  = TR_MENU_POTS;
  const pm_char STR_MENU_MAX[]  = TR_MENU_MAX;
  const pm_char STR_MENU_HELI[]  = TR_MENU_HELI;
  const pm_char STR_MENU_TRIMS[]  = TR_MENU_TRIMS;
  const pm_char STR_MENU_SWITCHES[]  = TR_MENU_SWITCHES;
  const pm_char STR_MENU_LOGICAL_SWITCHES[]  = TR_MENU_LOGICAL_SWITCHES;
  const pm_char STR_MENU_TRAINER[]  = TR_MENU_TRAINER;
  const pm_char STR_MENU_CHANNELS[]  = TR_MENU_CHANNELS;
  const pm_char STR_MENU_GVARS[]  = TR_MENU_GVARS;
  const pm_char STR_MENU_TELEMETRY[]  = TR_MENU_TELEMETRY;
  const pm_char STR_MENU_DISPLAY[]  = TR_MENU_DISPLAY;
  const pm_char STR_MENU_OTHER[]  = TR_MENU_OTHER;
  const pm_char STR_MENU_INVERT[]  = TR_MENU_INVERT;
  const pm_char STR_JITTER_FILTER[]  = TR_JITTER_FILTER;

#if MENUS_LOCK == 1
  const pm_char STR_UNLOCKED[]  = TR_UNLOCKED;
  const pm_char STR_MODS_FORBIDDEN[]  = TR_MODS_FORBIDDEN;
#endif

#if defined(PCBTARANIS) || defined(DSM2)
  const pm_char STR_MODULE_RANGE[]  = TR_MODULE_RANGE;
#endif

#if defined(BLUETOOTH)
  const pm_char STR_BLUETOOTH[]  = TR_BLUETOOTH;
  const pm_char STR_BLUETOOTH_DISC[]  = TR_BLUETOOTH_DISC;
  const pm_char STR_BLUETOOTH_INIT[]  = TR_BLUETOOTH_INIT;
  const pm_char STR_BLUETOOTH_DIST_ADDR[]  = TR_BLUETOOTH_DIST_ADDR;
  const pm_char STR_BLUETOOTH_LOCAL_ADDR[]  = TR_BLUETOOTH_LOCAL_ADDR;
  const pm_char STR_BLUETOOTH_PIN_CODE[]  = TR_BLUETOOTH_PIN_CODE;
#endif


  const pm_char STR_ABOUTUS[]  = TR_ABOUTUS;
  const pm_char STR_ABOUT_OPENTX_1[]  = TR_ABOUT_OPENTX_1;
  const pm_char STR_ABOUT_OPENTX_2[]  = TR_ABOUT_OPENTX_2;
  const pm_char STR_ABOUT_OPENTX_3[]  = TR_ABOUT_OPENTX_3;
  const pm_char STR_ABOUT_OPENTX_4[]  = TR_ABOUT_OPENTX_4;
  const pm_char STR_ABOUT_OPENTX_5[]  = TR_ABOUT_OPENTX_5;

  const pm_char STR_ABOUT_BERTRAND_1[]  = TR_ABOUT_BERTRAND_1;
  const pm_char STR_ABOUT_BERTRAND_2[]  = TR_ABOUT_BERTRAND_2;
  const pm_char STR_ABOUT_BERTRAND_3[]  = TR_ABOUT_BERTRAND_3;

  const pm_char STR_ABOUT_MIKE_1[]  = TR_ABOUT_MIKE_1;
  const pm_char STR_ABOUT_MIKE_2[]  = TR_ABOUT_MIKE_2;
  const pm_char STR_ABOUT_MIKE_3[]  = TR_ABOUT_MIKE_3;
  const pm_char STR_ABOUT_MIKE_4[]  = TR_ABOUT_MIKE_4;

  const pm_char STR_ABOUT_ROMOLO_1[]  = TR_ABOUT_ROMOLO_1;
  const pm_char STR_ABOUT_ROMOLO_2[]  = TR_ABOUT_ROMOLO_2;
  const pm_char STR_ABOUT_ROMOLO_3[]  = TR_ABOUT_ROMOLO_3;

  const pm_char STR_ABOUT_ANDRE_1[]  = TR_ABOUT_ANDRE_1;
  const pm_char STR_ABOUT_ANDRE_2[]  = TR_ABOUT_ANDRE_2;
  const pm_char STR_ABOUT_ANDRE_3[]  = TR_ABOUT_ANDRE_3;

  const pm_char STR_ABOUT_ROB_1[]  = TR_ABOUT_ROB_1;
  const pm_char STR_ABOUT_ROB_2[]  = TR_ABOUT_ROB_2;

  const pm_char STR_ABOUT_MARTIN_1[]  = TR_ABOUT_MARTIN_1;
  const pm_char STR_ABOUT_MARTIN_2[]  = TR_ABOUT_MARTIN_2;

  const pm_char STR_ABOUT_KJELL_1[]  = TR_ABOUT_KJELL_1;
  const pm_char STR_ABOUT_KJELL_2[]  = TR_ABOUT_KJELL_2;
  const pm_char STR_ABOUT_KJELL_3[]  = TR_ABOUT_KJELL_3;
  const pm_char STR_ABOUT_KJELL_4[]  = TR_ABOUT_KJELL_4;

  const pm_char STR_ABOUT_HARDWARE_1[]  = TR_ABOUT_HARDWARE_1;
  const pm_char STR_ABOUT_HARDWARE_2[]  = TR_ABOUT_HARDWARE_2;
  const pm_char STR_ABOUT_HARDWARE_3[]  = TR_ABOUT_HARDWARE_3;

  const pm_char STR_ABOUT_PARENTS_1[]  = TR_ABOUT_PARENTS_1;
  const pm_char STR_ABOUT_PARENTS_2[]  = TR_ABOUT_PARENTS_2;
  const pm_char STR_ABOUT_PARENTS_3[]  = TR_ABOUT_PARENTS_3;
  const pm_char STR_ABOUT_PARENTS_4[]  = TR_ABOUT_PARENTS_4;
