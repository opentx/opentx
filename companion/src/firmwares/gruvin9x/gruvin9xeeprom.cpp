#include <stdlib.h>
#include <algorithm>
#include "helpers.h"
#include "gruvin9xeeprom.h"
#include <QObject>

#define EEPROM_VER       106

extern void setEEPROMZString(char *dst, const char *src, int size);
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

int8_t gruvin9xFromSwitch(const RawSwitch & sw)
{
  switch (sw.type) {
    case SWITCH_TYPE_SWITCH:
      return sw.index;
    case SWITCH_TYPE_VIRTUAL:
      return sw.index > 0 ? (9 + sw.index) : (-9 + sw.index);
    default:
      return 0;
  }
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

t_Gruvin9xTrainerMix_v104::t_Gruvin9xTrainerMix_v104(TrainerMix &c9x)
{
  memset(this, 0, sizeof(t_Gruvin9xTrainerMix_v104));
  srcChn = c9x.src;
  studWeight = c9x.weight;
  mode = c9x.mode;
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

t_Gruvin9xTrainerData_v104::t_Gruvin9xTrainerData_v104(TrainerData &c9x)
{
  memset(this, 0, sizeof(t_Gruvin9xTrainerData_v104));
  for (int i=0; i<NUM_STICKS; i++) {
    calib[i] = c9x.calib[i];
    mix[i] = c9x.mix[i];
  }
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

  result.disableMemoryWarning = disableMemoryWarning;
  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = stickMode;
  result.inactivityTimer = inactivityTimer;
  result.throttleReversed = throttleReversed;
  result.minuteBeep = minuteBeep;
  result.preBeep = preBeep;
  result.flashBeep = flashBeep;
  result.splashMode = disableSplashScreen;
  result.enableTelemetryAlarm = enableTelemetryAlarm;
  result.filterInput = filterInput;
  result.templateSetup = templateSetup;
  result.PPM_Multiplier = PPM_Multiplier;
  // TODO frskyRssiAlarms[2];
  return result;
}

t_Gruvin9xGeneral_v104::t_Gruvin9xGeneral_v104(GeneralSettings &c9x)
{
  memset(this, 0, sizeof(t_Gruvin9xGeneral_v104));

  myVers = EEPROM_VER;

  for (int i=0; i<NUM_STICKS+BOARD_9X_NUM_POTS; i++) {
    calibMid[i] = c9x.calibMid[i];
    calibSpanNeg[i] = c9x.calibSpanNeg[i];
    calibSpanPos[i] = c9x.calibSpanPos[i];
  }

  uint16_t sum = 0;
  for (int i=0; i<12; i++)
    sum += calibMid[i];
  chkSum = sum;

  currModel = c9x.currModel;
  contrast = c9x.contrast;
  vBatWarn = c9x.vBatWarn;
  vBatCalib = c9x.vBatCalib;

  if (c9x.backlightMode == 4)
    lightSw = 22;
  if (c9x.backlightMode & 1)
    lightAutoOff = c9x.backlightDelay;

  trainer = c9x.trainer;
  view = c9x.view;
  disableThrottleWarning = c9x.disableThrottleWarning;
  if (c9x.beeperMode == e_quiet)
    beeperVal = 0;
  else if (c9x.beeperMode < e_all)
    beeperVal = 1;
  else
    beeperVal = std::min(4, c9x.beeperLength + 4);
  switchWarning = c9x.switchWarning;
  disableMemoryWarning = c9x.disableMemoryWarning;
  disableAlarmWarning = c9x.disableAlarmWarning;
  stickMode = c9x.stickMode;
  inactivityTimer = c9x.inactivityTimer;
  throttleReversed = c9x.throttleReversed;
  minuteBeep = c9x.minuteBeep;
  preBeep = c9x.preBeep;
  flashBeep = c9x.flashBeep;
  disableSplashScreen = c9x.splashMode;
  enableTelemetryAlarm = c9x.enableTelemetryAlarm;
  spare = 0;
  filterInput = c9x.filterInput;
  templateSetup = c9x.templateSetup;
  PPM_Multiplier = c9x.PPM_Multiplier;
  // TODO frskyRssiAlarms[2];
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
  result.beeperMode = (BeeperMode)beeperVal;
  result.disableMemoryWarning = disableMemoryWarning;
  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = stickMode;
  result.inactivityTimer = inactivityTimer;
  result.throttleReversed = throttleReversed;
  result.minuteBeep = minuteBeep;
  result.preBeep = preBeep;
  result.flashBeep = flashBeep;
  result.splashMode = disableSplashScreen;
  result.enableTelemetryAlarm = enableTelemetryAlarm;
  result.filterInput = filterInput;
  result.templateSetup = templateSetup;
  result.PPM_Multiplier = PPM_Multiplier;
  // TODO frskyRssiAlarms[2];
  return result;
}

t_Gruvin9xExpoData::t_Gruvin9xExpoData()
{
  memset(this, 0, sizeof(t_Gruvin9xExpoData));
}

t_Gruvin9xExpoData::t_Gruvin9xExpoData(ExpoData &c9x)
{
  mode = c9x.mode;
  chn = c9x.chn;
  if (c9x.curveMode==1) {
    curve = c9x.curveParam;
  } else {
    curve=0;
  }
  swtch = gruvin9xFromSwitch(c9x.swtch);
  int zeros=0;
  int ones=0;
  int phtemp=c9x.phases;
  for (int i=0; i<G9X_MAX_PHASES; i++) {
    if (phtemp & 1) {
      ones++;
    } else {
      zeros++;
    }
    phtemp >>=1;
  }
  if (zeros==G9X_MAX_PHASES || zeros==0) {
    phase=0;
    negPhase=0;
  } else if (zeros==1) {
    int phtemp=c9x.phases;
    int ph=0;
    for (int i=0; i<G9X_MAX_PHASES; i++) {
      if ((phtemp & 1)==0) {
        ph=i;
        break;
      }
      phtemp >>=1;
    }
    phase=ph+1;
    negPhase=0;
  } else if (ones==1) {
    int phtemp=c9x.phases;
    int ph=0;
    for (int i=0; i<G9X_MAX_PHASES; i++) {
      if (phtemp & 1) {
        ph=i;
        break;
      }
      phtemp >>=1;
    }
    phase=(ph+1);
    negPhase=1;
  } else {
    phase=0;
    EEPROMWarnings += ::QObject::tr("Flight modes settings on expos not exported") + "\n";
  }
  weight = c9x.weight;
  expo = c9x.expo;
}

t_Gruvin9xExpoData::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;
  c9x.curveParam = curve;
  c9x.curveMode=1;
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
  c9x.expo = expo;
  return c9x;
}

t_Gruvin9xLimitData::t_Gruvin9xLimitData()
{
  memset(this, 0, sizeof(t_Gruvin9xLimitData));
}

t_Gruvin9xLimitData::t_Gruvin9xLimitData(LimitData &c9x)
{
  min = c9x.min+100;
  max = c9x.max-100;
  revert = c9x.revert;
  offset = c9x.offset;
}

t_Gruvin9xLimitData::operator LimitData ()
{
  LimitData c9x;
  c9x.min = min-100;
  c9x.max = max+100;
  c9x.revert = revert;
  c9x.offset = offset;
  return c9x;
}


t_Gruvin9xMixData::t_Gruvin9xMixData()
{
  memset(this, 0, sizeof(t_Gruvin9xMixData));
}

t_Gruvin9xMixData::t_Gruvin9xMixData(MixData &c9x)
{
  destCh = c9x.destCh;
  mixWarn = c9x.mixWarn;
  swtch = gruvin9xFromSwitch(c9x.swtch);

  if (c9x.srcRaw.type == SOURCE_TYPE_NONE) {
    srcRaw = 0;
    swtch = 0;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_STICK) {
    srcRaw = 1 + c9x.srcRaw.index;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings += ::QObject::tr("Open9x on this board doesn't have Rotary Encoders") + "\n";
    srcRaw = 5 + c9x.srcRaw.index; // use pots instead
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
    srcRaw = 8; // MAX
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
    srcRaw = 9; // FULL
    swtch = c9x.srcRaw.index+1;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
    srcRaw = 10 + c9x.srcRaw.index;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
    srcRaw = 13 + c9x.srcRaw.index;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
    srcRaw = 21 + c9x.srcRaw.index;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_TRIM) {
    EEPROMWarnings += ::QObject::tr("gruvin9x doesn't have trims as source") + "\n";
    srcRaw = 0; // use pots instead
  }

  weight = c9x.weight;
  curve = c9x.curve;
  delayUp = c9x.delayUp;
  delayDown = c9x.delayDown;
  speedUp = c9x.speedUp;
  speedDown = c9x.speedDown;
  if (c9x.carryTrim<0) {
    EEPROMWarnings += ::QObject::tr("gruvin9x doesn't have swappable trims") + "\n";
    carryTrim=1;
  } else {
    carryTrim = c9x.carryTrim;
  }
  mltpx = (MltpxValue)c9x.mltpx;
//  phase = c9x.phase;
  sOffset = c9x.sOffset;
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

  c9x.curve = curve;
  c9x.delayUp = delayUp;
  c9x.delayDown = delayDown;
  c9x.speedUp = speedUp;
  c9x.speedDown = speedDown;
  c9x.carryTrim = carryTrim;
  c9x.mltpx = (MltpxValue)mltpx;
  c9x.mixWarn = mixWarn;
  // c9x.phase = phase;
  c9x.sOffset = sOffset;
  return c9x;
}


int8_t gruvin9xFromSource(RawSource source)
{
  int v1 = 0;
  if (source.type == SOURCE_TYPE_STICK)
    v1 = 1+source.index;
  else if (source.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings += ::QObject::tr("gruvin9x on this board doesn't have Rotary Encoders") + "\n";
    v1 = 5+source.index;
  }
  else if (source.type == SOURCE_TYPE_MAX)
    v1 = 8;
  /* else if (source.type == SOURCE_TYPE_3POS)
    v1 = 0; */
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

t_Gruvin9xCustomSwData::t_Gruvin9xCustomSwData(CustomSwData &c9x)
{
  func = c9x.func;
  v1 = c9x.val1;
  v2 = c9x.val2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    v1 = gruvin9xFromSource(RawSource(c9x.val1));
  }

  if (c9x.func >= CS_FN_EQUAL) {
    v2 = gruvin9xFromSource(RawSource(c9x.val2));
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    v1 = gruvin9xFromSwitch(RawSwitch(c9x.val1));
    v2 = gruvin9xFromSwitch(RawSwitch(c9x.val2));
  }

  if (func>GR9X_MAX_CSFUNC ) {
    EEPROMWarnings += ::QObject::tr("gruvin9x does not support Custom Switch function %1").arg(getFuncName(func)) + "\n";
    func=0;
    v1=0;
    v2=0;
  }  
}

Gruvin9xCustomSwData::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    c9x.val1 = gruvin9xToSource(v1).toValue();
  }

  if (c9x.func >= CS_FN_EQUAL) {
    c9x.val2 = gruvin9xToSource(v2).toValue();
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    c9x.val1 = gruvin9xToSwitch(v1).toValue();
    c9x.val2 = gruvin9xToSwitch(v2).toValue();
  }

  return c9x;
}

t_Gruvin9xFuncSwData::t_Gruvin9xFuncSwData(FuncSwData &c9x)
{
  swtch = gruvin9xFromSwitch(c9x.swtch);
  func = c9x.func - G9X_NUM_CHNOUT;
}

Gruvin9xFuncSwData::operator FuncSwData ()
{
  FuncSwData c9x;
  c9x.swtch = gruvin9xToSwitch(swtch);
  c9x.func = (AssignFunc)(func + G9X_NUM_CHNOUT);
  return c9x;
}

t_Gruvin9xSafetySwData::t_Gruvin9xSafetySwData(SafetySwData &c9x)
{
  swtch = gruvin9xFromSwitch(c9x.swtch);
  val = c9x.val;
}

t_Gruvin9xSafetySwData::operator SafetySwData ()
{
  SafetySwData c9x;
  c9x.swtch = gruvin9xToSwitch(swtch);
  c9x.val = val;
  return c9x;
}

t_Gruvin9xSwashRingData::t_Gruvin9xSwashRingData()
{
  memset(this, 0, sizeof(t_Gruvin9xSwashRingData));
}

t_Gruvin9xSwashRingData::t_Gruvin9xSwashRingData(SwashRingData &c9x)
{
  invertELE = c9x.invertELE;
  invertAIL = c9x.invertAIL;
  invertCOL = c9x.invertCOL;
  type = c9x.type;
  collectiveSource = gruvin9xFromSource(c9x.collectiveSource);
  value = c9x.value;
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

t_Gruvin9xPhaseData_v102::operator PhaseData ()
{
  PhaseData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = trim[i];
  c9x.swtch = gruvin9xToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  return c9x;
}

t_Gruvin9xPhaseData_v106::operator PhaseData ()
{
  PhaseData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = (((int16_t)trim[i]) << 2) + ((trim_ext >> (2*i)) & 0x03);
  c9x.swtch = gruvin9xToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  return c9x;
}

t_Gruvin9xPhaseData_v106::t_Gruvin9xPhaseData_v106(PhaseData &c9x)
{
  trim_ext = 0;
  for (int i=0; i<NUM_STICKS; i++) {
    trim[i] = (int8_t)(c9x.trim[i] >> 2);
    trim_ext = (trim_ext & ~(0x03 << (2*i))) + (((c9x.trim[i] & 0x03) << (2*i)));
  }
  swtch = gruvin9xFromSwitch(c9x.swtch);
  setEEPROMZString(name, c9x.name, sizeof(name));
  fadeIn = c9x.fadeIn;
  fadeOut = c9x.fadeOut;
}

extern TimerMode getEr9xTimerMode(int mode);
extern int setEr9xTimerMode(TimerMode mode);

t_Gruvin9xTimerData::operator TimerData ()
{
  TimerData c9x;
  c9x.mode = getEr9xTimerMode(mode);
  c9x.val = val;
  c9x.dir = dir;
  return c9x;
}

t_Gruvin9xTimerData::t_Gruvin9xTimerData(TimerData &c9x)
{
  mode = setEr9xTimerMode(c9x.mode);
  val = c9x.val;
  dir = c9x.dir;
}

t_Gruvin9xFrSkyChannelData::t_Gruvin9xFrSkyChannelData()
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyChannelData));
}

t_Gruvin9xFrSkyChannelData::t_Gruvin9xFrSkyChannelData(FrSkyChannelData &c9x)
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyChannelData));
  ratio = c9x.ratio;
  alarms_value[0] = c9x.alarms[0].value;
  alarms_value[1] = c9x.alarms[1].value;
  alarms_level = (c9x.alarms[1].level << 2) + c9x.alarms[0].level;
  alarms_greater = (c9x.alarms[1].greater << 1) + c9x.alarms[0].greater;
  type = c9x.type;
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

t_Gruvin9xFrSkyData::t_Gruvin9xFrSkyData(FrSkyData &c9x)
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyData));
  channels[0] = c9x.channels[0];
  channels[1] = c9x.channels[1];
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
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  for (int i=0; i<G9X_MAX_PHASES; i++)
    c9x.phaseData[i] = phaseData[i];
  for (int i=0; i<G9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<G9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<G9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];

  for (int i=0; i<G9X_MAX_CURVE5; i++) {
    c9x.curves[i].custom = false;
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<G9X_MAX_CURVE9; i++) {
    c9x.curves[G9X_MAX_CURVE5 + i].custom = false;
    c9x.curves[G9X_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }

  for (int i=0; i<G9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<G9X_NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];
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
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  for (int i=0; i<G9X_MAX_PHASES; i++)
    c9x.phaseData[i] = phaseData[i];
  for (int i=0; i<G9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<G9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<G9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];

  for (int i=0; i<G9X_MAX_CURVE5; i++) {
    c9x.curves[i].custom = false;
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<G9X_MAX_CURVE9; i++) {
    c9x.curves[G9X_MAX_CURVE5 + i].custom = false;
    c9x.curves[G9X_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }

  for (int i=0; i<G9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<G9X_NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];
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
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<G9X_MAX_PHASES; i++) {
    c9x.phaseData[i] = phaseData[i];
    for (int j=0; j<NUM_STICKS; j++) {
      if (phaseData[i].trim[j] > 125) {
        c9x.phaseData[i].trimRef[j] = 0;
        c9x.phaseData[i].trim[j] = 0;
      }
      else if (phaseData[i].trim[j] < -125) {
        c9x.phaseData[i].trimRef[j] = 129 + phaseData[i].trim[j];
        if (c9x.phaseData[i].trimRef[j] >= i)
          c9x.phaseData[i].trimRef[j] += 1;
        c9x.phaseData[i].trim[j] = 0;
      }
      else {
        c9x.phaseData[i].trim[j] += subtrim[j];
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
    c9x.curves[i].custom = false;
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<G9X_MAX_CURVE9; i++) {
    c9x.curves[G9X_MAX_CURVE5 + i].custom = false;
    c9x.curves[G9X_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }

  for (int i=0; i<G9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<G9X_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  for (int i=0; i<G9X_NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];
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
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<G9X_MAX_PHASES; i++) {
    c9x.phaseData[i] = phaseData[i];
    for (int j=0; j<NUM_STICKS; j++) {
      if (c9x.phaseData[i].trim[j] > 500) {
        c9x.phaseData[i].trimRef[j] = c9x.phaseData[i].trim[j] - 501;
        if (c9x.phaseData[i].trimRef[j] >= i)
          c9x.phaseData[i].trimRef[j] += 1;
        c9x.phaseData[i].trim[j] = 0;
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
    c9x.curves[i].custom = false;
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<G9X_MAX_CURVE9; i++) {
    c9x.curves[G9X_MAX_CURVE5 + i].custom = false;
    c9x.curves[G9X_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[G9X_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }

  for (int i=0; i<G9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<G9X_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  for (int i=0; i<G9X_NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;

  return c9x;
}

t_Gruvin9xModelData_v106::t_Gruvin9xModelData_v106(ModelData &c9x)
{
  if (c9x.used) {
    setEEPROMZString(name, c9x.name, sizeof(name));
    timer1 = c9x.timers[0];
    switch(c9x.moduleData[0].protocol) {
      case PPM:
        protocol = 0;
        break;
      case PXX_DJT:
        protocol = 1;
        break;
      case DSM2:
        protocol = 2;
        break;
      case SILV_A:
        protocol = 3;
        break;
      case SILV_B:
        protocol = 4;
        break;
      case SILV_C:
        protocol = 5;
        break;
      case CTP1009:
        protocol = 6;
        break;
      default:
        protocol = 0;
        EEPROMWarnings += QObject::tr("Er9x doesn't accept this protocol") + "\n";
        // TODO more explicit warning for each protocol
        break;
    }
    ppmNCH = (c9x.moduleData[0].channelsCount - 8) / 2;
    thrTrim = c9x.thrTrim;
    thrExpo = c9x.thrExpo;
    trimInc = c9x.trimInc;
    pulsePol = c9x.moduleData[0].ppmPulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.moduleData[0].ppmDelay - 300) / 50;
    beepANACenter = c9x.beepANACenter;
    timer2 = c9x.timers[1];
    for (int i=0; i<G9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<G9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<G9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[G9X_MAX_EXPOS].mode)
      EEPROMWarnings += QObject::tr("gruvin9x only accepts %1 expos").arg(G9X_MAX_EXPOS) + "\n";
    for (int i=0; i<G9X_MAX_CURVE5; i++)
      if  (c9x.curves[i].count==5) {
        if  (c9x.curves[i].custom)
          EEPROMWarnings += QObject::tr("gruvin9x doesn't support custom curves as curve%1, curve as been exported as fixed point ").arg(i+1) + "\n";    
        for (int j=0; j<5; j++)
            curves5[i][j] = c9x.curves[i].points[j].y;
      } else {
        EEPROMWarnings += QObject::tr("gruvin9x doesn't support curve with %1 point as curve%2 ").arg(c9x.curves[i].count).arg(i+1) + "\n";
      }   
    for (int i=0; i<G9X_MAX_CURVE9; i++)
      if  (c9x.curves[i+G9X_MAX_CURVE5].count==9) {
        if  (c9x.curves[i+G9X_MAX_CURVE5].custom)
          EEPROMWarnings += QObject::tr("gruvin9x doesn't support custom curves as curve%1, curve as been exported as fixed point ").arg(i+1+G9X_MAX_CURVE5) + "\n";    
        for (int j=0; j<9; j++)
            curves9[i][j] = c9x.curves[i+G9X_MAX_CURVE5].points[j].y;
      } else {
        EEPROMWarnings += QObject::tr("gruvin9x doesn't support curve with %1 point as curve%2 ").arg(c9x.curves[i+G9X_MAX_CURVE5].count).arg(i+1+G9X_MAX_CURVE5) + "\n";
      }   
    for (int i=0; i<G9X_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    for (int i=0; i<G9X_NUM_FSW; i++)
      funcSw[i] = c9x.funcSw[i];
    for (int i=0; i<G9X_NUM_CHNOUT; i++)
      safetySw[i] = c9x.safetySw[i];
    swashR = c9x.swashRingData;
    for (int i=0; i<G9X_MAX_PHASES; i++) {
      PhaseData phase = c9x.phaseData[i];
      for (int j=0; j<NUM_STICKS; j++) {
        if (phase.trimRef[j] >= 0) {
          phase.trim[j] = 501 + phase.trimRef[j] - (phase.trimRef[j] >= i ? 1 : 0);
        }
        else {
          phase.trim[j] = std::max(-500, std::min(500, phase.trim[j]));
        }
      }
      phaseData[i] = phase;
    }
    frsky = c9x.frsky;
  }
  else {
    memset(this, 0, sizeof(t_Gruvin9xModelData_v106));
  }
}

