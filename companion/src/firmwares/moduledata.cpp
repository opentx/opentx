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

#include "moduledata.h"
#include "eeprominterface.h"
#include "multiprotocols.h"
#include "radiodataconversionstate.h"

void ModuleData::convert(RadioDataConversionState & cstate)
{
  if (protocol == PULSES_PXX_R9M && (IS_TARANIS_XLITE(cstate.fromType) || IS_TARANIS_XLITE(cstate.toType))) {
    clear();
  }
}

bool ModuleData::isPxx2Module() const
{
  switch(protocol){
    case PULSES_ACCESS_ISRM:
    case PULSES_ACCESS_R9M:
    case PULSES_ACCESS_R9M_LITE:
    case PULSES_ACCESS_R9M_LITE_PRO:
    case PULSES_XJT_LITE_X16:
      return true;
    default:
      return false;
  }
}

QString ModuleData::rfProtocolToString() const
{
  switch (protocol) {
    case PULSES_MULTIMODULE:
      return Multiprotocols::protocolToString((int)multi.rfProtocol, multi.customProto);

    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

QString ModuleData::subTypeToString(int type) const
{
  static const char * strings[] = {
    "FCC",
    "EU",
    "868MHz",
    "915MHz"
  };

  if (type < 0)
    type = subType;

  switch (protocol) {
    case PULSES_MULTIMODULE:
      return Multiprotocols::subTypeToString(multi.customProto ? MM_RF_CUSTOM_SELECTED : (int)multi.rfProtocol, (unsigned)type);

    case PULSES_PXX_R9M:
      return CHECK_IN_ARRAY(strings, type);

    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

QString ModuleData::powerValueToString(Firmware * fw) const
{
  const QStringList & strRef = powerValueStrings(subType, fw);
  // EU module with telemetry can only be < 100/200mW.
  if (pxx.sport_out && subType == MODULE_SUBTYPE_R9M_EU && pxx.power > 1)
    return CPN_STR_UNKNOWN_ITEM;

  return strRef.value(pxx.power, CPN_STR_UNKNOWN_ITEM);
}

// static
QString ModuleData::indexToString(int index, Firmware * fw)
{
  if (index < 0)
    return tr("Trainer Port");

  if (fw->getCapability(NumModules) > 1) {
    if (IS_HORUS_OR_TARANIS(fw->getBoard()))
      return index == 0 ? tr("Internal Radio System") : tr("External Radio Module");
    if (index > 0)
      return tr("Extra Radio System");
  }
  return tr("Radio System");
}

// static
QString ModuleData::protocolToString(unsigned protocol)
{
  static const char * strings[] = {
    "OFF",
    "PPM",
    "Silverlit A", "Silverlit B", "Silverlit C",
    "CTP1009",
    "LP45", "DSM2", "DSMX",
    "PPM16", "PPMsim",
    "FrSky XJT (D16)", "FrSky XJT (D8)", "FrSky XJT (LR12)", "FrSky DJT",
    "TBS Crossfire",
    "DIY Multiprotocol Module",
    "FrSky PXX R9M",
    "FrSky PXX R9M Lite",
    "FrSky PXX R9M Lite Pro",
    "SBUS output at VBat",
    "FrSky ACCESS ISRM", "FrSky ACCST ISRM D16",
    "FrSky ACCESS R9M",
    "FrSky ACCESS R9M Lite",
    "FrSky ACCESS R9M Lite Pro",
    "FrSky XJT lite (D16)", "FrSky XJT lite (D8)", "FrSky XJT lite (LR12)"
  };

  return CHECK_IN_ARRAY(strings, protocol);
}

// static
QStringList ModuleData::powerValueStrings(int subType, Firmware * fw)
{
  static const QStringList strings[] = {
    { tr("10mW - 16CH"), tr("100mW - 16CH"), tr("500mW - 16CH"), tr("Auto <= 1W - 16CH") },                         // full-size FCC
    { tr("25mW - 8CH"), tr("25mW - 16CH"), tr("200mW - 16CH (no telemetry)"), tr("500mW - 16CH (no telemetry)") },  // full-size EU
    { tr("100mW - 16CH") },                                                                                         // mini FCC
    { tr("25mW - 8CH"), tr("25mW - 16CH"), tr("100mW - 16CH (no telemetry)") }                                      // mini EU
  };
  int strIdx = 0;
  if (subType == MODULE_SUBTYPE_R9M_EU)
    strIdx += 1;
  if (fw->getCapability(HasModuleR9MMini))
    strIdx += 2;
  return strings[strIdx];
}
