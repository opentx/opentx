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

QString TimerData::countdownBeepToString() const
{
  return countdownBeepToString(countdownBeep);
}

QString TimerData::countdownStartToString() const
{
  return countdownStartToString(countdownStart);
}

QString TimerData::persistentToString() const
{
  return persistentToString(persistent);
}

//  static
QString TimerData::countdownBeepToString(const int value)
{
  switch(value) {
    case COUNTDOWNBEEP_SILENT:
      return tr("Silent");
    case COUNTDOWNBEEP_BEEPS:
      return tr("Beeps");
    case COUNTDOWNBEEP_VOICE:
      return tr("Voice");
    case COUNTDOWNBEEP_HAPTIC:
      return tr("Haptic");
    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

//  static
QString TimerData::countdownStartToString(const int value)
{
  switch(value) {
    case COUNTDOWNSTART_5:
      return tr("5s");
    case COUNTDOWNSTART_10:
      return tr("10s");
    case COUNTDOWNSTART_20:
      return tr("20s");
    case COUNTDOWNSTART_30:
      return tr("30s");
    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

//  static
QString TimerData::persistentToString(const int value)
{
  switch(value) {
    case PERSISTENT_NOT:
      return tr("Not persistent");
    case PERSISTENT_FLIGHT:
      return tr("Persistent (flight)");
    case PERSISTENT_MANUALRESET:
      return tr("Persistent (manual reset)");
    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

//  static
AbstractStaticItemModel * TimerData::countdownBeepItemModel()
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName("timerdata.countdownBeep");

  for (int i = 0; i < COUNTDOWNBEEP_COUNT; i++) {
    QString str = countdownBeepToString(i);
  }

  mdl->loadItemList();
  return mdl;
}

//  static
AbstractStaticItemModel * TimerData::countdownStartItemModel()
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName("timerdata.countdownStart");

  for (int i = COUNTDOWNSTART_LAST - 1; i >= COUNTDOWNSTART_FIRST; i--) {
    QString str = countdownStartToString(i);
  }

  mdl->loadItemList();
  return mdl;
}

//  static
AbstractStaticItemModel * TimerData::persistentItemModel()
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName("timerdata.persistent");

  for (int i = 0; i < PERSISTENT_COUNT; i++) {
    QString str = persistentToString(i);
  }

  mdl->loadItemList();
  return mdl;
}
