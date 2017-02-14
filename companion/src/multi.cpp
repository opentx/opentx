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
 */

#include <QObject>

#include "multi.h"
#include "radiodata.h"

#define tr QObject::tr

#define STR_SUBTYPE_CUSTOM  {"Subtype 0", "Subtype 1", "Subtype 2", "Subtype 3", "Subtype 4", "Subtype 5", "Subtype 6", "Subtype 7"}
#define STR_SUBTYPE_FLYSKY  {"Standard", "V9x9", "V6x6", "V912", "CX20"}
#define STR_SUBTYPE_FRSKY   {"D16", "D8", "D16 8ch", "V8", "D16 EU-LBT", "D16 EU-LBT 8ch"}
#define STR_SUBTYPE_HISKY   {"HiSky", "HK310"}
#define STR_SUBTYPE_V2X2    {"V2x2", "JXD506"}
#define STR_SUBTYPE_DSM     {"DSM2 22ms", "DSM2 11ms", "DSMX 22ms", "DSMX 11ms"}
#define STR_SUBTYPE_YD717   {"YD717", "Skywalker", "Syma X2", "XINXUN", "NIHUI"}
#define STR_SUBTYPE_SYMAX   {"Standard", "Syma X5C"}
#define STR_SUBTYPE_SLT     {"SLT", "Vista"}
#define STR_SUBTYPE_CX10    {"Green", "Blue", "DM007", "-", "JC3015a", "JC3015b", "MK33041", "Q242"}
#define STR_SUBTYPE_CG023   {"CG023", "YD829", "H3 3D"}
#define STR_SUBTYPE_BAYANG  {"Bayang", "H8S3D"}
#define STR_SUBTYPE_KN      {"WLtoys", "FeiLun"}
#define STR_SUBTYPE_MT99    {"MT99", "H7", "YZ"}
#define STR_SUBTYPE_MJXQ    {"WLH08", "X600", "X800", "H26D", "E010"}
#define STR_SUBTYPE_FY326   {"FY326", "FY319"}
#define STR_SUBTYPE_HONTAI  {"Standard", "JJRC X1", "X5C1 Clone"}
#define STR_SUBTYPE_AFHDS2A {"PWM and IBUS", "PPM and IBUS", "PWM and SBUS", "PPM and SBUS"}
#define STR_SUBTYPE_Q2X2    {"Q222", "Q242", "Q282"}
#define STR_SUBTYPE_WK2x01  {"WK2801", "WK2401", "W6_5_1", "W6_6_1", "W6_HEL", "W6_HEL_I"}
#define STR_SUBTYPE_Q303    {"Q303", "CX35", "CX10D", "CX10WD"}
#define NO_SUBTYPE          {}

static const QString STR_MULTI_VIDFREQ = tr("Video TX frequency");
static const QString STR_MULTI_RFTUNE = tr("C2500 frequency fine tune");
static const QString STR_MULTI_TELEMETRY = tr("Telemetry");
static const QString STR_MULTI_RFPOWER = tr("Radio output power");
static const QString STR_MULTI_SERVOFREQ = tr("Servo output frequency");
static const QString STR_MULTI_OPTION= tr("Option value");


// Table is designed to be shared with gui_common_arm.cpp

const mm_protocol_definition multi_protocols[] = {
  { MM_RF_PROTO_FLYSKY,     STR_SUBTYPE_FLYSKY,   4,  nullptr             },
  { MM_RF_PROTO_HUBSAN,     NO_SUBTYPE,           0,  STR_MULTI_VIDFREQ   },
  { MM_RF_PROTO_FRSKY,      STR_SUBTYPE_FRSKY,    5,  STR_MULTI_RFTUNE    },
  { MM_RF_PROTO_HISKY,      STR_SUBTYPE_HISKY,    1,  nullptr             },
  { MM_RF_PROTO_V2X2,       STR_SUBTYPE_V2X2,     1,  nullptr             },
  { MM_RF_PROTO_DSM2,       STR_SUBTYPE_DSM,      3,  nullptr             },
  { MM_RF_PROTO_YD717,      STR_SUBTYPE_YD717,    4,  nullptr             },
  { MM_RF_PROTO_KN,         STR_SUBTYPE_KN,       1,  nullptr             },
  { MM_RF_PROTO_SYMAX,      STR_SUBTYPE_SYMAX,    1,  nullptr             },
  { MM_RF_PROTO_SLT,        STR_SUBTYPE_SLT,      1,  nullptr             },
  { MM_RF_PROTO_CX10,       STR_SUBTYPE_CX10,     7,  nullptr             },
  { MM_RF_PROTO_CG023,      STR_SUBTYPE_CG023,    2,  nullptr             },
  { MM_RF_PROTO_BAYANG,     STR_SUBTYPE_BAYANG,   1,  STR_MULTI_TELEMETRY },
  { MM_RF_PROTO_MT99XX,     STR_SUBTYPE_MT99,     4,  nullptr             },
  { MM_RF_PROTO_MJXQ,       STR_SUBTYPE_MJXQ,     5,  nullptr             },
  { MM_RF_PROTO_FY326,      STR_SUBTYPE_FY326,    1,  nullptr             },
  { MM_RF_PROTO_SFHSS,      NO_SUBTYPE,           0,  STR_MULTI_RFTUNE    },
  { MM_RF_PROTO_HONTAI,     STR_SUBTYPE_HONTAI,   3,  nullptr             },
  { MM_RF_PROTO_OLRS,       NO_SUBTYPE,           0,  STR_MULTI_RFPOWER   },
  { MM_RF_PROTO_FS_AFHDS2A, STR_SUBTYPE_AFHDS2A,  3,  STR_MULTI_SERVOFREQ },
  { MM_RF_PROTO_Q2X2,       STR_SUBTYPE_Q2X2,     1,  nullptr             },
  { MM_RF_PROTO_WK_2X01,    STR_SUBTYPE_WK2x01,   5,  nullptr             },
  { MM_RF_PROTO_Q303,       STR_SUBTYPE_Q303,     3,  nullptr             },
  { MM_RF_CUSTOM_SELECTED,  STR_SUBTYPE_CUSTOM,   7,  STR_MULTI_OPTION    },

  //Sential and default for protocols not listed above (MM_RF_CUSTOM is 0xff()
  { 0xfe,                   NO_SUBTYPE,           0,  nullptr             }
};

const std::pair<int,int> getMultiOptionMinMax(const mm_protocol_definition* pdef)
{
  if (pdef->optionsstr == STR_MULTI_RFPOWER)
    return std::make_pair(-1,7);
  else if (pdef->optionsstr == STR_MULTI_SERVOFREQ)
    return std::make_pair(0, 70);
  // Default
  return std::make_pair(-128,127);
}

const mm_protocol_definition getMultiProtocolDefinition (int protocol)
{
  const mm_protocol_definition *pdef;
  for (pdef = multi_protocols; pdef->protocol != 0xfe; pdef++) {
    if (pdef->protocol == protocol) {
      // Stop when protocol found
      break;
    }
  }
  // If no protocol has been found, we are at the last entry

  mm_protocol_definition pdefcopy = *pdef;
  auto minMax = getMultiOptionMinMax(pdef);
  pdefcopy.optionMin = minMax.first;
  pdefcopy.optionMax = minMax.second;
  return pdefcopy;
}
