/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

// NON ZERO TERMINATED STRINGS
#define LEN_OFFON              "\003"
#define TR_OFFON               "OFF""ON\0"

#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""INV"

#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014CH\0016CH\0018CH10CH12CH14CH16CH"

#define LEN_VBEEPMODE          "\005"
#define TR_VBEEPMODE           "Quiet""Alarm""NoKey""All  "

#define LEN_VBEEPLEN           "\005"
#define TR_VBEEPLEN            "0====""=0===""==0==""===0=""====0"

#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "No REaREb"

#define LEN_VBLMODE            "\004"
#define TR_VBLMODE             "OFF ""Keys""Stks""Both""ON\0"

#define LEN_TRNMODE            "\003"
#define TR_TRNMODE             "OFF"" +="" :="

#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "CH1CH2CH3CH4"

#define LEN_DATETIME           "\005"
#define TR_DATETIME            "DATE:""TIME:"

#define LEN_VLCD               "\006"
#define TR_VLCD                "NormalOptrex"

#define LEN_COUNTRYCODES       TR("\002", "\007")
#define TR_COUNTRYCODES        TR("US""JP""EU", "America""Japan\0 ""Europe\0")

#define LEN_VTRIMINC           TR("\006","\013")
#define TR_VTRIMINC            TR("Expo  ""ExFine""Fine  ""Medium""Coarse","Exponential""Extra Fine ""Fine       ""Medium     ""Coarse     ")

#define LEN_RETA123            "\001"

#if defined(PCBGRUVIN9X)
  #if ROTARY_ENCODERS > 2
    #define TR_RETA123         "RETA123abcd"
  #else
    #define TR_RETA123         "RETA123ab"
  #endif
#elif defined(PCBTARANIS)
  #define TR_RETA123           "RETA12LR"
#else
  #define TR_RETA123           "RETA123"
#endif

#define LEN_VPROTOS            "\006"

#if defined(PXX)
  #define TR_PXX               "PXX\0  "
#elif defined(DSM2) || defined(IRPROTOS)
  #define TR_PXX               "[PXX]\0"
#else
  #define TR_PXX
#endif

#if defined(DSM2)
  #define TR_DSM2              "LP45\0 ""DSM2\0 ""DSMX\0 "
#elif defined(IRPROTOS)
  #define TR_DSM2              "[LP45]""[DSM2]""[DSMX]"
#else
  #define TR_DSM2
#endif

#if defined(IRPROTOS)
  #define TR_IRPROTOS          "SILV  TRAC09PICZ  SWIFT\0"
#else
  #define TR_IRPROTOS
#endif

#if defined(CPUARM)
  #define TR_XPPM
#else
  #define TR_XPPM              "PPM16\0""PPMsim"
#endif

#define TR_VPROTOS             "PPM\0  " TR_XPPM TR_PXX TR_DSM2 TR_IRPROTOS

#define LEN_POSNEG             "\003"
#define TR_POSNEG              "POS""NEG"

#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX             "\010"
#define TR_VMLTPX              "Add\0    ""Multiply""Replace\0"

#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="

#define LEN_VMIXTRIMS          "\003"
#define TR_VMIXTRIMS           "OFF""ON\0""Rud""Ele""Thr""Ail"

#define LEN_VCSWFUNC           "\005"
#define TR_VCSWFUNC            "---\0 ""a{x\0 ""a>x\0 ""a<x\0 ""|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 ""a=b\0 ""a>b\0 ""a<b\0 ""d}x\0 ""|d|}x"

#define LEN_VFSWFUNC           "\012"

#if defined(VARIO)
  #define TR_VVARIO            "Vario\0    "
#else
  #define TR_VVARIO            "[Vario]\0  "
#endif

#if defined(AUDIO)
  #define TR_SOUND             "Play Sound"
#else
  #define TR_SOUND             "Beep\0     "
#endif

#if defined(PCBTARANIS)
  #define TR_HAPTIC
#elif defined(HAPTIC)
  #define TR_HAPTIC            "Haptic\0   "
#else
  #define TR_HAPTIC            "[Haptic]\0 "
#endif

#if defined(VOICE)
  #if defined(PCBSKY9X)
    #define TR_PLAY_TRACK      "Play\0     "
  #else
    #define TR_PLAY_TRACK      "Play Track"
  #endif
  #define TR_PLAY_BOTH         "Play Both\0"
  #define TR_PLAY_VALUE        "Play Val\0 "
#else
  #define TR_PLAY_TRACK        "[Play Trk]"
  #define TR_PLAY_BOTH         "[Play Bth]"
  #define TR_PLAY_VALUE        "[Play Val]"
#endif

#define TR_CFN_VOLUME          "Volume\0   "
#define TR_CFN_BG_MUSIC        "BgMusic\0  ""BgMusic ||"

#if defined(SDCARD)
  #define TR_SDCLOGS           "SD Logs\0  "
#else
  #define TR_SDCLOGS           "[SD Logs]\0"
#endif

#ifdef GVARS
  #define TR_CFN_ADJUST_GVAR   "Adjust \0  "
#else
  #define TR_CFN_ADJUST_GVAR
#endif

#ifdef DEBUG
  #define TR_CFN_TEST          "Test\0"
#else
  #define TR_CFN_TEST
#endif

#if defined(CPUARM)
  #define TR_VFSWFUNC          "Safety\0   ""Trainer\0  ""Inst. Trim" TR_SOUND TR_HAPTIC "Reset\0    " TR_VVARIO TR_PLAY_TRACK TR_PLAY_VALUE TR_SDCLOGS TR_CFN_VOLUME "Backlight\0" TR_CFN_BG_MUSIC TR_CFN_ADJUST_GVAR TR_CFN_TEST
#elif defined(PCBGRUVIN9X)
  #define TR_VFSWFUNC          "Safety\0   ""Trainer\0  ""Inst. Trim" TR_SOUND TR_HAPTIC "Reset\0    " TR_VVARIO TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_SDCLOGS "Backlight\0" TR_CFN_ADJUST_GVAR TR_CFN_TEST
#else
  #define TR_VFSWFUNC          "Safety\0   ""Trainer\0  ""Inst. Trim" TR_SOUND TR_HAPTIC "Reset\0    " TR_VVARIO TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE "Backlight\0" TR_CFN_ADJUST_GVAR TR_CFN_TEST
#endif

#define LEN_VFSWRESET          TR("\005", "\011")

#if defined(FRSKY)
  #define TR_FSW_RESET_TELEM   TR("Telem", "Telemetry")
#else
  #define TR_FSW_RESET_TELEM
#endif

#if ROTARY_ENCODERS == 2
  #define TR_FSW_RESET_ROTENC  TR("REa\0 ""REb\0 ", "RotEnc A\0""RotEnc B\0")
#elif ROTARY_ENCODERS == 1
  #define TR_FSW_RESET_ROTENC  TR("R.Enc", "RotEnc\0  ")
#else
  #define TR_FSW_RESET_ROTENC
#endif

#define TR_VFSWRESET           TR("Tmr1\0""Tmr2\0""All\0 " TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC, "Timer 1  ""Timer 2  ""All      " TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC)

#define LEN_FUNCSOUNDS         "\006"
#define TR_FUNCSOUNDS          "Beep1 ""Beep2 ""Beep3 ""Warn1 ""Warn2 ""Cheep ""Ring  ""SciFi ""Robot ""Chirp ""Tada  ""Crickt""Siren ""AlmClk""Ratata""Tick  "

#define LEN_VTELEMCHNS         "\004"
#define TR_VTELEMCHNS          "---\0""Batt""Tmr1""Tmr2""Tx\0 ""Rx\0 ""A1\0 ""A2\0 ""Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""A1-\0""A2-\0""Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+""Cur+""Acc\0""Time"

#if defined(CPUARM)
  #define LEN_VTELEMUNIT_NORM  "\003"
  #define TR_VTELEMUNIT_NORM   "v\0 ""A\0 ""m/s""-\0 ""kmh""m\0 ""@\0 ""%\0 ""mA\0""mAh""W\0 "
  #define LEN_VTELEMUNIT_IMP   "\003"
  #define TR_VTELEMUNIT_IMP    "v\0 ""A\0 ""m/s""-\0 ""kts""ft\0""@\0 ""%\0 ""mA\0""mAh""W\0 "
#else
  #if defined(IMPERIAL_UNITS)
    #define LENGTH_UNIT        "ft\0"
    #define SPEED_UNIT         "kts"
  #else
    #define LENGTH_UNIT        "m\0 "
    #define SPEED_UNIT         "kmh"
  #endif
  #define LEN_VTELEMUNIT       "\003"
  #define TR_VTELEMUNIT        "v\0 ""A\0 ""m/s""-\0 " SPEED_UNIT LENGTH_UNIT "@\0 ""%\0 ""mA\0""mAh""W\0 "
#endif

#define STR_V                  (STR_VTELEMUNIT+1)
#define STR_A                  (STR_VTELEMUNIT+4)

#define LEN_VALARM             "\003"
#define TR_VALARM              "---""Yel""Org""Red"

#define LEN_VALARMFN           "\001"
#define TR_VALARMFN            "<>"

#define LEN_VTELPROTO          "\007"
#define TR_VTELPROTO           "None\0  ""Hub\0   ""WSHHigh"

#define LEN_VOLTSRC            "\003"
#define TR_VOLTSRC             "---""A1\0""A2\0""FAS""Cel"

#define LEN_VARIOSRC           "\005"
#define TR_VARIOSRC            "Alti\0""Alti+""Vario""A1\0  ""A2\0"

#define LEN_VSCREEN            "\004"
#define TR_VSCREEN             "Nums""Bars"

#define LEN_GPSFORMAT          "\004"
#define TR_GPSFORMAT           "HMS NMEA"

#define LEN2_VTEMPLATES        13
#define LEN_VTEMPLATES         "\015"
#define TR_VTEMPLATES          "Clear Mixes\0\0""Simple 4-CH \0""Sticky-T-Cut\0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE         "\004"
#define TR_VSWASHTYPE          "---\0""120\0""120X""140\0""90\0"

#define LEN_VKEYS              "\005"
#define TR_VKEYS               TR(" Menu"" Exit"" Down""   Up""Right"" Left", " Menu"" Exit""Enter"" Page"" Plus""Minus")

#define LEN_VRENCODERS         "\003"
#define TR_VRENCODERS          "REa""REb"

#define LEN_VSWITCHES          "\003"
#define LEN_VSRCRAW            "\004"

#if defined(PCBTARANIS)
  #define TR_POTS_VSRCRAW      "S1\0 ""S2\0 ""LS\0 ""RS\0 "
  #define TR_SW_VSRCRAW        "SA\0 ""SB\0 ""SC\0 ""SD\0 ""SE\0 ""SF\0 ""SG\0 ""SH\0 "
#elif defined(EXTRA_3POS)
  #define TR_POTS_VSRCRAW      "P1\0 ""P2\0 "
  #define TR_SW_VSRCRAW        "3P1\0""3P2\0"
  #define TR_9X_3POS_SWITCHES  "ID0""ID1""ID2""ID3""ID4""ID5"
#else
  #define TR_POTS_VSRCRAW      "P1\0 ""P2\0 ""P3\0 "
  #define TR_SW_VSRCRAW        "3POS"
  #define TR_9X_3POS_SWITCHES  "ID0""ID1""ID2"
#endif

#if defined(CPUARM)
  #define TR_CUSTOMSW          "CS1""CS2""CS3""CS4""CS5""CS6""CS7""CS8""CS9""CSA""CSB""CSC""CSD""CSE""CSF""CSG""CSH""CSI""CSJ""CSK""CSL""CSM""CSN""CSO""CSP""CSQ""CSR""CSS""CST""CSU""CSV""CSW"
#elif defined(PCBGRUVIN9X) || defined(CPUM128)
  #define TR_CUSTOMSW          "CS1""CS2""CS3""CS4""CS5""CS6""CS7""CS8""CS9""CSA""CSB""CSC""CSD""CSE""CSF"
#else
  #define TR_CUSTOMSW          "CS1""CS2""CS3""CS4""CS5""CS6""CS7""CS8""CS9""CSA""CSB""CSC"
#endif

#if defined(PCBTARANIS)
  #define TR_VSWITCHES         "SA\300""SA-""SA\301""SB\300""SB-""SB\301""SC\300""SC-""SC\301""SD\300""SD-""SD\301""SE\300""SE-""SE\301""SF\300""SF\301""SG\300""SG-""SG\301""SH\300""SH\301" TR_CUSTOMSW "One"
#else
  #define TR_VSWITCHES         TR_9X_3POS_SWITCHES "THR""RUD""ELE""AIL""GEA""TRN" TR_CUSTOMSW "One"
#endif

#if defined(PCBSKY9X)
  #define TR_ROTARY_ENCODERS_VSRCRAW "REnc"
#elif defined(PCBGRUVIN9X) && ROTARY_ENCODERS > 2
  #define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb ""REc ""REd "
#elif defined(PCBGRUVIN9X) && ROTARY_ENCODERS <= 2
  #define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb "
#else
  #define TR_ROTARY_ENCODERS_VSRCRAW
#endif

#if defined(HELI)
  #define TR_CYC_VSRCRAW       "CYC1""CYC2""CYC3"
#else
  #define TR_CYC_VSRCRAW       "[C1]""[C2]""[C3]"
#endif

#define TR_VSRCRAW             "---\0""Rud\0""Ele\0""Thr\0""Ail\0" TR_POTS_VSRCRAW TR_ROTARY_ENCODERS_VSRCRAW "MAX\0" TR_CYC_VSRCRAW "TrmR" "TrmE" "TrmT" "TrmA" TR_SW_VSRCRAW

#define LEN_VTMRMODES          "\003"
#define TR_VTMRMODES           "OFF""ABS""THs""TH%""THt"

// ZERO TERMINATED STRINGS
#define INDENT                 "\001"
#define LEN_INDENT             1
#define INDENT_WIDTH           (FW/2)

#if defined(PCBTARANIS)
  #define TR_ENTER             "[ENTER]"
#else
  #define TR_ENTER             "[MENU]"
#endif

#define TR_POPUPS              TR_ENTER "\010[EXIT]"
#define OFS_EXIT               sizeof(TR_ENTER)

#define TR_MENUWHENDONE        CENTER "\006" TR_ENTER " WHEN DONE"
#define TR_FREE                "free"
#define TR_DELETEMODEL         "DELETE MODEL"
#define TR_COPYINGMODEL        "Copying model..."
#define TR_MOVINGMODEL         "Moving model..."
#define TR_LOADINGMODEL        "Loading model..."
#define TR_NAME                "Name"
#define TR_MODELNAME           "Model Name"
#define TR_PHASENAME           "Mode Name"
#define TR_MIXNAME             "Mix Name"
#define TR_EXPONAME            "Expo Name"
#define TR_BITMAP              "Model Image"
#define TR_TIMER               TR("Timer","Timer ")
#define TR_ELIMITS             TR("E.Limits","Extended Limits")
#define TR_ETRIMS              TR("E.Trims","Extended Trims")
#define TR_TRIMINC             "Trim Step"
#define TR_TTRACE              TR("T-Source","Throttle Source")
#define TR_TTRIM               TR("T-Trim","Throttle Trim")
#define TR_BEEPCTR             TR("Ctr Beep","Center Beep")
#define TR_PROTO               TR(INDENT"Proto",INDENT"Protocol")
#define TR_PPMFRAME            "PPM frame"
#define TR_MS                  "ms"
#define TR_SWITCH              "Switch"
#define TR_TRIMS               "Trims"
#define TR_FADEIN              "Fade In"
#define TR_FADEOUT             "Fade Out"
#define TR_DEFAULT             "(default)"
#define TR_CHECKTRIMS          CENTER "\006Check\012Trims"
#define OFS_CHECKTRIMS         CENTER_OFS+(9*FW)
#define TR_SWASHTYPE           "Swash Type"
#define TR_COLLECTIVE          TR("Collective","Collective source")
#define TR_SWASHRING           "Swash Ring"
#define TR_ELEDIRECTION        TR("ELE Direction","Long. cyc. direction")
#define TR_AILDIRECTION        TR("AIL Direction","Lateral cyc. direction")
#define TR_COLDIRECTION        TR("PIT Direction","Coll. pitch direction")
#define TR_MODE                INDENT"Mode"
#define TR_NOFREEEXPO          "No free expo!"
#define TR_NOFREEMIXER         "No free mixer!"
#define TR_INSERTMIX           "INSERT MIX "
#define TR_EDITMIX             "EDIT MIX "
#define TR_SOURCE              INDENT"Source"
#define TR_WEIGHT              "Weight"
#define TR_EXPO                TR("Expo","Exponential")
#define TR_SIDE                "Side"
#define TR_DIFFERENTIAL        "Differ"
#define TR_OFFSET              INDENT"Offset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DRex"
#define TR_CURVE               "Curve"
#define TR_FLMODE              TR("Mode","Modes")
#define TR_MIXWARNING          "Warning"
#define TR_OFF                 "OFF"
#define TR_MULTPX              "Multpx"
#define TR_DELAYDOWN           "Delay Dn"
#define TR_DELAYUP             "Delay Up"
#define TR_SLOWDOWN            "Slow  Dn"
#define TR_SLOWUP              "Slow  Up"
#define TR_MIXER               "MIXER"
#define TR_CV                  "CV"
#define TR_GV                  "GV"
#define TR_ACHANNEL            "A\004channel"
#define TR_RANGE               INDENT"Range"
#define TR_BAR                 "Bar"
#define TR_ALARM               INDENT"Alarm"
#define TR_USRDATA             "UsrData"
#define TR_BLADES              INDENT"Blades"
#define TR_SCREEN              "Screen "
#define TR_SOUND_LABEL         "Sound"
#define TR_LENGTH              INDENT"Length"
#define TR_SPKRPITCH           INDENT"Pitch"
#define TR_HAPTIC_LABEL        "Haptic"
#define TR_HAPTICSTRENGTH      INDENT"Strength"
#define TR_CONTRAST            "Contrast"
#define TR_ALARMS_LABEL        "Alarms"
#define TR_BATTERY_RANGE       TR("Battery range","Battery meter range")
#define TR_BATTERYWARNING      INDENT"Battery Low"
#define TR_INACTIVITYALARM     INDENT"Inactivity"
#define TR_MEMORYWARNING       INDENT"Memory Low"
#define TR_ALARMWARNING        INDENT"Sound Off"
#define TR_RENAVIG             "RotEnc Navig"
#define TR_THROTTLEREVERSE     TR("T-Reverse", "Throttle reverse")
#define TR_MINUTEBEEP          TR(INDENT"Minute",INDENT"Minute call")
#define TR_BEEPCOUNTDOWN       INDENT"Countdown"
#define TR_PERSISTENT          TR(INDENT"Persist.",INDENT"Persistent")
#define TR_BACKLIGHT_LABEL     "Backlight"
#define TR_BLDELAY             INDENT"Duration"
#define TR_BLONBRIGHTNESS      INDENT"ON Brightness"
#define TR_BLOFFBRIGHTNESS     INDENT"OFF Brightness"
#define TR_SPLASHSCREEN        "Splash screen"
#define TR_THROTTLEWARNING     TR("T-Warning","Throttle Warning")
#define TR_SWITCHWARNING       TR("S-Warning","Switch Warning")
#define TR_TIMEZONE            TR("Time Zone","GPS Time zone")
#define TR_RXCHANNELORD        TR("Rx Channel Ord","Default channel order")
#define TR_SLAVE               "Slave"
#define TR_MODESRC             "Mode\006% Source"
#define TR_MULTIPLIER          "Multiplier"
#define TR_CAL                 "Cal"
#define TR_VTRIM               "Trim- +"
#define TR_BG                  "BG:"
#define TR_MENUTOSTART         CENTER "\006" TR_ENTER " TO START"
#define TR_SETMIDPOINT         CENTER "\003SET STICKS MIDPOINT"
#define TR_MOVESTICKSPOTS      CENTER "\006MOVE STICKS/POTS"
#define TR_RXBATT              "Rx Batt:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              CENTER "NO DATA"
#define TR_TM1TM2              "TM1\032TM2"
#define TR_THRTHP              "THR\032TH%"
#define TR_TOT                 "TOT"
#define TR_TMR1LATMAXUS        "Tmr1Lat max\006us"
#define STR_US (STR_TMR1LATMAXUS+12)
#define TR_TMR1LATMINUS        "Tmr1Lat min\006us"
#define TR_TMR1JITTERUS        "Tmr1 Jitter\006us"

#if defined(CPUARM)
  #define TR_TMIXMAXMS         "Tmix max\012ms"
#else
  #define TR_TMIXMAXMS         "Tmix max\014ms"
#endif

#define TR_T10MSUS             "T10ms\016us"
#define TR_FREESTACKMINB       "Free Stack\010b"
#define TR_MENUTORESET         CENTER TR_ENTER " to reset"
#define TR_PPM                 "PPM"
#define TR_CH                  "CH"
#define TR_MODEL               "MODEL"
#define TR_FP                  "FM"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "EEPROM low mem"
#define TR_ALERT               "\016ALERT"
#define TR_PRESSANYKEYTOSKIP   CENTER "Press any key to skip"
#define TR_THROTTLENOTIDLE     CENTER "Throttle not idle"
#define TR_ALARMSDISABLED      CENTER "Alarms Disabled"
#define TR_PRESSANYKEY         TR("\010Press any Key", CENTER "Press any Key")
#define TR_BADEEPROMDATA       CENTER "Bad EEprom Data"
#define TR_EEPROMFORMATTING    CENTER "Formatting EEPROM"
#define TR_EEPROMOVERFLOW      CENTER "EEPROM overflow"
#define TR_MENURADIOSETUP      "RADIO SETUP"
#define TR_MENUDATEANDTIME     "DATE AND TIME"
#define TR_MENUTRAINER         "TRAINER"
#define TR_MENUVERSION         "VERSION"
#define TR_MENUDIAG            TR("SWITCHES","SWITCH TEST")
#define TR_MENUANA             TR("ANAS","ANALOG INPUTS")
#define TR_MENUCALIBRATION     "CALIBRATION"
#define TR_TRIMS2OFFSETS       "\006Trims => Offsets"
#define TR_MENUMODELSEL        TR("MODELSEL","MODEL SELECTION")
#define TR_MENUSETUP           TR("SETUP","MODEL SETUP")
#define TR_MENUFLIGHTPHASE     "FLIGHT MODE"
#define TR_MENUFLIGHTPHASES    "FLIGHT MODES"
#define TR_MENUHELISETUP       "HELI SETUP"

#if defined(PPM_CENTER_ADJUSTABLE) || defined(PPM_LIMITS_SYMETRICAL) // The right menu titles for the gurus ...
  #define TR_MENUDREXPO        "STICKS"
  #define TR_MENULIMITS        "SERVOS"
#else
  #define TR_MENUDREXPO        "DR/EXPO"
  #define TR_MENULIMITS        "LIMITS"
#endif

#define TR_MENUCURVES          "CURVES"
#define TR_MENUCURVE           "CURVE"
#define TR_MENUCUSTOMSWITCH    "CUSTOM SWITCH"
#define TR_MENUCUSTOMSWITCHES  "CUSTOM SWITCHES"
#define TR_MENUCUSTOMFUNC      "CUSTOM FUNCTIONS"
#define TR_MENUTELEMETRY       "TELEMETRY"
#define TR_MENUTEMPLATES       "TEMPLATES"
#define TR_MENUSTAT            "STATS"
#define TR_MENUDEBUG           "DEBUG"
#define TR_RXNUM               TR("RxNum", INDENT"Receiver No.")
#define TR_SYNCMENU            "Sync "TR_ENTER
#define TR_LIMIT               INDENT"Limit"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Latitude"
#define TR_LONGITUDE           "Longitude"
#define TR_GPSCOORD            TR("GPS Coords", "GPS coordinate format")
#define TR_VARIO               TR("Vario", "Variometer")
#define TR_SHUTDOWN            "SHUTTING DOWN"
#define TR_BATT_CALIB          "Battery Calib"
#define TR_CURRENT_CALIB       "Current Calib"
#define TR_VOLTAGE             INDENT"Voltage"
#define TR_CURRENT             INDENT"Current"
#define TR_SELECT_MODEL        "Select Model"
#define TR_CREATE_MODEL        "Create Model"
#define TR_COPY_MODEL          "Copy Model"
#define TR_MOVE_MODEL          "Move Model"
#define TR_BACKUP_MODEL        "Backup Model"
#define TR_DELETE_MODEL        "Delete Model"
#define TR_RESTORE_MODEL       "Restore Model"
#define TR_SDCARD_ERROR        "SDCARD Error"
#define TR_NO_SDCARD           "No SDCARD"
#define TR_INCOMPATIBLE        "Incompatible"
#define TR_WARNING             "WARNING"
#define TR_EEPROMWARN          "EEPROM"
#define TR_THROTTLEWARN        "THROTTLE"
#define TR_ALARMSWARN          "ALARMS"
#define TR_SWITCHWARN          "SWITCH"
#define TR_INVERT_THR          TR("Invert Thr?","Invert Throttle?")
#define TR_SPEAKER_VOLUME      INDENT"Volume"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          "Brightness"
#define TR_CPU_TEMP            "CPU Temp.\016>"
#define TR_CPU_CURRENT         "Current\022>"
#define TR_CPU_MAH             "Consumpt."
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "MB Temp. \016>"
#define TR_CAPAWARNING         INDENT"Capacity Low"
#define TR_TEMPWARNING         INDENT"Overheat"
#define TR_FUNC                "Func"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Duration"
#define TR_DELAY               "Delay"
#define TR_SD_CARD             "SD CARD"
#define TR_SDHC_CARD           "SD-HC CARD"
#define TR_NO_SOUNDS_ON_SD     "No Sounds on SD"
#define TR_NO_MODELS_ON_SD     "No Models on SD"
#define TR_NO_BITMAPS_ON_SD    "No Bitmaps on SD"
#define TR_PLAY_FILE           "Play"
#define TR_DELETE_FILE         "Delete"
#define TR_COPY_FILE           "Copy"
#define TR_RENAME_FILE         "Rename"
#define TR_REMOVED             " removed"
#define TR_SD_INFO             "Information"
#define TR_SD_FORMAT           "Format"
#define TR_NA                  "N/A"
#define TR_HARDWARE            "HARDWARE"
#define TR_FORMATTING          "Formatting..."
#define TR_TEMP_CALIB          "Temp. Calib"
#define TR_TIME                "Time"
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Type:"
#define TR_SD_SPEED            "Speed:"
#define TR_SD_SECTORS          "Sectors:"
#define TR_SD_SIZE             "Size:"
#define TR_CURVE_TYPE          "Type"
#define TR_GLOBAL_VARS         "Global Variables"
#define TR_OWN                 "Own"
#define TR_DATE                "Date"
#define TR_ROTARY_ENCODER      "R.Encs"
#define TR_CHANNELS_MONITOR    "CHANNEL MONITOR"
#define TR_INTERNALRF          "Internal RF"
#define TR_EXTERNALRF          "External RF"
#define TR_FAILSAFE            INDENT"Failsafe mode"
#define TR_FAILSAFESET         "FAILSAFE SETTINGS"
#define TR_COUNTRYCODE         "Country Code"
