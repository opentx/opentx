/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#ifndef o9xstrings_h
#define o9xstrings_h

#if defined(TRANSLATIONS_FR)
#include "translations/fr.h"
#define LEN_SPECIAL_CHARS 0
#elif defined(TRANSLATIONS_IT)
#include "translations/it.h"
#define LEN_SPECIAL_CHARS 0
#elif defined(TRANSLATIONS_SE)
#include "translations/se.h"
#define LEN_SPECIAL_CHARS 6
#else
#include "translations/en.h"
#define LEN_SPECIAL_CHARS 0
#endif

#define PSIZE(x) ( sizeof(x) - 1 )
#define EOFS(x)  ( OFS_##x + sizeof(TR_##x) )
// The non-0-terminated-strings

extern const pm_char STR_OPEN9X[];

#define OFS_OFFON      0
#define OFS_MMMINV     (OFS_OFFON + sizeof(TR_OFFON))
#define OFS_NCHANNELS  (OFS_MMMINV + sizeof(TR_MMMINV))
#define OFS_VBEEPMODE  (OFS_NCHANNELS + sizeof(TR_NCHANNELS))
#define OFS_VBEEPLEN   (OFS_VBEEPMODE + sizeof(TR_VBEEPMODE))
#define OFS_ADCFILTER  (OFS_VBEEPLEN + sizeof(TR_VBEEPLEN))
#define OFS_WARNSW     (OFS_ADCFILTER + sizeof(TR_ADCFILTER))
#define OFS_TRNMODE    (OFS_WARNSW + sizeof(TR_WARNSW))
#define OFS_TRNCHN     (OFS_TRNMODE + sizeof(TR_TRNMODE))
#define OFS_VTRIMINC   (OFS_TRNCHN + sizeof(TR_TRNCHN))
#define OFS_RETA123    (OFS_VTRIMINC + sizeof(TR_VTRIMINC))
#define OFS_VPROTOS    (OFS_RETA123 + sizeof(TR_RETA123))
#define OFS_POSNEG     (OFS_VPROTOS + sizeof(TR_VPROTOS))
#define OFS_VCURVEFUNC (OFS_POSNEG + sizeof(TR_POSNEG))
#define OFS_CURVMODES  (OFS_VCURVEFUNC + sizeof(TR_VCURVEFUNC))
#define OFS_EXPLABELS  (OFS_CURVMODES + sizeof(TR_CURVMODES))
#define OFS_VMLTPX     (OFS_EXPLABELS + sizeof(TR_EXPLABELS))
#define OFS_VMLTPX2    (OFS_VMLTPX + sizeof(TR_VMLTPX))
#define OFS_VMIXTRIMS  (OFS_VMLTPX2 + sizeof(TR_VMLTPX2))
#define OFS_VCSWFUNC   (OFS_VMIXTRIMS + sizeof(TR_VMIXTRIMS))
#define OFS_VFSWFUNC   (OFS_VCSWFUNC + sizeof(TR_VCSWFUNC))
#define OFS_VFSWRESET  (OFS_VFSWFUNC + sizeof(TR_VFSWFUNC))
#define OFS_FUNCSOUNDS (OFS_VFSWRESET + sizeof(TR_VFSWRESET))
#define OFS_VTELEMCHNS (OFS_FUNCSOUNDS + sizeof(TR_FUNCSOUNDS))
#ifdef FRSKY
#define OFS_VTELEMUNIT (OFS_VTELEMCHNS + sizeof(TR_VTELEMCHNS))
#define OFS_VALARM     (OFS_VTELEMUNIT + sizeof(TR_VTELEMUNIT))
#define OFS_VALARMFN   (OFS_VALARM + sizeof(TR_VALARM))
#define OFS_VTELPROTO  (OFS_VALARMFN + sizeof(TR_VALARMFN))
#define OFS_GPSFORMAT   (OFS_VTELPROTO + sizeof(TR_VTELPROTO))
#define OFS_ENDTELEM   (OFS_GPSFORMAT + sizeof(TR_GPSFORMAT))
#else
#define OFS_ENDTELEM   (OFS_VTELEMCHNS + sizeof(TR_VTELEMCHNS))
#endif
#ifdef TEMPLATES
#define OFS_VTEMPLATES (OFS_ENDTELEM)
#define OFS_ENDTPLS    (OFS_VTEMPLATES + sizeof(TR_VTEMPLATES))
#else
#define OFS_ENDTPLS    (OFS_ENDTELEM)
#endif
#ifdef HELI
#define OFS_VSWASHTYPE (OFS_ENDTPLS)
#define OFS_ENDHELI    (OFS_VSWASHTYPE + sizeof(TR_VSWASHTYPE))
#else
#define OFS_ENDHELI    (OFS_ENDTPLS)
#endif
#define OFS_VKEYS      (OFS_ENDHELI)
#define OFS_VSWITCHES  (OFS_VKEYS + sizeof(TR_VKEYS))
#define OFS_VSRCRAW    (OFS_VSWITCHES + sizeof(TR_VSWITCHES))
#define OFS_VTMRMODES  (OFS_VSRCRAW + sizeof(TR_VSRCRAW))
#if defined(DSM2)
#define OFS_DSM2MODE   (OFS_VTMRMODES + sizeof(TR_VTMRMODES))
#define OFS_ENDDSM2    (OFS_DSM2MODE + sizeof(TR_DSM2MODE))
#else
#define OFS_ENDDSM2    (OFS_VTMRMODES + sizeof(TR_VTMRMODES))
#endif
#if defined(PCBV4)
#define OFS_RE1RE2     (OFS_ENDDSM2)
#define OFS_DATETIME   (OFS_RE1RE2 + sizeof(TR_RE1RE2))
#endif

#define STR_OFFON      (STR_OPEN9X + OFS_OFFON)
#define STR_MMMINV     (STR_OPEN9X + OFS_MMMINV)
#define STR_NCHANNELS  (STR_OPEN9X + OFS_NCHANNELS)
#define STR_VBEEPMODE  (STR_OPEN9X + OFS_VBEEPMODE)
#define STR_VBEEPLEN   (STR_OPEN9X + OFS_VBEEPLEN)
#define STR_ADCFILTER  (STR_OPEN9X + OFS_ADCFILTER)
#define STR_WARNSW     (STR_OPEN9X + OFS_WARNSW)
#define STR_TRNMODE    (STR_OPEN9X + OFS_TRNMODE)
#define STR_TRNCHN     (STR_OPEN9X + OFS_TRNCHN)
#define STR_VTRIMINC   (STR_OPEN9X + OFS_VTRIMINC)
#define STR_RETA123    (STR_OPEN9X + OFS_RETA123)
#define STR_VPROTOS    (STR_OPEN9X + OFS_VPROTOS)
#define STR_POSNEG     (STR_OPEN9X + OFS_POSNEG)
#define STR_VCURVEFUNC (STR_OPEN9X + OFS_VCURVEFUNC)
#define STR_VWHEN      STR_VCURVEFUNC
#define LEN_VWHEN      LEN_VCURVEFUNC
#define STR_CURVMODES  (STR_OPEN9X + OFS_CURVMODES)
#define STR_EXPLABELS  (STR_OPEN9X + OFS_EXPLABELS)
#define STR_VMLTPX     (STR_OPEN9X + OFS_VMLTPX)
#define STR_VMLTPX2    (STR_OPEN9X + OFS_VMLTPX2)
#define STR_VMIXTRIMS  (STR_OPEN9X + OFS_VMIXTRIMS)
#define STR_VCSWFUNC   (STR_OPEN9X + OFS_VCSWFUNC)
#define STR_VFSWFUNC   (STR_OPEN9X + OFS_VFSWFUNC)
#define STR_VFSWRESET  (STR_OPEN9X + OFS_VFSWRESET)
#define STR_FUNCSOUNDS (STR_OPEN9X + OFS_FUNCSOUNDS)
#define STR_VTELEMCHNS (STR_OPEN9X + OFS_VTELEMCHNS)

#ifdef FRSKY
#define STR_VTELEMUNIT (STR_OPEN9X + OFS_VTELEMUNIT)
#define STR_VALARM     (STR_OPEN9X + OFS_VALARM)
#define STR_VALARMFN   (STR_OPEN9X + OFS_VALARMFN)
#define STR_VTELPROTO  (STR_OPEN9X + OFS_VTELPROTO)
#define STR_GPSFORMAT  (STR_OPEN9X + OFS_GPSFORMAT)
#define STR_TELEMCHNS  (STR_OPEN9X + OFS_TELEMCHNS)
#endif

#ifdef TEMPLATES
#define STR_VTEMPLATES (STR_OPEN9X + OFS_VTEMPLATES)
#endif

#ifdef HELI
#define STR_VSWASHTYPE (STR_OPEN9X + OFS_VSWASHTYPE)
#endif

#define STR_VKEYS      (STR_OPEN9X + OFS_VKEYS)
#define STR_VSWITCHES  (STR_OPEN9X + OFS_VSWITCHES)
#define STR_VSRCRAW    (STR_OPEN9X + OFS_VSRCRAW)
#define STR_VTMRMODES  (STR_OPEN9X + OFS_VTMRMODES)

#if defined(DSM2)
#define STR_DSM2MODE   (STR_OPEN9X + OFS_DSM2MODE)
#endif

#if defined(PCBV4)
#define STR_RE1RE2     (STR_OPEN9X + OFS_RE1RE2)
#define STR_DATETIME   (STR_OPEN9X + OFS_DATETIME)
#endif

// The 0-terminated-strings

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
extern const pm_char STR_NOFREEEXPO[];
extern const pm_char STR_NOFREEMIXER[];
extern const pm_char STR_INSERTMIX[];
extern const pm_char STR_EDITMIX[];
extern const pm_char STR_SOURCE[];
extern const pm_char STR_WEIGHT[];
extern const pm_char STR_DIFFERENTIAL[];
extern const pm_char STR_OFFSET[];
extern const pm_char STR_TRIM[];
extern const pm_char STR_CURVES[];
extern const pm_char STR_FPHASE[];
extern const pm_char STR_WARNING[];
extern const pm_char STR_OFF[];
extern const pm_char STR_MULTPX[];
extern const pm_char STR_DELAYDOWN[];
extern const pm_char STR_DELAYUP[];
extern const pm_char STR_SLOWDOWN[];
extern const pm_char STR_SLOWUP[];
extern const pm_char STR_MIXER[];
extern const pm_char STR_CV[];
extern const pm_char STR_SW[];
extern const pm_char STR_ACHANNEL[];
extern const pm_char STR_RANGE[];
extern const pm_char STR_BAR[];
extern const pm_char STR_ALARM[];
extern const pm_char STR_USRDATA[];
extern const pm_char STR_BLADES[];
extern const pm_char STR_BARS[];
extern const pm_char STR_DISPLAY[];
extern const pm_char STR_BEEPERMODE[];
extern const pm_char STR_BEEPERLEN[];
extern const pm_char STR_SPKRPITCH[];
extern const pm_char STR_HAPTICMODE[];
extern const pm_char STR_HAPTICSTRENGTH[];
extern const pm_char STR_CONTRAST[];
extern const pm_char STR_BATTERYWARNING[];
extern const pm_char STR_INACTIVITYALARM[];
extern const pm_char STR_FILTERADC[];
extern const pm_char STR_THROTTLEREVERSE[];
extern const pm_char STR_MINUTEBEEP[];
extern const pm_char STR_BEEPCOUNTDOWN[];
extern const pm_char STR_FLASHONBEEP[];
extern const pm_char STR_LIGHTSWITCH[];
extern const pm_char STR_LIGHTOFFAFTER[];
extern const pm_char STR_SPLASHSCREEN[];
extern const pm_char STR_THROTTLEWARNING[];
extern const pm_char STR_SWITCHWARNING[];
extern const pm_char STR_MEMORYWARNING[];
extern const pm_char STR_ALARMWARNING[];
extern const pm_char STR_NODATAALARM[];
extern const pm_char STR_TIMEZONE[];
extern const pm_char STR_GPSCOORD[];
extern const pm_char STR_RXCHANNELORD[];
extern const pm_char STR_SLAVE[];
extern const pm_char STR_MODESRC[];
extern const pm_char STR_MULTIPLIER[];
extern const pm_char STR_CAL[];
extern const pm_char STR_EEPROMV[];
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
extern const pm_char STR_TMAINMAXMS[];
extern const pm_char STR_T10MSUS[];
extern const pm_char STR_FREESTACKMINB[];
extern const pm_char STR_MENUTORESET[];
extern const pm_char STR_PPM[];
extern const pm_char STR_CH[];
extern const pm_char STR_MODEL[];
extern const pm_char STR_FP[];
extern const pm_char STR_EEPROMLOWMEM[];
extern const pm_char STR_ALERT[];
extern const pm_char STR_PRESSANYKEYTOSKIP[];
extern const pm_char STR_THROTTLENOTIDLE[];
extern const pm_char STR_RESETTHROTTLE[];
extern const pm_char STR_ALARMSDISABLED[];
extern const pm_char STR_SWITCHESNOTOFF[];
extern const pm_char STR_PLEASERESETTHEM[];
extern const pm_char STR_MESSAGE[];
extern const pm_char STR_PRESSANYKEY[];
#define LEN_PRESSANYKEY PSIZE(TR_PRESSANYKEY)
extern const pm_char STR_BADEEPROMDATA[];
extern const pm_char STR_EEPROMFORMATTING[];
extern const pm_char STR_EEPROMOVERFLOW[];
extern const pm_char STR_MENUSERROR[];
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
extern const pm_char STR_MENUCUSTOMSWITCHES[];
extern const pm_char STR_MENUFUNCSWITCHES[];
extern const pm_char STR_MENUTELEMETRY[];
extern const pm_char STR_MENUTEMPLATES[];
extern const pm_char STR_MENUSTAT[];
extern const pm_char STR_MENUDEBUG[];
extern const pm_char STR_RXNUM[];
extern const pm_char STR_SYNCMENU[];
#endif

#ifdef NAVIGATION_RE1
extern const pm_char STR_BACK[];
#define LEN_BACK PSIZE(TR_BACK)
extern const pm_char STR_MIXERWEIGHT[];
extern const pm_char STR_MIXEROFFSET[];
extern const pm_char STR_DRWEIGHT[];
extern const pm_char STR_DREXPO[];
extern const pm_char STR_MINLIMIT[];
extern const pm_char STR_MAXLIMIT[];
#endif

#ifdef FRSKY_HUB
extern const pm_char STR_MINRSSI[];
extern const pm_char STR_LATITUDE[];
extern const pm_char STR_LONGITUDE[];
#endif

#if defined(PCBARM) || defined(PCBV4)
extern const pm_char STR_SHUTDOWN[];
#endif

#if defined(PCBARM)
extern const pm_char STR_BATT_CALIB[];
extern const pm_char STR_CURRENT_CALIB[];
extern const pm_char STR_CURRENT[];
#endif

extern const pm_uchar font[];
extern const pm_uchar font_dblsize[];
