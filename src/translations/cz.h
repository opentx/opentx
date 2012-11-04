// NON ZERO TERMINATED STRINGS
#define LEN_OFFON              "\003"
#define TR_OFFON               "VYP""ZAP"

#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""INV"

#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014CH\0016CH\0018CH10CH12CH14CH16CH"

#define LEN_VBEEPMODE          "\005"
#define TR_VBEEPMODE           "Tich\212""Alarm""BezKl""V\207e  "

#define LEN_VBEEPLEN           "\005"
#define TR_VBEEPLEN            "O====""=O===""==O==""===O=""====O"

#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "Ne REaREb"

#define LEN_VFILTERADC         "\004"
#define TR_VFILTERADC          "SING""OSMP""FILT"

#define LEN_VBLMODE            "\005"
#define TR_VBLMODE             "Vyp\0 ""Kl\200v.""P\200ky\0""Kl+P\200""Zap\0 "

#define LEN_TRNMODE            "\003"
#define TR_TRNMODE             " X "" +="" :="

#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "CH1CH2CH3CH4"

#define LEN_DATETIME           "\006"
#define TR_DATETIME            "DATUM:""\201AS:  "

#define LEN_VLCD               "\006"
#define TR_VLCD                "NormalOptrex"

#define LEN_VTRIMINC           "\007"
#define TR_VTRIMINC            "Expo   ""ExJemn\212""Jemn\212  ""St\206edn\204""Hrub\212  "

#define LEN_RETA123            "\001"
#if defined(PCBGRUVIN9X)
#if defined(EXTRA_ROTARY_ENCODERS)
#define TR_RETA123             "SVPK123abcd"
#else //EXTRA_ROTARY_ENCODERS
#define TR_RETA123             "SVPK123ab"
#endif //EXTRA_ROTARY_ENCODERS
#else
#define TR_RETA123             "SVPK123"
#endif

#define LEN_VPROTOS            "\006"
#ifdef PXX
#define TR_PXX                 "PXX\0  "
#else
#define TR_PXX                 "[PXX]\0"
#endif
#ifdef DSM2
#define TR_DSM2                "DSM2\0 "
#else
#define TR_DSM2                "[DSM2]"
#endif
#ifdef IRPROTOS
#define TR_IRPROTOS            "SILV  TRAC09PICZ  SWIFT\0"
#else
#define TR_IRPROTOS
#endif
#define TR_VPROTOS             "PPM\0  ""PPM16\0""PPMsim" TR_PXX TR_DSM2 TR_IRPROTOS

#define LEN_POSNEG             "\003"
#define TR_POSNEG              "POZ""NEG"

#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX             "\010"
#define TR_VMLTPX              "Se\201\204st  ""N\200sobit ""Zam\203nit  "

#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="

#define LEN_VMIXTRIMS          "\004"
#define TR_VMIXTRIMS           "VYP\0""ZAP\0""Sm\203r""V\212\207k""Plyn""K\206id"

#define LEN_VCSWFUNC           "\010"
#define TR_VCSWFUNC            "---\0    ""v>ofs\0  ""v<ofs\0  ""|v|>ofs\0""|v|<ofs\0""AND\0    ""OR\0     ""XOR\0    ""v1==v2\0 ""v1!=v2\0 ""v1>v2\0  ""v1<v2\0  ""v1>=v2\0 ""v1<=v2\0 ""d>=ofs\0 ""|d|>=ofs"

#define LEN_VFSWFUNC           "\015"
#if defined(VARIO)
#define TR_VVARIO               "Vario\0       "
#else
#define TR_VVARIO               "[Vario]\0     "
#endif
#if defined(AUDIO)
#define TR_SOUND               "P\206ehr\200t\0     "
#else
#define TR_SOUND               "P\204pnout\0     "
#endif
#if defined(HAPTIC)
#define TR_HAPTIC              "Vibrovat\0    "
#else
#define TR_HAPTIC              "[Vibrovat]\0  "
#endif
#if defined(VOICE)
#define TR_PLAY_TRACK          "Hr\200t Stopu\0  "
#define TR_PLAY_VALUE          "Hr\200t TTS\0    "
#else
#define TR_PLAY_TRACK          "[Hr\200t Stopu]\0"
#define TR_PLAY_VALUE          "[Hr\200t TTS]\0  "
#endif
#if defined(PCBSKY9X)
#if defined(SDCARD)
#define TR_SDCLOGS             "Logov\200n\204 SD\0 "
#else
#define TR_SDCLOGS             "[Logov\200n\204 SD]"
#endif
#define TR_FSW_VOLUME          "Hlasitost\0   "
#define TR_FSW_BG_MUSIC        "BgHudba\0     ""BgHudba Pauza"
#elif defined(PCBGRUVIN9X)
#if defined(SDCARD)
#define TR_SDCLOGS             "Logování SD\0 "
#else
#define TR_SDCLOGS             "[Logování SD]"
#endif
#define TR_FSW_VOLUME
#define TR_FSW_BG_MUSIC
#else
#define TR_SDCLOGS
#define TR_FSW_VOLUME
#define TR_FSW_BG_MUSIC
#endif
#ifdef GVARS
#define TR_FSW_ADJUST_GVAR     "Nastav \0     "
#else
#define TR_FSW_ADJUST_GVAR
#endif
#ifdef DEBUG
#define TR_FSW_TEST            "Test\0        "
#else
#define TR_FSW_TEST
#endif
#define TR_VFSWFUNC            "Bezpe\201\0      ""Tren\202r  \0    ""Instant Trim\0" TR_SOUND TR_HAPTIC "Reset\0       " TR_VVARIO TR_PLAY_TRACK TR_PLAY_VALUE TR_SDCLOGS TR_FSW_VOLUME "Podsv\203tlen\204\0 " TR_FSW_BG_MUSIC TR_FSW_ADJUST_GVAR TR_FSW_TEST

#define LEN_VFSWRESET          "\006"
#define TR_VFSWRESET           "Timer1""Timer2""V\207e   ""Telem."

#define LEN_FUNCSOUNDS         "\006"
#define TR_FUNCSOUNDS          "Beep1 ""Beep2 ""Beep3 ""Warn1 ""Warn2 ""Cheep ""Ring  ""SciFi ""Robot ""Chirp ""Tada  ""Crickt""Siren ""AlmClk""Ratata""Tick  "

#define LEN_VTELEMCHNS         "\004"
#define TR_VTELEMCHNS          "---\0""Tmr1""Tmr2""Tx\0 ""Rx\0 ""A1\0 ""A2\0 ""Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""A1-\0""A2-\0""Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+""Cur+""Acc\0""Time"

#ifdef IMPERIAL_UNITS
#define LENGTH_UNIT            "ft\0"
#define SPEED_UNIT             "kts"
#else
#define LENGTH_UNIT            "m\0 "
#define SPEED_UNIT             "kmh"
#endif

#define LEN_VTELEMUNIT         "\003"
#define TR_VTELEMUNIT          "v\0 ""A\0 ""m/s""-\0 " SPEED_UNIT LENGTH_UNIT "@\0 ""%\0 ""mA\0""mAh""W\0 "
#define STR_V                  (STR_VTELEMUNIT+1)
#define STR_A                  (STR_VTELEMUNIT+4)

#define LEN_VALARM             "\004"
#define TR_VALARM              "----""\217lut""oran""\201erv"

#define LEN_VALARMFN           "\001"
#define TR_VALARMFN            "<>"

#define LEN_VTELPROTO          "\007"
#define TR_VTELPROTO           "---\0   ""Hub\0   ""WSHHigh""Halcyon"

#define LEN_VOLTSRC            "\003"
#define TR_VOLTSRC             "---""A1\0""A2\0""FAS""Cel"

#define LEN_VARIOSRC           "\004"
#define TR_VARIOSRC            "Data""A1\0 ""A2\0"

#define LEN_VSCREEN            "\010"
#define TR_VSCREEN             "Hodnota ""Ukazatel"

#define LEN_GPSFORMAT          "\004"
#define TR_GPSFORMAT           "HMS NMEA"

#define LEN2_VTEMPLATES        13
#define LEN_VTEMPLATES         "\015"
#define TR_VTEMPLATES          "Smazat Mixy\0\0""Z\200kl. 4kan\200l\0""Sticky-T-Cut\0""V-Ocas      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE         "\004"
#define TR_VSWASHTYPE          "--- ""120 ""120X""140 ""90\0"

#define LEN_VKEYS              "\005"
#define TR_VKEYS               " Menu"" Exit"" Dol\211""Nhoru""Vprvo""Vlevo"

#define LEN_VRENCODERS         "\003"
#define TR_VRENCODERS          "REa""REb"

#define LEN_VSWITCHES          "\003"
#if defined(PCBSKY9X)
#define TR_VSWITCHES     "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""CS1""CS2""CS3""CS4""CS5""CS6""CS7""CS8""CS9""CSA""CSB""CSC""CSD""CSE""CSF""CSG""CSH""CSI""CSJ""CSK""CSL""CSM""CSN""CSO""CSP""CSQ""CSR""CSS""CST""CSU""CSV""CSW""ZAP"
#else
#define TR_VSWITCHES     "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""CS1""CS2""CS3""CS4""CS5""CS6""CS7""CS8""CS9""CSA""CSB""CSC""ZAP"
#endif

#define LEN_VSRCRAW            "\004"
#if defined(PCBSKY9X)
#define TR_ROTARY_ENCODERS_VSRCRAW "REa "
#elif defined(PCBGRUVIN9X) && defined(EXTRA_ROTARY_ENCODERS)
#define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb ""REc ""REd "
#elif defined(PCBGRUVIN9X) && !defined(EXTRA_ROTARY_ENCODERS)
#define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb "
#else
#define TR_ROTARY_ENCODERS_VSRCRAW
#endif
#if defined(HELI)
#define TR_CYC_VSRCRAW         "CYC1""CYC2""CYC3"
#else
#define TR_CYC_VSRCRAW         "[C1]""[C2]""[C3]"
#endif
#define TR_VSRCRAW             "Sm\203r""V\212\207k""Plyn""K\206id""Pot1""Pot2""Pot3" TR_ROTARY_ENCODERS_VSRCRAW "TrmS" "TrmV" "TrmP" "TrmK" "MAX ""3POS" TR_CYC_VSRCRAW

#define LEN_VTMRMODES          "\003"
#define TR_VTMRMODES           "VYP""ABS""THs""TH%""THt"

#define LEN_DSM2MODE           "\007"
#define TR_DSM2MODE            "LP4/LP5DSMonlyDSMX   "

// ZERO TERMINATED STRINGS
#define INDENT                 "\001"
#define LEN_INDENT             1
#define INDENT_WIDTH           (FW/2)

#define TR_POPUPS              "[MENU]\010[EXIT]"
#define OFS_EXIT               7
#define TR_MENUWHENDONE        "\001[MENU] > DAL\207\214"
#define TR_FREE                "voln\202:"
#define TR_DELETEMODEL         "SMAZAT MODEL"
#define TR_COPYINGMODEL        "Kop\204ruji model.."
#define TR_MOVINGMODEL         "P\206esouv\200m model."
#define TR_LOADINGMODEL        "Aktivuji model.."
#define TR_NAME                "Jm\202no"
#define TR_TIMER               "Stopky "
#define TR_ELIMITS             "Limit+25%"
#define TR_ETRIMS              "\207ir\207\204Trim"
#define TR_TRIMINC             "KrokTrimu"
#define TR_TTRACE              "StopaPlyn"
#define TR_TTRIM               "TrimPlynu"
#define TR_BEEPCTR             "P\204pSt\206ed"
#define TR_PROTO               INDENT"Protokol"
#define TR_PPMFRAME            "PPM frame"
#define TR_MS                  "ms"
#define TR_SWITCH              "Sp\204na\201"
#define TR_TRIMS               "Trimy"
#define TR_FADEIN              "P\206echod Zap"
#define TR_FADEOUT             "P\206echod Vyp"
#define TR_DEFAULT             "(v\212choz\204)"
#define TR_CHECKTRIMS          "\006Kont.\012Trimy"
#define OFS_CHECKTRIMS         (9*FW)
#define TR_SWASHTYPE           "Typ Cykliky"
#define TR_COLLECTIVE          "Kolektiv"
#define TR_SWASHRING           "Cyklika"
#define TR_ELEDIRECTION        "Sm\203r:V\212\207kovka"
#define TR_AILDIRECTION        "\012K\206id\202lka"
#define TR_COLDIRECTION        "\012Kolektiv"
#define TR_MODE                INDENT"M\205d"
#define TR_NOFREEEXPO          "Nen\204 voln\202 expo!"
#define TR_NOFREEMIXER         "Nen\204 voln\212 mix!"
#define TR_INSERTMIX           "VLO\217IT MIX "
#define TR_EDITMIX             "UPRAVIT MIX "
#define TR_SOURCE              INDENT"Zdroj"
#define TR_WEIGHT              "V\200ha"
#define TR_EXPO                "Expo"
#define TR_SIDE                "Strana"
#define TR_DIFFERENTIAL        "Dif.V\212chylek"
#define TR_OFFSET              INDENT"Ofset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DVex"
#define TR_CURVE               "K\206ivka"
#define TR_FPHASE              "F\200ze"
#define TR_MIXWARNING          "Varov\200n\204"
#define TR_OFF                 "VYP"
#define TR_MULTPX              "Mat.Operace"
#define TR_DELAYDOWN           "Zpo\217d\203n\204 Vyp"
#define TR_DELAYUP             "Zpo\217d\203n\204 Zap"
#define TR_SLOWDOWN            "Zpomalen\204(-)"
#define TR_SLOWUP              "Zpomalen\204(+)"
#define TR_MIXER               "MIXER"
#define TR_CV                  "CV"
#define TR_GV                  "GV"
#define TR_ACHANNEL            "A\004Kan\200l"
#define TR_RANGE               INDENT"Rozsah"
#define TR_BAR                 "Ukazatel"
#define TR_ALARM               INDENT"Alarm"
#define TR_USRDATA             "U\217ivData"
#define TR_BLADES              INDENT"ListyVr"
#define TR_SCREEN              "Panel "
#define TR_SOUND_LABEL         "Zvuk"
#define TR_LENGTH              INDENT"D\202lka"
#define TR_SPKRPITCH           INDENT"T\205n"
#define TR_HAPTIC_LABEL        "Vibrace"
#define TR_HAPTICSTRENGTH      INDENT"S\204la"
#define TR_CONTRAST            "Kontrast LCD"
#define TR_ALARMS_LABEL        "Alarmy"
#define TR_BATTERYWARNING      INDENT"Vybit\200 Baterie"
#define TR_INACTIVITYALARM     INDENT"Ne\201innost"
#define TR_MEMORYWARNING       INDENT"Pln\200 Pam\203t'"
#define TR_ALARMWARNING        INDENT"Vypnut\212 Zvuk"
#define TR_RENAVIG             "Navig. RotEnc"
#define TR_FILTERADC           "Filtr ADC"
#define TR_THROTTLEREVERSE     "Revers Plynu"
#define TR_BEEP_LABEL          "Upozorn\203n\204 Stopek"
#define TR_MINUTEBEEP          INDENT"Cel\200 minuta"
#define TR_BEEPCOUNTDOWN       INDENT"Odpo\201et \201asu"
#define TR_BACKLIGHT_LABEL     "Podsv\203tlen\204"
#define TR_BLDELAY             INDENT"Zhasnout po"
#define TR_SPLASHSCREEN        "\210vodn\204Logo"
#define TR_THROTTLEWARNING     "Upoz:Plyn"
#define TR_SWITCHWARNING       "+=Sp\204na\201e"
#define TR_TIMEZONE            "\201asov\202 p\200smo"
#define TR_RXCHANNELORD        "Po\206ad\204 Kan\200l\211"
#define TR_SLAVE               "Pod\206\204zen\212"
#define TR_MODESRC             " M\205d\006% Zdroj"
#define TR_MULTIPLIER          "N\200sobi\201"
#define TR_CAL                 "Kal."
#define TR_VTRIM               "Trim- +"
#define TR_BG                  "BG:"
#define TR_MENUTOSTART         "\001[MENU] = START"
#define TR_SETMIDPOINT         "NASTAV ST\215EDY"
#define TR_MOVESTICKSPOTS      "H\216BEJ P\213KY/POTY"
#define TR_RXBATT              "Rx Bat.:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              "NO DATA"
#define TR_TM1TM2              "TM1\032TM2"
#define TR_THRTHP              "THR\032TH%"
#define TR_TOT                 "TOT"
#define TR_TMR1LATMAXUS        "Tmr1Lat max\006us"
#define STR_US                 (STR_TMR1LATMAXUS+12)
#define TR_TMR1LATMINUS        "Tmr1Lat min\006us"
#define TR_TMR1JITTERUS        "Tmr1 Jitter\006us"
#if defined(PCBSKY9X)
#define TR_TMAINMAXMS          "Tmain max\010ms"
#else
#define TR_TMAINMAXMS          "Tmain max\012ms"
#endif
#define TR_T10MSUS             "T10ms\016us"
#define TR_FREESTACKMINB       "Free Stack\010b"
#define TR_MENUTORESET         "[MENU] >> Reset"
#define TR_PPM                 "PPM"
#define TR_CH                  "CH"
#define TR_MODEL               "MODEL"
#define TR_FP                  "FP"
#define TR_EEPROMLOWMEM        "Doch\200z\204 EEPROM"
#define TR_ALERT               "\006(!)POZOR"
#define TR_PRESSANYKEYTOSKIP   "Kl\200vesa >>> p\206esko\201it"
#define TR_THROTTLENOTIDLE     "P\200ka plynu je pohnut\200"
#define TR_ALARMSDISABLED      "Alarmy jsou zak\200z\200ny"
#define TR_PRESSANYKEY         "\006Stiskni Kl\200vesu"
#define TR_BADEEPROMDATA       "\006Chyba dat EEprom"
#define TR_EEPROMFORMATTING    " Formatov\200n\204 EEPROM"
#define TR_EEPROMOVERFLOW      "P\206etekla EEPROM"
#define TR_MENURADIOSETUP      "NASTAVEN\214 R\213DIA"
#define TR_MENUDATEANDTIME     "DATUM A \201AS"
#define TR_MENUTRAINER         "TREN\220R"
#define TR_MENUVERSION         "VERZE"
#define TR_MENUDIAG            "DIAG"
#define TR_MENUANA             "ANALOGY"
#define TR_MENUCALIBRATION     "KALIBRACE"
#define TR_TRIMS2OFFSETS       "Trimy => Subtrimy"
#define TR_MENUMODELSEL        "MODEL"
#define TR_MENUSETUP           "NASTAVEN\214"
#define TR_MENUFLIGHTPHASE     "LETOV\213 F\213ZE"
#define TR_MENUFLIGHTPHASES    "LETOV\220 F\213ZE"
#define TR_MENUHELISETUP       "HELI"
#define TR_MENUDREXPO          "DV/EXPO"
#define TR_MENULIMITS          "LIMITY"
#define TR_MENUCURVES          "K\215IVKY"
#define TR_MENUCURVE           "\003CV"
#define TR_MENUCUSTOMSWITCH    "VLASTN\214 SP\214NA\201"
#define TR_MENUCUSTOMSWITCHES  "VLASTN\214 SP\214NA\201E"
#define TR_MENUCUSTOMFUNC      "FUNKCE"
#define TR_MENUTELEMETRY       "TELEMETRIE"
#define TR_MENUTEMPLATES       "\207ABLONY"
#define TR_MENUSTAT            "STATISTIKA"
#define TR_MENUDEBUG           "DIAG"
#define TR_RXNUM               "RxNum"
#define TR_SYNCMENU            "Sync [MENU]"
#define TR_BACK                "Back"
#define TR_LIMIT               INDENT"Limit"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Latitude"
#define TR_LONGITUDE           "Longitude"
#define TR_GPSCOORD            "Gps Sou\206adnice"
#define TR_VARIO               "Vario"
#define TR_SHUTDOWN            "VYP\214N\213\N\204.."
#define TR_BATT_CALIB          "Kalib:Baterie"
#define TR_CURRENT_CALIB       " +=\006Proud"
#define TR_VOLTAGE             INDENT"Nap\203t\204"
#define TR_CURRENT             INDENT"Proud"
#define TR_SELECT_MODEL        "Vyber Model"
#define TR_CREATE_MODEL        "Vytvo\206 Model"
#define TR_BACKUP_MODEL        "Z\200lohuj Model"
#define TR_DELETE_MODEL        "Sma\217 Model" // TODO merged into DELETEMODEL?
#define TR_RESTORE_MODEL       "Obnov Model"
#define TR_SDCARD_ERROR        "Chyba SDkarty"
#define TR_NO_SDCARD           "Nen\204 SDkarta"
#define TR_INCOMPATIBLE        "Nekompatibiln\204"
#define TR_WARNING             "KONTROLA"
#define TR_EEPROMWARN          "EEPROM"
#define TR_THROTTLEWARN        "PLYNU"
#define TR_ALARMSWARN          "ALARMU"
#define TR_SWITCHWARN          "POZICE"
#define TR_INVERT_THR          "Invertovat Plyn?"
#define TR_SPEAKER_VOLUME      INDENT"Hlasitost"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          "Jas"
#define TR_CPU_TEMP            "Tepl. CPU\016>"
#define TR_CPU_CURRENT         "Proud\030>"
#define TR_CPU_MAH             "Spot\206eba"
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "Tepl. MB \016>"
#define TR_CAPAWARNING         "Alarm Kapacity"
#define TR_TEMPWARNING         "Alarm Teploty"
#define TR_FUNC                "Fce."
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Trv\200n\204"
#define TR_DELAY               "Zdr\217et"
#define TR_SD_CARD             "SD"
#define TR_SDHC_CARD           "SD-HC"
#define TR_NO_SOUNDS_ON_SD     "\217\200dn\212 zvuk na SD"
#define TR_NO_MODELS_ON_SD     "\217\200dn\212 model na SD"
#define TR_PLAY_FILE           "P\206ehr\200t"
#define TR_DELETE_FILE         "Odstranit"
#define TR_COPY_FILE           "Kop\204rovat"
#define TR_RENAME_FILE         "P\206ejmenovat"
#define TR_REMOVED             " odstran\203n"
#define TR_SD_INFO             "Informace"
#define TR_SD_FORMAT           "Form\200t"
#define TR_NA                  "[X]"
#define TR_HARDWARE            "HARDWARE"
#define TR_FORMATTING          "Form\200tovan\204.."
#define TR_TEMP_CALIB          " +=\006Teplota"
#define TR_TIME                "\201as"
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Typ:"
#define TR_SD_SPEED            "Rychlost:"
#define TR_SD_SECTORS          "Sektor\211 :"
#define TR_SD_SIZE             "Velikost:"
#define TR_CURVE_TYPE          TR_SD_TYPE
#define TR_GLOBAL_VARS         "Glob\200ln\204 prom\203nn\202"
#define TR_OWN                 "Sv\200"
