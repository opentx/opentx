// NON ZERO TERMINATED STRINGS
#define LEN_OFFON        "\003"
#define TR_OFFON         "OFF""ON "

#define LEN_MMMINV       "\003"
#define TR_MMMINV        "---""INV"

#define LEN_NCHANNELS    "\004"
#define TR_NCHANNELS     "4CH 6CH 8CH 10CH12CH14CH16CH"

#define LEN_VBEEPMODE    "\006"
#define TR_VBEEPMODE     "Quiet ""Alarms""NoKey ""Tout  "

#define LEN_VBEEPLEN     "\006"
#define TR_VBEEPLEN      "xShort""Short ""Normal""Long  ""xLong "

#define LEN_ADCFILTER    "\004"
#define TR_ADCFILTER     "SING""OSMP""FILT"

#define LEN_WARNSW       "\004"
#define TR_WARNSW        "Bas ""OFF ""Haut"

#define LEN_TRNMODE      "\003"
#define TR_TRNMODE       "off"" +="" :="

#define LEN_TRNCHN       "\003"
#define TR_TRNCHN        "ch1ch2ch3ch4"

#define LEN_DATETIME     "\005"
#define TR_DATETIME      "DATE:""HEURE"

#define LEN_VTRIMINC     "\006"
#define TR_VTRIMINC      "Exp   ""ExFin ""Fin   ""Moyen ""Gros  "

#define LEN_RETA123      "\001"
#define TR_RETA123       "RETA123"

#define LEN_VPROTOS      "\005"
#define TR_VPROTOS       "PPM\0 ""PXX\0 ""DSM2\0""PPM16""FUT\0 "

#define LEN_POSNEG       "\003"
#define TR_POSNEG        "POS""NEG"

#define LEN_VCURVEFUNC   "\003"
#define TR_VCURVEFUNC    "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_CURVMODES    "\005"
#define TR_CURVMODES     "EDIT ""PRSET""A.GAZ"

#define LEN_EXPLABELS    "\006"
#define TR_EXPLABELS     "Ratio ""Expo  ""Courbe""Phase ""Inter ""Mode  ""      "

#define LEN_VMLTPX       "\010"
#define TR_VMLTPX        "Ajouter ""Multiply""Remplace"

#define LEN_VMLTPX2      "\002"
#define TR_VMLTPX2       "+=""*="":="

#define LEN_VMIXTRIMS    "\006"
#define TR_VMIXTRIMS     "ON    ""OFF   ""Offset"

#define LEN_VCSWFUNC     "\007"
#define TR_VCSWFUNC      "----\0  ""v>ofs  ""v<ofs  ""|v|>ofs""|v|<ofs""AND    ""OR     ""XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 "

#define LEN_VFSWFUNC     "\015"
#if defined(FRSKY_HUB) or defined(WS_HOW_HIGH)
#define TR_VARIO         "Vario        "
#else
#define TR_VARIO
#endif
#ifdef LOGS
#define TR_SDCLOGGS      "SDCARD Logs  "
#else
#define TR_SDCLOGGS
#endif
#ifdef SOMO
#define TR_SOMO          "Jouer fichier"
#else
#define TR_SOMO
#endif
#ifdef AUDIO
#define TR_SOUND         "Jouer son\0   "
#else
#define TR_SOUND         "Beep\0        "
#endif
#ifdef DEBUG
#define TR_TEST          "Test\0        "
#else
#define TR_TEST
#endif
#define TR_VFSWFUNC      "Securite \0   ""Ecolage \0    ""Trim instant " TR_SOUND TR_SOMO "Reset\0       " TR_VARIO TR_SDCLOGGS TR_TEST

#define LEN_VFSWRESET    "\006"
#define TR_VFSWRESET     "Timer1""Timer2""All   ""Telem."

#define LEN_FUNCSOUNDS   "\006"
#define TR_FUNCSOUNDS    "Warn1 ""Warn2 ""Cheep ""Ring  ""SciFi ""Robot ""Chirp ""Tada  ""Crickt""Siren ""AlmClk""Ratata""Tick  ""Haptc1""Haptc2""Haptc3"

#define LEN_VTELEMCHNS   "\004"
#define TR_VTELEMCHNS    "---\0""A1\0 ""A2\0 ""Alt\0""Rpm\0""Fuel""t1\0 ""t2\0 ""Spd\0""Dist""Cell""Tx\0 ""Rx\0 ""AccX""AccY""AccZ""Hdg\0""a1\0 ""a2\0 ""cell""CELL""ALT\0""RPM\0""T1\0 ""T2\0 ""SPD\0""DIST""Tmr1""Tmr2""ACC\0""Time"

#define LEN_VTELEMUNIT   "\003"
#define TR_VTELEMUNIT    "v\0 ""A\0 ""-\0 ""kts""kmh""M/h""m\0 ""@\0 ""%\0"
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
#endif

#define LEN2_VTEMPLATES  15
#define LEN_VTEMPLATES   "\017"
#define TR_VTEMPLATES    "Suppr Mixages\0\0""4 Voies simple\0""Coupure Gaz   \0""Empennage V   \0""Elevon\\Delta  \0""eCCPM         \0""Conf. Helico  \0""Servo Test    \0"

#define LEN_VSWASHTYPE   "\004"
#define TR_VSWASHTYPE    "--- ""120 ""120X""140 ""90  "

#define LEN_VKEYS        "\005"
#define TR_VKEYS         " Menu"" Exit""  Bas"" Haut""Droit""Gauch"

#define LEN_RE1RE2       "\003"
#define TR_RE1RE2        "RE1""RE2"

#define LEN_VSWITCHES    "\003"
#define TR_VSWITCHES     "GAZ""DIR""PRF""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC"

#define LEN_VSRCRAW      "\004"
#define TR_VSRCRAW       "Dir ""Prf ""Gaz ""Ail ""P1  ""P2  ""P3  ""MAX ""3POS""CYC1""CYC2""CYC3"

#define LEN_VTMRMODES    "\003"
#define TR_VTMRMODES     "OFF""ABS""GZs""GZ%""GZt"

#define LEN_DSM2MODE     "\007"
#define TR_DSM2MODE      "LP4/LP5DSMonlyDSMX   "

// ZERO TERMINATED STRINGS
#define TR_POPUPS       "[MENU]\004[QUIT]"
#define OFS_EXIT        7
#define TR_MENUWHENDONE "[MENU]Quand PRET"
#define TR_FREE         "libre"
#define TR_DELETEMODEL  "SUPPRIMER MODELE"
#define TR_COPYINGMODEL "Copie..."
#define TR_MOVINGMODEL  "Deplacement..."
#define TR_LOADINGMODEL "Chargement..."
#define TR_NAME         "Nom"
#define TR_TIMER        "Chrono"
#define TR_ELIMITS      "E.Limites"
#define TR_ETRIMS       "E.Trims"
#define TR_TRIMINC      "Trim Inc"
#define TR_TTRACE       "T-Trace"
#define TR_TTRIM        "T-Trim"
#define TR_BEEPCTR      "Bip Ctr"
#define TR_PROTO        "Proto"
#define TR_PPMFRAME     "PPM frame"
#define TR_MS           "ms"
#define TR_SWITCH       "Inter"
#define TR_TRIMS        "Trims"
#define TR_FADEIN       "Fade In"
#define TR_FADEOUT      "Fade Out"
#define TR_DEFAULT      "(defaut)"
#define TR_CHECKTRIMS   "Verif\005Trims"
#define TR_SWASHTYPE    "Type de Plateau"
#define TR_COLLECTIVE   "Collectif"
#define TR_SWASHRING    "Swash Ring"
#define TR_ELEDIRECTION "Direction PRF"
#define TR_AILDIRECTION "Direction AIL"
#define TR_COLDIRECTION "Direction COL"
#define TR_NOFREEEXPO   "Max expos atteint!"
#define TR_NOFREEMIXER  "Max mixages atteint!"
#define TR_INSERTMIX    "INSERER MIXAGE "
#define TR_EDITMIX      "EDITER MIXAGE "
#define TR_SOURCE       "Source"
#define TR_WEIGHT       "Ratio"
#define TR_MIXERWEIGHT  "Ratio Mixage"
#define TR_DIFFERENTIAL "Differ"
#define TR_OFFSET       "Offset"
#define TR_MIXEROFFSET  "Offset Mixage"
#define TR_DRWEIGHT     "Ratio DR"
#define TR_DREXPO       "Expo DR"
#define TR_TRIM         "Trim"
#define TR_CURVES       "Courbe"
#define TR_FPHASE       "P.Vol"
#define TR_WARNING      "Alerte"
#define TR_OFF          "OFF"
#define TR_MULTPX       "Multpx"
#define TR_DELAYDOWN    "Retard Bas"
#define TR_DELAYUP      "Retard Haut"
#define TR_SLOWDOWN     "Ralentir Haut"
#define TR_SLOWUP       "Ralentir Bas"
#define TR_MIXER        "MIXEUR"
#define TR_CV           "CV"
#define TR_SW           "SW"
#define TR_ACHANNEL     "A\002voie"
#define TR_RANGE        "Range"
#define TR_BAR          "Bar"
#define TR_ALARM        "Alarme"
#define TR_USRDATA      "UsrData"
#define TR_BLADES       "Blades"
#define TR_BARS         "Gauges"
#define TR_DISPLAY      "Display"
#ifdef AUDIO
#define TR_BEEPERMODE   "Speaker Mode"
#define TR_BEEPERLEN    "Speaker Length"
#define TR_SPKRPITCH    "Speaker Pitch"
#else
#define TR_BEEPERMODE   "Beeper Mode"
#define TR_BEEPERLEN    "Beeper Length"
#endif
#define TR_HAPTICMODE   "Mode Vibreur"
#define TR_HAPTICSTRENGTH "Force Vibreur"
#define TR_CONTRAST     "Contraste"
#define TR_BATTERYWARNING "Alarme Batterie"
#define TR_INACTIVITYALARM "Alarme Inactivite"
#define TR_FILTERADC    "Filtre ADC"
#define TR_THROTTLEREVERSE "Inversion Gaz"
#define TR_MINUTEBEEP   "Bip t.minutes"
#define TR_BEEPCOUNTDOWN "Bip a rebours"
#define TR_FLASHONBEEP  "Flash quand bip"
#define TR_LIGHTSWITCH  "Inter eclairage"
#define TR_LIGHTOFFAFTER "Eclair. off apres"
#define TR_SPLASHSCREEN  "Ecran d'accueil"
#define TR_THROTTLEWARNING "Alerte Gaz"
#define TR_SWITCHWARNING "Alerte Inter"
#define TR_MEMORYWARNING "Alerte Memoire"
#define TR_ALARMWARNING "Alerte Alarme"
#define TR_NODATAALARM  "Alerte Telem."
#define TR_TIMEZONE     "Fuseau Horaire"
#define TR_RXCHANNELORD "Ordre Voies Rx"
#define TR_MODE         "Mode"
#define TR_SLAVE        "Esclave"
#define TR_MODESRC      "mode\003% src"
#define TR_MULTIPLIER   "Multiplieur"
#define TR_CAL          "Cal"
#define TR_EEPROMV      "EEPROM v"
#define TR_VTRIM        "Trim- +"
#define TR_BG           "BG"
#define TR_MENUTOSTART  "[MENU]POUR DEBUT"
#define TR_SETMIDPOINT  "SET NEUTRE"
#define TR_MOVESTICKSPOTS "BOUGER MANCHES/POTS "
#define TR_RXBATT       "Rx Batt:"
#define TR_TXnRX        "Tx:\0Rx:"
#define OFS_RX          4
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
#define TR_MENUTORESET  "[MENU]pour reset"
#define TR_PPM          "PPM"
#define TR_CH           "CH"
#define TR_TMR          "TMR"
#define TR_MODEL        "MODELE"
#define TR_FP           "PV"
#define TR_EEPROMLOWMEM "EEPROM low mem"
#define TR_ALERT        "ALERTE"
#define TR_PRESSANYKEYTOSKIP "Appuyez sur touche"
#define TR_THROTTLENOTIDLE "Gaz pas a zero"
#define TR_RESETTHROTTLE    "Reinitialisez le"
#define TR_ALARMSDISABLED   "Alarmes Desactivees"
#define TR_SWITCHESNOTOFF   "Switches enclenches"
#define TR_PLEASERESETTHEM  "Reinitialisez les"
#define TR_MESSAGE          "MESSAGE"
#define TR_PRESSANYKEY      "Touche pour continuer"
#define TR_BADEEPROMDATA    "Bad EEprom Data"
#define TR_EEPROMFORMATTING "Formattage EEPROM"
#define TR_EEPROMOVERFLOW   "Depassement EEPROM"
#define TR_MENURADIOSETUP "CONFIG RADIO"
#define TR_MENUDATEANDTIME "DATE ET HEURE"
#define TR_MENUTRAINER  "ECOLAGE"
#define TR_MENUVERSION  "VERSION"
#define TR_MENUDIAG     "DIAG"
#define TR_MENUANA      "ANA"
#define TR_MENUCALIBRATION "CALIBRATION"
#define TR_MENUSERROR   "MENUS OVERFLOW"
#define TR_MENUMODELSEL "SELMODE"
#define TR_MENUSETUP    "SETUP"
#define TR_MENUFLIGHTPHASE "PHASE DE VOL"
#define TR_MENUFLIGHTPHASES "PHASES DE VOL"
#define TR_MENUHELISETUP "CONF.HELI"
#define TR_MENUDREXPO   "DR/EXPO"
#define TR_MENULIMITS   "LIMITES"
#define TR_MENUCURVES   "COURBES"
#define TR_MENUCURVE    "COURBE"
#define TR_MENUCUSTOMSWITCHES "INTERS PERS"
#define TR_MENUFUNCSWITCHES   "INTERS FONCTIONS"
#define TR_MENUTELEMETRY "TELEMETRIE"
#define TR_MENUTEMPLATES "GABARITS"
#define TR_MENUSTAT      "STATS"
#define TR_MENUDEBUG     "DEBUG"
#define TR_RXNUM         "RxNum"
#define TR_SYNCMENU      "Sync [MENU]"
#define TR_BACK          "Back"
#define TR_MINLIMIT      "Min Limit"
#define TR_MAXLIMIT      "Max Limit"
#define TR_MINRSSI       "Min Rssi"
#define TR_LATITUDE      "Latitude"
#define TR_LONGITUDE     "Longitude"
