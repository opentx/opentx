// NON ZERO TERMINATED STRINGS
#define LEN_OFFON        "\003"
#define TR_OFFON         "OFF""ON "

#define LEN_MMMINV       "\003"
#define TR_MMMINV        "---""INV"

#define LEN_NCHANNELS    "\004"
#define TR_NCHANNELS     "4CH 6CH 8CH 10CH12CH14CH16CH"

#define LEN_VBEEPMODE    "\005"
#define TR_VBEEPMODE     "Silen""Avvis""Notst""Tutti"

#define LEN_VBEEPLEN     "\005"
#define TR_VBEEPLEN      "xBrev""Breve""Norml""Ampio""xAmpi"

#define LEN_VRENAVIG     "\003"
#define TR_VRENAVIG      "No REaREb"

#define LEN_VFILTERADC   "\004"
#define TR_VFILTERADC    "SING""OSMP""FILT"

#define LEN_WARNSW       "\004"
#define TR_WARNSW        "Giù ""OFF ""Sù  "

#define LEN_TRNMODE      "\003"
#define TR_TRNMODE       "off"" +="" :="

#define LEN_TRNCHN       "\003"
#define TR_TRNCHN        "ch1ch2ch3ch4"

#define LEN_DATETIME     "\005"
#define TR_DATETIME      "DATA:""ORA :"

#define LEN_VTRIMINC     "\006"
#define TR_VTRIMINC      "Exp   ""ExFine""Fine  ""Medio ""Ampio "

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
#define TR_PXX "PXX\0  "
#else
#define TR_PXX
#endif
#ifdef DSM2
#define TR_DSM2 "DSM2\0 "
#else
#define TR_DSM2
#endif
#define TR_VPROTOS       "PPM\0  ""PPM16\0""PPMsim" TR_PXX TR_DSM2

#define LEN_POSNEG       "\003"
#define TR_POSNEG        "POS""NEG"

#define LEN_VCURVEFUNC   "\003"
#define TR_VCURVEFUNC    "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_CURVMODES    "\005"
#define TR_CURVMODES     "PERS.""PRSET""A.THR"

#define LEN_EXPLABELS    "\006"
#ifdef FLIGHT_PHASES
#define TR_EXPLABEL_FP   "Fase  "
#else
#define TR_EXPLABEL_FP
#endif
#define TR_EXPLABELS     "Peso  ""Expo  ""Curva " TR_EXPLABEL_FP "Switch""Lato ""      " // TODO remove all the trailing spaces

#define LEN_VMLTPX       "\010"
#define TR_VMLTPX        "Aggiungi""Moltipl.""Riscrivi"

#define LEN_VMLTPX2      "\002"
#define TR_VMLTPX2       "+=""*="":="

#define LEN_VMIXTRIMS    "\006"
#define TR_VMIXTRIMS     "ON    ""OFF   ""Offset"

#define LEN_VCSWFUNC     "\007"
#define TR_VCSWFUNC      "----\0  ""v>ofs  ""v<ofs  ""|v|>ofs""|v|<ofs""AND    ""OR     ""XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 "

#define LEN_VFSWFUNC     "\015"
#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
#define TR_VVARIO         "Vario        "
#else
#define TR_VVARIO
#endif
#ifdef SDCARD
#define TR_SDCLOGGS      "SDCARD Logs  "
#else
#define TR_SDCLOGGS
#endif
#ifdef SOMO
#define TR_SOMO          "EseguiBran\0  "
#else
#define TR_SOMO
#endif
#ifdef AUDIO
#define TR_SOUND         "EseguiAudi\0  "
#else
#define TR_SOUND         "Beep\0        "
#endif
#ifdef HAPTIC
#define TR_HAPTIC        "Aptico\0      "
#else
#define TR_HAPTIC
#endif
#ifdef DEBUG
#define TR_TEST          "Test\0        "
#else
#define TR_TEST
#endif
#define TR_VFSWFUNC      "Sicura   \0   ""Trainer \0    ""Insta-Trim   " TR_SOUND TR_HAPTIC TR_SOMO "Reset\0       " TR_VVARIO TR_SDCLOGGS TR_TEST

#define LEN_VFSWRESET    "\006"
#define TR_VFSWRESET     "Timer1""Timer2""Tutto ""Telem."

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

#define LEN_VALARM       "\003"
#define TR_VALARM        "---""Yel""Org""Red"

#define LEN_VALARMFN     "\001"
#define TR_VALARMFN      "<>"

#define LEN_VTELPROTO    "\004"
#if defined(WS_HOW_HIGH)
#define TR_VTELPROTO     "NoneHub WSHH"
#elif defined(FRSKY_HUB)
#define TR_VTELPROTO     "NoneHub "
#else
#define TR_VTELPROTO     ""
#endif

#define LEN_VARIOSRC     "\004"
#define TR_VARIOSRC      "None""Baro""A1  ""A2  "

#define LEN_GPSFORMAT    "\004"
#define TR_GPSFORMAT     "HMS NMEA"

#define LEN2_VTEMPLATES  13
#define LEN_VTEMPLATES   "\015"
#define TR_VTEMPLATES    "Clear Mixes\0\0""Simple 4-CH \0""T-Cut       \0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE   "\004"
#define TR_VSWASHTYPE    "--- ""120 ""120X""140 ""90  "

#define LEN_VKEYS        "\005"
#define TR_VKEYS         " MENU"" EXIT""  DN ""   UP""   SX""   DX"

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
#define TR_VSRCRAW       "Rud ""Ele ""Thr ""Ail ""P1  ""P2  ""P3  " TR_ROTARY_ENCODERS_VSRCRAW "MAX ""3POS""CYC1""CYC2""CYC3"

#define LEN_VTMRMODES    "\003"
#define TR_VTMRMODES     "OFF""ABS""THs""TH%""THt"

#define LEN_DSM2MODE     "\007"
#define TR_DSM2MODE      "LP4/LP5DSMonlyDSMX   "

// ZERO TERMINATED STRINGS
#define TR_POPUPS       "[MENU]\004[EXIT]"
#define OFS_EXIT        7
#define TR_MENUWHENDONE "[MENU] A TERMINE"
#define TR_FREE         " Disp."
#define TR_DELETEMODEL  "Elimina modello?"
#define TR_COPYINGMODEL "Copia in corso..."
#define TR_MOVINGMODEL  "Spostamento..."
#define TR_LOADINGMODEL "Caricamento..."
#define TR_NAME         "Nome"
#define TR_TIMER        "Timer"
#define TR_ELIMITS      "LimitiEx"
#define TR_ETRIMS       "TrimEx "
#define TR_TRIMINC      "Inc Trim"
#define TR_TTRACE       "T-Trace"
#define TR_TTRIM        "T-Trim"
#define TR_BEEPCTR      "Beep Ctr"
#define TR_PROTO        "Proto"
#define TR_PPMFRAME     "PPM frame"
#define TR_MS           "ms"
#define TR_SWITCH       "Switch"
#define TR_TRIMS        "Trim"
#define TR_FADEIN       "Diss.In"
#define TR_FADEOUT      "Diss.Out"
#define TR_DEFAULT      "(default)"
#define TR_CHECKTRIMS   "Contr.\005 Trim"
#define TR_SWASHTYPE    "Tipo Ciclico"
#define TR_COLLECTIVE   "Collettivo"
#define TR_SWASHRING    "AnelloCiclico"
#define TR_ELEDIRECTION "Direzione ELE"
#define TR_AILDIRECTION "Direzione AIL"
#define TR_COLDIRECTION "Direzione COL"
#define TR_MODE         "Mode"
#define TR_NOFREEEXPO   "Expo pieni!"
#define TR_NOFREEMIXER  "Mixer pieni!"
#define TR_INSERTMIX    "INSER. MIX"
#define TR_EDITMIX      "MOD. MIX"
#define TR_SOURCE       "Sorg."
#define TR_WEIGHT       "Peso"
#define TR_MIXERWEIGHT  "Peso Mixer"
#define TR_DIFFERENTIAL "Differ"
#define TR_OFFSET       "Offset"
#define TR_MIXEROFFSET  "Offset Mixer"
#define TR_DRWEIGHT     "Peso DR  "
#define TR_DREXPO       "Expo DR"
#define TR_TRIM         "Trim"
#define TR_CURVES       "Curva"
#define TR_FPHASE       "Fase V."
#define TR_WARNING      "Avviso"
#define TR_OFF          "OFF"
#define TR_MULTPX       "Moltpx"
#define TR_DELAYDOWN    "Post. Giù "
#define TR_DELAYUP      "Post. Sù"
#define TR_SLOWDOWN     "Rall. Giù "
#define TR_SLOWUP       "Rall. Sù"
#define TR_MIXER        "MIXER"
#define TR_CV           "CV"
#define TR_SW           "SW"
#define TR_ACHANNEL     "A\002channel"
#define TR_RANGE        "Range"
#define TR_BAR          "Bar"
#define TR_ALARM        "Alarm"
#define TR_USRDATA      "UsrData"
#define TR_BLADES       "Blades"
#define TR_BARS         "Bars"
#define TR_DISPLAY      "Display"
#ifdef AUDIO
#define TR_BEEPERMODE   "Modo Audio"
#define TR_BEEPERLEN    "Durata Audio"
#define TR_SPKRPITCH    "Tono Audio"
#else
#define TR_BEEPERMODE   "Modo   Beep"
#define TR_BEEPERLEN    "Durata Beep"
#endif
#define TR_HAPTICMODE   "Modo Aptico"
#define TR_HAPTICSTRENGTH "IntensitàAptico"
#define TR_HAPTICLENGTH "Durata Aptico"
#define TR_CONTRAST     "Contrasto"
#define TR_BATTERYWARNING "Avviso Batteria"
#define TR_INACTIVITYALARM "Avviso Inattiv."
#define TR_RENAVIG      "Navig RotEnc"
#define TR_FILTERADC    "Filtro ADC"
#define TR_THROTTLEREVERSE "Thr inverso"
#define TR_MINUTEBEEP   "Beep minuto"
#define TR_BEEPCOUNTDOWN "Beep cont.rov."
#define TR_FLASHONBEEP  "Lamp. al beep"
#define TR_LIGHTSWITCH  "Switch ill. "
#define TR_LIGHTOFFAFTER "Spegni ill.dopo"
#define TR_SPLASHSCREEN  "Schermata avvio"
#define TR_THROTTLEWARNING "Avviso Thr "
#define TR_SWITCHWARNING "Avviso Switch "
#define TR_MEMORYWARNING "Avviso Memoria"
#define TR_ALARMWARNING "Avviso Allarme"
#define TR_NODATAALARM  "NO DATA Alarm"
#define TR_TIMEZONE     "Zona ora "
#define TR_RXCHANNELORD "Ordine ch RX  "
#define TR_SLAVE        "Slave"
#define TR_MODESRC      "mode\003% src"
#define TR_MULTIPLIER   "Moltiplica"
#define TR_CAL          "Cal"
#define TR_EEPROMV      "EEPROM v"
#define TR_VTRIM        "Trim- +"
#define TR_BG           "BG:"
#define TR_MENUTOSTART  "[MENU] PER CAL."
#define TR_SETMIDPOINT  "SETTA CENTRO"
#define TR_MOVESTICKSPOTS "MUOVI STICK/POTS"
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
#define TR_MENUTORESET  "[MENU] PER RESET"
#define TR_PPM          "PPM"
#define TR_CH           "CH"
#define TR_MODEL        "MODELLO"
#define TR_FP           "FV"
#define TR_EEPROMLOWMEM "EEPROM quasi piena!"
#define TR_ALERT        "\007ALERT"
#define TR_PRESSANYKEYTOSKIP "Premi qualsiasi tasto"
#define TR_THROTTLENOTIDLE "Throttle non in pos."
#define TR_RESETTHROTTLE "Riposiziona Throttle"
#define TR_ALARMSDISABLED "Avvisi Disattivati!"
#define TR_SWITCHESNOTOFF "Switch attivi!  "
#define TR_PLEASERESETTHEM "Disattivarli"
#define TR_MESSAGE      "\004MESSAGE"
#define TR_PRESSANYKEY  "\004Premi un tasto"
#define TR_BADEEPROMDATA "Dati corrotti!"
#define TR_EEPROMFORMATTING "Formatto EEPROM..."
#define TR_EEPROMOVERFLOW "EEPROM Piena"
#define TR_MENURADIOSETUP "CONFIGURATX"
#define TR_MENUDATEANDTIME "DATA E ORA"
#define TR_MENUTRAINER  "TRAINER"
#define TR_MENUVERSION  "VERSION"
#define TR_MENUDIAG     "DIAG"
#define TR_MENUANA      "ANAS"
#define TR_MENUCALIBRATION "CALIBRAZIONE"
#define TR_MENUSERROR   "ERRORE MENU   "
#define TR_TRIMS2OFFSETS "Trim  => Offset "
#define TR_MENUMODELSEL "MODELLI"
#define TR_MENUSETUP    "SETUP"
#define TR_MENUFLIGHTPHASE "FASE DI VOLO"
#define TR_MENUFLIGHTPHASES "FASI DI VOLO"
#define TR_MENUHELISETUP "SETUP  ELI"
#define TR_MENUDREXPO   "DR/EXPO" // TODO flash saving this string is 2 times here
#define TR_MENULIMITS   "LIMITI"
#define TR_MENUCURVES   "CURVE"
#define TR_MENUCURVE    "CURVA"
#define TR_MENUCUSTOMSWITCHES "SWITCH PERSON."
#define TR_MENUFUNCSWITCHES "SWITCH FUNZIONE"
#define TR_MENUTELEMETRY "TELEMETRIA"
#define TR_MENUTEMPLATES "TEMPLATES"
#define TR_MENUSTAT      "STATS"
#define TR_MENUDEBUG     "DEBUG"
#define TR_RXNUM         "RxNum"
#define TR_SYNCMENU      "Sinc.[MENU]"
#define TR_BACK          "Prec"
#define TR_MINLIMIT      "LimiteMin"
#define STR_LIMIT        (STR_MINLIMIT+4)
#define TR_MAXLIMIT      "LimiteMax"
#define TR_MINRSSI       "Min Rssi"
#define TR_LATITUDE      "Latitude"
#define TR_LONGITUDE     "Longitude"
#define TR_GPSCOORD      "Gps Coords"
#define TR_VARIO         "Vario"
#define TR_SHUTDOWN      "SPEGNENDO"
#define TR_BATT_CALIB    "Calibra batt."
#define TR_CURRENT_CALIB "Calibra corr."
#define TR_CURRENT       "Corr."
#define TR_SELECT_MODEL  "Scegli Memo."
#define TR_CREATE_MODEL  "Crea Memoria"
#define TR_BACKUP_MODEL  "Salva Memo."
#define TR_DELETE_MODEL  "Canc. Memo." // TODO merged into DELETEMODEL?
#define TR_RESTORE_MODEL "Ripr. Memoria"
#define TR_SDCARD_ERROR  "Errore SD"
#define TR_NO_SDCARD     "No SDCARD"
