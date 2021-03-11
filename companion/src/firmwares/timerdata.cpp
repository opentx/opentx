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

#include "timerdata.h"
#include "radiodataconversionstate.h"

void TimerData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent(tr("TMR"), 1);
  cstate.setSubComp(tr("Timer %1").arg(cstate.subCompIdx + 1));
  mode.convert(cstate);
}

void TimerData::clear()
{
  memset(reinterpret_cast<void *>(this), 0, sizeof(TimerData));
  mode = RawSwitch(SWITCH_TYPE_TIMER_MODE, 0);
}

bool TimerData::isEmpty()
{
  return (mode == RawSwitch(SWITCH_TYPE_TIMER_MODE, 0) && name[0] == '\0' && minuteBeep == 0 && countdownBeep == COUNTDOWN_SILENT && val == 0 && persistent == 0 /*&& pvalue == 0*/);
}

bool TimerData::isModeOff()
{
  return mode == RawSwitch(SWITCH_TYPE_TIMER_MODE, 0);
}

QString TimerData::nameToString(int index) const
{
  return DataHelpers::getElementName(tr("TMR", "as in Timer"), index + 1, name);
}
