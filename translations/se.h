// NON ZERO TERMINATED STRINGS
#define LEN_OFFON        "\003"
#define TR_OFFON         "AV ""P\203 "

#define LEN_MMMINV       "\003"
#define TR_MMMINV        "---""INV"

#define LEN_NCHANNELS    "\004"
#define TR_NCHANNELS     "4CH 6KN 8KN 10KN12KN14KN16KN"

#define LEN_VBEEPMODE    "\006"
#define TR_VBEEPMODE     "Tyst  ""Alarm ""Ej Knp""Alla  "

#define LEN_VBEEPLEN     "\006"
#define TR_VBEEPLEN      "xKort ""Kort  ""Normal""L\200ng  ""xL\200ng "

#define LEN_ADCFILTER    "\004"
#define TR_ADCFILTER     "SING""OSMP""FILT"

#define LEN_WARNSW       "\004"
#define TR_WARNSW        "Ned ""AV  ""Upp "

#define LEN_TRNMODE      "\003"
#define TR_TRNMODE       " AV"" +="" :="

#define LEN_TRNCHN       "\003"
#define TR_TRNCHN        "kn1kn2kn3kn4"

#define LEN_DATETIME     "\005"
#define TR_DATETIME      "DAG: ""TID: "

#define LEN_VTRIMINC     "\006"
#define TR_VTRIMINC      "Exp   ""xFin  ""Fin   ""Medium""Grov  "

#define LEN_RETA123      "\001"
#define TR_RETA123       "RETA123"

#define LEN_VPROTOS      "\005"
#define TR_VPROTOS       "PPM  ""PXX  ""DSM2 ""PPM16"

#define LEN_POSNEG       "\003"
#define TR_POSNEG        "POS""NEG"

#define LEN_VCURVEFUNC   "\003"
#define TR_VCURVEFUNC    "---""x>0""x<0""\201x\201""f>0""f<0""\201f\201"

#define LEN_CURVMODES    "\005"
#define TR_CURVMODES     "EDIT ""PRSET""A.THR"

#define LEN_EXPLABELS    "\006"
#define TR_EXPLABELS     "Vikt  ""Expo  ""Kurva ""Fas   ""Brytar""N\201r   ""      " // TODO remove all the trailing spaces

#define LEN_VMLTPX       "\010"
#define TR_VMLTPX        "Addera  ""F\202rst\201rk""Ers\201tt  "

#define LEN_VMLTPX2      "\002"
#define TR_VMLTPX2       "+=""*="":="

#define LEN_VMIXTRIMS    "\006"
#define TR_VMIXTRIMS     "P\203    ""AV    ""Offset"

#define LEN_VCSWFUNC     "\007"
#define TR_VCSWFUNC      "----\0  ""v>ofs  ""v<ofs  ""(v)>ofs""(v)<ofs""AND    ""OR     ""XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 "

#define LEN_VFSWFUNC     "\015"
#ifdef LOGS
#define TR_SDCLOGGS      "SDCARD Logs  "
#else
#define TR_SDCLOGGS
#endif
#ifdef SOMO
#define TR_SOMO          "Spela Sp\200r\0  "
#else
#define TR_SOMO
#endif
#ifdef AUDIO
#define TR_SOUND         "Spela Ljud\0  "
#else
#define TR_SOUND         "Pip \0        "
#endif
#define TR_VFSWFUNC      "S\201kerhet \0   ""Trainer \0    ""Instant Trim " TR_SOUND TR_SOMO TR_SDCLOGGS

#define LEN_FUNCSOUNDS   "\006"
#define TR_FUNCSOUNDS    "Varn1 ""Varn2 ""F\200r   ""Ring  ""SciFi ""Robot ""Pip   ""Tada  ""Syrsa ""Siren ""Alarm ""Ratata""Tick  ""Vibr1 ""Vibr2 ""Vibr3 "

#define LEN_VTELEMBARS   "\004"
#define TR_VTELEMBARS    "---\0""A1\0 ""A2\0 ""H\205JD""VARV""TANK""T1\0 ""T2\0 ""FART""BATT"

#define LEN_VTELEMUNIT   "\003"
#define TR_VTELEMUNIT    "v\0 ""A\0 ""-\0 ""kts""kmh""M/h""m\0 ""@\0 ""%\0"
#define STR_V            (STR_VTELEMUNIT+1)
#define STR_A            (STR_VTELEMUNIT+4)

#define LEN_VALARM       "\003"
#define TR_VALARM        "---""Gul""Ora""R\202d"

#define LEN_VALARMFN     "\001"
#define TR_VALARMFN      "<>"

#define LEN_VTELPROTO    "\004"
#if defined(WS_HOW_HIGH)
#define TR_VTELPROTO     "--- Hub WSHH"
#elif defined(FRSKY_HUB)
#define TR_VTELPROTO     "--- Hub "
#endif

#define LEN_VTEMPLATES   "\014"
#define TR_VTEMPLATES    "Enkel 4kanal""Gasklippning""V-Stj\201rt    ""Deltavinge  ""eCCPM       ""Helikopter  ""Servotest   "

#define LEN_VSWASHTYPE   "\004"
#define TR_VSWASHTYPE    "--- ""120 ""120X""140 ""90  "

#define LEN_VKEYS        "\005"
#define TR_VKEYS         " MENU"" EXIT""  Ned""  Upp""H\202ger""V\201nst"

#define LEN_RE1RE2       "\003"
#define TR_RE1RE2        "RE1""RE2"

#define LEN_VSWITCHES    "\003"
#define TR_VSWITCHES     "GAS""SID""H\205J""ID0""ID1""ID2""SKE""LAN""TRN""BR1""BR2""BR3""BR4""BR5""BR6""BR7""BR8""BR9""BRA""BRB""BRC"

#define LEN_VSRCRAW      "\004"
#define TR_VSRCRAW       "SID ""H\205J ""GAS ""SKE ""P1  ""P2  ""P3  ""MAX ""FULL""CYK1""CYK2""CYK3"

#define LEN_VTMRMODES    "\003"
#define TR_VTMRMODES     "AV ""ABS""THs""TH%""THt"

#if defined(FRSKY_HUB)
#define LEN_TELEMCHNS    "\004"
#define TR_TELEMCHNS     "AD1 AD2 H\205JDVARVTANKT@1 T@2 FARTBATT"
#elif defined(WS_HOW_HIGH)
#define LEN_TELEMCHNS    "\003"
#define TR_TELEMCHNS     "AD1AD2H\205J"
#elif defined(FRSKY)
#define LEN_TELEMCHNS    "\003"
#define TR_TELEMCHNS     "AD1AD2"
#endif

#define LEN_DSM2MODE     "\007"
#define TR_DSM2MODE      "LP4/LP5DSMonlyDSMX   "

// ZERO TERMINATED STRINGS
#define TR_POPUPS       "[MENU]\004[EXIT]"
#define OFS_EXIT        7
#define TR_MENUWHENDONE "[MENU] AVSLUTAR"
#define TR_FREE         "Ledigt"
#define TR_DELETEMODEL  "RADERA MODELL"
#define TR_COPYINGMODEL "Kopierar Modell"
#define TR_MOVINGMODEL  "Flyttar Modell"
#define TR_LOADINGMODEL "Laddar Modell"
#define TR_NAME         "Namn"
#define TR_TIMER        "Timer"
#define TR_ELIMITS      "Gr\201nser++"
#define TR_ETRIMS       "Trimmar++"
#define TR_TRIMINC      "Trim\202kn."
#define TR_TTRACE       "F\202lj Gas"
#define TR_TTRIM        "GasTrim"
#define TR_BEEPCTR      "Centerpip"
#define TR_PROTO        "Protokoll"
#define TR_PPMFRAME     "PPM frame"
#define TR_MS           "ms"
#define TR_SWITCH       "Brytare"
#define TR_TRIMS        "Trimmar"
#define TR_FADEIN       "Tona In"
#define TR_FADEOUT      "Tona Ut"
#define TR_DEFAULT      "Standard"
#define TR_CHECKTRIMS   "Kolla\005Trimmar"
#define TR_SWASHTYPE    "Swashtyp"
#define TR_COLLECTIVE   "Kollektiv"
#define TR_SWASHRING    "Swashring"
#define TR_ELEDIRECTION "H\205JD Riktning"
#define TR_AILDIRECTION "SKEV Riktning"
#define TR_COLDIRECTION "KOLL Riktning"
#define TR_MODE         "Mod"
#define TR_NOFREEEXPO   "Expo saknas!"
#define TR_NOFREEMIXER  "Mixer saknas!"
#define TR_INSERTMIX    "ADDERA MIX"
#define TR_EDITMIX      "EDITERA MIX"
#define TR_SOURCE       "K\201lla"
#define TR_WEIGHT       "Vikt"
#define TR_MIXERWEIGHT  "Mixer-vikt"
#define TR_DIFFERENTIAL "Differential"
#define TR_OFFSET       "Offset"
#define TR_MIXEROFFSET  "Mixer-offset"
#define TR_DRWEIGHT     "DR-vikt"
#define TR_DREXPO       "DR-expo"
#define TR_TRIM         "Trim"
#define TR_CURVES       "Kurvor"
#define TR_FPHASE       "Flygfas"
#define TR_WARNING      "Varning"
#define TR_OFF          "AV "
#define TR_MULTPX       "Multpx"
#define TR_DELAYDOWN    "F\202rdr\202jn. Ned"
#define TR_DELAYUP      "F\202rdr\202jn. Upp"
#define TR_SLOWDOWN     "Tr\202ghet Ned"
#define TR_SLOWUP       "Tr\202ghet Upp"
#define TR_MIXER        "MIXER"
#define TR_CV           "KU"
#define TR_SW           "BR"
#define TR_ACHANNEL     "A\002kanal  "
#define TR_RANGE        "Omr\200de"
#define TR_BAR          "Data"
#define TR_ALARM        "Alarm"
#define TR_USRDATA      "Anv\201ndardata"
#define TR_BLADES       "Blad"
#define TR_BARS         "Staplar som visas"
#define TR_CLEARMIXMENU "NOLLA MIXAR [MENU]"
#ifdef AUDIO
#define TR_BEEPERMODE   "H\202gtalare l\201ge"
#define TR_BEEPERLEN    "H\202gtalare tid"
#define TR_SPKRPITCH    "H\202gtalare ton"
#else
#define TR_BEEPERMODE   "Summer l\201ge"
#define TR_BEEPERLEN    "Summer tid"
#endif
#define TR_HAPTICMODE   "Vibrator l\201ge"
#define TR_HAPTICSTRENGTH "Vibrator Niv\200"
#define TR_CONTRAST     "Kontrast"
#define TR_BATTERYWARNING "Batterivarning"
#define TR_INACTIVITYALARM "Inaktivitetslarm"
#define TR_FILTERADC    "ADC-filter"
#define TR_THROTTLEREVERSE "Inverterad Gas"
#define TR_MINUTEBEEP   "Minutpip"
#define TR_BEEPCOUNTDOWN "Nedr\201kningspip"
#define TR_FLASHONBEEP  "Pip vid blink"
#define TR_LIGHTSWITCH  "Ljusbrytare"
#define TR_LIGHTOFFAFTER "Ljus av efter"
#define TR_SPLASHSCREEN  "Bild vid start"
#define TR_THROTTLEWARNING "Gasvarning"
#define TR_SWITCHWARNING "Brytarvarning"
#define TR_MEMORYWARNING "Minnesvarning"
#define TR_ALARMWARNING "Alarmvarning"
#define TR_NODATAALARM  "NO DATA Varning"
#define TR_RXCHANNELORD "Kanalordning RX"
#define TR_SLAVE        "Slav"
#define TR_MODESRC      "effekt % k\201lla"
#define TR_MULTIPLIER   "F\202rst\201rkn."
#define TR_CAL          "Cal"
#define TR_EEPROMV      "EEPROM v"
#define TR_VTRIM        "Trim- +"
#define TR_BG           "BG"
#define TR_MENUTOSTART  "[MENU] STARTAR"
#define TR_SETMIDPOINT  " CENTRERA "
#define TR_MOVESTICKSPOTS "R\205R SPAKAR/RATTAR"
#define TR_ALTnDST      "Hjd:\010Dst:"
#define TR_RXBATT       "Rx Batt:"
#define TR_TXnRX        "Tx:\0Rx:"
#define OFS_RX          4
#define TR_SPDnMAX      "Frt:\010Max:"
#define TR_TEMP1nTEMP2  "T@1:\007T@2:"
#define TR_RPMnFUEL     "RMP:\006Tank:"
#define TR_ACCEL        "Acc:"
#define TR_NODATA       "NO DATA"
#define TR_TM1TM2       "TM1\015TM2"
#define TR_THRTHP       "THR\015TH%"
#define TR_TOT          "TOT"
#define TR_TMR1LATMAXUS "tmr1Lat max\004us"
#define TR_TMR1LATMINUS "tmr1Lat min\004us"
#define TR_TMR1JITTERUS "tmr1 Jitter\004us"
#define TR_TMAINMAXMS   "tmain max\006ms"
#define TR_T10MSUS      "t10ms\010us"
#define TR_FREESTACKMINB "Free Stack min\004b"
#define TR_MENUTORESET  "[MENU] NOLLAR  "
#define TR_PPM          "PPM"
#define TR_CH           "KN"
#define TR_TMR          "TMR"
#define TR_MODEL        "MODELL"
#define TR_FP           "FF"
#define TR_EEPROMLOWMEM "EEPROM low mem"
#define TR_ALERT        " OBS"
#define TR_PRESSANYKEYTOSKIP "Tryck ned en knapp"
#define TR_THROTTLENOTIDLE "Gasen ej avst\201ngd!"
#define TR_RESETTHROTTLE "Sl\200 av gasen"
#define TR_ALARMSDISABLED "Alarmen Avst\201ngda!"
#define TR_SWITCHESNOTOFF "Brytare P\200slagna!"
#define TR_PLEASERESETTHEM "St\201ng av dem"
#define TR_MESSAGE      "MEDDELANDE"
#define TR_PRESSANYKEY  "Tryck ned en knapp"
#define TR_BADEEPROMDATA "EEprom Datafel "
#define TR_EEPROMFORMATTING "Formaterar EEprom"
#define TR_EEPROMOVERFLOW "Fel i EEprom"
#define TR_MENURADIOSETUP "INST\204LLNINGAR"
#define TR_MENUDATEANDTIME "DAG OCH TID"
#define TR_MENUTRAINER  "TRAINER (PPM IN)"
#define TR_MENUVERSION  "VERSION"
#define TR_MENUDIAG     "DIAGNOS"
#define TR_MENUANA      "ANA"
#define TR_MENUCALIBRATION "KALIBRERING"
#define TR_MENUSERROR   "FEL I MENU"
#define TR_MENUMODELSEL "MODELL"
#define TR_MENUSETUP    "KONF."
#define TR_MENUFLIGHTPHASE "FLYGFAS"
#define TR_MENUFLIGHTPHASES "FLYGFASER"
#define TR_MENUHELISETUP "HELIKOPTER"
#define TR_MENUDREXPO   "DR/EXPO" 
#define TR_MENULIMITS   "GR\204NSER"
#define TR_MENUCURVES   "KURVOR"
#define TR_MENUCURVE    "KURVA"
#define TR_MENUCUSTOMSWITCHES "SPEC.BRYTARE"
#define TR_MENUFUNCSWITCHES "FUNK.BRYTARE"
#define TR_MENUTELEMETRY "TELEMETRI"
#define TR_MENUTEMPLATES "MALLAR"
#define TR_MENUSTAT      "STATSISTIK"
#define TR_MENUDEBUG     "DEBUG"
#define TR_RXNUM         "RxNum"
#define TR_SYNCMENU      "Synk [MENU]"
#define TR_BACK          "EXIT"
#define TR_MINLIMIT      "Mininiv\200"
#define TR_MAXLIMIT      "Maxiniv\200"
