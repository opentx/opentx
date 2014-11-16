#include <algorithm>
#include "helpers.h"
#include "th9xeeprom.h"

RawSwitch th9xToSwitch(int8_t sw)
{
  if (sw == 0)
    return RawSwitch(SWITCH_TYPE_NONE);
  else if (sw <= 9)
    return RawSwitch(SWITCH_TYPE_SWITCH, sw);
  else
    return RawSwitch(SWITCH_TYPE_VIRTUAL, sw > 0 ? sw-9 : sw+9);
}

t_Th9xTrainerMix::t_Th9xTrainerMix()
{
  memset(this, 0, sizeof(t_Th9xTrainerMix));
}

t_Th9xTrainerMix::operator TrainerMix()
{
  TrainerMix c9x;
  c9x.src = srcChn;
  c9x.swtch = th9xToSwitch(swtch);
  c9x.weight = (25 * studWeight) / 8;
  c9x.mode = mode;
  return c9x;
}

t_Th9xTrainerData::t_Th9xTrainerData()
{
  memset(this, 0, sizeof(t_Th9xTrainerData));
}

t_Th9xTrainerData::operator TrainerData ()
{
  TrainerData c9x;
  for (int i=0; i<NUM_STICKS; i++) {
    c9x.calib[i] = calib[i];
    c9x.mix[i] = mix[i];
  }
  return c9x;
}

t_Th9xGeneral::t_Th9xGeneral()
{
  memset(this, 0, sizeof(t_Th9xGeneral));
}

Th9xGeneral::operator GeneralSettings ()
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
  // result.lightSw = th9xToSwitch(lightSw);
  result.trainer = trainer;
  result.view = view;
  result.disableThrottleWarning = disableThrottleWarning;
  result.switchWarning = disableSwitchWarning ? 0 : -1;
  result.disableMemoryWarning = disableMemoryWarning;
  switch (beeperVal) {
    case 0:
      result.beeperMode = GeneralSettings::BEEPER_QUIET;
      break;
    case 1:
      result.beeperMode = GeneralSettings::BEEPER_NOKEYS;
      break;
    case 2:
      result.beeperMode = GeneralSettings::BEEPER_ALL;
      break;
    case 3:
      result.beeperMode = GeneralSettings::BEEPER_ALL;
      result.beeperLength = 2;
  }
  result.stickMode = stickMode;
  result.inactivityTimer = inactivityMin;
  return result;
}


t_Th9xExpoData::t_Th9xExpoData()
{
  memset(this, 0, sizeof(t_Th9xExpoData));
}

t_Th9xExpoData::operator ExpoData ()
{
  ExpoData c9x;
  if (exp5) {
    c9x.curve.type = CurveReference::CURVE_REF_EXPO;
    c9x.curve.value = exp5;
  }
  else if (curve > 6) {
    c9x.curve.type = CurveReference::CURVE_REF_CUSTOM;
    c9x.curve.value = curve - 6;
  }
  else if (curve > 0) {
    c9x.curve.type = CurveReference::CURVE_REF_FUNC;
    c9x.curve.value = curve;
  }
  c9x.mode = mode3;
  c9x.weight = weight6;
  c9x.chn = chn;
  c9x.swtch = th9xToSwitch(drSw);
  return c9x;
}

t_Th9xLimitData::t_Th9xLimitData()
{
  memset(this, 0, sizeof(t_Th9xLimitData));
}

t_Th9xLimitData::operator LimitData ()
{
  LimitData c9x;
  c9x.min = 10*(min-100);
  c9x.max = 10*(max+100);
  c9x.revert = revert;
  c9x.offset = offset;
  return c9x;
}

t_Th9xMixData::t_Th9xMixData()
{
  memset(this, 0, sizeof(t_Th9xMixData));
}

t_Th9xMixData::operator MixData ()
{
  MixData c9x;
  c9x.destCh = destCh;
  if (srcRaw < 7)
    c9x.srcRaw = RawSource(SOURCE_TYPE_STICK, srcRaw);
  else if (srcRaw < 10)
    c9x.srcRaw = RawSource(SOURCE_TYPE_NONE); // TODO
  else if (srcRaw == 10)
    c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
  else if (srcRaw == 11)
    c9x.srcRaw = RawSource(SOURCE_TYPE_NONE); // TODO CUR
  else if (srcRaw < 24)
    c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-12);
  else /* always true if (srcRaw < 32) */
    c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-24);
  c9x.weight = weight;
  c9x.swtch = th9xToSwitch(swtch);
  if (curve > 6) {
    c9x.curve.type = CurveReference::CURVE_REF_CUSTOM;
    c9x.curve.value = curve - 6;
  }
  else if (curve > 0) {
    c9x.curve.type = CurveReference::CURVE_REF_FUNC;
    c9x.curve.value = curve;
  }
  c9x.speedUp = speedUp;
  c9x.speedDown = speedDown;
  c9x.mltpx = (MltpxValue)mixMode;
  return c9x;
}

t_Th9xLogicalSwitchData::operator LogicalSwitchData ()
{
  LogicalSwitchData c9x;
  c9x.func = opCmp;
  c9x.val1 = val1;
  c9x.val2 = val2;

  if ((c9x.func >= LS_FN_VPOS && c9x.func <= LS_FN_ANEG) || c9x.func >= LS_FN_EQUAL) {
    c9x.val1 = toSource(val1).toValue();
  }

  if (c9x.func >= LS_FN_EQUAL) {
    c9x.val2 = toSource(val2).toValue();
  }

  if (c9x.func >= LS_FN_AND && c9x.func <= LS_FN_XOR) {
    c9x.val1 = th9xToSwitch(val1).toValue();
    c9x.val2 = th9xToSwitch(val2).toValue();
  }

  return c9x;
}

int8_t t_Th9xLogicalSwitchData::fromSource(RawSource source)
{
  int v1 = 0;
  if (source.type == SOURCE_TYPE_STICK)
    v1 = 1+source.index;
  else if (source.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings.push_back(::QObject::tr("th9x on this board doesn't have Rotary Encoders"));
    v1 = 5+source.index;
  }
  else if (source.type == SOURCE_TYPE_MAX)
    v1 = 8;
  /* else if (source.type == SOURCE_TYPE_3POS)
    v1 = 9; */
  else if (source.type == SOURCE_TYPE_CYC)
    v1 = 10+source.index;
  else if (source.type == SOURCE_TYPE_PPM)
    v1 = 13+source.index;
  else if (source.type == SOURCE_TYPE_CH)
    v1 = 21+source.index;
  else if (source.type == SOURCE_TYPE_TELEMETRY)
    v1 = 36+source.index;
  return v1;
}

RawSource t_Th9xLogicalSwitchData::toSource(int8_t value)
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
  /* else if (value == 9) {
    return RawSource(SOURCE_TYPE_3POS);
  } */
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

t_Th9xModelData::t_Th9xModelData()
{
  memset(this, 0, sizeof(t_Th9xModelData));
}

t_Th9xModelData::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMString(c9x.name, name, sizeof(name));
  /*switch(tmrMode) {
    case 1:
      c9x.timers[0].mode = TMRMODE_ABS;
      break;
    case 2:
      c9x.timers[0].mode = TMRMODE_THs;
      break;
    case 3:
      c9x.timers[0].mode = TMRMODE_THp;
      break;
    default:
      c9x.timers[0].mode = TMRMODE_OFF;
      break;
  }*/
  // c9x.timers[0].dir = tmrDir;
  c9x.timers[0].val = tmrVal;
  /*c9x.protocol = (Protocol)protocol;
  c9x.moduleData[0].channelsCount = 8 + 2 * ppmNCH;
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.customFn[0].func = FuncTrims2Offsets;
  if (trimSw) {
    c9x.customFn[0].swtch = trimSw;
  }
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.swashRingData.invertELE = swashInvertELE;
  c9x.swashRingData.invertAIL = swashInvertAIL;
  c9x.swashRingData.invertCOL = swashInvertCOL;
  c9x.swashRingData.type = swashType;
  c9x.swashRingData.collectiveSource = swashCollectiveSource;
  c9x.swashRingData.value = swashRingValue;*/
  for (int i=0; i<TH9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<TH9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoTab[i];
  for (int i=0; i<TH9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<NUM_STICKS; i++)
    c9x.flightModeData[0].trim[i] = trimData[i].itrim;
  for (int i=0; i<TH9X_MAX_CURVES3; i++) {
    c9x.curves[i].count = 3;
    for (int j=0; j<3; j++) {
      c9x.curves[i].points[j].x = -100 + 100*i;
      c9x.curves[i].points[j].y = curves3[i][j];
    }
  }
  for (int i=0; i<TH9X_MAX_CURVES5; i++) {
    c9x.curves[i+TH9X_MAX_CURVES3].count = 5;
    for (int j=0; j<5; j++) {
      c9x.curves[i+TH9X_MAX_CURVES3].points[j].x = -100 + 50*i;
      c9x.curves[i+TH9X_MAX_CURVES3].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<TH9X_MAX_CURVES9; i++) {
    c9x.curves[i+TH9X_MAX_CURVES3+TH9X_MAX_CURVES5].count = 5;
    for (int j=0; j<9; j++) {
      c9x.curves[i+TH9X_MAX_CURVES3+TH9X_MAX_CURVES5].points[j].x = -100 + 50*i;
      c9x.curves[i+TH9X_MAX_CURVES3+TH9X_MAX_CURVES5].points[j].y = curves9[i][j];
    }
  }

  return c9x;
}

