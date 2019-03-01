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

#include "multiprotocols.h"
#include "radiodata.h"

#define STR_MULTI_SUBTYPE                    QT_TRANSLATE_NOOP("Multiprotocols", "Subtype")
#define STR_MULTI_VIDFREQ                    QT_TRANSLATE_NOOP("Multiprotocols", "Video TX frequency")
#define STR_MULTI_RFTUNE                     QT_TRANSLATE_NOOP("Multiprotocols", "CC2500 frequency fine tune")
#define STR_MULTI_TELEMETRY                  QT_TRANSLATE_NOOP("Multiprotocols", "Telemetry")
#define STR_MULTI_RFPOWER                    QT_TRANSLATE_NOOP("Multiprotocols", "Radio output power")
#define STR_MULTI_SERVOFREQ                  QT_TRANSLATE_NOOP("Multiprotocols", "Servo output frequency")
#define STR_MULTI_OPTION                     QT_TRANSLATE_NOOP("Multiprotocols", "Option value")
#define STR_MULTI_DEFAULT                    QT_TRANSLATE_NOOP("Multiprotocols", "DEFAULT")

static const QStringList STR_SUBTYPE_CUSTOM  ({
                                                STR_MULTI_SUBTYPE " 0", STR_MULTI_SUBTYPE " 1", STR_MULTI_SUBTYPE " 2", STR_MULTI_SUBTYPE " 3",
                                                STR_MULTI_SUBTYPE " 4", STR_MULTI_SUBTYPE " 5", STR_MULTI_SUBTYPE " 6", STR_MULTI_SUBTYPE " 7"
                                              });
static const QStringList STR_SUBTYPE_FLYSKY  {"Standard", "V9x9", "V6x6", "V912", "CX20"};
static const QStringList STR_SUBTYPE_HUBSAN  {"H107", "H301", "H501"};
static const QStringList STR_SUBTYPE_FRSKY   {"D16", "D8", "D16 8ch", "V8", "D16 EU-LBT", "D16 EU-LBT 8ch"};
static const QStringList STR_SUBTYPE_HISKY   {"Standard", "HK310"};
static const QStringList STR_SUBTYPE_V2X2    {"Standard", "JXD506"};
static const QStringList STR_SUBTYPE_DSM     {"DSM2 22ms", "DSM2 11ms", "DSMX 22ms", "DSMX 11ms"};
static const QStringList STR_SUBTYPE_YD717   {"Standard", "Skywalker", "Syma X4", "XINXUN", "NIHUI"};
static const QStringList STR_SUBTYPE_KN      {"WLtoys", "FeiLun"};
static const QStringList STR_SUBTYPE_SYMAX   {"Standard", "Syma X5C"};
static const QStringList STR_SUBTYPE_SLT     {"V1 (6 Channel)", "V2 (8 Channel)", "Q100", "Q200", "MR100"};
static const QStringList STR_SUBTYPE_CX10    {"Green", "Blue", "DM007", "-", "JC3015a", "JC3015b", "MK33041"};
static const QStringList STR_SUBTYPE_CG023   {"Standard", "YD829"};
static const QStringList STR_SUBTYPE_BAYANG  {"Standard", "H8S3D", "X16 AH", "IRDRONE"};
static const QStringList STR_SUBTYPE_MT99    {"MT99", "H7", "YZ", "LS", "FY805"};
static const QStringList STR_SUBTYPE_MJXQ    {"WLH08", "X600", "X800", "H26D", "E010", "H26WH"};
static const QStringList STR_SUBTYPE_FY326   {"Standard", "FY319"};
static const QStringList STR_SUBTYPE_HONTAI  {"Standard", "JJRC X1", "X5C1 Clone", "FQ777_951"};
static const QStringList STR_SUBTYPE_AFHDS2A {"PWM and IBUS", "PPM and IBUS", "PWM and SBUS", "PPM and SBUS"};
static const QStringList STR_SUBTYPE_Q2X2    {"Q222", "Q242", "Q282"};
static const QStringList STR_SUBTYPE_WK2x01  {"WK2801", "WK2401", "W6_5_1", "W6_6_1", "W6_HEL", "W6_HEL_I"};
static const QStringList STR_SUBTYPE_Q303    {"Standard", "CX35", "CX10D", "CX10WD"};
static const QStringList STR_SUBTYPE_CABELL  {"Cabell V3", "Cab V3 Telem", "-", "-", "-", "-", "Set FailSafe", "Unbind"};
static const QStringList STR_SUBTYPE_H83D    {"H8 Mini 3D", "H20H", "H20 Mini", "H30 Mini"};
static const QStringList STR_SUBTYPE_CORONA  {"V1", "V2", "Flydream V3"};
static const QStringList STR_SUBTYPE_HITEC 	 {"Optima", "Optima Hub Telem", "Minima"};
static const QStringList STR_SUBTYPE_E01X    {"E012", "E015"};

static const QStringList NO_SUBTYPE          {STR_MULTI_DEFAULT};


// Table is designed to be shared with gui_common.cpp

const Multiprotocols multiProtocols {
  {MM_RF_PROTO_FLYSKY,     4, false,      STR_SUBTYPE_FLYSKY,  nullptr},
  {MM_RF_PROTO_HUBSAN,     2, false,      STR_SUBTYPE_HUBSAN,  STR_MULTI_VIDFREQ},
  {MM_RF_PROTO_FRSKY,      5, false,      STR_SUBTYPE_FRSKY,   STR_MULTI_RFTUNE},
  {MM_RF_PROTO_HISKY,      1, false,      STR_SUBTYPE_HISKY,   nullptr},
  {MM_RF_PROTO_V2X2,       1, false,      STR_SUBTYPE_V2X2,    nullptr},
  {MM_RF_PROTO_DSM2,       3, false,      STR_SUBTYPE_DSM,     nullptr},
  {MM_RF_PROTO_YD717,      4, false,      STR_SUBTYPE_YD717,   nullptr},
  {MM_RF_PROTO_KN,         1, false,      STR_SUBTYPE_KN,      nullptr},
  {MM_RF_PROTO_SYMAX,      1, false,      STR_SUBTYPE_SYMAX,   nullptr},
  {MM_RF_PROTO_SLT,        4, false,      STR_SUBTYPE_SLT,     nullptr},
  {MM_RF_PROTO_CX10,       6, false,      STR_SUBTYPE_CX10,    nullptr},
  {MM_RF_PROTO_CG023,      1, false,      STR_SUBTYPE_CG023,   nullptr},
  {MM_RF_PROTO_BAYANG,     3, false,      STR_SUBTYPE_BAYANG,  STR_MULTI_TELEMETRY},
  {MM_RF_PROTO_MT99XX,     4, false,      STR_SUBTYPE_MT99,    nullptr},
  {MM_RF_PROTO_MJXQ,       5, false,      STR_SUBTYPE_MJXQ,    nullptr},
  {MM_RF_PROTO_FY326,      1, false,      STR_SUBTYPE_FY326,   nullptr},
  {MM_RF_PROTO_SFHSS,      0, true,       NO_SUBTYPE,          STR_MULTI_RFTUNE},
  {MM_RF_PROTO_HONTAI,     3, false,      STR_SUBTYPE_HONTAI,  nullptr},
  {MM_RF_PROTO_OLRS,       0, false,      NO_SUBTYPE,          STR_MULTI_RFPOWER},
  {MM_RF_PROTO_FS_AFHDS2A, 3, true,       STR_SUBTYPE_AFHDS2A, STR_MULTI_SERVOFREQ},
  {MM_RF_PROTO_Q2X2,       2, false,      STR_SUBTYPE_Q2X2,    nullptr},
  {MM_RF_PROTO_WK_2X01,    5, false,      STR_SUBTYPE_WK2x01,  nullptr},
  {MM_RF_PROTO_Q303,       3, false,      STR_SUBTYPE_Q303,    nullptr},
  {MM_RF_PROTO_CABELL,     7, false,      STR_SUBTYPE_CABELL,  STR_MULTI_OPTION},
  {MM_RF_PROTO_H83D,       3, false,      STR_SUBTYPE_H83D,    nullptr},
  {MM_RF_PROTO_CORONA,     2, false,      STR_SUBTYPE_CORONA,  STR_MULTI_RFTUNE},
  {MM_RF_PROTO_HITEC,      2, false,      STR_SUBTYPE_HITEC,   STR_MULTI_RFTUNE},
  {MM_RF_PROTO_E01X,       1, false,      STR_SUBTYPE_E01X,    nullptr},
  {MM_RF_CUSTOM_SELECTED,  7, true,       STR_SUBTYPE_CUSTOM,  STR_MULTI_OPTION},

  // Sentinel and default for protocols not listed above (MM_RF_CUSTOM is 0xff)
  { 0xfe,                  0, false,      NO_SUBTYPE,           nullptr}
};

int Multiprotocols::MultiProtocolDefinition::getOptionMin() const {
  if (optionsstr == STR_MULTI_RFPOWER)
    return -1;
  else if (optionsstr == STR_MULTI_SERVOFREQ)
    return 0;
  else
    return -128;
}

int Multiprotocols::MultiProtocolDefinition::getOptionMax() const {
  if (optionsstr == STR_MULTI_RFPOWER)
    return 7;
  else if (optionsstr == STR_MULTI_SERVOFREQ)
    return 70;
  else
    return 127;
}

const Multiprotocols::MultiProtocolDefinition & Multiprotocols::getProtocol(int protocol) const {
  for (const Multiprotocols::MultiProtocolDefinition & proto: protocols)
    if (proto.protocol == protocol)
      return proto;

  // Protocol not found, return the default (last) proto
  Q_ASSERT(protocols.rbegin()->protocol == 0xfe);
  return *protocols.rbegin();
}

// static
QString Multiprotocols::protocolToString(int protocol, bool custom)
{
  static const QStringList strings({
    "FlySky", "Hubsan", "FrSky", "Hisky", "V2x2", "DSM", "Devo", "YD717", "KN", "SymaX", "SLT", "CX10", "CG023",
    "Bayang", "ESky", "MT99XX", "MJXQ", "Shenqi", "FY326", "SFHSS", "J6 PRO","FQ777","Assan","Hontai","Open LRS",
    "FlySky AFHDS2A", "Q2x2", "Walkera", "Q303", "GW008", "DM002", "Cabell", "ESky 150", "H8 3D", "Corona", "CFlie",
	"Hitec", "Wfly", "Bugs", "Bugs Mini", "Traxxas", "NCC-1701-A", "E01X", "V911S", "GD00X"
  });

  if (protocol == MM_RF_CUSTOM_SELECTED || custom)
    return tr("Custom - proto %1)").arg(protocol);
  else
    return strings.value(protocol, CPN_STR_UNKNOWN_ITEM);
}

// static
QString Multiprotocols::subTypeToString(int protocol, unsigned subType)
{
  return tr(qPrintable(multiProtocols.getProtocol(protocol).subTypeStrings.value(subType, CPN_STR_UNKNOWN_ITEM)));
}
