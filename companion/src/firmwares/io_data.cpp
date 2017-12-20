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
  memset(this, 0, sizeof(LimitData));
  min = -1000;
  max = +1000;
}


/*
 * CurveData
 */

CurveData::CurveData()
{
  clear(5);
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

void FlightModeData::clear(const int phase)
{
  memset(this, 0, sizeof(FlightModeData));
  if (phase != 0) {
    for (int idx=0; idx<CPN_MAX_GVARS; idx++) {
      gvars[idx] = 1025;
    }
    for (int idx=0; idx<CPN_MAX_ENCODERS; idx++) {
      rotaryEncoders[idx] = 1025;
    }
  }
}

QString FlightModeData::nameToString(int index) const
{
  return RadioData::getElementName(tr("FM"), index, name);  // names are zero-based, FM0, FM1, etc
}

void FlightModeData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent("FMD", 2);
  cstate.setSubComp(nameToString(cstate.subCompIdx));
  swtch.convert(cstate);
}
