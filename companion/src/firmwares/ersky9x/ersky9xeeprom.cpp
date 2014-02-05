#include <algorithm>
#include "helpers.h"
#include "ersky9xeeprom.h"
#include <QObject>

extern RawSwitch er9xToSwitch(int8_t sw);

TimerMode getErSky9xTimerMode(int mode)
{
  if (mode<4)
    return TimerMode(mode);
  else if (mode < 20)
    return TimerMode(TMRMODE_FIRST_CHPERC+(mode-4));
  else
    return TimerMode(0);
}

RawSwitch ersky9xToSwitch(int8_t sw)
{
  uint8_t swa = abs(sw);
  if (swa == 0)
    return RawSwitch(SWITCH_TYPE_NONE);
  else if (swa <= 9)
    return RawSwitch(SWITCH_TYPE_SWITCH, sw);
  else if (swa <= 33)
    return RawSwitch(SWITCH_TYPE_VIRTUAL, sw > 0 ? sw-9 : sw+9);
  else if (sw == 34)
    return RawSwitch(SWITCH_TYPE_ON);
  else if (sw == -34)
    return RawSwitch(SWITCH_TYPE_OFF);
  else if (swa <= 34+9)
    return RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, sw > 0 ? sw-34 : sw+34);
  else
    return RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, sw > 0 ? sw-34-9 : sw+34+9);
}

t_Ersky9xTrainerMix::t_Ersky9xTrainerMix()
{
  memset(this, 0, sizeof(t_Ersky9xTrainerMix));
}

t_Ersky9xTrainerMix::operator TrainerMix()
{
  TrainerMix c9x;
  c9x.src = srcChn;
  c9x.swtch = er9xToSwitch(swtch);
  c9x.weight = (25 * studWeight) / 8;
  c9x.mode = mode;
  return c9x;
}

t_Ersky9xTrainerData::t_Ersky9xTrainerData()
{
  memset(this, 0, sizeof(t_Ersky9xTrainerData));
}

t_Ersky9xTrainerData::operator TrainerData ()
{
  TrainerData c9x;
  for (int i=0; i<NUM_STICKS; i++) {
    c9x.calib[i] = calib[i];
    c9x.mix[i] = mix[i];
  }
  return c9x;
}

t_Ersky9xGeneral::t_Ersky9xGeneral()
{
  memset(this, 0, sizeof(t_Ersky9xGeneral));
}

Ersky9xGeneral::operator GeneralSettings ()
{
  GeneralSettings result;
  
  result.version = myVers;
  
  for (int i=0; i<NUM_STICKSnPOTS; i++) {
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
  else if (lightOnStickMove) {
    result.backlightMode |= 2;
    result.backlightDelay = lightOnStickMove;
  }

  result.trainer = trainer;
  result.view = std::min((uint8_t)4, view);
  result.disableThrottleWarning = disableThrottleWarning;
  result.switchWarning = disableSwitchWarning ? 0 : -1;
  result.disableMemoryWarning = disableMemoryWarning;

  switch (beeperVal) {
    case 0:
      result.beeperMode = e_quiet;
      break;
    case 1:
      result.beeperMode = e_no_keys;
      break;
    default:
      result.beeperMode = e_all;
      result.beeperLength = beeperVal - 4;
      break;
  }

  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = std::max((uint8_t)0, std::min(stickMode, (uint8_t)3));
  result.inactivityTimer = inactivityTimer + 10;
  result.minuteBeep = minuteBeep;
  result.preBeep = preBeep;
  result.flashBeep = flashBeep;
  result.splashMode = disableSplashScreen;
  result.disablePotScroll=(disablePotScroll==1);
  result.disableBG=(disableBG==1);
  result.filterInput = filterInput;
  result.templateSetup = templateSetup;
  result.PPM_Multiplier = PPM_Multiplier;
  getEEPROMString(result.ownerName, ownerName, sizeof(ownerName));
  result.optrexDisplay = optrexDisplay;
  result.speakerPitch = speakerPitch;
  result.hapticStrength = hapticStrength;
  result.speakerMode = speakerMode;
  result.switchWarningStates = switchWarningStates;
  result.speakerVolume = volume;
  result.backlightBright = bright ;
  result.currentCalib = current_calib;
  return result;
}

t_Ersky9xLimitData::t_Ersky9xLimitData()
{
  memset(this, 0, sizeof(t_Ersky9xLimitData));
}

t_Ersky9xLimitData::operator LimitData ()
{
  LimitData c9x;
  c9x.min = min-100;
  c9x.max = max+100;
  c9x.revert = revert;
  c9x.offset = offset;
  return c9x;
}

t_Ersky9xMixData_v10::t_Ersky9xMixData_v10()
{
  memset(this, 0, sizeof(t_Ersky9xMixData_v10));
}

t_Ersky9xMixData_v10::operator MixData ()
{
  MixData c9x;
  c9x.destCh = destCh;
  c9x.weight = weight;
  c9x.swtch = er9xToSwitch(swtch);

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
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, swtch < -3 ? -swtch-3 : -swtch);
      c9x.weight = -weight;
    }
    else if (swtch > 0) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, swtch > 3 ? swtch-3 : swtch);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
    }
    if (mltpx != MLTPX_REP)
      c9x.swtch = RawSwitch(SWITCH_TYPE_NONE);
  }
  else if (srcRaw == 37) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, 0);
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

  if (curve > 6) {
    c9x.curve.type = CurveReference::CURVE_REF_CUSTOM;
    c9x.curve.value = curve - 6;
  }
  else if (curve > 0) {
    c9x.curve.type = CurveReference::CURVE_REF_FUNC;
    c9x.curve.value = curve;
  }

  c9x.delayUp = delayUp;
  c9x.delayDown = delayDown;
  c9x.speedUp = speedUp;
  c9x.speedDown = speedDown;
  c9x.carryTrim = carryTrim;
  c9x.mltpx = (MltpxValue)mltpx;
  c9x.mixWarn = mixWarn;
  // c9x.enableFmTrim=enableFmTrim;
  c9x.sOffset = sOffset;
  return c9x;
}


t_Ersky9xMixData_v11::t_Ersky9xMixData_v11()
{
  memset(this, 0, sizeof(t_Ersky9xMixData_v11));
}

t_Ersky9xMixData_v11::operator MixData ()
{
  MixData c9x;
  c9x.destCh = destCh;
  c9x.weight = weight;
  c9x.swtch = ersky9xToSwitch(swtch);

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
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, swtch < -3 ? -swtch-3 : -swtch);
      c9x.weight = -weight;
    }
    else if (swtch > 0) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, swtch > 3 ? swtch-3 : swtch);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
    }
    if (mltpx != MLTPX_REP)
      c9x.swtch = RawSwitch(SWITCH_TYPE_NONE);
  }
  else if (srcRaw == 45) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, 0);
  }
  else if (srcRaw > 45) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_GVAR, srcRaw-46);
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

  if (curve > 6) {
    c9x.curve.type = CurveReference::CURVE_REF_CUSTOM;
    c9x.curve.value = curve - 6;
  }
  else if (curve > 0) {
    c9x.curve.type = CurveReference::CURVE_REF_FUNC;
    c9x.curve.value = curve;
  }

  c9x.delayUp = delayUp;
  c9x.delayDown = delayDown;
  c9x.speedUp = speedUp;
  c9x.speedDown = speedDown;
  c9x.carryTrim = carryTrim;
  c9x.mltpx = (MltpxValue)mltpx;
  c9x.mixWarn = mixWarn;
  // c9x.enableFmTrim=enableFmTrim;
  c9x.sOffset = sOffset;
  return c9x;
}

RawSource ersky9xToSource_v10(int8_t value)
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

RawSource ersky9xToSource_v11(int8_t value)
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

Ersky9xCustomSwData_v10::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    c9x.val1 = ersky9xToSource_v10(v1).toValue();
  }

  if (c9x.func >= CS_FN_EQUAL) {
    c9x.val2 = ersky9xToSource_v10(v2).toValue();
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    c9x.val1 = er9xToSwitch(v1).toValue();
    c9x.val2 = er9xToSwitch(v2).toValue();
  }

  return c9x;
}

Ersky9xCustomSwData_v11::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    c9x.val1 = ersky9xToSource_v11(v1).toValue();
  }

  if (c9x.func >= CS_FN_EQUAL) {
    c9x.val2 = ersky9xToSource_v11(v2).toValue();
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    c9x.val1 = er9xToSwitch(v1).toValue();
    c9x.val2 = er9xToSwitch(v2).toValue();
  }

  return c9x;
}

t_Ersky9xSafetySwData_v10::t_Ersky9xSafetySwData_v10()
{
  memset(this, 0, sizeof(t_Ersky9xSafetySwData_v10));
}

t_Ersky9xSafetySwData_v11::t_Ersky9xSafetySwData_v11()
{
  memset(this, 0, sizeof(t_Ersky9xSafetySwData_v11));
}

t_Ersky9xFrSkyChannelData_v10::t_Ersky9xFrSkyChannelData_v10()
{
  memset(this, 0, sizeof(t_Ersky9xFrSkyChannelData_v10));
}

t_Ersky9xFrSkyChannelData_v10::operator FrSkyChannelData ()
{
  FrSkyChannelData c9x;
  c9x.ratio = ratio;
  c9x.alarms[0].value = alarms_value[0];
  c9x.alarms[0].level =  alarms_level & 3;
  c9x.alarms[0].greater = alarms_greater & 1;
  c9x.alarms[1].value = alarms_value[1];
  c9x.alarms[1].level =  (alarms_level >> 2) & 3;
  c9x.alarms[1].greater = (alarms_greater >> 1) & 1;
  if (type==2) {
    c9x.type = 0;
    c9x.multiplier=1;
  } else {
    c9x.type = type;
  }
  return c9x;
}

t_Ersky9xFrSkyChannelData_v11::t_Ersky9xFrSkyChannelData_v11()
{
  memset(this, 0, sizeof(t_Ersky9xFrSkyChannelData_v11));
}

t_Ersky9xFrSkyChannelData_v11::operator FrSkyChannelData ()
{
  FrSkyChannelData c9x;
  c9x.ratio = ratio;
  c9x.alarms[0].value = alarms_value[0];
  c9x.alarms[0].level =  alarms_level & 3;
  c9x.alarms[0].greater = alarms_greater & 1;
  c9x.alarms[1].value = alarms_value[1];
  c9x.alarms[1].level =  (alarms_level >> 2) & 3;
  c9x.alarms[1].greater = (alarms_greater >> 1) & 1;
  if (type==2) {
    c9x.type = 0;
    c9x.multiplier=1;
  } else {
    c9x.type = type;
  }
  return c9x;
}

t_Ersky9xFrSkyData_v10::t_Ersky9xFrSkyData_v10()
{
  memset(this, 0, sizeof(t_Ersky9xFrSkyData_v10));
}

t_Ersky9xFrSkyData_v10::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  return c9x;
}

t_Ersky9xFrSkyData_v11::t_Ersky9xFrSkyData_v11()
{
  memset(this, 0, sizeof(t_Ersky9xFrSkyData_v11));
}

t_Ersky9xFrSkyData_v11::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  return c9x;
}

t_Ersky9xModelData_v10::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMString(c9x.name, name, sizeof(name));
  for (int i=0; i<2; i++) {
    c9x.timers[i].mode = getErSky9xTimerMode(timer[i].tmrModeA);
    c9x.timers[i].dir = timer[i].tmrDir;
    c9x.timers[i].val = timer[i].tmrVal;
  }
  switch(protocol) {
    case 1:
      c9x.moduleData[0].protocol = PXX_DJT;
      break;
    case 2:
      c9x.moduleData[0].protocol = DSM2;
      break;
    case 3:
      c9x.moduleData[0].protocol = PPM16;
      break;
    default:
      c9x.moduleData[0].protocol = PPM;
      break;
  }
  c9x.traineron= traineron;
  // c9x.t2throttle =  t2throttle;
  c9x.moduleData[0].ppmFrameLength=ppmFrameLength;
  c9x.moduleData[0].channelsCount = 8 + 2 * ppmNCH;
  c9x.thrTrim = thrTrim;
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc-2;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.funcSw[0].func = FuncInstantTrim;
  if (trimSw) {
    c9x.funcSw[0].swtch = er9xToSwitch(trimSw);
  }
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.swashRingData.invertELE = swashInvertELE;
  c9x.swashRingData.invertAIL = swashInvertAIL;
  c9x.swashRingData.invertCOL = swashInvertCOL;
  c9x.swashRingData.type = swashType;
  c9x.swashRingData.collectiveSource = ersky9xToSource_v10(swashCollectiveSource);
  c9x.swashRingData.value = swashRingValue;
  for (int i=0; i<ERSKY9X_MAX_MIXERS_V10; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<ERSKY9X_NUM_CHNOUT_V10; i++)
    c9x.limitData[i] = limitData[i];

  // expoData
  int e = 0;
  for (int ch = 0; ch < 4 && e < C9X_MAX_EXPOS; ch++) {
    for (int dr = 0, pos = 0; dr < 3 && e < C9X_MAX_EXPOS; dr++, pos++) {
      if ((dr == 0 && !expoData[ch].drSw1) || (dr == 1 && !expoData[ch].drSw2))
        dr = 2;
      if (dr == 2 && !expoData[ch].expo[0][0][0] && !expoData[ch].expo[0][0][1] && !expoData[ch].expo[0][1][0] && !expoData[ch].expo[0][1][1])
        break;
      if (expoData[ch].drSw1 && !expoData[ch].drSw2) {
        c9x.expoData[e].swtch = er9xToSwitch(dr == 0 ? expoData[ch].drSw1 : 0);
        pos = dr == 0 ? 1 : 0;
      }
      else {
        c9x.expoData[e].swtch = er9xToSwitch(dr == 0 ? -expoData[ch].drSw1 : (dr == 1 ? -expoData[ch].drSw2 : 0));
      }
      c9x.expoData[e].chn = ch;
      if (expoData[ch].expo[pos][0][0]) {
        c9x.expoData[e].curve.type = CurveReference::CURVE_REF_EXPO;
        c9x.expoData[e].curve.value = expoData[ch].expo[pos][0][0];
      }
      c9x.expoData[e].weight = 100 + expoData[ch].expo[pos][1][0];
      if (expoData[ch].expo[pos][0][0] == expoData[ch].expo[pos][0][1] && expoData[ch].expo[pos][1][0] == expoData[ch].expo[pos][1][1]) {
        c9x.expoData[e++].mode = 3;
      }
      else {
        c9x.expoData[e].mode = 2;
        if (e < C9X_MAX_EXPOS - 1) {
          c9x.expoData[e + 1].swtch = c9x.expoData[e].swtch;
          c9x.expoData[++e].chn = ch;
          c9x.expoData[e].mode = 1;
          if (expoData[ch].expo[pos][0][1]) {
            c9x.expoData[e].curve.type = CurveReference::CURVE_REF_EXPO;
            c9x.expoData[e].curve.value = expoData[ch].expo[pos][0][1];
          }
          c9x.expoData[e++].weight = 100 + expoData[ch].expo[pos][1][1];
        }
      }
    }
  }

  for (int i=0; i<NUM_STICKS; i++)
    c9x.phaseData[0].trim[i] = trim[i];

  for (int i=0; i<ERSKY9X_MAX_CURVE5; i++) {
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<ERSKY9X_MAX_CURVE9; i++) {
    c9x.curves[ERSKY9X_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[ERSKY9X_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[ERSKY9X_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }

  for (int i=0; i<ERSKY9X_NUM_CSW_V10; i++)
    c9x.customSw[i] = customSw[i];

  // for (int i=0; i<ERSKY9X_NUM_CHNOUT_V10; i++)
  //   c9x.safetySw[i] = safetySw[i];

  c9x.frsky = frsky;
  c9x.frsky.usrProto=FrSkyUsrProto;
  c9x.frsky.imperial=FrSkyImperial;
  c9x.frsky.FrSkyGpsAlt=FrSkyGpsAlt;
  return c9x;
}

t_Ersky9xModelData_v11::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMString(c9x.name, name, sizeof(name));
  for (int i=0; i<2; i++) {
    c9x.timers[i].mode = getErSky9xTimerMode(timer[i].tmrModeA);
    c9x.timers[i].dir = timer[i].tmrDir;
    c9x.timers[i].val = timer[i].tmrVal;
  }
  switch(protocol) {
    case 1:
      c9x.moduleData[0].protocol = PXX_DJT;
      break;
    case 2:
      c9x.moduleData[0].protocol = DSM2;
      break;
    case 3:
      c9x.moduleData[0].protocol = PPM16;
      break;
    default:
      c9x.moduleData[0].protocol = PPM;
      break;
  }
  c9x.traineron= traineron;
  // c9x.t2throttle =  t2throttle;
  c9x.moduleData[0].ppmFrameLength=ppmFrameLength;
  c9x.moduleData[0].channelsCount = 8 + 2 * ppmNCH;
  c9x.thrTrim = thrTrim;
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc-2;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.funcSw[0].func = FuncInstantTrim;
  if (trimSw) {
    c9x.funcSw[0].swtch = er9xToSwitch(trimSw);
  }
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.swashRingData.invertELE = swashInvertELE;
  c9x.swashRingData.invertAIL = swashInvertAIL;
  c9x.swashRingData.invertCOL = swashInvertCOL;
  c9x.swashRingData.type = swashType;
  c9x.swashRingData.collectiveSource = ersky9xToSource_v11(swashCollectiveSource);
  c9x.swashRingData.value = swashRingValue;
  for (int i=0; i<ERSKY9X_MAX_MIXERS_V11; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<ERSKY9X_NUM_CHNOUT_V11; i++)
    c9x.limitData[i] = limitData[i];

  // expoData
  int e = 0;
  for (int ch = 0; ch < 4 && e < C9X_MAX_EXPOS; ch++) {
    for (int dr = 0, pos = 0; dr < 3 && e < C9X_MAX_EXPOS; dr++, pos++) {
      if ((dr == 0 && !expoData[ch].drSw1) || (dr == 1 && !expoData[ch].drSw2))
        dr = 2;
      if (dr == 2 && !expoData[ch].expo[0][0][0] && !expoData[ch].expo[0][0][1] && !expoData[ch].expo[0][1][0] && !expoData[ch].expo[0][1][1])
        break;
      if (expoData[ch].drSw1 && !expoData[ch].drSw2) {
        c9x.expoData[e].swtch = er9xToSwitch(dr == 0 ? expoData[ch].drSw1 : 0);
        pos = dr == 0 ? 1 : 0;
      }
      else {
        c9x.expoData[e].swtch = er9xToSwitch(dr == 0 ? -expoData[ch].drSw1 : (dr == 1 ? -expoData[ch].drSw2 : 0));
      }
      c9x.expoData[e].chn = ch;
      if (expoData[ch].expo[pos][0][0]) {
        c9x.expoData[e].curve.type = CurveReference::CURVE_REF_EXPO;
        c9x.expoData[e].curve.value = expoData[ch].expo[pos][0][0];
      }
      c9x.expoData[e].weight = 100 + expoData[ch].expo[pos][1][0];
      if (expoData[ch].expo[pos][0][0] == expoData[ch].expo[pos][0][1] && expoData[ch].expo[pos][1][0] == expoData[ch].expo[pos][1][1]) {
        c9x.expoData[e++].mode = 3;
      }
      else {
        c9x.expoData[e].mode = 2;
        if (e < C9X_MAX_EXPOS - 1) {
          c9x.expoData[e + 1].swtch = c9x.expoData[e].swtch;
          c9x.expoData[++e].chn = ch;
          c9x.expoData[e].mode = 1;
          if (expoData[ch].expo[pos][0][1]) {
            c9x.expoData[e].curve.type = CurveReference::CURVE_REF_EXPO;
            c9x.expoData[e].curve.value = expoData[ch].expo[pos][0][1];
          }
          c9x.expoData[e++].weight = 100 + expoData[ch].expo[pos][1][1];
        }
      }
    }
  }

  for (int i=0; i<NUM_STICKS; i++)
    c9x.phaseData[0].trim[i] = trim[i];

  for (int i=0; i<ERSKY9X_MAX_CURVE5; i++) {
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<ERSKY9X_MAX_CURVE9; i++) {
    c9x.curves[ERSKY9X_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[ERSKY9X_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[ERSKY9X_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }

  for (int i=0; i<ERSKY9X_NUM_CSW_V11; i++)
    c9x.customSw[i] = customSw[i];

  // for (int i=0; i<ERSKY9X_NUM_CHNOUT_V11; i++)
  //  c9x.safetySw[i] = safetySw[i];

  c9x.frsky = frsky;
  c9x.frsky.usrProto=FrSkyUsrProto;
  c9x.frsky.imperial=FrSkyImperial;
  c9x.frsky.FrSkyGpsAlt=FrSkyGpsAlt;
  for (int i=0; i<8; i++) {
    c9x.frskyalarms[i].frskyAlarmLimit=frskyAlarms.alarmData[i].frskyAlarmLimit;
    c9x.frskyalarms[i].frskyAlarmSound=frskyAlarms.alarmData[i].frskyAlarmSound;
    c9x.frskyalarms[i].frskyAlarmType=frskyAlarms.alarmData[i].frskyAlarmType;
  }
  for (int i=0; i<8; i++) {
    c9x.customdisplay[i]=customDisplayIndex[i];
  }
  for (int i=0; i<5;i++) {
    c9x.phaseData[0].gvars[i]=gvars[i].gvar;
    c9x.gvsource[i]=gvars[i].gvsource;
  }
  return c9x;
}

