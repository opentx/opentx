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

#include "opentx.h"

#if defined(PCBTARANIS) || defined(PCBHORUS)
uint8_t switchToMix(uint8_t source)
{
  div_t qr = div(source-1, 3);
  return qr.quot+MIXSRC_FIRST_SWITCH;
}
#else
uint8_t switchToMix(uint8_t source)
{
  if (source <= 3)
    return MIXSRC_3POS;
  else
    return MIXSRC_FIRST_SWITCH - 3 + source;
}
#endif

int circularIncDec(int current, int inc, int min, int max, IsValueAvailable isValueAvailable)
{
  do {
    current += inc;
    if (current < min)
      current = max;
    else if (current > max)
      current = min;
    if (!isValueAvailable || isValueAvailable(current))
      return current;
  } while(1);
  return 0;
}

bool isInputAvailable(int input)
{
  for (int i=0; i<MAX_EXPOS; i++) {
    ExpoData * expo = expoAddress(i);
    if (!EXPO_VALID(expo))
      break;
    if (expo->chn == input)
      return true;
  }
  return false;
}

bool isRssiSensorAvailable(int sensor)
{
  if (sensor == 0)
    return true;
  else {
    TelemetrySensor &telemSensor = g_model.telemetrySensors[abs(sensor) - 1];
    return (telemSensor.isAvailable() && telemSensor.id == RSSI_ID);
  }
}


bool isSensorAvailable(int sensor)
{
  if (sensor == 0)
    return true;
  else
    return isTelemetryFieldAvailable(abs(sensor) - 1);
}

bool isSensorUnit(int sensor, uint8_t unit)
{
  if (sensor <= 0 || sensor > MAX_TELEMETRY_SENSORS ) {
    return true;
  }
  else {
    return g_model.telemetrySensors[sensor-1].unit == unit;
  }
}

bool isCellsSensor(int sensor)
{
  return isSensorUnit(sensor, UNIT_CELLS);
}

bool isGPSSensor(int sensor)
{
  return isSensorUnit(sensor, UNIT_GPS);
}

bool isAltSensor(int sensor)
{
  return isSensorUnit(sensor, UNIT_DIST) || isSensorUnit(sensor, UNIT_FEET);
}

bool isVoltsSensor(int sensor)
{
  return isSensorUnit(sensor, UNIT_VOLTS) || isSensorUnit(sensor, UNIT_CELLS);
}

bool isCurrentSensor(int sensor)
{
  return isSensorUnit(sensor, UNIT_AMPS);
}

bool isTelemetryFieldAvailable(int index)
{
  TelemetrySensor & sensor = g_model.telemetrySensors[index];
  return sensor.isAvailable();
}

bool isTelemetryFieldComparisonAvailable(int index)
{
  if (!isTelemetryFieldAvailable(index))
    return false;

  TelemetrySensor & sensor = g_model.telemetrySensors[index];
  if (sensor.unit >= UNIT_DATETIME)
    return false;
  return true;
}

bool isChannelUsed(int channel)
{
  for (int i=0; i<MAX_MIXERS; ++i) {
    MixData *md = mixAddress(i);
    if (md->srcRaw == 0) return false;
    if (md->destCh == channel) return true;
    if (md->destCh > channel) return false;
  }
  return false;
}

int getChannelsUsed()
{
  int result = 0;
  int lastCh = -1;
  for (int i=0; i<MAX_MIXERS; ++i) {
    MixData *md = mixAddress(i);
    if (md->srcRaw == 0) return result;
    if (md->destCh != lastCh) { ++result; lastCh = md->destCh; }
  }
  return result;
}

bool isSourceAvailable(int source)
{
  if (source < 0)
    return false;

  if (source>=MIXSRC_FIRST_INPUT && source<=MIXSRC_LAST_INPUT) {
    return isInputAvailable(source - MIXSRC_FIRST_INPUT);
  }

#if defined(LUA_MODEL_SCRIPTS)
  if (source>=MIXSRC_FIRST_LUA && source<=MIXSRC_LAST_LUA) {
    div_t qr = div(source-MIXSRC_FIRST_LUA, MAX_SCRIPT_OUTPUTS);
    return (qr.rem<scriptInputsOutputs[qr.quot].outputsCount);
  }
#elif defined(LUA_INPUTS)
  if (source>=MIXSRC_FIRST_LUA && source<=MIXSRC_LAST_LUA)
    return false;
#endif

  if (source>=MIXSRC_FIRST_POT && source<=MIXSRC_LAST_POT) {
    return IS_POT_SLIDER_AVAILABLE(POT1+source-MIXSRC_FIRST_POT);
  }

#if defined(PCBX10)
  if ((source>=MIXSRC_S3 && source<=MIXSRC_S4) || (source>=MIXSRC_MOUSE1 && source<=MIXSRC_MOUSE2))
    return false;
#endif

  if (source>=MIXSRC_FIRST_SWITCH && source<=MIXSRC_LAST_SWITCH) {
    return SWITCH_EXISTS(source-MIXSRC_FIRST_SWITCH);
  }

#if !defined(HELI)
  if (source>=MIXSRC_CYC1 && source<=MIXSRC_CYC3)
    return false;
#endif

  if (source>=MIXSRC_FIRST_CH && source<=MIXSRC_LAST_CH) {
    return isChannelUsed(source-MIXSRC_FIRST_CH);
  }

  if (source>=MIXSRC_FIRST_LOGICAL_SWITCH && source<=MIXSRC_LAST_LOGICAL_SWITCH) {
    LogicalSwitchData * cs = lswAddress(source-MIXSRC_FIRST_LOGICAL_SWITCH);
    return (cs->func != LS_FUNC_NONE);
  }

#if !defined(GVARS)
  if (source>=MIXSRC_GVAR1 && source<=MIXSRC_LAST_GVAR)
    return false;
#endif

  if (source>=MIXSRC_FIRST_RESERVE && source<=MIXSRC_LAST_RESERVE)
    return false;

  if (source>=MIXSRC_FIRST_TELEM && source<=MIXSRC_LAST_TELEM) {
    div_t qr = div(source-MIXSRC_FIRST_TELEM, 3);
    if (qr.rem == 0)
      return isTelemetryFieldAvailable(qr.quot);
    else
      return isTelemetryFieldComparisonAvailable(qr.quot);
  }

  return true;
}

bool isSourceAvailableInGlobalFunctions(int source)
{
  if (source>=MIXSRC_FIRST_TELEM && source<=MIXSRC_LAST_TELEM) {
    return false;
  }
  return isSourceAvailable(source);
}

bool isSourceAvailableInCustomSwitches(int source)
{
  bool result = isSourceAvailable(source);

#if defined(TELEMETRY_FRSKY)
  if (result && source>=MIXSRC_FIRST_TELEM && source<=MIXSRC_LAST_TELEM) {
    div_t qr = div(source-MIXSRC_FIRST_TELEM, 3);
    result = isTelemetryFieldComparisonAvailable(qr.quot);
  }
#endif

  return result;
}

bool isInputSourceAvailable(int source)
{
  if (source >= MIXSRC_FIRST_POT && source <= MIXSRC_LAST_POT)
    return IS_POT_SLIDER_AVAILABLE(POT1+source-MIXSRC_FIRST_POT);

#if defined(GYRO)
  if (source >= MIXSRC_GYRO1 && source <= MIXSRC_GYRO2)
    return true;
#endif

  if (source >= MIXSRC_Rud && source <= MIXSRC_MAX)
    return true;

  if (source >= MIXSRC_FIRST_TRIM && source <= MIXSRC_LAST_TRIM)
    return true;

  if (source >= MIXSRC_FIRST_SWITCH && source <= MIXSRC_LAST_SWITCH)
    return SWITCH_EXISTS(source - MIXSRC_FIRST_SWITCH);

  if (source >= MIXSRC_FIRST_CH && source <= MIXSRC_LAST_CH)
    return true;

  if (source >= MIXSRC_FIRST_LOGICAL_SWITCH && source <= MIXSRC_LAST_LOGICAL_SWITCH) {
    LogicalSwitchData * cs = lswAddress(source - MIXSRC_SW1);
    return (cs->func != LS_FUNC_NONE);
  }

  if (source >= MIXSRC_FIRST_TRAINER && source <= MIXSRC_LAST_TRAINER)
    return true;

  if (source >= MIXSRC_FIRST_TELEM && source <= MIXSRC_LAST_TELEM) {
    div_t qr = div(source - MIXSRC_FIRST_TELEM, 3);
    return isTelemetryFieldAvailable(qr.quot) && isTelemetryFieldComparisonAvailable(qr.quot);
  }

  return false;
}

enum SwitchContext
{
    LogicalSwitchesContext,
    ModelCustomFunctionsContext,
    GeneralCustomFunctionsContext,
    TimersContext,
    MixesContext
};

bool isLogicalSwitchAvailable(int index)
{
  LogicalSwitchData * lsw = lswAddress(index);
  return (lsw->func != LS_FUNC_NONE);
}

bool isSwitchAvailable(int swtch, SwitchContext context)
{
  bool negative = false;

  if (swtch < 0) {
    if (swtch == -SWSRC_ON || swtch == -SWSRC_ONE) {
      return false;
    }
    negative = true;
    swtch = -swtch;
  }

#if defined(PCBSKY9X)
  if (swtch >= SWSRC_FIRST_SWITCH && swtch <= SWSRC_LAST_SWITCH) {
    UNUSED(negative);
    return true;
  }
#else
  if (swtch >= SWSRC_FIRST_SWITCH && swtch <= SWSRC_LAST_SWITCH) {
    div_t swinfo = switchInfo(swtch);
    if (!SWITCH_EXISTS(swinfo.quot)) {
      return false;
    }
    if (!IS_CONFIG_3POS(swinfo.quot)) {
      if (negative) {
        return false;
      }
      if (swinfo.rem == 1) {
        // mid position not available for 2POS switches
        return false;
      }
    }
    return true;
  }
#endif

#if NUM_XPOTS > 0
  if (swtch >= SWSRC_FIRST_MULTIPOS_SWITCH && swtch <= SWSRC_LAST_MULTIPOS_SWITCH) {
    int index = (swtch - SWSRC_FIRST_MULTIPOS_SWITCH) / XPOTS_MULTIPOS_COUNT;
    if (IS_POT_MULTIPOS(POT1+index)) {
      StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[POT1+index];
      return (calib->count >= ((swtch - SWSRC_FIRST_MULTIPOS_SWITCH) % XPOTS_MULTIPOS_COUNT));
    }
    else {
      return false;
    }
  }
#endif

#if defined(PCBSKY9X) && defined(REVX)
  if (swtch == SWSRC_REa) {
    return false;
  }
#endif

  if (swtch >= SWSRC_FIRST_LOGICAL_SWITCH && swtch <= SWSRC_LAST_LOGICAL_SWITCH) {
    if (context == GeneralCustomFunctionsContext) {
      return false;
    }
    else if (context != LogicalSwitchesContext) {
      return isLogicalSwitchAvailable(swtch - SWSRC_FIRST_LOGICAL_SWITCH);
    }
  }

  if (context != ModelCustomFunctionsContext && context != GeneralCustomFunctionsContext && (swtch == SWSRC_ON || swtch == SWSRC_ONE)) {
    return false;
  }

  if (swtch >= SWSRC_FIRST_FLIGHT_MODE && swtch <= SWSRC_LAST_FLIGHT_MODE) {
    if (context == MixesContext || context == GeneralCustomFunctionsContext) {
      return false;
    }
    else {
      swtch -= SWSRC_FIRST_FLIGHT_MODE;
      if (swtch == 0) {
        return true;
      }
      FlightModeData * fm = flightModeAddress(swtch);
      return (fm->swtch != SWSRC_NONE);
    }
  }

  if (swtch >= SWSRC_FIRST_SENSOR && swtch <= SWSRC_LAST_SENSOR) {
    if (context == GeneralCustomFunctionsContext)
      return false;
    else
      return isTelemetryFieldAvailable(swtch - SWSRC_FIRST_SENSOR);
  }

  return true;
}

bool isSwitchAvailableInLogicalSwitches(int swtch)
{
  return isSwitchAvailable(swtch, LogicalSwitchesContext);
}

bool isSwitchAvailableInCustomFunctions(int swtch)
{
  if (menuHandlers[menuLevel] == menuModelSpecialFunctions)
    return isSwitchAvailable(swtch, ModelCustomFunctionsContext);
  else
    return isSwitchAvailable(swtch, GeneralCustomFunctionsContext);
}

bool isSwitchAvailableInMixes(int swtch)
{
  return isSwitchAvailable(swtch, MixesContext);
}

#if defined(COLORLCD)
bool isSwitch2POSWarningStateAvailable(int state)
{
  return (state != 2); // two pos switch - middle state not available
}
#endif // #if defined(COLORLCD)

bool isSwitchAvailableInTimers(int swtch)
{
  if (swtch >= 0) {
    if (swtch < TMRMODE_COUNT)
      return true;
    else
      swtch -= TMRMODE_COUNT-1;
  }
  else {
    if (swtch > -TMRMODE_COUNT)
      return false;
    else
      swtch += TMRMODE_COUNT-1;
  }

  return isSwitchAvailable(swtch, TimersContext);
}

bool isThrottleSourceAvailable(int source)
{
  if (source >= THROTTLE_SOURCE_FIRST_POT && source < THROTTLE_SOURCE_FIRST_POT+NUM_POTS+NUM_SLIDERS && !IS_POT_SLIDER_AVAILABLE(POT1+source-THROTTLE_SOURCE_FIRST_POT))
    return false;
  else
    return true;
}

bool isLogicalSwitchFunctionAvailable(int function)
{
  return function != LS_FUNC_RANGE;
}

bool isAssignableFunctionAvailable(int function)
{
#if defined(OVERRIDE_CHANNEL_FUNCTION) || defined(GVARS)
  bool modelFunctions = (menuHandlers[menuLevel] == menuModelSpecialFunctions);
#endif

  switch (function) {
    case FUNC_OVERRIDE_CHANNEL:
#if defined(OVERRIDE_CHANNEL_FUNCTION)
      return modelFunctions;
#else
      return false;
#endif
    case FUNC_ADJUST_GVAR:
#if defined(GVARS)
      return modelFunctions;
#else
      return false;
#endif
#if !defined(HAPTIC)
      case FUNC_HAPTIC:
#endif
    case FUNC_RESERVE4:
#if !defined(DANGEROUS_MODULE_FUNCTIONS)
    case FUNC_RANGECHECK:
    case FUNC_BIND:
#endif
#if !defined(LUA)
      case FUNC_PLAY_SCRIPT:
#endif
    case FUNC_RESERVE5:
      return false;

    default:
      return true;
  }
}

bool isSourceAvailableInGlobalResetSpecialFunction(int index)
{
  if (index >= FUNC_RESET_PARAM_FIRST_TELEM)
    return false;
  else
    return isSourceAvailableInResetSpecialFunction(index);
}

bool isSourceAvailableInResetSpecialFunction(int index)
{
  if (index >= FUNC_RESET_PARAM_FIRST_TELEM) {
    TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index-FUNC_RESET_PARAM_FIRST_TELEM];
    return telemetrySensor.isAvailable();
  }
#if TIMERS < 3
    else if (index == FUNC_RESET_TIMER3) {
    return false;
  }
#endif
#if TIMERS < 2
    else if (index == FUNC_RESET_TIMER2) {
    return false;
  }
#endif
  else {
    return true;
  }
}

#if defined(PCBXLITE)
bool isR9MModeAvailable(int mode)
{
#if defined(MODULE_R9M_FLEX_FW)
  return mode >= MODULE_SUBTYPE_R9M_EUPLUS;
#else
  return mode <= MODULE_SUBTYPE_R9M_EU;
#endif
}

#else
bool isR9MModeAvailable(int mode)
{
#if defined(MODULE_R9M_FLEX_FW)
  return mode < MODULE_SUBTYPE_R9M_EUPLUS;
#else
  return true;
#endif
}
#endif

bool isPXX2ChannelsCountAllowed(int channels)
{
  return (channels % 8 == 0);
}

bool isModuleUSingSport(uint8_t moduleBay, uint8_t moduleType)
{
  switch(moduleType) {
    case MODULE_TYPE_NONE:
    case MODULE_TYPE_SBUS:
    case MODULE_TYPE_PPM:
    case MODULE_TYPE_DSM2:
    case MODULE_TYPE_MULTIMODULE:
    case MODULE_TYPE_ACCESS_R9M_LITE:
    case MODULE_TYPE_ACCESS_R9M_LITE_PRO:
      return false;

    case MODULE_TYPE_ACCESS_ISRM:
      if (moduleBay == EXTERNAL_MODULE)
        return false;

#if defined(INTMODULE_USART)
      return false;
#endif

    default:
      return true;
  }
}

#if defined(HARDWARE_INTERNAL_MODULE)
bool isInternalModuleAvailable(int moduleType)
{
  if (moduleType == MODULE_TYPE_NONE)
    return true;

#if defined(PXX1) && defined(INTERNAL_MODULE_PXX1)
  if (moduleType == MODULE_TYPE_PXX_XJT)
    return !isModuleUSingSport(EXTERNAL_MODULE, g_model.moduleData[EXTERNAL_MODULE].type);
#else
  if (moduleType == MODULE_TYPE_PXX_XJT)
    return false;
#endif

#if defined(PXX2)
  if (moduleType == MODULE_TYPE_ACCESS_ISRM)
#if defined(INTMODULE_USART)
    return true;
#else
    return (!isModuleUSingSport(EXTERNAL_MODULE, g_model.moduleData[EXTERNAL_MODULE].type));
#endif
#endif

  return false;
}
#endif

bool isExternalModuleAvailable(int moduleType)
{
#if !defined(PCBXLITE)
  if (moduleType == MODULE_TYPE_PXX_R9M_LITE || moduleType == MODULE_TYPE_ACCESS_R9M_LITE || moduleType == MODULE_TYPE_ACCESS_R9M_LITE_PRO) {
    return false;
  }
#endif
#if !defined(PXX1)
  if (moduleType == MODULE_TYPE_PXX_XJT || moduleType == MODULE_TYPE_PXX_R9M || moduleType == MODULE_TYPE_PXX_R9M_LITE) {
    return false;
  }
#endif
#if !defined(PXX2)
  if (moduleType == MODULE_TYPE_ACCESS_ISRM || moduleType == MODULE_TYPE_ACCESS_R9M || moduleType == MODULE_TYPE_ACCESS_R9M_LITE || moduleType == MODULE_TYPE_ACCESS_R9M_LITE_PRO) {
    return false;
  }
#endif
#if defined(CROSSFIRE)
  if (moduleType == MODULE_TYPE_CROSSFIRE && g_model.moduleData[INTERNAL_MODULE].type != MODULE_TYPE_NONE) {
    return false;
  }
#else
  if (moduleType == MODULE_TYPE_CROSSFIRE) {
    return false;
  }
#endif
#if !defined(DSM2)
  if (moduleType == MODULE_TYPE_DSM2) {
     return false;
  }
#endif
#if !defined(MULTIMODULE)
  if (moduleType == MODULE_TYPE_MULTIMODULE) {
    return false;
  }
#endif
#if defined(HARDWARE_INTERNAL_MODULE)
  if (isModuleUSingSport(EXTERNAL_MODULE, moduleType) && isModuleUSingSport(INTERNAL_MODULE, g_model.moduleData[INTERNAL_MODULE].type))
    return false;
#endif

  return true;
}

bool isRfProtocolAvailable(int protocol)
{
#if defined(CROSSFIRE)
  if (protocol != RF_PROTO_OFF && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_CROSSFIRE) {
    return false;
  }
#endif
#if defined(MODULE_D16_EU_ONLY_SUPPORT)
  if (protocol == RF_PROTO_D8) {
    return false;
  }
#endif
#if defined(PCBTARANIS) || defined(PCBHORUS)
  if (protocol != RF_PROTO_OFF && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PXX_R9M) {
    return false;
  }
  if (protocol != RF_PROTO_OFF && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_ACCESS_R9M) {
    return false;
  }
#endif

  return true;
}

bool isTelemetryProtocolAvailable(int protocol)
{
#if defined(PCBTARANIS)
  if (protocol == PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY && g_eeGeneral.serial2Mode != UART_MODE_TELEMETRY) {
    return false;
  }
#endif

  if (protocol== PROTOCOL_TELEMETRY_CROSSFIRE) {
    return false;
  }

#if !defined(MULTIMODULE)
  if (protocol == PROTOCOL_TELEMETRY_SPEKTRUM || protocol == PROTOCOL_TELEMETRY_FLYSKY_IBUS || protocol == PROTOCOL_TELEMETRY_MULTIMODULE) {
    return false;
  }
#endif

#if defined(PCBHORUS)
  if (protocol == PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY) {
    return false;
  }
#endif

  return true;
}

#if defined(PCBHORUS)
bool isTrainerModeAvailable(int mode)
{
  return true;
}
#elif defined(PCBX9E)
bool isTrainerModeAvailable(int mode)
{
  if (IS_EXTERNAL_MODULE_ENABLED() && (mode == TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE || mode == TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE))
    return false;
#if defined(USEHORUSBT)
  else if (mode == TRAINER_MODE_MASTER_BATTERY_COMPARTMENT)
#else
  else if (mode == TRAINER_MODE_MASTER_BLUETOOTH || mode == TRAINER_MODE_MASTER_BATTERY_COMPARTMENT || mode == TRAINER_MODE_SLAVE_BLUETOOTH)
#endif
    return false;
  else
    return true;
}
#elif defined(PCBX9D) || defined(PCBX9DP) || defined(PCBX9E)
bool isTrainerModeAvailable(int mode)
{
  if (IS_EXTERNAL_MODULE_ENABLED() && (mode == TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE || mode == TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE))
    return false;
  else
    return true;
}
#elif defined(PCBX7) || defined(PCBXLITES) || defined(PCBX9LITE)
bool isTrainerModeAvailable(int mode)
{
  if (IS_EXTERNAL_MODULE_ENABLED() && (mode == TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE || mode == TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE))
    return false;
  else if (mode == TRAINER_MODE_MASTER_BATTERY_COMPARTMENT)
    return false;
#if defined(BLUETOOTH)
  else if (g_eeGeneral.bluetoothMode != BLUETOOTH_TRAINER && (mode == TRAINER_MODE_MASTER_BLUETOOTH || mode == TRAINER_MODE_SLAVE_BLUETOOTH))
    return false;
#endif
  else
    return true;
}
#elif defined(PCBXLITES)
bool isTrainerModeAvailable(int mode)
{
  if (mode == TRAINER_MODE_MASTER_TRAINER_JACK || mode == TRAINER_MODE_SLAVE)
    return true;
  else if (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER && (mode == TRAINER_MODE_MASTER_BLUETOOTH || mode == TRAINER_MODE_SLAVE_BLUETOOTH))
    return true;
  else
    return false;
}
#elif defined(PCBXLITE)
bool isTrainerModeAvailable(int mode)
{
  if (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER && (mode == TRAINER_MODE_MASTER_BLUETOOTH || mode == TRAINER_MODE_SLAVE_BLUETOOTH))
    return true;
  else
    return false;
}
#endif

bool modelHasNotes()
{
  char filename[sizeof(MODELS_PATH)+1+sizeof(g_model.header.name)+sizeof(TEXT_EXT)] = MODELS_PATH "/";
  char *buf = strcat_currentmodelname(&filename[sizeof(MODELS_PATH)]);
  strcpy(buf, TEXT_EXT);
  if (isFileAvailable(filename)) {
    return true;
  }

#if !defined(EEPROM)
  buf = strAppendFilename(&filename[sizeof(MODELS_PATH)], g_eeGeneral.currModelFilename, LEN_MODEL_FILENAME);
  strcpy(buf, TEXT_EXT);
  if (isFileAvailable(filename)) {
    return true;
  }
#endif

  return false;
}

int getFirstAvailable(int min, int max, IsValueAvailable isValueAvailable)
{
  int retval = 0;
  for (int i = min; i <= max; i++) {
    if (isValueAvailable(i)) {
      retval = i;
      break;
    }
  }
  return retval;
}
#if defined(MULTIMODULE)
// Third row is number of subtypes -1 (max valid subtype)
#define NO_SUBTYPE  nullptr

// Table is designed to be shared with companion multi.cpp

// The subtype and options strings are only referenced here, so
// define them here to avoid duplication in all language files
// Also since these strings are ARM only and likely stay ARM only
// we don't need the special eeprom/flash string handling, just define them as
// local strings

const char STR_SUBTYPE_FLYSKY[] = 	"\004""Std\0""V9x9""V6x6""V912""CX20";
const char STR_SUBTYPE_HUBSAN[] = 	"\005"" H107"" H301"" H501";
const char STR_SUBTYPE_FRSKY[] =  	"\007""D16\0   ""D8\0    ""D16 8ch""V8\0    ""LBT(EU)""LBT 8ch";
const char STR_SUBTYPE_HISKY[] =  	"\005""Std\0 ""HK310";
const char STR_SUBTYPE_V2X2[] =   	"\006""Std\0  ""JXD506";
const char STR_SUBTYPE_DSM[] =    	"\006""2 22ms""2 11ms""X 22ms""X 11ms";
const char STR_SUBTYPE_YD717[] =  	"\007""Std\0   ""SkyWlkr""Syma X4""XINXUN\0""NIHUI\0 ";
const char STR_SUBTYPE_KN[] =     	"\006""WLtoys""FeiLun";
const char STR_SUBTYPE_SYMAX[] =  	"\003""Std""X5C";
const char STR_SUBTYPE_SLT[] =    	"\006""V1_6ch""V2_8ch""Q100\0 ""Q200\0 ""MR100\0";
const char STR_SUBTYPE_CX10[] =   	"\007""Green\0 ""Blue\0  ""DM007\0 ""-\0     ""JC3015a""JC3015b""MK33041";
const char STR_SUBTYPE_CG023[] =  	"\005""Std\0 ""YD829";
const char STR_SUBTYPE_BAYANG[] = 	"\007"" Std\0  "" H8S3D\0"" X16 AH""IRDrone";
const char STR_SUBTYPE_MT99[] =   	"\006"" MT99\0"" H7\0  "" YZ\0  "" LS\0  "" FY805";
const char STR_SUBTYPE_MJXQ[] =   	"\005""WLH08""X600\0""X800\0""H26D\0""E010\0""H26WH";
const char STR_SUBTYPE_FY326[] =  	"\005""Std\0 ""FY319";
const char STR_SUBTYPE_HONTAI[] = 	"\007"" Std\0  ""JJRC X1"" X5C1\0 "" FQ_951";
const char STR_SUBTYPE_AFHDS2A[] =	"\010""PWM,IBUS""PPM,IBUS""PWM,SBUS""PPM,SBUS";
const char STR_SUBTYPE_Q2X2[] =   	"\004""Q222""Q242""Q282";
const char STR_SUBTYPE_WK2x01[] = 	"\006""WK2801""WK2401""W6_5_1""W6_6_1""W6_HeL""W6_HeI";
const char STR_SUBTYPE_Q303[] =   	"\006""Std\0  ""CX35\0 ""CX10D\0""CX10WD";
const char STR_SUBTYPE_CABELL[] = 	"\007""V3\0    ""V3 Telm""-\0     ""-\0     ""-\0     ""-\0     ""F-Safe\0""Unbind\0";
const char STR_SUBTYPE_H83D[] =   	"\007""Std\0   ""H20H\0  ""H20Mini""H30Mini";
const char STR_SUBTYPE_CORONA[] = 	"\006"" V1\0  "" V2\0  "" FD V3";
const char STR_SUBTYPE_HITEC[] =  	"\007""Optima\0""Opt Hub""Minima\0";
const char STR_SUBTYPE_E01X[] =  	"\004""E012""E015";

const mm_protocol_definition multi_protocols[] = {

  {MM_RF_PROTO_FLYSKY,     4, false,      STR_SUBTYPE_FLYSKY,  nullptr},
  {MM_RF_PROTO_HUBSAN,     2, false,      STR_SUBTYPE_HUBSAN,  STR_MULTI_VIDFREQ},
  {MM_RF_PROTO_FRSKY,      5, false,      STR_SUBTYPE_FRSKY,   STR_MULTI_RFTUNE},
  {MM_RF_PROTO_HISKY,      1, false,      STR_SUBTYPE_HISKY,   nullptr},
  {MM_RF_PROTO_V2X2,       1, false,      STR_SUBTYPE_V2X2,    nullptr},
  {MM_RF_PROTO_DSM2,       3, false,      STR_SUBTYPE_DSM,     nullptr},
  {MM_RF_PROTO_YD717,      4, false,      STR_SUBTYPE_YD717,   nullptr},
  {MM_RF_PROTO_KN,         1, false,      STR_SUBTYPE_KN,      nullptr},
  {MM_RF_PROTO_SYMAX,      1, false,      STR_SUBTYPE_SYMAX,   nullptr},
  {MM_RF_PROTO_SLT,        4, false,      STR_SUBTYPE_SLT,     nullptr},
  {MM_RF_PROTO_CX10,       6, false,      STR_SUBTYPE_CX10,    nullptr},
  {MM_RF_PROTO_CG023,      1, false,      STR_SUBTYPE_CG023,   nullptr},
  {MM_RF_PROTO_BAYANG,     3, false,      STR_SUBTYPE_BAYANG,  STR_MULTI_TELEMETRY},
  {MM_RF_PROTO_MT99XX,     4, false,      STR_SUBTYPE_MT99,    nullptr},
  {MM_RF_PROTO_MJXQ,       5, false,      STR_SUBTYPE_MJXQ,    nullptr},
  {MM_RF_PROTO_FY326,      1, false,      STR_SUBTYPE_FY326,   nullptr},
  {MM_RF_PROTO_SFHSS,      0, true,       NO_SUBTYPE,          STR_MULTI_RFTUNE},
  {MM_RF_PROTO_HONTAI,     3, false,      STR_SUBTYPE_HONTAI,  nullptr},
  {MM_RF_PROTO_OLRS,       0, false,      NO_SUBTYPE,          STR_MULTI_RFPOWER},
  {MM_RF_PROTO_FS_AFHDS2A, 3, true,       STR_SUBTYPE_AFHDS2A, STR_MULTI_SERVOFREQ},
  {MM_RF_PROTO_Q2X2,       2, false,      STR_SUBTYPE_Q2X2,    nullptr},
  {MM_RF_PROTO_WK_2X01,    5, false,      STR_SUBTYPE_WK2x01,  nullptr},
  {MM_RF_PROTO_Q303,       3, false,      STR_SUBTYPE_Q303,    nullptr},
  {MM_RF_PROTO_CABELL,     7, false,      STR_SUBTYPE_CABELL,  STR_MULTI_OPTION},
  {MM_RF_PROTO_H83D,       3, false,      STR_SUBTYPE_H83D,    nullptr},
  {MM_RF_PROTO_CORONA,     2, false,      STR_SUBTYPE_CORONA,  STR_MULTI_RFTUNE},
  {MM_RF_PROTO_HITEC,      2, false,      STR_SUBTYPE_HITEC,   STR_MULTI_RFTUNE},
  {MM_RF_PROTO_E01X,       1, false,      STR_SUBTYPE_E01X,    nullptr},
  {MM_RF_CUSTOM_SELECTED,  7, true,       NO_SUBTYPE,          STR_MULTI_OPTION},

  // Sentinel and default for protocols not listed above (MM_RF_CUSTOM is 0xff)
  {0xfe,                   0, false,      NO_SUBTYPE,          nullptr}
};

#undef NO_SUBTYPE

const mm_protocol_definition *getMultiProtocolDefinition (uint8_t protocol)
{
  const mm_protocol_definition *pdef;
  for (pdef = multi_protocols; pdef->protocol != 0xfe; pdef++) {
    if (pdef->protocol == protocol)
      return pdef;
  }
  // Return the empty last protocol
  return pdef;
}
#endif

void editStickHardwareSettings(coord_t x, coord_t y, int idx, event_t event, LcdFlags flags)
{
  lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, idx+1, 0);
  if (ZEXIST(g_eeGeneral.anaNames[idx]) || (flags && s_editMode > 0))
    editName(x, y, g_eeGeneral.anaNames[idx], LEN_ANA_NAME, event, flags);
  else
    lcdDrawMMM(x, y, flags);
}
