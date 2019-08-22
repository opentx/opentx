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

#ifndef _GUI_COMMON_H_
#define _GUI_COMMON_H_

#include "lcd.h"
#include "keys.h"
#include "telemetry/telemetry_sensors.h"

#define READONLY_ROW                   ((uint8_t)-1)
#define TITLE_ROW                      READONLY_ROW
#define HIDDEN_ROW                     ((uint8_t)-2)

#if defined(ROTARY_ENCODER_NAVIGATION)
  #define CASE_EVT_ROTARY_LEFT         case EVT_ROTARY_LEFT:
  #define CASE_EVT_ROTARY_RIGHT        case EVT_ROTARY_RIGHT:
#else
  #define CASE_EVT_ROTARY_LEFT
  #define CASE_EVT_ROTARY_RIGHT
#endif

typedef bool (*IsValueAvailable)(int);

int circularIncDec(int current, int inc, int min, int max, IsValueAvailable isValueAvailable=nullptr);
int getFirstAvailable(int min, int max, IsValueAvailable isValueAvailable);

bool isTrimModeAvailable(int mode);
bool isInputAvailable(int input);
bool isInputSourceAvailable(int source);
bool isThrottleSourceAvailable(int source);
bool isLogicalSwitchFunctionAvailable(int function);
bool isLogicalSwitchAvailable(int index);
bool isAssignableFunctionAvailable(int function);
bool isSourceAvailable(int source);
bool isSourceAvailableInGlobalFunctions(int source);
bool isSourceAvailableInCustomSwitches(int source);
bool isSourceAvailableInResetSpecialFunction(int index);
bool isSourceAvailableInGlobalResetSpecialFunction(int index);
bool isSwitchAvailableInLogicalSwitches(int swtch);
bool isSwitchAvailableInCustomFunctions(int swtch);
bool isSwitchAvailableInMixes(int swtch);
bool isSwitchAvailableInTimers(int swtch);
bool isR9MModeAvailable(int mode);
bool isPxx2IsrmChannelsCountAllowed(int channels);
bool isExternalModuleAvailable(int moduleType);
bool isInternalModuleAvailable(int moduleType);
bool isRfProtocolAvailable(int protocol);
bool isTelemetryProtocolAvailable(int protocol);
bool isTrainerModeAvailable(int mode);

bool isSensorUnit(int sensor, uint8_t unit);
bool isCellsSensor(int sensor);
bool isGPSSensor(int sensor);
bool isAltSensor(int sensor);
bool isVoltsSensor(int sensor);
bool isCurrentSensor(int sensor);
bool isTelemetryFieldAvailable(int index);
bool isTelemetryFieldComparisonAvailable(int index);
bool isSensorAvailable(int sensor);
bool isRssiSensorAvailable(int sensor);

bool modelHasNotes();

#if defined(COLORLCD)
bool isSwitch2POSWarningStateAvailable(int state);
#endif

#if defined(GUI)
#define IS_INSTANT_TRIM_ALLOWED()      (IS_MAIN_VIEW_DISPLAYED() || IS_TELEMETRY_VIEW_DISPLAYED() || IS_OTHER_VIEW_DISPLAYED())
#else
#define IS_INSTANT_TRIM_ALLOWED()      true
#endif

#if defined(FLIGHT_MODES)
void drawFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
#endif

swsrc_t checkIncDecMovedSwitch(swsrc_t val);

void drawValueWithUnit(coord_t x, coord_t y, int val, uint8_t unit, LcdFlags flags);
void drawCurveRef(coord_t x, coord_t y, CurveRef & curve, LcdFlags flags=0);
void drawDate(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags flags=0);
void drawTelemScreenDate(coord_t x, coord_t y, source_t sensor, LcdFlags flags=0);
void drawGPSPosition(coord_t x, coord_t y, int32_t longitude, int32_t latitude, LcdFlags flags=0);
void drawGPSSensorValue(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags flags=0);
void drawSensorCustomValue(coord_t x, coord_t y, uint8_t sensor, int32_t value, LcdFlags flags=0);
void drawSourceCustomValue(coord_t x, coord_t y, source_t channel, int32_t val, LcdFlags flags=0);
void drawSourceValue(coord_t x, coord_t y, source_t channel, LcdFlags flags=0);

void drawCurve(coord_t offset=0);

#if defined(COLORLCD)
void drawStringWithIndex(coord_t x, coord_t y, const char * str, int idx, LcdFlags flags=0, const char * prefix=nullptr, const char * suffix=nullptr);
int editChoice(coord_t x, coord_t y, const char * values, int value, int min, int max, LcdFlags flags, event_t event, IsValueAvailable isValueAvailable=nullptr);
uint8_t editCheckBox(uint8_t value, coord_t x, coord_t y, LcdFlags flags, event_t event);
swsrc_t editSwitch(coord_t x, coord_t y, swsrc_t value, LcdFlags flags, event_t event);
void drawFatalErrorScreen(const char * message);
void runFatalErrorScreen(const char * message);
#endif

void lcdDrawMMM(coord_t x, coord_t y, LcdFlags flags=0);

// model_setup Defines that are used in all uis in the same way
#define INTERNAL_MODULE_CHANNELS_ROWS   IF_INTERNAL_MODULE_ON((uint8_t)1)
#define EXTERNAL_MODULE_CHANNELS_ROWS   IF_EXTERNAL_MODULE_ON((isModuleDSM2(EXTERNAL_MODULE) || isModuleCrossfire(EXTERNAL_MODULE) || isModuleSBUS(EXTERNAL_MODULE) || (isModuleMultimodule(EXTERNAL_MODULE) && g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) != MODULE_SUBTYPE_MULTI_DSM2)) ? (uint8_t)0 : (uint8_t)1)
#define EXTERNAL_MODULE_BIND_ROWS       (isModuleXJTD8(EXTERNAL_MODULE) || isModuleSBUS(EXTERNAL_MODULE)) ? (uint8_t)1 : (isModulePPM(EXTERNAL_MODULE) || isModulePXX1(EXTERNAL_MODULE) || isModulePXX2(EXTERNAL_MODULE) || isModuleDSM2(EXTERNAL_MODULE) || isModuleMultimodule(EXTERNAL_MODULE)) ? (uint8_t)2 : HIDDEN_ROW

#if defined(MULTIMODULE)
// When using packed, the pointer in here end up not being aligned, which clang and gcc complain about
// Keep the order of the fields that the so that the size stays small
struct mm_protocol_definition {
    uint8_t protocol;
    uint8_t maxSubtype;
    bool failsafe;
    const char *subTypeString;
    const char *optionsstr;
};
const mm_protocol_definition *getMultiProtocolDefinition (uint8_t protocol);
#define MULTIMODULE_STATUS_ROWS         isModuleMultimodule(EXTERNAL_MODULE) ? TITLE_ROW : HIDDEN_ROW, (isModuleMultimodule(EXTERNAL_MODULE) && multiSyncStatus.isValid()) ? TITLE_ROW : HIDDEN_ROW,
#define MULTIMODULE_MODULE_ROWS         isModuleMultimodule(EXTERNAL_MODULE) ? (uint8_t) 0 : HIDDEN_ROW,
#define MULTIMODULE_MODE_ROWS(x)        (g_model.moduleData[x].multi.customProto) ? (uint8_t) 3 :MULTIMODULE_HAS_SUBTYPE(g_model.moduleData[x].getMultiProtocol(true)) ? (uint8_t)2 : (uint8_t)1
inline bool MULTIMODULE_HAS_SUBTYPE(uint8_t moduleIdx)
{
  return getMultiProtocolDefinition(moduleIdx)->maxSubtype > 0;
}
inline uint8_t MULTIMODULE_RFPROTO_COLUMNS(uint8_t moduleIdx)
{
  return (g_model.moduleData[moduleIdx].multi.customProto ? (uint8_t) 1 : MULTIMODULE_HAS_SUBTYPE(g_model.moduleData[moduleIdx].getMultiProtocol(true)) ? (uint8_t) 0 : HIDDEN_ROW);
}
#define MULTIMODULE_SUBTYPE_ROWS(x)     isModuleMultimodule(x) ? MULTIMODULE_RFPROTO_COLUMNS(x) : HIDDEN_ROW,
#define MULTIMODULE_HASOPTIONS(x)       (getMultiProtocolDefinition(x)->optionsstr != nullptr)
#define MULTIMODULE_OPTIONS_ROW         (isModuleMultimodule(EXTERNAL_MODULE) && MULTIMODULE_HASOPTIONS(g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true))) ? (uint8_t) 0: HIDDEN_ROW

#else
#define MULTIMODULE_STATUS_ROWS
#define MULTIMODULE_MODULE_ROWS
#define MULTIMODULE_SUBTYPE_ROWS(x)
#define MULTIMODULE_MODE_ROWS(x)        (uint8_t)0
#define MULTIMODULE_OPTIONS_ROW         HIDDEN_ROW
#endif

#define FAILSAFE_ROWS(x)               isModuleFailsafeAvailable(x) ? (g_model.moduleData[x].failsafeMode==FAILSAFE_CUSTOM ? (uint8_t)1 : (uint8_t)0) : HIDDEN_ROW
#define EXTERNAL_MODULE_OPTION_ROW     (isModuleR9MNonAccess(EXTERNAL_MODULE) || isModuleSBUS(EXTERNAL_MODULE)  ? TITLE_ROW : MULTIMODULE_OPTIONS_ROW)
#define EXTERNAL_MODULE_POWER_ROW      (isModuleMultimodule(EXTERNAL_MODULE) || isModuleR9MNonAccess(EXTERNAL_MODULE)) ? (isModuleR9MLiteNonPro(EXTERNAL_MODULE) ? (isModuleR9M_FCC_VARIANT(EXTERNAL_MODULE) ? READONLY_ROW : (uint8_t)0) : (uint8_t)0) : HIDDEN_ROW

void editStickHardwareSettings(coord_t x, coord_t y, int idx, event_t event, LcdFlags flags);

const char * writeScreenshot();

#endif // _GUI_COMMON_H_
