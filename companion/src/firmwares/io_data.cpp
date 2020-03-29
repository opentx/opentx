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

#include "io_data.h"

#include "radiodata.h"  // for RadioData::getElementName
#include "radiodataconversionstate.h"

/*
 * ExpoData
 */

void ExpoData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent(tr("INP"), 3);
  cstate.setSubComp(RawSource(SOURCE_TYPE_VIRTUAL_INPUT, chn).toString(cstate.fromModel(), cstate.fromGS(), cstate.fromType) % tr(" (@%1)").arg(cstate.subCompIdx));
  srcRaw.convert(cstate);
  swtch.convert(cstate);
}

bool ExpoData::isEmpty() const
{
  return (chn == 0 && mode == INPUT_MODE_NONE);
}

/*
 * MixData
 */

void MixData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent(tr("MIX"), 4);
  cstate.setSubComp(RawSource(SOURCE_TYPE_CH, destCh-1).toString(cstate.fromModel(), cstate.fromGS(), cstate.fromType) % tr(" (@%1)").arg(cstate.subCompIdx));
  srcRaw.convert(cstate);
  swtch.convert(cstate);
}

bool MixData::isEmpty() const
{
  return (destCh == 0);
}

/*
 * LimitData
 */

QString LimitData::minToString() const
{
  return QString::number((qreal)min/10);
}

QString LimitData::maxToString() const
{
  return QString::number((qreal)max/10);
}

QString LimitData::revertToString() const
{
  return revert ? tr("INV") : tr("NOR");
}

QString LimitData::nameToString(int index) const
{
  return RadioData::getElementName(tr("CH"), index + 1, name);
}

QString LimitData::offsetToString() const
{
  return QString::number((qreal)offset/10, 'f', 1);
}

void LimitData::clear()
{
  memset(reinterpret_cast<void *>(this), 0, sizeof(LimitData));
  min = -1000;
  max = +1000;
}

bool LimitData::isEmpty() const
{
  return (min == -1000 && max == 1000 && !revert && !offset && !ppmCenter && !symetrical && name[0] == '\0' && !curve.isSet());
}

/*
 * CurveData
 */

CurveData::CurveData()
{
  clear();
}

void CurveData::clear(int count)
{
  memset(this, 0, sizeof(CurveData));
  this->count = count;
}

bool CurveData::isEmpty() const
{
  for (int i=0; i<count; i++) {
    if (points[i].y != 0) {
      return false;
    }
  }
  return true;
}

QString CurveData::nameToString(const int idx) const
{
  return RadioData::getElementName(tr("CV"), idx + 1, name);
}


/*
 * FlightModeData
 */

void FlightModeData::clear(const int phaseIdx)
{
  memset(reinterpret_cast<void *>(this), 0, sizeof(FlightModeData));
  for (int i = 0; i < CPN_MAX_GVARS; i++) {
    gvars[i] = linkedGVarFlightModeZero(phaseIdx);
  }
  for (int i = 0; i < CPN_MAX_ENCODERS; i++) {
    rotaryEncoders[i] = linkedREncFlightModeZero(phaseIdx);
  }
}

QString FlightModeData::nameToString(int phaseIdx) const
{
  return RadioData::getElementName(tr("FM"), phaseIdx, name);  // names are zero-based, FM0, FM1, etc
}

void FlightModeData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent("FMD", 2);
  cstate.setSubComp(nameToString(cstate.subCompIdx));
  swtch.convert(cstate);
}

bool FlightModeData::isEmpty(int phaseIdx) const
{
  if (name[0] != '\0' || swtch.isSet() || fadeIn != 0 || fadeOut != 0)
    return false;
  for (int i = 0; i < CPN_MAX_TRIMS; i++) {
    if (trim[i] != 0 || trimRef[i] != 0 || trimMode[i] != 0)
      return false;
  }
  for (int i = 0; i < CPN_MAX_GVARS; i++) {
    if (!isGVarEmpty(phaseIdx, i))
      return false;
  }
  for (int i = 0; i < CPN_MAX_ENCODERS; i++) {
    if (!isREncEmpty(phaseIdx, i))
      return false;
  }
  return true;
}

bool FlightModeData::isGVarEmpty(int phaseIdx, int gvIdx) const
{
  if ((phaseIdx == 0 && gvars[gvIdx] == 0) || (phaseIdx != 0 && gvars[gvIdx] == linkedGVarFlightModeZero(phaseIdx)))
    return true;
  return false;
}

bool FlightModeData::isREncEmpty(int phaseIdx, int reIdx) const
{
  if ((phaseIdx == 0 && rotaryEncoders[reIdx] == 0) || (phaseIdx != 0 && rotaryEncoders[reIdx] == linkedREncFlightModeZero(phaseIdx)))
    return true;
  return false;
}

int FlightModeData::linkedFlightModeZero(int phaseIdx, int maxOwnValue) const
{
  if (phaseIdx == 0)
    return 0;
  return maxOwnValue + 1;
}

int FlightModeData::linkedGVarFlightModeZero(int phaseIdx) const
{
  return linkedFlightModeZero(phaseIdx, GVAR_MAX_VALUE);
}

int FlightModeData::linkedREncFlightModeZero(int phaseIdx) const
{
  return linkedFlightModeZero(phaseIdx, RENC_MAX_VALUE);
}
