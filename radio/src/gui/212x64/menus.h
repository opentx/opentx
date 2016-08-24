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

typedef void (*MenuHandlerFunc)(uint8_t event);

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

void menuFirstCalib(uint8_t event);
void menuMainViewChannelsMonitor(uint8_t event);
void menuChannelsView(uint8_t event);
void menuMainView(uint8_t event);
void menuViewTelemetryFrsky(uint8_t event);
void menuSpecialFunctions(uint8_t event, CustomFunctionData * functions, CustomFunctionsContext * functionsContext);
void menuModelNotes(uint8_t event);
void menuStatisticsView(uint8_t event);
void menuStatisticsDebug(uint8_t event);
void menuAboutView(uint8_t event);
#if defined(DEBUG_TRACE_BUFFER)
void menuTraceBuffer(uint8_t event);
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

void menuRadioSetup(uint8_t event);
void menuRadioSdManager(uint8_t event);
void menuRadioSpecialFunctions(uint8_t event);
void menuRadioTrainer(uint8_t event);
void menuRadioVersion(uint8_t event);
void menuRadioDiagKeys(uint8_t event);
void menuRadioDiagAnalogs(uint8_t event);
void menuRadioHardware(uint8_t event);
void menuRadioCalibration(uint8_t event);

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

void menuModelSelect(uint8_t event);
void menuModelSetup(uint8_t event);
void menuModelFailsafe(uint8_t event);
void menuModelHeli(uint8_t event);
void menuModelFlightModesAll(uint8_t event);
void menuModelExposAll(uint8_t event);
void menuModelMixAll(uint8_t event);
void menuModelLimits(uint8_t event);
void menuModelCurvesAll(uint8_t event);
void menuModelCurveOne(uint8_t event);
void menuModelGVars(uint8_t event);
void menuModelLogicalSwitches(uint8_t event);
void menuModelSpecialFunctions(uint8_t event);
void menuModelCustomScripts(uint8_t event);
void menuModelTelemetryFrsky(uint8_t event);
void menuModelDisplay(uint8_t event);
void menuModelExpoOne(uint8_t event);

extern const MenuHandlerFunc menuTabModel[MENU_MODEL_PAGES_COUNT];

#endif // _MENUS_H_
