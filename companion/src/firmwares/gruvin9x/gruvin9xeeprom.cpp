#include <stdlib.h>
#include <algorithm>
#include "helpers.h"
#include "gruvin9xeeprom.h"
#include <QObject>

#define EEPROM_VER       106

extern void getEEPROMZString(char *dst, const char *src, int size);

RawSwitch gruvin9xToSwitch(int8_t sw)
{
  if (sw == 0)
    return RawSwitch(SWITCH_TYPE_NONE);
  else if (sw <= 9)
    return RawSwitch(SWITCH_TYPE_SWITCH, sw);
  else
    return RawSwitch(SWITCH_TYPE_VIRTUAL, sw > 0 ? sw-9 : sw+9);
}

t_Gruvin9xTrainerMix_v103::operator TrainerMix()
{
  TrainerMix c9x;
  c9x.src = srcChn;
  c9x.swtch = gruvin9xToSwitch(swtch);
  c9x.weight = (25 * studWeight) / 8;
  c9x.mode = mode;
  return c9x;
}

t_Gruvin9xTrainerMix_v104::operator TrainerMix()
{
  TrainerMix c9x;
  c9x.src = srcChn;
  c9x.weight = studWeight;
  c9x.mode = mode;
  return c9x;
}

t_Gruvin9xTrainerData_v103::operator TrainerData ()
{
  TrainerData c9x;
  for (int i=0; i<NUM_STICKS; i++) {
    c9x.calib[i] = calib[i];
    c9x.mix[i] = mix[i];
  }
  return c9x;
}

t_Gruvin9xTrainerData_v104::operator TrainerData ()
{
  TrainerData c9x;
  for (int i=0; i<NUM_STICKS; i++) {
    c9x.calib[i] = calib[i];
    c9x.mix[i] = mix[i];
  }
  return c9x;
}

Gruvin9xGeneral_v103::operator GeneralSettings ()
{
  GeneralSettings result;

  for (int i=0; i<NUM_STICKS+BOARD_9X_NUM_POTS; i++) {
    result.calibMid[i] = calibMid[i];
    result.calibSpanNeg[i] = calibSpanNeg[i];
    result.calibSpanPos[i] = calibSpanPos[i];
  }

  result.currModel = currModel;
  result.contrast = contrast;
  result.vBatWarn = vBatWarn;
  result.vBatCalib = vBatCalib;

  result.backlightMode = 0;
  if (lightSw == 22) {
    result.backlightMode = 4;
  }
  else if (lightAutoOff) {
    result.backlightMode |= 1;
    result.backlightDelay = lightAutoOff;
  }

  result.trainer = trainer;
  result.view = view;
  result.disableThrottleWarning = disableThrottleWarning;
  result.switchWarning = switchWarning;

  switch (beeperVal) {
    case 0:
      result.beeperMode = GeneralSettings::BEEPER_QUIET;
      break;
    case 1:
      result.beeperMode = GeneralSettings::BEEPER_NOKEYS;
      break;
    default:
      result.beeperMode = GeneralSettings::BEEPER_ALL;
      result.beeperLength = beeperVal - 4;
      break;
  }

  result.disableMemoryWarning = disableMemoryWarning;
  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = stickMode;
  result.inactivityTimer = inactivityTimer;
  result.minuteBeep = minuteBeep;
  result.preBeep = preBeep;
  result.flashBeep = flashBeep;
  result.splashMode = disableSplashScreen;
  result.enableTelemetryAlarm = enableTelemetryAlarm;
  result.templateSetup = templateSetup;
  result.PPM_Multiplier = PPM_Multiplier;
  // TODO frskyRssiAlarms[2];
  return result;
}

Gruvin9xGeneral_v104::operator GeneralSettings ()
{
  GeneralSettings result;

  for (int i=0; i<NUM_STICKS+BOARD_9X_NUM_POTS; i++) {
    result.calibMid[i] = calibMid[i];
    result.calibSpanNeg[i] = calibSpanNeg[i];
    result.calibSpanPos[i] = calibSpanPos[i];
  }

  result.currModel = currModel;
  result.contrast = contrast;
  result.vBatWarn = vBatWarn;
  result.vBatCalib = vBatCalib;

  result.backlightMode = 0;
  if (lightSw == 22) {
    result.backlightMode = 4;
  }
  else if (lightAutoOff) {
    result.backlightMode |= 1;
    result.backlightDelay = lightAutoOff;
  }

  result.trainer = trainer;
  result.view = view;
  result.disableThrottleWarning = disableThrottleWarning;
  result.switchWarning = switchWarning;
  result.beeperMode = (GeneralSettings::BeeperMode)beeperVal;
  result.disableMemoryWarning = disableMemoryWarning;
  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = stickMode;
  result.inactivityTimer = inactivityTimer;
  result.minuteBeep = minuteBeep;
  result.preBeep = preBeep;
  result.flashBeep = flashBeep;
  result.splashMode = disableSplashScreen;
  result.enableTelemetryAlarm = enableTelemetryAlarm;
  result.templateSetup = templateSetup;
  result.PPM_Multiplier = PPM_Multiplier;
  // TODO frskyRssiAlarms[2];
  return result;
}

t_Gruvin9xExpoData::t_Gruvin9xExpoData()
{
  memset(this, 0, sizeof(t_Gruvin9xExpoData));
}

t_Gruvin9xExpoData::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;
  c9x.swtch = gruvin9xToSwitch(swtch);
  if (negPhase) {
    c9x.phases= 1 << (phase -1);
  } else if (phase==0) {
    c9x.phases=0;
  } else {
    c9x.phases=63;
    c9x.phases &= ~(1 << (phase -1));
  }  
  c9x.weight = weight;
  if (expo) {
    c9x.curve.type = CurveReference::CURVE_REF_EXPO;
    c9x.curve.value = expo;
  }
  else if (curve > 6) {
    c9x.curve.type = CurveReference::CURVE_REF_CUSTOM;
    c9x.curve.value = curve - 6;
  }
  else if (curve > 0) {
    c9x.curve.type = CurveReference::CURVE_REF_FUNC;
    c9x.curve.value = curve;
  }
  return c9x;
}

t_Gruvin9xLimitData::t_Gruvin9xLimitData()
{
  memset(this, 0, sizeof(t_Gruvin9xLimitData));
}

t_Gruvin9xLimitData::operator LimitData ()
{
  LimitData c9x;
  c9x.min = 10*(min-100);
  c9x.max = 10*(max+100);
  c9x.revert = revert;
  c9x.offset = offset;
  return c9x;
}

t_Gruvin9xMixData::t_Gruvin9xMixData()
{
  memset(this, 0, sizeof(t_Gruvin9xMixData));
}

t_Gruvin9xMixData::operator MixData ()
{
  MixData c9x;
  c9x.destCh = destCh;
  c9x.weight = weight;
  c9x.swtch = gruvin9xToSwitch(swtch);

  if (srcRaw == 0) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_NONE);
  }
  else if (srcRaw <= 7) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_STICK, srcRaw-1);
  }
  else if (srcRaw == 8) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
  }
  else if (srcRaw == 9) {
    if (swtch < 0) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, -swtch - 1);
      c9x.weight = -weight;
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, swtch - 1);
    }
    if (mltpx != MLTPX_REP)
      c9x.swtch = RawSwitch(SWITCH_TYPE_NONE);
  }
  else if (srcRaw <= 12) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-10);
  }
  else if (srcRaw <= 20) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-13);
  }
  else {
    c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-21);
  }

  c9x.delayUp = delayUp;
  c9x.delayDown = delayDown;
  c9x.speedUp = speedUp;
  c9x.speedDown = speedDown;
  c9x.carryTrim = carryTrim;
  c9x.mltpx = (MltpxValue)mltpx;
  c9x.mixWarn = mixWarn;
  c9x.sOffset = sOffset;

  if (curve > 6) {
    c9x.curve.type = CurveReference::CURVE_REF_CUSTOM;
    c9x.curve.value = curve - 6;
  }
  else if (curve > 0) {
    c9x.curve.type = CurveReference::CURVE_REF_FUNC;
    c9x.curve.value = curve;
  }

  return c9x;
}

RawSource gruvin9xToSource(int8_t value)
{
  if (value == 0) {
    return RawSource(SOURCE_TYPE_NONE);
  }
  else if (value <= 7) {
    return RawSource(SOURCE_TYPE_STICK, value - 1);
  }
  else if (value == 8) {
    return RawSource(SOURCE_TYPE_MAX);
  }
  else if (value == 9) {
    return RawSource(SOURCE_TYPE_MAX);
  }
  else if (value <= 12) {
    return RawSource(SOURCE_TYPE_CYC, value-10);
  }
  else if (value <= 20) {
    return RawSource(SOURCE_TYPE_PPM, value-13);
  }
  else if (value <= 36) {
    return RawSource(SOURCE_TYPE_CH, value-21);
  }
  else {
    return RawSource(SOURCE_TYPE_TELEMETRY, value-36);
  }
}

Gruvin9xLogicalSwitchData::operator LogicalSwitchData ()
{
  LogicalSwitchData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= LS_FN_VPOS && c9x.func <= LS_FN_ANEG) || c9x.func >= LS_FN_EQUAL) {
    c9x.val1 = gruvin9xToSource(v1).toValue();
  }

  if (c9x.func >= LS_FN_EQUAL) {
    c9x.val2 = gruvin9xToSource(v2).toValue();
  }

  if (c9x.func >= LS_FN_AND && c9x.func <= LS_FN_XOR) {
    c9x.val1 = gruvin9xToSwitch(v1).toValue();
    c9x.val2 = gruvin9xToSwitch(v2).toValue();
  }

  return c9x;
}

Gruvin9xCustomFunctionData::operator CustomFunctionData ()
{
  CustomFunctionData c9x;
  c9x.swtch = gruvin9xToSwitch(swtch);
  c9x.func = (AssignFunc)(func + G9X_NUM_CHNOUT);
  return c9x;
}

t_Gruvin9xSwashRingData::t_Gruvin9xSwashRingData()
{
  memset(this, 0, sizeof(t_Gruvin9xSwashRingData));
}

t_Gruvin9xSwashRingData::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.invertELE = invertELE;
  c9x.invertAIL = invertAIL;
  c9x.invertCOL = invertCOL;
  c9x.type = type;
  c9x.collectiveSource = gruvin9xToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Gruvin9xFlightModeData_v102::operator FlightModeData ()
{
  FlightModeData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = trim[i];
  c9x.swtch = gruvin9xToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  return c9x;
}

t_Gruvin9xFlightModeData_v106::operator FlightModeData ()
{
  FlightModeData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = (((int16_t)trim[i]) << 2) + ((trim_ext >> (2*i)) & 0x03);
  c9x.swtch = gruvin9xToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  return c9x;
}

extern RawSwitch getEr9xTimerMode(int mode);

t_Gruvin9xTimerData::operator TimerData ()
{
  TimerData c9x;
  c9x.mode = getEr9xTimerMode(mode);
  c9x.val = val;
  return c9x;
}

t_Gruvin9xFrSkyChannelData::t_Gruvin9xFrSkyChannelData()
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyChannelData));
}

t_Gruvin9xFrSkyChannelData::operator FrSkyChannelData ()
{
  FrSkyChannelData c9x;
  c9x.ratio = ratio;
  c9x.alarms[0].value = alarms_value[0];
  c9x.alarms[0].level =  alarms_level & 3;
  c9x.alarms[0].greater = alarms_greater & 1;
  c9x.alarms[1].value = alarms_value[1];
  c9x.alarms[1].level =  (alarms_level >> 2) & 3;
  c9x.alarms[1].greater = (alarms_greater >> 1) & 1;
  c9x.type = type;
  return c9x;
}

t_Gruvin9xFrSkyData::t_Gruvin9xFrSkyData()
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyData));
}

t_Gruvin9xFrSkyData::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  return c9x;
}

t_Gruvin9xModelData_v102::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMString(c9x.name, name, sizeof(name));
  c9x.timers[0] = timer1;
  c9x.timers[1] = timer2;

  switch(protocol) {
    case 1:
      c9x.moduleData[0].protocol = PXX_DJT;
      break;
    case 2:
      c9x.moduleData[0].protocol = DSM2;
      break;
    case 3:
      c9x.moduleData[0].protocol = SILV_A;
      break;
    case 4:
      c9x.moduleData[0].protocol = SILV_B;
      break;
    case 5:
      c9x.moduleData[0].protocol = SILV_C;
      break;
    case 6:
      c9x.moduleData[0].protocol = CTP1009;
      break;
    default:
      c9x.moduleData[0].protocol = PPM;
      break;
  }
  c9x.moduleData[0].channelsCount = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc-2;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  for (int i=0; i<G9X_MAX_FLIGHT_MODES; i++)
    c9x.flightModeData[i] = flightModeData[i];
  for (int i=0; i<G9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<G9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<G9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];

  for (int i=0; i<G9X_MAX_CURVE5; i++) {
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<G9X_MAX_CURVE9; i++) {
    c9x.curves[G9X_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }

  for (int i=0; i<G9X_NUM_CSW; i++)
    c9x.logicalSw[i] = logicalSw[i];
  // for (int i=0; i<G9X_NUM_CHNOUT; i++)
  //  c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  return c9x;
}

t_Gruvin9xModelData_v103::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.timers[0] = timer1;
  c9x.timers[1] = timer2;
  switch(protocol) {
    case 1:
      c9x.moduleData[0].protocol = PXX_DJT;
      break;
    case 2:
      c9x.moduleData[0].protocol = DSM2;
      break;
    case 3:
      c9x.moduleData[0].protocol = SILV_A;
      break;
    case 4:
      c9x.moduleData[0].protocol = SILV_B;
      break;
    case 5:
      c9x.moduleData[0].protocol = SILV_C;
      break;
    case 6:
      c9x.moduleData[0].protocol = CTP1009;
      break;
    default:
      c9x.moduleData[0].protocol = PPM;
      break;
  }
  c9x.moduleData[0].channelsCount = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc-2;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  for (int i=0; i<G9X_MAX_FLIGHT_MODES; i++)
    c9x.flightModeData[i] = flightModeData[i];
  for (int i=0; i<G9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<G9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<G9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];

  for (int i=0; i<G9X_MAX_CURVE5; i++) {
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<G9X_MAX_CURVE9; i++) {
    c9x.curves[G9X_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }

  for (int i=0; i<G9X_NUM_CSW; i++)
    c9x.logicalSw[i] = logicalSw[i];
  // for (int i=0; i<G9X_NUM_CHNOUT; i++)
  //   c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  return c9x;
}

t_Gruvin9xModelData_v105::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.timers[0] = timer1;
  c9x.timers[1] = timer2;
  switch(protocol) {
    case 1:
      c9x.moduleData[0].protocol = PXX_DJT;
      break;
    case 2:
      c9x.moduleData[0].protocol = DSM2;
      break;
    case 3:
      c9x.moduleData[0].protocol = SILV_A;
      break;
    case 4:
      c9x.moduleData[0].protocol = SILV_B;
      break;
    case 5:
      c9x.moduleData[0].protocol = SILV_C;
      break;
    case 6:
      c9x.moduleData[0].protocol = CTP1009;
      break;
    default:
      c9x.moduleData[0].protocol = PPM;
      break;
  }
  c9x.moduleData[0].channelsCount = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc-2;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<G9X_MAX_FLIGHT_MODES; i++) {
    c9x.flightModeData[i] = flightModeData[i];
    for (int j=0; j<NUM_STICKS; j++) {
      if (flightModeData[i].trim[j] > 125) {
        c9x.flightModeData[i].trimRef[j] = 0;
        c9x.flightModeData[i].trim[j] = 0;
      }
      else if (flightModeData[i].trim[j] < -125) {
        c9x.flightModeData[i].trimRef[j] = 129 + flightModeData[i].trim[j];
        if (c9x.flightModeData[i].trimRef[j] >= i)
          c9x.flightModeData[i].trimRef[j] += 1;
        c9x.flightModeData[i].trim[j] = 0;
      }
      else {
        c9x.flightModeData[i].trim[j] += subtrim[j];
      }
    }
  }
  for (int i=0; i<G9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<G9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<G9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];

  for (int i=0; i<G9X_MAX_CURVE5; i++) {
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<G9X_MAX_CURVE9; i++) {
    c9x.curves[G9X_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }

  for (int i=0; i<G9X_NUM_CSW; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<G9X_NUM_FSW; i++)
    c9x.customFn[i] = customFn[i];
  // for (int i=0; i<G9X_NUM_CHNOUT; i++)
  //  c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;

  return c9x;
}

t_Gruvin9xModelData_v106::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.timers[0] = timer1;
  c9x.timers[1] = timer2;
  switch(protocol) {
    case 1:
      c9x.moduleData[0].protocol = PXX_DJT;
      break;
    case 2:
      c9x.moduleData[0].protocol = DSM2;
      break;
    case 3:
      c9x.moduleData[0].protocol = SILV_A;
      break;
    case 4:
      c9x.moduleData[0].protocol = SILV_B;
      break;
    case 5:
      c9x.moduleData[0].protocol = SILV_C;
      break;
    case 6:
      c9x.moduleData[0].protocol = CTP1009;
      break;
    default:
      c9x.moduleData[0].protocol = PPM;
      break;
  }
  c9x.moduleData[0].channelsCount = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc-2;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<G9X_MAX_FLIGHT_MODES; i++) {
    c9x.flightModeData[i] = flightModeData[i];
    for (int j=0; j<NUM_STICKS; j++) {
      if (c9x.flightModeData[i].trim[j] > 500) {
        c9x.flightModeData[i].trimRef[j] = c9x.flightModeData[i].trim[j] - 501;
        if (c9x.flightModeData[i].trimRef[j] >= i)
          c9x.flightModeData[i].trimRef[j] += 1;
        c9x.flightModeData[i].trim[j] = 0;
      }
    }
  }
  for (int i=0; i<G9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<G9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<G9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];

  for (int i=0; i<G9X_MAX_CURVE5; i++) {
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<G9X_MAX_CURVE9; i++) {
    c9x.curves[G9X_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }

  for (int i=0; i<G9X_NUM_CSW; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<G9X_NUM_FSW; i++)
    c9x.customFn[i] = customFn[i];
  // for (int i=0; i<G9X_NUM_CHNOUT; i++)
  //   c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;

  return c9x;
}

