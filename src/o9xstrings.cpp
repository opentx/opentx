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

#include "open9x.h"

// The non-0-terminated-strings
const pm_char STR_OPEN9X[] PROGMEM =
    TR_OFFON
    TR_MMMINV
    TR_NCHANNELS
    TR_VBEEPMODE
    TR_VBEEPLEN
    TR_ADCFILTER
    TR_WARNSW
    TR_TRNMODE
    TR_TRNCHN
    TR_VTRIMINC
    TR_RETA123
    TR_VPROTOS
    TR_POSNEG
    TR_VCURVEFUNC
    TR_CURVMODES
    TR_EXPLABELS
    TR_VMLTPX
    TR_VMLTPX2
    TR_VMIXTRIMS
    TR_VCSWFUNC
    TR_VFSWFUNC
    TR_FUNCSOUNDS
#ifdef FRSKY
    TR_VTELEMBARS
    TR_VTELEMUNIT
    TR_VALARM
    TR_VALARMFN
    TR_VTELPROTO
    TR_TELEMCHNS
#endif
#ifdef TEMPLATES
    TR_VTEMPLATES
#endif
#ifdef HELI
    TR_VSWASHTYPE
#endif
    TR_VKEYS
    TR_VSWITCHES
    TR_VSRCRAW
    TR_VTMRMODES
#if defined(DSM2)
    TR_DSM2MODE
#endif
#if defined(PCBV4)
    TR_RE1RE2
    TR_DATETIME
#endif
    ;

// The 0-terminated-strings
const pm_char STR_POPUPS[] PROGMEM = TR_POPUPS;
const pm_char STR_MENUWHENDONE[] PROGMEM = TR_MENUWHENDONE;
const pm_char STR_FREE[] PROGMEM = TR_FREE;
const pm_char STR_DELETEMODEL[] PROGMEM = TR_DELETEMODEL;
const pm_char STR_COPYINGMODEL[] PROGMEM = TR_COPYINGMODEL;
const pm_char STR_MOVINGMODEL[] PROGMEM = TR_MOVINGMODEL;
const pm_char STR_LOADINGMODEL[] PROGMEM = TR_LOADINGMODEL;
const pm_char STR_NAME[] PROGMEM = TR_NAME;
const pm_char STR_TIMER[] PROGMEM = TR_TIMER;
const pm_char STR_ELIMITS[] PROGMEM = TR_ELIMITS;
const pm_char STR_ETRIMS[] PROGMEM = TR_ETRIMS;
const pm_char STR_TRIMINC[] PROGMEM = TR_TRIMINC;
const pm_char STR_TTRACE[] PROGMEM = TR_TTRACE;
const pm_char STR_TTRIM[] PROGMEM = TR_TTRIM;
const pm_char STR_BEEPCTR[] PROGMEM = TR_BEEPCTR;
const pm_char STR_PROTO[] PROGMEM = TR_PROTO;
const pm_char STR_PPMFRAME[] PROGMEM = TR_PPMFRAME;
const pm_char STR_MS[] PROGMEM = TR_MS;
const pm_char STR_SWITCH[] PROGMEM = TR_SWITCH;
const pm_char STR_TRIMS[] PROGMEM = TR_TRIMS;
const pm_char STR_FADEIN[] PROGMEM = TR_FADEIN;
const pm_char STR_FADEOUT[] PROGMEM = TR_FADEOUT;
const pm_char STR_DEFAULT[] PROGMEM = TR_DEFAULT;
const pm_char STR_CHECKTRIMS[] PROGMEM = TR_CHECKTRIMS;
#ifdef HELI
const pm_char STR_SWASHTYPE[] PROGMEM = TR_SWASHTYPE;
const pm_char STR_COLLECTIVE[] PROGMEM = TR_COLLECTIVE;
const pm_char STR_SWASHRING[] PROGMEM = TR_SWASHRING;
const pm_char STR_ELEDIRECTION[] PROGMEM = TR_ELEDIRECTION;
const pm_char STR_AILDIRECTION[] PROGMEM = TR_AILDIRECTION;
const pm_char STR_COLDIRECTION[] PROGMEM = TR_COLDIRECTION;
#endif
const pm_char STR_MODE[] PROGMEM = TR_MODE;
const pm_char STR_NOFREEEXPO[] PROGMEM = TR_NOFREEEXPO;
const pm_char STR_NOFREEMIXER[] PROGMEM = TR_NOFREEMIXER;
const pm_char STR_INSERTMIX[] PROGMEM = TR_INSERTMIX;
const pm_char STR_EDITMIX[] PROGMEM = TR_EDITMIX;
const pm_char STR_SOURCE[] PROGMEM = TR_SOURCE;
const pm_char STR_WEIGHT[] PROGMEM = TR_WEIGHT;
const pm_char STR_OFFSET[] PROGMEM = TR_OFFSET;
const pm_char STR_TRIM[] PROGMEM = TR_TRIM;
const pm_char STR_CURVES[] PROGMEM = TR_CURVES;
const pm_char STR_FPHASE[] PROGMEM = TR_FPHASE;
const pm_char STR_WARNING[] PROGMEM = TR_WARNING;
const pm_char STR_OFF[] PROGMEM = TR_OFF;
const pm_char STR_MULTPX[] PROGMEM = TR_MULTPX;
const pm_char STR_DELAYDOWN[] PROGMEM = TR_DELAYDOWN;
const pm_char STR_DELAYUP[] PROGMEM = TR_DELAYUP;
const pm_char STR_SLOWDOWN[] PROGMEM = TR_SLOWDOWN;
const pm_char STR_SLOWUP[] PROGMEM = TR_SLOWUP;
const pm_char STR_DREXPO[] PROGMEM = TR_DREXPO;
const pm_char STR_MIXER[] PROGMEM = TR_MIXER;
const pm_char STR_COPYTRIMMENU[] PROGMEM = TR_COPYTRIMMENU;
const pm_char STR_CV[] PROGMEM = TR_CV;
const pm_char STR_SW[] PROGMEM = TR_SW;
const pm_char STR_ACHANNEL[] PROGMEM = TR_ACHANNEL;
const pm_char STR_RANGE[] PROGMEM = TR_RANGE;
const pm_char STR_BAR[] PROGMEM = TR_BAR;
const pm_char STR_ALARM[] PROGMEM = TR_ALARM;
const pm_char STR_USRDATA[] PROGMEM = TR_USRDATA;
const pm_char STR_BLADES[] PROGMEM = TR_BLADES; // TODO check that it is optimized away when no FRSKY
const pm_char STR_BARS[] PROGMEM = TR_BARS;
const pm_char STR_CLEARMIXMENU[] PROGMEM = TR_CLEARMIXMENU;
const pm_char STR_BEEPERMODE[] PROGMEM = TR_BEEPERMODE;
const pm_char STR_BEEPERLEN[] PROGMEM = TR_BEEPERLEN;
#if defined(AUDIO)
const pm_char STR_SPKRPITCH[] PROGMEM = TR_SPKRPITCH;
#endif
#if defined(HAPTIC)
const pm_char STR_HAPTICMODE[] PROGMEM = TR_HAPTICMODE;
const pm_char STR_HAPTICSTRENGTH[] PROGMEM = TR_HAPTICSTRENGTH;
#endif
const pm_char STR_CONTRAST[] PROGMEM = TR_CONTRAST;
const pm_char STR_BATTERYWARNING[] PROGMEM = TR_BATTERYWARNING;
const pm_char STR_INACTIVITYALARM[] PROGMEM = TR_INACTIVITYALARM;
const pm_char STR_FILTERADC[] PROGMEM = TR_FILTERADC;
const pm_char STR_THROTTLEREVERSE[] PROGMEM = TR_THROTTLEREVERSE;
const pm_char STR_MINUTEBEEP[] PROGMEM = TR_MINUTEBEEP;
const pm_char STR_BEEPCOUNTDOWN[] PROGMEM = TR_BEEPCOUNTDOWN;
const pm_char STR_FLASHONBEEP[] PROGMEM = TR_FLASHONBEEP;
const pm_char STR_LIGHTSWITCH[] PROGMEM = TR_LIGHTSWITCH;
const pm_char STR_LIGHTOFFAFTER[] PROGMEM = TR_LIGHTOFFAFTER;
const pm_char STR_SPLASHSCREEN[] PROGMEM = TR_SPLASHSCREEN;
const pm_char STR_THROTTLEWARNING[] PROGMEM = TR_THROTTLEWARNING;
const pm_char STR_SWITCHWARNING[] PROGMEM = TR_SWITCHWARNING;
const pm_char STR_MEMORYWARNING[] PROGMEM = TR_MEMORYWARNING;
const pm_char STR_ALARMWARNING[] PROGMEM = TR_ALARMWARNING;
const pm_char STR_NODATAALARM[] PROGMEM = TR_NODATAALARM;
const pm_char STR_RXCHANNELORD[] PROGMEM = TR_RXCHANNELORD;
const pm_char STR_MODE2[] PROGMEM = TR_MODE2;
const pm_char STR_SLAVE[] PROGMEM = TR_SLAVE;
const pm_char STR_MODESRC[] PROGMEM = TR_MODESRC;
const pm_char STR_MULTIPLIER[] PROGMEM = TR_MULTIPLIER;
const pm_char STR_CAL[] PROGMEM = TR_CAL;
const pm_char STR_EEPROMV[] PROGMEM = TR_EEPROMV;
const pm_char STR_VTRIM[] PROGMEM = TR_VTRIM;
const pm_char STR_BG[] PROGMEM = TR_BG;
const pm_char STR_MENUTOSTART[] PROGMEM = TR_MENUTOSTART;
const pm_char STR_SETMIDPOINT[] PROGMEM = TR_SETMIDPOINT;
const pm_char STR_MOVESTICKSPOTS[] PROGMEM = TR_MOVESTICKSPOTS;
const pm_char STR_ALTnDST[] PROGMEM = TR_ALTnDST;
const pm_char STR_RXBATT[] PROGMEM = TR_RXBATT;
const pm_char STR_TX[] PROGMEM = TR_TXnRX;
const pm_char STR_SPDnMAX[] PROGMEM = TR_SPDnMAX;
const pm_char STR_TEMP1nTEMP2[] PROGMEM = TR_TEMP1nTEMP2;
const pm_char STR_RPMnFUEL[] PROGMEM = TR_RPMnFUEL;
const pm_char STR_ACCEL[] PROGMEM = TR_ACCEL;
const pm_char STR_NODATA[] PROGMEM = TR_NODATA;
const pm_char STR_TM1TM2[] PROGMEM = TR_TM1TM2;
const pm_char STR_THRTHP[] PROGMEM = TR_THRTHP;
const pm_char STR_TOT[] PROGMEM = TR_TOT;
const pm_char STR_TMR1LATMAXUS[] PROGMEM = TR_TMR1LATMAXUS;
const pm_char STR_TMR1LATMINUS[] PROGMEM = TR_TMR1LATMINUS;
const pm_char STR_TMR1JITTERUS[] PROGMEM = TR_TMR1JITTERUS;
const pm_char STR_TMAINMAXMS[] PROGMEM = TR_TMAINMAXMS;
#ifdef DEBUG
const pm_char STR_T10MSUS[] PROGMEM = TR_T10MSUS;
#endif
const pm_char STR_FREESTACKMINB[] PROGMEM = TR_FREESTACKMINB;
const pm_char STR_MENUTORESET[] PROGMEM = TR_MENUTORESET;
const pm_char STR_PPM[] PROGMEM = TR_PPM;
const pm_char STR_CH[] PROGMEM = TR_CH;
const pm_char STR_TMR[] PROGMEM = TR_TMR;
const pm_char STR_MODEL[] PROGMEM = TR_MODEL;
const pm_char STR_FP[] PROGMEM = TR_FP;
const pm_char STR_EEPROMLOWMEM[] PROGMEM = TR_EEPROMLOWMEM;
const pm_char STR_ALERT[] PROGMEM = TR_ALERT;
const pm_char STR_PRESSANYKEYTOSKIP[] PROGMEM = TR_PRESSANYKEYTOSKIP;
const pm_char STR_THROTTLENOTIDLE[] PROGMEM = TR_THROTTLENOTIDLE;
const pm_char STR_RESETTHROTTLE[] PROGMEM = TR_RESETTHROTTLE;
const pm_char STR_ALARMSDISABLED[] PROGMEM = TR_ALARMSDISABLED;
const pm_char STR_SWITCHESNOTOFF[] PROGMEM = TR_SWITCHESNOTOFF;
const pm_char STR_PLEASERESETTHEM[] PROGMEM = TR_PLEASERESETTHEM;
const pm_char STR_MESSAGE[] PROGMEM = TR_MESSAGE;
const pm_char STR_PRESSANYKEY[] PROGMEM = TR_PRESSANYKEY;
const pm_char STR_BADEEPROMDATA[] PROGMEM = TR_BADEEPROMDATA;
const pm_char STR_EEPROMFORMATTING[] PROGMEM = TR_EEPROMFORMATTING;
const pm_char STR_EEPROMOVERFLOW[] PROGMEM = TR_EEPROMOVERFLOW;
const pm_char STR_MENUSERROR[] PROGMEM = TR_MENUSERROR;

const pm_char STR_MENURADIOSETUP[] PROGMEM = TR_MENURADIOSETUP;

#ifdef PCBV4
const pm_char STR_MENUDATEANDTIME[] PROGMEM = TR_MENUDATEANDTIME;
#endif

const pm_char STR_MENUTRAINER[] PROGMEM = TR_MENUTRAINER;
const pm_char STR_MENUVERSION[] PROGMEM = TR_MENUVERSION;
const pm_char STR_MENUDIAG[] PROGMEM = TR_MENUDIAG;
const pm_char STR_MENUANA[] PROGMEM = TR_MENUANA;
const pm_char STR_MENUCALIBRATION[] PROGMEM = TR_MENUCALIBRATION;

const pm_char STR_MENUMODELSEL[] PROGMEM = TR_MENUMODELSEL;
const pm_char STR_MENUSETUP[] PROGMEM = TR_MENUSETUP;
const pm_char STR_MENUFLIGHTPHASE[] PROGMEM = TR_MENUFLIGHTPHASE;
const pm_char STR_MENUFLIGHTPHASES[] PROGMEM = TR_MENUFLIGHTPHASES;

#ifdef HELI
const pm_char STR_MENUHELISETUP[] PROGMEM = TR_MENUHELISETUP;
#endif

const pm_char STR_MENUDREXPO[] PROGMEM = TR_MENUDREXPO;
const pm_char STR_MENULIMITS[] PROGMEM = TR_MENULIMITS;
const pm_char STR_MENUCURVES[] PROGMEM = TR_MENUCURVES;
const pm_char STR_MENUCURVE[] PROGMEM = TR_MENUCURVE;
const pm_char STR_MENUCUSTOMSWITCHES[] PROGMEM = TR_MENUCUSTOMSWITCHES;
const pm_char STR_MENUFUNCSWITCHES[] PROGMEM = TR_MENUFUNCSWITCHES;

#ifdef FRSKY
const pm_char STR_MENUTELEMETRY[] PROGMEM = TR_MENUTELEMETRY;
#endif

#ifdef TEMPLATES
const pm_char STR_MENUTEMPLATES[] PROGMEM = TR_MENUTEMPLATES;
#endif

const pm_char STR_MENUSTAT[] PROGMEM = TR_MENUSTAT;
const pm_char STR_MENUDEBUG[] PROGMEM = TR_MENUDEBUG;

#if defined(DSM2) || defined(PXX)
const pm_char STR_RXNUM[] PROGMEM = TR_RXNUM;
#endif

#if defined(PXX)
const pm_char STR_SYNCMENU[] PROGMEM = TR_SYNCMENU;
#endif
