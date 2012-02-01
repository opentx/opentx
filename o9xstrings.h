#ifndef o9xstrings_h
#define o9xstrings_h

#if defined(TRANSLATIONS_FR)
#include "translations/fr.h"
#elif defined(TRANSLATIONS_IT)
#include "translations/it.h"
#else
#include "translations/en.h"
#endif

#define PSIZE(x) ( sizeof(x) - 1 )

// The non-0-terminated-strings

extern const PROGMEM char STR_OPEN9X[];

#define OFS_OFFON      0
#define OFS_MMMINV     (OFS_OFFON + PSIZE(TR_OFFON))
#define OFS_NCHANNELS  (OFS_MMMINV + PSIZE(TR_MMMINV))
#define OFS_VBEEPER    (OFS_NCHANNELS + PSIZE(TR_NCHANNELS))
#define OFS_ADCFILTER  (OFS_VBEEPER + PSIZE(TR_VBEEPER))
#define OFS_WARNSW     (OFS_ADCFILTER + PSIZE(TR_ADCFILTER))
#define OFS_TRNMODE    (OFS_WARNSW + PSIZE(TR_WARNSW))
#define OFS_TRNCHN     (OFS_TRNMODE + PSIZE(TR_TRNMODE))
#define OFS_VTRIMINC   (OFS_TRNCHN + PSIZE(TR_TRNCHN))
#define OFS_RETA123    (OFS_VTRIMINC + PSIZE(TR_VTRIMINC))
#define OFS_VPROTOS    (OFS_RETA123 + PSIZE(TR_RETA123))
#define OFS_POSNEG     (OFS_VPROTOS + PSIZE(TR_VPROTOS))
#define OFS_VCURVEFUNC (OFS_POSNEG + PSIZE(TR_POSNEG))
#define OFS_CURVMODES  (OFS_VCURVEFUNC + PSIZE(TR_VCURVEFUNC))
#define OFS_EXPLABELS  (OFS_CURVMODES + PSIZE(TR_CURVMODES))
#define OFS_VMLTPX     (OFS_EXPLABELS + PSIZE(TR_EXPLABELS))
#define OFS_VMLTPX2    (OFS_VMLTPX + PSIZE(TR_VMLTPX))
#define OFS_VMIXTRIMS  (OFS_VMLTPX2 + PSIZE(TR_VMLTPX2))
#define OFS_VCSWFUNC   (OFS_VMIXTRIMS + PSIZE(TR_VMIXTRIMS))
#define OFS_VFSWFUNC   (OFS_VCSWFUNC + PSIZE(TR_VCSWFUNC))
#ifdef FRSKY
#define OFS_VTELEMUNIT (OFS_VFSWFUNC + PSIZE(TR_VFSWFUNC))
#define OFS_VALARM     (OFS_VTELEMUNIT + PSIZE(TR_VTELEMUNIT))
#define OFS_VALARMFN   (OFS_VALARM + PSIZE(TR_VALARM))
#define OFS_VTELPROTO  (OFS_VALARMFN + PSIZE(TR_VALARMFN))
#define OFS_TELEMCHNS  (OFS_VTELPROTO + PSIZE(TR_VTELPROTO))
#define OFS_ENDTELEM   (OFS_TELEMCHNS + PSIZE(TR_TELEMCHNS))
#else
#define OFS_ENDTELEM   (OFS_VFSWFUNC + PSIZE(TR_VFSWFUNC))
#endif
#ifdef TEMPLATES
#define OFS_VTEMPLATES (OFS_ENDTELEM)
#define OFS_ENDTPLS    (OFS_VTEMPLATES + PSIZE(TR_VTEMPLATES))
#else
#define OFS_ENDTPLS    (OFS_ENDTELEM)
#endif
#ifdef HELI
#define OFS_VSWASHTYPE (OFS_ENDTPLS)
#define OFS_ENDHELI    (OFS_VSWASHTYPE + PSIZE(TR_VSWASHTYPE))
#else
#define OFS_ENDHELI    (OFS_ENDTPLS)
#endif
#define OFS_VKEYS      (OFS_ENDHELI)
#define OFS_VSWITCHES  (OFS_VKEYS + PSIZE(TR_VKEYS))
#define OFS_VSRCRAW    (OFS_VSWITCHES + PSIZE(TR_VSWITCHES))
#define OFS_VTMRMODES  (OFS_VSRCRAW + PSIZE(TR_VSRCRAW))
#if defined(DSM2)
#define OFS_DSM2MODE   (OFS_VTMRMODES + PSIZE(TR_VTMRMODES))
#define OFS_ENDDSM2    (OFS_DSM2MODE + PSIZE(TR_DSM2MODE))
#else
#define OFS_ENDDSM2    (OFS_VTMRMODES + PSIZE(TR_VTMRMODES))
#endif
#if defined(PCBV4)
#define OFS_RE1RE2     (OFS_ENDDSM2)
#define OFS_DATETIME   (OFS_RE1RE2 + PSIZE(TR_RE1RE2))
#endif

#define STR_OFFON      (STR_OPEN9X + OFS_OFFON)
#define STR_ONOFF      (STR_OPEN9X + OFS_OFFON + LEN_OFFON)
#define STR_MMMINV     (STR_OPEN9X + OFS_MMMINV)
#define STR_NCHANNELS  (STR_OPEN9X + OFS_NCHANNELS)
#define STR_VBEEPER    (STR_OPEN9X + OFS_VBEEPER)
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

#ifdef FRSKY
#define STR_VTELEMUNIT (STR_OPEN9X + OFS_VTELEMUNIT)
#define STR_VALARM     (STR_OPEN9X + OFS_VALARM)
#define STR_VALARMFN   (STR_OPEN9X + OFS_VALARMFN)
#define STR_VTELPROTO  (STR_OPEN9X + OFS_VTELPROTO)
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

extern const PROGMEM char STR_POPUPS[];
extern const PROGMEM char STR_MENUWHENDONE[];
extern const PROGMEM char STR_FREE[];
#define LEN_FREE PSIZE(TR_FREE)
extern const PROGMEM char STR_DELETEMODEL[];
extern const PROGMEM char STR_COPYINGMODEL[];
extern const PROGMEM char STR_MOVINGMODEL[];
extern const PROGMEM char STR_LOADINGMODEL[];
extern const PROGMEM char STR_NAME[];
extern const PROGMEM char STR_TIMER[];
extern const PROGMEM char STR_ELIMITS[];
extern const PROGMEM char STR_ETRIMS[];
extern const PROGMEM char STR_TRIMINC[];
extern const PROGMEM char STR_TTRACE[];
extern const PROGMEM char STR_TTRIM[];
extern const PROGMEM char STR_BEEPCTR[];
extern const PROGMEM char STR_PROTO[];
extern const PROGMEM char STR_PPMFRAME[];
extern const PROGMEM char STR_MS[];
extern const PROGMEM char STR_SWITCH[];
extern const PROGMEM char STR_TRIMS[];
extern const PROGMEM char STR_FADEIN[];
extern const PROGMEM char STR_FADEOUT[];
extern const PROGMEM char STR_DEFAULT[];
extern const PROGMEM char STR_CHECKTRIMS[];
extern const PROGMEM char STR_SWASHTYPE[];
extern const PROGMEM char STR_COLLECTIVE[];
extern const PROGMEM char STR_SWASHRING[];
extern const PROGMEM char STR_ELEDIRECTION[];
extern const PROGMEM char STR_AILDIRECTION[];
extern const PROGMEM char STR_COLDIRECTION[];
extern const PROGMEM char STR_MODE[];
extern const PROGMEM char STR_NOFREEEXPO[];
extern const PROGMEM char STR_NOFREEMIXER[];
extern const PROGMEM char STR_INSERTMIX[];
extern const PROGMEM char STR_EDITMIX[];
extern const PROGMEM char STR_SOURCE[];
extern const PROGMEM char STR_WEIGHT[];
extern const PROGMEM char STR_OFFSET[];
extern const PROGMEM char STR_TRIM[];
extern const PROGMEM char STR_CURVES[];
extern const PROGMEM char STR_FPHASE[];
extern const PROGMEM char STR_WARNING[];
extern const PROGMEM char STR_OFF[];
extern const PROGMEM char STR_MULTPX[];
extern const PROGMEM char STR_DELAYDOWN[];
extern const PROGMEM char STR_DELAYUP[];
extern const PROGMEM char STR_SLOWDOWN[];
extern const PROGMEM char STR_SLOWUP[];
extern const PROGMEM char STR_DREXPO[];
extern const PROGMEM char STR_MIXER[];
extern const PROGMEM char STR_COPYTRIMMENU[];
extern const PROGMEM char STR_CV[];
extern const PROGMEM char STR_SW[];
extern const PROGMEM char STR_ACHANNEL[];
extern const PROGMEM char STR_MAX[];
extern const PROGMEM char STR_CALIB[];
extern const PROGMEM char STR_BAR[];
extern const PROGMEM char STR_ALARM[];
extern const PROGMEM char STR_USRPROTO[];
extern const PROGMEM char STR_CLEARMIXMENU[];
extern const PROGMEM char STR_BEEPER[];
extern const PROGMEM char STR_CONTRAST[];
extern const PROGMEM char STR_BATTERYWARNING[];
extern const PROGMEM char STR_INACTIVITYALARM[];
extern const PROGMEM char STR_FILTERADC[];
extern const PROGMEM char STR_THROTTLEREVERSE[];
extern const PROGMEM char STR_MINUTEBEEP[];
extern const PROGMEM char STR_BEEPCOUNTDOWN[];
extern const PROGMEM char STR_FLASHONBEEP[];
extern const PROGMEM char STR_LIGHTSWITCH[];
extern const PROGMEM char STR_LIGHTOFFAFTER[];
extern const PROGMEM char STR_SPLASHSCREEN[];
extern const PROGMEM char STR_THROTTLEWARNING[];
extern const PROGMEM char STR_SWITCHWARNING[];
extern const PROGMEM char STR_MEMORYWARNING[];
extern const PROGMEM char STR_ALARMWARNING[];
extern const PROGMEM char STR_NODATAALARM[];
extern const PROGMEM char STR_RXCHANNELORD[];
extern const PROGMEM char STR_MODE2[];
extern const PROGMEM char STR_SLAVE[];
extern const PROGMEM char STR_MODESRC[];
extern const PROGMEM char STR_MULTIPLIER[];
extern const PROGMEM char STR_CAL[];
extern const PROGMEM char STR_EEPROMV[];
extern const PROGMEM char STR_VTRIM[];
extern const PROGMEM char STR_BG[];
extern const PROGMEM char STR_MENUTOSTART[];
extern const PROGMEM char STR_SETMIDPOINT[];
extern const PROGMEM char STR_MOVESTICKSPOTS[];
extern const PROGMEM char STR_ALT[];
extern const PROGMEM char STR_RXBATT[];
extern const PROGMEM char STR_RX[];
extern const PROGMEM char STR_TX[];
extern const PROGMEM char STR_HDG[];
extern const PROGMEM char STR_SPD[];
extern const PROGMEM char STR_TEMP1[];
extern const PROGMEM char STR_TEMP2[];
extern const PROGMEM char STR_RPM[];
extern const PROGMEM char STR_FUEL[];
extern const PROGMEM char STR_VOLTS[];
extern const PROGMEM char STR_ACCEL[];
extern const PROGMEM char STR_TELE[];
extern const PROGMEM char STR_RSSI[];
extern const PROGMEM char STR_NODATA[];
extern const PROGMEM char STR_TM1[];
extern const PROGMEM char STR_TM2[];
extern const PROGMEM char STR_THR[];
extern const PROGMEM char STR_TH[];
extern const PROGMEM char STR_TOT[];
extern const PROGMEM char STR_TMR1LATMAXUS[];
extern const PROGMEM char STR_TMR1LATMINUS[];
extern const PROGMEM char STR_TMR1JITTERUS[];
extern const PROGMEM char STR_TMAINMAXMS[];
extern const PROGMEM char STR_T10MSUS[];
extern const PROGMEM char STR_FREESTACKMINB[];
extern const PROGMEM char STR_MENUTORESET[];
extern const PROGMEM char STR_PPM[];
extern const PROGMEM char STR_CH[];
extern const PROGMEM char STR_TMR[];
extern const PROGMEM char STR_MODEL[];
extern const PROGMEM char STR_FP[];
extern const PROGMEM char STR_EEPROMLOWMEM[];
extern const PROGMEM char STR_ALERT[];
extern const PROGMEM char STR_PRESSANYKEYTOSKIP[];
extern const PROGMEM char STR_THROTTLENOTIDLE[];
extern const PROGMEM char STR_RESETTHROTTLE[];
extern const PROGMEM char STR_ALARMSDISABLED[];
extern const PROGMEM char STR_SWITCHESNOTOFF[];
extern const PROGMEM char STR_PLEASERESETTHEM[];
extern const PROGMEM char STR_MESSAGE[];
extern const PROGMEM char STR_PRESSANYKEY[];
#define LEN_PRESSANYKEY PSIZE(TR_PRESSANYKEY)
extern const PROGMEM char STR_BADEEPROMDATA[];
extern const PROGMEM char STR_EEPROMFORMATTING[];
extern const PROGMEM char STR_EEPROMOVERFLOW[];
extern const PROGMEM char STR_MENUSERROR[];
extern const PROGMEM char STR_MENURADIOSETUP[];
extern const PROGMEM char STR_MENUDATEANDTIME[];
extern const PROGMEM char STR_MENUTRAINER[];
extern const PROGMEM char STR_MENUVERSION[];
extern const PROGMEM char STR_MENUDIAG[];
extern const PROGMEM char STR_MENUANA[];
extern const PROGMEM char STR_MENUCALIBRATION[];
extern const PROGMEM char STR_MENUMODELSEL[];
extern const PROGMEM char STR_MENUSETUP[];
extern const PROGMEM char STR_MENUFLIGHTPHASE[];
extern const PROGMEM char STR_MENUFLIGHTPHASES[];
extern const PROGMEM char STR_MENUHELISETUP[];
extern const PROGMEM char STR_MENUDREXPO[];
extern const PROGMEM char STR_MENULIMITS[];
extern const PROGMEM char STR_MENUCURVES[];
extern const PROGMEM char STR_MENUCURVE[];
extern const PROGMEM char STR_MENUCUSTOMSWITCHES[];
extern const PROGMEM char STR_MENUFUNCSWITCHES[];
extern const PROGMEM char STR_MENUSAFETYSWITCHES[];
extern const PROGMEM char STR_MENUTELEMETRY[];
extern const PROGMEM char STR_MENUTEMPLATES[];
extern const PROGMEM char STR_MENUSTAT[];
extern const PROGMEM char STR_MENUDEBUG[];
extern const PROGMEM char STR_RXNUM[];
extern const PROGMEM char STR_SYNCMENU[];
extern const PROGMEM char STR_HAPTICSTRENGTH[];
extern const PROGMEM char STR_SPKRPITCH[];

#endif
