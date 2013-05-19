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

#ifndef translations_h
#define translations_h

#if defined(TRANSLATIONS_FR)
#include "translations/fr.h"
#define LEN_SPECIAL_CHARS 3
#elif defined(TRANSLATIONS_IT)
#include "translations/it.h"
#define LEN_SPECIAL_CHARS 1
#elif defined(TRANSLATIONS_SE)
#include "translations/se.h"
#define LEN_SPECIAL_CHARS 6
#elif defined(TRANSLATIONS_DE)
#include "translations/de.h"
#define LEN_SPECIAL_CHARS 6
#elif defined(TRANSLATIONS_CZ)
#include "translations/cz.h"
#define LEN_SPECIAL_CHARS 0
#elif defined(TRANSLATIONS_ES)
#include "translations/es.h"
#define LEN_SPECIAL_CHARS 0
#elif defined(TRANSLATIONS_PT)
#include "translations/pt.h"
#define LEN_SPECIAL_CHARS 0
#else
#include "translations/en.h"
#define LEN_SPECIAL_CHARS 0
#endif

#define PSIZE(x) ( sizeof(x) - 1 )
#define EOFS(x)  ( OFS_##x + sizeof(TR_##x) )

#if LCD_W >= 212
  #define TR(x,y) y
#else
  #define TR(x,y) x
#endif

// The non-0-terminated-strings

extern const pm_char STR_OPEN9X[];

#define OFS_OFFON              0
#define OFS_MMMINV             (OFS_OFFON + sizeof(TR_OFFON))
#define OFS_NCHANNELS          (OFS_MMMINV + sizeof(TR_MMMINV))
#if defined(GRAPHICS)
#define OFS_VBEEPMODE          (OFS_NCHANNELS + sizeof(TR_NCHANNELS))
#else
#define OFS_VBEEPLEN           (OFS_NCHANNELS + sizeof(TR_NCHANNELS))
#define OFS_VBEEPMODE          (OFS_VBEEPLEN + sizeof(TR_VBEEPLEN))
#endif
#if defined(ROTARY_ENCODERS)
#define OFS_VRENAVIG           (OFS_VBEEPMODE + sizeof(TR_VBEEPMODE))
#define OFS_VRENCODERS         (OFS_VRENAVIG + sizeof(TR_VRENAVIG))
#define OFS_TRNMODE            (OFS_VRENCODERS + sizeof(TR_VRENCODERS))
#elif defined(ROTARY_ENCODER_NAVIGATION)
#define OFS_VRENCODERS         (OFS_VBEEPMODE + sizeof(TR_VBEEPMODE))
#define OFS_TRNMODE            (OFS_VRENCODERS + sizeof(TR_VRENCODERS))
#else
#define OFS_TRNMODE            (OFS_VBEEPMODE + sizeof(TR_VBEEPMODE))
#endif
#define OFS_TRNCHN             (OFS_TRNMODE + sizeof(TR_TRNMODE))
#define OFS_VTRIMINC           (OFS_TRNCHN + sizeof(TR_TRNCHN))
#define OFS_RETA123            (OFS_VTRIMINC + sizeof(TR_VTRIMINC))
#define OFS_VPROTOS            (OFS_RETA123 + sizeof(TR_RETA123))
#define OFS_POSNEG             (OFS_VPROTOS + sizeof(TR_VPROTOS))
#define OFS_VBLMODE            (OFS_POSNEG + sizeof(TR_POSNEG))
#define OFS_VCURVEFUNC         (OFS_VBLMODE + sizeof(TR_VBLMODE))
#define OFS_VMLTPX             (OFS_VCURVEFUNC + sizeof(TR_VCURVEFUNC))
#define OFS_VMLTPX2            (OFS_VMLTPX + sizeof(TR_VMLTPX))
#define OFS_VMIXTRIMS          (OFS_VMLTPX2 + sizeof(TR_VMLTPX2))
#define OFS_VCSWFUNC           (OFS_VMIXTRIMS + sizeof(TR_VMIXTRIMS))
#define OFS_VFSWFUNC           (OFS_VCSWFUNC + sizeof(TR_VCSWFUNC))
#define OFS_VFSWRESET          (OFS_VFSWFUNC + sizeof(TR_VFSWFUNC))
#define OFS_FUNCSOUNDS         (OFS_VFSWRESET + sizeof(TR_VFSWRESET))
#define OFS_VTELEMCHNS         (OFS_FUNCSOUNDS + sizeof(TR_FUNCSOUNDS))
#if defined(FRSKY) || defined(CPUARM)
  #if defined(CPUARM)
   #define OFS_VTELEMUNIT_IMP  (OFS_VTELEMCHNS + sizeof(TR_VTELEMCHNS))
   #define OFS_VTELEMUNIT_NORM (OFS_VTELEMUNIT_IMP + sizeof(TR_VTELEMUNIT_IMP))
   #define OFS_VALARM          (OFS_VTELEMUNIT_NORM + sizeof(TR_VTELEMUNIT_NORM))
  #else
   #define OFS_VTELEMUNIT      (OFS_VTELEMCHNS + sizeof(TR_VTELEMCHNS))
   #define OFS_VALARM          (OFS_VTELEMUNIT + sizeof(TR_VTELEMUNIT))
  #endif
  #define OFS_VALARMFN         (OFS_VALARM + sizeof(TR_VALARM))
  #define OFS_VTELPROTO        (OFS_VALARMFN + sizeof(TR_VALARMFN))
  #define OFS_GPSFORMAT        (OFS_VTELPROTO + sizeof(TR_VTELPROTO))
  #define OFS_VOLTSRC          (OFS_GPSFORMAT + sizeof(TR_GPSFORMAT))
  #define OFS_VARIOSRC         (OFS_VOLTSRC + sizeof(TR_VOLTSRC))
  #define OFS_VSCREEN          (OFS_VARIOSRC + sizeof(TR_VARIOSRC))
  #define OFS_VTEMPLATES       (OFS_VSCREEN + sizeof(TR_VSCREEN))
#else
  #define OFS_VTEMPLATES       (OFS_VTELEMCHNS + sizeof(TR_VTELEMCHNS))
#endif
#if defined(TEMPLATES)
  #define OFS_VSWASHTYPE       (OFS_VTEMPLATES + sizeof(TR_VTEMPLATES))
#else
  #define OFS_VSWASHTYPE       (OFS_VTEMPLATES)
#endif
#if defined(HELI)
  #define OFS_VKEYS            (OFS_VSWASHTYPE + sizeof(TR_VSWASHTYPE))
#else
  #define OFS_VKEYS            (OFS_VSWASHTYPE)
#endif
#define OFS_VSWITCHES          (OFS_VKEYS + sizeof(TR_VKEYS))
#define OFS_VSRCRAW            (OFS_VSWITCHES + sizeof(TR_VSWITCHES))
#define OFS_VTMRMODES          (OFS_VSRCRAW + sizeof(TR_VSRCRAW))
#define OFS_DATETIME           (OFS_VTMRMODES + sizeof(TR_VTMRMODES))
#if defined(PCBGRUVIN9X) || defined(CPUARM)
  #define OFS_VLCD             (OFS_DATETIME + sizeof(TR_DATETIME))
  #define OFS_VUNITSSYSTEM     (OFS_VLCD + sizeof(TR_VLCD))
#else
  #define OFS_VLCD             (OFS_DATETIME)
  #define OFS_VUNITSSYSTEM     (OFS_DATETIME)
#endif
#if defined(CPUARM)
  #define OFS_COUNTRYCODES     (OFS_VUNITSSYSTEM + sizeof(TR_VUNITSSYSTEM))
#else
  #define OFS_COUNTRYCODES     (OFS_VUNITSSYSTEM)
#endif
#if defined(PXX)
  #define OFS_VTRAINERMODES    (OFS_COUNTRYCODES + sizeof(TR_COUNTRYCODES))
#else
  #define OFS_VTRAINERMODES    (OFS_COUNTRYCODES)
#endif
#if defined(PCBTARANIS)
  #define OFS_SPARE            (OFS_VTRAINERMODES + sizeof(TR_VTRAINERMODES))
#else
  #define OFS_SPARE            (OFS_VTRAINERMODES)
#endif

#define STR_OFFON              (STR_OPEN9X + OFS_OFFON)
#define STR_MMMINV             (STR_OPEN9X + OFS_MMMINV)
#define STR_NCHANNELS          (STR_OPEN9X + OFS_NCHANNELS)
#if !defined(GRAPHICS)
#define STR_VBEEPLEN           (STR_OPEN9X + OFS_VBEEPLEN)
#endif
#define STR_VBEEPMODE          (STR_OPEN9X + OFS_VBEEPMODE)
#define STR_TRNMODE            (STR_OPEN9X + OFS_TRNMODE)
#define STR_TRNCHN             (STR_OPEN9X + OFS_TRNCHN)
#define STR_VTRIMINC           (STR_OPEN9X + OFS_VTRIMINC)
#define STR_RETA123            (STR_OPEN9X + OFS_RETA123)
#define STR_VPROTOS            (STR_OPEN9X + OFS_VPROTOS)
#define STR_POSNEG             (STR_OPEN9X + OFS_POSNEG)
#define STR_VBLMODE            (STR_OPEN9X + OFS_VBLMODE)
#define STR_VCURVEFUNC         (STR_OPEN9X + OFS_VCURVEFUNC)
#define STR_VSIDE              STR_VCURVEFUNC
#define LEN_VSIDE              LEN_VCURVEFUNC
#define STR_VMLTPX             (STR_OPEN9X + OFS_VMLTPX)
#define STR_VMLTPX2            (STR_OPEN9X + OFS_VMLTPX2)
#define STR_VMIXTRIMS          (STR_OPEN9X + OFS_VMIXTRIMS)
#define STR_VCSWFUNC           (STR_OPEN9X + OFS_VCSWFUNC)
#define STR_VFSWFUNC           (STR_OPEN9X + OFS_VFSWFUNC)
#define STR_VFSWRESET          (STR_OPEN9X + OFS_VFSWRESET)
#define STR_FUNCSOUNDS         (STR_OPEN9X + OFS_FUNCSOUNDS)
#define STR_VTELEMCHNS         (STR_OPEN9X + OFS_VTELEMCHNS)

#if defined(FRSKY) || defined(CPUARM)
  #if defined(CPUARM)
    #define STR_VTELEMUNIT     (STR_OPEN9X + (g_eeGeneral.imperial ? OFS_VTELEMUNIT_IMP : OFS_VTELEMUNIT_NORM))
  #else
    #define STR_VTELEMUNIT     (STR_OPEN9X + OFS_VTELEMUNIT)
  #endif
#define STR_VALARM             (STR_OPEN9X + OFS_VALARM)
#define STR_VALARMFN           (STR_OPEN9X + OFS_VALARMFN)
#define STR_VTELPROTO          (STR_OPEN9X + OFS_VTELPROTO)
#define STR_GPSFORMAT          (STR_OPEN9X + OFS_GPSFORMAT)
#define STR_VOLTSRC            (STR_OPEN9X + OFS_VOLTSRC)
#define STR_VARIOSRC           (STR_OPEN9X + OFS_VARIOSRC)
#define STR_VSCREEN            (STR_OPEN9X + OFS_VSCREEN)
#define STR_TELEMCHNS          (STR_OPEN9X + OFS_TELEMCHNS)
#endif

#ifdef TEMPLATES
#define STR_VTEMPLATES         (STR_OPEN9X + OFS_VTEMPLATES)
#endif

#ifdef HELI
#define STR_VSWASHTYPE         (STR_OPEN9X + OFS_VSWASHTYPE)
#endif

#define STR_VKEYS              (STR_OPEN9X + OFS_VKEYS)
#define STR_VSWITCHES          (STR_OPEN9X + OFS_VSWITCHES)
#define STR_VSRCRAW            (STR_OPEN9X + OFS_VSRCRAW)
#define STR_VTMRMODES          (STR_OPEN9X + OFS_VTMRMODES)

#if defined(ROTARY_ENCODERS)
  #define STR_VRENAVIG         (STR_OPEN9X + OFS_VRENAVIG)
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
  #define STR_VRENCODERS       (STR_OPEN9X + OFS_VRENCODERS)
#endif

#if defined(PCBGRUVIN9X) || defined(CPUARM)
#define STR_DATETIME           (STR_OPEN9X + OFS_DATETIME)
#endif

#if defined(CPUARM)
  #define STR_VLCD             (STR_OPEN9X + OFS_VLCD)
  #define STR_VUNITSSYSTEM     (STR_OPEN9X + OFS_VUNITSSYSTEM)
#endif

#if defined(PXX)
  #define STR_COUNTRYCODES     (STR_OPEN9X + OFS_COUNTRYCODES)
#endif

#if defined(PCBTARANIS)
  #define STR_VTRAINERMODES    (STR_OPEN9X + OFS_VTRAINERMODES)
#endif

// The 0-terminated-strings
#define NO_INDENT(x) (x)+LEN_INDENT

extern const pm_char STR_POPUPS[];
#define STR_EXIT (STR_POPUPS + OFS_EXIT)
extern const pm_char STR_MENUWHENDONE[];
extern const pm_char STR_FREE[];
#define LEN_FREE PSIZE(TR_FREE)
extern const pm_char STR_DELETEMODEL[];
extern const pm_char STR_COPYINGMODEL[];
extern const pm_char STR_MOVINGMODEL[];
extern const pm_char STR_LOADINGMODEL[];
extern const pm_char STR_NAME[];
extern const pm_char STR_BITMAP[];
extern const pm_char STR_TIMER[];
extern const pm_char STR_ELIMITS[];
extern const pm_char STR_ETRIMS[];
extern const pm_char STR_TRIMINC[];
extern const pm_char STR_TTRACE[];
extern const pm_char STR_TTRIM[];
extern const pm_char STR_BEEPCTR[];
extern const pm_char STR_PROTO[];
extern const pm_char STR_PPMFRAME[];
extern const pm_char STR_MS[];
extern const pm_char STR_SWITCH[];
extern const pm_char STR_TRIMS[];
extern const pm_char STR_FADEIN[];
extern const pm_char STR_FADEOUT[];
extern const pm_char STR_DEFAULT[];
extern const pm_char STR_CHECKTRIMS[];
extern const pm_char STR_SWASHTYPE[];
extern const pm_char STR_COLLECTIVE[];
extern const pm_char STR_SWASHRING[];
extern const pm_char STR_ELEDIRECTION[];
extern const pm_char STR_AILDIRECTION[];
extern const pm_char STR_COLDIRECTION[];
extern const pm_char STR_MODE[];
#if defined(AUDIO) && defined(BUZZER)
extern const pm_char STR_SPEAKER[];
extern const pm_char STR_BUZZER[];
#else
#define STR_SPEAKER STR_MODE
#define STR_BUZZER  STR_MODE
#endif
extern const pm_char STR_NOFREEEXPO[];
extern const pm_char STR_NOFREEMIXER[];
extern const pm_char STR_INSERTMIX[];
extern const pm_char STR_EDITMIX[];
extern const pm_char STR_SOURCE[];
extern const pm_char STR_WEIGHT[];
extern const pm_char STR_EXPO[];
extern const pm_char STR_SIDE[];
extern const pm_char STR_DIFFERENTIAL[];
extern const pm_char STR_OFFSET[];
extern const pm_char STR_TRIM[];
extern const pm_char STR_DREX[];
extern const pm_char STR_CURVE[];
extern const pm_char STR_FLMODE[];
extern const pm_char STR_MIXWARNING[];
extern const pm_char STR_OFF[];
extern const pm_char STR_MULTPX[];
extern const pm_char STR_DELAYDOWN[];
extern const pm_char STR_DELAYUP[];
extern const pm_char STR_SLOWDOWN[];
extern const pm_char STR_SLOWUP[];
extern const pm_char STR_MIXER[];
extern const pm_char STR_CV[];
extern const pm_char STR_GV[];
extern const pm_char STR_ACHANNEL[];
extern const pm_char STR_RANGE[];
extern const pm_char STR_BAR[];
extern const pm_char STR_ALARM[];
extern const pm_char STR_USRDATA[];
extern const pm_char STR_BLADES[];
extern const pm_char STR_SCREEN[];
extern const pm_char STR_SOUND_LABEL[];
extern const pm_char STR_LENGTH[];
extern const pm_char STR_SPKRPITCH[];
extern const pm_char STR_HAPTIC_LABEL[];
extern const pm_char STR_HAPTICSTRENGTH[];
extern const pm_char STR_CONTRAST[];
extern const pm_char STR_ALARMS_LABEL[];
#if defined(BATTGRAPH) || defined(PCBTARANIS)
extern const pm_char STR_BATTERY_RANGE[];
#endif
extern const pm_char STR_BATTERYWARNING[];
extern const pm_char STR_INACTIVITYALARM[];
extern const pm_char STR_MEMORYWARNING[];
extern const pm_char STR_ALARMWARNING[];
extern const pm_char STR_RENAVIG[];
extern const pm_char STR_THROTTLEREVERSE[];
extern const pm_char STR_MINUTEBEEP[];
extern const pm_char STR_BEEPCOUNTDOWN[];
extern const pm_char STR_PERSISTENT[];
extern const pm_char STR_BACKLIGHT_LABEL[];
extern const pm_char STR_BLDELAY[];
#if defined(PWM_BACKLIGHT)
extern const pm_char STR_BLONBRIGHTNESS[];
extern const pm_char STR_BLOFFBRIGHTNESS[];
#endif
extern const pm_char STR_SPLASHSCREEN[];
extern const pm_char STR_THROTTLEWARNING[];
extern const pm_char STR_SWITCHWARNING[];
extern const pm_char STR_TIMEZONE[];
extern const pm_char STR_GPSCOORD[];
extern const pm_char STR_VARIO[];
extern const pm_char STR_RXCHANNELORD[];
extern const pm_char STR_SLAVE[];
extern const pm_char STR_MODESRC[];
extern const pm_char STR_MULTIPLIER[];
#define LEN_MULTIPLIER PSIZE(TR_MULTIPLIER)
extern const pm_char STR_CAL[];
extern const pm_char STR_VTRIM[];
extern const pm_char STR_BG[];
extern const pm_char STR_MENUTOSTART[];
extern const pm_char STR_SETMIDPOINT[];
extern const pm_char STR_MOVESTICKSPOTS[];
extern const pm_char STR_RXBATT[];
extern const pm_char STR_TX[];
#define STR_RX (STR_TX+OFS_RX)
extern const pm_char STR_ACCEL[];
extern const pm_char STR_NODATA[];
extern const pm_char STR_TM1TM2[];
extern const pm_char STR_THRTHP[];
extern const pm_char STR_TOT[];
extern const pm_char STR_TMR1LATMAXUS[];
extern const pm_char STR_TMR1LATMINUS[];
extern const pm_char STR_TMR1JITTERUS[];
extern const pm_char STR_TMIXMAXMS[];
extern const pm_char STR_T10MSUS[];
extern const pm_char STR_FREESTACKMINB[];
extern const pm_char STR_MENUTORESET[];
extern const pm_char STR_PPM[];
extern const pm_char STR_CH[];
extern const pm_char STR_MODEL[];
extern const pm_char STR_FP[];
#if defined(CPUARM)
extern const pm_char STR_MIX[];
#endif
extern const pm_char STR_EEPROMLOWMEM[];
extern const pm_char STR_ALERT[];
extern const pm_char STR_PRESSANYKEYTOSKIP[];
extern const pm_char STR_THROTTLENOTIDLE[];
extern const pm_char STR_ALARMSDISABLED[];
extern const pm_char STR_PRESSANYKEY[];
#define LEN_PRESSANYKEY PSIZE(TR_PRESSANYKEY)
extern const pm_char STR_BADEEPROMDATA[];
extern const pm_char STR_EEPROMFORMATTING[];
extern const pm_char STR_EEPROMOVERFLOW[];
extern const pm_char STR_TRIMS2OFFSETS[];
extern const pm_char STR_MENURADIOSETUP[];
extern const pm_char STR_MENUDATEANDTIME[];
extern const pm_char STR_MENUTRAINER[];
extern const pm_char STR_MENUVERSION[];
extern const pm_char STR_MENUDIAG[];
extern const pm_char STR_MENUANA[];
extern const pm_char STR_MENUCALIBRATION[];
extern const pm_char STR_MENUMODELSEL[];
extern const pm_char STR_MENUSETUP[];
extern const pm_char STR_MENUFLIGHTPHASE[];
extern const pm_char STR_MENUFLIGHTPHASES[];
extern const pm_char STR_MENUHELISETUP[];
extern const pm_char STR_MENUDREXPO[];
extern const pm_char STR_MENULIMITS[];
extern const pm_char STR_MENUCURVES[];
extern const pm_char STR_MENUCURVE[];
extern const pm_char STR_MENUCUSTOMSWITCH[];
extern const pm_char STR_MENUCUSTOMSWITCHES[];
extern const pm_char STR_MENUCUSTOMFUNC[];
extern const pm_char STR_MENUTELEMETRY[];
extern const pm_char STR_MENUTEMPLATES[];
extern const pm_char STR_MENUSTAT[];
extern const pm_char STR_MENUDEBUG[];
extern const pm_char STR_MENUGLOBALVARS[];
extern const pm_char STR_INVERT_THR[];
extern const pm_char STR_AND_SWITCH[];
extern const pm_char STR_CF[];

#if defined(DSM2) || defined(PXX)
extern const pm_char STR_RXNUM[];
#endif

#if defined(PXX)
extern const pm_char STR_SYNCMENU[];
extern const pm_char STR_INTERNALRF[];
extern const pm_char STR_EXTERNALRF[];
extern const pm_char STR_FAILSAFE[];
extern const pm_char STR_FAILSAFESET[];
extern const pm_char STR_VFAILSAFE[]; // TODO non-zero terminated
extern const pm_char STR_COUNTRYCODE[];
#endif

#if defined(FRSKY)
extern const pm_char STR_LIMIT[];
#endif

#ifdef FRSKY_HUB
extern const pm_char STR_MINRSSI[];
extern const pm_char STR_LATITUDE[];
extern const pm_char STR_LONGITUDE[];
#endif

#if defined(CPUARM) || defined(PCBGRUVIN9X)
extern const pm_char STR_SHUTDOWN[];
#endif

extern const pm_char STR_BATT_CALIB[];

#if defined(CPUARM) || defined(FRSKY)
extern const pm_char STR_VOLTAGE[];
extern const pm_char STR_CURRENT[];
#endif

#if defined(CPUARM)
  extern const pm_char STR_CURRENT_CALIB[];
  #define LEN_CALIB_FIELDS (PSIZE(TR_BATT_CALIB) > PSIZE(TR_CURRENT_CALIB) ? PSIZE(TR_BATT_CALIB) : PSIZE(TR_CURRENT_CALIB))
  extern const pm_char STR_UNITSSYSTEM[];
  extern const pm_char STR_VOICELANG[];
  extern const pm_char STR_MODELIDUSED[];
#else
  #define LEN_CALIB_FIELDS PSIZE(TR_BATT_CALIB)
#endif

#if defined(NAVIGATION_MENUS)
  extern const pm_char STR_SELECT_MODEL[];
  extern const pm_char STR_CREATE_MODEL[];
  extern const pm_char STR_COPY_MODEL[];
  extern const pm_char STR_MOVE_MODEL[];
  extern const pm_char STR_DELETE_MODEL[];
  extern const pm_char STR_EDIT[];
  extern const pm_char STR_INSERT_BEFORE[];
  extern const pm_char STR_INSERT_AFTER[];
  extern const pm_char STR_COPY[];
  extern const pm_char STR_MOVE[];
  extern const pm_char STR_DELETE[];
  extern const pm_char STR_RESET_FLIGHT[];
  extern const pm_char STR_RESET_TIMER1[];
  extern const pm_char STR_RESET_TIMER2[];
  extern const pm_char STR_RESET_TELEMETRY[];
  extern const pm_char STR_STATISTICS[];
  extern const pm_char STR_ABOUT_US[];
#endif

extern const pm_char STR_RESET[];

#if defined(SDCARD)
  extern const pm_char STR_BACKUP_MODEL[];
  extern const pm_char STR_RESTORE_MODEL[];
  extern const pm_char STR_SDCARD_ERROR[];
  extern const pm_char STR_NO_SDCARD[];
  extern const pm_char STR_INCOMPATIBLE[];
  extern const pm_char STR_LOGS_PATH[];
  extern const pm_char STR_LOGS_EXT[];
  extern const pm_char STR_MODELS_PATH[];
  extern const pm_char STR_MODELS_EXT[];
  #define STR_UPDATE_LIST STR_DELAYDOWN
#endif

extern const pm_uchar font_5x7[];
extern const pm_uchar font_10x14[];

#if defined(CPUARM)
extern const pm_uchar font_3x5[];
extern const pm_uchar font_4x6[];
extern const pm_uchar font_8x10[];
extern const pm_uchar font_5x7_extra[];
extern const pm_uchar font_10x14_extra[];
extern const pm_uchar font_4x6_extra[];
#endif

extern const pm_char STR_WARNING[];
extern const pm_char STR_EEPROMWARN[];
extern const pm_char STR_THROTTLEWARN[];
extern const pm_char STR_ALARMSWARN[];
extern const pm_char STR_SWITCHWARN[];

extern const pm_char STR_SPEAKER_VOLUME[];
extern const pm_char STR_LCD[];
extern const pm_char STR_BRIGHTNESS[];
extern const pm_char STR_CPU_TEMP[];
extern const pm_char STR_CPU_CURRENT[];
extern const pm_char STR_CPU_MAH[];
extern const pm_char STR_COPROC[];
extern const pm_char STR_COPROC_TEMP[];
extern const pm_char STR_CAPAWARNING[];
extern const pm_char STR_TEMPWARNING[];
extern const pm_char STR_FUNC[];
extern const pm_char STR_V1[];
extern const pm_char STR_V2[];
extern const pm_char STR_DURATION[];
extern const pm_char STR_DELAY[];
extern const pm_char STR_SD_CARD[];
extern const pm_char STR_SDHC_CARD[];
extern const pm_char STR_NO_SOUNDS_ON_SD[];
extern const pm_char STR_NO_MODELS_ON_SD[];
extern const pm_char STR_NO_BITMAPS_ON_SD[];
extern const pm_char STR_PLAY_FILE[];
extern const pm_char STR_DELETE_FILE[];
extern const pm_char STR_COPY_FILE[];
extern const pm_char STR_RENAME_FILE[];
extern const pm_char STR_REMOVED[];
extern const pm_char STR_SD_INFO[];
extern const pm_char STR_SD_FORMAT[];
extern const pm_char STR_NA[];
extern const pm_char STR_HARDWARE[];
extern const pm_char STR_FORMATTING[];
extern const pm_char STR_TEMP_CALIB[];
extern const pm_char STR_TIME[];
extern const pm_char STR_BAUDRATE[];
extern const pm_char STR_SD_INFO_TITLE[];
extern const pm_char STR_SD_TYPE[];
extern const pm_char STR_SD_SPEED[];
extern const pm_char STR_SD_SECTORS[];
extern const pm_char STR_SD_SIZE[];
extern const pm_char STR_TYPE[];
extern const pm_char STR_GLOBAL_VARS[];
extern const pm_char STR_GLOBAL_VAR[];
extern const pm_char STR_OWN[];
extern const pm_char STR_ROTARY_ENCODER[];
extern const pm_char STR_DATE[];
extern const pm_char STR_CHANNELS_MONITOR[];

#if defined(VOICE) && defined(CPUARM)
  struct LanguagePack {
    const char *id;
    const char *name;
    void (*playNumber)(getvalue_t number, uint8_t unit, uint8_t att, uint8_t id);
    void (*playDuration)(int16_t seconds, uint8_t id);
  };
  extern LanguagePack * languagePacks[];
  extern LanguagePack * currentLanguagePack;
  extern uint8_t currentLanguagePackIdx;
  #define LANGUAGE_PACK_DECLARE(lng, name) LanguagePack lng ## LanguagePack = { #lng, name, lng ## _ ## playNumber, lng ## _ ## playDuration }
  #define LANGUAGE_PACK_DECLARE_DEFAULT(lng, name) LANGUAGE_PACK_DECLARE(lng, name); LanguagePack * currentLanguagePack = & lng ## LanguagePack; uint8_t currentLanguagePackIdx
  inline PLAY_FUNCTION(playNumber, getvalue_t number, uint8_t unit, uint8_t att) { currentLanguagePack->playNumber(number, unit, att, id); }
  inline PLAY_FUNCTION(playDuration, int16_t seconds) { currentLanguagePack->playDuration(seconds, id); }
#elif defined(VOICE)
  PLAY_FUNCTION(playNumber, getvalue_t number, uint8_t unit, uint8_t att);
  PLAY_FUNCTION(playDuration, int16_t seconds);
  #define LANGUAGE_PACK_DECLARE(lng, name)
  #define LANGUAGE_PACK_DECLARE_DEFAULT(lng, name)
#else
  #define LANGUAGE_PACK_DECLARE(lng, name)
  #define LANGUAGE_PACK_DECLARE_DEFAULT(lng, name)
#endif

#if LCD_W >= 212
  extern const pm_char STR_MODELNAME[];
  extern const pm_char STR_PHASENAME[];
  extern const pm_char STR_MIXNAME[];
  extern const pm_char STR_EXPONAME[];
#else
  #define STR_MODELNAME STR_NAME
  #define STR_PHASENAME STR_NAME
  #define STR_MIXNAME   STR_NAME
  #define STR_EXPONAME  STR_NAME
#endif

#if LCD_W >= 212
  extern const char * STR_PHASES_HEADERS[];
  extern const char * STR_LIMITS_HEADERS[];
  extern const char * STR_CSW_HEADERS[];
#endif

#if defined(PCBTARANIS)
  extern const pm_char STR_BYTES[];
  extern const pm_char STR_MODULE_BIND[];
  extern const pm_char STR_MODULE_RANGE[];
  extern const pm_char STR_SET[];
  extern const pm_char STR_TRAINER[];
  extern const pm_char STR_ANTENNAPROBLEM[];
  extern const pm_char STR_MODULE[];
  extern const pm_char STR_CHANNELRANGE[];
  extern const pm_char STR_LOWALARM[];
  extern const pm_char STR_CRITICALALARM[];
#endif

#define CHR_SHORT  TR_CHR_SHORT
#define CHR_LONG   TR_CHR_LONG
#define CHR_TOGGLE TR_CHR_TOGGLE
#define CHR_HOUR   TR_CHR_HOUR

#endif
