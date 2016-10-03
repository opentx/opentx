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
bool isSwitchWarningStateAvailable(int state);
#endif

#if defined(GUI)
#define IS_INSTANT_TRIM_ALLOWED()      (IS_MAIN_VIEW_DISPLAYED() || IS_TELEMETRY_VIEW_DISPLAYED() || IS_OTHER_VIEW_DISPLAYED())
#else
#define IS_INSTANT_TRIM_ALLOWED()      true
#endif

#if defined(FLIGHT_MODES)
void drawFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
#endif

#if defined(CPUARM)
#include "telemetry/telemetry_sensors.h"
void drawValueWithUnit(coord_t x, coord_t y, int32_t val, uint8_t unit, LcdFlags flags);
void drawCurveRef(coord_t x, coord_t y, CurveRef & curve, LcdFlags flags=0);
void drawDate(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags flags=0);
void drawGPSPosition(coord_t x, coord_t y, int32_t longitude, int32_t latitude, LcdFlags flags=0);
void drawGPSSensorValue(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags flags=0);
void drawSensorCustomValue(coord_t x, coord_t y, uint8_t sensor, int32_t value, LcdFlags flags=0);
void drawSourceCustomValue(coord_t x, coord_t y, source_t channel, int32_t val, LcdFlags flags=0);
void drawSourceValue(coord_t x, coord_t y, source_t channel, LcdFlags flags=0);
#endif

void drawCurve(coord_t offset=0);

#if defined(COLORLCD)
void drawStringWithIndex(coord_t x, coord_t y, const char * str, int idx, LcdFlags flags=0, const char * prefix=NULL);
int editChoice(coord_t x, coord_t y, const char * values, int value, int min, int max, LcdFlags flags, event_t event);
uint8_t editCheckBox(uint8_t value, coord_t x, coord_t y, LcdFlags flags, event_t event);
swsrc_t editSwitch(coord_t x, coord_t y, swsrc_t value, LcdFlags flags, event_t event);
void drawFatalErrorScreen(const char * message);
void runFatalErrorScreen(const char * message);
#endif

// model_setup Defines that are used in all uis in the same way
#define EXTERNAL_MODULE_CHANNELS_ROWS   IF_EXTERNAL_MODULE_ON((IS_MODULE_DSM2(EXTERNAL_MODULE) || IS_MODULE_CROSSFIRE(EXTERNAL_MODULE) || (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) && g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol() != MM_RF_PROTO_DSM2)) ? (uint8_t)0 : (uint8_t)1)


#if defined(MULTIMODULE)
#define MULTIMODULE_MODULE_ROWS         IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? (uint8_t) 0 : HIDDEN_ROW, IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? (uint8_t) 0 : HIDDEN_ROW,
#define MULTIMODULE_MODE_ROWS(x)        (g_model.moduleData[x].multi.customProto) ? (uint8_t) 3 :MULTIMODULE_HAS_SUBTYPE(g_model.moduleData[x].getMultiProtocol()) ? (uint8_t)2 : (uint8_t)1
#define MULTIMODULE_RFPROTO_ROWS(x)     (g_model.moduleData[x].multi.customProto) ? (uint8_t) 1 :MULTIMODULE_HAS_SUBTYPE(g_model.moduleData[x].getMultiProtocol()) ? (uint8_t) 0 : HIDDEN_ROW
#define MULTIMODULE_SUBTYPE_ROWS(x)     IS_MODULE_MULTIMODULE(x) ? MULTIMODULE_RFPROTO_ROWS(x) : HIDDEN_ROW,
#define MULTIMODULE_HAS_SUBTYPE(x)      (x == MM_RF_PROTO_FLYSKY || x == MM_RF_PROTO_FRSKY || x == MM_RF_PROTO_HISKY || x == MM_RF_PROTO_DSM2 || x == MM_RF_PROTO_YD717 || x == MM_RF_PROTO_KN || x == MM_RF_PROTO_SYMAX || x == MM_RF_PROTO_CX10 || x == MM_RF_PROTO_CG023 || x == MM_RF_PROTO_MT99XX || x == MM_RF_PROTO_MJXQ || x == MM_RF_PROTO_HONTAI)
#define MULTIMODULE_HASOPTIONS(x)       (x == MM_RF_PROTO_HUBSAN || x == MM_RF_PROTO_FRSKY || x == MM_RF_PROTO_DSM2 )
#define MULTIMODULE_FAILSAFEROWS(x)     (IS_MODULE_MULTIMODULE(x) && (g_model.moduleData[x].multi.customProto || MULTIMODULE_HASOPTIONS(g_model.moduleData[x].getMultiProtocol()))) ? (uint8_t) 0: HIDDEN_ROW

#else
#define MULTIMODULE_MODULE_ROWS
#define MULTIMODULE_FAILSAFEROWS(x)     HIDDEN_ROW
#define MULTIMODULE_SUBTYPE_ROWS(x)
#define MULTIMODULE_MODE_ROWS(x)        (uint8_t)0
#endif

#define IS_D8_RX(x)                    (g_model.moduleData[x].rfProtocol == RF_PROTO_D8)
#define IF_TRAINER_ON(x)               (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)(x) : HIDDEN_ROW)

#define FAILSAFE_ROWS(x)               (IS_MODULE_XJT(x) && HAS_RF_PROTOCOL_FAILSAFE(g_model.moduleData[x].rfProtocol) ? (g_model.moduleData[x].failsafeMode==FAILSAFE_CUSTOM ? (uint8_t)1 : (uint8_t)0) : MULTIMODULE_FAILSAFEROWS(x))

#endif // _GUI_COMMON_H_
