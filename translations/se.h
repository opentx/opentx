// NON ZERO TERMINATED STRINGS
#define LEN_OFFON        "\003"
#define TR_OFFON         "AV ""P\203 "

#define LEN_MMMINV       "\003"
#define TR_MMMINV        "---""INV"

#define LEN_NCHANNELS    "\004"
#define TR_NCHANNELS     "4CH 6KN 8KN 10KN12KN14KN16KN"

#define LEN_VBEEPMODE    "\005"
#define TR_VBEEPMODE     "Tyst ""Alarm""EjKnp""Alla "

#define LEN_VBEEPLEN     "\005"
#define TR_VBEEPLEN      "xKort""Kort ""Norml""L\200ng ""xL\200ng"

#define LEN_VRENAVIG     "\003"
#define TR_VRENAVIG      "No REaREb"

#define LEN_VFILTERADC   "\004"
#define TR_VFILTERADC    "SING""OSMP""FILT"

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
#if defined(PCBV4)
#if defined(EXTRA_ROTARY_ENCODERS)
#define TR_RETA123       "RETA123abcde"
#else //EXTRA_ROTARY_ENCODERS
#define TR_RETA123       "RETA123ab"
#endif //EXTRA_ROTARY_ENCODERS
#else
#define TR_RETA123       "RETA123"
#endif

#define LEN_VPROTOS      "\006"
#ifdef PXX
#define TR_PXX  "PXX\0  "
#else
#define TR_PXX  "[PXX]\0"
#endif
#ifdef DSM2
#define TR_DSM2 "DSM2\0 "
#else
#define TR_DSM2 "[DSM2]"
#endif
#define TR_VPROTOS       "PPM\0  ""PPM16\0""PPMsim" TR_PXX TR_DSM2

#define LEN_POSNEG       "\003"
#define TR_POSNEG        "POS""NEG"

#define LEN_VCURVEFUNC   "\003"
#define TR_VCURVEFUNC    "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_CURVMODES    "\005"
#define TR_CURVMODES     "EDIT ""PRSET""A.THR"

#define LEN_EXPLABELS    "\006"
#ifdef FLIGHT_PHASES
#define TR_EXPLABEL_FP   "Fas   "
#else
#define TR_EXPLABEL_FP
#endif
#define TR_EXPLABELS     "Vikt  ""Expo  ""Kurva " TR_EXPLABEL_FP "Brytar""N\201r   ""      " // TODO remove all the trailing spaces

#define LEN_VMLTPX       "\010"
#define TR_VMLTPX        "Addera  ""F\202rst\201rk""Ers\201tt  "

#define LEN_VMLTPX2      "\002"
#define TR_VMLTPX2       "+=""*="":="

#define LEN_VMIXTRIMS    "\006"
#define TR_VMIXTRIMS     "P\203    ""AV    ""Offset"

#define LEN_VCSWFUNC     "\007"
#define TR_VCSWFUNC      "----\0  ""v>ofs  ""v<ofs  ""(v)>ofs""(v)<ofs""AND    ""OR     ""XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 "

#define LEN_VFSWFUNC     "\015"
#if defined(VARIO)
#define TR_VVARIO         "Vario        "
#else
#define TR_VVARIO         "[Vario]      "
#endif
#if defined(AUDIO)
#define TR_SOUND         "Play Sound\0  "
#else
#define TR_SOUND         "Beep\0        "
#endif
#if defined(HAPTIC)
#define TR_HAPTIC        "Haptic\0      "
#else
#define TR_HAPTIC        "[Haptic]\0    "
#endif
#if defined(PCBV4)
#if defined(SDCARD)
#define TR_SDCLOGS       "SDCARD Logs  "
#else
#define TR_SDCLOGS       "[SDCARD Logs]"
#endif
#if defined(SOMO)
#define TR_SOMO          "Spela Sp\200r\0  "
#else
#define TR_SOMO          "[Spela Sp\200r]\0"
#endif
#else
#define TR_SDCLOGS
#define TR_SOMO
#endif
#ifdef DEBUG
#define TR_TEST          "Test\0        "
#else
#define TR_TEST
#endif
#define TR_VFSWFUNC      "S\201kerhet \0   ""Trainer \0    ""Instant Trim " TR_SOUND TR_HAPTIC TR_SOMO "Reset\0       " TR_VVARIO TR_SDCLOGS TR_TEST

#define LEN_VFSWRESET    "\006"
#define TR_VFSWRESET     "Timer1""Timer2""All   ""Telem."

#define LEN_FUNCSOUNDS   "\006"
#define TR_FUNCSOUNDS    "Varn1 ""Varn2 ""F\200r   ""Ring  ""SciFi ""Robot ""Pip   ""Tada  ""Syrsa ""Siren ""Alarm ""Ratata""Tick  "

#define LEN_VTELEMCHNS   "\004"
#define TR_VTELEMCHNS    "---\0""Tmr1""Tmr2""A1\0 ""A2\0 ""Tx\0 ""Rx\0 ""Alt\0""Rpm\0""Tank""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""AccX""AccY""AccZ""Hdg\0""VSpd""A1-\0""A2-\0""Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+""Acc\0""Tid\0"

#ifdef IMPERIAL_UNITS
#define LENGTH_UNIT "ft\0"
#define SPEED_UNIT  "kts"
#else
#define LENGTH_UNIT "m\0 "
#define SPEED_UNIT  "kmh"
#endif

#define LEN_VTELEMUNIT   "\003"
#define TR_VTELEMUNIT    "v\0 ""A\0 ""m/s""-\0 " SPEED_UNIT LENGTH_UNIT "@\0 ""%\0 ""mA\0"
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
#else
#define TR_VTELPROTO     ""
#endif

#define LEN_VARIOSRC     "\006"
#define TR_VARIOSRC      "BaroV1""BaroV2""A1    ""A2    "

#define LEN_GPSFORMAT    "\004"
#define TR_GPSFORMAT     "HMS NMEA"

#define LEN_VUNITSFORMAT "\004"
#define TR_VUNITSFORMAT  "MetrImpr"

#define LEN2_VTEMPLATES  13
#define LEN_VTEMPLATES   "\015"
#define TR_VTEMPLATES    "Nolla Mixar\0\0""Enkel 4kanal\0""Gasklippning\0""V-Stj\201rt    \0""Deltavinge  \0""eCCPM       \0""Helikopter  \0""Servotest   \0"

#define LEN_VSWASHTYPE   "\004"
#define TR_VSWASHTYPE    "--- ""120 ""120X""140 ""90  "

#define LEN_VKEYS        "\005"
#define TR_VKEYS         " MENU"" EXIT""  Ned""  Upp""H\202ger""V\201nst"

#define LEN_RE1RE2       "\003"
#define TR_RE1RE2        "RE1""RE2"

#define LEN_VSWITCHES    "\003"
#if defined(PCBARM)
#define TR_VSWITCHES     "GAS""SID""H\205J""ID0""ID1""ID2""SKE""LAN""TRN""BR1""BR2""BR3""BR4""BR5""BR6""BR7""BR8""BR9""BRA""BRB""BRC""BRD""BRE""BRF""BRG""BRH""BRI""BRJ""BRK""BRL""BRM""BRN""BRO""BRP""BRQ""BRR""BRS""BRT""BRU""BRV""BRW"
#else
#define TR_VSWITCHES     "GAS""SID""H\205J""ID0""ID1""ID2""SKE""LAN""TRN""BR1""BR2""BR3""BR4""BR5""BR6""BR7""BR8""BR9""BRA""BRB""BRC"
#endif

#define LEN_VSRCRAW      "\004"
#if defined(PCBV4)
#if defined(EXTRA_ROTARY_ENCODERS)
#define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb ""REc ""REd ""REe "
#else
#define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb "
#endif
#else
#define TR_ROTARY_ENCODERS_VSRCRAW
#endif
#if defined(HELI)
#define TR_CYC_VSRCRAW "CYK1""CYK2""CYK3"
#else
#define TR_CYC_VSRCRAW "[C1]""[C2]""[C3]"
#endif
#define TR_VSRCRAW       "SID ""H\205J ""GAS ""SKE ""P1  ""P2  ""P3  " TR_ROTARY_ENCODERS_VSRCRAW "MAX ""FULL" TR_CYC_VSRCRAW

#define LEN_VTMRMODES    "\003"
#define TR_VTMRMODES     "AV ""ABS""THs""TH%""THt"

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
#define TR_DISPLAY      "Display"
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
#define TR_HAPTICLENGTH "Vibrator tid"
#define TR_CONTRAST     "Kontrast"
#define TR_BATTERYWARNING "Batterivarning"
#define TR_INACTIVITYALARM "Inaktivitetslarm"
#define TR_RENAVIG      "Navig RotEnc"
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
#define TR_TIMEZONE     "Tidszon"
#define TR_RXCHANNELORD "Kanalordning RX"
#define TR_SLAVE        "Slav"
#define TR_MODESRC      "effekt % k\201lla"
#define TR_MULTIPLIER   "F\202rst\201rkn."
#define TR_CAL          "Cal"
#define TR_EEPROMV      "EEPROM v"
#define TR_VTRIM        "Trim- +"
#define TR_BG           "BG:"
#define TR_MENUTOSTART  "[MENU] STARTAR"
#define TR_SETMIDPOINT  " CENTRERA "
#define TR_MOVESTICKSPOTS "R\205R SPAKAR/RATTAR"
#define TR_RXBATT       "Rx Batt:"
#define TR_TXnRX        "Tx:\0Rx:"
#define OFS_RX          4
#define TR_ACCEL        "Acc:"
#define TR_NODATA       "NO DATA"
#define TR_TM1TM2       "TM1\015TM2"
#define TR_THRTHP       "THR\015TH%"
#define TR_TOT          "TOT"
#define TR_TMR1LATMAXUS "Tmr1Lat max\003us"
#define STR_US (STR_TMR1LATMAXUS+12)
#define TR_TMR1LATMINUS "Tmr1Lat min\003us"
#define TR_TMR1JITTERUS "Tmr1 Jitter\003us"
#define TR_TMAINMAXMS   "Tmain max\005ms"
#define TR_T10MSUS      "T10ms\007us"
#define TR_FREESTACKMINB "Free Stack\004b"
#define TR_MENUTORESET  "[MENU] NOLLAR  "
#define TR_PPM          "PPM"
#define TR_CH           "KN"
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
#define TR_MENUANA      "ANAS"
#define TR_MENUCALIBRATION "KALIBRERING"
#define TR_MENUSERROR   "FEL I MENU"
#define TR_TRIMS2OFFSETS "Trims => Offsets"
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
#define STR_LIMIT        (STR_MINLIMIT)
#define TR_MAXLIMIT      "Maxiniv\200"
#define TR_MINRSSI       "Min Rssi"
#define TR_LATITUDE      "Breddgrad"
#define TR_LONGITUDE     "L\201ngdgrad"
#define TR_GPSCOORD      "GPS-Koord."
#define TR_VARIO         "Vario"
#define TR_UNITSFORMAT   "Units Format"
#define TR_SHUTDOWN      "ST\204NGER AV"
#define TR_BATT_CALIB    "Battery Calib"
#define TR_CURRENT_CALIB "Current Calib"
#define TR_CURRENT       "Sp\201nning"
#define TR_SELECT_MODEL  "Select Model"
#define TR_CREATE_MODEL  "Create Model"
#define TR_BACKUP_MODEL  "Backup Model"
#define TR_DELETE_MODEL  "Delete Model"
#define TR_RESTORE_MODEL "Restore Model"
#define TR_SDCARD_ERROR  "SDCARD Error"
#define TR_NO_SDCARD     "No SDCARD"
