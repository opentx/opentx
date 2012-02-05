// NON ZERO TERMINATED STRINGS
#define LEN_OFFON        3
#define TR_OFFON         "OFF""ON "

#define LEN_MMMINV       3
#define TR_MMMINV        "---""INV"

#define LEN_NCHANNELS    4
#define TR_NCHANNELS     "4CH 6CH 8CH 10CH12CH14CH16CH"

#define LEN_VBEEPER      6
#define TR_VBEEPER       "Quiet ""NoKey ""xShort""Court ""Norm  ""Long  ""xLong "

#define LEN_ADCFILTER    4
#define TR_ADCFILTER     "SING""OSMP""FILT"

#define LEN_WARNSW       4
#define TR_WARNSW        "Bas ""OFF ""Haut"

#define LEN_TRNMODE      3
#define TR_TRNMODE       "off"" +="" :="

#define LEN_TRNCHN       3
#define TR_TRNCHN        "ch1ch2ch3ch4"

#define LEN_DATETIME     5
#define TR_DATETIME      "DATE:""HEURE"

#define LEN_VTRIMINC     6
#define TR_VTRIMINC      "Exp   ""ExFin ""Fin   ""Moyen ""Gros  "

#define LEN_RETA123      1
#define TR_RETA123       "RETA123"

#define LEN_VPROTOS      5
#define TR_VPROTOS       "PPM  ""PXX  ""DSM2 ""PPM16"

#define LEN_POSNEG       3
#define TR_POSNEG        "POS""NEG"

#define LEN_VCURVEFUNC   3
#define TR_VCURVEFUNC    "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_CURVMODES    5
#define TR_CURVMODES     "EDIT ""PRSET""A.GAZ"

#define LEN_EXPLABELS    6
#define TR_EXPLABELS     "Ratio ""Expo  ""Phase ""Inter ""Mode  ""Courbe""      "

#define LEN_VMLTPX       8
#define TR_VMLTPX        "Ajouter ""Multiply""Remplace"

#define LEN_VMLTPX2      2
#define TR_VMLTPX2       "+=""*="":="

#define LEN_VMIXTRIMS    6
#define TR_VMIXTRIMS     "ON    ""OFF   ""Offset"

#define LEN_VCSWFUNC     7
#define TR_VCSWFUNC      "----   ""v>ofs  ""v<ofs  ""|v|>ofs""|v|<ofs""ET     ""OU     ""XOU    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 "

#define LEN_VFSWFUNC     13
#ifdef LOGS
#define TR_SDCLOGGS      "SDCARD Logs  "
#else
#define TR_SDCLOGGS
#endif
#define TR_VFSWFUNC      "Securite \0   ""Ecolage \0    ""Trim instant ""Trims2Offsets""Jouer son    " TR_SDCLOGGS

#define LEN_FUNCSOUNDS   6
#define TR_FUNCSOUNDS    "Warn1 ""Warn2 ""Cheep ""Ring  ""SciFi ""Robot ""Chirp ""Tada  ""Crickt""Siren ""AlmClk""Ratata""Tick  ""Haptc1""Haptc2""Haptc3"

#define LEN_VTELEMUNIT   4
#define TR_VTELEMUNIT    "v\0  ""-\0  ""kts\0""km/h""M/h\0""m\0  ""@C\0 "

#define LEN_VALARM       3
#define TR_VALARM        "---""Yel""Org""Red"

#define LEN_VALARMFN     1
#define TR_VALARMFN      "<>"

#define LEN_VTELPROTO    4
#if defined(WS_HOW_HIGH)
#define TR_VTELPROTO     "NoneHub WSHH"
#elif defined(FRSKY_HUB)
#define TR_VTELPROTO     "NoneHub "
#endif

#define LEN_VTEMPLATES   14
#define TR_VTEMPLATES    "4 Voies simple""Coupure Gaz   ""Empennage V   ""Elevon\\Delta  ""eCCPM         ""Conf.Helico   ""Servo Test    "

#define LEN_VSWASHTYPE   6
#define TR_VSWASHTYPE    "---   ""120   ""120X  ""140   ""90    "

#define LEN_VKEYS        5
#define TR_VKEYS         " Menu"" Exit""  Bas"" Haut""Droit""Gauch"

#define LEN_RE1RE2       3
#define TR_RE1RE2        "RE1""RE2"

#define LEN_VSWITCHES    3
#define TR_VSWITCHES     "GAZ""DIR""PRF""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC"

#define LEN_VSRCRAW      4
#define TR_VSRCRAW       "DIR ""PRF ""GAZ ""AIL ""P1  ""P2  ""P3  ""MAX ""FULL""CYC1""CYC2""CYC3"

#define LEN_VTMRMODES    3
#define TR_VTMRMODES     "OFF""ABS""GZs""GZ%""GZt"

#define LEN_TELEMCHNS    3
#if defined(FRSKY_HUB)
#define TR_TELEMCHNS     "AD1AD2ALTRPM"
#elif defined(WS_HOW_HIGH)
#define TR_TELEMCHNS     "AD1AD2ALT"
#elif defined(FRSKY)
#define TR_TELEMCHNS     "AD1AD2"
#endif

#define LEN_DSM2MODE     7
#define TR_DSM2MODE      "LP4/LP5DSMonlyDSMX   "

// ZERO TERMINATED STRINGS
#define TR_POPUPS       "[MENU]    [EXIT]"
#define TR_MENUWHENDONE " [MENU]Quand PRET"
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
#define TR_CHECKTRIMS   "Verif     Trims"
#define TR_SWASHTYPE    "Type de Plateau"
#define TR_COLLECTIVE   "Collectif"
#define TR_SWASHRING    "Swash Ring"
#define TR_ELEDIRECTION "Direction PRF"
#define TR_AILDIRECTION "Direction AIL"
#define TR_COLDIRECTION "Direction COL"
#define TR_MODE         "MODE"
#define TR_NOFREEEXPO   "Max expo atteint!"
#define TR_NOFREEMIXER  "Max mixages atteint!"
#define TR_INSERTMIX    "INSERER MIXAGE "
#define TR_EDITMIX      "EDITER MIXAGE "
#define TR_SOURCE       "Source"
#define TR_WEIGHT       "Ratio"
#define TR_OFFSET       "Offset"
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
#define TR_DREXPO       "DR/EXPO"
#define TR_MIXER        "MIXEUR"
#define TR_COPYTRIMMENU "COPIER TRIM [MENU]"
#define TR_CV           "CV"
#define TR_SW           "SW"
#define TR_ACHANNEL     "A  voie"
#define TR_RANGE        "Range"
#define TR_BAR          "Bar"
#define TR_ALARM        "Alarme"
#define TR_USRPROTO     "UsrProto"
#define TR_CLEARMIXMENU "EFFACER MIXAGES[MENU]"
#define TR_BEEPER       "Bipeur"
#define TR_CONTRAST     "Contraste"
#define TR_BATTERYWARNING "Alarme Batterie"
#define TR_INACTIVITYALARM "Alarme inactivite"
#define TR_FILTERADC    "Filtre ADC"
#define TR_THROTTLEREVERSE "Inversion Gaz"
#define TR_MINUTEBEEP   "Bip t.minutes"
#define TR_BEEPCOUNTDOWN "Bip  a rebours"
#define TR_FLASHONBEEP  "Flash quand bip"
#define TR_LIGHTSWITCH  "Inter eclairage"
#define TR_LIGHTOFFAFTER "Eclair. off apres"
#define TR_SPLASHSCREEN  "Ecran d'accueil"
#define TR_THROTTLEWARNING "Alerte Gaz"
#define TR_SWITCHWARNING "Alerte Inter"
#define TR_MEMORYWARNING "Alerte Memoire"
#define TR_ALARMWARNING "Alerte Alarme"
#define TR_NODATAALARM  "PAS DE DONNEES Alarme"
#define TR_RXCHANNELORD "Ordre Voies Rx"
#define TR_MODE2        "Mode"
#define TR_SLAVE        "Esclave"
#define TR_MODESRC      "mode   % src"
#define TR_MULTIPLIER   "Multiplieur"
#define TR_CAL          "Cal"
#define TR_EEPROMV      "EEPROM v"
#define TR_VTRIM        "Trim- +"
#define TR_BG           "BG"
#define TR_MENUTOSTART  "[MENU]POUR DEBUT"
#define TR_SETMIDPOINT  "   SET NEUTRE   "
#define TR_MOVESTICKSPOTS " BOUGER MANCHES/POTS "
#define TR_ALT          "Alt:"
#define TR_RXBATT       "Rx Batt:"
#define TR_RX           "Rx:"
#define TR_TX           "Tx:"
#define TR_SPD          "Spd:"
#define TR_TEMP1        "T@1:"
#define TR_TEMP2        "T@2:"
#define TR_RPM          "RPM:"
#define TR_FUEL         "Fuel:"
#define TR_VOLTS        "Vlts:"
#define TR_ACCEL        "Acc:"
#define TR_TELE         "Tele:"
#define TR_RSSI         "RSSI:"
#define TR_NODATA       "NO DATA"
#define TR_TM1          "TM1"
#define TR_TM2          "TM2"
#define TR_THR          "GAZ"
#define TR_TH           "TH%"
#define TR_TOT          "TOT"
#define TR_TMR1LATMAXUS "tmr1Lat max    us"
#define TR_TMR1LATMINUS "tmr1Lat min    us"
#define TR_TMR1JITTERUS "tmr1 Jitter    us"
#define TR_TMAINMAXMS   "tmain max      ms"
#define TR_T10MSUS      "t10ms          us"
#define TR_FREESTACKMINB "Free Stack min    b"
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
#define TR_MENUSAFETYSWITCHES "INTERS SECURITE"
#define TR_MENUTELEMETRY "TELEMETRIE"
#define TR_MENUTEMPLATES "GABARITS"
#define TR_MENUSTAT      "STAT"
#define TR_MENUDEBUG     "DEBUG"
#define TR_RXNUM         "RxNum"
#define TR_SYNCMENU      "Sync [MENU]"
#define TR_HAPTICSTRENGTH "Haptic Strength"
#define TR_SPKRPITCH     "Speaker Pitch"
