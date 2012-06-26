// NON ZERO TERMINATED STRINGS
#define LEN_OFFON        "\003"
#define TR_OFFON         "OFF""ON\0"

#define LEN_MMMINV       "\003"
#define TR_MMMINV        "---""INV"

#define LEN_NCHANNELS    "\004"
#define TR_NCHANNELS     "\0014CH\0016CH\0018CH10CH12CH14CH16CH"

#define LEN_VBEEPMODE    "\005"
#define TR_VBEEPMODE     "Quiet""Alarm""NoKey""All  "

#define LEN_VBEEPLEN     "\005"
#define TR_VBEEPLEN      "0====""=0===""==0==""===0=""====0"

#define LEN_VRENAVIG     "\003"
#define TR_VRENAVIG      "No REaREb"

#define LEN_VFILTERADC   "\004"
#define TR_VFILTERADC    "SING""OSMP""FILT"

#define LEN_TRNMODE      "\003"
#define TR_TRNMODE       "off"" +="" :="

#define LEN_TRNCHN       "\003"
#define TR_TRNCHN        "ch1ch2ch3ch4"

#define LEN_DATETIME     "\005"
#define TR_DATETIME      "DATE:""TIME:"

#define LEN_VTRIMINC     "\006"
#define TR_VTRIMINC      "Exp   ""ExFine""Fine  ""Medium""Coarse"

#define LEN_RETA123      "\001"
#if defined(PCBV4)
#if defined(EXTRA_ROTARY_ENCODERS)
#define TR_RETA123       "RETA123abcd"
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

#define LEN_CURVMODES    "\005"
#define TR_CURVMODES     "EDIT ""PRSET""A.THR"

#define LEN_EXPLABELS    "\006"
#ifdef FLIGHT_PHASES
#define TR_EXPLABEL_FP   "Phase "
#else
#define TR_EXPLABEL_FP
#endif
#define TR_EXPLABELS     "Weight""Expo  ""Curve " TR_EXPLABEL_FP "Swtch ""Side\0 ""\0"

#define LEN_VMLTPX       "\010"
#define TR_VMLTPX        "Add     ""Multiply""Replace "

#define LEN_VMLTPX2      "\002"
#define TR_VMLTPX2       "+=""*="":="

#define LEN_VMIXTRIMS    "\003"
#define TR_VMIXTRIMS     "OFF""ON ""Rud""Ele""Thr""Ail"

#define LEN_VCSWFUNC     "\007"
#define TR_VCSWFUNC      "----\0  ""v>ofs  ""v<ofs  ""|v|>ofs""|v|<ofs""AND    ""OR     ""XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 "

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
#define TR_PLAY_TRACK    "Play Track\0  "
#define TR_PLAY_VALUE    "Play Value\0  "
#else
#define TR_PLAY_TRACK    "[Play Track]\0"
#define TR_PLAY_VALUE    "[Play Value]\0"
#endif
#else
#define TR_SDCLOGS
#define TR_PLAY_TRACK
#define TR_PLAY_VALUE

#endif
#ifdef DEBUG
#define TR_TEST          "Test\0"
#else
#define TR_TEST
#endif
#define TR_VFSWFUNC      "Safety\0      ""Trainer \0    ""Instant Trim " TR_SOUND TR_HAPTIC "Reset\0       " TR_VVARIO TR_PLAY_TRACK TR_PLAY_VALUE TR_SDCLOGS TR_TEST

#define LEN_VFSWRESET    "\006"
#define TR_VFSWRESET     "Timer1""Timer2""All   ""Telem."

#define LEN_FUNCSOUNDS   "\006"
#define TR_FUNCSOUNDS    "Warn1 ""Warn2 ""Cheep ""Ring  ""SciFi ""Robot ""Chirp ""Tada  ""Crickt""Siren ""AlmClk""Ratata""Tick  "

#define LEN_VTELEMCHNS   "\004"
#define TR_VTELEMCHNS    "---\0""Tmr1""Tmr2""A1\0 ""A2\0 ""Tx\0 ""Rx\0 ""Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""A\0  ""mAh\0""AccX""AccY""AccZ""Hdg\0""VSpd""A1-\0""A2-\0""Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""A+\0 ""Dst+""Acc\0""Time"

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

#define LEN_VARIOSRC     "\006"
#define TR_VARIOSRC      "BaroV1""BaroV2""A1\0   ""A2\0"

#define LEN_GPSFORMAT    "\004"
#define TR_GPSFORMAT     "HMS NMEA"

#define LEN2_VTEMPLATES  13
#define LEN_VTEMPLATES   "\015"
#define TR_VTEMPLATES    "Clear Mixes\0\0""Simple 4-CH \0""T-Cut       \0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE   "\004"
#define TR_VSWASHTYPE    "--- ""120 ""120X""140 ""90\0"

#define LEN_VKEYS        "\005"
#define TR_VKEYS         " Menu"" Exit"" Down""   Up""Right"" Left"

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
#define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb ""REc ""REd "
#else
#define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb "
#endif
#else
#define TR_ROTARY_ENCODERS_VSRCRAW
#endif
#if defined(HELI)
#define TR_CYC_VSRCRAW   "CYC1""CYC2""CYC3"
#else
#define TR_CYC_VSRCRAW   "[C1]""[C2]""[C3]"
#endif
#define TR_VSRCRAW       "Rud ""Ele ""Thr ""Ail ""P1  ""P2  ""P3  " TR_ROTARY_ENCODERS_VSRCRAW "TrmR" "TrmE" "TrmT" "TrmA" "MAX ""3POS" TR_CYC_VSRCRAW

#define LEN_VTMRMODES    "\003"
#define TR_VTMRMODES     "OFF""ABS""THs""TH%""THt"

#define LEN_DSM2MODE     "\007"
#define TR_DSM2MODE      "LP4/LP5DSMonlyDSMX   "

// ZERO TERMINATED STRINGS
#define TR_POPUPS       "[MENU]\004[EXIT]"
#define OFS_EXIT        7
#define TR_MENUWHENDONE "[MENU] WHEN DONE"
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
#define TR_CHECKTRIMS   "Check\005Trims"
#define TR_SWASHTYPE    "Swash Type"
#define TR_COLLECTIVE   "Collective"
#define TR_SWASHRING    "Swash Ring"
#define TR_ELEDIRECTION "ELE Direction"
#define TR_AILDIRECTION "AIL Direction"
#define TR_COLDIRECTION "COL Direction"
#define TR_MODE         "Mode"
#define TR_NOFREEEXPO   "No free expo!"
#define TR_NOFREEMIXER  "No free mixer!"
#define TR_INSERTMIX    "INSERT MIX "
#define TR_EDITMIX      "EDIT MIX "
#define TR_SOURCE       "Source"
#define TR_WEIGHT       "Weight"
#define TR_MIXERWEIGHT  "Mixer Weight"
#define TR_DIFFERENTIAL "Differ"
#define TR_OFFSET       "Offset"
#define TR_MIXEROFFSET  "Mixer Offset"
#define TR_DRWEIGHT     "DR Weight"
#define TR_DREXPO       "DR Expo"
#define TR_TRIM         "Trim"
#define TR_CURVES       "Curves"
#define TR_FPHASE       "F.Phase"
#define TR_MIXWARNING   "Warning"
#define TR_OFF          "OFF"
#define TR_MULTPX       "Multpx"
#define TR_DELAYDOWN    "Delay Dn"
#define TR_DELAYUP      "Delay Up"
#define TR_SLOWDOWN     "Slow  Dn"
#define TR_SLOWUP       "Slow  Up"
#define TR_MIXER        "MIXER"
#define TR_CV           "CV"
#define TR_ACHANNEL     "A\002channel"
#define TR_RANGE        "Range"
#define TR_BAR          "Bar"
#define TR_ALARM        "Alarm"
#define TR_USRDATA      "UsrData"
#define TR_BLADES       "Blades"
#define TR_BARS         "Bars"
#define TR_DISPLAY      "Display"
#ifdef AUDIO
#define TR_BEEPERMODE   "Speaker Mode"
#define TR_BEEPERLEN    "Speaker Length"
#define TR_SPKRPITCH    "Speaker Pitch"
#else
#define TR_BEEPERMODE   "Beeper Mode"
#define TR_BEEPERLEN    "Beeper Length"
#endif
#define TR_HAPTICMODE   "Haptic Mode"
#define TR_HAPTICSTRENGTH "Haptic Strength"
#define TR_HAPTICLENGTH "Haptic Length"
#define TR_CONTRAST     "Contrast"
#define TR_BATTERYWARNING "Battery Warning"
#define TR_INACTIVITYALARM "Inactivity alrm"
#define TR_RENAVIG      "Navig RotEnc"
#define TR_FILTERADC    "Filter ADC"
#define TR_THROTTLEREVERSE "Thr reverse"
#define TR_MINUTEBEEP   "Minute beep"
#define TR_BEEPCOUNTDOWN "Beep countdown"
#define TR_FLASHONBEEP  "Flash on beep"
#define TR_LIGHTSWITCH  "Light switch"
#define TR_LIGHTOFFAFTER "Light off after"
#define TR_SPLASHSCREEN  "Splash screen"
#define TR_THROTTLEWARNING "T-Warning"
#define TR_SWITCHWARNING "S-Warning"
#define TR_MEMORYWARNING "Memory Warning"
#define TR_ALARMWARNING "Alarm Warning"
#define TR_TIMEZONE     "Time Zone"
#define TR_RXCHANNELORD "Rx Channel Ord"
#define TR_SLAVE        "Slave"
#define TR_MODESRC      "mode\003% src"
#define TR_MULTIPLIER   "Multiplier"
#define TR_CAL          "Cal"
#define TR_EEPROMV      "EEPROM v"
#define TR_VTRIM        "Trim- +"
#define TR_BG           "BG:"
#define TR_MENUTOSTART  "[MENU] TO START"
#define TR_SETMIDPOINT  "SET MIDPOINT"
#define TR_MOVESTICKSPOTS "MOVE STICKS/POTS"
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
#define TR_ALERT        "\007ALERT"
#define TR_PRESSANYKEYTOSKIP "Press any key to skip"
#define TR_THROTTLENOTIDLE "Throttle not idle"
#define TR_ALARMSDISABLED "Alarms Disabled"
#define TR_PRESSANYKEY  "\004Press any Key"
#define TR_BADEEPROMDATA "Bad EEprom Data"
#define TR_EEPROMFORMATTING "Formatting EEPROM"
#define TR_EEPROMOVERFLOW "EEPROM overflow"
#define TR_MENURADIOSETUP "RADIO SETUP"
#define TR_MENUDATEANDTIME "DATE AND TIME"
#define TR_MENUTRAINER  "TRAINER"
#define TR_MENUVERSION  "VERSION"
#define TR_MENUDIAG     "DIAG"
#define TR_MENUANA      "ANAS"
#define TR_MENUCALIBRATION "CALIBRATION"
#define TR_TRIMS2OFFSETS "Trims => Offsets"
#define TR_MENUMODELSEL "MODELSEL"
#define TR_MENUSETUP    "SETUP"
#define TR_MENUFLIGHTPHASE "FLIGHT PHASE"
#define TR_MENUFLIGHTPHASES "FLIGHT PHASES"
#define TR_MENUHELISETUP "HELI SETUP"
#define TR_MENUDREXPO   "DR/EXPO" // TODO flash saving this string is 2 times here
#define TR_MENULIMITS   "LIMITS"
#define TR_MENUCURVES   "CURVES"
#define TR_MENUCURVE    "CURVE"
#define TR_MENUCUSTOMSWITCHES "CUSTOM SWITCHES"
#define TR_MENUFUNCSWITCHES "FUNC SWITCHES"
#define TR_MENUTELEMETRY "TELEMETRY"
#define TR_MENUTEMPLATES "TEMPLATES"
#define TR_MENUSTAT      "STATS"
#define TR_MENUDEBUG     "DEBUG"
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
#define TR_BATT_CALIB    "Battery Calib"
#define TR_CURRENT_CALIB "Current Calib"
#define TR_CURRENT       "Current"
#define TR_SELECT_MODEL  "Select Model"
#define TR_CREATE_MODEL  "Create Model"
#define TR_BACKUP_MODEL  "Backup Model"
#define TR_DELETE_MODEL  "Delete Model" // TODO merged into DELETEMODEL?
#define TR_RESTORE_MODEL "Restore Model"
#define TR_SDCARD_ERROR  "SDCARD Error"
#define TR_NO_SDCARD     "No SDCARD"
#define TR_WARNING       "WARNING"
#define TR_EEPROMWARN    "EEPROM"
#define TR_THROTTLEWARN  "THROTTLE"
#define TR_ALARMSWARN    "ALARMS"
#define TR_SWITCHWARN    "SWITCH"
#define TR_INVERT_THR    "Invert Thr?"
