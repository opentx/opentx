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

#if defined(ROTARY_ENCODER_NAVIGATION)
  #define CASE_EVT_ROTARY_LEFT         case EVT_ROTARY_LEFT:
  #define CASE_EVT_ROTARY_RIGHT        case EVT_ROTARY_RIGHT:
#else
  #define CASE_EVT_ROTARY_LEFT
  #define CASE_EVT_ROTARY_RIGHT
#endif

#if defined(CPUARM)
typedef bool (*IsValueAvailable)(int);

int circularIncDec(int current, int inc, int min, int max, IsValueAvailable isValueAvailable=NULL);
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
bool isModuleAvailable(int module);
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

bool modelHasNotes();
#endif

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

#if defined(CPUARM)
#include "telemetry/telemetry_sensors.h"
void drawValueWithUnit(coord_t x, coord_t y, int32_t val, uint8_t unit, LcdFlags flags);
void drawCurveRef(coord_t x, coord_t y, CurveRef & curve, LcdFlags flags=0);
void drawDate(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags flags=0);
void drawTelemScreenDate(coord_t x, coord_t y, source_t sensor, LcdFlags flags=0);
void drawGPSPosition(coord_t x, coord_t y, int32_t longitude, int32_t latitude, LcdFlags flags=0);
void drawGPSSensorValue(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags flags=0);
void drawSensorCustomValue(coord_t x, coord_t y, uint8_t sensor, int32_t value, LcdFlags flags=0);
void drawSourceCustomValue(coord_t x, coord_t y, source_t channel, int32_t val, LcdFlags flags=0);
void drawSourceValue(coord_t x, coord_t y, source_t channel, LcdFlags flags=0);
#endif

void drawCurve(coord_t offset=0);

#if defined(COLORLCD)
void drawStringWithIndex(coord_t x, coord_t y, const char * str, int idx, LcdFlags flags=0, const char * prefix=NULL, const char * suffix=NULL);
int editChoice(coord_t x, coord_t y, const char * values, int value, int min, int max, LcdFlags flags, event_t event);
uint8_t editCheckBox(uint8_t value, coord_t x, coord_t y, LcdFlags flags, event_t event);
swsrc_t editSwitch(coord_t x, coord_t y, swsrc_t value, LcdFlags flags, event_t event);
void drawFatalErrorScreen(const char * message);
void runFatalErrorScreen(const char * message);
#endif

void lcdDrawMMM(coord_t x, coord_t y, LcdFlags flags=0);

// model_setup Defines that are used in all uis in the same way
#define EXTERNAL_MODULE_CHANNELS_ROWS   IF_EXTERNAL_MODULE_ON((IS_MODULE_DSM2(EXTERNAL_MODULE) || IS_MODULE_CROSSFIRE(EXTERNAL_MODULE) || IS_MODULE_SBUS(EXTERNAL_MODULE) || (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) && g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) != MM_RF_PROTO_DSM2)) ? (uint8_t)0 : (uint8_t)1)


#if defined(MULTIMODULE)
#define MULTIMODULE_STATUS_ROWS         IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? TITLE_ROW : HIDDEN_ROW, (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) && multiSyncStatus.isValid()) ? TITLE_ROW : HIDDEN_ROW,
#define MULTIMODULE_MODULE_ROWS         IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? (uint8_t) 0 : HIDDEN_ROW,
#define MULTIMODULE_MODE_ROWS(x)        (g_model.moduleData[x].multi.customProto) ? (uint8_t) 3 :MULTIMODULE_HAS_SUBTYPE(g_model.moduleData[x].getMultiProtocol(true)) ? (uint8_t)2 : (uint8_t)1
#define MULTIMODULE_RFPROTO_ROWS(x)     (g_model.moduleData[x].multi.customProto) ? (uint8_t) 1 :MULTIMODULE_HAS_SUBTYPE(g_model.moduleData[x].getMultiProtocol(true)) ? (uint8_t) 0 : HIDDEN_ROW
#define MULTIMODULE_SUBTYPE_ROWS(x)     IS_MODULE_MULTIMODULE(x) ? MULTIMODULE_RFPROTO_ROWS(x) : HIDDEN_ROW,
#define MULTIMODULE_HAS_SUBTYPE(x)      (getMultiProtocolDefinition(x)->maxSubtype > 0)
#define MULTIMODULE_HASOPTIONS(x)       (getMultiProtocolDefinition(x)->optionsstr != nullptr)
#define MULTI_MAX_RX_NUM(x)             (g_model.moduleData[x].getMultiProtocol(true) == MM_RF_PROTO_OLRS ? 4 : 15)
#define MULTIMODULE_HASFAILSAFE(x)      (IS_MODULE_MULTIMODULE(x) && multiModuleStatus.isValid() && multiModuleStatus.supportsFailsafe())
#define MULTIMODULE_OPTIONS_ROW         (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) && MULTIMODULE_HASOPTIONS(g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true))) ? (uint8_t) 0: HIDDEN_ROW

// When using packed, the pointer in here end up not being aligned, which clang and gcc complain about
// Keep the order of the fields that the so that the size stays small
struct mm_protocol_definition {
  uint8_t protocol;
  uint8_t maxSubtype;
  bool failsafe;
  const pm_char *subTypeString;
  const char *optionsstr;
};

const mm_protocol_definition *getMultiProtocolDefinition (uint8_t protocol);
#else
#define MULTIMODULE_STATUS_ROWS
#define MULTIMODULE_MODULE_ROWS
#define MULTIMODULE_HASFAILSAFE(x)         false
#define MULTIMODULE_SUBTYPE_ROWS(x)
#define MULTIMODULE_MODE_ROWS(x)        (uint8_t)0
#define MULTI_MAX_RX_NUM(x)             15
#define MULTIMODULE_OPTIONS_ROW         HIDDEN_ROW
#endif


#define MAX_RX_NUM(x)                  (IS_MODULE_DSM2(x) ? 20 : IS_MODULE_MULTIMODULE(x) ? MULTI_MAX_RX_NUM(x) : 63)
#define IS_D8_RX(x)                    (g_model.moduleData[x].rfProtocol == RF_PROTO_D8)
#define IS_R9M_OR_XJTD16(x)            ((IS_MODULE_XJT(x) && g_model.moduleData[x].rfProtocol== RF_PROTO_X16) || IS_MODULE_R9M(x))

#define FAILSAFE_ROWS(x)               ((IS_MODULE_XJT(x) && HAS_RF_PROTOCOL_FAILSAFE(g_model.moduleData[x].rfProtocol)) || MULTIMODULE_HASFAILSAFE(x) || IS_MODULE_R9M(x))  ? (g_model.moduleData[x].failsafeMode==FAILSAFE_CUSTOM ? (uint8_t)1 : (uint8_t)0) : HIDDEN_ROW

#define EXTERNAL_MODULE_OPTION_ROW     (IS_MODULE_R9M(EXTERNAL_MODULE) || IS_MODULE_SBUS(EXTERNAL_MODULE)  ? TITLE_ROW : MULTIMODULE_OPTIONS_ROW)
#if defined(PCBXLITE) && !defined(MODULE_R9M_FULLSIZE)
#define EXTERNAL_MODULE_POWER_ROW      (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) || IS_MODULE_R9M(EXTERNAL_MODULE)) ? (IS_MODULE_R9M_FCC_VARIANT(EXTERNAL_MODULE) ? TITLE_ROW : (uint8_t) 0) : HIDDEN_ROW
#else
#define EXTERNAL_MODULE_POWER_ROW      (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) || IS_MODULE_R9M(EXTERNAL_MODULE)) ? (uint8_t) 0 : HIDDEN_ROW
#endif

void editStickHardwareSettings(coord_t x, coord_t y, int idx, event_t event, LcdFlags flags);



#endif // _GUI_COMMON_H_
