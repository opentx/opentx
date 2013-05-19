/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Kjell Kernen <kjell.kernen@gmail.com>
 * *
 *
 *
 *
 *
 *
 * 
 *    << Blank area used to row align file with en.h  >>
 *
 *
 *
 *
 *
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
 *
*/ 

// NON ZERO TERMINATED STRINGS
#define LEN_OFFON              "\003"
#define TR_OFFON               "AV ""P\203\0"

#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""INV"

#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014KN\0016KN\0018KN10KN12KN14KN16KN"

#define LEN_VBEEPMODE          "\005"
#define TR_VBEEPMODE           "Tyst ""Alarm""EjKnp""Alla "

#define LEN_VBEEPLEN           "\005"
#define TR_VBEEPLEN            "0====""=0===""==0==""===0=""====0"

#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "No REaREb"

#define LEN_VBLMODE            "\004"
#define TR_VBLMODE             "AV  ""Knpp""Spak""B\200da""P\203\0"

#define LEN_TRNMODE            "\003"
#define TR_TRNMODE             " AV"" +="" :="

#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "KN1KN2KN3KN4"

#define LEN_DATETIME           "\005"
#define TR_DATETIME            "DAG: ""TID: "

#define LEN_VLCD               "\006"
#define TR_VLCD                "NormalOptrex"

#define LEN_COUNTRYCODES       TR("\002", "\007")
#define TR_COUNTRYCODES        TR("US""JP""EU", "Amerika""Japan\0 ""Europa\0")

#define LEN_VTRIMINC           TR("\006","\014")
#define TR_VTRIMINC            TR("Expo  ""xFin  ""Fin   ""Medium""Grov  ","Exponentiell""Extra Fin   ""Fin         ""Medium      ""Grov        ")

#define LEN_RETA123            "\001"

#if defined(PCBGRUVIN9X)
  #if ROTARY_ENCODERS > 2
    #define TR_RETA123         "RHGS123abcd"
  #else
    #define TR_RETA123         "RHGS123ab"
  #endif
#elif defined(PCBTARANIS)
  #define TR_RETA123           "RHGS12HV"
#else
  #define TR_RETA123           "RHGS123"
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
#define TR_POSNEG              "Pos""Neg"

#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX             "\010"
#define TR_VMLTPX              "Addera  ""F\202rst\201rk""Ers\201tt  "

#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="

#define LEN_VMIXTRIMS          "\003"
#define TR_VMIXTRIMS           "AV ""P\203\0""Rod""Hjd""Gas""Ske"

#define LEN_VCSWFUNC           "\005"
#define TR_VCSWFUNC            "---\0 ""a{x\0 ""a>x\0 ""a<x\0 ""|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 ""a=b\0 ""a>b\0 ""a<b\0 ""d}x\0 ""|d|}x"

#define LEN_VFSWFUNC           "\012"

#if defined(VARIO)
  #define TR_VVARIO            "Vario     "
#else
  #define TR_VVARIO            "[Vario]   "
#endif

#if defined(AUDIO)
  #define TR_SOUND             "Spela Ljud"
#else
  #define TR_SOUND             "Pip\0      "
#endif

#if defined(PCBTARANIS)
  #define TR_HAPTIC
#elif defined(HAPTIC)
  #define TR_HAPTIC            "Vibrator\0 "
#else
  #define TR_HAPTIC            "[Vibrator]"
#endif

#if defined(VOICE)
  #if defined(PCBSKY9X)
    #define TR_PLAY_TRACK      "Spela Sp\200r"
  #else
    #define TR_PLAY_TRACK      "Spela\0    "
  #endif
  #define TR_PLAY_BOTH         "Spela B\203da"
  #define TR_PLAY_VALUE        "Spela Val "
#else
  #define TR_PLAY_TRACK        "[Sp. Sp\200r]"
  #define TR_PLAY_BOTH         "[Sp. B\200da]"
  #define TR_PLAY_VALUE        "[Sp. V\201rd]"
#endif

#define TR_CFN_VOLUME          "Volym\0    "
#define TR_CFN_BG_MUSIC        "BgMusik\0  ""BgMusik ||"

#if defined(SDCARD)
  #define TR_SDCLOGS           "SD Loggar\0"
#else
  #define TR_SDCLOGS           "[SD Logg]\0"
#endif
 
#ifdef GVARS
  #define TR_CFN_ADJUST_GVAR   "Justera\0  "
#else
  #define TR_CFN_ADJUST_GVAR
#endif

#ifdef DEBUG
  #define TR_CFN_TEST          "Test\0"
#else
  #define TR_CFN_TEST
#endif

#if defined(CPUARM)
  #define TR_VFSWFUNC          "S\201kra\0    ""Trainer\0  ""S\201tt Trim\0" TR_SOUND TR_HAPTIC "Nollst\201ll\0" TR_VVARIO TR_PLAY_TRACK TR_PLAY_VALUE TR_SDCLOGS TR_CFN_VOLUME "Belysning\0" TR_CFN_BG_MUSIC TR_CFN_ADJUST_GVAR TR_CFN_TEST
#elif defined(PCBGRUVIN9X)
  #define TR_VFSWFUNC          "S\201kra\0    ""Trainer\0  ""S\201tt Trim\0" TR_SOUND TR_HAPTIC "Nollst\201ll\0" TR_VVARIO TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_SDCLOGS "Belysning\0" TR_CFN_ADJUST_GVAR TR_CFN_TEST
#else
  #define TR_VFSWFUNC          "S\201kra\0    ""Trainer\0  ""S\201tt Trim\0" TR_SOUND TR_HAPTIC "Nollst\201ll\0" TR_VVARIO TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE "Belysning\0" TR_CFN_ADJUST_GVAR TR_CFN_TEST
#endif

#define LEN_VFSWRESET          TR("\005", "\011")

#if defined(FRSKY)
  #define TR_FSW_RESET_TELEM   TR("Telem", "Telemetri")
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

#define TR_VFSWRESET           TR("Tmr1\0""Tmr2\0""Allt\0" TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC, "Timer 1  ""Timer 2  ""Allt     " TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC)

#define LEN_FUNCSOUNDS         "\006"
#define TR_FUNCSOUNDS          "Pip1  ""Pip2  ""Pip3  ""Varn1 ""Varn2 ""F\200r   ""Ring  ""SciFi ""Robot ""F\200gel ""Tada  ""Syrsa ""Siren ""Alarm ""Ratata""Tick  "

#define LEN_VTELEMCHNS         "\004"
#if defined(PCBTARANIS)
  #define TR_RSSI_0            "SWR\0"
  #define TR_RSSI_1            "RSSI"
#else
  #define TR_RSSI_0            "Tx\0 "
  #define TR_RSSI_1            "Rx\0 "
#endif
#define TR_VTELEMCHNS          "---\0""Batt""Tmr1""Tmr2""Tx\0 ""Rx\0 ""A1\0 ""A2\0 ""H\202jd""Varv""Tank""T1\0 ""T2\0 ""Fart""Avst""GHjd""Batt""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VFrt""A1-\0""A2-\0""Hjd-""Hjd+""Rpm+""T1+\0""T2+\0""Frt+""Avs+""Cur+""Acc\0""Tid\0"

#if defined(CPUARM)
  #define LEN_VUNITSSYSTEM     TR("\006", "\010")
  #define TR_VUNITSSYSTEM      TR("MetricImper.", "Metric\0 Imperial")
  #define LEN_VTELEMUNIT_NORM  "\003"
  #define TR_VTELEMUNIT_NORM   "v\0 ""A\0 ""m/s""-\0 ""kmh""m\0 ""@\0 ""%\0 ""mA\0""mAh""W\0 "
  #define LEN_VTELEMUNIT_IMP   "\003"
  #define TR_VTELEMUNIT_IMP    "v\0 ""A\0 ""m/s""-\0 ""kts""ft\0""@\0 ""%\0 ""mA\0""mAh""W\0 "
#else
  #if defined(IMPERIAL_UNITS)
    #define LENGTH_UNIT        "fot"
    #define SPEED_UNIT         "knp"
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
#define TR_VALARM              "---""Gul""Ora""R\202d"

#define LEN_VALARMFN           "\001"
#define TR_VALARMFN            "<>"

#define LEN_VTELPROTO          "\007"
#define TR_VTELPROTO           "---\0   ""Hub\0   ""WSHH\202g."

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
#define TR_VTEMPLATES          "Nolla Mixar\0\0""Enkel 4kanal\0""Gasklippning\0""V-Stj\201rt    \0""Deltavinge  \0""eCCPM       \0""Helikopter  \0""Servotest   \0"

#define LEN_VSWASHTYPE         "\004"
#define TR_VSWASHTYPE          "--- ""120 ""120X""140 ""90\0"

#define LEN_VKEYS              "\005"
#define TR_VKEYS               TR(" Menu"" Exit""  Ned""  Upp""H\202ger""V\201nst", " Menu"" Exit""Enter"" Page"" Plus""Minus")

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
  #define TR_CUSTOMSW          "LB1""LB2""LB3""LB4""LB5""LB6""LB7""LB8""LB9""LBA""LBB""LBC""LBD""LBE""LBF""LBG""LBH""LBI""LBJ""LBK""LBL""LBM""LBN""LBO""LBP""LBQ""LBR""LBS""LBT""LBU""LBV""LBW"
#elif defined(PCBGRUVIN9X) || defined(CPUM2561) || defined(CPUM128)
  #define TR_CUSTOMSW          "LB1""LB2""LB3""LB4""LB5""LB6""LB7""LB8""LB9""LBA""LBB""LBC""LBD""LBE""LBF"
#else
  #define TR_CUSTOMSW          "LB1""LB2""LB3""LB4""LB5""LB6""LB7""LB8""LB9""LBA""LBB""LBC"
#endif

#if defined(PCBTARANIS)
  #define TR_VSWITCHES         "SA\300""SA-""SA\301""SB\300""SB-""SB\301""SC\300""SC-""SC\301""SD\300""SD-""SD\301""SE\300""SE-""SE\301""SF\300""SF\301""SG\300""SG-""SG\301""SH\300""SH\301" TR_CUSTOMSW " P\203"
#else
  #define TR_VSWITCHES         TR_9X_3POS_SWITCHES "GAS""SID""HJD""SKE""LAN""TRN" TR_CUSTOMSW " P\203"
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
  #define TR_CYC_VSRCRAW   "CYK1""CYK2""CYK3"
#else
  #define TR_CYC_VSRCRAW   "[C1]""[C2]""[C3]"
#endif

#define TR_VSRCRAW             "---\0""Rod\0""H\202j\0""Gas\0""Ske\0" TR_POTS_VSRCRAW TR_ROTARY_ENCODERS_VSRCRAW "MAX " TR_CYC_VSRCRAW "TrmR" "TrmH" "TrmG" "TrmS" TR_SW_VSRCRAW

#define LEN_VTMRMODES          "\003"
#define TR_VTMRMODES           "AV ""ABS""GAs""GA%""GAt"

#if defined (PCBTARANIS)
  #define LEN_VTRAINERMODES      "\006"
  #define TR_VTRAINERMODES       "MasterSlave\0"
#endif

// ZERO TERMINATED STRINGS
#define INDENT                 "\001"
#define LEN_INDENT             1
#define INDENT_WIDTH           (FW/2)

#if defined(PCBTARANIS)
  #define TR_ENTER             "[ENTER]"
#else
  #define TR_ENTER             "[MENU]"
#endif

#define TR_POPUPS              TR_ENTER"\010[EXIT]"
#define OFS_EXIT               sizeof(TR_ENTER)

#define TR_MENUWHENDONE        CENTER"\006"TR_ENTER" Avslutar "
#define TR_FREE                "kvar"
#define TR_DELETEMODEL         "Radera Modell"
#define TR_COPYINGMODEL        "Kopierar Modell "
#define TR_MOVINGMODEL         "Flyttar Modell "
#define TR_LOADINGMODEL        "Laddar Modell   "
#define TR_NAME                "Namn"
#define TR_MODELNAME           "Modellnamn"
#define TR_PHASENAME           "Modnamn   "
#define TR_MIXNAME             "Mixnamn "
#define TR_EXPONAME            "Exponamn "
#define TR_BITMAP              "Modell-bild"
#define TR_TIMER               TR("Timer","Timer ")
#define TR_ELIMITS             TR("Gr\201nser++","Ut\202kade Gr\201nser")
#define TR_ETRIMS              TR("Trimmar++","Ut\202kade Trimmar")
#define TR_TRIMINC             TR("Trim\202kn.","Trim\202kning")
#define TR_TTRACE              "F\202lj Gas"
#define TR_TTRIM               TR("G-K\201lla","Gas-k\201lla")
#define TR_BEEPCTR             TR("Cent.pip", "Centerpip")
#define TR_PROTO               TR(INDENT"Proto",INDENT"Protokoll")
#define TR_PPMFRAME            "PPM-paket"
#define TR_MS                  "ms"
#define TR_SWITCH              "Brytare"
#define TR_TRIMS               "Trimmar"
#define TR_FADEIN              "Tona In"
#define TR_FADEOUT             "Tona Ut"
#define TR_DEFAULT             "Standard"
#define TR_CHECKTRIMS          CENTER"\006Kolla\012Trimmar"
#define OFS_CHECKTRIMS         CENTER_OFS+(9*FW)
#define TR_SWASHTYPE           "Swashtyp"
#define TR_COLLECTIVE          "Kollektiv"
#define TR_SWASHRING           "Swashring"
#define TR_ELEDIRECTION        "H\205JD Riktning"
#define TR_AILDIRECTION        "SKEV Riktning"
#define TR_COLDIRECTION        "KOLL.Riktning"
#define TR_MODE                INDENT"L\201ge"
#define TR_NOFREEEXPO          "Expo saknas!"
#define TR_NOFREEMIXER         "Mixer saknas!"
#define TR_INSERTMIX           "Addera Mix"
#define TR_EDITMIX             "Redigera Mix"
#define TR_SOURCE              INDENT"K\201lla"
#define TR_WEIGHT              "Vikt"
#define TR_EXPO                TR("Expo","Exponentiell")
#define TR_SIDE                "Sida"
#define TR_DIFFERENTIAL        "Diff."
#define TR_OFFSET              INDENT"Offset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DRex"
#define TR_CURVE               "Kurva"
#define TR_FLMODE              TR("Mod","Moder")
#define TR_MIXWARNING          "Varning"
#define TR_OFF                 "AV "
#define TR_MULTPX              "Multpx"
#define TR_DELAYDOWN           "Dr\202j Ned"
#define TR_DELAYUP             "Dr\202j Upp"
#define TR_SLOWDOWN            "Tr\202g Ned"
#define TR_SLOWUP              "Tr\202g Upp"
#define TR_MIXER               "Mixar"
#define TR_CV                  "KU"
#define TR_GV                  "GV"
#define TR_ACHANNEL            "A\004kanal  "
#define TR_RANGE               INDENT"MinMax"
#define TR_BAR                 "Data"
#define TR_ALARM               INDENT"Alarm"
#define TR_USRDATA             "Anv\201ndardata"
#define TR_BLADES              INDENT"Blad"
#define TR_SCREEN              "Sk\201rm  "
#define TR_SOUND_LABEL         "Ljud "
#define TR_LENGTH              INDENT"Tid"
#define TR_SPKRPITCH           INDENT"Ton"
#define TR_HAPTIC_LABEL        "Vibrator"
#define TR_HAPTICSTRENGTH      INDENT"Styrka"
#define TR_CONTRAST            "Kontrast"
#define TR_ALARMS_LABEL        "Alarm"
#define TR_BATTERY_RANGE       "Batteri-MinMax"
#define TR_BATTERYWARNING      INDENT"Batteri"
#define TR_INACTIVITYALARM     INDENT"Inaktivitet"
#define TR_MEMORYWARNING       INDENT"Lite Minne"
#define TR_ALARMWARNING        INDENT"Ljud AV"
#define TR_RENAVIG             "RotEnc Navig"
#define TR_THROTTLEREVERSE     "Reverserad Gas"
#define TR_MINUTEBEEP          INDENT"Varje Minut"
#define TR_BEEPCOUNTDOWN       INDENT"R\201kna Ned"
#define TR_PERSISTENT          TR("J\201mt p\200 ",INDENT"Alltid P\200")
#define TR_BACKLIGHT_LABEL     "Belysning"
#define TR_BLDELAY             INDENT"Av efter"
#define TR_BLONBRIGHTNESS      INDENT"P\200 Ljusstyrka"
#define TR_BLOFFBRIGHTNESS     INDENT"AV Ljusstyrka"
#define TR_SPLASHSCREEN        "Bild vid start"
#define TR_THROTTLEWARNING     TR("Gasvarn.","Gasvarning")
#define TR_SWITCHWARNING       TR("Bryt.varn.","Brytarvarning")
#define TR_TIMEZONE            TR("Tidszon","GPS Tidszon")
#define TR_RXCHANNELORD        "Kanalordning RX"
#define TR_SLAVE               "Slav"
#define TR_MODESRC             " l\201ge\006% k\201lla"
#define TR_MULTIPLIER          "Styrka"
#define TR_CAL                 "Kalib."
#define TR_VTRIM               "Trim- +"
#define TR_BG                  "BG:"
#define TR_MENUTOSTART         CENTER"\006"TR_ENTER" Startar "
#define TR_SETMIDPOINT         CENTER"\010Centrera Allt"
#define TR_MOVESTICKSPOTS      CENTER"\004R\202r Rattar/Spakar"
#define TR_RXBATT              "Rx Batt:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              CENTER"NO DATA"
#define TR_TM1TM2              "TM1\032TM2"
#define TR_THRTHP              "THR\032TH%"
#define TR_TOT                 "TOT"
#define TR_TMR1LATMAXUS        "Tmr1Lat max\006us"
#define STR_US                 (STR_TMR1LATMAXUS+12)
#define TR_TMR1LATMINUS        "Tmr1Lat min\006us"
#define TR_TMR1JITTERUS        "Tmr1 Jitter\006us"

#if defined(CPUARM)
  #define TR_TMIXMAXMS         "Tmix max\012ms"
#else
  #define TR_TMIXMAXMS         "Tmix max\014ms"
#endif

#define TR_T10MSUS             "T10ms\016us"
#define TR_FREESTACKMINB       "Free Stack\010b"
#define TR_MENUTORESET         CENTER TR_ENTER" Nollar"
#define TR_PPM                 "PPM"
#define TR_CH                  "KN"
#define TR_MODEL               "Modell"
#define TR_FP                  "FF"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "EEPROM low mem"
#define TR_ALERT               "\016OBS"
#define TR_PRESSANYKEYTOSKIP   CENTER"Tryck ned en knapp"
#define TR_THROTTLENOTIDLE     CENTER"Gasen ej avst\201ngd!"
#define TR_ALARMSDISABLED      CENTER"Alarmen Avst\201ngda!"
#define TR_PRESSANYKEY         TR("Tryck ned en knapp",CENTER"Tryck ned en knapp")
#define TR_BADEEPROMDATA       CENTER"EEprom Datafel "
#define TR_EEPROMFORMATTING    CENTER"Formaterar EEprom"
#define TR_EEPROMOVERFLOW      CENTER"Fel i EEprom"
#define TR_MENURADIOSETUP      "Inst\201llningar"
#define TR_MENUDATEANDTIME     "Dag och Tid"
#define TR_MENUTRAINER         "Trainer (PPM in)"
#define TR_MENUVERSION         "Version"
#define TR_MENUDIAG            TR("Brytare","Test av Brytare")
#define TR_MENUANA             "Analoga V\201rden"
#define TR_MENUCALIBRATION     "Kalibrering"
#define TR_TRIMS2OFFSETS       "\004[Spara Trimv\201rden]"
#define TR_MENUMODELSEL        TR("Modell","V\201lj Modell")
#define TR_MENUSETUP           TR("V\201rden","Modellinst\201llningar")
#define TR_MENUFLIGHTPHASE     "Flygmod"
#define TR_MENUFLIGHTPHASES    "Flygmoder"
#define TR_MENUHELISETUP       "Helikopter"

#if defined(PPM_CENTER_ADJUSTABLE) || defined(PPM_LIMITS_SYMETRICAL) // The right menu titles for the gurus ...
  #define TR_MENUDREXPO        "Spakar" 
  #define TR_MENULIMITS        "Outputs"
#else
  #define TR_MENUDREXPO        "DR/Expo"
  #define TR_MENULIMITS        "Gr\201nser"
#endif

#define TR_MENUCURVES          "Kurvor"
#define TR_MENUCURVE           "Kurva"
#define TR_MENUCUSTOMSWITCH    "Logisk Brytare"
#define TR_MENUCUSTOMSWITCHES  "Logiska Brytare"
#define TR_MENUCUSTOMFUNC      "BrytarFunktioner"
#define TR_MENUTELEMETRY       "Telemetri"
#define TR_MENUTEMPLATES       "Mallar"
#define TR_MENUSTAT            "Statistik"
#define TR_MENUDEBUG           "Debug"
#define TR_RXNUM               "RxNum"
#define TR_SYNCMENU            "Synk [MENU]"
#define TR_LIMIT               INDENT"Niv\200"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Breddgrad"
#define TR_LONGITUDE           "L\201ngdgrad"
#define TR_GPSCOORD            TR("GPS-Koordinater", "GPS-koordinatsystem")
#define TR_VARIO               TR("Vario", "Variometer")
#define TR_SHUTDOWN            "ST\204NGER AV"
#define TR_BATT_CALIB          "Kalib. Batteri"
#define TR_CURRENT_CALIB       "Kalib. Str\202m"
#define TR_VOLTAGE             INDENT"Sp\201nning"
#define TR_CURRENT             INDENT"Str\202m"
#define TR_SELECT_MODEL        "V\201lj Modell"
#define TR_CREATE_MODEL        "Skapa Modell"
#define TR_COPY_MODEL          "Kopiera Modell"
#define TR_MOVE_MODEL          "Flytta Modell"
#define TR_BACKUP_MODEL        "Modell-backup"
#define TR_DELETE_MODEL        "Ta Bort Modell"
#define TR_RESTORE_MODEL       "\203terst\201ll Modell"
#define TR_SDCARD_ERROR        "SDCARD-fel"
#define TR_NO_SDCARD           "SDCARD saknas"
#define TR_INCOMPATIBLE        "Inkompatibel"
#define TR_WARNING             "VARNING"
#define TR_EEPROMWARN          "EEPROM"
#define TR_THROTTLEWARN        "GAS"
#define TR_ALARMSWARN          "ALARM"
#define TR_SWITCHWARN          "BRYTARE"
#define TR_INVERT_THR          TR("Revers.Gas?","Reversera Gasen?")
#define TR_SPEAKER_VOLUME      INDENT"Volym"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          "Ljusstyrka"
#define TR_CPU_TEMP            "CPU temp.\016>"
#define TR_CPU_CURRENT         "Str\202m\022>"
#define TR_CPU_MAH             "F\202rbrukn."
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "MB temp. \016>"
#define TR_CAPAWARNING         INDENT "L\200g Kapacitet"
#define TR_TEMPWARNING         TR(INDENT "H\202g Temp ",INDENT "H\202g Temperatur")
#define TR_FUNC                "Funk"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Tidl\201ngd"
#define TR_DELAY               "F\202rdr\202j"
#define TR_SD_CARD             "SD-kort"
#define TR_SDHC_CARD           "SD/HC-kort"
#define TR_NO_SOUNDS_ON_SD     "Inga ljud i SD"
#define TR_NO_MODELS_ON_SD     "Ingen modell i SD"
#define TR_NO_BITMAPS_ON_SD    "Bilder saknas p\200 SD"
#define TR_PLAY_FILE           "Spela"
#define TR_DELETE_FILE         "Radera"
#define TR_COPY_FILE           "Kopia"
#define TR_RENAME_FILE         "Byt namn"
#define TR_REMOVED             " raderad"
#define TR_SD_INFO             "Information"
#define TR_SD_FORMAT           "Format"
#define TR_NA                  "N/A"
#define TR_HARDWARE            "H\200rdvara"
#define TR_FORMATTING          "Formaterar..."
#define TR_TEMP_CALIB          "Temp.kalib."
#define TR_TIME                "Tid "
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Typ: "
#define TR_SD_SPEED            "Hastighet:"
#define TR_SD_SECTORS          "Sektorer:"
#define TR_SD_SIZE             "Strl:"
#define TR_TYPE                "Typ "
#define TR_GLOBAL_VARS         "Globala Variabler"
#define TR_OWN                 "Egen"
#define TR_DATE                "Datum"
#define TR_ROTARY_ENCODER      "R.Enks"
#define TR_CHANNELS_MONITOR    "Kanal\205versikt"
#define TR_INTERNALRF          "Intern RF"
#define TR_EXTERNALRF          "Extern RF"
#define TR_FAILSAFE            "Failsafe-l\201ge"
#define TR_FAILSAFESET         "FailsafeInst\201llning"
#define TR_COUNTRYCODE         "Landskod"
#define TR_ANTENNAPROBLEM      CENTER "TX Antenna problem!"
#if defined (PCBTARANIS)
#define TR_MODELIDUSED         "Model ID already used"
#else
#define TR_MODELIDUSED         "ID already used"
#endif
#define TR_VOICELANG           "Voice Language"
#define TR_UNITSSYSTEM         "Units"
