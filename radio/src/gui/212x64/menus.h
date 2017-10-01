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

typedef int8_t   horzpos_t;
typedef uint16_t vertpos_t;

typedef void (*MenuHandlerFunc)(event_t event);

extern tmr10ms_t menuEntryTime;
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

void onMainViewMenu(const char * result);

void menuFirstCalib(event_t event);
void menuMainViewChannelsMonitor(event_t event);
void menuChannelsView(event_t event);
void menuMainView(event_t event);
void menuViewTelemetryFrsky(event_t event);
void menuSpecialFunctions(event_t event, CustomFunctionData * functions, CustomFunctionsContext * functionsContext);
void menuModelNotes(event_t event);
void menuStatisticsView(event_t event);
void menuStatisticsDebug(event_t event);
void menuStatisticsDebug2(event_t event);
void menuAboutView(event_t event);
#if defined(DEBUG_TRACE_BUFFER)
void menuTraceBuffer(event_t event);
#endif

enum MenuRadioIndexes {
  MENU_RADIO_SETUP,
  MENU_RADIO_SD_MANAGER,
  MENU_RADIO_SPECIAL_FUNCTIONS,
  MENU_RADIO_TRAINER,
  MENU_RADIO_VERSION,
  MENU_RADIO_SWITCHES_TEST,
  MENU_RADIO_ANALOGS_TEST,
  MENU_RADIO_HARDWARE,
  MENU_RADIO_CALIBRATION,
  MENU_RADIO_PAGES_COUNT
};

void menuRadioSetup(event_t event);
void menuRadioSdManager(event_t event);
void menuRadioSpecialFunctions(event_t event);
void menuRadioTrainer(event_t event);
void menuRadioVersion(event_t event);
void menuRadioDiagKeys(event_t event);
void menuRadioDiagAnalogs(event_t event);
void menuRadioHardware(event_t event);
void menuRadioCalibration(event_t event);

extern const MenuHandlerFunc menuTabGeneral[MENU_RADIO_PAGES_COUNT];

enum MenuModelIndexes {
  MENU_MODEL_SELECT,
  MENU_MODEL_SETUP,
  CASE_HELI(MENU_MODEL_HELI)
  CASE_FLIGHT_MODES(MENU_MODEL_FLIGHT_MODES)
  MENU_MODEL_INPUTS,
  MENU_MODEL_MIXES,
  MENU_MODEL_OUTPUTS,
  CASE_CURVES(MENU_MODEL_CURVES)
  CASE_GVARS(MENU_MODEL_GVARS)
  MENU_MODEL_LOGICAL_SWITCHES,
  MENU_MODEL_SPECIAL_FUNCTIONS,
#if defined(LUA_MODEL_SCRIPTS)
  MENU_MODEL_CUSTOM_SCRIPTS,
#endif
  MENU_MODEL_TELEMETRY_FRSKY,
  CASE_MAVLINK(MENU_MODEL_TELEMETRY_MAVLINK)
  MENU_MODEL_DISPLAY,
  MENU_MODEL_PAGES_COUNT
};

void menuModelSelect(event_t event);
void menuModelSetup(event_t event);
void menuModelFailsafe(event_t event);
void menuModelHeli(event_t event);
void menuModelFlightModesAll(event_t event);
void menuModelExpoOne(event_t event);
void menuModelExposAll(event_t event);
void menuModelMixOne(event_t event);
void menuModelMixAll(event_t event);
void menuModelLimits(event_t event);
void menuModelCurvesAll(event_t event);
void menuModelCurveOne(event_t event);
void menuModelGVars(event_t event);
void menuModelLogicalSwitches(event_t event);
void menuModelSpecialFunctions(event_t event);
void menuModelCustomScripts(event_t event);
void menuModelTelemetryFrsky(event_t event);
void menuModelDisplay(event_t event);

extern const MenuHandlerFunc menuTabModel[MENU_MODEL_PAGES_COUNT];

#endif // _MENUS_H_
