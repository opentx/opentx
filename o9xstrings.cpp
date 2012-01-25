#include "open9x.h"

// The non-0-terminated-strings
const prog_char APM STR_OPEN9X[] =
    TR_OFFON
    TR_MMMINV
    TR_NCHANNELS
    TR_VBEEPER
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
    TR_VCSWFUNC
    TR_VFSWFUNC
#ifdef FRSKY
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
#if defined(PCBV3)
    TR_RE1RE2
    TR_DATETIME
#endif
    ;

// The 0-terminated-strings
const prog_char APM STR_POPUPS[] = TR_POPUPS;
const prog_char APM STR_MENUWHENDONE[] = TR_MENUWHENDONE;
const prog_char APM STR_FREE[] = TR_FREE;
const prog_char APM STR_DELETEMODEL[] = TR_DELETEMODEL;
const prog_char APM STR_COPYINGMODEL[] = TR_COPYINGMODEL;
const prog_char APM STR_MOVINGMODEL[] = TR_MOVINGMODEL;
const prog_char APM STR_LOADINGMODEL[] = TR_LOADINGMODEL;
const prog_char APM STR_NAME[] = TR_NAME;
const prog_char APM STR_TIMER[] = TR_TIMER;
const prog_char APM STR_ELIMITS[] = TR_ELIMITS;
const prog_char APM STR_ETRIMS[] = TR_ETRIMS;
const prog_char APM STR_TRIMINC[] = TR_TRIMINC;
const prog_char APM STR_TTRACE[] = TR_TTRACE;
const prog_char APM STR_TTRIM[] = TR_TTRIM;
const prog_char APM STR_BEEPCTR[] = TR_BEEPCTR;
const prog_char APM STR_PROTO[] = TR_PROTO;
const prog_char APM STR_PPMFRAME[] = TR_PPMFRAME;
const prog_char APM STR_MS[] = TR_MS;
const prog_char APM STR_SWITCH[] = TR_SWITCH;
const prog_char APM STR_TRIMS[] = TR_TRIMS;
const prog_char APM STR_FADEIN[] = TR_FADEIN;
const prog_char APM STR_FADEOUT[] = TR_FADEOUT;
const prog_char APM STR_DEFAULT[] = TR_DEFAULT;
const prog_char APM STR_CHECKTRIMS[] = TR_CHECKTRIMS;
#ifdef HELI
const prog_char APM STR_SWASHTYPE[] = TR_SWASHTYPE;
const prog_char APM STR_COLLECTIVE[] = TR_COLLECTIVE;
const prog_char APM STR_SWASHRING[] = TR_SWASHRING;
const prog_char APM STR_ELEDIRECTION[] = TR_ELEDIRECTION;
const prog_char APM STR_AILDIRECTION[] = TR_AILDIRECTION;
const prog_char APM STR_COLDIRECTION[] = TR_COLDIRECTION;
#endif
const prog_char APM STR_MODE[] = TR_MODE;
const prog_char APM STR_NOFREEEXPO[] = TR_NOFREEEXPO;
const prog_char APM STR_NOFREEMIXER[] = TR_NOFREEMIXER;
const prog_char APM STR_INSERTMIX[] = TR_INSERTMIX;
const prog_char APM STR_EDITMIX[] = TR_EDITMIX;
const prog_char APM STR_SOURCE[] = TR_SOURCE;
const prog_char APM STR_WEIGHT[] = TR_WEIGHT;
const prog_char APM STR_OFFSET[] = TR_OFFSET;
const prog_char APM STR_TRIM[] = TR_TRIM;
const prog_char APM STR_CURVES[] = TR_CURVES;
const prog_char APM STR_FPHASE[] = TR_FPHASE;
const prog_char APM STR_WARNING[] = TR_WARNING;
const prog_char APM STR_OFF[] = TR_OFF;
const prog_char APM STR_MULTPX[] = TR_MULTPX;
const prog_char APM STR_DELAYDOWN[] = TR_DELAYDOWN;
const prog_char APM STR_DELAYUP[] = TR_DELAYUP;
const prog_char APM STR_SLOWDOWN[] = TR_SLOWDOWN;
const prog_char APM STR_SLOWUP[] = TR_SLOWUP;
const prog_char APM STR_DREXPO[] = TR_DREXPO;
const prog_char APM STR_MIXER[] = TR_MIXER;
const prog_char APM STR_COPYTRIMMENU[] = TR_COPYTRIMMENU;
const prog_char APM STR_CV[] = TR_CV;
const prog_char APM STR_SW[] = TR_SW;
const prog_char APM STR_ACHANNEL[] = TR_ACHANNEL;
const prog_char APM STR_MAX[] = TR_MAX;
const prog_char APM STR_CALIB[] = TR_CALIB;
const prog_char APM STR_BAR[] = TR_BAR;
const prog_char APM STR_ALARM[] = TR_ALARM;
const prog_char APM STR_USRPROTO[] = TR_USRPROTO;
const prog_char APM STR_CLEARMIXMENU[] = TR_CLEARMIXMENU;
const prog_char APM STR_BEEPER[] = TR_BEEPER;
const prog_char APM STR_CONTRAST[] = TR_CONTRAST;
const prog_char APM STR_BATTERYWARNING[] = TR_BATTERYWARNING;
const prog_char APM STR_INACTIVITYALARM[] = TR_INACTIVITYALARM;
const prog_char APM STR_FILTERADC[] = TR_FILTERADC;
const prog_char APM STR_THROTTLEREVERSE[] = TR_THROTTLEREVERSE;
const prog_char APM STR_MINUTEBEEP[] = TR_MINUTEBEEP;
const prog_char APM STR_BEEPCOUNTDOWN[] = TR_BEEPCOUNTDOWN;
const prog_char APM STR_FLASHONBEEP[] = TR_FLASHONBEEP;
const prog_char APM STR_LIGHTSWITCH[] = TR_LIGHTSWITCH;
const prog_char APM STR_LIGHTOFFAFTER[] = TR_LIGHTOFFAFTER;
const prog_char APM STR_SPLASHSCREEN[] = TR_SPLASHSCREEN;
const prog_char APM STR_THROTTLEWARNING[] = TR_THROTTLEWARNING;
const prog_char APM STR_SWITCHWARNING[] = TR_SWITCHWARNING;
const prog_char APM STR_MEMORYWARNING[] = TR_MEMORYWARNING;
const prog_char APM STR_ALARMWARNING[] = TR_ALARMWARNING;
const prog_char APM STR_NODATAALARM[] = TR_NODATAALARM;
const prog_char APM STR_RXCHANNELORD[] = TR_RXCHANNELORD;
const prog_char APM STR_MODE2[] = TR_MODE2;
const prog_char APM STR_SLAVE[] = TR_SLAVE;
const prog_char APM STR_MODESRC[] = TR_MODESRC;
const prog_char APM STR_MULTIPLIER[] = TR_MULTIPLIER;
const prog_char APM STR_CAL[] = TR_CAL;
const prog_char APM STR_EEPROMV[] = TR_EEPROMV;
const prog_char APM STR_VTRIM[] = TR_VTRIM;
const prog_char APM STR_BG[] = TR_BG;
const prog_char APM STR_MENUTOSTART[] = TR_MENUTOSTART;
const prog_char APM STR_SETMIDPOINT[] = TR_SETMIDPOINT;
const prog_char APM STR_MOVESTICKSPOTS[] = TR_MOVESTICKSPOTS;
const prog_char APM STR_ALT[] = TR_ALT;
const prog_char APM STR_RXBATT[] = TR_RXBATT;
const prog_char APM STR_RX[] = TR_RX;
const prog_char APM STR_TX[] = TR_TX;
const prog_char APM STR_HDG[] = TR_HDG;
const prog_char APM STR_SPD[] = TR_SPD;
const prog_char APM STR_TEMP1[] = TR_TEMP1;
const prog_char APM STR_TEMP2[] = TR_TEMP2;
const prog_char APM STR_RPM[] = TR_RPM;
const prog_char APM STR_FUEL[] = TR_FUEL;
const prog_char APM STR_VOLTS[] = TR_VOLTS;
const prog_char APM STR_ACCEL[] = TR_ACCEL;
const prog_char APM STR_TELE[] = TR_TELE;
const prog_char APM STR_RSSI[] = TR_RSSI;
const prog_char APM STR_NODATA[] = TR_NODATA;
const prog_char APM STR_TM1[] = TR_TM1;
const prog_char APM STR_TM2[] = TR_TM2;
const prog_char APM STR_THR[] = TR_THR;
const prog_char APM STR_TH[] = TR_TH;
const prog_char APM STR_TOT[] = TR_TOT;
const prog_char APM STR_TMR1LATMAXUS[] = TR_TMR1LATMAXUS;
const prog_char APM STR_TMR1LATMINUS[] = TR_TMR1LATMINUS;
const prog_char APM STR_TMR1JITTERUS[] = TR_TMR1JITTERUS;
const prog_char APM STR_TMAINMAXMS[] = TR_TMAINMAXMS;
#ifdef DEBUG
const prog_char APM STR_T10MSUS[] = TR_T10MSUS;
#endif
const prog_char APM STR_FREESTACKMINB[] = TR_FREESTACKMINB;
const prog_char APM STR_MENUTORESET[] = TR_MENUTORESET;
const prog_char APM STR_PPM[] = TR_PPM;
const prog_char APM STR_CH[] = TR_CH;
const prog_char APM STR_TMR[] = TR_TMR;
const prog_char APM STR_MODEL[] = TR_MODEL;
const prog_char APM STR_FP[] = TR_FP;
const prog_char APM STR_EEPROMLOWMEM[] = TR_EEPROMLOWMEM;
const prog_char APM STR_ALERT[] = TR_ALERT;
const prog_char APM STR_PRESSANYKEYTOSKIP[] = TR_PRESSANYKEYTOSKIP;
const prog_char APM STR_THROTTLENOTIDLE[] = TR_THROTTLENOTIDLE;
const prog_char APM STR_RESETTHROTTLE[] = TR_RESETTHROTTLE;
const prog_char APM STR_ALARMSDISABLED[] = TR_ALARMSDISABLED;
const prog_char APM STR_SWITCHESNOTOFF[] = TR_SWITCHESNOTOFF;
const prog_char APM STR_PLEASERESETTHEM[] = TR_PLEASERESETTHEM;
const prog_char APM STR_MESSAGE[] = TR_MESSAGE;
const prog_char APM STR_PRESSANYKEY[] = TR_PRESSANYKEY;
const prog_char APM STR_BADEEPROMDATA[] = TR_BADEEPROMDATA;
const prog_char APM STR_EEPROMFORMATTING[] = TR_EEPROMFORMATTING;
const prog_char APM STR_EEPROMOVERFLOW[] = TR_EEPROMOVERFLOW;
const prog_char APM STR_MENUSERROR[] = TR_MENUSERROR;

const prog_char APM STR_MENURADIOSETUP[] = TR_MENURADIOSETUP;

#ifdef PCBV3
const prog_char APM STR_MENUDATEANDTIME[] = TR_MENUDATEANDTIME;
#endif

const prog_char APM STR_MENUTRAINER[] = TR_MENUTRAINER;
const prog_char APM STR_MENUVERSION[] = TR_MENUVERSION;
const prog_char APM STR_MENUDIAG[] = TR_MENUDIAG;
const prog_char APM STR_MENUANA[] = TR_MENUANA;
const prog_char APM STR_MENUCALIBRATION[] = TR_MENUCALIBRATION;

const prog_char APM STR_MENUMODELSEL[] = TR_MENUMODELSEL;
const prog_char APM STR_MENUSETUP[] = TR_MENUSETUP;
const prog_char APM STR_MENUFLIGHTPHASE[] = TR_MENUFLIGHTPHASE;
const prog_char APM STR_MENUFLIGHTPHASES[] = TR_MENUFLIGHTPHASES;

#ifdef HELI
const prog_char APM STR_MENUHELISETUP[] = TR_MENUHELISETUP;
#endif

const prog_char APM STR_MENUDREXPO[] = TR_MENUDREXPO;
const prog_char APM STR_MENULIMITS[] = TR_MENULIMITS;
const prog_char APM STR_MENUCURVES[] = TR_MENUCURVES;
const prog_char APM STR_MENUCURVE[] = TR_MENUCURVE;
const prog_char APM STR_MENUCUSTOMSWITCHES[] = TR_MENUCUSTOMSWITCHES;
const prog_char APM STR_MENUFUNCSWITCHES[] = TR_MENUFUNCSWITCHES;
const prog_char APM STR_MENUSAFETYSWITCHES[] = TR_MENUSAFETYSWITCHES;

#ifdef FRSKY
const prog_char APM STR_MENUTELEMETRY[] = TR_MENUTELEMETRY;
#endif

#ifdef TEMPLATES
const prog_char APM STR_MENUTEMPLATES[] = TR_MENUTEMPLATES;
#endif

const prog_char APM STR_MENUSTAT[] = TR_MENUSTAT;
const prog_char APM STR_MENUDEBUG[] = TR_MENUDEBUG;

#if defined(DSM2) || defined(PXX)
const prog_char APM STR_RXNUM[] = TR_RXNUM;
#endif

#if defined(PXX)
const prog_char APM STR_SYNCMENU[] = TR_SYNCMENU;
#endif
