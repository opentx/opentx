// NON ZERO TERMINATED STRINGS
#define LEN_OFFON        3
#define TR_OFFON         "OFF""ON "

#define LEN_MMMINV       3
#define TR_MMMINV        "---""INV"

#define LEN_NCHANNELS    4
#define TR_NCHANNELS     "4CH 6CH 8CH 10CH12CH14CH16CH"

#define LEN_VBEEPER       6
#define TR_VBEEPER        "Quiet ""NoKey ""xShort""Short ""Norm  ""Long  ""xLong "

#define LEN_ADCFILTER    4
#define TR_ADCFILTER     "SING""OSMP""FILT"

#define LEN_WARNSW       4
#define TR_WARNSW        "Down""OFF ""Up  "

#define LEN_TRNMODE      3
#define TR_TRNMODE       "off"" +="" :="

#define LEN_TRNCHN       3
#define TR_TRNCHN        "ch1ch2ch3ch4"

#define LEN_DATETIME     5
#define TR_DATETIME      "DATE:""TIME:"

#define LEN_VTRIMINC     6
#define TR_VTRIMINC      "Exp   ""ExFine""Fine  ""Medium""Coarse"

#define LEN_RETA123      1
#define TR_RETA123       "RETA123"

#define LEN_VPROTOS      5
#define TR_VPROTOS       "PPM  ""PXX  ""DSM2 ""PPM16"

#define LEN_POSNEG       3
#define TR_POSNEG        "POS""NEG"

#define LEN_VCURVEFUNC   3
#define TR_VCURVEFUNC    "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_CURVMODES    5
#define TR_CURVMODES     "EDIT ""PRSET""A.THR"

#define LEN_EXPLABELS    6
#define TR_EXPLABELS     "Weight""Expo  ""Phase ""Swtch ""When  ""Curve ""      "

#define LEN_VMLTPX       8
#define TR_VMLTPX        "Add     ""Multiply""Replace "

#define LEN_VMLTPX2      2
#define TR_VMLTPX2       "+=""*="":="

#define LEN_VCSWFUNC     7
#define TR_VCSWFUNC      "----   ""v>ofs  ""v<ofs  ""|v|>ofs""|v|<ofs""AND    ""OR     ""XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 "

#define LEN_VFSWFUNC     14
#ifdef LOGS
#define TR_SDCLOGGS      "SDCARD Logs   "
#else
#define TR_SDCLOGGS
#endif
#define TR_VFSWFUNC      "----          ""Trainer       ""Trainer RUD   ""Trainer ELE   ""Trainer THR   ""Trainer AIL   ""Instant Trim  ""Trims2Offsets " TR_SDCLOGGS

#define LEN_VTELEMUNIT   1
#define TR_VTELEMUNIT    "v""-"

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

#define LEN_VTEMPLATES   12
#define TR_VTEMPLATES    "Simple 4-CH ""T-Cut       ""V-Tail      ""Elevon\\Delta""eCCPM       ""Heli Setup  ""Servo Test  "

#define LEN_VSWASHTYPE   6
#define TR_VSWASHTYPE    "---   ""120   ""120X  ""140   ""90    "

#define LEN_VKEYS        5
#define TR_VKEYS         " Menu"" Exit"" Down""   Up""Right"" Left"

#define LEN_RE1RE2       3
#define TR_RE1RE2        "RE1""RE2"

#define LEN_VSWITCHES    3
#define TR_VSWITCHES     "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC"

#define LEN_VSRCRAW      4
#define TR_VSRCRAW       "RUD ""ELE ""THR ""AIL ""P1  ""P2  ""P3  ""MAX ""FULL""CYC1""CYC2""CYC3"

#define LEN_VTMRMODES    3
#define TR_VTMRMODES     "OFF""ABS""THs""TH%""THt"

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
#define TR_MENUWHENDONE " [MENU] WHEN DONE"
#define TR_FREE         "free"
#define TR_DELETEMODEL  "DELETE MODEL"
#define TR_COPYINGMODEL "Copying model..."
#define TR_MOVINGMODEL  "Moving model..."
#define TR_LOADINGMODEL "Loading model..."
#define TR_NAME         "Name"
#define TR_TIMER        "Timer"
#define TR_ELIMITS      "E.Limits"
#define TR_ETRIMS       "E.Trims"
#define TR_TRIMINC      "Trim Inc"
#define TR_TTRACE       "T-Trace"
#define TR_TTRIM        "T-Trim"
#define TR_BEEPCTR      "Beep Ctr"
#define TR_PROTO        "Proto"
#define TR_PPMFRAME     "PPM frame"
#define TR_MS           "ms"
#define TR_SWITCH       "Switch"
#define TR_TRIMS        "Trims"
#define TR_FADEIN       "Fade In"
#define TR_FADEOUT      "Fade Out"
#define TR_DEFAULT      "(default)"
#define TR_CHECKTRIMS   "Check     Trims"
#define TR_SWASHTYPE    "Swash Type"
#define TR_COLLECTIVE   "Collective"
#define TR_SWASHRING    "Swash Ring"
#define TR_ELEDIRECTION "ELE Direction"
#define TR_AILDIRECTION "AIL Direction"
#define TR_COLDIRECTION "COL Direction"
#define TR_MODE         "MODE"
#define TR_NOFREEEXPO   "No free expo!"
#define TR_NOFREEMIXER  "No free mixer!"
#define TR_INSERTMIX    "INSERT MIX "
#define TR_EDITMIX      "EDIT MIX "
#define TR_SOURCE       "Source"
#define TR_WEIGHT       "Weight"
#define TR_OFFSET       "Offset"
#define TR_TRIM         "Trim"
#define TR_CURVES       "Curves"
#define TR_FPHASE       "F.Phase"
#define TR_WARNING      "Warning"
#define TR_OFF          "OFF"
#define TR_MULTPX       "Multpx"
#define TR_DELAYDOWN    "Delay Down"
#define TR_DELAYUP      "Delay Up"
#define TR_SLOWDOWN     "Slow  Down"
#define TR_SLOWUP       "Slow  Up"
#define TR_DREXPO       "DR/EXPO"
#define TR_MIXER        "MIXER"
#define TR_COPYTRIMMENU "COPY TRIM [MENU]"
#define TR_CV           "CV"
#define TR_SW           "SW"
#define TR_ACHANNEL     "A  channel"
#define TR_MAX          "Max"
#define TR_CALIB        "Calib"
#define TR_BAR          "Bar"
#define TR_ALARM        "Alarm"
#define TR_USRPROTO     "UsrProto"
#define TR_CLEARMIXMENU "CLEAR MIXES [MENU]"
#define TR_BEEPER       "Beeper"
#define TR_CONTRAST     "Contrast"
#define TR_BATTERYWARNING "Battery warning"
#define TR_INACTIVITYALARM "Inactivity alarm"
#define TR_FILTERADC    "Filter ADC"
#define TR_THROTTLEREVERSE "Throttle reverse"
#define TR_MINUTEBEEP   "Minute beep"
#define TR_BEEPCOUNTDOWN "Beep countdown"
#define TR_FLASHONBEEP  "Flash on beep"
#define TR_LIGHTSWITCH  "Light switch"
#define TR_LIGHTOFFAFTER "Light off after"
#define TR_SPLASHSCREEN  "Splash screen"
#define TR_THROTTLEWARNING "Throttle Warning"
#define TR_SWITCHWARNING "Switch Warning"
#define TR_MEMORYWARNING "Memory Warning"
#define TR_ALARMWARNING "Alarm Warning"
#define TR_NODATAALARM  "NO DATA Alarm"
#define TR_RXCHANNELORD "Rx Channel Ord"
#define TR_MODE2        "Mode"
#define TR_SLAVE        "Slave"
#define TR_MODESRC      "mode   % src"
#define TR_MULTIPLIER   "Multiplier"
#define TR_CAL          "Cal"
#define TR_EEPROMV      "EEPROM v"
#define TR_VTRIM        "Trim- +"
#define TR_BG           "BG"
#define TR_MENUTOSTART  " [MENU] TO START  "
#define TR_SETMIDPOINT  "   SET MIDPOINT   "
#define TR_MOVESTICKSPOTS " MOVE STICKS/POTS "
#define TR_ALT          "Alt:"
#define TR_RXBATT       "Rx Batt:"
#define TR_RX           "Rx:"
#define TR_TX           "Tx:"
#define TR_HDG          "Hdg:"
#define TR_SPD          "Spd:"
#define TR_TEMP1        "Temp1:"
#define TR_TEMP2        "Temp2:"
#define TR_RPM          "RPM:"
#define TR_FUEL         "Fuel:"
#define TR_VOLTS        "Volts:"
#define TR_ACCEL        "Accel"
#define TR_TELE         "Tele:"
#define TR_RSSI         "RSSI:"
#define TR_NODATA       "NO DATA"
#define TR_TM1          "TM1"
#define TR_TM2          "TM2"
#define TR_THR          "THR"
#define TR_TH           "TH%"
#define TR_TOT          "TOT"
#define TR_TMR1LATMAXUS "tmr1Lat max    us"
#define TR_TMR1LATMINUS "tmr1Lat min    us"
#define TR_TMR1JITTERUS "tmr1 Jitter    us"
#define TR_TMAINMAXMS   "tmain max      ms"
#define TR_T10MSUS      "t10ms          us"
#define TR_FREESTACKMINB "Free Stack min    b"
#define TR_MENUTORESET  "[MENU] to reset"
#define TR_PPM          "PPM"
#define TR_CH           "CH"
#define TR_TMR          "TMR"
#define TR_MODEL        "MODEL"
#define TR_FP           "FP"
#define TR_EEPROMLOWMEM "EEPROM low mem"
#define TR_ALERT        "ALERT"
#define TR_PRESSANYKEYTOSKIP "Press any key to skip"
#define TR_THROTTLENOTIDLE "Throttle not idle"
#define TR_RESETTHROTTLE "Reset throttle"
#define TR_ALARMSDISABLED "Alarms Disabled"
#define TR_SWITCHESNOTOFF "Switches not off"
#define TR_PLEASERESETTHEM "Please reset them"
#define TR_MESSAGE      "MESSAGE"
#define TR_PRESSANYKEY  "press any Key"
#define TR_BADEEPROMDATA "Bad EEprom Data"
#define TR_EEPROMFORMATTING "EEPROM Formatting"
#define TR_EEPROMOVERFLOW "EEPROM overflow"
#define TR_MENURADIOSETUP "RADIO SETUP"
#define TR_MENUDATEANDTIME "DATE AND TIME"
#define TR_MENUTRAINER  "TRAINER"
#define TR_MENUVERSION  "VERSION"
#define TR_MENUDIAG     "DIAG"
#define TR_MENUANA      "ANA"
#define TR_MENUCALIBRATION "CALIBRATION"
#define TR_MENUSERROR   "MENUS OVERFLOW"
#define TR_MENUMODELSEL "MODELSEL"
#define TR_MENUSETUP    "SETUP"
#define TR_MENUFLIGHTPHASE "FLIGHT PHASE"
#define TR_MENUFLIGHTPHASES "FLIGHT PHASES"
#define TR_MENUHELISETUP "HELI SETUP"
#define TR_MENUDREXPO   "DR/EXPO"
#define TR_MENULIMITS   "LIMITS"
#define TR_MENUCURVES   "CURVES"
#define TR_MENUCURVE    "CURVE"
#define TR_MENUCUSTOMSWITCHES "CUSTOM SWITCHES"
#define TR_MENUFUNCSWITCHES "FUNC SWITCHES"
#define TR_MENUSAFETYSWITCHES "SAFETY SWITCHES"
#define TR_MENUTELEMETRY "TELEMETRY"
#define TR_MENUTEMPLATES "TEMPLATES"
#define TR_MENUSTAT      "STAT"
#define TR_MENUDEBUG     "DEBUG"
#define TR_RXNUM         "RxNum"
#define TR_SYNCMENU      "Sync [MENU]"
