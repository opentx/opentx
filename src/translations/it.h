#define LEN_OFFON        "\003"
#define TR_OFFON         "OFF""ON\0"

#define LEN_MMMINV       "\003"
#define TR_MMMINV        "---""INV"

#define LEN_NCHANNELS    "\004"
#define TR_NCHANNELS     "\0014CH\0016CH\0018CH10CH12CH14CH16CH"

#define LEN_VBEEPMODE    "\005"
#define TR_VBEEPMODE     "Silen""Avvis""Notst""Tutti"

#define LEN_VBEEPLEN     "\005"
#define TR_VBEEPLEN      "+Cort""Corta""Media""Lunga""+Lung"

#define LEN_VRENAVIG     "\003"
#define TR_VRENAVIG      "No REaREb"

#define LEN_VBLMODE      "\004"
#define TR_VBLMODE       "OFF ""Tast""Stks""Tutt""ON\0"

#define LEN_TRNMODE      "\003"
#define TR_TRNMODE       "off"" +="" :="

#define LEN_TRNCHN       "\003"
#define TR_TRNCHN        "ch1ch2ch3ch4"

#define LEN_DATETIME     "\005"
#define TR_DATETIME      "DATA:""ORA :"

#define LEN_VLCD         "\006"
#define TR_VLCD          "NormalOptrex"

#define LEN_VTRIMINC     "\006"
#define TR_VTRIMINC      "Exp   ""ExFine""Fine  ""Medio ""Ampio "

#define LEN_RETA123      "\001"
#if defined(PCBGRUVIN9X)
#if ROTARY_ENCODERS > 2
#define TR_RETA123       "RETA123abcd"
#else
#define TR_RETA123       "RETA123ab"
#endif
#else
#define TR_RETA123       "RETA123"
#endif

#define LEN_VPROTOS      "\006"
#ifdef PXX
#define TR_PXX "PXX\0  "
#else
#define TR_PXX "[PXX]\0"
#endif
#ifdef DSM2
#define TR_DSM2 "DSM2\0 "
#else
#define TR_DSM2 "[DSM2]"
#endif
#ifdef IRPROTOS
#define TR_IRPROTOS "SILV  TRAC09PICZ  SWIFT\0"
#else
#define TR_IRPROTOS
#endif
#define TR_VPROTOS       "PPM\0  ""PPM16\0""PPMsim" TR_PXX TR_DSM2 TR_IRPROTOS

#define LEN_POSNEG       "\003"
#define TR_POSNEG        "POS""NEG"

#define LEN_VCURVEFUNC   "\003"
#define TR_VCURVEFUNC    "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX       "\010"
#define TR_VMLTPX        "Aggiungi""Moltipl.""Riscrivi"

#define LEN_VMLTPX2      "\002"
#define TR_VMLTPX2       "+=""*="":="

#define LEN_VMIXTRIMS    "\003"
#define TR_VMIXTRIMS     "OFF""ON ""Rud""Ele""Thr""Ail"

#define LEN_VCSWFUNC     "\010"
#define TR_VCSWFUNC      "---\0    ""v>ofs\0  ""v<ofs\0  ""|v|>ofs\0""|v|<ofs\0""AND\0    ""OR\0     ""XOR\0    ""v1==v2\0 ""v1!=v2\0 ""v1>v2\0  ""v1<v2\0  ""v1>=v2\0 ""v1<=v2\0 ""d>=ofs\0 ""|d|>=ofs"

#define LEN_VFSWFUNC     "\015"
#if defined(VARIO)
#define TR_VVARIO        "Vario\0       "
#else
#define TR_VVARIO        "[Vario]\0     "
#endif
#if defined(AUDIO)
#define TR_SOUND         "Suona\0       "
#else
#define TR_SOUND         "Beep\0        "
#endif
#if defined(HAPTIC)
#define TR_HAPTIC        "Vibrazione\0  "
#else
#define TR_HAPTIC        "[Vibrazione]\0"
#endif
#if defined(VOICE)
#define TR_PLAY_TRACK    "Brano\0       "
#define TR_PLAY_VALUE    "LeggiValore\0 "
#else
#define TR_PLAY_TRACK    "[Brano]\0     "
#define TR_PLAY_VALUE    "[LeggiValore]"
#endif
#if defined(PCBSKY9X)
#if defined(SDCARD)
#define TR_SDCLOGS       "SDCARD Logs\0 "
#else
#define TR_SDCLOGS       "[SDCARD Logs]"
#endif
#define TR_FSW_VOLUME    "Volume\0      "
#define TR_FSW_BG_MUSIC  "BgMusic\0     ""BgMusic Pause"
#elif defined(PCBGRUVIN9X)
#if defined(SDCARD)
#define TR_SDCLOGS       "SDCARD Logs"
#else
#define TR_SDCLOGS       "[SDCARD Logs]"
#endif
#define TR_FSW_VOLUME
#define TR_FSW_BG_MUSIC
#else
#define TR_SDCLOGS
#define TR_FSW_VOLUME
#define TR_FSW_BG_MUSIC
#endif
#ifdef GVARS
#define TR_FSW_ADJUST_GVAR  "Adjust \0     "
#else
#define TR_FSW_ADJUST_GVAR
#endif
#ifdef DEBUG
#define TR_FSW_TEST          "Test\0        "
#else
#define TR_FSW_TEST
#endif
#define TR_VFSWFUNC      "Blocco\0      ""Trainer \0    ""Instant Trim " TR_SOUND TR_HAPTIC "Reset\0       " TR_VVARIO TR_PLAY_TRACK TR_PLAY_VALUE TR_SDCLOGS TR_FSW_VOLUME "Retroillum.\0 " TR_FSW_BG_MUSIC TR_FSW_ADJUST_GVAR TR_FSW_TEST

#define LEN_VFSWRESET    "\006"
#define TR_VFSWRESET     "Timer1""Timer2""Tutto ""Telem."

#define LEN_FUNCSOUNDS   "\006"
#define TR_FUNCSOUNDS    "Beep1 ""Beep2 ""Beep3 ""Warn1 ""Warn2 ""Cheep ""Ring  ""SciFi ""Robot ""Chirp ""Tada  ""Crickt""Siren ""AlmClk""Ratata""Tick  "

#define LEN_VTELEMCHNS   "\004"
#define TR_VTELEMCHNS    "---\0""Tmr1""Tmr2""Tx\0 ""Rx\0 ""A1\0 ""A2\0 ""Alt\0""Rpm\0""Carb""T1\0 ""T2\0 ""Vel\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsm""Powr""AccX""AccY""AccZ""Dir\0""VelV""A1-\0""A2-\0""Alt-""Alt+""Rpm+""T1+\0""T2+\0""Vel+""Dst+""Cur+""Acc\0""Time"

#ifdef IMPERIAL_UNITS
#define LENGTH_UNIT "ft\0"
#define SPEED_UNIT  "kts"
#else
#define LENGTH_UNIT "m\0 "
#define SPEED_UNIT  "kmh"
#endif

#define LEN_VTELEMUNIT   "\003"
#define TR_VTELEMUNIT    "v\0 ""A\0 ""m/s""-\0 " SPEED_UNIT LENGTH_UNIT "@\0 ""%\0 ""mA\0""mAh""W\0 "
#define STR_V            (STR_VTELEMUNIT+1)
#define STR_A            (STR_VTELEMUNIT+4)

#define LEN_VALARM       "\003"
#define TR_VALARM        "---""Yel""Org""Red"

#define LEN_VALARMFN     "\001"
#define TR_VALARMFN      "<>"

#define LEN_VTELPROTO    "\007"
#define TR_VTELPROTO     "---\0   ""Hub\0   ""WSHHigh""Halcyon"

#define LEN_VOLTSRC      "\003"
#define TR_VOLTSRC       "---""A1\0""A2\0""FAS""Cel"

#define LEN_VARIOSRC     "\004"
#define TR_VARIOSRC      "Data""A1\0 ""A2\0"

#define LEN_VSCREEN      "\004"
#define TR_VSCREEN       "Nums""Bars"

#define LEN_GPSFORMAT    "\004"
#define TR_GPSFORMAT     "HMS NMEA"

#define LEN2_VTEMPLATES  13
#define LEN_VTEMPLATES   "\015"
#define TR_VTEMPLATES    "Clear Mixes\0\0""Simple 4-CH \0""Sticky-T-Cut\0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE   "\004"
#define TR_VSWASHTYPE    "--- ""120 ""120X""140 ""90\0"

#define LEN_VKEYS        "\005"
#define TR_VKEYS         " Men\200"" EXIT""  DN ""   UP""   SX""   DX"

#define LEN_VRENCODERS       "\003"
#define TR_VRENCODERS        "REa""REb"

#define LEN_VSWITCHES    "\003"
#if defined(PCBSKY9X)
#define TR_VSWITCHES     "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC""SWD""SWE""SWF""SWG""SWH""SWI""SWJ""SWK""SWL""SWM""SWN""SWO""SWP""SWQ""SWR""SWS""SWT""SWU""SWV""SWW"" ON"
#else
#define TR_VSWITCHES     "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC"" ON"
#endif

#define LEN_VSRCRAW      "\004"
#if defined(PCBSKY9X)
#define TR_ROTARY_ENCODERS_VSRCRAW "REa "
#elif defined(PCBGRUVIN9X) && ROTARY_ENCODERS > 2
#define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb ""REc ""REd "
#elif defined(PCBGRUVIN9X) && ROTARY_ENCODERS <= 2
#define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb "
#else
#define TR_ROTARY_ENCODERS_VSRCRAW
#endif
#if defined(HELI)
#define TR_CYC_VSRCRAW "CYC1""CYC2""CYC3"
#else
#define TR_CYC_VSRCRAW "[C1]""[C2]""[C3]"
#endif
#define TR_VSRCRAW       "Rud ""Ele ""Thr ""Ail ""P1  ""P2  ""P3  " TR_ROTARY_ENCODERS_VSRCRAW "TrmR" "TrmE" "TrmT" "TrmA" "MAX ""3POS" TR_CYC_VSRCRAW

#define LEN_VTMRMODES    "\003"
#define TR_VTMRMODES     "OFF""ABS""THs""TH%""THt"

#define LEN_DSM2MODE     "\007"
#define TR_DSM2MODE      "LP4/LP5DSMonlyDSMX   "

// ZERO TERMINATED STRINGS
#define INDENT                 "\001"
#define LEN_INDENT             1
#define INDENT_WIDTH           (FW/2)

#define TR_POPUPS              "[Men\200]\004[Exit]"
#define OFS_EXIT               7
#define TR_MENUWHENDONE        CENTER"\006[Men\200] Conferma"
#define TR_FREE                " Disp."
#define TR_DELETEMODEL         "Elimina modello?"
#define TR_COPYINGMODEL        "Copia in corso.."
#define TR_MOVINGMODEL         "Spostamento..."
#define TR_LOADINGMODEL        "Caricamento..."
#define TR_NAME                "Nome"
#define TR_TIMER               "Timer"
#define TR_ELIMITS             "LimitiEx"
#define TR_ETRIMS              "TrimEx"
#define TR_TRIMINC             "Inc Trim"
#define TR_TTRACE              "T-Trace"
#define TR_TTRIM               "T-Trim"
#define TR_BEEPCTR             "Beep Ctr"
#define TR_PROTO               INDENT"Proto"
#define TR_PPMFRAME            "PPM frame"
#define TR_MS                  "ms"
#define TR_SWITCH              "Switch"
#define TR_TRIMS               "Trim"
#define TR_FADEIN              "Diss.In"
#define TR_BITMAP              "Bitmap"
#define TR_FADEOUT             "Diss.Out"
#define TR_DEFAULT             "(default)"
#define TR_CHECKTRIMS          "\003Contr.\004Trim"
#define OFS_CHECKTRIMS         (9*FW)
#define TR_SWASHTYPE           "Tipo Ciclico"
#define TR_COLLECTIVE          "Collettivo"
#define TR_SWASHRING           "AnelloCiclico"
#define TR_ELEDIRECTION        "Direzione ELE"
#define TR_AILDIRECTION        "Direzione AIL"
#define TR_COLDIRECTION        "Direzione COL"
#define TR_MODE                INDENT"Mode"
#define TR_NOFREEEXPO          "Expo pieni!"
#define TR_NOFREEMIXER         "Mixer pieni!"
#define TR_INSERTMIX           "INSER. MIX"
#define TR_EDITMIX             "MOD. MIX"
#define TR_SOURCE              INDENT"Sorg."
#define TR_WEIGHT              "Peso"
#define TR_EXPO                "Espo"
#define TR_SIDE                "Lato"
#define TR_DIFFERENTIAL        "Differ"
#define TR_OFFSET              INDENT"Offset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DRex"
#define TR_CURVE               "Curva"
#define TR_FPHASE              "Fase"
#define TR_MIXWARNING          "Avviso"
#define TR_OFF                 "OFF"
#define TR_MULTPX              "MultPx"
#define TR_DELAYDOWN           "Post. Gi\200 "
#define TR_DELAYUP             "Post. S\200"
#define TR_SLOWDOWN            "Rall. Gi\200 "
#define TR_SLOWUP              "Rall. S\200"
#define TR_MIXER               "MIXER"
#define TR_CV                  "CV"
#define TR_GV                  "GV"
#define TR_ACHANNEL            "A\002ingresso"
#define TR_RANGE               INDENT"Range"
#define TR_BAR                 "Barra"
#define TR_ALARM               INDENT"Allarme"
#define TR_USRDATA             "Dati"
#define TR_BLADES              INDENT"Pale"
#define TR_SCREEN              "Screen "
#define TR_SOUND_LABEL         "Suono"
#define TR_LENGTH              INDENT"Durata"
#define TR_SPKRPITCH           INDENT"Tono"
#define TR_HAPTIC_LABEL        "Vibraz"
#define TR_HAPTICSTRENGTH      INDENT"Forza"
#define TR_CONTRAST            "Contrasto"
#define TR_ALARMS_LABEL        "Allarmi"
#define TR_BATTERYWARNING      INDENT"Avviso batteria"
#define TR_INACTIVITYALARM     INDENT"Inattivita'"
#define TR_MEMORYWARNING       INDENT"Avviso memoria"
#define TR_ALARMWARNING        INDENT"Spento"
#define TR_RENAVIG             "Navig RotEnc "
#define TR_THROTTLEREVERSE     "Thr inverso"
#define TR_BEEP_LABEL          "Timer events"
#define TR_MINUTEBEEP          INDENT"Minuto"
#define TR_BEEPCOUNTDOWN       INDENT"Conto rov."
#define TR_BACKLIGHT_LABEL     "Retroillum."
#define TR_BLDELAY             INDENT"Durata"
#define TR_SPLASHSCREEN        "Schermata avvio"
#define TR_THROTTLEWARNING     "All. Thr"
#define TR_SWITCHWARNING       "Avv. Inter."
#define TR_TIMEZONE            "Ora locale"
#define TR_RXCHANNELORD        "Ordine ch RX"
#define TR_SLAVE               "Slave"
#define TR_MODESRC             "Mode\006% Source"
#define TR_MULTIPLIER          "Moltiplica"
#define TR_CAL                 "Cal"
#define TR_VTRIM               "Trim- +"
#define TR_BG                  "BG:"
#define TR_MENUTOSTART         CENTER"\006[Men\200] per Cal."
#define TR_SETMIDPOINT         CENTER"\011SETTA CENTRO"
#define TR_MOVESTICKSPOTS      CENTER"\006MUOVI STICK/POT"
#define TR_RXBATT              "Rx Batt:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              "NO DATA"
#define TR_TM1TM2              "TM1\032TM2"
#define TR_THRTHP              "THR\032TH%"
#define TR_TOT                 "TOT"
#define TR_TMR1LATMAXUS        "Tmr1Lat max\006us"
#define STR_US (STR_TMR1LATMAXUS+12)
#define TR_TMR1LATMINUS        "Tmr1Lat min\006us"
#define TR_TMR1JITTERUS        "Tmr1 Jitter\006us"
#if defined(PCBSKY9X)
#define TR_TMIXMAXMS           "Tmix max\012ms"
#else
#define TR_TMIXMAXMS           "Tmix max\014ms"
#endif
#define TR_T10MSUS             "T10ms\007us"
#define TR_FREESTACKMINB       "Free Stack\004b"
#define TR_MENUTORESET         "[Men\200] x Azzerare"
#define TR_PPM                 "PPM"
#define TR_CH                  "CH"
#define TR_MODEL               "MODELLO"
#define TR_FP                  "FV"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "EEPROM quasi piena!"
#define TR_ALERT               "\016ALERT"
#define TR_PRESSANYKEYTOSKIP   "\010Premi un tasto"
#define TR_THROTTLENOTIDLE     "Throttle non in pos."
#define TR_ALARMSDISABLED      "Avvisi Disattivati!"
#define TR_PRESSANYKEY         "\010Premi un tasto"
#define TR_BADEEPROMDATA       "Dati corrotti!"
#define TR_EEPROMFORMATTING    "Formatto EEPROM..."
#define TR_EEPROMOVERFLOW      "EEPROM Piena"
#define TR_MENURADIOSETUP      "CONFIGURATX"
#define TR_MENUDATEANDTIME     "DATA E ORA"
#define TR_MENUTRAINER         "TRAINER"
#define TR_MENUVERSION         "VERSIONE"
#define TR_MENUDIAG            "DIAG"
#define TR_MENUANA             "ANAS"
#define TR_MENUCALIBRATION     "CALIBRAZIONE"
#define TR_TRIMS2OFFSETS       "\006Trim  => Offset "
#define TR_MENUMODELSEL        "MODELLI"
#define TR_MENUSETUP           "SETUP"
#define TR_MENUFLIGHTPHASE     "FASE DI VOLO"
#define TR_MENUFLIGHTPHASES    "FASI DI VOLO"
#define TR_MENUHELISETUP       "SETUP  ELI"
#if defined(PPM_CENTER_ADJUSTABLE) || defined(PPM_LIMITS_SYMETRICAL) // The right menu titles for the gurus ...
#define TR_MENUDREXPO          "STICK"
#define TR_MENULIMITS          "USCITE"
#else
#define TR_MENUDREXPO          "DR/ESPO"
#define TR_MENULIMITS          "LIMITI"
#endif
#define TR_MENUCURVES          "CURVE"
#define TR_MENUCURVE           "CURVA"
#define TR_MENUCUSTOMSWITCH    "SWITCH PERSON."
#define TR_MENUCUSTOMSWITCHES  "SWITCH PERSON."
#define TR_MENUCUSTOMFUNC      "SWITCH FUNZIONE"
#define TR_MENUTELEMETRY       "TELEMETRIA"
#define TR_MENUTEMPLATES       "TEMPLATES"
#define TR_MENUSTAT            "STATS"
#define TR_MENUDEBUG           "DEBUG"
#define TR_RXNUM               "RxNum"
#define TR_SYNCMENU            "Sinc.[Men\200]"
#define TR_BACK                "Prec"
#define TR_LIMIT               "Limit"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Latitud."
#define TR_LONGITUDE           "Longitud."
#define TR_GPSCOORD            "Coord. GPS"
#define TR_VARIO               "Vario"
#define TR_SHUTDOWN            "ARRESTO.."
#define TR_BATT_CALIB          "Calibra batt."
#define TR_CURRENT_CALIB       "Calibra corr."
#define TR_VOLTAGE             "Voltage"
#define TR_CURRENT             "Corr."
#define TR_SELECT_MODEL        "Scegli Memo."
#define TR_CREATE_MODEL        "Crea Modello"
#define TR_BACKUP_MODEL        "Salva Modello"
#define TR_DELETE_MODEL        "Canc. Modello" // TODO merged into DELETEMODEL?
#define TR_RESTORE_MODEL       "Ripr. Modello"
#define TR_SDCARD_ERROR        "Errore SD"
#define TR_NO_SDCARD           "No SDCARD"
#define TR_INCOMPATIBLE        "Incompatibile"
#define TR_WARNING             "AVVISO"
#define TR_EEPROMWARN          "EEPROM"
#define TR_THROTTLEWARN        "MOTORE"
#define TR_ALARMSWARN          "ALLARMI"
#define TR_SWITCHWARN          "SWITCH"
#define TR_INVERT_THR          "Invert Thr?"
#define TR_SPEAKER_VOLUME      "Volume Audio"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          "Luminosita'"
#define TR_CPU_TEMP            "Temp CPU \016>"
#define TR_CPU_CURRENT         "Corrente\022>"
#define TR_CPU_MAH             "Consumo"
#define TR_COPROC              "CoProc."  
#define TR_COPROC_TEMP         "Temp. MB \016>"
#define TR_CAPAWARNING         INDENT "Capacita Low"
#define TR_TEMPWARNING         INDENT "Temp High"
#define TR_FUNC                "Funz"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Durata"
#define TR_DELAY               "Ritardo"
#define TR_SD_CARD             "SD Card"
#define TR_SDHC_CARD           "SD-HC Card"
#define TR_NO_SOUNDS_ON_SD     "No Suoni su SD"
#define TR_NO_MODELS_ON_SD     "No Model. su SD"
#define TR_PLAY_FILE           "Suona"
#define TR_DELETE_FILE         "Cancella"
#define TR_COPY_FILE           "Copia"
#define TR_RENAME_FILE         "Rinomomina"
#define TR_REMOVED             " rimosso"
#define TR_SD_INFO             "Informazioni"
#define TR_SD_FORMAT           "Format"
#define TR_NA                  "N/A"
#define TR_HARDWARE            "HARDWARE"
#define TR_FORMATTING          "Formattazione."
#define TR_TEMP_CALIB          "Temp. Calib"
#define TR_TIME                "Ora"
#define TR_BAUDRATE            "BT Baudrate"
#define TR_NO_BITMAPS_ON_SD    "No Bitmaps on SD"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Tipo:"
#define TR_SD_SPEED            "Veloc.:"
#define TR_SD_SECTORS          "Settori:"
#define TR_SD_SIZE             "Dimens:"
#define TR_CURVE_TYPE          "Tipo"
#define TR_GLOBAL_VARS         "Variabili Globali"
#define TR_OWN                 "Own"
#define TR_DATE                "Date"
#define TR_ROTARY_ENCODER      "R.Encs"
#define TR_CHANNELS_MONITOR    "CHANNELS MONITOR"
