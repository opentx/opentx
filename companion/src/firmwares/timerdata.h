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

#pragma once

#include "rawswitch.h"
#include "datahelpers.h"

#include <QtCore>

class RadioDataConversionState;

#define TIMER_NAME_LEN 8

class TimerData {
  Q_DECLARE_TR_FUNCTIONS(TimerData)

  public:
    enum CountDownMode {
      COUNTDOWN_SILENT,
      COUNTDOWN_BEEPS,
      COUNTDOWN_VOICE,
      COUNTDOWN_HAPTIC,
      COUNTDOWN_COUNT
    };

    TimerData() { clear(); }

    RawSwitch    mode;
    char         name[TIMER_NAME_LEN + 1];
    bool         minuteBeep;
    unsigned int countdownBeep;
    unsigned int val;
    unsigned int persistent;
    int          countdownStart;
    unsigned int direction;
    int          pvalue;

    void convert(RadioDataConversionState & cstate);
    void clear();
    bool isEmpty();
    bool isModeOff();
    QString nameToString(int index) const;
};
