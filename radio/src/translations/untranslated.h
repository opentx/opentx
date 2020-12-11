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
  #define TR_POTS_VSRCRAW              "\310S1\0""\3106P\0""\310S2\0""\313L1\0""\313L2\0""\311LS\0""\311RS\0""\310JSx""\310JSy"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SE\0""\312SF\0""\312SG\0""\312SH\0""\312SI\0""\312SJ\0"
#elif defined(PCBX10)
  #define TR_POTS_VSRCRAW              "\310S1\0""\3106P\0""\310S2\0""\310EX1""\310EX2""\311LS\0""\311RS\0""\310JSx""\310JSy"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SE\0""\312SF\0""\312SG\0""\312SH\0""\312SI\0""\312SJ\0"
#elif defined(PCBX9E)
  #define TR_POTS_VSRCRAW              "\310F1\0""\310F2\0""\310F3\0""\310F4\0""\311S1\0""\311S2\0""\311LS\0""\311RS\0"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SE\0""\312SF\0""\312SG\0""\312SH\0""\312SI\0""\312SJ\0""\312SK\0""\312SL\0""\312SM\0""\312SN\0""\312SO\0""\312SP\0""\312SQ\0""\312SR\0"
#elif defined(PCBXLITE)
  #define TR_POTS_VSRCRAW              "\310S1\0""\310S2\0"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SE\0""\312SF\0"
#elif defined(RADIO_T12)
  #define TR_POTS_VSRCRAW              "\310S1\0""\310S2\0"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SG\0""\312SH\0""\312SI\0""\312SJ\0"
#elif defined(RADIO_TX12)
  #define TR_POTS_VSRCRAW              "\310S1\0""\310S2\0"
  #define TR_SW_VSRCRAW                "\312SA\0""\312SB\0""\312SC\0""\312SD\0""\312SE\0""\312SF\0""\312SI\0""\312SJ\0"
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
#endif

#if defined(PCBTARANIS) || defined(PCBHORUS)
  // only special switches here
  #define TR_VSWITCHES                 "---" TR_TRIMS_SWITCHES TR_ON_ONE_SWITCHES
#elif defined(PCBSKY9X)
  #define TR_VSWITCHES                 "---" "ID0""ID1""ID2" "THR""RUD""ELE""AIL""GEA""TRN" TR_TRIMS_SWITCHES TR_ROTENC_SWITCHES TR_ON_ONE_SWITCHES
#endif

#if defined(PCBHORUS) && defined(AUX_SERIAL)
  #define TR_VTRAINERMODES             TR_VTRAINER_MASTER_JACK TR_VTRAINER_SLAVE_JACK TR_VTRAINER_MASTER_BATTERY TR_VTRAINER_BLUETOOTH TR_VTRAINER_MULTI
#elif defined(PCBHORUS)
  #define TR_VTRAINERMODES             TR_VTRAINER_MASTER_JACK TR_VTRAINER_SLAVE_JACK TR_VTRAINER_BLUETOOTH TR_VTRAINER_MULTI
#elif defined(PCBTARANIS)
  #define TR_VTRAINERMODES             TR_VTRAINER_MASTER_JACK TR_VTRAINER_SLAVE_JACK TR_VTRAINER_MASTER_SBUS_MODULE TR_VTRAINER_MASTER_CPPM_MODULE TR_VTRAINER_MASTER_BATTERY TR_VTRAINER_BLUETOOTH TR_VTRAINER_MULTI
#else
  #define TR_VTRAINERMODES             TR_VTRAINER_MASTER_JACK TR_VTRAINER_SLAVE_JACK TR_VTRAINER_MASTER_CPPM_MODULE TR_VTRAINER_MASTER_BATTERY TR_VTRAINER_BLUETOOTH TR_VTRAINER_MULTI
#endif

#define TR_VSRCRAW                     "---\0" TR_STICKS_VSRCRAW TR_POTS_VSRCRAW TR_GYR_VSRCRAW "MAX\0" TR_CYC_VSRCRAW TR_TRIMS_VSRCRAW TR_SW_VSRCRAW TR_EXTRA_VSRCRAW

#if defined(MODULE_PROTOCOL_FLEX)
#define TR_MODULE_R9M_LITE "R9ML\0       "
#else
#define TR_MODULE_R9M_LITE "R9MLite\0    "
#endif

#define LEN_EXTERNAL_MODULE_PROTOCOLS  "\014"
#define TR_EXTERNAL_MODULE_PROTOCOLS   "OFF\0        ""PPM\0        ""XJT\0        ""ISRM\0       ""DSM2\0       ""CRSF\0       ""MULTI\0      ""R9M\0        ""R9M ACCESS\0 " TR_MODULE_R9M_LITE "R9ML ACCESS\0""GHST\0       ""R9MLP ACCESS""SBUS\0       ""XJT Lite\0   ""AFHDS3\0  "

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
#define TR_MULTI_PROTOCOLS             "FlySky\0""Hubsan\0""FrSky\0 ""Hisky\0 ""V2x2\0  ""DSM\0   ""Devo\0  ""YD717\0 ""KN\0    ""SymaX\0 ""SLT\0   ""CX10\0  ""CG023\0 ""Bayang\0""ESky\0  ""MT99XX\0""MJXq\0  ""Shenqi\0""FY326\0 ""Futaba\0""J6 Pro\0""FQ777\0 ""Assan\0 ""Hontai\0""OpenLrs""FlSky2A""Q2x2\0  ""Walkera""Q303\0  ""GW008\0 ""DM002\0 ""Cabell\0""Esky150""H8 3D\0 ""Corona\0""CFlie\0 ""Hitec\0 ""WFly\0  ""Bugs\0  ""BugMini""Traxxas""NCC1701""E01X\0  ""V911S\0 ""GD00X\0 ""V761\0  ""KF606\0 ""Redpine""Potensi""ZSX\0   ""Height\0""Scanner""FrSkyRX""FS2A_RX""HoTT\0  ""FX816\0 ""BayanRX""Pelikan""Tiger\0 ""XK\0    ""XN297DU""FrSkyX2""FrSkyR9""Propel\0""FrSkyL\0""Skyartc""ESky-v2""DSM RX\0""JJRC345""Q90C\0  ""Kyosho\0""RadLink""ExpLRS\0""Realacc""OMP\0   ""M-Link\0""Wfly-RF"

#define LEN_MULTI_POWER                "\005"
#define TR_MULTI_POWER                 "10mW\0""25mW\0""50mW\0""100mW""200mW""300mW""500mW""1W\0  ""2W\0  "

#define LEN_AFHDS3_PROTOCOLS           "\x008"
#define TR_AFHDS3_PROTOCOLS            "PWM/IBUS""PWM/SBUS""PPM/IBUS""PPM/SBUS"

#define LEN_AFHDS3_POWERS              "\006"
#define TR_AFHDS3_POWERS               "25 mW\0""100 mW""500 mW""1 W\0  ""2 W\0  "
