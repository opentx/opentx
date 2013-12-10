#include <stdlib.h>
#include <algorithm>
#include "helpers.h"
#include "open9xSky9xeeprom.h"
#include <QObject>
#include <QMessageBox>

extern void setEEPROMZString(char *dst, const char *src, int size);
extern void getEEPROMZString(char *dst, const char *src, int size);

int8_t open9xArmFromSwitch(const RawSwitch & sw)
{
  switch (sw.type) {
    case SWITCH_TYPE_SWITCH:
      return sw.index;
    case SWITCH_TYPE_VIRTUAL:
      return sw.index > 0 ? (9 + sw.index) : (-9 + sw.index);
    case SWITCH_TYPE_ON:
      return 42;
    case SWITCH_TYPE_OFF:
      return -42;
    case SWITCH_TYPE_MOMENT_SWITCH:
      return sw.index > 0 ? (42 + sw.index) : (-42 + sw.index);
    case SWITCH_TYPE_MOMENT_VIRTUAL:
      return sw.index > 0 ? (51 + sw.index) : (-51 + sw.index);
    case SWITCH_TYPE_ONM:
      return 84;
    default:
      return 0;
  }
}

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
    return RawSwitch(SWITCH_TYPE_ONM);
  else if (sw == -42)
    return RawSwitch(SWITCH_TYPE_OFF);
  else if (swa <= 42+9)
    return RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, sw > 0 ? sw-42 : sw+42);
  else
    return RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, sw > 0 ? sw-42-9 : sw+42+9);
}

int8_t open9xArm208FromSource(RawSource source)
{
  int v1 = 0;
  if (source.type == SOURCE_TYPE_STICK)
    v1 = 1+source.index;
  else if (source.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings += ::QObject::tr("Open9x on this board doesn't have Rotary Encoders") + "\n";
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
    v1 = 52+source.index;
  return v1;
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

int8_t open9xArm209FromSource(RawSource source)
{
  int v1 = 0;
  if (source.type == SOURCE_TYPE_STICK)
    v1 = 1+source.index;
  else if (source.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings += ::QObject::tr("Open9x on this board doesn't have Rotary Encoders") + "\n";
    v1 = 5+source.index;
  }
  else if (source.type == SOURCE_TYPE_TRIM)
    v1 = 8 + source.index;
  else if (source.type == SOURCE_TYPE_MAX)
    v1 = 12;
  /* else if (source.type == SOURCE_TYPE_3POS)
    v1 = 13; */
  else if (source.type == SOURCE_TYPE_CYC)
    v1 = 14+source.index;
  else if (source.type == SOURCE_TYPE_PPM)
    v1 = 17+source.index;
  else if (source.type == SOURCE_TYPE_CH)
    v1 = 25+source.index;
  else if (source.type == SOURCE_TYPE_TELEMETRY)
    v1 = 56+source.index;
  return v1;
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

int8_t open9xArm210FromSource(RawSource source)
{
  int v1 = 0;
  if (source.type == SOURCE_TYPE_STICK)
    v1 = 1+source.index;
  else if (source.type == SOURCE_TYPE_ROTARY_ENCODER)
    v1 = 8;
  else if (source.type == SOURCE_TYPE_TRIM)
    v1 = 9 + source.index;
  else if (source.type == SOURCE_TYPE_MAX)
    v1 = 13;
  /* else if (source.type == SOURCE_TYPE_3POS)
    v1 = 14; */
  else if (source.type == SOURCE_TYPE_CYC)
    v1 = 15+source.index;
  else if (source.type == SOURCE_TYPE_PPM)
    v1 = 18+source.index;
  else if (source.type == SOURCE_TYPE_CH)
    v1 = 26+source.index;
  else if (source.type == SOURCE_TYPE_TELEMETRY)
    v1 = 57+source.index;
  return v1;
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

  if (mode <= -42)
    c9x.mode = TimerMode(TMRMODE_FIRST_NEG_MOMENT_SWITCH+(mode+22));
  else if (mode <= -1)
    c9x.mode = TimerMode(TMRMODE_FIRST_NEG_SWITCH+(mode+1));
  else if (mode < 5)
    c9x.mode = TimerMode(mode);
  else if (mode < 5+41)
    c9x.mode = TimerMode(TMRMODE_FIRST_SWITCH+(mode-5));
  else
    c9x.mode = TimerMode(TMRMODE_FIRST_MOMENT_SWITCH+(mode-5-21));

  c9x.val = val;
  c9x.persistent = false;
  c9x.dir = (val == 0);
  return c9x;
}

t_Open9xArmTimerData_v202::t_Open9xArmTimerData_v202(TimerData &c9x)
{
  val = c9x.val;

  if (c9x.mode >= TMRMODE_OFF && c9x.mode <= TMRMODE_THt)
    mode = 0+c9x.mode-TMRMODE_OFF;
  else if (c9x.mode >= TMRMODE_FIRST_MOMENT_SWITCH)
    mode = 46+c9x.mode-TMRMODE_FIRST_MOMENT_SWITCH;
  else if (c9x.mode >= TMRMODE_FIRST_SWITCH)
    mode = 5+c9x.mode-TMRMODE_FIRST_SWITCH;
  else if (c9x.mode <= TMRMODE_FIRST_NEG_MOMENT_SWITCH)
    mode = -42+c9x.mode-TMRMODE_FIRST_NEG_MOMENT_SWITCH;
  else if (c9x.mode <= TMRMODE_FIRST_NEG_SWITCH)
    mode = -1+c9x.mode-TMRMODE_FIRST_NEG_SWITCH;
  else
    mode = 0;
}

t_Open9xArmExpoData_v208::t_Open9xArmExpoData_v208(ExpoData &c9x)
{
  mode = c9x.mode;
  chn = c9x.chn;
  // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
  curve = 0;
  expo = 0;
  if (c9x.curveMode==1) {
    if (c9x.curveParam >=0 && c9x.curveParam <= 10)
      curve = c9x.curveParam;
    else if (c9x.curveParam >= 15 && c9x.curveParam <= 19)
      curve = c9x.curveParam - 4;
    else
      EEPROMWarnings += ::QObject::tr("Open9x doesn't allow Curve%1 in expos").arg(c9x.curveParam-6) + "\n";
  }
  else {
    expo=c9x.curveParam;
  }

  swtch = open9xArmFromSwitch(c9x.swtch);
  int zeros=0;
  int ones=0;
  int phtemp=c9x.phases;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
    if (phtemp & 1) {
      ones++;
    } else {
      zeros++;
    }
    phtemp >>=1;
  }
  if (zeros==O9X_MAX_PHASES || zeros==0) {
    phase=0;
  } else if (zeros==1) {
    int phtemp=c9x.phases;
    int ph=0;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
      if ((phtemp & 1)==0) {
        ph=i;
        break;
      }
      phtemp >>=1;
    }
    phase=ph+1;
  } else if (ones==1) {
    int phtemp=c9x.phases;
    int ph=0;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
      if (phtemp & 1) {
        ph=i;
        break;
      }
      phtemp >>=1;
    }
    phase=-(ph+1);
  } else {
    phase=0;
    EEPROMWarnings += ::QObject::tr("Flight modes settings on expos not exported") + "\n";
  }

  weight = c9x.weight;
  expo = c9x.expo;
}

t_Open9xArmExpoData_v208::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;

  if (expo!=0 && curve!=0) {
    EEPROMWarnings += ::QObject::tr("Simultaneous usage of expo and curves is no longer supported") + "\n";
  }
  else {
    if (curve == 0) {
        c9x.curveMode=0;
        c9x.curveParam=expo;
    } else {
        c9x.curveMode=1;
        if (curve <= 10)
          c9x.curveParam = curve;
        else
          c9x.curveParam = curve + 4;
    }
  }
  c9x.swtch = open9xArmToSwitch(swtch);
  if (phase<0) {
    c9x.phases= 1 << (-phase -1);
  } else if (phase==0) {
    c9x.phases=0;
  } else {
    c9x.phases=63;
    c9x.phases &= ~(1 << (phase -1));
  }  
  c9x.weight = weight;
  return c9x;
}

t_Open9xArmExpoData_v210::t_Open9xArmExpoData_v210(ExpoData &c9x)
{
  mode = c9x.mode;
  chn = c9x.chn;
  // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
  curve = 0;
  expo = 0;
  if (c9x.curveMode==1) {
    if (c9x.curveParam >=0 && c9x.curveParam <= 10)
      curve = c9x.curveParam;
    else if (c9x.curveParam >= 15 && c9x.curveParam <= 19)
      curve = c9x.curveParam - 4;
    else
      EEPROMWarnings += ::QObject::tr("Open9x doesn't allow Curve%1 in expos").arg(c9x.curveParam-6) + "\n";
  }
  else {
      expo=c9x.curveParam;
  }

  swtch = open9xArmFromSwitch(c9x.swtch);
  int zeros=0;
  int ones=0;
  int phtemp=c9x.phases;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
    if (phtemp & 1) {
      ones++;
    } else {
      zeros++;
    }
    phtemp >>=1;
  }
  if (zeros==O9X_MAX_PHASES || zeros==0) {
    phase=0;
  } else if (zeros==1) {
    int phtemp=c9x.phases;
    int ph=0;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
      if ((phtemp & 1)==0) {
        ph=i;
        break;
      }
      phtemp >>=1;
    }
    phase=ph+1;
  } else if (ones==1) {
    int phtemp=c9x.phases;
    int ph=0;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
      if (phtemp & 1) {
        ph=i;
        break;
      }
      phtemp >>=1;
    }
    phase=-(ph+1);
  } else {
    phase=0;
    EEPROMWarnings += ::QObject::tr("Flight modes settings on expos not exported") + "\n";
  }
  weight = c9x.weight;
  expo = c9x.expo;
  setEEPROMZString(name, c9x.name, sizeof(name));
}

t_Open9xArmExpoData_v210::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;

  if (expo!=0 && curve!=0) {
    EEPROMWarnings += ::QObject::tr("Simultaneous usage of expo and curves is no longer supported") + "\n";
  }
  else {
    if (curve == 0) {
      c9x.curveMode=0;
      c9x.curveParam=expo;
    }
    else {
      c9x.curveMode=1;
      if (curve <= 10)
        c9x.curveParam = curve;
      else
        c9x.curveParam = curve + 4;
    }
  }
  c9x.swtch = open9xArmToSwitch(swtch);
  if (phase<0) {
    c9x.phases= 1 << (-phase -1);
  } else if (phase==0) {
    c9x.phases=0;
  } else {
    c9x.phases=63;
    c9x.phases &= ~(1 << (phase -1));
  }  
  c9x.weight = weight;
  c9x.expo = expo;
  getEEPROMZString(c9x.name, name, sizeof(name));
  return c9x;
}

t_Open9xArmExpoData_v212::t_Open9xArmExpoData_v212(ExpoData &c9x)
{
  mode = c9x.mode;
  chn = c9x.chn;
  // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
  curveParam=c9x.curveParam;
  curveMode=c9x.curveMode;
  swtch = open9xArmFromSwitch(c9x.swtch);
  phases=c9x.phases;
  weight = c9x.weight;
  setEEPROMZString(name, c9x.name, sizeof(name));
}

t_Open9xArmExpoData_v212::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;

  c9x.curveMode=curveMode;
  c9x.curveParam=curveParam;
  c9x.swtch = open9xArmToSwitch(swtch);
  c9x.phases= phases;
  c9x.weight = weight;
  getEEPROMZString(c9x.name, name, sizeof(name));
  return c9x;
}

t_Open9xArmPhaseData_v208::operator PhaseData ()
{
  PhaseData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = trim[i];
  c9x.swtch = open9xArmToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  return c9x;
}

t_Open9xArmPhaseData_v208::t_Open9xArmPhaseData_v208(PhaseData &c9x)
{
  for (int i=0; i<NUM_STICKS; i++)
    trim[i] = c9x.trim[i];
  swtch = open9xArmFromSwitch(c9x.swtch);
  setEEPROMZString(name, c9x.name, sizeof(name));
  fadeIn = c9x.fadeIn;
  fadeOut = c9x.fadeOut;
}

t_Open9xArmPhaseData_v212::operator PhaseData ()
{
  PhaseData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = trim[i];
  c9x.swtch = open9xArmToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  c9x.rotaryEncoders[0] = rotaryEncoders[0];
  return c9x;
}

t_Open9xArmPhaseData_v212::t_Open9xArmPhaseData_v212(PhaseData &c9x)
{
  for (int i=0; i<NUM_STICKS; i++)
    trim[i] = c9x.trim[i];
  swtch = open9xArmFromSwitch(c9x.swtch);
  setEEPROMZString(name, c9x.name, sizeof(name));
  fadeIn = c9x.fadeIn;
  fadeOut = c9x.fadeOut;
  rotaryEncoders[0] = c9x.rotaryEncoders[0];
}

t_Open9xArmMixData_v208::t_Open9xArmMixData_v208(MixData &c9x)
{
  if (c9x.destCh) {
    destCh = c9x.destCh-1;
    mixWarn = c9x.mixWarn;
    swtch = open9xArmFromSwitch(c9x.swtch);
    if (c9x.srcRaw.type == SOURCE_TYPE_NONE) {
      srcRaw = 0;
      swtch = 0;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_STICK) {
      srcRaw = 1 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
      srcRaw = 8;
    }
    /* else if (c9x.srcRaw.type == SOURCE_TYPE_3POS) {
      srcRaw = 9;
    } */
    else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
      srcRaw = 9 + open9xArmFromSwitch(RawSwitch(c9x.srcRaw.index));
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
      srcRaw = 10+9+O9X_ARM_NUM_CSW + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
      srcRaw = 10+9+O9X_ARM_NUM_CSW+NUM_CYC + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
      srcRaw = 10+9+O9X_ARM_NUM_CSW+NUM_CYC+NUM_PPM + c9x.srcRaw.index;
    }
    weight = c9x.weight;
    differential = c9x.differential/2;
    curve = c9x.curve;
    delayUp = c9x.delayUp;
    delayDown = c9x.delayDown;
    speedUp = c9x.speedUp;
    speedDown = c9x.speedDown;
    carryTrim = c9x.carryTrim;
    mltpx = (MltpxValue)c9x.mltpx;
    int zeros=0;
    int ones=0;
    int phtemp=c9x.phases;
    for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
      if (phtemp & 1) {
        ones++;
      } else {
        zeros++;
      }
      phtemp >>=1;
    }
    if (zeros==O9X_ARM_MAX_PHASES || zeros==0) {
      phase=0;
    } else if (zeros==1) {
      int phtemp=c9x.phases;
      int ph=0;
      for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
        if ((phtemp & 1)==0) {
          ph=i;
          break;
        }
        phtemp >>=1;
      }
      phase=ph+1;
    } else if (ones==1) {
      int phtemp=c9x.phases;
      int ph=0;
      for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
        if (phtemp & 1) {
          ph=i;
          break;
        }
        phtemp >>=1;
      }
      phase=-(ph+1);
    } else {
      phase=0;
      EEPROMWarnings += ::QObject::tr("Flight modes settings on mixers not exported") + "\n";
    }
    sOffset = c9x.sOffset;
  }
  else {
    memset(this, 0, sizeof(t_Open9xArmMixData_v208));
  }
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
    else if (srcRaw <= 9+9+O9X_ARM_NUM_CSW) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-9);
    }
    else if (srcRaw <= 9+9+O9X_ARM_NUM_CSW+NUM_CYC) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-10-9-O9X_ARM_NUM_CSW);
    }
    else if (srcRaw <= 9+9+O9X_ARM_NUM_CSW+NUM_CYC+NUM_PPM) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-10-9-O9X_ARM_NUM_CSW-NUM_CYC);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-10-9-O9X_ARM_NUM_CSW-NUM_CYC-NUM_PPM);
    }
    c9x.weight = weight;
    c9x.differential = differential*2;
    c9x.swtch = open9xArmToSwitch(swtch);
    c9x.curve = curve;
    c9x.delayUp = delayUp;
    c9x.delayDown = delayDown;
    c9x.speedUp = speedUp;
    c9x.speedDown = speedDown;
    c9x.carryTrim = carryTrim;
    c9x.mltpx = (MltpxValue)mltpx;
    c9x.mixWarn = mixWarn;
//    c9x.phase = phase;
    if (phase<0) {
      c9x.phases= 1 << (-phase -1);
    } else if (phase==0) {
      c9x.phases=0;
    } else {
      c9x.phases=511;
      c9x.phases &= ~(1 << (phase -1));
    }    
    c9x.sOffset = sOffset;
  }
  return c9x;
}

t_Open9xArmMixData_v209::t_Open9xArmMixData_v209(MixData &c9x)
{
  if (c9x.destCh) {
    destCh = c9x.destCh-1;
    mixWarn = c9x.mixWarn;
    swtch = open9xArmFromSwitch(c9x.swtch);
    if (c9x.srcRaw.type == SOURCE_TYPE_NONE) {
      srcRaw = 0;
      swtch = 0;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_STICK) {
      srcRaw = 1 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
      srcRaw = 8;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_TRIM) {
      srcRaw = 9 + c9x.srcRaw.index;
    }
    /* else if (c9x.srcRaw.type == SOURCE_TYPE_3POS) {
      srcRaw = 13;
    } */
    else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
      srcRaw = 13 + open9xArmFromSwitch(RawSwitch(c9x.srcRaw.index));
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
      srcRaw = 14+9+O9X_ARM_NUM_CSW + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
      srcRaw = 14+9+O9X_ARM_NUM_CSW+NUM_CYC + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
      srcRaw = 14+9+O9X_ARM_NUM_CSW+NUM_CYC+NUM_PPM + c9x.srcRaw.index;
    }
    weight = c9x.weight;
    differential = c9x.differential/2;
    curve = c9x.curve;
    delayUp = c9x.delayUp;
    delayDown = c9x.delayDown;
    speedUp = c9x.speedUp;
    speedDown = c9x.speedDown;
    carryTrim = c9x.carryTrim;
    mltpx = (MltpxValue)c9x.mltpx;
    int zeros=0;
    int ones=0;
    int phtemp=c9x.phases;
    for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
      if (phtemp & 1) {
        ones++;
      } else {
        zeros++;
      }
      phtemp >>=1;
    }
    if (zeros==O9X_ARM_MAX_PHASES || zeros==0) {
      phase=0;
    } else if (zeros==1) {
      int phtemp=c9x.phases;
      int ph=0;
      for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
        if ((phtemp & 1)==0) {
          ph=i;
          break;
        }
        phtemp >>=1;
      }
      phase=ph+1;
    } else if (ones==1) {
      int phtemp=c9x.phases;
      int ph=0;
      for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
        if (phtemp & 1) {
          ph=i;
          break;
        }
        phtemp >>=1;
      }
      phase=-(ph+1);
    } else {
      phase=0;
      EEPROMWarnings += ::QObject::tr("Flight modes settings on mixers not exported") + "\n";
    }
    sOffset = c9x.sOffset;
  }
  else {
    memset(this, 0, sizeof(t_Open9xArmMixData_v209));
  }
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
    else if (srcRaw <= 13+9+O9X_ARM_NUM_CSW) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-13);
    }
    else if (srcRaw <= 13+9+O9X_ARM_NUM_CSW+NUM_CYC) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-14-9-O9X_ARM_NUM_CSW);
    }
    else if (srcRaw <= 13+9+O9X_ARM_NUM_CSW+NUM_CYC+NUM_PPM) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-14-9-O9X_ARM_NUM_CSW-NUM_CYC);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-14-9-O9X_ARM_NUM_CSW-NUM_CYC-NUM_PPM);
    }
    c9x.weight = weight;
    c9x.differential = differential*2;
    c9x.swtch = open9xArmToSwitch(swtch);
    c9x.curve = curve;
    c9x.delayUp = delayUp;
    c9x.delayDown = delayDown;
    c9x.speedUp = speedUp;
    c9x.speedDown = speedDown;
    c9x.carryTrim = carryTrim;
    c9x.mltpx = (MltpxValue)mltpx;
    c9x.mixWarn = mixWarn;
   // c9x.phase = phase;
    if (phase<0) {
      c9x.phases= 1 << (-phase -1);
    } else if (phase==0) {
      c9x.phases=0;
    } else {
      c9x.phases=511;
      c9x.phases &= ~(1 << (phase -1));
    }    
    c9x.sOffset = sOffset;
  }
  return c9x;
}

t_Open9xArmMixData_v210::t_Open9xArmMixData_v210(MixData &c9x)
{
  if (c9x.destCh) {
    destCh = c9x.destCh-1;
    mixWarn = c9x.mixWarn;
    swtch = open9xArmFromSwitch(c9x.swtch);
    if (c9x.srcRaw.type == SOURCE_TYPE_NONE) {
      srcRaw = 0;
      swtch = 0;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_STICK) {
      srcRaw = 1 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_TRIM) {
      srcRaw = 8 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
      srcRaw = 12;
    }
    /* else if (c9x.srcRaw.type == SOURCE_TYPE_3POS) {
      srcRaw = 13;
    } */
    else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
      srcRaw = 13 + open9xArmFromSwitch(RawSwitch(c9x.srcRaw.index));
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
      srcRaw = 14+9+O9X_ARM_NUM_CSW + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
      srcRaw = 14+9+O9X_ARM_NUM_CSW+NUM_CYC + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
      srcRaw = 14+9+O9X_ARM_NUM_CSW+NUM_CYC+NUM_PPM + c9x.srcRaw.index;
    }
    weight = c9x.weight;
    differential = c9x.differential/2;
    curve = c9x.curve;
    delayUp = c9x.delayUp;
    delayDown = c9x.delayDown;
    speedUp = c9x.speedUp;
    speedDown = c9x.speedDown;
    carryTrim = c9x.carryTrim;
    mltpx = (MltpxValue)c9x.mltpx;
    int zeros=0;
    int ones=0;
    int phtemp=c9x.phases;
    for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
      if (phtemp & 1) {
        ones++;
      } else {
        zeros++;
      }
      phtemp >>=1;
    }
    if (zeros==O9X_ARM_MAX_PHASES || zeros==0) {
      phase=0;
    } else if (zeros==1) {
      int phtemp=c9x.phases;
      int ph=0;
      for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
        if ((phtemp & 1)==0) {
          ph=i;
          break;
        }
        phtemp >>=1;
      }
      phase=ph+1;
    } else if (ones==1) {
      int phtemp=c9x.phases;
      int ph=0;
      for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
        if (phtemp & 1) {
          ph=i;
          break;
        }
        phtemp >>=1;
      }
      phase=-(ph+1);
    } else {
      phase=0;
      EEPROMWarnings += ::QObject::tr("Flight modes settings on mixers not exported") + "\n";
    }
    sOffset = c9x.sOffset;
    setEEPROMZString(name, c9x.name, sizeof(name));
  }
  else {
    memset(this, 0, sizeof(t_Open9xArmMixData_v210));
  }
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
    else if (srcRaw <= 13+9+O9X_ARM_NUM_CSW) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-13);
    }
    else if (srcRaw <= 13+9+O9X_ARM_NUM_CSW+NUM_CYC) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-14-9-O9X_ARM_NUM_CSW);
    }
    else if (srcRaw <= 13+9+O9X_ARM_NUM_CSW+NUM_CYC+NUM_PPM) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-14-9-O9X_ARM_NUM_CSW-NUM_CYC);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-14-9-O9X_ARM_NUM_CSW-NUM_CYC-NUM_PPM);
    }
    c9x.weight = weight;
    c9x.differential = differential*2;
    c9x.swtch = open9xArmToSwitch(swtch);
    c9x.curve = curve;
    c9x.delayUp = delayUp;
    c9x.delayDown = delayDown;
    c9x.speedUp = speedUp;
    c9x.speedDown = speedDown;
    c9x.carryTrim = carryTrim;
    c9x.mltpx = (MltpxValue)mltpx;
    c9x.mixWarn = mixWarn;
  //  c9x.phase = phase;
    if (phase<0) {
      c9x.phases= 1 << (-phase -1);
    } else if (phase==0) {
      c9x.phases=0;
    } else {
      c9x.phases=511;
      c9x.phases &= ~(1 << (phase -1));
    }
    c9x.sOffset = sOffset;
    getEEPROMZString(c9x.name, name, sizeof(name));
  }
  return c9x;
}

t_Open9xArmMixData_v212::t_Open9xArmMixData_v212(MixData &c9x)
{
  if (c9x.destCh) {
    destCh = c9x.destCh-1;
    mixWarn = c9x.mixWarn;
    swtch = open9xArmFromSwitch(c9x.swtch);
    if (c9x.srcRaw.type == SOURCE_TYPE_NONE) {
      srcRaw = 0;
      swtch = 0;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_STICK) {
      srcRaw = 1 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_ROTARY_ENCODER) {
      srcRaw = 8;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_TRIM) {
      srcRaw = 9 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
      srcRaw = 13;
    }
    /* else if (c9x.srcRaw.type == SOURCE_TYPE_3POS) {
      srcRaw = 14;
    } */
    else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
      srcRaw = 14 + open9xArmFromSwitch(RawSwitch(c9x.srcRaw.index));
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
      srcRaw = 15+9+O9X_ARM_NUM_CSW + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
      srcRaw = 15+9+O9X_ARM_NUM_CSW+NUM_CYC + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
      srcRaw = 15+9+O9X_ARM_NUM_CSW+NUM_CYC+NUM_PPM + c9x.srcRaw.index;
    }
    weight = c9x.weight;
    if (c9x.curve==0) {
      curveMode=0;
      curveParam=c9x.differential;
    } else {
      curveMode=1;
      curveParam = c9x.curve;
    }
    delayUp = c9x.delayUp;
    delayDown = c9x.delayDown;
    speedUp = c9x.speedUp;
    speedDown = c9x.speedDown;
    carryTrim = c9x.carryTrim;
    noExpo = c9x.noExpo;
    mltpx = (MltpxValue)c9x.mltpx;
    phases = c9x.phases;
    sOffset = c9x.sOffset;
    setEEPROMZString(name, c9x.name, sizeof(name));
  }
  else {
    memset(this, 0, sizeof(t_Open9xArmMixData_v210));
  }
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
    else if (srcRaw <= 14+9+O9X_ARM_NUM_CSW) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, srcRaw-14);
    }
    else if (srcRaw <= 14+9+O9X_ARM_NUM_CSW+NUM_CYC) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-15-9-O9X_ARM_NUM_CSW);
    }
    else if (srcRaw <= 14+9+O9X_ARM_NUM_CSW+NUM_CYC+NUM_PPM) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-15-9-O9X_ARM_NUM_CSW-NUM_CYC);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-15-9-O9X_ARM_NUM_CSW-NUM_CYC-NUM_PPM);
    }
    c9x.weight = weight;
    if (curveMode==0) {
      c9x.differential = curveParam;
      c9x.curve=0;
    } else {
      c9x.curve=curveParam;
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
    c9x.phases = phases;
    c9x.sOffset = sOffset;
    getEEPROMZString(c9x.name, name, sizeof(name));
  }
  return c9x;
}

t_Open9xArmCustomSwData_v208::t_Open9xArmCustomSwData_v208(CustomSwData &c9x)
{
  func = c9x.func;
  v1 = c9x.val1;
  v2 = c9x.val2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    v1 = open9xArm208FromSource(RawSource(c9x.val1));
  }

  if (c9x.func >= CS_FN_EQUAL) {
    v2 = open9xArm208FromSource(RawSource(c9x.val2));
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    v1 = open9xArmFromSwitch(RawSwitch(c9x.val1));
    v2 = open9xArmFromSwitch(RawSwitch(c9x.val2));
  }
  
  if (func>O9X_ARM_MAX_CSFUNCOLD ) {
    EEPROMWarnings += ::QObject::tr("This version of open9x does not support Custom Switch function %1").arg(getFuncName(func)) + "\n";
    func=0;
    v1=0;
    v2=0;
  }    
}

t_Open9xArmCustomSwData_v208::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    c9x.val1 = open9xArm208ToSource(v1).toValue();
  }

  if (c9x.func >= CS_FN_EQUAL) {
    c9x.val2 = open9xArm208ToSource(v2).toValue();
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    c9x.val1 = open9xArmToSwitch(v1).toValue();
    c9x.val2 = open9xArmToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xArmCustomSwData_v209::t_Open9xArmCustomSwData_v209(CustomSwData &c9x)
{
  func = c9x.func;
  v1 = c9x.val1;
  v2 = c9x.val2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    v1 = open9xArm209FromSource(RawSource(c9x.val1));
  }

  if (c9x.func >= CS_FN_EQUAL) {
    v2 = open9xArm209FromSource(RawSource(c9x.val2));
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    v1 = open9xArmFromSwitch(RawSwitch(c9x.val1));
    v2 = open9xArmFromSwitch(RawSwitch(c9x.val2));
  }
  
  if (func>O9X_ARM_MAX_CSFUNCOLD ) {
    EEPROMWarnings += ::QObject::tr("This version of open9x does not support Custom Switch function %1").arg(getFuncName(func)) + "\n";
    func=0;
    v1=0;
    v2=0;
  }    
}

t_Open9xArmCustomSwData_v209::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    c9x.val1 = open9xArm209ToSource(v1).toValue();
  }

  if (c9x.func >= CS_FN_EQUAL) {
    c9x.val2 = open9xArm209ToSource(v2).toValue();
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    c9x.val1 = open9xArmToSwitch(v1).toValue();
    c9x.val2 = open9xArmToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xArmCustomSwData_v210::t_Open9xArmCustomSwData_v210(CustomSwData &c9x)
{
  func = c9x.func;
  v1 = c9x.val1;
  v2 = c9x.val2;

  delay = c9x.delay;
  duration = c9x.duration;
  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    v1 = open9xArm210FromSource(RawSource(c9x.val1));
  }

  if (c9x.func >= CS_FN_EQUAL && c9x.func <= CS_FN_ELESS) {
    v2 = open9xArm210FromSource(RawSource(c9x.val2));
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    v1 = open9xArmFromSwitch(RawSwitch(c9x.val1));
    v2 = open9xArmFromSwitch(RawSwitch(c9x.val2));
  }
}

t_Open9xArmCustomSwData_v210::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;
  c9x.delay = delay;
  c9x.duration = duration;
  
  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    c9x.val1 = open9xArm210ToSource(v1).toValue();
  }

  if (c9x.func >= CS_FN_EQUAL && c9x.func <= CS_FN_ELESS) {
    c9x.val2 = open9xArm210ToSource(v2).toValue();
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    c9x.val1 = open9xArmToSwitch(v1).toValue();
    c9x.val2 = open9xArmToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xArmFuncSwData_v208::t_Open9xArmFuncSwData_v208(FuncSwData &c9x)
{
  swtch = open9xArmFromSwitch(c9x.swtch);
  if (c9x.func <= FuncSafetyCh16) {
    param = ((c9x.param>>1)<<1);
    param |=(c9x.enabled & 0x01);
  } else {
    param = c9x.param;
    if ((c9x.func == FuncPlayValue || c9x.func == FuncVolume) && param > 7) {
      param -= -2;
    }
  }
  func = c9x.func;  
}

t_Open9xArmFuncSwData_v208::operator FuncSwData ()
{
  FuncSwData c9x;
  c9x.swtch = open9xArmToSwitch(swtch);
  c9x.func = (AssignFunc)(func);
  if (c9x.func <= FuncSafetyCh16) {
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

t_Open9xArmFuncSwData_v210::t_Open9xArmFuncSwData_v210(FuncSwData &c9x)
{
  swtch = open9xArmFromSwitch(c9x.swtch);
  uint32_t value;
  if (c9x.func <= FuncInstantTrim) {
    value = ((c9x.param>>1)<<1);
    value |=(c9x.enabled & 0x01);
  }
  else {
    if (c9x.func == FuncPlayPrompt || c9x.func == FuncBackgroundMusic) {
      memcpy(param, c9x.paramarm, sizeof(param));
    }
    else {
     value = c9x.param;
     if ((c9x.func == FuncPlayValue || c9x.func == FuncVolume) && value > 7) {
       value-=2;
     }
     *((uint32_t *)param) = value;
    }
  }
  func = c9x.func;
}

t_Open9xArmFuncSwData_v210::operator FuncSwData ()
{
  FuncSwData c9x;
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

t_Open9xArmFuncSwData_v211::t_Open9xArmFuncSwData_v211(FuncSwData &c9x)
{
  swtch = open9xArmFromSwitch(c9x.swtch);
  uint32_t value;
  if (c9x.func <= FuncInstantTrim) {
     value = c9x.param;
     *((uint32_t *)param) = value;
    delay=(c9x.enabled & 0x01);
  }
  else {
    if (c9x.func == FuncPlayPrompt || c9x.func == FuncBackgroundMusic) {
      memcpy(param, c9x.paramarm, sizeof(param));
    }
    else {
      value = c9x.param;
      if ((c9x.func == FuncPlayValue || c9x.func == FuncVolume || (c9x.func >= FuncAdjustGV1 && c9x.func <= FuncAdjustGV5)) && value > 7) {
        value--;
      }
      *((uint32_t *)param) = value;
    }
  }
  func = c9x.func;
}

t_Open9xArmFuncSwData_v211::operator FuncSwData ()
{
  FuncSwData c9x;
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
      if ((c9x.func == FuncPlayValue || c9x.func == FuncVolume || (c9x.func >= FuncAdjustGV1 && c9x.func <= FuncAdjustGV5)) && value > 7) {
        c9x.param++;
      } 
    }
  }
  return c9x;
}

t_Open9xArmSwashRingData_v208::t_Open9xArmSwashRingData_v208(SwashRingData &c9x)
{
  invertELE = c9x.invertELE;
  invertAIL = c9x.invertAIL;
  invertCOL = c9x.invertCOL;
  type = c9x.type;
  collectiveSource = open9xArm208FromSource(c9x.collectiveSource);
  value = c9x.value;
}

t_Open9xArmSwashRingData_v208::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.invertELE = invertELE;
  c9x.invertAIL = invertAIL;
  c9x.invertCOL = invertCOL;
  c9x.type = type;
  c9x.collectiveSource = open9xArm208ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xArmSwashRingData_v209::t_Open9xArmSwashRingData_v209(SwashRingData &c9x)
{
  invertELE = c9x.invertELE;
  invertAIL = c9x.invertAIL;
  invertCOL = c9x.invertCOL;
  type = c9x.type;
  collectiveSource = open9xArm209FromSource(c9x.collectiveSource);
  value = c9x.value;
}

t_Open9xArmSwashRingData_v209::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.invertELE = invertELE;
  c9x.invertAIL = invertAIL;
  c9x.invertCOL = invertCOL;
  c9x.type = type;
  c9x.collectiveSource = open9xArm209ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xArmSwashRingData_v210::t_Open9xArmSwashRingData_v210(SwashRingData &c9x)
{
  invertELE = c9x.invertELE;
  invertAIL = c9x.invertAIL;
  invertCOL = c9x.invertCOL;
  type = c9x.type;
  collectiveSource = open9xArm210FromSource(c9x.collectiveSource);
  value = c9x.value;
}

t_Open9xArmSwashRingData_v210::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.invertELE = invertELE;
  c9x.invertAIL = invertAIL;
  c9x.invertCOL = invertCOL;
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
  c9x.blades = blades;
  c9x.currentSource=currentSource;
  c9x.screens[0].type = 1;
  for (int i=0; i<4; i++) {
    c9x.screens[0].body.bars[i].source = bars[i].source;
    c9x.screens[0].body.bars[i].barMin = bars[i].barMin;
    c9x.screens[0].body.bars[i].barMax = bars[i].barMax;
  }
  c9x.rssiAlarms[0] = rssiAlarms[0].get(0);
  c9x.rssiAlarms[1] = rssiAlarms[1].get(1);

  for (int line=0; line<4; line++) {
    for (int col=0; col<2; col++) {
      int i=line*2+col;
      c9x.screens[1].body.lines[line].source[col] = lines[i];
    }
  }

  return c9x;
}

t_Open9xArmFrSkyData_v210::t_Open9xArmFrSkyData_v210(FrSkyData &c9x)
{
  memset(this, 0, sizeof(t_Open9xArmFrSkyData_v210));
  channels[0] = c9x.channels[0];
  channels[1] = c9x.channels[1];
  usrProto = c9x.usrProto;
  voltsSource = c9x.voltsSource;
  blades = c9x.blades;
  currentSource=c9x.currentSource;
  for (int i=0; i<4; i++) {
    bars[i].source = c9x.screens[0].body.bars[i].source;
    bars[i].barMin = c9x.screens[0].body.bars[i].barMin;
    bars[i].barMax = c9x.screens[0].body.bars[i].barMax;
  }
  for (int line=0; line<4; line++) {
    for (int col=0; col<2; col++) {
      int i=line*2+col;
      lines[i]=c9x.screens[1].body.lines[line].source[col];
    }
  }
  rssiAlarms[0] = Open9xFrSkyRSSIAlarm(0, c9x.rssiAlarms[0]);
  rssiAlarms[1] = Open9xFrSkyRSSIAlarm(1, c9x.rssiAlarms[1]);
}


t_Open9xArmFrSkyData_v211::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  c9x.voltsSource = voltsSource;
  c9x.blades = blades;
  c9x.currentSource=currentSource;
  c9x.screens[0].type = 1;
  for (int i=0; i<4; i++) {
    c9x.screens[0].body.bars[i].source = bars[i].source;
    c9x.screens[0].body.bars[i].barMin = bars[i].barMin;
    c9x.screens[0].body.bars[i].barMax = bars[i].barMax;
  }
  c9x.rssiAlarms[0] = rssiAlarms[0].get(0);
  c9x.rssiAlarms[1] = rssiAlarms[1].get(1);

  for (int line=0; line<4; line++) {
    for (int col=0; col<2; col++) {
      int i=line*2+col;
      c9x.screens[1].body.lines[line].source[col] = lines[i];
    }
  }

  c9x.varioSource = varioSource;
  c9x.varioCenterMax = varioSpeedUpMin;
  c9x.varioCenterMin = varioSpeedDownMin;

  return c9x;
}

t_Open9xArmFrSkyData_v211::t_Open9xArmFrSkyData_v211(FrSkyData &c9x)
{
  memset(this, 0, sizeof(t_Open9xArmFrSkyData_v211));
  channels[0] = c9x.channels[0];
  channels[1] = c9x.channels[1];
  usrProto = c9x.usrProto;
  voltsSource = c9x.voltsSource;
  blades = c9x.blades;
  currentSource=c9x.currentSource;
  for (int i=0; i<4; i++) {
    bars[i].source = c9x.screens[0].body.bars[i].source;
    bars[i].barMin = c9x.screens[0].body.bars[i].barMin;
    bars[i].barMax = c9x.screens[0].body.bars[i].barMax;
  }
  for (int line=0; line<4; line++) {
    for (int col=0; col<2; col++) {
      int i=line*2+col;
      lines[i]=c9x.screens[1].body.lines[line].source[col];
    }
  }
  rssiAlarms[0] = Open9xFrSkyRSSIAlarm(0, c9x.rssiAlarms[0]);
  rssiAlarms[1] = Open9xFrSkyRSSIAlarm(1, c9x.rssiAlarms[1]);
  varioSource = c9x.varioSource;
  varioSpeedUpMin = c9x.varioCenterMax;
  varioSpeedDownMin = c9x.varioCenterMin;
}

t_Open9xArmModelData_v208::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<O9X_MAX_TIMERS; i++)
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
  c9x.trimInc = trimInc;
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_ARM_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<O9X_209_MAX_CURVE5; i++) {
    c9x.curves[i].custom = false;
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<O9X_209_MAX_CURVE9; i++) {
    c9x.curves[O9X_209_MAX_CURVE5 + i].custom = false;
    c9x.curves[O9X_209_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }
  for (int i=0; i<O9X_ARM_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_ARM_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.frsky.varioSource = varioSource;
  c9x.frsky.varioCenterMax = varioSpeedUpMin;
  c9x.frsky.varioCenterMin = varioSpeedDownMin;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;
  for (int line=0; line<4; line++) {
    for (int col=0; col<2; col++) {
      c9x.frsky.screens[1].body.lines[line].source[col] = (col==0 ? (frskyLines[line] & 0x0f) : ((frskyLines[line] & 0xf0) / 16));
      c9x.frsky.screens[1].body.lines[line].source[col] += (((frskyLinesXtra >> (4*line+2*col)) & 0x03) * 16);
    }
  }
  for (int i=0; i<O9X_NUM_CHNOUT; i++) {
    c9x.limitData[i].ppmCenter = servoCenter[i];
  }

  return c9x;
}

#define MODEL_DATA_SIZE_208 1882
t_Open9xArmModelData_v208::t_Open9xArmModelData_v208(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_208) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_208));
  }

  if (c9x.used) {
    setEEPROMZString(name, c9x.name, sizeof(name));
    for (int i=0; i<O9X_MAX_TIMERS; i++)
      timers[i] = c9x.timers[i];
    switch(c9x.moduleData[0].protocol) {
      case PPM:
        protocol = 0;
        break;
      case PPM16:
        protocol = 1;
        break;
      case PPMSIM:
        protocol = 2;
        break;
      case PXX_DJT:
        protocol = 3;
        break;
      case DSM2:
        protocol = 4;
        break;
      default:
        protocol = 0;
        EEPROMWarnings += ::QObject::tr("Open9x doesn't accept this protocol") + "\n";
        // TODO more explicit warning for each protocol
        break;
    }
    thrTrim = c9x.thrTrim;
    ppmNCH = (c9x.moduleData[0].channelsCount - 8) / 2;
    trimInc = c9x.trimInc;
    disableThrottleWarning=c9x.disableThrottleWarning;
    pulsePol = c9x.moduleData[0].ppmPulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.moduleData[0].ppmDelay - 300) / 50;
    beepANACenter = c9x.beepANACenter;
    for (int i=0; i<C9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
/* TODO   for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j]; */
    for (int i=0; i<O9X_ARM_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_ARM_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xArmFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
    varioSource = c9x.frsky.varioSource;
    varioSpeedUpMin = c9x.frsky.varioCenterMax;
    varioSpeedDownMin = c9x.frsky.varioCenterMin;

    ppmFrameLength = c9x.moduleData[0].ppmFrameLength;
    thrTraceSrc = c9x.thrTraceSrc;
    modelId = c9x.modelId;
    frskyLinesXtra=0;
    for (int j=0; j<4; j++) {
      frskyLines[j] = 0;
      for (int k=0; k<2; k++) {
        int value = c9x.frsky.screens[1].body.lines[j].source[k];
        frskyLines[j] |= (k==0 ? (value & 0x0f) : ((value & 0x0f) << 4));
        frskyLinesXtra |= (value / 16) << (4*j+2*k);
      }
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      servoCenter[i] = c9x.limitData[i].ppmCenter;
    }
  }
  else {
    memset(this, 0, sizeof(t_Open9xArmModelData_v208));
  }
}

t_Open9xArmModelData_v209::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<O9X_MAX_TIMERS; i++)
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
  c9x.trimInc = trimInc;
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_ARM_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<O9X_209_MAX_CURVE5; i++) {
    c9x.curves[i].custom = false;
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<O9X_209_MAX_CURVE9; i++) {
    c9x.curves[O9X_209_MAX_CURVE5 + i].custom = false;
    c9x.curves[O9X_209_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }
  for (int i=0; i<O9X_ARM_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_ARM_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.frsky.varioSource = varioSource;
  c9x.frsky.varioCenterMax = varioSpeedUpMin;
  c9x.frsky.varioCenterMin = varioSpeedDownMin;
  c9x.switchWarningStates = switchWarningStates;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;
  for (int line=0; line<4; line++) {
    for (int col=0; col<2; col++) {
      c9x.frsky.screens[1].body.lines[line].source[col] = (col==0 ? (frskyLines[line] & 0x0f) : ((frskyLines[line] & 0xf0) / 16));
      c9x.frsky.screens[1].body.lines[line].source[col] += (((frskyLinesXtra >> (4*line+2*col)) & 0x03) * 16);
    }
  }
  for (int i=0; i<O9X_NUM_CHNOUT; i++) {
    c9x.limitData[i].ppmCenter = servoCenter[i];
  }

  return c9x;
}

#define MODEL_DATA_SIZE_209 1883
t_Open9xArmModelData_v209::t_Open9xArmModelData_v209(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_209) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_209));
  }

  memset(this, 0, sizeof(t_Open9xArmModelData_v209));

  if (c9x.used) {
    setEEPROMZString(name, c9x.name, sizeof(name));
    for (int i=0; i<O9X_MAX_TIMERS; i++)
      timers[i] = c9x.timers[i];
    switch(c9x.moduleData[0].protocol) {
      case PPM:
        protocol = 0;
        break;
      case PPM16:
        protocol = 1;
        break;
      case PPMSIM:
        protocol = 2;
        break;
      case PXX_DJT:
        protocol = 3;
        break;
      case DSM2:
        protocol = 4;
        break;
      default:
        protocol = 0;
        EEPROMWarnings += ::QObject::tr("Open9x doesn't accept this protocol") + "\n";
        // TODO more explicit warning for each protocol
        break;
    }
    thrTrim = c9x.thrTrim;
    ppmNCH = (c9x.moduleData[0].channelsCount - 8) / 2;
    trimInc = c9x.trimInc;
    disableThrottleWarning=c9x.disableThrottleWarning;
    pulsePol = c9x.moduleData[0].ppmPulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.moduleData[0].ppmDelay - 300) / 50;
    beepANACenter = c9x.beepANACenter;
    for (int i=0; i<C9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    /* TODO for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j]; */
    for (int i=0; i<O9X_ARM_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_ARM_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xArmFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
    varioSource = c9x.frsky.varioSource;
    varioSpeedUpMin = c9x.frsky.varioCenterMax;
    varioSpeedDownMin = c9x.frsky.varioCenterMin;
    switchWarningStates = c9x.switchWarningStates;
    
    ppmFrameLength = c9x.moduleData[0].ppmFrameLength;
    thrTraceSrc = c9x.thrTraceSrc;
    modelId = c9x.modelId;
    frskyLinesXtra=0;
    for (int j=0; j<4; j++) {
      frskyLines[j] = 0;
      for (int k=0; k<2; k++) {
        int value = c9x.frsky.screens[1].body.lines[j].source[k];
        frskyLines[j] |= (k==0 ? (value & 0x0f) : ((value & 0x0f) << 4));
        frskyLinesXtra |= (value / 16) << (4*j+2*k);
      }
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      servoCenter[i] = c9x.limitData[i].ppmCenter;
    }
  }
}


t_Open9xArmModelData_v210::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<O9X_MAX_TIMERS; i++)
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
  c9x.trimInc = trimInc;
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_ARM_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<O9X_209_MAX_CURVE5; i++) {
    c9x.curves[i].custom = false;
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<O9X_209_MAX_CURVE9; i++) {
    c9x.curves[O9X_209_MAX_CURVE5 + i].custom = false;
    c9x.curves[O9X_209_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }
  for (int i=0; i<O9X_ARM_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_ARM_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.frsky.varioSource = varioSource;
  c9x.frsky.varioCenterMax = varioSpeedUpMin;
  c9x.frsky.varioCenterMin = varioSpeedDownMin;
  c9x.switchWarningStates = switchWarningStates;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;
  for (int i=0; i<O9X_NUM_CHNOUT; i++) {
    c9x.limitData[i].ppmCenter = servoCenter[i];
  }

  return c9x;
}

#define MODEL_DATA_SIZE_210 3085
t_Open9xArmModelData_v210::t_Open9xArmModelData_v210(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_210) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_210));
  }

  memset(this, 0, sizeof(t_Open9xArmModelData_v210));

  if (c9x.used) {
    setEEPROMZString(name, c9x.name, sizeof(name));
    for (int i=0; i<O9X_MAX_TIMERS; i++)
      timers[i] = c9x.timers[i];
    switch(c9x.moduleData[0].protocol) {
      case PPM:
        protocol = 0;
        break;
      case PPM16:
        protocol = 1;
        break;
      case PPMSIM:
        protocol = 2;
        break;
      case PXX_DJT:
        protocol = 3;
        break;
      case DSM2:
        protocol = 4;
        break;
      default:
        protocol = 0;
        EEPROMWarnings += ::QObject::tr("Open9x doesn't accept this protocol") + "\n";
        // TODO more explicit warning for each protocol
        break;
    }
    thrTrim = c9x.thrTrim;
    ppmNCH = (c9x.moduleData[0].channelsCount - 8) / 2;
    trimInc = c9x.trimInc;
    disableThrottleWarning=c9x.disableThrottleWarning;
    pulsePol = c9x.moduleData[0].ppmPulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.moduleData[0].ppmDelay - 300) / 50;
    beepANACenter = c9x.beepANACenter;
    for (int i=0; i<C9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    /* TODO for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j]; */
    for (int i=0; i<O9X_ARM_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_ARM_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xArmFromSwitch(c9x.safetySw[i].swtch);
        *((uint32_t *)funcSw[count].param) = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
    varioSource = c9x.frsky.varioSource;
    varioSpeedUpMin = c9x.frsky.varioCenterMax;
    varioSpeedDownMin = c9x.frsky.varioCenterMin;
    switchWarningStates = c9x.switchWarningStates;

    ppmFrameLength = c9x.moduleData[0].ppmFrameLength;
    thrTraceSrc = c9x.thrTraceSrc;
    modelId = c9x.modelId;

    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      servoCenter[i] = c9x.limitData[i].ppmCenter;
    }
  }
}

t_Open9xArmModelData_v211::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<O9X_MAX_TIMERS; i++)
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
  c9x.trimInc = trimInc;
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_ARM_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<O9X_ARM_MAX_CURVES; i++) {
    CurveInfo crvinfo = curveinfo(this, i);
    c9x.curves[i].custom = crvinfo.custom;
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
  for (int i=0; i<O9X_ARM_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_ARM_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.switchWarningStates = switchWarningStates;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;
  for (int i=0; i<O9X_NUM_CHNOUT; i++) {
    c9x.limitData[i].ppmCenter = servoCenter[i];
  }

  return c9x;
}

#define MODEL_DATA_SIZE_ARM_211 3585
t_Open9xArmModelData_v211::t_Open9xArmModelData_v211(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_ARM_211) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_ARM_211));
  }

  memset(this, 0, sizeof(t_Open9xArmModelData_v211));

  if (c9x.used) {
    setEEPROMZString(name, c9x.name, sizeof(name));
    for (int i=0; i<O9X_MAX_TIMERS; i++)
      timers[i] = c9x.timers[i];
    switch(c9x.moduleData[0].protocol) {
      case PPM:
        protocol = 0;
        break;
      case PPM16:
        protocol = 1;
        break;
      case PPMSIM:
        protocol = 2;
        break;
      case PXX_DJT:
        protocol = 3;
        break;
      case DSM2:
        protocol = 4;
        break;
      default:
        protocol = 0;
        EEPROMWarnings += ::QObject::tr("Open9x doesn't accept this protocol") + "\n";
        // TODO more explicit warning for each protocol
        break;
    }
    thrTrim = c9x.thrTrim;
    ppmNCH = (c9x.moduleData[0].channelsCount - 8) / 2;
    trimInc = c9x.trimInc;
    disableThrottleWarning=c9x.disableThrottleWarning;
    pulsePol = c9x.moduleData[0].ppmPulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.moduleData[0].ppmDelay - 300) / 50;
    beepANACenter = c9x.beepANACenter;
    for (int i=0; i<C9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];

    int8_t * cur = &points[0];
    int offset = 0;
    for (int i=0; i<O9X_ARM_MAX_CURVES; i++) {
      offset += (c9x.curves[i].custom ? c9x.curves[i].count * 2 - 2 : c9x.curves[i].count) - 5;
      if (offset > O9X_ARM_NUM_POINTS - 5 * O9X_ARM_MAX_CURVES) {
        EEPROMWarnings += ::QObject::tr("open9x only accepts %1 points in all curves").arg(O9X_ARM_NUM_POINTS) + "\n";
        break;
      }
      curves[i] = offset;
      for (int j=0; j<c9x.curves[i].count; j++) {
        *cur++ = c9x.curves[i].points[j].y;
      }
      if (c9x.curves[i].custom) {
        for (int j=1; j<c9x.curves[i].count-1; j++) {
          *cur++ = c9x.curves[i].points[j].x;
        }
      }
    }

    for (int i=0; i<O9X_ARM_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_ARM_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xArmFromSwitch(c9x.safetySw[i].swtch);
        *((uint32_t *)funcSw[count].param) = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
    switchWarningStates = c9x.switchWarningStates;

    ppmFrameLength = c9x.moduleData[0].ppmFrameLength;
    thrTraceSrc = c9x.thrTraceSrc;
    modelId = c9x.modelId;

    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      servoCenter[i] = c9x.limitData[i].ppmCenter;
    }
  }
}

t_Open9xArmModelData_v212::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<O9X_MAX_TIMERS; i++) {
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
  c9x.trimInc = trimInc;
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_ARM_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<O9X_ARM_MAX_CURVES; i++) {
    CurveInfo crvinfo = curveinfo(this, i);
    c9x.curves[i].custom = crvinfo.custom;
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
  for (int i=0; i<O9X_ARM_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_ARM_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.switchWarningStates = switchWarningStates;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;
  return c9x;
}

#define MODEL_DATA_SIZE_212 3159
t_Open9xArmModelData_v212::t_Open9xArmModelData_v212(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_212) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_212));
  }

  memset(this, 0, sizeof(t_Open9xArmModelData_v212));

  if (c9x.used) {
    setEEPROMZString(name, c9x.name, sizeof(name));
    for (int i=0; i<O9X_MAX_TIMERS; i++) {
      timers[i] = c9x.timers[i];
      timersXtra[i].value=c9x.timers[i].val;
      timersXtra[i].remanent=c9x.timers[i].persistent;
    }
    switch(c9x.moduleData[0].protocol) {
      case PPM:
        protocol = 0;
        break;
      case PPM16:
        protocol = 1;
        break;
      case PPMSIM:
        protocol = 2;
        break;
      case PXX_DJT:
        protocol = 3;
        break;
      case DSM2:
        protocol = 4;
        break;
      default:
        protocol = 0;
        EEPROMWarnings += ::QObject::tr("Open9x doesn't accept this protocol") + "\n";
        // TODO more explicit warning for each protocol
        break;
    }
    thrTrim = c9x.thrTrim;
    ppmNCH = (c9x.moduleData[0].channelsCount - 8) / 2;
    trimInc = c9x.trimInc;
    disableThrottleWarning=c9x.disableThrottleWarning;
    pulsePol = c9x.moduleData[0].ppmPulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare1 = 0;
    ppmDelay = (c9x.moduleData[0].ppmDelay - 300) / 50;
    beepANACenter = c9x.beepANACenter;
    for (int i=0; i<C9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];

    int8_t * cur = &points[0];
    int offset = 0;
    for (int i=0; i<O9X_ARM_MAX_CURVES; i++) {
      offset += (c9x.curves[i].custom ? c9x.curves[i].count * 2 - 2 : c9x.curves[i].count) - 5;
      if (offset > O9X_ARM_NUM_POINTS - 5 * O9X_ARM_MAX_CURVES) {
        EEPROMWarnings += ::QObject::tr("open9x only accepts %1 points in all curves").arg(O9X_ARM_NUM_POINTS) + "\n";
        break;
      }
      curves[i] = offset;
      for (int j=0; j<c9x.curves[i].count; j++) {
        *cur++ = c9x.curves[i].points[j].y;
      }
      if (c9x.curves[i].custom) {
        for (int j=1; j<c9x.curves[i].count-1; j++) {
          *cur++ = c9x.curves[i].points[j].x;
        }
      }
    }

    for (int i=0; i<O9X_ARM_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_ARM_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xArmFromSwitch(c9x.safetySw[i].swtch);
        *((uint32_t *)funcSw[count].param) = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
    switchWarningStates = c9x.switchWarningStates;

    ppmFrameLength = c9x.moduleData[0].ppmFrameLength;
    thrTraceSrc = c9x.thrTraceSrc;
    modelId = c9x.modelId;
  }
}
