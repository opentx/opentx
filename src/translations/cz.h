/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Martin Hotar <mhotar@gmail.com>
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
#define TR_OFFON               "VYP""ZAP"

#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""INV"

#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014CH\0016CH\0018CH10CH12CH14CH16CH"

#define LEN_VBEEPMODE          TR("\005","\012")
#define TR_VBEEPMODE           TR("Tich\212""Alarm""BezKl""V\207e\0", "Tich\212\0    ""Jen alarm\0""Bez kl\200ves""V\207e\0      ")

#define LEN_VBEEPLEN           "\005"
#define TR_VBEEPLEN            "O====""=O===""==O==""===O=""====O"

#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "Ne REaREb"

#define LEN_VBLMODE            TR("\005", "\007")
#define TR_VBLMODE             TR("Vyp\0 ""Kl\200v.""P\200ky\0""Kl+P\200""Zap\0 ", "Vypnuto""Kl\200vesy""P\200ky\0  ""Kl+P\200ky""Zapnuto")

#define LEN_TRNMODE            "\003"
#define TR_TRNMODE             " X "" +="" :="

#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "CH1CH2CH3CH4"

#define LEN_DATETIME           "\006"
#define TR_DATETIME            "DATUM:""\201AS:  "

#define LEN_VLCD               "\006"
#define TR_VLCD                "NormalOptrex"

#define LEN_COUNTRYCODES       TR("\002", "\010")
#define TR_COUNTRYCODES        TR("US""JP""EU", "Amerika\0""Japonsko""Evropa\0 ")

#define LEN_VTRIMINC           "\007"
#define TR_VTRIMINC            "Expo\0  ""ExJemn\212""Jemn\212\0 ""St\206edn\204""Hrub\212\0 "

#define LEN_RETA123            "\001"

#if defined(PCBGRUVIN9X)
  #if ROTARY_ENCODERS > 2
    #define TR_RETA123         "SVPK123abcd"
  #else
    #define TR_RETA123         "SVPK123ab"
  #endif
#elif defined(PCBTARANIS)
  #define TR_RETA123           "SVPK12LR"
#else
  #define TR_RETA123           "SVPK123"
#endif

#define LEN_VPROTOS            "\006"

#if defined(PXX)
  #define TR_PXX              "PXX\0  "
#elif defined(DSM2) || defined(IRPROTOS)
  #define TR_PXX             "[PXX]\0"
#else
  #define TR_PXX
#endif

#if defined(DSM2)
  #define TR_DSM2            "LP45\0 ""DSM2\0 ""DSMX\0 "
#elif defined(IRPROTOS)
  #define TR_DSM2            "[LP45]""[DSM2]""[DSMX]"
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
#define TR_POSNEG              "POZ""NEG"

#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX             "\010"
#define TR_VMLTPX              "Se\201\204st  ""N\200sobit ""Zam\203nit  "

#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="

#define LEN_VMIXTRIMS          "\004"
#define TR_VMIXTRIMS           "VYP\0""ZAP\0""Sm\203r""V\212\207k""Plyn""K\206id"

#define LEN_VCSWFUNC           "\005"
#define TR_VCSWFUNC            "---\0 ""a{x\0 ""a>x\0 ""a<x\0 ""|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 ""a=b\0 ""a>b\0 ""a<b\0 ""d}x\0 ""|d|}x"

#define LEN_VFSWFUNC           "\013"

#if defined(VARIO)
  #define TR_VVARIO            "Vario\0     "
#else
  #define TR_VVARIO            "[Vario]\0   "
#endif

#if defined(AUDIO)
  #define TR_SOUND             "\221\222Zvuk\0    "
#else
  #define TR_SOUND             "P\204pnout\0   "
#endif

#if defined(PCBTARANIS)
  #define TR_HAPTIC
#elif defined(HAPTIC)
  #define TR_HAPTIC            "Vibrovat\0  "
#else
  #define TR_HAPTIC            "[Vibrovat]\0"
#endif

#if defined(VOICE)
  #if defined(PCBSKY9X)
    #define TR_PLAY_TRACK      "\221\222Zvuk\0    "
  #else
    #define TR_PLAY_TRACK      "\221\222Stopa\0   "
  #endif
  #define TR_PLAY_BOTH         "\221\222P\200r\0     "
  #define TR_PLAY_VALUE        "\221\222TTS\0     "
#else
  #define TR_PLAY_TRACK        "[\221\222Stopa]\0 "
  #define TR_PLAY_BOTH         "[\221\222P\200r]\0   "
  #define TR_PLAY_VALUE        "[\221\222TTS]\0   "
#endif

#define TR_CFN_VOLUME        "Hlasitost\0 "
#define TR_CFN_BG_MUSIC      "BgHudba\0   ""BgHudba ||\0"

#if defined(SDCARD)
  #define TR_SDCLOGS         "Log na SD\0 "
#else
  #define TR_SDCLOGS         "[Logov\200n\204]\0"
#endif

#ifdef GVARS
  #define TR_CFN_ADJUST_GVAR     "Nastav \0   "
#else
  #define TR_CFN_ADJUST_GVAR
#endif

#ifdef DEBUG
  #define TR_CFN_TEST            "Test\0      "
#else
  #define TR_CFN_TEST
#endif

#if defined(CPUARM)
  #define TR_VFSWFUNC          "Z\200mek \0    ""Tren\202r \0   ""Insta-Trim\0" TR_SOUND TR_HAPTIC "Reset\0     " TR_VVARIO TR_PLAY_TRACK TR_PLAY_VALUE TR_SDCLOGS TR_CFN_VOLUME "Podsv\203tlen\204" TR_CFN_BG_MUSIC TR_CFN_ADJUST_GVAR TR_CFN_TEST
#elif defined(PCBGRUVIN9X)
  #define TR_VFSWFUNC          "Z\200mek \0    ""Tren\202r \0   ""Insta-Trim\0" TR_SOUND TR_HAPTIC "Reset\0     " TR_VVARIO TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_SDCLOGS "Podsv\203tlen\204" TR_CFN_ADJUST_GVAR TR_CFN_TEST
#else
  #define TR_VFSWFUNC          "Z\200mek \0    ""Tren\202r \0   ""Insta-Trim\0" TR_SOUND TR_HAPTIC "Reset\0     " TR_VVARIO TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE "Podsv\203tlen\204" TR_CFN_ADJUST_GVAR TR_CFN_TEST
#endif

#define LEN_VFSWRESET           "\005"

#if defined(FRSKY)
  #define TR_FSW_RESET_TELEM    "Telem"
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

#define TR_VFSWRESET            "Tmr1\0""Tmr2\0""V\207e\0 " TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC

#define LEN_FUNCSOUNDS         "\006"
#define TR_FUNCSOUNDS          "Beep1 ""Beep2 ""Beep3 ""Warn1 ""Warn2 ""Cheep ""Ring  ""SciFi ""Robot ""Chirp ""Tada  ""Crickt""Siren ""AlmClk""Ratata""Tick  "

#define LEN_VTELEMCHNS         "\004"
#if defined(PCBTARANIS)
  #define TR_RSSI_0            "SWR\0"
  #define TR_RSSI_1            "RSSI"
#else
  #define TR_RSSI_0            "Tx\0 "
  #define TR_RSSI_1            "Rx\0 "
#endif
#define TR_VTELEMCHNS          "---\0""Bat\0""Tmr1""Tmr2""Tx\0 ""Rx\0 ""A1\0 ""A2\0 ""Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""A1-\0""A2-\0""Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+""Cur+""Acc\0""Time"

#if defined(CPUARM)
  #define LEN_VUNITSSYSTEM     TR("\006", "\010")
  #define TR_VUNITSSYSTEM      TR("Metric""Imper.", "Metric\0 ""Imperial")
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

#define LEN_VALARM             "\004"
#define TR_VALARM              "----""\217lut""oran""\201erv"

#define LEN_VALARMFN           "\001"
#define TR_VALARMFN            "<>"

#define LEN_VTELPROTO          "\007"
#define TR_VTELPROTO           "---\0   ""Hub\0   ""WSHHigh""Halcyon"

#define LEN_VOLTSRC            "\003"
#define TR_VOLTSRC             "---""A1\0""A2\0""FAS""Cel"

#define LEN_VARIOSRC           "\005"
#define TR_VARIOSRC            "Alti\0""Alti+""Vario""A1\0  ""A2\0"

#define LEN_VSCREEN            "\010"
#define TR_VSCREEN             "Hodnota ""Ukazatel"

#define LEN_GPSFORMAT          "\004"
#define TR_GPSFORMAT           "HMS NMEA"

#define LEN2_VTEMPLATES        13
#define LEN_VTEMPLATES         "\015"
#define TR_VTEMPLATES          "Smazat mixy\0\0""Z\200kl. 4kan\200l\0""Sticky-T-Cut\0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE         "\004"
#define TR_VSWASHTYPE          "--- ""120 ""120X""140 ""90\0"

#define LEN_VKEYS              "\005"
#define TR_VKEYS               TR(" Menu"" Exit"" Dol\211""Nhoru""Vprvo""Vlevo", " Menu"" Exit""Enter"" Page"" Plus""M\204nus")

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
  #define TR_CUSTOMSW          "VS1""VS2""VS3""VS4""VS5""VS6""VS7""VS8""VS9""VSA""VSB""VSC""VSD""VSE""VSF""VSG""VSH""VSI""VSJ""VSK""VSL""VSM""VSN""VSO""VSP""VSQ""VSR""VSS""VST""VSU""VSV""VSW"
#elif defined(PCBGRUVIN9X) || defined(CPUM2561) || defined(CPUM128)
  #define TR_CUSTOMSW          "VS1""VS2""VS3""VS4""VS5""VS6""VS7""VS8""VS9""VSA""VSB""VSC""VSD""VSE""VSF"
#else
  #define TR_CUSTOMSW          "VS1""VS2""VS3""VS4""VS5""VS6""VS7""VS8""VS9""VSA""VSB""VSC"
#endif

#if defined(PCBTARANIS)
  #define TR_VSWITCHES         "SA\300""SA-""SA\301""SB\300""SB-""SB\301""SC\300""SC-""SC\301""SD\300""SD-""SD\301""SE\300""SE-""SE\301""SF\300""SF\301""SG\300""SG-""SG\301""SH\300""SH\301" TR_CUSTOMSW " * "
#else
  #define TR_VSWITCHES         TR_9X_3POS_SWITCHES "THR""RUD""ELE""AIL""GEA""TRN" TR_CUSTOMSW " * "
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

#define TR_VSRCRAW             "---\0""Sm\203r""V\212\207k""Plyn""K\206id" TR_POTS_VSRCRAW TR_ROTARY_ENCODERS_VSRCRAW "MAX\0" TR_CYC_VSRCRAW "TrmS" "TrmV" "TrmP" "TrmK" TR_SW_VSRCRAW

#define LEN_VTMRMODES          "\003"
#define TR_VTMRMODES           "VYP""ABS""THs""TH%""THt"

#define LEN_VTRAINERMODES      "\006"
#define TR_VTRAINERMODES       "Master""Slave\0"

#define LEN_VFAILSAFE          "\011"
#define TR_VFAILSAFE           "Hold\0    ""Custom\0  ""No pulses"

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

#define TR_MENUWHENDONE        CENTER "\007"TR_ENTER" > DAL\207\214"
#define TR_FREE                TR("voln\202:", "free")
#define TR_DELETEMODEL         "SMAZAT MODEL"
#define TR_COPYINGMODEL        "Kop\204ruji model.."
#define TR_MOVINGMODEL         "P\206esouv\200m model."
#define TR_LOADINGMODEL        "Aktivuji model.."
#define TR_NAME                "Jm\202no"
#define TR_MODELNAME             "N\200zev modelu"
#define TR_PHASENAME             "Jm\202no"
#define TR_MIXNAME               "Jm\202no"
#define TR_EXPONAME              "Jm\202no"
#define TR_BITMAP                "Obr\200zek"
#define TR_TIMER               "Stopky "
#define TR_ELIMITS             TR("Limit+25%", "Kan\200l +/- 125%")
#define TR_ETRIMS              TR("\207ir\207\204Trim", "\207irok\212 trim")
#define TR_TRIMINC             TR("KrokTrimu", "Krok trimu")
#define TR_TTRACE              TR("StopaPlyn", "Stopa\206 plynu")
#define TR_TTRIM               TR("TrimPlynu", "TrimPlynu-volnob\203h")
#define TR_BEEPCTR             "(\043)St\206edy"
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
#define TR_SWASHTYPE           "Typ cykliky"
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
#define TR_DIFFERENTIAL        "Dif.v\212chylek"
#define TR_OFFSET              INDENT"Ofset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DRex"
#define TR_CURVE               "K\206ivka"
#define TR_FLMODE              "Re\217im"
#define TR_MIXWARNING          "Varov\200n\204"
#define TR_OFF                 "VYP"
#define TR_MULTPX              TR("Mat.operace", "Operace")
#define TR_DELAYDOWN           "Zpo\217d\203n\204 Vyp"
#define TR_DELAYUP             "Zpo\217d\203n\204 Zap"
#define TR_SLOWDOWN            TR("Zpomalen\204(-)", "Zpomal(-)")
#define TR_SLOWUP              TR("Zpomalen\204(+)", "Zpomal(+)")
#define TR_MIXER               "MIXER"
#define TR_CV                  "k"
#define TR_GV                  "GP"
#define TR_ACHANNEL            "A\004Kan\200l"
#define TR_RANGE               INDENT"Rozsah"
#define TR_BAR                 "Ukazatel"
#define TR_ALARM               INDENT"Alarm"
#define TR_USRDATA             "U\217ivData"
#define TR_BLADES              INDENT"ListyVrt"
#define TR_SCREEN              "Panel "
#define TR_SOUND_LABEL         "Zvuk"
#define TR_LENGTH              INDENT"D\202lka"
#define TR_SPKRPITCH           INDENT"T\205n"
#define TR_HAPTIC_LABEL        "Vibrace"
#define TR_HAPTICSTRENGTH      INDENT"S\204la"
#define TR_CONTRAST            "Kontrast LCD"
#define TR_ALARMS_LABEL        "Alarmy"
#define TR_BATTERY_RANGE       TR("Rozsah bat.", "Ukazetel baterie")
#define TR_BATTERYWARNING      INDENT"Vybit\200 baterie"
#define TR_INACTIVITYALARM     INDENT"Ne\201innost"
#define TR_MEMORYWARNING       INDENT"Pln\200 pam\203t'"
#define TR_ALARMWARNING        INDENT"Vypnut\212 zvuk"
#define TR_RENAVIG             "Navig. RotEnc"
#define TR_THROTTLEREVERSE     TR("ReversPlyn", "Revers plynu")
#define TR_MINUTEBEEP          TR(INDENT"Minuta", INDENT"Oznamovat minuty")
#define TR_BEEPCOUNTDOWN       TR(INDENT"Odpo\201et", INDENT"Hlasit\212 odpo\201et")
#define TR_PERSISTENT          INDENT"Trval\202"
#define TR_BACKLIGHT_LABEL     "Podsv\203tlen\204"
#define TR_BLDELAY             INDENT"Zhasnout po"
#define TR_BLONBRIGHTNESS      INDENT"Jas Zap."
#define TR_BLOFFBRIGHTNESS     INDENT"Jas Vyp."
#define TR_SPLASHSCREEN        "\210vodn\204Logo"
#define TR_THROTTLEWARNING     "(\043)Plyn"
#define TR_SWITCHWARNING       "(\043)Sp\204na\201e"
#define TR_TIMEZONE            "\201asov\202 p\200smo"
#define TR_RXCHANNELORD        TR("Po\206ad\204 kan\200l\211", "V\212choz\204 po\206ad\204 kan\200l\211")
#define TR_SLAVE               "Pod\206\204zen\212"
#define TR_MODESRC             " M\205d\006% Zdroj"
#define TR_MULTIPLIER          "N\200sobi\201"
#define TR_CAL                 "Kal."
#define TR_VTRIM               "Trim- +"
#define TR_BG                  "BG:"
#define TR_MENUTOSTART         CENTER "\007" TR_ENTER " = START"
#define TR_SETMIDPOINT         TR(CENTER "\001Nastav p\200ky na st\206ed", CENTER "\002Nastav p\200ky na st\206ed")
#define TR_MOVESTICKSPOTS      TR(CENTER "\004H\212bej p\200kami/poty", "\007H\212bej p\200kami i potenciometry")
#define TR_RXBATT              "Rx Bat.:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              CENTER "NO DATA"
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
#define TR_MENUTORESET         CENTER TR_ENTER" >> Reset"
#define TR_PPM                 "PPM"
#define TR_CH                  "CH"
#define TR_MODEL               "MODEL"
#define TR_FP                  "LR"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "Doch\200z\204 EEPROM"
#define TR_ALERT               "\006(!)POZOR"
#define TR_PRESSANYKEYTOSKIP   CENTER "Kl\200vesa >>> p\206esko\201it"
#define TR_THROTTLENOTIDLE     TR(CENTER "P\200ka plynu je pohnut\200", CENTER "P\200ka plynu nen\204 na nule")
#define TR_ALARMSDISABLED      CENTER "Alarmy jsou zak\200z\200ny"
#define TR_PRESSANYKEY         CENTER "\006Stiskni kl\200vesu"
#define TR_BADEEPROMDATA       CENTER "\006Chyba dat EEprom"
#define TR_EEPROMFORMATTING    CENTER "\004Formatov\200n\204 EEPROM"
#define TR_EEPROMOVERFLOW      CENTER "P\206etekla EEPROM"
#define TR_MENURADIOSETUP      "NASTAVEN\214 R\213DIA"
#define TR_MENUDATEANDTIME     "DATUM A \201AS"
#define TR_MENUTRAINER         "TREN\220R"
#define TR_MENUVERSION         "VERZE"
#define TR_MENUDIAG            "DIAG"
#define TR_MENUANA             "ANALOGY"
#define TR_MENUCALIBRATION     "KALIBRACE"
#define TR_TRIMS2OFFSETS       "\004Trimy => Subtrimy"
#define TR_MENUMODELSEL        "MODEL"
#define TR_MENUSETUP           "NASTAVEN\214"
#define TR_MENUFLIGHTPHASE     "LETOV\216 RE\217IM"
#define TR_MENUFLIGHTPHASES    "LETOV\220 RE\217IMY"
#define TR_MENUHELISETUP       "HELI"


// Alignment


#define TR_MENUDREXPO          "DR/EXPO"
#define TR_MENULIMITS          "LIMITY"


#define TR_MENUCURVES          "K\215IVKY"
#define TR_MENUCURVE           "\002k"
#define TR_MENUCUSTOMSWITCH    "VIRT. SP\214NA\201"
#define TR_MENUCUSTOMSWITCHES  "VIRTU\213LN\214 SP\214NA\201E"
#define TR_MENUCUSTOMFUNC      "FUNKCE"
#define TR_MENUTELEMETRY       "TELEMETRIE"
#define TR_MENUTEMPLATES       "\207ABLONY"
#define TR_MENUSTAT            "STATISTIKA"
#define TR_MENUDEBUG           "DIAG"
#define TR_RXNUM               "RxNum"
#define TR_SYNCMENU            "Sync [MENU]"
#define TR_LIMIT               INDENT"Limit"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Latitude"
#define TR_LONGITUDE           "Longitude"
#define TR_GPSCOORD            "GPS sou\206adnice"
#define TR_VARIO               "Vario"
#define TR_SHUTDOWN            "VYP\214N\213N\204.."
#define TR_BATT_CALIB          "Kalib:Baterie"
#define TR_CURRENT_CALIB       " +=\006Proud"
#define TR_VOLTAGE             INDENT"Nap\203t\204"
#define TR_CURRENT             INDENT"Proud"
#define TR_SELECT_MODEL        "Vyber model"
#define TR_CREATE_MODEL        "Nov\212 model"
#define TR_COPY_MODEL          "Kop\204rovat"
#define TR_MOVE_MODEL          "P\206esunout"
#define TR_BACKUP_MODEL        "Z\200lohovat"
#define TR_DELETE_MODEL        "Sma\217 model"
#define TR_RESTORE_MODEL       "Obnov model"
#define TR_SDCARD_ERROR        "Chyba SD karty"
#define TR_NO_SDCARD           "Nen\204 SD karta"
#define TR_INCOMPATIBLE        "Nekompatibiln\204"
#define TR_WARNING             "KONTROLA"
#define TR_EEPROMWARN          "EEPROM"
#define TR_THROTTLEWARN        "PLYNU"
#define TR_ALARMSWARN          "ALARMU"
#define TR_SWITCHWARN          "POZICE"
#define TR_INVERT_THR          "Invertovat plyn?"
#define TR_SPEAKER_VOLUME      INDENT"Hlasitost"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          "Jas"
#define TR_CPU_TEMP            "Tepl. CPU\016>"
#define TR_CPU_CURRENT         "Proud\030>"
#define TR_CPU_MAH             "Spot\206eba"
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "Tepl. MB \016>"
#define TR_CAPAWARNING         INDENT "Kapacita"
#define TR_TEMPWARNING         INDENT "Teplota"
#define TR_FUNC                "Fce."
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Trv\200n\204"
#define TR_DELAY               "Zdr\217et"
#define TR_SD_CARD             "SD"
#define TR_SDHC_CARD           "SD-HC"
#define TR_NO_SOUNDS_ON_SD     "\217\200dn\212 zvuk na SD"
#define TR_NO_MODELS_ON_SD     "\217\200dn\212 model na SD"
#define TR_NO_BITMAPS_ON_SD    "No Bitmaps on SD"
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
#define TR_TYPE                TR_SD_TYPE
#define TR_GLOBAL_VARS         "Glob\200ln\204 prom\203nn\202"
#define TR_GLOBAL_VAR          "Global Variable"
#define TR_MENUGLOBALVARS      "GLOB\200LN\204 PROM\203NN\202"
#define TR_OWN                 " \043 "
#define TR_DATE                "Datum"
#define TR_ROTARY_ENCODER      "R.Encs"
#define TR_CHANNELS_MONITOR    "MONITOR KAN\213LU"
#define TR_INTERNALRF          "Vnit\206n\204 RF modul"
#define TR_EXTERNALRF          "Extern\204 RF modul"
#define TR_FAILSAFE            "M\205d Failsafe"
#define TR_FAILSAFESET         "NASTAVEN\214 FAILSAFE"
#define TR_COUNTRYCODE         "K\205d regionu"
#define TR_VOICELANG           "Voice Language"
#define TR_UNITSSYSTEM         "Units"
#define TR_EDIT                "Edit"
#define TR_INSERT_BEFORE       "Insert Before"
#define TR_INSERT_AFTER        "Insert After"
#define TR_COPY                "Copy"
#define TR_MOVE                "Move"
#define TR_DELETE              "Delete"
#define TR_RESET_FLIGHT        "Reset Flight"
#define TR_RESET_TIMER1        "Reset Timer1"
#define TR_RESET_TIMER2        "Reset Timer2"
#define TR_RESET_TELEMETRY     "Reset Telemetry"
#define TR_STATISTICS          "Statistics"
#define TR_ABOUT_US            "About Us"
#define TR_AND_SWITCH          "AND Switch"
#define TR_CF                  "CF"
#define TR_SPEAKER             INDENT"Speaker"
#define TR_BUZZER              INDENT"Buzzer"
#define TR_BYTES               "bytes"
#define TR_MODULE_BIND         "[Bind]"
#define TR_MODULE_RANGE        "[Range]"
#define TR_RESET               "[Reset]"
#define TR_SET                 "[Set]"
#define TR_TRAINER             "Trainer"
#define TR_ANTENNAPROBLEM      CENTER "TX Antenna problem!"
#define TR_MODELIDUSED         TR("ID already used","Model ID already used")
#define TR_MODULE              INDENT "Module"
#define TR_CHANNELRANGE        INDENT "Channel Range"
#define TR_LOWALARM            INDENT "Low Alarm"
#define TR_CRITICALALARM       INDENT "Critical Alarm"

// Taranis column headers
#define TR_PHASES_HEADERS      { " Name ", " Switch ", " Trims ", " Fade In ", " Fade Out " }
#define TR_LIMITS_HEADERS      { " Name ", " Offset ", " Min ", " Max ", " Direction ", " PPM Center ", " Symmetrical " }
#define TR_CSW_HEADERS         { " Function ", " V1 ", " V2 ", " AND Switch ", " Duration ", " Delay " }

//Taranis About screen
#define TR_ABOUTUS             "ABOUT US"

#define TR_ABOUT_OPENTX_1      "OpenTX is open source, non-"
#define TR_ABOUT_OPENTX_2      "commercial and comes with no"
#define TR_ABOUT_OPENTX_3      "warranties. It was developed"
#define TR_ABOUT_OPENTX_4      "for free. Support through"
#define TR_ABOUT_OPENTX_5      "donations is welcome!"

#define TR_ABOUT_BERTRAND_1    "Bertrand Songis"
#define TR_ABOUT_BERTRAND_2    "OpenTX main author"
#define TR_ABOUT_BERTRAND_3    "Companion9x co-developer"

#define TR_ABOUT_MIKE_1        "Mike Blandford"
#define TR_ABOUT_MIKE_2        "Code and drivers guru"
#define TR_ABOUT_MIKE_3        "Arguably, one of the best"
#define TR_ABOUT_MIKE_4        "Inspirational"
       
#define TR_ABOUT_ROMOLO_1      "Romolo Manfredini"
#define TR_ABOUT_ROMOLO_2      "Companion9x main developer"
#define TR_ABOUT_ROMOLO_3      ""

#define TR_ABOUT_ANDRE_1       "Andre Bernet"
#define TR_ABOUT_ANDRE_2       "Functionality, usability,"
#define TR_ABOUT_ANDRE_3       "debugging, documentation"

#define TR_ABOUT_ROB_1         "Rob Thomson"
#define TR_ABOUT_ROB_2         "openrcforums webmaster"

#define TR_ABOUT_MARTIN_1      "Martin Hotar"
#define TR_ABOUT_MARTIN_2      "Graphics designer"

#define TR_ABOUT_HARDWARE_1    "FrSky"
#define TR_ABOUT_HARDWARE_2    "Hardware designer/producer"
#define TR_ABOUT_HARDWARE_3    ""

#define TR_ABOUT_PARENTS_1     "Parent projects"
#define TR_ABOUT_PARENTS_2     "ersky9x (Mike Blandford)"
#define TR_ABOUT_PARENTS_3     "ER9X (Erez Raviv)"
#define TR_ABOUT_PARENTS_4     "TH9X (Thomas Husterer)"

#define TR_CHR_SHORT  's'
#define TR_CHR_LONG   'l'
#define TR_CHR_TOGGLE 't'
#define TR_CHR_HOUR   'h'