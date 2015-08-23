#include <stdlib.h>
#include <algorithm>
#include "helpers.h"
#include "open9xSky9xeeprom.h"
#include <QObject>
#include <QMessageBox>

extern void getEEPROMZString(char *dst, const char *src, int size);

RawSwitch open9xArmToSwitch(int8_t sw)
{
  uint8_t swa = abs(sw);
  if (swa == 0)
    return RawSwitch(SWITCH_TYPE_NONE);
  else if (swa <= 9)
    return RawSwitch(SWITCH_TYPE_SWITCH, sw);
  else if (swa <= 9+32)
    return RawSwitch(SWITCH_TYPE_VIRTUAL, sw > 0 ? sw-9 : sw+9);
  else if (sw == 42)
    return RawSwitch(SWITCH_TYPE_ON);
  else if (sw == 84)
    return RawSwitch(SWITCH_TYPE_ON);
  else if (sw == -42)
    return RawSwitch(SWITCH_TYPE_OFF);
  else if (swa <= 42+9)
    return RawSwitch(SWITCH_TYPE_SWITCH, sw > 0 ? sw-42 : sw+42);
  else
    return RawSwitch(SWITCH_TYPE_VIRTUAL, sw > 0 ? sw-42-9 : sw+42+9);
}

RawSource open9xArm208ToSource(int8_t value)
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
  else if (value <= 52) {
    return RawSource(SOURCE_TYPE_CH, value-21);
  }
  else {
    return RawSource(SOURCE_TYPE_TELEMETRY, value-52);
  }
}

RawSource open9xArm209ToSource(int8_t value)
{
  if (value == 0) {
    return RawSource(SOURCE_TYPE_NONE);
  }
  else if (value <= 7) {
    return RawSource(SOURCE_TYPE_STICK, value - 1);
  }
  else if (value <= 11) {
    return RawSource(SOURCE_TYPE_TRIM, value - 8);
  }
  else if (value == 12) {
    return RawSource(SOURCE_TYPE_MAX);
  }
  /* else if (value == 13) {
    return RawSource(SOURCE_TYPE_3POS);
  } */
  else if (value <= 16) {
    return RawSource(SOURCE_TYPE_CYC, value-14);
  }
  else if (value <= 24) {
    return RawSource(SOURCE_TYPE_PPM, value-17);
  }
  else if (value <= 56) {
    return RawSource(SOURCE_TYPE_CH, value-25);
  }
  else {
    return RawSource(SOURCE_TYPE_TELEMETRY, value-56);
  }
}

RawSource open9xArm210ToSource(int8_t value)
{
  if (value == 0) {
    return RawSource(SOURCE_TYPE_NONE);
  }
  else if (value <= 7) {
    return RawSource(SOURCE_TYPE_STICK, value - 1);
  }
  else if (value == 8) {
    return RawSource(SOURCE_TYPE_ROTARY_ENCODER, value - 8);
  }
  else if (value <= 12) {
    return RawSource(SOURCE_TYPE_TRIM, value - 9);
  }
  else if (value == 13) {
    return RawSource(SOURCE_TYPE_MAX);
  }
  /* else if (value == 14) {
    return RawSource(SOURCE_TYPE_3POS);
  } */
  else if (value <= 17) {
    return RawSource(SOURCE_TYPE_CYC, value-15);
  }
  else if (value <= 25) {
    return RawSource(SOURCE_TYPE_PPM, value-18);
  }
  else if (value <= 57) {
    return RawSource(SOURCE_TYPE_CH, value-26);
  }
  else {
    return RawSource(SOURCE_TYPE_TELEMETRY, value-57);
  }
}

t_Open9xArmTimerData_v202::operator TimerData ()
{
  TimerData c9x;
/*
  if (mode <= -42)
    c9x.mode = TimerMode(TMRMODE_FIRST_NEG_SWITCH+(mode+22));
  else if (mode <= -1)
    c9x.mode = TimerMode(TMRMODE_FIRST_NEG_SWITCH+(mode+1));
  else if (mode < 5)
    c9x.mode = TimerMode(mode);
  else if (mode < 5+41)
    c9x.mode = TimerMode(TMRMODE_FIRST_SWITCH+(mode-5));
  else
    c9x.mode = TimerMode(TMRMODE_FIRST_SWITCH+(mode-5-21));
*/
  c9x.val = val;
  c9x.persistent = false;
  return c9x;
}

t_Open9xArmExpoData_v208::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;

  if (expo!=0 && curve!=0) {
    EEPROMWarnings.push_back(::QObject::tr("Simultaneous usage of expo and curves is no longer supported"));
  }
  else {
    if (curve == 0) {
      c9x.curve = CurveReference(CurveReference::CURVE_REF_EXPO, expo);
    }
    else {
      // TODO
    }
  }
  c9x.swtch = open9xArmToSwitch(swtch);
  if (phase<0) {
    c9x.flightModes= 1 << (-phase -1);
  } else if (phase==0) {
    c9x.flightModes=0;
  } else {
    c9x.flightModes=63;
    c9x.flightModes &= ~(1 << (phase -1));
  }  
  c9x.weight = weight;
  return c9x;
}

t_Open9xArmExpoData_v210::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;

  if (expo!=0 && curve!=0) {
    EEPROMWarnings.push_back(::QObject::tr("Simultaneous usage of expo and curves is no longer supported"));
  }
  else {
    if (curve == 0) {
      c9x.curve = CurveReference(CurveReference::CURVE_REF_EXPO, expo);
    }
    else {
      // TODO
    }
  }
  c9x.swtch = open9xArmToSwitch(swtch);
  if (phase<0) {
    c9x.flightModes= 1 << (-phase -1);
  }
  else if (phase==0) {
    c9x.flightModes=0;
  }
  else {
    c9x.flightModes=63;
    c9x.flightModes &= ~(1 << (phase -1));
  }  
  c9x.weight = weight;
  getEEPROMZString(c9x.name, name, sizeof(name));
  return c9x;
}

t_Open9xArmExpoData_v212::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;

  // TODO c9x.curveMode=curveMode;
  // c9x.curveParam=curveParam;
  c9x.swtch = open9xArmToSwitch(swtch);
  c9x.flightModes= phases;
  c9x.weight = weight;
  getEEPROMZString(c9x.name, name, sizeof(name));
  return c9x;
}

t_Open9xArmFlightModeData_v208::operator FlightModeData ()
{
  FlightModeData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = trim[i];
  c9x.swtch = open9xArmToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  return c9x;
}

t_Open9xArmFlightModeData_v212::operator FlightModeData ()
{
  FlightModeData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = trim[i];
  c9x.swtch = open9xArmToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  c9x.rotaryEncoders[0] = rotaryEncoders[0];
  return c9x;
}

t_Open9xArmMixData_v208::operator MixData ()
{
  MixData c9x;
  if (srcRaw) {
    c9x.destCh = destCh+1;
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
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, 0);
    }
    else if (srcRaw <= 9+9+32) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-9);
    }
    else if (srcRaw <= 9+9+32+NUM_CYC) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-10-9-32);
    }
    else if (srcRaw <= 9+9+32+NUM_CYC+8) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-10-9-32-NUM_CYC);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-10-9-32-NUM_CYC-8);
    }
    c9x.weight = weight;
    c9x.swtch = open9xArmToSwitch(swtch);

    if (differential) {
      c9x.curve = CurveReference(CurveReference::CURVE_REF_DIFF, differential*2);
    }
    else {
      // TODO c9x.curve = curve;
    }

    c9x.delayUp = delayUp;
    c9x.delayDown = delayDown;
    c9x.speedUp = speedUp;
    c9x.speedDown = speedDown;
    c9x.carryTrim = carryTrim;
    c9x.mltpx = (MltpxValue)mltpx;
    c9x.mixWarn = mixWarn;

    if (phase<0) {
      c9x.flightModes= 1 << (-phase -1);
    }
    else if (phase==0) {
      c9x.flightModes=0;
    }
    else {
      c9x.flightModes=511;
      c9x.flightModes &= ~(1 << (phase -1));
    }

    c9x.sOffset = sOffset;
  }
  return c9x;
}

t_Open9xArmMixData_v209::operator MixData ()
{
  MixData c9x;
  if (srcRaw) {
    c9x.destCh = destCh+1;
    if (srcRaw == 0) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_NONE);
    }
    else if (srcRaw <= 7) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_STICK, srcRaw-1);
    }
    else if (srcRaw <= 11) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_TRIM, srcRaw-8);
    }
    else if (srcRaw == 12) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
    }
    else if (srcRaw == 13) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, 0);
    }
    else if (srcRaw <= 13+9+32) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-13);
    }
    else if (srcRaw <= 13+9+32+NUM_CYC) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-14-9-32);
    }
    else if (srcRaw <= 13+9+32+NUM_CYC+8) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-14-9-32-NUM_CYC);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-14-9-32-NUM_CYC-8);
    }
    c9x.weight = weight;

    if (differential) {
      c9x.curve = CurveReference(CurveReference::CURVE_REF_DIFF, differential*2);
    }
    else {
      // TODO c9x.curve = curve;
    }

    c9x.swtch = open9xArmToSwitch(swtch);
    c9x.delayUp = delayUp;
    c9x.delayDown = delayDown;
    c9x.speedUp = speedUp;
    c9x.speedDown = speedDown;
    c9x.carryTrim = carryTrim;
    c9x.mltpx = (MltpxValue)mltpx;
    c9x.mixWarn = mixWarn;

    if (phase<0) {
      c9x.flightModes= 1 << (-phase -1);
    }
    else if (phase==0) {
      c9x.flightModes=0;
    }
    else {
      c9x.flightModes=511;
      c9x.flightModes &= ~(1 << (phase -1));
    }

    c9x.sOffset = sOffset;
  }
  return c9x;
}

t_Open9xArmMixData_v210::operator MixData ()
{
  MixData c9x;
  if (srcRaw) {
    c9x.destCh = destCh+1;
    if (srcRaw == 0) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_NONE);
    }
    else if (srcRaw <= 7) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_STICK, srcRaw-1);
    }
    else if (srcRaw <= 11) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_TRIM, srcRaw-8);
    }
    else if (srcRaw == 12) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
    }
    else if (srcRaw == 13) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, 0);
    }
    else if (srcRaw <= 13+9+32) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-13);
    }
    else if (srcRaw <= 13+9+32+NUM_CYC) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-14-9-32);
    }
    else if (srcRaw <= 13+9+32+NUM_CYC+8) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-14-9-32-NUM_CYC);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-14-9-32-NUM_CYC-8);
    }
    c9x.weight = weight;

    if (differential) {
      c9x.curve = CurveReference(CurveReference::CURVE_REF_DIFF, differential*2);
    }
    else {
      // TODO c9x.curve = curve;
    }

    c9x.swtch = open9xArmToSwitch(swtch);
    c9x.delayUp = delayUp;
    c9x.delayDown = delayDown;
    c9x.speedUp = speedUp;
    c9x.speedDown = speedDown;
    c9x.carryTrim = carryTrim;
    c9x.mltpx = (MltpxValue)mltpx;
    c9x.mixWarn = mixWarn;

    if (phase<0) {
      c9x.flightModes= 1 << (-phase -1);
    }
    else if (phase==0) {
      c9x.flightModes=0;
    }
    else {
      c9x.flightModes=511;
      c9x.flightModes &= ~(1 << (phase -1));
    }

    c9x.sOffset = sOffset;
    getEEPROMZString(c9x.name, name, sizeof(name));
  }
  return c9x;
}

t_Open9xArmMixData_v212::operator MixData ()
{
  MixData c9x;
  if (srcRaw) {
    c9x.destCh = destCh+1;
    if (srcRaw == 0) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_NONE);
    }
    else if (srcRaw <= 7) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_STICK, srcRaw-1);
    }
    else if (srcRaw <= 8) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_ROTARY_ENCODER, srcRaw-8);
    }
    else if (srcRaw <= 12) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_TRIM, srcRaw-9);
    }
    else if (srcRaw == 13) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
    }
    else if (srcRaw == 14) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, 0);
    }
    else if (srcRaw <= 14+9+32) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-14);
    }
    else if (srcRaw <= 14+9+32+NUM_CYC) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-15-9-32);
    }
    else if (srcRaw <= 14+9+32+NUM_CYC+8) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-15-9-32-NUM_CYC);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-15-9-32-NUM_CYC-8);
    }
    c9x.weight = weight;

    if (curveMode==0) {
      c9x.curve = CurveReference(CurveReference::CURVE_REF_DIFF, curveParam);
    }
    else {
      // TODO
    }

    c9x.swtch = open9xArmToSwitch(swtch);
    c9x.delayUp = delayUp;
    c9x.delayDown = delayDown;
    c9x.speedUp = speedUp;
    c9x.speedDown = speedDown;
    c9x.carryTrim = carryTrim;
    c9x.noExpo = noExpo;
    c9x.mltpx = (MltpxValue)mltpx;
    c9x.mixWarn = mixWarn;
    c9x.flightModes = phases;
    c9x.sOffset = sOffset;
    getEEPROMZString(c9x.name, name, sizeof(name));
  }
  return c9x;
}

t_Open9xArmLogicalSwitchData_v208::operator LogicalSwitchData ()
{
  LogicalSwitchData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= LS_FN_VPOS && c9x.func <= LS_FN_ANEG) || c9x.func >= LS_FN_EQUAL) {
    c9x.val1 = open9xArm208ToSource(v1).toValue();
  }

  if (c9x.func >= LS_FN_EQUAL) {
    c9x.val2 = open9xArm208ToSource(v2).toValue();
  }

  if (c9x.func >= LS_FN_AND && c9x.func <= LS_FN_XOR) {
    c9x.val1 = open9xArmToSwitch(v1).toValue();
    c9x.val2 = open9xArmToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xArmLogicalSwitchData_v209::operator LogicalSwitchData ()
{
  LogicalSwitchData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= LS_FN_VPOS && c9x.func <= LS_FN_ANEG) || c9x.func >= LS_FN_EQUAL) {
    c9x.val1 = open9xArm209ToSource(v1).toValue();
  }

  if (c9x.func >= LS_FN_EQUAL) {
    c9x.val2 = open9xArm209ToSource(v2).toValue();
  }

  if (c9x.func >= LS_FN_AND && c9x.func <= LS_FN_XOR) {
    c9x.val1 = open9xArmToSwitch(v1).toValue();
    c9x.val2 = open9xArmToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xArmLogicalSwitchData_v210::operator LogicalSwitchData ()
{
  LogicalSwitchData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;
  c9x.delay = delay;
  c9x.duration = duration;
  
  if ((c9x.func >= LS_FN_VPOS && c9x.func <= LS_FN_ANEG) || c9x.func >= LS_FN_EQUAL) {
    c9x.val1 = open9xArm210ToSource(v1).toValue();
  }

  if (c9x.func >= LS_FN_EQUAL && c9x.func <= LS_FN_ELESS) {
    c9x.val2 = open9xArm210ToSource(v2).toValue();
  }

  if (c9x.func >= LS_FN_AND && c9x.func <= LS_FN_XOR) {
    c9x.val1 = open9xArmToSwitch(v1).toValue();
    c9x.val2 = open9xArmToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xArmCustomFunctionData_v208::operator CustomFunctionData ()
{
  CustomFunctionData c9x;
  c9x.swtch = open9xArmToSwitch(swtch);
  c9x.func = (AssignFunc)(func);
  if (c9x.func <= FuncOverrideCH32) {
    c9x.enabled=param & 0x01;
    c9x.param = (param>>1)<<1;
  }
  else {
    if ((c9x.func == FuncPlayValue || c9x.func == FuncVolume) && param > 7) {
      param+=2;
    } 
    c9x.param = param;
  }
  return c9x;
}

t_Open9xArmCustomFunctionData_v210::operator CustomFunctionData ()
{
  CustomFunctionData c9x;
  c9x.swtch = open9xArmToSwitch(swtch);
  c9x.func = (AssignFunc)(func);
  uint32_t value = *((uint32_t *)param);
  if (c9x.func <= FuncInstantTrim) {
    c9x.enabled = value & 0x01;
    c9x.param = (value>>1)<<1;
  }
  else {
    if (c9x.func == FuncPlayPrompt || c9x.func == FuncBackgroundMusic) {
      memcpy(c9x.paramarm, param, sizeof(c9x.paramarm));
    }
    else {
      if ((c9x.func == FuncPlayValue || c9x.func == FuncVolume) && value > 7) {
        value+=2;
      } 
      c9x.param = value;
    }
  }
  return c9x;
}

t_Open9xArmCustomFunctionData_v211::operator CustomFunctionData ()
{
  CustomFunctionData c9x;
  c9x.swtch = open9xArmToSwitch(swtch);
  c9x.func = (AssignFunc)(func);
  uint32_t value = *((uint32_t *)param);
  if (c9x.func <= FuncInstantTrim) {
    c9x.enabled = delay & 0x01;
    c9x.param = value;
  }
  else {
    if (c9x.func == FuncPlayPrompt || c9x.func == FuncBackgroundMusic) {
      memcpy(c9x.paramarm, param, sizeof(c9x.paramarm));
    }
    else {
      c9x.param = value;
      if ((c9x.func == FuncPlayValue || c9x.func == FuncVolume || (c9x.func >= FuncAdjustGV1 && c9x.func <= FuncAdjustGVLast)) && value > 7) {
        c9x.param++;
      } 
    }
  }
  return c9x;
}

t_Open9xArmSwashRingData_v208::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.elevatorWeight = invertELE ? -100 : 100;
  c9x.aileronWeight = invertAIL ? -100 : 100;
  c9x.collectiveWeight = invertCOL ? -100 : 100;
  c9x.type = type;
  c9x.collectiveSource = open9xArm208ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xArmSwashRingData_v209::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.elevatorWeight = invertELE ? -100 : 100;
  c9x.aileronWeight = invertAIL ? -100 : 100;
  c9x.collectiveWeight = invertCOL ? -100 : 100;
  c9x.type = type;
  c9x.collectiveSource = open9xArm209ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xArmSwashRingData_v210::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.elevatorWeight = invertELE ? -100 : 100;
  c9x.aileronWeight = invertAIL ? -100 : 100;
  c9x.collectiveWeight = invertCOL ? -100 : 100;
  c9x.type = type;
  c9x.collectiveSource = open9xArm210ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xArmFrSkyData_v210::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  c9x.voltsSource = voltsSource;
  c9x.blades = blades+2;
  c9x.currentSource=currentSource;
  c9x.screens[0].type = 1;
  c9x.rssiAlarms[0] = rssiAlarms[0].get(0);
  c9x.rssiAlarms[1] = rssiAlarms[1].get(1);
  return c9x;
}

t_Open9xArmFrSkyData_v211::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  c9x.voltsSource = voltsSource;
  c9x.blades = blades+2;
  c9x.currentSource=currentSource;
  c9x.screens[0].type = 1;
  c9x.rssiAlarms[0] = rssiAlarms[0].get(0);
  c9x.rssiAlarms[1] = rssiAlarms[1].get(1);
  c9x.varioSource = varioSource;
  c9x.varioCenterMax = varioSpeedUpMin;
  c9x.varioCenterMin = varioSpeedDownMin;

  return c9x;
}

t_Open9xArmModelData_v208::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<2; i++)
    c9x.timers[i] = timers[i];
  switch(protocol) {
    case 1:
      c9x.moduleData[0].protocol = PPM16;
      break;
    case 2:
      c9x.moduleData[0].protocol = PPMSIM;
      break;
    case 3:
      c9x.moduleData[0].protocol = PXX_DJT;
      break;
    case 4:
      c9x.moduleData[0].protocol = DSM2;
      break;
    default:
      c9x.moduleData[0].protocol = PPM;
      break;
  }
  c9x.moduleData[0].channelsCount = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc - 2;
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<9; i++) {
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
  for (int i=0; i<64; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<32; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<32; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<8; i++) {
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<8; i++) {
    c9x.curves[8 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[8 + i].points[j].x = -100 + 25 * i;
      c9x.curves[8 + i].points[j].y = curves9[i][j];
    }
  }
  for (int i=0; i<32; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<32; i++)
    c9x.customFn[i] = customFn[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.frsky.varioSource = varioSource;
  c9x.frsky.varioCenterMax = varioSpeedUpMin;
  c9x.frsky.varioCenterMin = varioSpeedDownMin;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.moduleData[0].modelId = modelId;
  for (int i=0; i<16; i++) {
    c9x.limitData[i].ppmCenter = servoCenter[i];
  }

  return c9x;
}

t_Open9xArmModelData_v209::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<2; i++)
    c9x.timers[i] = timers[i];
  switch(protocol) {
    case 1:
      c9x.moduleData[0].protocol = PPM16;
      break;
    case 2:
      c9x.moduleData[0].protocol = PPMSIM;
      break;
    case 3:
      c9x.moduleData[0].protocol = PXX_DJT;
      break;
    case 4:
      c9x.moduleData[0].protocol = DSM2;
      break;
    default:
      c9x.moduleData[0].protocol = PPM;
      break;
  }
  c9x.moduleData[0].channelsCount = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc - 2;
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<9; i++) {
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
  for (int i=0; i<64; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<32; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<32; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<8; i++) {
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<8; i++) {
    c9x.curves[8 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[8 + i].points[j].x = -100 + 25 * i;
      c9x.curves[8 + i].points[j].y = curves9[i][j];
    }
  }
  for (int i=0; i<32; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<32; i++)
    c9x.customFn[i] = customFn[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.frsky.varioSource = varioSource;
  c9x.frsky.varioCenterMax = varioSpeedUpMin;
  c9x.frsky.varioCenterMin = varioSpeedDownMin;
  c9x.switchWarningStates = switchWarningStates;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.moduleData[0].modelId = modelId;
  for (int i=0; i<16; i++) {
    c9x.limitData[i].ppmCenter = servoCenter[i];
  }

  return c9x;
}

t_Open9xArmModelData_v210::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<2; i++)
    c9x.timers[i] = timers[i];
  switch(protocol) {
    case 1:
      c9x.moduleData[0].protocol = PPM16;
      break;
    case 2:
      c9x.moduleData[0].protocol = PPMSIM;
      break;
    case 3:
      c9x.moduleData[0].protocol = PXX_DJT;
      break;
    case 4:
      c9x.moduleData[0].protocol = DSM2;
      break;
    default:
      c9x.moduleData[0].protocol = PPM;
      break;
  }
  c9x.moduleData[0].channelsCount = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc - 2;
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<9; i++) {
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
  for (int i=0; i<64; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<32; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<32; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<8; i++) {
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<8; i++) {
    c9x.curves[8 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[8 + i].points[j].x = -100 + 25 * i;
      c9x.curves[8 + i].points[j].y = curves9[i][j];
    }
  }
  for (int i=0; i<32; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<32; i++)
    c9x.customFn[i] = customFn[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.frsky.varioSource = varioSource;
  c9x.frsky.varioCenterMax = varioSpeedUpMin;
  c9x.frsky.varioCenterMin = varioSpeedDownMin;
  c9x.switchWarningStates = switchWarningStates;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.moduleData[0].modelId = modelId;
  for (int i=0; i<16; i++) {
    c9x.limitData[i].ppmCenter = servoCenter[i];
  }

  return c9x;
}

t_Open9xArmModelData_v211::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<2; i++)
    c9x.timers[i] = timers[i];
  switch(protocol) {
    case 1:
      c9x.moduleData[0].protocol = PPM16;
      break;
    case 2:
      c9x.moduleData[0].protocol = PPMSIM;
      break;
    case 3:
      c9x.moduleData[0].protocol = PXX_DJT;
      break;
    case 4:
      c9x.moduleData[0].protocol = DSM2;
      break;
    default:
      c9x.moduleData[0].protocol = PPM;
      break;
  }
  c9x.moduleData[0].channelsCount = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc - 2;
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<9; i++) {
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
  for (int i=0; i<64; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<32; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<32; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<16; i++) {
    CurveInfo crvinfo = curveinfo(this, i);
    c9x.curves[i].type = (crvinfo.custom ? CurveData::CURVE_TYPE_CUSTOM : CurveData::CURVE_TYPE_STANDARD);
    c9x.curves[i].count = crvinfo.points;
    for (int j=0; j<crvinfo.points; j++)
      c9x.curves[i].points[j].y = crvinfo.crv[j];
    if (crvinfo.custom) {
      c9x.curves[i].points[0].x = -100;
      for (int j=1; j<crvinfo.points-1; j++)
        c9x.curves[i].points[j].x = crvinfo.crv[crvinfo.points+j-1];
      c9x.curves[i].points[crvinfo.points-1].x = +100;
    }
    else {
      for (int j=0; j<crvinfo.points; j++)
        c9x.curves[i].points[j].x = -100 + (200*i) / (crvinfo.points-1);
    }
  }
  for (int i=0; i<32; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<32; i++)
    c9x.customFn[i] = customFn[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.switchWarningStates = switchWarningStates;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.moduleData[0].modelId = modelId;
  for (int i=0; i<16; i++) {
    c9x.limitData[i].ppmCenter = servoCenter[i];
  }

  return c9x;
}

t_Open9xArmModelData_v212::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<2; i++) {
    c9x.timers[i] = timers[i];
    c9x.timers[i].persistent = timersXtra[i].remanent;
    c9x.timers[i].val = timersXtra[i].value;
  }
  switch(protocol) {
    case 1:
      c9x.moduleData[0].protocol = PPM16;
      break;
    case 2:
      c9x.moduleData[0].protocol = PPMSIM;
      break;
    case 3:
      c9x.moduleData[0].protocol = PXX_DJT;
      break;
    case 4:
      c9x.moduleData[0].protocol = DSM2;
      break;
    default:
      c9x.moduleData[0].protocol = PPM;
      break;
  }
  c9x.moduleData[0].channelsCount = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc - 2;
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<9; i++) {
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
  for (int i=0; i<64; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<32; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<32; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<16; i++) {
    CurveInfo crvinfo = curveinfo(this, i);
    c9x.curves[i].type = (crvinfo.custom ? CurveData::CURVE_TYPE_CUSTOM : CurveData::CURVE_TYPE_STANDARD);
    c9x.curves[i].count = crvinfo.points;
    for (int j=0; j<crvinfo.points; j++)
      c9x.curves[i].points[j].y = crvinfo.crv[j];
    if (crvinfo.custom) {
      c9x.curves[i].points[0].x = -100;
      for (int j=1; j<crvinfo.points-1; j++)
        c9x.curves[i].points[j].x = crvinfo.crv[crvinfo.points+j-1];
      c9x.curves[i].points[crvinfo.points-1].x = +100;
    }
    else {
      for (int j=0; j<crvinfo.points; j++)
        c9x.curves[i].points[j].x = -100 + (200*i) / (crvinfo.points-1);
    }
  }
  for (int i=0; i<32; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<32; i++)
    c9x.customFn[i] = customFn[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.switchWarningStates = switchWarningStates;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.moduleData[0].modelId = modelId;
  return c9x;
}
