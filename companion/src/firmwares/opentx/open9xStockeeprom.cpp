#include <stdlib.h>
#include <algorithm>
#include "helpers.h"
#include "open9xStockeeprom.h"
#include <QObject>
#include <QMessageBox>

static const char specialCharsTab[] = "_-.,";

void DUMP(void *data, int size)
{
  for (int i=0; i<size; i++) {
    if (i%50 == 0) printf("\n");
    printf("%.02X ", ((uint8_t *)data)[i]);
    fflush(stdout);
  }
}

int8_t char2idx(char c)
{
  if (c==' ') return 0;
  if (c>='A' && c<='Z') return 1+c-'A';
  if (c>='a' && c<='z') return -1-c+'a';
  if (c>='0' && c<='9') return 27+c-'0';
  for (int8_t i=0;;i++) {
    char cc = specialCharsTab[i];
    if (cc==0) return 0;
    if (cc==c) return 37+i;
  }
}

#define ZCHAR_MAX 40
char idx2char(int8_t idx)
{
  if (idx == 0) return ' ';
  if (idx < 0) {
    if (idx > -27) return 'a' - idx - 1;
    idx = -idx;
  }
  if (idx < 27) return 'A' + idx - 1;
  if (idx < 37) return '0' + idx - 27;
  if (idx <= ZCHAR_MAX) return specialCharsTab[idx-37];
  return ' ';
}

void setEEPROMZString(char *dst, const char *src, int size)
{
  for (int i=size-1; i>=0; i--)
    dst[i] = char2idx(src[i]);
}

void getEEPROMZString(char *dst, const char *src, int size)
{
  for (int i=size-1; i>=0; i--)
    dst[i] = idx2char(src[i]);
  dst[size] = '\0';
  for (int i=size-1; i>=0; i--) {
    if (dst[i] == ' ')
      dst[i] = '\0';
    else
      break;
  }
}

RawSwitch open9xStockToSwitch(int8_t sw)
{
  uint8_t swa = abs(sw);
  if (swa == 0)
    return RawSwitch(SWITCH_TYPE_NONE);
  else if (swa <= 9)
    return RawSwitch(SWITCH_TYPE_SWITCH, sw);
  else if (swa <= 21)
    return RawSwitch(SWITCH_TYPE_VIRTUAL, sw > 0 ? sw-9 : sw+9);
  else if (sw == 22)
    return RawSwitch(SWITCH_TYPE_ON);
  else if (sw == 44)
    return RawSwitch(SWITCH_TYPE_ON);
  else if (sw == -22)
    return RawSwitch(SWITCH_TYPE_OFF);
  else if (swa <= 22+9)
    return RawSwitch(SWITCH_TYPE_SWITCH, sw > 0 ? sw-22 : sw+22);
  else
    return RawSwitch(SWITCH_TYPE_VIRTUAL, sw > 0 ? sw-22-9 : sw+22+9);
}

t_Open9xExpoData_v201::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;
  if (expo != 0 && curve != 0) {
    EEPROMWarnings.push_back(::QObject::tr("Simultaneous usage of expo and curves is no longer supported in OpenTX"));
  }
  else {
    if (curve == 0) {
      c9x.curve = CurveReference(CurveReference::CURVE_REF_EXPO, expo);
    }
    else {
      // TODO
    }
  }
  c9x.swtch = open9xStockToSwitch(swtch);
  if (negPhase) {
    c9x.flightModes = 1 << (phase -1);
  }
  else if (phase == 0) {
    c9x.flightModes = 0;
  }
  else {
    c9x.flightModes = 63;
    c9x.flightModes &= ~(1 << (phase -1));
  }  
  c9x.weight = weight;
  return c9x;
}

t_Open9xExpoData_v211::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;
  // TODO c9x.curve
  c9x.swtch = open9xStockToSwitch(swtch);
  c9x.flightModes = phases;
  c9x.weight = weight;
  return c9x;
}

t_Open9xLimitData_v201::operator LimitData ()
{
  LimitData c9x;
  c9x.min = 10 * (min - 100);
  c9x.max = 10 * (max + 100);
  c9x.revert = revert;
  c9x.offset = offset;
  return c9x;
}

t_Open9xLimitData_v211::operator LimitData ()
{
  LimitData c9x;
  c9x.min = 10 * (min - 100);
  c9x.max = 10 * (max + 100);
  c9x.revert = revert;
  c9x.offset = offset;
  c9x.symetrical = symetrical;
  c9x.ppmCenter = ppmCenter;
  return c9x;
}

t_Open9xMixData_v201::operator MixData ()
{
  MixData c9x;
  c9x.destCh = destCh;
  c9x.weight = weight;
  c9x.swtch = open9xStockToSwitch(swtch);

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
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, -swtch);
      c9x.weight = -weight;
    }
    else if (swtch > 0) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, swtch);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
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

  // TODO c9x.curve = curve;
  c9x.delayUp = delayUp;
  c9x.delayDown = delayDown;
  c9x.speedUp = speedUp;
  c9x.speedDown = speedDown;
  c9x.carryTrim = carryTrim;
  c9x.mltpx = (MltpxValue)mltpx;
  c9x.mixWarn = mixWarn;

  if (phase < 0) {
    c9x.flightModes = 1 << (-phase -1);
  } else if (phase == 0) {
    c9x.flightModes = 0;
  } else {
    c9x.flightModes = 63;
    c9x.flightModes &= ~(1 << (phase -1));
  }  
  c9x.sOffset = sOffset;
  return c9x;
}

t_Open9xMixData_v203::operator MixData ()
{
  MixData c9x;
  c9x.destCh = destCh;
  c9x.weight = weight;
  c9x.swtch = open9xStockToSwitch(swtch);

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
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, -swtch);
      c9x.weight = -weight;
    }
    else if (swtch > 0) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, swtch);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
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

  // TODO c9x.curve = curve;
  c9x.delayUp = delayUp;
  c9x.delayDown = delayDown;
  c9x.speedUp = speedUp;
  c9x.speedDown = speedDown;
  c9x.carryTrim = carryTrim;
  c9x.mltpx = (MltpxValue)mltpx;
  c9x.mixWarn = mixWarn;

  if (phase < 0) {
    c9x.flightModes = 1 << (-phase -1);
  } else if (phase == 0) {
    c9x.flightModes = 0;
  } else {
    c9x.flightModes = 63;
    c9x.flightModes &= ~(1 << (phase -1));
  } 
  c9x.sOffset = sOffset;
  return c9x;
}

t_Open9xMixData_v205::operator MixData ()
{
  MixData c9x;

  if (srcRaw) {
    c9x.destCh = destCh+1;
    c9x.swtch = open9xStockToSwitch(swtch);

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
    else if (srcRaw <= 30) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-9);
    }
    else if (srcRaw <= 33) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-31);
    }
    else if (srcRaw <= 41) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-34);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-42);
    }
    c9x.weight = weight;

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

    if (phase < 0) {
      c9x.flightModes = 1 << (-phase -1);
    }
    else if (phase == 0) {
      c9x.flightModes = 0;
    }
    else {
      c9x.flightModes = 63;
      c9x.flightModes &= ~(1 << (phase -1));
    }
    c9x.sOffset = sOffset;
  }
  return c9x;
}

t_Open9xMixData_v209::operator MixData ()
{
  MixData c9x;

  if (srcRaw) {
    c9x.destCh = destCh + 1;
    c9x.swtch = open9xStockToSwitch(swtch);

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
    else if (srcRaw <= 34) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-13);
    }
    else if (srcRaw <= 37) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-35);
    }
    else if (srcRaw <= 45) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-38);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-46);
    }
    c9x.weight = weight;

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

    if (phase < 0) {
      c9x.flightModes = 1 << (-phase -1);
    }
    else if (phase ==0 ) {
      c9x.flightModes = 0;
    }
    else {
      c9x.flightModes = 63;
      c9x.flightModes &= ~(1 << (phase -1));
    }
    c9x.sOffset = sOffset;
  }
  return c9x;
}

t_Open9xMixData_v211::operator MixData ()
{
  MixData c9x;

  if (srcRaw) {
    c9x.destCh = destCh + 1;
    c9x.swtch = open9xStockToSwitch(swtch);

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
    else if (srcRaw <= 34) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-13);
    }
    else if (srcRaw <= 37) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-35);
    }
    else if (srcRaw <= 45) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-38);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-46);
    }
    c9x.weight = weight;

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

RawSource open9x208ToSource(int8_t value)
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

RawSource open9xStock209ToSource(int8_t value)
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
  else if (value <= 40) {
    return RawSource(SOURCE_TYPE_CH, value-25);
  }
  else {
    return RawSource(SOURCE_TYPE_TELEMETRY, value-40);
  }
}

Open9xLogicalSwitchData_v208::operator LogicalSwitchData ()
{
  LogicalSwitchData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= LS_FN_VPOS && c9x.func <= LS_FN_ANEG) || c9x.func >= LS_FN_EQUAL) {
    c9x.val1 = open9x208ToSource(v1).toValue();
  }

  if (c9x.func >= LS_FN_EQUAL) {
    c9x.val2 = open9x208ToSource(v2).toValue();
  }

  if (c9x.func >= LS_FN_AND && c9x.func <= LS_FN_XOR) {
    c9x.val1 = open9xStockToSwitch(v1).toValue();
    c9x.val2 = open9xStockToSwitch(v2).toValue();
  }

  return c9x;
}

Open9xLogicalSwitchData_v209::operator LogicalSwitchData ()
{
  LogicalSwitchData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= LS_FN_VPOS && c9x.func <= LS_FN_ANEG) || c9x.func >= LS_FN_EQUAL) {
    c9x.val1 = open9xStock209ToSource(v1).toValue();
  }

  if (c9x.func >= LS_FN_EQUAL && c9x.func <= LS_FN_ELESS) {
    c9x.val2 = open9xStock209ToSource(v2).toValue();
  }

  if (c9x.func >= LS_FN_AND && c9x.func <= LS_FN_XOR) {
    c9x.val1 = open9xStockToSwitch(v1).toValue();
    c9x.val2 = open9xStockToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xCustomFunctionData_v201::operator CustomFunctionData ()
{
  CustomFunctionData c9x;
  c9x.swtch = open9xStockToSwitch(swtch);
  c9x.func = (AssignFunc)(func+16);
  return c9x;
}

enum Functions {
  FUNC_OVERRIDE_CH1,
  FUNC_OVERRIDE_CH16=FUNC_OVERRIDE_CH1+15,
  FUNC_TRAINER,
  FUNC_TRAINER_RUD,
  FUNC_TRAINER_ELE,
  FUNC_TRAINER_THR,
  FUNC_TRAINER_AIL,
  FUNC_INSTANT_TRIM,
  FUNC_PLAY_SOUND,
  FUNC_HAPTIC,
#if defined(PCBV4)
  FUNC_PLAY_SOMO,
#endif
  FUNC_RESET,
  FUNC_VARIO,
#if defined(PCBV4)
  FUNC_LOGS,
#endif
  FUNC_BACKLIGHT,
#if defined(DEBUG)
  FUNC_TEST, // should remain the last before MAX as not added in companion
#endif
  FUNC_MAX
};

t_Open9xCustomFunctionData_v203::operator CustomFunctionData ()
{
  CustomFunctionData c9x;
  c9x.swtch = open9xStockToSwitch(swtch);
  if (func < 16) {
    c9x.enabled = param & 0x01;
    c9x.param = (param>>1) << 1;
    c9x.func = (AssignFunc)(func);
  } else {
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
      c9x.func = FuncBacklight;
    else
      c9x.clear();
  }
  return c9x; 
}

t_Open9xCustomFunctionData_v210::operator CustomFunctionData ()
{
  CustomFunctionData c9x;
  c9x.swtch = open9xStockToSwitch(swtch);
  c9x.param = param;
  if (func < 22) {
    c9x.enabled = delay;
    c9x.func = (AssignFunc)(func);
  }
  else {
    if (func == 22)
      c9x.func = FuncPlaySound;
    else if (func == 23) {
      c9x.func = FuncPlayHaptic;
    }
    else if (func == 24)
      c9x.func = FuncReset;
    else if (func == 25)
      c9x.func = FuncVario;
    else if (func == 26)
      c9x.func = FuncPlayPrompt;
    else if (func == 27) {
      c9x.func = FuncPlayValue;
      if (param > 7) {
        c9x.param += 2;
      }
    } else if (func == 28)
      c9x.func = FuncBacklight;
    else
      c9x.clear();
  }
  return c9x;
}

t_Open9xSwashRingData_v208::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.elevatorWeight = invertELE ? -100 : 100;
  c9x.aileronWeight = invertAIL ? -100 : 100;
  c9x.collectiveWeight = invertCOL ? -100 : 100;
  c9x.type = type;
  c9x.collectiveSource = open9x208ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xSwashRingData_v209::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.elevatorWeight = invertELE ? -100 : 100;
  c9x.aileronWeight = invertAIL ? -100 : 100;
  c9x.collectiveWeight = invertCOL ? -100 : 100;
  c9x.type = type;
  c9x.collectiveSource = open9xStock209ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xFlightModeData_v201::operator FlightModeData ()
{
  FlightModeData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = (((int16_t)trim[i]) << 2) + ((trim_ext >> (2*i)) & 0x03);
  c9x.swtch = open9xStockToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  return c9x;
}

t_Open9xTimerData_v201::operator TimerData ()
{
  TimerData c9x;
  // c9x.mode = TMRMODE_OFF;
  c9x.val = val;
  c9x.persistent = persistent;
  return c9x;
}

t_Open9xTimerData_v202::operator TimerData ()
{
  TimerData c9x;
/*
  if (mode <= -22)
    c9x.mode = TimerMode(TMRMODE_FIRST_NEG_SWITCH+(mode+22));
  else if (mode <= -1)
    c9x.mode = TimerMode(TMRMODE_FIRST_NEG_SWITCH+(mode+1));
  else if (mode < 5)
    c9x.mode = TimerMode(mode);
  else if (mode < 5+21)
    c9x.mode = TimerMode(TMRMODE_FIRST_SWITCH+(mode-5));
  else
    c9x.mode = TimerMode(TMRMODE_FIRST_SWITCH+(mode-5-21));
*/
  c9x.val = val;
  c9x.persistent = false;
  return c9x;
}

FrSkyRSSIAlarm t_Open9xFrSkyRSSIAlarm::get(int index)
{
  FrSkyRSSIAlarm c9x;
  c9x.level = (2 + index + level) % 4;
  c9x.value = value + 50;
  return c9x;
}

t_Open9xFrSkyChannelData_v201::operator FrSkyChannelData ()
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
  c9x.offset = offset;
  return c9x;
}

t_Open9xFrSkyChannelData_v203::operator FrSkyChannelData ()
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
  c9x.offset = offset;
  return c9x;
}

t_Open9xFrSkyChannelData_v204::operator FrSkyChannelData ()
{
  FrSkyChannelData c9x;
  c9x.ratio = ratio;
  c9x.type = type;
  c9x.multiplier = multiplier;
  c9x.alarms[0].value = alarms_value[0];
  c9x.alarms[0].level =  alarms_level & 3;
  c9x.alarms[0].greater = alarms_greater & 1;
  c9x.alarms[1].value = alarms_value[1];
  c9x.alarms[1].level =  (alarms_level >> 2) & 3;
  c9x.alarms[1].greater = (alarms_greater >> 1) & 1;
  // TODO c9x.multiplier = multiplier;
  c9x.offset = offset;
  return c9x;
}

t_Open9xFrSkyChannelData_v208::operator FrSkyChannelData ()
{
  FrSkyChannelData c9x;
  c9x.ratio = ratio;
  c9x.type = type;
  c9x.multiplier = multiplier;
  c9x.alarms[0].value = alarms_value[0];
  c9x.alarms[0].level =  alarms_level & 3;
  c9x.alarms[0].greater = alarms_greater & 1;
  c9x.alarms[1].value = alarms_value[1];
  c9x.alarms[1].level =  (alarms_level >> 2) & 3;
  c9x.alarms[1].greater = (alarms_greater >> 1) & 1;
  // TODO c9x.multiplier = multiplier;
  c9x.offset = offset;
  return c9x;
}

t_Open9xFrSkyData_v201::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  return c9x;
}

t_Open9xFrSkyData_v202::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  return c9x;
}

t_Open9xFrSkyData_v203::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  return c9x;
}

t_Open9xFrSkyBarData_v204::operator FrSkyBarData ()
{
  FrSkyBarData c9x;
  c9x.source = RawSource(SOURCE_TYPE_TELEMETRY, source);
  c9x.barMin = barMin;
  c9x.barMax = barMax;
  return c9x;
}

t_Open9xFrSkyData_v204::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  c9x.blades = blades + 2;
  c9x.screens[0].type = 1;
  for (int i=0; i<4; i++)
    c9x.screens[0].body.bars[i] = bars[i];
  return c9x;
}

t_Open9xFrSkyData_v205::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  c9x.blades = blades + 2;
  c9x.screens[0].type = 1;
  for (int i=0; i<4; i++)
    c9x.screens[0].body.bars[i] = bars[i];
  c9x.rssiAlarms[0] = rssiAlarms[0].get(0);
  c9x.rssiAlarms[1] = rssiAlarms[1].get(1);
  return c9x;
}

t_Open9xFrSkyData_v208::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  c9x.voltsSource = voltsSource;
  c9x.blades = blades + 2;
  c9x.currentSource=currentSource;
  c9x.screens[0].type = 1;
  for (int i=0; i<4; i++)
    c9x.screens[0].body.bars[i] = bars[i];
  c9x.rssiAlarms[0] = rssiAlarms[0].get(0);
  c9x.rssiAlarms[1] = rssiAlarms[1].get(1);
  return c9x;
}

t_Open9xFrSkyData_v210::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  c9x.voltsSource = voltsSource;
  c9x.blades = blades + 2;
  c9x.currentSource=currentSource;

  int lines_screen_index = 0;
  c9x.screens[0].type = 1;
  for (int i=0; i<4; i++) {
    if (bars[i].source) {
      c9x.screens[0].body.bars[i] = bars[i];
      lines_screen_index = 1;
    }
  }

  c9x.screens[lines_screen_index].type = 0;

  c9x.rssiAlarms[0] = rssiAlarms[0].get(0);
  c9x.rssiAlarms[1] = rssiAlarms[1].get(1);
  c9x.varioSource = varioSource;
  c9x.varioCenterMax = varioSpeedUpMin;
  c9x.varioCenterMin = varioSpeedDownMin;
  return c9x;
}

t_Open9xModelData_v201::operator ModelData ()
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
  for (int i=0; i<12; i++)
    c9x.customFn[i] = customFn[i];
  // for (int i=0; i<16; i++)
  //   c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;

  return c9x;
}

t_Open9xModelData_v202::operator ModelData ()
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
  c9x.beepANACenter = beepANACenter,
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
  for (int i=0; i<12; i++)
    c9x.customFn[i] = customFn[i];
  // for (int i=0; i<16; i++)
  //  c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.moduleData[0].modelId = modelId;
  return c9x;
}

t_Open9xModelData_v203::operator ModelData ()
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

t_Open9xModelData_v204::operator ModelData ()
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
  c9x.frsky.rssiAlarms[0] = frskyRssiAlarms[0].get(0);
  c9x.frsky.rssiAlarms[1] = frskyRssiAlarms[1].get(1);
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.moduleData[0].modelId = modelId;
  return c9x;
}

t_Open9xModelData_v205::operator ModelData ()
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
    case 4:
      c9x.moduleData[0].protocol = PPM;
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
  c9x.moduleData[0].    modelId = modelId;
  c9x.frsky.screens[1].type = 0;
  return c9x;
}

t_Open9xModelData_v208::operator ModelData ()
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
  c9x.disableThrottleWarning=disableThrottleWarning;
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
  c9x.frsky.varioSource = varioSource;
  c9x.frsky.varioCenterMax = varioSpeedUpMin;
  c9x.frsky.varioCenterMin = varioSpeedDownMin;

  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.moduleData[0].modelId = modelId;
  c9x.frsky.screens[1].type = 0;
  for (int i=0; i<16; i++) {
    c9x.limitData[i].ppmCenter = servoCenter[i];
  }
  return c9x;
}

t_Open9xModelData_v209::operator ModelData ()
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
  c9x.disableThrottleWarning=disableThrottleWarning;
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
  c9x.frsky.varioSource = varioSource;
  c9x.frsky.varioCenterMax = varioSpeedUpMin;
  c9x.frsky.varioCenterMin = varioSpeedDownMin;
  c9x.switchWarningStates = switchWarningStates;

  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.moduleData[0].modelId = modelId;
  c9x.frsky.screens[1].type = 0;
  for (int i=0; i<16; i++) {
    c9x.limitData[i].ppmCenter = servoCenter[i];
  }
  return c9x;
}

t_Open9xModelData_v210::operator ModelData ()
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
  c9x.disableThrottleWarning=disableThrottleWarning;
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
    c9x.limitData[i].ppmCenter=servoCenter[i];
  }

  return c9x;
}

t_Open9xModelData_v211::operator ModelData ()
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
  c9x.disableThrottleWarning=disableThrottleWarning;
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
