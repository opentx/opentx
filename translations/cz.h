// NON ZERO TERMINATED STRINGS
#define LEN_OFFON        "\003"
#define TR_OFFON         "VYP""ZAP"

#define LEN_MMMINV       "\003"
#define TR_MMMINV        "---""INV"

#define LEN_NCHANNELS    "\004"
#define TR_NCHANNELS     "4CH 6CH 8CH 10CH12CH14CH16CH"

#define LEN_VBEEPMODE    "\005"
#define TR_VBEEPMODE     "Tichy""Alarm""BezKL""Vse  "

#define LEN_VBEEPLEN     "\005"
#define TR_VBEEPLEN      "O====""=O===""==O==""===O=""====O"

#define LEN_VRENAVIG     "\003"
#define TR_VRENAVIG      "Ne REaREb"

#define LEN_VFILTERADC   "\004"
#define TR_VFILTERADC    "SING""OSMP""FILT"

#define LEN_WARNSW       "\005"
#define TR_WARNSW        "Vpred""VYP  ""Vzad"

#define LEN_TRNMODE      "\003"
#define TR_TRNMODE       " X "" +="" :="

#define LEN_TRNCHN       "\003"
#define TR_TRNCHN        "ch1ch2ch3ch4"

#define LEN_DATETIME     "\006"
#define TR_DATETIME      "DATUM:""CAS:  "

#define LEN_VTRIMINC     "\007"
#define TR_VTRIMINC      "Exp    ""ExJemny""Jemny  ""Stredni""Hruby  "

#define LEN_RETA123      "\001"
#if defined(PCBV4)
#if defined(EXTRA_ROTARY_ENCODERS)
#define TR_RETA123       "SVPK123abcde"
#else //EXTRA_ROTARY_ENCODERS
#define TR_RETA123       "SVPK123ab"
#endif //EXTRA_ROTARY_ENCODERS
#else
#define TR_RETA123       "SVPK123"
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
#define TR_EXPLABEL_FP   "Faze  "
#else
#define TR_EXPLABEL_FP
#endif
#define TR_EXPLABELS     "Vaha  ""Expo  ""Krivka" TR_EXPLABEL_FP "Spinac""Strana""      " // TODO remove all the trailing spaces

#define LEN_VMLTPX       "\010"
#define TR_VMLTPX        "Secist  ""Nasobit ""Zamenit  "

#define LEN_VMLTPX2      "\002"
#define TR_VMLTPX2       "+=""*="":="

#define LEN_VMIXTRIMS    "\006"
#define TR_VMIXTRIMS     "ZAP   ""VYP   ""Offset"

#define LEN_VCSWFUNC     "\007"
#define TR_VCSWFUNC      "----\0  ""v>ofs  ""v<ofs  ""|v|>ofs""|v|<ofs""AND    ""OR     ""XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 "

#define LEN_VFSWFUNC     "\015"
#if defined(VARIO)
#define TR_VVARIO         "Vario        "
#else
#define TR_VVARIO         "[Vario]      "
#endif
#if defined(AUDIO)
#define TR_SOUND         "Prehrat\0     "
#else
#define TR_SOUND         "Pipnout\0     "
#endif
#if defined(HAPTIC)
#define TR_HAPTIC        "Vibrovat\0    "
#else
#define TR_HAPTIC        "[Vibrovat]\0  "
#endif
#if defined(PCBV4)
#if defined(SDCARD)
#define TR_SDCLOGS       "SDCARD Logs  "
#else
#define TR_SDCLOGS       "[SDCARD Logs]"
#endif
#if defined(SOMO)
#define TR_SOMO          "Play Track\0  "
#else
#define TR_SOMO          "[Play Track]\0"
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
#define TR_VFSWFUNC      "Bezpecny CH\0 ""Trener\0      ""Instant Trim\0" TR_SOUND TR_HAPTIC TR_SOMO "Reset\0       " TR_VVARIO TR_SDCLOGS TR_TEST

#define LEN_VFSWRESET    "\006"
#define TR_VFSWRESET     "Timer1""Timer2""Vse   ""Telem."

#define LEN_FUNCSOUNDS   "\006"
#define TR_FUNCSOUNDS    "Warn1 ""Warn2 ""Cheep ""Ring  ""SciFi ""Robot ""Chirp ""Tada  ""Crickt""Siren ""AlmClk""Ratata""Tick  "

#define LEN_VTELEMCHNS   "\004"
#define TR_VTELEMCHNS    "---\0""Tmr1""Tmr2""A1\0 ""A2\0 ""Tx\0 ""Rx\0 ""Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""AccX""AccY""AccZ""Hdg\0""VSpd""A1-\0""A2-\0""Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+""Acc\0""Time"

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

#define LEN_VALARM       "\004"
#define TR_VALARM        "----""Zlut""Oran""Cerv"

#define LEN_VALARMFN     "\001"
#define TR_VALARMFN      "<>"

#define LEN_VTELPROTO    "\004"
#if defined(WS_HOW_HIGH)
#define TR_VTELPROTO     "----Hub WSHH"
#elif defined(FRSKY_HUB)
#define TR_VTELPROTO     "----Hub "
#else
#define TR_VTELPROTO     ""
#endif

#define LEN_VARIOSRC     "\006"
#define TR_VARIOSRC      "BaroV1""BaroV2""A1    ""A2    "

#define LEN_GPSFORMAT    "\004"
#define TR_GPSFORMAT     "HMS NMEA"

#define LEN2_VTEMPLATES  13
#define LEN_VTEMPLATES   "\015"
#define TR_VTEMPLATES    "Smazat Mixy\0\0""Zakladni 4CH\0""T-Cut       \0""V-Ocas      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE   "\004"
#define TR_VSWASHTYPE    "--- ""120 ""120X""140 ""90  "

#define LEN_VKEYS        "\005"
#define TR_VKEYS         " Menu"" Exit"" Dolu""Nhoru""Vprvo""Vlevo"

#define LEN_RE1RE2       "\003"
#define TR_RE1RE2        "RE1""RE2"

#define LEN_VSWITCHES    "\003"
#if defined(PCBARM)
#define TR_VSWITCHES     "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC""SWD""SWE""SWF""SWG""SWH""SWI""SWJ""SWK""SWL""SWM""SWN""SWO""SWP""SWQ""SWR""SWS""SWT""SWU""SWV""SWW"
#else
#define TR_VSWITCHES     "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC"
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
#define TR_CYC_VSRCRAW "CYC1""CYC2""CYC3"
#else
#define TR_CYC_VSRCRAW "[C1]""[C2]""[C3]"
#endif
#define TR_VSRCRAW       "Smer""Vysk""Plyn""Krid""Pot1""Pot2""Pot3" TR_ROTARY_ENCODERS_VSRCRAW "MAX ""3POS" TR_CYC_VSRCRAW

#define LEN_VTMRMODES    "\003"
#define TR_VTMRMODES     "VYP""ABS""THs""TH%""THt"

#define LEN_DSM2MODE     "\007"
#define TR_DSM2MODE      "LP4/LP5DSMonlyDSMX   "

// ZERO TERMINATED STRINGS
#define TR_POPUPS       "[MENU]\004[EXIT]"
#define OFS_EXIT        7
#define TR_MENUWHENDONE "[MENU] = DOKONCIT"
#define TR_FREE         "volne"
#define TR_DELETEMODEL  "SMAZAT MODEL"
#define TR_COPYINGMODEL "Kopiruji model..."
#define TR_MOVINGMODEL  "Presouvam model..."
#define TR_LOADINGMODEL "Aktivuji model..."
#define TR_NAME         "Jmeno"
#define TR_TIMER        "Stopky "
#define TR_ELIMITS      "Limit+25%"
#define TR_ETRIMS       "SirsiTrim"
#define TR_TRIMINC      "KrokTrimu"
#define TR_TTRACE       "StopaPlyn"
#define TR_TTRIM        "TrimPlynu"
#define TR_BEEPCTR      "PipStred"
#define TR_PROTO        "Proto."
#define TR_PPMFRAME     "PPM frame"
#define TR_MS           "ms"
#define TR_SWITCH       "Spinac"
#define TR_TRIMS        "Trimy"
#define TR_FADEIN       "PrechodZap"
#define TR_FADEOUT      "PrechodVyp"
#define TR_DEFAULT      "(vychozi)"
#define TR_CHECKTRIMS   "Kont.\005Trimy"
#define TR_SWASHTYPE    "Swash Type"
#define TR_COLLECTIVE   "Collective"
#define TR_SWASHRING    "Swash Ring"
#define TR_ELEDIRECTION "ELE Direction"
#define TR_AILDIRECTION "AIL Direction"
#define TR_COLDIRECTION "COL Direction"
#define TR_MODE         "Mod"
#define TR_NOFREEEXPO   "Neni volne expo!"
#define TR_NOFREEMIXER  "Neni volny mix!"
#define TR_INSERTMIX    "VLOZIT MIX "
#define TR_EDITMIX      "UPRAVIT MIX "
#define TR_SOURCE       "Zdroj"
#define TR_WEIGHT       "Vaha"
#define TR_MIXERWEIGHT  "Vaha Mixu"
#define TR_DIFFERENTIAL "DifVych"
#define TR_OFFSET       "Ofset"
#define TR_MIXEROFFSET  "Offset Mixu"
#define TR_DRWEIGHT     "Vaha DV"
#define TR_DREXPO       "Expo DV"
#define TR_TRIM         "Trim"
#define TR_CURVES       "Krivka"
#define TR_FPHASE       "Faze"
#define TR_WARNING      "Varovani"
#define TR_OFF          "VYP"
#define TR_MULTPX       "Multpx"
#define TR_DELAYDOWN    "Zpozdeni Vyp"
#define TR_DELAYUP      "Zpozdeni Zap"
#define TR_SLOWDOWN     "Zpomal. Vyp"
#define TR_SLOWUP       "Zpomal. Zap"
#define TR_MIXER        "MIXER"
#define TR_CV           "CV"
#define TR_ACHANNEL     "A\002Kanal"
#define TR_RANGE        "Rozsah"
#define TR_BAR          "Bar"
#define TR_ALARM        "Alarm"
#define TR_USRDATA      "UzivData"
#define TR_BLADES       "Blades"
#define TR_BARS         "Ukazatele"
#define TR_DISPLAY      "Zobrazeni"
#ifdef AUDIO
#define TR_BEEPERMODE   "Repro Mod"
#define TR_BEEPERLEN    "Repro Delka"
#define TR_SPKRPITCH    "Repro Ton"
#else
#define TR_BEEPERMODE   "Mod Zvuku"
#define TR_BEEPERLEN    "Delka Zvuku"
#endif
#define TR_HAPTICMODE   "Mod Vibrace"
#define TR_HAPTICSTRENGTH "Sila Vibrace"
#define TR_HAPTICLENGTH "Delka Vibrace"
#define TR_CONTRAST     "Kontrast"
#define TR_BATTERYWARNING "Alarm Baterie"
#define TR_INACTIVITYALARM "UpozNaNecinnost"
#define TR_RENAVIG      "Navig RotEnc"
#define TR_FILTERADC    "Filtr ADC"
#define TR_THROTTLEREVERSE "Revers Plynu"
#define TR_MINUTEBEEP   "Pipak-Minuta"
#define TR_BEEPCOUNTDOWN "Pipak-Odecet"
#define TR_FLASHONBEEP  "BlikatPodsvetl."
#define TR_LIGHTSWITCH  "SpinacPodsvetl."
#define TR_LIGHTOFFAFTER "Zhasnout po"
#define TR_SPLASHSCREEN  "UvodniLogo"
#define TR_THROTTLEWARNING "UpozPlynu"
#define TR_SWITCHWARNING "KontrolaSpinacu"
#define TR_MEMORYWARNING "UpozNaPlnPamet"
#define TR_ALARMWARNING "UpozNaAlarm"
#define TR_NODATAALARM  "NO DATA Alarm"
#define TR_TIMEZONE     "Time Zone"
#define TR_RXCHANNELORD "Poradi Kanalu"
#define TR_SLAVE        "Podrizeny"
#define TR_MODESRC      " Mod\003% Zdroj"
#define TR_MULTIPLIER   "Nasobic"
#define TR_CAL          "Kalib"
#define TR_EEPROMV      "EEPROM v"
#define TR_VTRIM        "Trim- +"
#define TR_BG           "BG:"
#define TR_MENUTOSTART  " [MENU] = START"
#define TR_SETMIDPOINT  "NASTAV STREDY"
#define TR_MOVESTICKSPOTS "HYBEJ PAKY/POTY"
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
#define TR_MENUTORESET  "[MENU] to reset"
#define TR_PPM          "PPM"
#define TR_CH           "CH"
#define TR_MODEL        "MODEL"
#define TR_FP           "FP"
#define TR_EEPROMLOWMEM "EEPROM low mem"
#define TR_ALERT        "\007POZOR"
#define TR_PRESSANYKEYTOSKIP " Klavesa = preskocit"
#define TR_THROTTLENOTIDLE "Paka plynu je pohnuta."
#define TR_RESETTHROTTLE "  ..vynulujte plyn!"
#define TR_ALARMSDISABLED "Alarmy Zakazany"
#define TR_SWITCHESNOTOFF "  Spinace nejsou ve"
#define TR_PLEASERESETTHEM "   spravne poloze!"
#define TR_MESSAGE      "\004MESSAGE"
#define TR_PRESSANYKEY  "\004Stiskni Klavesu"
#define TR_BADEEPROMDATA "Chyba dat EEprom"
#define TR_EEPROMFORMATTING "Formatovani EEPROM"
#define TR_EEPROMOVERFLOW "Pretekla EEPROM"
#define TR_MENURADIOSETUP "NASTAVENI RADIA"
#define TR_MENUDATEANDTIME "DATUM A CAS"
#define TR_MENUTRAINER  "TRENER"
#define TR_MENUVERSION  "VERZE"
#define TR_MENUDIAG     "DIAG"
#define TR_MENUANA      "ANALOGY"
#define TR_MENUCALIBRATION "KALIBRACE"
#define TR_MENUSERROR   "MENUS OVERFLOW"
#define TR_TRIMS2OFFSETS "Trimy => Offsety"
#define TR_MENUMODELSEL "MODEL"
#define TR_MENUSETUP    "NASTAVENI"
#define TR_MENUFLIGHTPHASE "FAZE LETU"
#define TR_MENUFLIGHTPHASES "FAZE LETU"
#define TR_MENUHELISETUP "HELI SETUP"
#define TR_MENUDREXPO   "DV/EXPO" // TODO flash saving this string is 2 times here
#define TR_MENULIMITS   "LIMITY"
#define TR_MENUCURVES   "KRIVKY"
#define TR_MENUCURVE    "   CV"
#define TR_MENUCUSTOMSWITCHES "VLASTNI SPINACE"
#define TR_MENUFUNCSWITCHES "FUNKC.SPINACE"
#define TR_MENUTELEMETRY "TELEMETRIE"
#define TR_MENUTEMPLATES "SABLONY"
#define TR_MENUSTAT      "STATISTIKA"
#define TR_MENUDEBUG     "DIAG"
#define TR_RXNUM         "RxNum"
#define TR_SYNCMENU      "Sync [MENU]"
#define TR_BACK          "Back"
#define TR_MINLIMIT      "Min Limit"
#define STR_LIMIT        (STR_MINLIMIT+4)
#define TR_MAXLIMIT      "Max Limit"
#define TR_MINRSSI       "Min Rssi"
#define TR_LATITUDE      "Latitude"
#define TR_LONGITUDE     "Longitude"
#define TR_GPSCOORD      "Gps Coords"
#define TR_VARIO         "Vario"
#define TR_SHUTDOWN      "SHUTTING DOWN"
#define TR_BATT_CALIB    "Kalibrace Bat"
#define TR_CURRENT_CALIB "Current Calib"
#define TR_CURRENT       "Current"
#define TR_SELECT_MODEL  "Select Model"
#define TR_CREATE_MODEL  "Create Model"
#define TR_BACKUP_MODEL  "Backup Model"
#define TR_DELETE_MODEL  "Delete Model" // TODO merged into DELETEMODEL?
#define TR_RESTORE_MODEL "Restore Model"
#define TR_SDCARD_ERROR  "SDCARD Error"
#define TR_NO_SDCARD     "No SDCARD"
