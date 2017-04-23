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

#ifndef _MENUS_H_
#define _MENUS_H_

#include "keys.h"

#if defined(PCBX7)
typedef int8_t horzpos_t;
#define NAVIGATION_LINE_BY_LINE        0x40
#define IS_LINE_SELECTED(sub, k)       ((sub)==(k) && menuHorizontalPosition < 0)
#else
typedef uint8_t horzpos_t;
#define NAVIGATION_LINE_BY_LINE        0
#define IS_LINE_SELECTED(sub, k)       (false)
#endif

#if defined(CPUARM) || defined(SDCARD)
typedef uint16_t vertpos_t;
#else
typedef uint8_t vertpos_t;
#endif

typedef void (*MenuHandlerFunc)(event_t event);

#if defined(CPUARM)
extern tmr10ms_t menuEntryTime;
#endif

extern vertpos_t menuVerticalPosition;
extern horzpos_t menuHorizontalPosition;
extern vertpos_t menuVerticalOffset;
extern uint8_t menuCalibrationState;

extern MenuHandlerFunc menuHandlers[5];
extern uint8_t menuVerticalPositions[4];
extern uint8_t menuLevel;
extern uint8_t menuEvent;

void chainMenu(MenuHandlerFunc newMenu);
void pushMenu(MenuHandlerFunc newMenu);
void popMenu();

inline MenuHandlerFunc lastPopMenu()
{
  return menuHandlers[menuLevel+1];
}

void onMainViewMenu(const char * result);

void menuFirstCalib(event_t event);
void menuMainView(event_t event);
void menuViewTelemetryFrsky(event_t event);
void menuViewTelemetryMavlink(event_t event);

void menuRadioSetup(event_t event);
void menuRadioCalibration(event_t event);

enum MenuModelIndexes
{
  MENU_MODEL_INPUTS,
  MENU_MODEL_OUTPUTS,
  MENU_MODEL_CURVES,
  MENU_MODEL_CONTROLS,
  MENU_RADIO_SETUP,
  MENU_RADIO_CALIBRATION,
  MENU_MODEL_PAGES_COUNT
};

void menuModelExposAll(event_t event);
void menuModelLimits(event_t event);
void menuModelCurvesAll(event_t event);
void menuModelCurveOne(event_t event);
void menuModelControls(event_t event);

static const MenuHandlerFunc menuTabModel[] PROGMEM = {
  menuModelExposAll,
  menuModelLimits,
  menuModelCurvesAll,
  menuModelControls,
  menuRadioSetup,
  menuRadioCalibration
};

void menuStatisticsView(event_t event);
void menuStatisticsDebug(event_t event);
void menuAboutView(event_t event);

#if defined(DEBUG_TRACE_BUFFER)
void menuTraceBuffer(event_t event);
#endif

#endif // _MENUS_H_
