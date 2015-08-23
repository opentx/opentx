#include <stdlib.h>
#include <algorithm>
#include "helpers.h"
#include "open9xGruvin9xeeprom.h"
#include <QObject>
#include <QMessageBox>

extern void getEEPROMZString(char *dst, const char *src, int size);
extern RawSwitch open9xStockToSwitch(int8_t sw);

t_Open9xGruvin9xFlightModeData_v207::operator FlightModeData ()
{
  FlightModeData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = (((int16_t)trim[i]) << 2) + ((trim_ext >> (2*i)) & 0x03);
  c9x.swtch = open9xStockToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  for (int i=0; i<2; i++)
    c9x.rotaryEncoders[i] = rotaryEncoders[i];
  return c9x;
}

t_Open9xGruvin9xFlightModeData_v208::operator FlightModeData ()
{
  FlightModeData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = trim[i];
  c9x.swtch = open9xStockToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  for (int i=0; i<2; i++)
    c9x.rotaryEncoders[i] = rotaryEncoders[i];
  return c9x;
}

t_Open9xGruvin9xMixData_v207::operator MixData ()
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
    else if (srcRaw <= 9) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_ROTARY_ENCODER, srcRaw-8);
    }
    else if (srcRaw == 10) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
    }
    /* else if (srcRaw == 11) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_3POS);
    } */
    else if (srcRaw <= 32) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-11);
    }
    else if (srcRaw <= 35) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-33);
    }
    else if (srcRaw <= 43) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-36);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-44);
    }
    c9x.weight = weight;

    if (differential) {
      c9x.curve = CurveReference(CurveReference::CURVE_REF_DIFF, differential*2);
    }
    else {
      // TODO c9x.curve = curve;
    }

    c9x.swtch = open9xStockToSwitch(swtch);
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
      c9x.flightModes=63;
      c9x.flightModes &= ~(1 << (phase -1));
    }
    c9x.sOffset = sOffset;
  }
  return c9x;
}

t_Open9xGruvin9xMixData_v209::operator MixData ()
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
    else if (srcRaw <= 9) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_ROTARY_ENCODER, srcRaw-8);
    }
    else if (srcRaw <= 13) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_TRIM, srcRaw-10);
    }
    else if (srcRaw == 14) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
    }
    else if (srcRaw == 15) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, 0);
    }
    else if (srcRaw <= 36) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-15);
    }
    else if (srcRaw <= 39) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-37);
    }
    else if (srcRaw <= 47) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-40);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-48);
    }
    c9x.weight = weight;

    if (differential) {
      c9x.curve = CurveReference(CurveReference::CURVE_REF_DIFF, differential*2);
    }
    else {
      // TODO c9x.curve = curve;
    }

    c9x.swtch = open9xStockToSwitch(swtch);
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
      c9x.flightModes=63;
      c9x.flightModes &= ~(1 << (phase -1));
    }    
    c9x.sOffset = sOffset;
  }
  return c9x;
}

t_Open9xGruvin9xMixData_v211::operator MixData ()
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
    else if (srcRaw <= 9) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_ROTARY_ENCODER, srcRaw-8);
    }
    else if (srcRaw <= 13) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_TRIM, srcRaw-10);
    }
    else if (srcRaw == 14) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
    }
    else if (srcRaw == 15) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, 0);
    }
    else if (srcRaw <= 36) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-15);
    }
    else if (srcRaw <= 39) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-37);
    }
    else if (srcRaw <= 47) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-40);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-48);
    }
    c9x.weight = weight;
    c9x.swtch = open9xStockToSwitch(swtch);

    if (curveMode==0) {
      c9x.curve = CurveReference(CurveReference::CURVE_REF_DIFF, curveParam);
    }
    else {
      // TODO c9x.curve = curve;
    }

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
  }
  return c9x;
}

RawSource open9xV4207ToSource(int8_t value)
{
  if (value == 0) {
    return RawSource(SOURCE_TYPE_NONE);
  }
  else if (value <= 7) {
    return RawSource(SOURCE_TYPE_STICK, value - 1);
  }
  else if (value <= 9) {
    return RawSource(SOURCE_TYPE_ROTARY_ENCODER, value - 8);
  }
  else if (value == 10) {
    return RawSource(SOURCE_TYPE_MAX);
  }
  /* else if (value == 11) {
    return RawSource(SOURCE_TYPE_3POS);
  } */
  else if (value <= 14) {
    return RawSource(SOURCE_TYPE_CYC, value-12);
  }
  else if (value <= 22) {
    return RawSource(SOURCE_TYPE_PPM, value-15);
  }
  else if (value <= 38) {
    return RawSource(SOURCE_TYPE_CH, value-23);
  }
  else {
    return RawSource(SOURCE_TYPE_TELEMETRY, value-38);
  }
}

RawSource open9xV4209ToSource(int8_t value)
{
  if (value == 0) {
    return RawSource(SOURCE_TYPE_NONE);
  }
  else if (value <= 7) {
    return RawSource(SOURCE_TYPE_STICK, value - 1);
  }
  else if (value <= 9) {
    return RawSource(SOURCE_TYPE_ROTARY_ENCODER, value - 8);
  }
  else if (value <= 13) {
    return RawSource(SOURCE_TYPE_STICK, value - 10);
  }
  else if (value == 14) {
    return RawSource(SOURCE_TYPE_MAX);
  }
  /* else if (value == 15) {
    return RawSource(SOURCE_TYPE_3POS);
  } */
  else if (value <= 18) {
    return RawSource(SOURCE_TYPE_CYC, value-16);
  }
  else if (value <= 26) {
    return RawSource(SOURCE_TYPE_PPM, value-19);
  }
  else if (value <= 42) {
    return RawSource(SOURCE_TYPE_CH, value-27);
  }
  else {
    return RawSource(SOURCE_TYPE_TELEMETRY, value-42);
  }
}

Open9xGruvin9xLogicalSwitchData_v207::operator LogicalSwitchData ()
{
  LogicalSwitchData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= LS_FN_VPOS && c9x.func <= LS_FN_ANEG) || c9x.func >= LS_FN_EQUAL) {
    c9x.val1 = open9xV4207ToSource(v1).toValue();
  }

  if (c9x.func >= LS_FN_EQUAL) {
    c9x.val2 = open9xV4207ToSource(v2).toValue();
  }

  if (c9x.func >= LS_FN_AND && c9x.func <= LS_FN_XOR) {
    c9x.val1 = open9xStockToSwitch(v1).toValue();
    c9x.val2 = open9xStockToSwitch(v2).toValue();
  }

  return c9x;
}

Open9xGruvin9xLogicalSwitchData_v209::operator LogicalSwitchData ()
{
  LogicalSwitchData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= LS_FN_VPOS && c9x.func <= LS_FN_ANEG) || c9x.func >= LS_FN_EQUAL) {
    c9x.val1 = open9xV4209ToSource(v1).toValue();
  }

  if (c9x.func >= LS_FN_EQUAL && c9x.func <= LS_FN_ELESS) {
    c9x.val2 = open9xV4209ToSource(v2).toValue();
  }

  if (c9x.func >= LS_FN_AND && c9x.func <= LS_FN_XOR) {
    c9x.val1 = open9xStockToSwitch(v1).toValue();
    c9x.val2 = open9xStockToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xGruvin9xCustomFunctionData_v203::operator CustomFunctionData ()
{
  CustomFunctionData c9x;
  c9x.swtch = open9xStockToSwitch(swtch);
  if (func < 16) {
    c9x.func = (AssignFunc)(func);
    c9x.enabled=param & 0x01;
    c9x.param = (param>>1)<<1;
  }
  else {
    c9x.param = param;
    if (func <= 20)
      c9x.func = (AssignFunc)(func);
    else if (func == 21)
      c9x.func = FuncInstantTrim;
    else if (func == 22)
      c9x.func = FuncPlaySound;
    else if (func == 23)
      c9x.func = FuncPlayHaptic;
    else if (func == 24)
      c9x.func = FuncReset;
    else if (func == 25)
      c9x.func = FuncVario;
    else if (func == 26)
      c9x.func = FuncPlayPrompt;
    else if (func == 27)
      c9x.func = FuncPlayValue;
    else if (func == 28)
      c9x.func = FuncLogs;
    else if (func == 29)
      c9x.func = FuncBacklight;
    else if (func >= 30 && func <= 34)
      c9x.func = AssignFunc(FuncAdjustGV1 + func - 30);
    else
      c9x.clear();
  }
  return c9x;
}

t_Open9xGruvin9xCustomFunctionData_v210::operator CustomFunctionData ()
{
  CustomFunctionData c9x;
  c9x.swtch = open9xStockToSwitch(swtch);
  c9x.param = param;
  if (func < 22) {
    c9x.func = (AssignFunc)(func);
    c9x.enabled = delay;
  } else {
    if (func == 22)
      c9x.func = FuncPlaySound;
    else if (func == 23)
      c9x.func = FuncPlayHaptic;
    else if (func == 24)
      c9x.func = FuncReset;
    else if (func == 25)
      c9x.func = FuncVario;
    else if (func == 26)
      c9x.func = FuncPlayPrompt;
    else if (func == 27)
      c9x.func = FuncPlayValue;
    else if (func == 28)
      c9x.func = FuncLogs;
    else
      c9x.clear();
  }
  return c9x;
}

t_Open9xGruvin9xSwashRingData_v208::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.elevatorWeight = invertELE ? -100 : 100;
  c9x.aileronWeight = invertAIL ? -100 : 100;
  c9x.collectiveWeight = invertCOL ? -100 : 100;
  c9x.type = type;
  c9x.collectiveSource = open9xV4207ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xGruvin9xSwashRingData_v209::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.elevatorWeight = invertELE ? -100 : 100;
  c9x.aileronWeight = invertAIL ? -100 : 100;
  c9x.collectiveWeight = invertCOL ? -100 : 100;
  c9x.type = type;
  c9x.collectiveSource = open9xV4209ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xGruvin9xModelData_v207::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<2; i++)
    c9x.timers[i] = timers[i];
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
  c9x.moduleData[0].channelsCount = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc - 2;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<5; i++) {
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
  for (int i=0; i<32; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<16; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<14; i++)
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

  for (int i=0; i<12; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<16; i++)
    c9x.customFn[i] = customFn[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.moduleData[0].modelId = modelId;
  return c9x;
}

t_Open9xGruvin9xModelData_v208::operator ModelData ()
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
  for (int i=0; i<5; i++) {
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
  for (int i=0; i<32; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<16; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<14; i++)
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
  for (int i=0; i<12; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<16; i++)
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

t_Open9xGruvin9xModelData_v209::operator ModelData ()
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
  for (int i=0; i<5; i++) {
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
  for (int i=0; i<32; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<16; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<14; i++)
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
  for (int i=0; i<12; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<16; i++)
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

t_Open9xGruvin9xModelData_v210::operator ModelData ()
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
  for (int i=0; i<5; i++) {
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
  for (int i=0; i<32; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<16; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<14; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<8; i++) {
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
  for (int i=0; i<12; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<16; i++)
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

t_Open9xGruvin9xModelData_v211::operator ModelData ()
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
  for (int i=0; i<5; i++) {
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
  for (int i=0; i<32; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<16; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<14; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<8; i++) {
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
  for (int i=0; i<12; i++)
    c9x.logicalSw[i] = logicalSw[i];
  for (int i=0; i<16; i++)
    c9x.customFn[i] = customFn[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.switchWarningStates = switchWarningStates;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.moduleData[0].modelId = modelId;
  return c9x;
}
