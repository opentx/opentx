// NON ZERO TERMINATED STRINGS
#define LEN_OFFON                       "\003"
#define TR_OFFON                        "AUS""AN\0"

#define LEN_MMMINV                      "\003"
#define TR_MMMINV                       "---""INV"

#define LEN_NCHANNELS                   "\004"
#define TR_NCHANNELS                    "\0014CH\0016CH\0018CH10CH12CH14CH16CH"

#define LEN_VBEEPMODE                   "\005"
#define TR_VBEEPMODE                    "Stumm""Alarm""NoKey""Alle\0"

#define LEN_VBEEPLEN                    "\005"
#define TR_VBEEPLEN                     "0====""=0===""==0==""===0=""====0"

#define LEN_VRENAVIG                    "\003"
#define TR_VRENAVIG                     "AUSDGaDGb"

#define LEN_VFILTERADC                  "\004"
#define TR_VFILTERADC                   "SING""OSMP""FILT"

#define LEN_VBLMODE                     "\004"
#define TR_VBLMODE                      "OFF ""Tast""Knpl""Alle""ON\0"

#define LEN_TRNMODE                     "\003"
#define TR_TRNMODE                      "AUS"" +="" :="

#define LEN_TRNCHN                      "\003"
#define TR_TRNCHN                       "CH1CH2CH3CH4"

#define LEN_DATETIME                    "\005"
#define TR_DATETIME                     "DATE:""TIME:"

#define LEN_VLCD                        "\006"
#define TR_VLCD                         "NormalOptrex"

#define LEN_VTRIMINC                    "\006"
#define TR_VTRIMINC                     "Expo. ""X-Fein""Fein  ""Mittel""Grob  "

#define LEN_RETA123                     "\001"
#if defined(PCBGRUVIN9X)
#if defined(EXTRA_ROTARY_ENCODERS)
#define TR_RETA123                      "SHGQ123abcd"
#else //EXTRA_ROTARY_ENCODERS
#define TR_RETA123                      "SHGQ123ab"
#endif //EXTRA_ROTARY_ENCODERS
#else
#define TR_RETA123                      "SHGQ123"
#endif

#define LEN_VPROTOS                     "\006"
#ifdef PXX
#define TR_PXX                          "PXX\0  "
#else
#define TR_PXX                          "[PXX]\0"
#endif
#ifdef DSM2
#define TR_DSM2                         "DSM2\0 "
#else
#define TR_DSM2                         "[DSM2]"
#endif
#ifdef IRPROTOS
#define TR_IRPROTOS                     "SILV  TRAC09PICZ  SWIFT\0"
#else
#define TR_IRPROTOS
#endif
#define TR_VPROTOS                      "PPM\0  ""PPM16\0""PPMsim" TR_PXX TR_DSM2

#define LEN_POSNEG                      "\003"
#define TR_POSNEG                       "POS""NEG"

#define LEN_VCURVEFUNC                  "\003"
#define TR_VCURVEFUNC                   "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX                      "\010"
#define TR_VMLTPX                       "Addieren""Multipl.""Ersetzen"

#define LEN_VMLTPX2                     "\002"
#define TR_VMLTPX2                      "+=""*="":="

#define LEN_VMIXTRIMS                   "\003"
#define TR_VMIXTRIMS                    "AUS""AN ""StR""H\203R""Gas""QuR"

#define LEN_VCSWFUNC                    "\010"
#define TR_VCSWFUNC                     "---\0    ""v>ofs\0  ""v<ofs\0  ""|v|>ofs\0""|v|<ofs\0""UND\0    ""ODER\0   ""XOR\0    ""v1==v2\0 ""v1!=v2\0 ""v1>v2\0  ""v1<v2\0  ""v1>=v2\0 ""v1<=v2\0 ""d>=ofs\0 ""|d|>=ofs"

#define LEN_VFSWFUNC                    "\015"
#if defined(VARIO)
#define TR_VVARIO                       "Vario\0       "
#else
#define TR_VVARIO                       "[Vario]\0     "
#endif
#if defined(AUDIO)
#define TR_SOUND                        "Ton spiel.\0  "
#else
#define TR_SOUND                        "Piep\0        "
#endif
#if defined(HAPTIC)
#define TR_HAPTIC                       "Haptic\0      "
#else
#define TR_HAPTIC                       "[Haptic]\0    "
#endif
#if defined(VOICE)
#define TR_PLAY_TRACK                   "Ton spiel.\0  "
#define TR_PLAY_VALUE                   "Wert sagen\0  "
#else
#define TR_PLAY_TRACK                   "[Ton spiel.]\0"
#define TR_PLAY_VALUE                   "[Wert sagen]\0"
#endif
#if defined(PCBSKY9X)
#if defined(SDCARD)
#define TR_SDCLOGS                      "SD Aufz.\0    "
#else
#define TR_SDCLOGS                      "[SD Aufz.]\0  "
#endif
#define TR_FSW_VOLUME                   "Lautst\201rke\0  "
#define TR_FSW_BG_MUSIC                 "Musik Start\0 ""Musik Pause\0 "
#elif defined(PCBGRUVIN9X)
#if defined(SDCARD)
#define TR_SDCLOGS                      "SD Aufz.\0    "
#else
#define TR_SDCLOGS                      "[SD Aufz.]\0  "
#endif
#define TR_FSW_VOLUME
#define TR_FSW_BG_MUSIC
#else
#define TR_SDCLOGS
#define TR_FSW_VOLUME
#define TR_FSW_BG_MUSIC
#endif
#ifdef GVARS
#define TR_FSW_ADJUST_GVAR  		"\200ndere \0     "
#else
#define TR_FSW_ADJUST_GVAR
#endif
#ifdef DEBUG
#define TR_FSW_TEST                     "Test\0        "
#else
#define TR_FSW_TEST
#endif

#define TR_VFSWFUNC                     "Sicher\0      ""Lehrsch.\0    ""Instant. Trim" TR_SOUND TR_HAPTIC "R\205cksetz.\0   " TR_VVARIO TR_PLAY_TRACK TR_PLAY_VALUE TR_SDCLOGS TR_FSW_VOLUME "Beleuchtung\0 " TR_FSW_BG_MUSIC TR_FSW_ADJUST_GVAR TR_FSW_TEST

#define LEN_VFSWRESET                   "\006"
#define TR_VFSWRESET                    "S.Uhr1""S.Uhr2""Alle  ""Telem."

#define LEN_FUNCSOUNDS                  "\006"
#define TR_FUNCSOUNDS                   "Piep1\0""Piep2\0""Piep3\0""Warn1\0""Warn2\0""Cheep\0""Ring\0 ""SciFi\0""Robot\0""Chirp\0""Tada\0 ""Crickt""Siren\0""AlmClk""Ratata""Tick\0 "

#define LEN_VTELEMCHNS                  "\004"
#define TR_VTELEMCHNS                   "---\0""Uhr1""Uhr2""Tx\0 ""Rx\0 ""A1\0 ""A2\0 ""H\203he""Umdr""Stof""T1\0 ""T2\0 ""Gesc""Dist""GH\203h""Zell""Zels""Vfas""Strm""Verb""Leis""BesX""BesY""BesZ""Rich""VGes""A1-\0""A2-\0""H\203h-""H\203h+""Umd+""T1+\0""T2+\0""Ges+""Dst+""Str+""Besc""Zeit"

#ifdef IMPERIAL_UNITS
#define LENGTH_UNIT                     "ft\0"
#define SPEED_UNIT                      "kts"
#else
#define LENGTH_UNIT                     "m\0 "
#define SPEED_UNIT                      "kmh"
#endif

#define LEN_VTELEMUNIT                  "\003"
#define TR_VTELEMUNIT                   "v\0 ""A\0 ""m/s""-\0 " SPEED_UNIT LENGTH_UNIT "@\0 ""%\0 ""mA\0""mAh""W\0 "
#define STR_V                           (STR_VTELEMUNIT+1)
#define STR_A                           (STR_VTELEMUNIT+4)

#define LEN_VALARM                      "\003"
#define TR_VALARM                       "---""Gel""Org""Rot"

#define LEN_VALARMFN                    "\001"
#define TR_VALARMFN                     "<>"

#define LEN_VTELPROTO                   "\007"
#define TR_VTELPROTO                    "Kein\0  ""Hub\0   ""WSHHigh""Halcyon"

#define LEN_VOLTSRC                     "\003"
#define TR_VOLTSRC                      "---""A1\0""A2\0""FAS""Cel"

#define LEN_VARIOSRC                    "\005"
#define TR_VARIOSRC                     "Daten""A1\0  ""A2\0  "

#define LEN_VSCREEN      								"\004"
#define TR_VSCREEN       								"Wert""Str."

#define LEN_GPSFORMAT                   "\004"
#define TR_GPSFORMAT                    "HMS NMEA"

#define LEN2_VTEMPLATES                 15
#define LEN_VTEMPLATES                  "\017"
#define TR_VTEMPLATES                   "Misch. L\203sch.\0 ""Einfach. 4-CH\0 ""Feste Gassp.\0  ""V-Stabilisator\0""Delta - Xmix\0  ""eCCPM\0         ""Hubschrauber\0  ""Servo Test\0    "

#define LEN_VSWASHTYPE                  "\004"
#define TR_VSWASHTYPE                   "--- ""120 ""120X""140 ""90\0"

#define LEN_VKEYS                       "\005"
#define TR_VKEYS                        " Menu"" Exit""Unten"" Oben""  Re.""  Li."

#define LEN_VRENCODERS                  "\003"
#define TR_VRENCODERS                   "DGa""DGb"

#define LEN_VSWITCHES                   "\003"
#if defined(PCBSKY9X)
#define TR_VSWITCHES                    "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""PS1""PS2""PS3""PS4""PS5""PS6""PS7""PS8""PS9""PSA""PSB""PSC""PSD""PSE""PSF""PSG""PSH""PSI""PSJ""PSK""PSL""PSM""PSN""PSO""PSP""PSQ""PSR""PSS""PST""PSU""PSV""PSW"" ON"
#else
#define TR_VSWITCHES                    "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""PS1""PS2""PS3""PS4""PS5""PS6""PS7""PS8""PS9""PSA""PSB""PSC"" ON"
#endif

#define LEN_VSRCRAW                     "\004"
#if defined(PCBSKY9X)
#define TR_ROTARY_ENCODERS_VSRCRAW 			"DGa\0"
#elif defined(PCBGRUVIN9X) && defined(EXTRA_ROTARY_ENCODERS)
#define TR_ROTARY_ENCODERS_VSRCRAW "DGa ""DGb ""DGc ""DGd "
#elif defined(PCBGRUVIN9X) && !defined(EXTRA_ROTARY_ENCODERS)
#define TR_ROTARY_ENCODERS_VSRCRAW "DGa ""DGb "
#else
#define TR_ROTARY_ENCODERS_VSRCRAW
#endif
#if defined(HELI)
#define TR_CYC_VSRCRAW                  "ZYK1""ZYK2""ZYK3"
#else
#define TR_CYC_VSRCRAW                  "[C1]""[C2]""[C3]"
#endif
#define TR_VSRCRAW                      "StR\0""H\203R\0""Gas\0""QuR\0""P1\0 ""P2\0 ""P3\0 " TR_ROTARY_ENCODERS_VSRCRAW "TrmR" "TrmE" "TrmT" "TrmA" "MAX\0""3POS" TR_CYC_VSRCRAW

#define LEN_VTMRMODES                   "\003"
#define TR_VTMRMODES                    "AUS""ABS""GSs""GS%""GSt"

#define LEN_DSM2MODE                    "\007"
#define TR_DSM2MODE                     "LP4/LP5DSMonlyDSMX   "

// ZERO TERMINATED STRINGS
#define INDENT                 					"\001"
#define LEN_INDENT            					1
#define INDENT_WIDTH           					(FW/2)

#define TR_POPUPS                       "[MENU]\010[EXIT]"
#define OFS_EXIT                        7
#define TR_MENUWHENDONE                 "[MENU] > Weiter"
#define TR_FREE                         "frei"
#define TR_DELETEMODEL                  "Modell l\203schen?"
#define TR_COPYINGMODEL                 "Kopiere Modell"
#define TR_MOVINGMODEL                  "Schiebe Modell"
#define TR_LOADINGMODEL                 "Lade Modell..."
#define TR_NAME                         "Name"
#define TR_TIMER                        "Stoppuhr"
#define TR_ELIMITS                      "Erw.Limit"
#define TR_ETRIMS                       "Erw.Trims"
#define TR_TRIMINC                      "Trim Schr."
#define TR_TTRACE                       "Gasquelle"
#define TR_TTRIM                        "Gastrim"
#define TR_BEEPCTR                      "Mitt.Piep"
#define TR_PROTO                        INDENT"Proto."
#define TR_PPMFRAME                     "PPM Einst."
#define TR_MS                           "ms"
#define TR_SWITCH                       "Schalt."
#define TR_TRIMS                        "Trims"
#define TR_FADEIN                       "Einblendung"
#define TR_FADEOUT                      "Abblendung"
#define TR_DEFAULT                      "(Normal)"
#define TR_CHECKTRIMS                   "\006Trims\012Pr\205fen"
#define OFS_CHECKTRIMS                  (9*FW)
#define TR_SWASHTYPE                    "Taumelsch."
#define TR_COLLECTIVE                   "Pitch"
#define TR_SWASHRING                    "Zykl. Begrenz."
#define TR_ELEDIRECTION                 "Nick Richtung"
#define TR_AILDIRECTION                 "Roll Richtung"
#define TR_COLDIRECTION                 "Pit. Richtung"
#define TR_MODE                         INDENT"Modus"
#define TR_NOFREEEXPO                   "Expos voll!"
#define TR_NOFREEMIXER                  "Mischer voll!"
#define TR_INSERTMIX                    "MISCHER HINZ."
#define TR_EDITMIX                      "MISCHER"
#define TR_SOURCE                       INDENT"Quelle"
#define TR_WEIGHT                       "Gewicht"
#define TR_EXPO                         "Expo"
#define TR_SIDE                         "Seite"
#define TR_DIFFERENTIAL                 "Differz."
#define TR_OFFSET                       INDENT"Ausgl."
#define TR_TRIM                         "Trim"
#define TR_DREX                         "DRex"
#define TR_CURVE                        "Kurve"
#define TR_FPHASE                       "Phase"
#define TR_MIXWARNING                   "Warnung"
#define TR_OFF                          "AUS"
#define TR_MULTPX                       "Wirkung"
#define TR_DELAYDOWN                    "Verz. Unten"
#define TR_DELAYUP                      "Verz. Oben"
#define TR_SLOWDOWN                     "Langs.Unten"
#define TR_SLOWUP                       "Langs.Oben"
#define TR_MIXER                        "MISCHER"
#define TR_CV                           "KV"
#define TR_GV                           "GV"
#define TR_ACHANNEL                     "A"
#define TR_RANGE                        INDENT"Skala"
#define TR_BAR                          "Streifen"
#define TR_ALARM                        INDENT"Alarm"
#define TR_USRDATA                      "Daten"
#define TR_BLADES                       INDENT"Bl\201tter"
#define TR_SCREEN                       "Schirm"
#define TR_SOUND_LABEL         					"Ton"
#define TR_LENGTH	             					INDENT"Dauer"
#define TR_SPKRPITCH                    INDENT"Tonh\203he"
#define TR_HAPTIC_LABEL        					"Vibration"
#define TR_HAPTICSTRENGTH               INDENT"St\201rke"
#define TR_CONTRAST                     "Kontrast"
#define TR_ALARMS_LABEL        					"Alarm bei:"
#define TR_BATTERYWARNING               INDENT"Batterie leer"
#define TR_INACTIVITYALARM              INDENT"Inaktivit\201t"
#define TR_MEMORYWARNING       					INDENT"Speicher voll"
#define TR_ALARMWARNING        					INDENT"Ton aus"
#define TR_RENAVIG                      "Drehgeb. Navig."
#define TR_FILTERADC                    "ADC Filter"
#define TR_THROTTLEREVERSE              "Gasumkehrung"
#define TR_BEEP_LABEL          					"Stoppuhrpieper"
#define TR_MINUTEBEEP                   INDENT"Minuten"
#define TR_BEEPCOUNTDOWN                INDENT"Countdown"
#define TR_BACKLIGHT_LABEL     					"Beleuchtung"
#define TR_BLDELAY                      INDENT"Dauer"
#define TR_SPLASHSCREEN                 "Startbild"
#define TR_THROTTLEWARNING              "Gasalarm"
#define TR_SWITCHWARNING                "Sch.Alarm"
#define TR_TIMEZONE                     "Zeitzone"
#define TR_RXCHANNELORD                 "Kanalanordnung"
#define TR_SLAVE                        "Sch\205ler"
#define TR_MODESRC                      " Modus\002% Quelle"
#define TR_MULTIPLIER                   "Multiplikator"
#define TR_CAL                          "Kal."
#define TR_VTRIM                        "Trim- +"
#define TR_BG                           "BG:"
#define TR_MENUTOSTART                  "[MENU] > Starten"
#define TR_SETMIDPOINT                  "NEUTRALSTELLUNG"
#define TR_MOVESTICKSPOTS               "KNPL/POTIS BEWEGEN"
#define TR_RXBATT                       "Rx Batt:"
#define TR_TXnRX                        "Tx:\0Rx:"
#define OFS_RX                          4
#define TR_ACCEL                        "Acc:"
#define TR_NODATA                       "NO DATA"
#define TR_TM1TM2                       "TM1\032TM2"
#define TR_THRTHP                       "THR\032TH%"
#define TR_TOT                          "TOT"
#define TR_TMR1LATMAXUS                 "Tmr1Lat max\006us"
#define STR_US (STR_TMR1LATMAXUS+12)
#define TR_TMR1LATMINUS                 "Tmr1Lat min\006us"
#define TR_TMR1JITTERUS                 "Tmr1 Jitter\006us"
#if defined(PCBSKY9X)
#define TR_TMAINMAXMS                   "Tmain max\010ms"
#else
#define TR_TMAINMAXMS                   "Tmain max\012ms"
#endif
#define TR_T10MSUS                      "T10ms\016us"
#define TR_FREESTACKMINB                "Free Stack\010b"
#define TR_MENUTORESET                  "[MENU] f\205r Reset"
#define TR_PPM                          "PPM"
#define TR_CH                           "CH"
#define TR_MODEL                        "MODELL"
#define TR_FP                           "FP"
#define TR_EEPROMLOWMEM                 "EEPROM voll"
#define TR_ALERT                        "WARNUNG"
#define TR_PRESSANYKEYTOSKIP            "Taste dr\205cken"
#define TR_THROTTLENOTIDLE              "Gas nicht Null!"
#define TR_ALARMSDISABLED               "Alarme ausgeschaltet"
#define TR_PRESSANYKEY                  "Taste dr\205cken"
#define TR_BADEEPROMDATA                "EEPROM ung\205ltig"
#define TR_EEPROMFORMATTING             "EEPROM Initialisieren"
#define TR_EEPROMOVERFLOW               "EEPROM \204berlauf"
#define TR_MENURADIOSETUP               "SYSTEMEINST."
#define TR_MENUDATEANDTIME              "DATUM UND ZEIT"
#define TR_MENUTRAINER                  "LEHR/SCH\204L."
#define TR_MENUVERSION                  "VERSION"
#define TR_MENUDIAG                     "SCHALT."
#define TR_MENUANA                      "ANALOG TEST"
#define TR_MENUCALIBRATION              "KALIBRIERUNG"
#define TR_TRIMS2OFFSETS                "Trims => Ausgl."
#define TR_MENUMODELSEL                 "MODELLE"
#define TR_MENUSETUP                    "KONF."
#define TR_MENUFLIGHTPHASE              "FLUGPHASE"
#define TR_MENUFLIGHTPHASES             "FLUGPHASEN"
#define TR_MENUHELISETUP                "HUBSCHRAUBER"
#if defined(PPM_CENTER_ADJUSTABLE) || defined(PPM_LIMITS_SYMETRICAL) // The right menu titles for the gurus ...
#define TR_MENUDREXPO          "KN\204PPEL"
#define TR_MENULIMITS          "AUSG\200NGE"
#else
#define TR_MENUDREXPO          "DR/EXPO"
#define TR_MENULIMITS          "GRENZEN"
#endif
#define TR_MENUCURVES                   "KURVEN"
#define TR_MENUCURVE                    "KURVE"
#define TR_MENUCUSTOMSWITCH             "PROG.SCHALTER"
#define TR_MENUCUSTOMSWITCHES           TR_MENUCUSTOMSWITCH
#define TR_MENUCUSTOMFUNC             	"SPEZ. FUNKTIONEN"
#define TR_MENUTELEMETRY                "TELEMETRIE"
#define TR_MENUTEMPLATES                "VORLAGEN"
#define TR_MENUSTAT                     "STAT."
#define TR_MENUDEBUG                    "DEBUG"
#define TR_RXNUM                        "RxNum"
#define TR_SYNCMENU                     "Sync [MENU]"
#define TR_BACK                         "Zur\205ck"
#define TR_LIMIT                        INDENT"Limite"
#define TR_MINRSSI                      "Min. RSSI"
#define TR_LATITUDE                     "Latitude"
#define TR_LONGITUDE                    "Longitude"
#define TR_GPSCOORD                     "Gps Koord."
#define TR_VARIO                        "Vario"
#define TR_SHUTDOWN                     "HERUNTERFAHREN"
#define TR_BATT_CALIB                   "Batt. Kalib."
#define TR_CURRENT_CALIB                "Stromkalib."
#define TR_VOLTAGE                      INDENT"Spann."
#define TR_CURRENT                      INDENT"Strom"
#define TR_SELECT_MODEL                 "Modell W\201hlen"
#define TR_CREATE_MODEL                 "Neues Modell"
#define TR_BACKUP_MODEL                 "Modell Backup"
#define TR_DELETE_MODEL                 "Modell L\203schen" // TODO merged into DELETEMODEL?
#define TR_RESTORE_MODEL                "Modell Restore"
#define TR_SDCARD_ERROR                 "SDcard Fehler"
#define TR_NO_SDCARD                    "Keine SDcard"
#define TR_INCOMPATIBLE                 "Nicht kompatibel"
#define TR_WARNING                      "WARNUNG"
#define TR_EEPROMWARN                   "EEPROM"
#define TR_THROTTLEWARN                 "GAS"
#define TR_ALARMSWARN                   "ALARM"
#define TR_SWITCHWARN                   "SCHALTER"
#define TR_INVERT_THR                   "Gas umkehren?"
#define TR_SPEAKER_VOLUME      INDENT"Lautst\201rke"
#define TR_LCD                 "Bildschirm"
#define TR_BRIGHTNESS          "Helligkeit"
#define TR_CPU_TEMP            "CPU Temp.\016>"
#define TR_CPU_CURRENT         "Strom\022>"
#define TR_CPU_MAH             "Verbrauch"
#define TR_COPROC              "CoProz."
#define TR_COPROC_TEMP         "MB Temp. \016>"
#define TR_CAPAWARNING         INDENT "Kapaz. niedrig"
#define TR_TEMPWARNING         INDENT "Hohe Temp."
#define TR_FUNC                "Funktion"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Dauer"
#define TR_DELAY               "Verz\203g."
#define TR_SD_CARD             "SD Karte"
#define TR_SDHC_CARD           "SD-HC Karte"
#define TR_NO_SOUNDS_ON_SD     "Keine T\203ne auf SD"
#define TR_NO_MODELS_ON_SD     "Keine Modelle auf SD"
#define TR_PLAY_FILE           "Spielen"
#define TR_DELETE_FILE         "L\203schen"
#define TR_COPY_FILE           "Kopieren"
#define TR_RENAME_FILE         "Umbenennen"
#define TR_REMOVED             " gel\203scht"
#define TR_SD_INFO             "Information"
#define TR_SD_FORMAT           "Formattieren"
#define TR_NA                  "N/V"
#define TR_HARDWARE            "HARDWARE"
#define TR_FORMATTING          "Formattierung..."
#define TR_TEMP_CALIB          "Temp. Kalib."
#define TR_TIME                "Zeit"
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Typ:"
#define TR_SD_SPEED            "Geschw:"
#define TR_SD_SECTORS          "Sektoren:"
#define TR_SD_SIZE             "Gr\203\206e:"
#define TR_CURVE_TYPE          "Typ"
#define TR_GLOBAL_VARS         "Globale Variablen"
#define TR_OWN                 "Eigen"
#define TR_DATE                "Datum"
#define TR_ROTARY_ENCODER      "Drehgeber"
