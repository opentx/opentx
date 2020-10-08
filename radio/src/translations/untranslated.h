/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
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

/*
 * Formatting octal codes available in TR_ strings:
 *  \037\x           -sets LCD x-coord (x value in octal)
 *  \036             -newline
 *  \035             -horizontal tab (ARM only)
 *  \001 to \034     -extended spacing (value * FW/2)
 *  \0               -ends current string
 */


#if defined(PCBX12S)
  #define TR_POTS_VSRCRAW              "\210S1\0""\2106P\0""\210S2\0""\213L1\0""\213L2\0""\211LS\0""\211RS\0""\210JSx""\210JSy"
  #define TR_SW_VSRCRAW                "\212SA\0""\212SB\0""\212SC\0""\212SD\0""\212SE\0""\212SF\0""\212SG\0""\212SH\0""\212SI\0""\212SJ\0"
#elif defined(PCBX10)
  #define TR_POTS_VSRCRAW              "\210S1\0""\2106P\0""\210S2\0""\210EX1""\210EX2""\211LS\0""\211RS\0""\210JSx""\210JSy"
  #define TR_SW_VSRCRAW                "\212SA\0""\212SB\0""\212SC\0""\212SD\0""\212SE\0""\212SF\0""\212SG\0""\212SH\0""\212SI\0""\212SJ\0"
#elif defined(PCBX9E)
  #define TR_POTS_VSRCRAW              "\310F1\0""\310F2\0""\310F3\0""\310F4\0""\311S1\0""\311S2\0""\311LS\0""\311RS\0"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SE\0""\312SF\0""\312SG\0""\312SH\0""\312SI\0""\312SJ\0""\312SK\0""\312SL\0""\312SM\0""\312SN\0""\312SO\0""\312SP\0""\312SQ\0""\312SR\0"
#elif defined(PCBXLITE)
  #define TR_POTS_VSRCRAW              "\310S1\0""\310S2\0"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SE\0""\312SF\0"
#elif defined(RADIO_T12)
  #define TR_POTS_VSRCRAW              "\210S1\0""\210S2\0"
  #define TR_SW_VSRCRAW                "\212SA\0""\212SB\0""\212SC\0""\212SD\0""\212SG\0""\212SH\0""\212SI\0""\212SJ\0"
#elif defined(PCBX7)
  #define TR_POTS_VSRCRAW              "\310S1\0""\310S2\0"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SF\0""\312SH\0""\312SI\0""\312SJ\0"
#elif defined(PCBX9LITES)
  #define TR_POTS_VSRCRAW              "\310S1\0"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SE\0""\312SF\0""\312SG\0"
#elif defined(PCBX9LITE)
  #define TR_POTS_VSRCRAW              "\310S1\0"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SE\0"
#elif defined(PCBTARANIS)
  #define TR_POTS_VSRCRAW              "\310S1\0""\310S2\0""\310S3\0""\311LS\0""\311RS\0"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SE\0""\312SF\0""\312SG\0""\312SH\0""\312SI\0"
#elif defined(PCBSKY9X)
  #define TR_POTS_VSRCRAW              "P1\0 ""P2\0 ""P3\0 "
  #define TR_SW_VSRCRAW                "3POS" "THR\0""RUD\0""ELE\0""AIL\0""GEA\0""TRN\0"
#elif defined(PCBNV14)
  #define TR_POTS_VSRCRAW              "\210S1\0""\210S2\0"
  #define TR_SW_VSRCRAW                "\212SA\0""\212SB\0""\212SC\0""\212SD\0""\212SE\0""\212SF\0""\212SG\0""\212SH\0"
#endif

#if defined(PCBFRSKY) || defined(PCBFLYSKY)
  // only special switches here
  #define TR_VSWITCHES                 "---" TR_TRIMS_SWITCHES TR_ON_ONE_SWITCHES
#elif defined(PCBSKY9X)
  #define TR_VSWITCHES                 "---" "ID0""ID1""ID2" "THR""RUD""ELE""AIL""GEA""TRN" TR_TRIMS_SWITCHES TR_ROTENC_SWITCHES TR_ON_ONE_SWITCHES
#endif

#if defined(PCBNV14)
  #define TR_VTRAINERMODES             TR_VTRAINER_MASTER_OFF TR_VTRAINER_MASTER_JACK TR_VTRAINER_SLAVE_JACK TR_VTRAINER_BLUETOOTH TR_VTRAINER_MULTI
#elif defined(PCBHORUS) && defined(AUX_SERIAL)
  #define TR_VTRAINERMODES             TR_VTRAINER_MASTER_JACK TR_VTRAINER_SLAVE_JACK TR_VTRAINER_MASTER_BATTERY TR_VTRAINER_BLUETOOTH TR_VTRAINER_MULTI
#elif defined(PCBHORUS)
  #define TR_VTRAINERMODES             TR_VTRAINER_MASTER_JACK TR_VTRAINER_SLAVE_JACK TR_VTRAINER_BLUETOOTH TR_VTRAINER_MULTI
#elif defined(PCBTARANIS)
  #define TR_VTRAINERMODES             TR_VTRAINER_MASTER_JACK TR_VTRAINER_SLAVE_JACK TR_VTRAINER_MASTER_SBUS_MODULE TR_VTRAINER_MASTER_CPPM_MODULE TR_VTRAINER_MASTER_BATTERY TR_VTRAINER_BLUETOOTH TR_VTRAINER_MULTI
#else
  #define TR_VTRAINERMODES             TR_VTRAINER_MASTER_JACK TR_VTRAINER_SLAVE_JACK TR_VTRAINER_MASTER_CPPM_MODULE TR_VTRAINER_MASTER_BATTERY TR_VTRAINER_BLUETOOTH TR_VTRAINER_MULTI
#endif

#define TR_VSRCRAW                     "---\0" TR_STICKS_VSRCRAW TR_POTS_VSRCRAW TR_GYR_VSRCRAW "MAX\0" TR_CYC_VSRCRAW TR_TRIMS_VSRCRAW TR_SW_VSRCRAW TR_EXTRA_VSRCRAW

#define LEN_CRSF_BAUDRATE              "\004"
#define TR_CRSF_BAUDRATE               "115k""400k"

#if defined(MODULE_PROTOCOL_FLEX)
#define TR_MODULE_R9M_LITE "R9ML\0       "
#else
#define TR_MODULE_R9M_LITE "R9MLite\0    "
#endif

#define LEN_EXTERNAL_MODULE_PROTOCOLS  "\014"
#define TR_EXTERNAL_MODULE_PROTOCOLS   "OFF\0        ""PPM\0        ""XJT\0        ""ISRM\0       ""DSM2\0       ""CRSF\0       ""MULTI\0      ""R9M\0        ""R9M ACCESS\0 " TR_MODULE_R9M_LITE "R9ML ACCESS\0""GHST\0       ""R9MLP ACCESS""SBUS\0       ""XJT Lite\0   ""AFHDS3\0  "

#define LEN_FLYSKY_PROTOCOLS           "\006"
#define TR_FLYSKY_PROTOCOLS

#define LEN_INTERNAL_MODULE_PROTOCOLS  LEN_EXTERNAL_MODULE_PROTOCOLS
#define TR_INTERNAL_MODULE_PROTOCOLS   TR_EXTERNAL_MODULE_PROTOCOLS

#define LEN_XJT_ACCST_RF_PROTOCOLS         "\004"
#define TR_XJT_ACCST_RF_PROTOCOLS          "OFF\0""D16\0""D8\0 ""LR12"

#if defined(INTERNAL_MODULE_PXX1) || defined(PCBHORUS)
#define LEN_ISRM_RF_PROTOCOLS     "\006"
#define TR_ISRM_RF_PROTOCOLS      "ACCESS""D16\0  ""LR12"
#else
#define LEN_ISRM_RF_PROTOCOLS     "\012"
#define TR_ISRM_RF_PROTOCOLS      "OFF\0      ""ACCESS\0   ""ACCST D16\0""ACCST LR12""ACCST D8"
#endif

#define LEN_R9M_PXX2_RF_PROTOCOLS      "\006"
#define TR_R9M_PXX2_RF_PROTOCOLS       "ACCESS""FCC\0  ""EU\0   ""Flex"

#define LEN_R9M_REGION                 "\006"
#define TR_R9M_REGION                  "FCC\0  ""EU\0   ""868MHz""915MHz"

#define LEN_R9M_LITE_FCC_POWER_VALUES  "\010"
#define TR_R9M_LITE_FCC_POWER_VALUES   "(100mW)"

#define LEN_R9M_LITE_LBT_POWER_VALUES  "\014"
#define TR_R9M_LITE_LBT_POWER_VALUES   "25mW 8CH\0   ""25mW 16CH\0  ""100mW NoTele"

#define LEN_R9M_FCC_POWER_VALUES       "\011"
#define TR_R9M_FCC_POWER_VALUES        "10mW\0    " "100mW\0   " "500mW\0   " "1W (auto)"

#define LEN_R9M_LBT_POWER_VALUES       "\014"
#define TR_R9M_LBT_POWER_VALUES        "25mW 8CH\0   ""25mW 16CH\0  ""200mW NoTele""500mW NoTele"

#define LEN_DSM_PROTOCOLS              "\004"
#define TR_DSM_PROTOCOLS               "LP45""DSM2""DSMX"

#define LEN_MULTI_PROTOCOLS            "\007"
#define TR_MULTI_PROTOCOLS             "FlySky\0""Hubsan\0""FrSky\0 ""Hisky\0 ""V2x2\0  ""DSM\0   ""Devo\0  ""YD717\0 ""KN\0    ""SymaX\0 ""SLT\0   ""CX10\0  ""CG023\0 ""Bayang\0""ESky\0  ""MT99XX\0""MJXq\0  ""Shenqi\0""FY326\0 ""SFHSS\0 ""J6 Pro\0""FQ777\0 ""Assan\0 ""Hontai\0""OpenLrs""FlSky2A""Q2x2\0  ""Walkera""Q303\0  ""GW008\0 ""DM002\0 ""Cabell\0""Esky150""H8 3D\0 ""Corona\0""CFlie\0 ""Hitec\0 ""WFly\0  ""Bugs\0  ""BugMini""Traxxas""NCC1701""E01X\0  ""V911S\0 ""GD00X\0 ""V761\0  ""KF606\0 ""Redpine""Potensi""ZSX\0   ""FlyZone""Scanner""FrSkyRX""FS2A_RX""HoTT\0  ""FX816\0 ""BayanRX""Pelikan""Tiger\0 ""XK\0    ""XN297DU""FrSkyX2""FrSkyR9""Propel\0""FrSkyL\0""Skyarte""ESky-v2""DSM RX\0""JJRC345""Q90C\0  "

#define LEN_MULTI_POWER                "\005"
#define TR_MULTI_POWER                 "10mW\0""25mW\0""50mW\0""100mW""200mW""300mW""500mW""1W\0  ""2W\0  "

#define LEN_AFHDS3_PROTOCOLS           "\x008"
#define TR_AFHDS3_PROTOCOLS            "PWM/IBUS""PWM/SBUS""PPM/IBUS""PPM/SBUS"

#define LEN_AFHDS3_POWERS              "\006"
#define TR_AFHDS3_POWERS               "25 mW\0""100 mW""500 mW""1 W\0  ""2 W\0  "

#define LEN_PPM_POL                    "\001"
#define TR_PPM_POL                     "+""-"

#define LEN_SBUS_INVERSION_VALUES      "\014"
#define TR_SBUS_INVERSION_VALUES       "normal\0     ""not inverted"

#define TR_SENSOR_RSSI                      "RSSI"
#define TR_SENSOR_R9PW                      "R9PW"
#define TR_SENSOR_RAS                       "SWR"
#define TR_SENSOR_A1                        "A1"
#define TR_SENSOR_A2                        "A2"
#define TR_SENSOR_A3                        "A3"
#define TR_SENSOR_A4                        "A4"
#define TR_SENSOR_BATT                      "RxBt"
#define TR_SENSOR_ALT                       "Alt"
#define TR_SENSOR_TEMP1                     "Tmp1"
#define TR_SENSOR_TEMP2                     "Tmp2"
#define TR_SENSOR_TEMP3                     "Tmp3"
#define TR_SENSOR_TEMP4                     "Tmp4"
#define TR_SENSOR_RPM2                      "RPM2"
#define TR_SENSOR_PRES                      "Pres"
#define TR_SENSOR_ODO1                      "Odo1"
#define TR_SENSOR_ODO2                      "Odo2"
#define TR_SENSOR_TXV                       "TX_V"
#define TR_SENSOR_CURR_SERVO1               "CSv1"
#define TR_SENSOR_CURR_SERVO2               "CSv2"
#define TR_SENSOR_CURR_SERVO3               "CSv3"
#define TR_SENSOR_CURR_SERVO4               "CSv4"
#define TR_SENSOR_DIST                      "Dist"
#define TR_SENSOR_ARM                       "Arm"
#define TR_SENSOR_C50                       "C50"
#define TR_SENSOR_C200                      "C200"
#define TR_SENSOR_RPM                       "RPM"
#define TR_SENSOR_FUEL                      "Fuel"
#define TR_SENSOR_VSPD                      "VSpd"
#define TR_SENSOR_ACCX                      "AccX"
#define TR_SENSOR_ACCY                      "AccY"
#define TR_SENSOR_ACCZ                      "AccZ"
#define TR_SENSOR_GYROX                     "GYRX"
#define TR_SENSOR_GYROY                     "GYRY"
#define TR_SENSOR_GYROZ                     "GYRZ"
#define TR_SENSOR_CURR                      "Curr"
#define TR_SENSOR_CAPACITY                  "Capa"
#define TR_SENSOR_VFAS                      "VFAS"
#define TR_SENSOR_ASPD                      "ASpd"
#define TR_SENSOR_GSPD                      "GSpd"
#define TR_SENSOR_HDG                       "Hdg"
#define TR_SENSOR_SATELLITES                "Sats"
#define TR_SENSOR_CELLS                     "Cels"
#define TR_SENSOR_GPSALT                    "GAlt"
#define TR_SENSOR_GPSDATETIME               "Date"
#define TR_SENSOR_GPS                       "GPS"
#define TR_SENSOR_BATT1_VOLTAGE             "RB1V"
#define TR_SENSOR_BATT2_VOLTAGE             "RB2V"
#define TR_SENSOR_BATT1_CURRENT             "RB1A"
#define TR_SENSOR_BATT2_CURRENT             "RB2A"
#define TR_SENSOR_BATT1_CONSUMPTION         "RB1C"
#define TR_SENSOR_BATT2_CONSUMPTION         "RB2C"
#define TR_SENSOR_BATT1_TEMP                "RB1T"
#define TR_SENSOR_BATT2_TEMP                "RB2T"
#define TR_SENSOR_RB_STATE                  "RBS"
#define TR_SENSOR_CHANS_STATE               "RBCS"
#define TR_SENSOR_RX_RSSI1                  "1RSS"
#define TR_SENSOR_RX_RSSI2                  "2RSS"
#define TR_SENSOR_RX_QUALITY                "RQly"
#define TR_SENSOR_RX_SNR                    "RSNR"
#define TR_SENSOR_RX_NOISE                  "RNse"
#define TR_SENSOR_ANTENNA                   "ANT"
#define TR_SENSOR_RF_MODE                   "RFMD"
#define TR_SENSOR_TX_POWER                  "TPWR"
#define TR_SENSOR_TX_RSSI                   "TRSS"
#define TR_SENSOR_TX_QUALITY                "TQly"
#define TR_SENSOR_TX_SNR                    "TSNR"
#define TR_SENSOR_TX_NOISE                  "TNse"
#define TR_SENSOR_PITCH                     "Ptch"
#define TR_SENSOR_ROLL                      "Roll"
#define TR_SENSOR_YAW                       "Yaw"
#define TR_SENSOR_FLIGHT_MODE               "FM"
#define TR_SENSOR_THROTTLE                  "Thr"
#define TR_SENSOR_QOS_A                     "FdeA"
#define TR_SENSOR_QOS_B                     "FdeB"
#define TR_SENSOR_QOS_L                     "FdeL"
#define TR_SENSOR_QOS_R                     "FdeR"
#define TR_SENSOR_QOS_F                     "FLss"
#define TR_SENSOR_QOS_H                     "Hold"
#define TR_SENSOR_BIND                      "BIND"
#define TR_SENSOR_LAP_NUMBER                "Lap "
#define TR_SENSOR_GATE_NUMBER               "Gate"
#define TR_SENSOR_LAP_TIME                  "LapT"
#define TR_SENSOR_GATE_TIME                 "GteT"
#define TR_SENSOR_ESC_VOLTAGE               "EscV"
#define TR_SENSOR_ESC_CURRENT               "EscA"
#define TR_SENSOR_ESC_RPM                   "EscR"
#define TR_SENSOR_ESC_CONSUMPTION           "EscC"
#define TR_SENSOR_ESC_TEMP                  "EscT"
#define TR_SENSOR_SD1_CHANNEL               "Chan"
#define TR_SENSOR_GASSUIT_TEMP1             "GTp1"
#define TR_SENSOR_GASSUIT_TEMP2             "GTp2"
#define TR_SENSOR_GASSUIT_RPM               "GRPM"
#define TR_SENSOR_GASSUIT_FLOW              "GFlo"
#define TR_SENSOR_GASSUIT_CONS              "GFue"
#define TR_SENSOR_GASSUIT_RES_VOL           "GRVl"
#define TR_SENSOR_GASSUIT_RES_PERC          "GRPc"
#define TR_SENSOR_GASSUIT_MAX_FLOW          "GMFl"
#define TR_SENSOR_GASSUIT_AVG_FLOW          "GAFl"
#define TR_SENSOR_SBEC_VOLTAGE              "BecV"
#define TR_SENSOR_SBEC_CURRENT              "BecA"

#define CHAR_UP        '\200'
#define CHAR_DOWN      '\201'
#define CHAR_STICK     '\207'
#define CHAR_POT       '\210'
#define CHAR_GVAR      '\211'
#define CHAR_SWITCH    '\212'
#define CHAR_TRIM      '\213'
#define CHAR_INPUT     '\214'
#define CHAR_FUNCTION  '\215'
#define CHAR_CYC       '\216'
#define CHAR_TRAINER   '\217'
#define CHAR_CHANNEL   '\220'
#define CHAR_TELEMETRY '\221'
#define CHAR_LUA       '\222'
