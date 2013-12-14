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
    if(cc==0) return 0;
    if(cc==c) return 37+i;
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

int8_t open9xStockFromSwitch(const RawSwitch & sw)
{
  switch (sw.type) {
    case SWITCH_TYPE_SWITCH:
      return sw.index;
    case SWITCH_TYPE_VIRTUAL:
      return sw.index > 0 ? (9 + sw.index) : (-9 + sw.index);
    case SWITCH_TYPE_ON:
      return 22;
    case SWITCH_TYPE_OFF:
      return -22;
    case SWITCH_TYPE_MOMENT_SWITCH:
      return sw.index > 0 ? (22 + sw.index) : (-22 + sw.index);
    case SWITCH_TYPE_MOMENT_VIRTUAL:
      return sw.index > 0 ? (31 + sw.index) : (-31 + sw.index);
    case SWITCH_TYPE_ONM:
      return 44;
    default:
      return 0;
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
    return RawSwitch(SWITCH_TYPE_ONM);
  else if (sw == -22)
    return RawSwitch(SWITCH_TYPE_OFF);
  else if (swa <= 22+9)
    return RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, sw > 0 ? sw-22 : sw+22);
  else
    return RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, sw > 0 ? sw-22-9 : sw+22+9);
}

t_Open9xExpoData_v201::t_Open9xExpoData_v201(ExpoData &c9x)
{
  mode = c9x.mode;
  chn = c9x.chn;
  // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
  curve = 0;
  expo = 0;
  if (c9x.curveMode == 1) {
    if (c9x.curveParam >=0 && c9x.curveParam <= 10)
      curve = c9x.curveParam;
    else if (c9x.curveParam >= 15 && c9x.curveParam <= 19)
      curve = c9x.curveParam - 4;
    else
      EEPROMWarnings += ::QObject::tr("Open9x doesn't allow Curve%1 in expos").arg(c9x.curveParam-6) + "\n";
  }
  else {
    expo=c9x.expo;
  }
  swtch = open9xStockFromSwitch(c9x.swtch);
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
    negPhase=0;
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
    negPhase=0;
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
    phase=(ph+1);
    negPhase=1;
  } else {
    phase=0;
    EEPROMWarnings += ::QObject::tr("Flight modes settings on expos not exported") + "\n";
  }
  weight = c9x.weight;
}

t_Open9xExpoData_v201::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;
  if (expo!=0 && curve!=0) {
    EEPROMWarnings += ::QObject::tr("Simultaneous usage of expo and curves is no longer supported in open9x") + "\n";
  }
  else {
    if (curve == 0) {
      c9x.curveMode = 0;
      c9x.curveParam = expo;
    }
    else {
      c9x.curveMode = 1;
      if (curve <= 10)
        c9x.curveParam = curve;
      else
        c9x.curveParam = curve + 4;
    }
  }
  c9x.swtch = open9xStockToSwitch(swtch);
  if (negPhase) {
    c9x.phases= 1 << (phase -1);
  }
  else if (phase==0) {
    c9x.phases=0;
  }
  else {
    c9x.phases=63;
    c9x.phases &= ~(1 << (phase -1));
  }  
  c9x.weight = weight;
  return c9x;
}

t_Open9xExpoData_v211::t_Open9xExpoData_v211(ExpoData &c9x)
{
  mode = c9x.mode;
  chn = c9x.chn;
  // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13

  swtch = open9xStockFromSwitch(c9x.swtch);
  phases = c9x.phases;
  weight = c9x.weight;
  curveMode = c9x.curveMode;
  curveParam = c9x.curveParam;
}

t_Open9xExpoData_v211::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;
  c9x.curveMode = curveMode;
  c9x.curveParam = curveParam;
  c9x.swtch = open9xStockToSwitch(swtch);
  c9x.phases = phases;
  c9x.weight = weight;
  return c9x;
}

t_Open9xLimitData_v201::t_Open9xLimitData_v201(LimitData &c9x)
{
  min = c9x.min+100;
  max = c9x.max-100;
  revert = c9x.revert;
  offset = c9x.offset;
}

t_Open9xLimitData_v201::operator LimitData ()
{
  LimitData c9x;
  c9x.min = min-100;
  c9x.max = max+100;
  c9x.revert = revert;
  c9x.offset = offset;
  return c9x;
}

t_Open9xLimitData_v211::t_Open9xLimitData_v211(LimitData &c9x)
{
  min = c9x.min+100;
  max = c9x.max-100;
  revert = c9x.revert;
  offset = c9x.offset;
  ppmCenter = c9x.ppmCenter;
  symetrical = c9x.symetrical;
}

t_Open9xLimitData_v211::operator LimitData ()
{
  LimitData c9x;
  c9x.min = min-100;
  c9x.max = max+100;
  c9x.revert = revert;
  c9x.offset = offset;
  c9x.symetrical=symetrical;
  c9x.ppmCenter=ppmCenter;
  return c9x;
}

t_Open9xMixData_v201::t_Open9xMixData_v201(MixData &c9x)
{
  destCh = c9x.destCh;
  mixWarn = c9x.mixWarn;
  swtch = open9xStockFromSwitch(c9x.swtch);

  if (c9x.srcRaw.type == SOURCE_TYPE_NONE) {
    srcRaw = 0;
    swtch = 0;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_STICK) {
    srcRaw = 1 + c9x.srcRaw.index;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings += ::QObject::tr("open9x on this board doesn't have Rotary Encoders") + "\n";
    srcRaw = 5 + c9x.srcRaw.index; // use pots instead
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
    srcRaw = 8; // MAX
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
    srcRaw = 9; // FULL
    swtch = open9xStockFromSwitch(RawSwitch(c9x.srcRaw.index));
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

  weight = c9x.weight;
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
    EEPROMWarnings += ::QObject::tr("Flight modes settings on mixers not exported") + "\n";
  }
  sOffset = c9x.sOffset;
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
    c9x.phases=63;
    c9x.phases &= ~(1 << (phase -1));
  }  
  c9x.sOffset = sOffset;
  return c9x;
}

t_Open9xMixData_v203::t_Open9xMixData_v203(MixData &c9x)
{
  destCh = c9x.destCh;
  mixWarn = c9x.mixWarn;
  swtch = open9xStockFromSwitch(c9x.swtch);

  if (c9x.srcRaw.type == SOURCE_TYPE_NONE) {
    srcRaw = 0;
    swtch = 0;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_STICK) {
    srcRaw = 1 + c9x.srcRaw.index;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings += ::QObject::tr("open9x on this board doesn't have Rotary Encoders") + "\n";
    srcRaw = 5 + c9x.srcRaw.index; // use pots instead
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
    srcRaw = 8; // MAX
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
    srcRaw = 9; // FULL
    swtch = open9xStockFromSwitch(RawSwitch(c9x.srcRaw.index));
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

  weight = c9x.weight;
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
    EEPROMWarnings += ::QObject::tr("Flight modes settings on mixers not exported") + "\n";
  }
  sOffset = c9x.sOffset;
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
    c9x.phases=63;
    c9x.phases &= ~(1 << (phase -1));
  } 
  c9x.sOffset = sOffset;
  return c9x;
}

t_Open9xMixData_v205::t_Open9xMixData_v205(MixData &c9x)
{
  if (c9x.destCh) {
    destCh = c9x.destCh-1;
    mixWarn = c9x.mixWarn;
    swtch = open9xStockFromSwitch(c9x.swtch);

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
    /* else if (c9x.srcRaw.type == SOURCE_TYPE_3POS) {
      srcRaw = 9;
    } */
    else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
      srcRaw = 9 + open9xStockFromSwitch(RawSwitch(c9x.srcRaw.index));
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
      srcRaw = 31 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
      srcRaw = 34 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
      srcRaw = 42 + c9x.srcRaw.index;
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
      EEPROMWarnings += ::QObject::tr("Flight modes settings on mixers not exported") + "\n";
    }
    sOffset = c9x.sOffset;
  }
  else {
    memset(this, 0, sizeof(t_Open9xMixData_v205));
  }
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
    c9x.differential = differential*2;
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
      c9x.phases=63;
      c9x.phases &= ~(1 << (phase -1));
    }
    c9x.sOffset = sOffset;
  }
  return c9x;
}

t_Open9xMixData_v209::t_Open9xMixData_v209(MixData &c9x)
{
  if (c9x.destCh) {
    destCh = c9x.destCh-1;
    mixWarn = c9x.mixWarn;
    swtch = open9xStockFromSwitch(c9x.swtch);

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
    else if (c9x.srcRaw.type == SOURCE_TYPE_TRIM) {
      srcRaw = 8 + c9x.srcRaw.index; // use pots instead
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
      srcRaw = 12; // MAX
    }
    /* else if (c9x.srcRaw.type == SOURCE_TYPE_3POS) {
      srcRaw = 13;
    } */
    else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
      srcRaw = 13 + open9xStockFromSwitch(RawSwitch(c9x.srcRaw.index));
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
      srcRaw = 35 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
      srcRaw = 38 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
      srcRaw = 46 + c9x.srcRaw.index;
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
      EEPROMWarnings += ::QObject::tr("Flight modes settings on mixers not exported") + "\n";
    }
    sOffset = c9x.sOffset;
  }
  else {
    memset(this, 0, sizeof(t_Open9xMixData_v209));
  }
}

t_Open9xMixData_v209::operator MixData ()
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
    c9x.differential = differential*2;
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
      c9x.phases=63;
      c9x.phases &= ~(1 << (phase -1));
    }
    c9x.sOffset = sOffset;
  }
  return c9x;
}

t_Open9xMixData_v211::t_Open9xMixData_v211(MixData &c9x)
{
  if (c9x.destCh) {
    destCh = c9x.destCh-1;
    mixWarn = c9x.mixWarn;
    swtch = open9xStockFromSwitch(c9x.swtch);

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
    else if (c9x.srcRaw.type == SOURCE_TYPE_TRIM) {
      srcRaw = 8 + c9x.srcRaw.index; // use pots instead
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
      srcRaw = 12; // MAX
    }
    /* else if (c9x.srcRaw.type == SOURCE_TYPE_3POS) {
      srcRaw = 13;
    } */
    else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
      srcRaw = 13 + open9xStockFromSwitch(RawSwitch(c9x.srcRaw.index));
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
      srcRaw = 35 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
      srcRaw = 38 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
      srcRaw = 46 + c9x.srcRaw.index;
    }

    weight = c9x.weight;
    if (c9x.curve==0) {
      curveMode = 0;
      curveParam = c9x.differential;
    }
    else {
      curveMode = 1;
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
  }
  else {
    memset(this, 0, sizeof(t_Open9xMixData_v211));
  }
}

t_Open9xMixData_v211::operator MixData ()
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
      c9x.differential = curveParam;
      c9x.curve=0;
    } else {
      c9x.curve=curveParam;
    }    
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
  }
  return c9x;
}

int8_t open9x208FromSource(RawSource source)
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
    v1 = 36+source.index;
  return v1;
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

int8_t open9xStock209FromSource(RawSource source)
{
  int v1 = 0;
  if (source.type == SOURCE_TYPE_STICK)
    v1 = 1+source.index;
  else if (source.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings += ::QObject::tr("Open9x on this board doesn't have Rotary Encoders") + "\n";
    v1 = 5+source.index;
  }
  else if (source.type == SOURCE_TYPE_TRIM)
    v1 = 8+source.index;
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
    v1 = 40+source.index;
  return v1;
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

t_Open9xCustomSwData_v208::t_Open9xCustomSwData_v208(CustomSwData &c9x)
{
  func = c9x.func;
  v1 = c9x.val1;
  v2 = c9x.val2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    v1 = open9x208FromSource(RawSource(c9x.val1));
  }

  if (c9x.func >= CS_FN_EQUAL) {
    v2 = open9x208FromSource(RawSource(c9x.val2));
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    v1 = open9xStockFromSwitch(RawSwitch(c9x.val1));
    v2 = open9xStockFromSwitch(RawSwitch(c9x.val2));
  }

  if (func>O9X_MAX_CSFUNCOLD ) {
    EEPROMWarnings += ::QObject::tr("This version of open9x does not support Custom Switch function %1").arg(getFuncName(func)) + "\n";
    func=0;
    v1=0;
    v2=0;
  }
}

Open9xCustomSwData_v208::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    c9x.val1 = open9x208ToSource(v1).toValue();
  }

  if (c9x.func >= CS_FN_EQUAL) {
    c9x.val2 = open9x208ToSource(v2).toValue();
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    c9x.val1 = open9xStockToSwitch(v1).toValue();
    c9x.val2 = open9xStockToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xCustomSwData_v209::t_Open9xCustomSwData_v209(CustomSwData &c9x)
{
  func = c9x.func;
  v1 = c9x.val1;
  v2 = c9x.val2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    v1 = open9xStock209FromSource(RawSource(c9x.val1));
  }

  if (c9x.func >= CS_FN_EQUAL && c9x.func <= CS_FN_ELESS) {
    v2 = open9xStock209FromSource(RawSource(c9x.val2));
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    v1 = open9xStockFromSwitch(RawSwitch(c9x.val1));
    v2 = open9xStockFromSwitch(RawSwitch(c9x.val2));
  }
}

Open9xCustomSwData_v209::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= CS_FN_VPOS && c9x.func <= CS_FN_ANEG) || c9x.func >= CS_FN_EQUAL) {
    c9x.val1 = open9xStock209ToSource(v1).toValue();
  }

  if (c9x.func >= CS_FN_EQUAL && c9x.func <= CS_FN_ELESS) {
    c9x.val2 = open9xStock209ToSource(v2).toValue();
  }

  if (c9x.func >= CS_FN_AND && c9x.func <= CS_FN_XOR) {
    c9x.val1 = open9xStockToSwitch(v1).toValue();
    c9x.val2 = open9xStockToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xFuncSwData_v201::t_Open9xFuncSwData_v201(FuncSwData &c9x)
{
  swtch = open9xStockFromSwitch(c9x.swtch);
  func = c9x.func - O9X_NUM_CHNOUT;
}

t_Open9xFuncSwData_v201::operator FuncSwData ()
{
  FuncSwData c9x;
  c9x.swtch = open9xStockToSwitch(swtch);
  c9x.func = (AssignFunc)(func+O9X_NUM_CHNOUT);
  return c9x;
}

enum Functions {
  FUNC_SAFETY_CH1,
  FUNC_SAFETY_CH16=FUNC_SAFETY_CH1+15,
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
  FUNC_TEST, // should remain the last before MAX as not added in companion9x
#endif
  FUNC_MAX
};

t_Open9xFuncSwData_v203::t_Open9xFuncSwData_v203(FuncSwData &c9x)
{
  swtch = open9xStockFromSwitch(c9x.swtch);
  if (c9x.func <= FuncSafetyCh16) {
    param = ((c9x.param>>1)<<1);
    param |=(c9x.enabled & 0x01);
    func = c9x.func;
  } else {
    param = c9x.param;;
    if (c9x.func <= FuncTrainerAIL)
      func = 16 + c9x.func - FuncTrainer;
    else if (c9x.func == FuncInstantTrim)
      func = 21;
    else if (c9x.func == FuncPlaySound)
      func = 22;
    else if (c9x.func == FuncPlayHaptic)
      func = 23;
    else if (c9x.func == FuncReset)
      func = 24;
    else if (c9x.func == FuncVario)
      func = 25;
    else if (c9x.func == FuncBacklight)
      func = 26;    
    else {
      swtch = 0;
      func = 0;
      param = 0;
    }
  }
}

t_Open9xFuncSwData_v203::operator FuncSwData ()
{
  FuncSwData c9x;
  c9x.swtch = open9xStockToSwitch(swtch);
  if (func < 16) {
    c9x.enabled=param & 0x01;
    c9x.param = (param>>1)<<1;
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

t_Open9xFuncSwData_v210::t_Open9xFuncSwData_v210(FuncSwData &c9x)
{
  swtch = open9xStockFromSwitch(c9x.swtch);
  param = c9x.param;
  if (c9x.func <= FuncInstantTrim) {
    delay = c9x.enabled;
    func = (uint8_t)c9x.func;
  }
  else if (c9x.func == FuncPlaySound)
    func = 22;
  else if (c9x.func == FuncPlayHaptic)
    func = 23;
  else if (c9x.func == FuncReset)
    func = 24;
  else if (c9x.func == FuncVario)
    func = 25;
  else if (c9x.func == FuncPlayPrompt)
    func = 26;
  else if (c9x.func == FuncPlayValue) {
    func = 27;
    if (c9x.param > 7) {
       param -= 2;
    }
  } else if (c9x.func == FuncBacklight)
    func = 28;
  else {
    swtch = 0;
    func = 0;
    param = 0;
  }
}

t_Open9xFuncSwData_v210::operator FuncSwData ()
{
  FuncSwData c9x;
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

t_Open9xSafetySwData::t_Open9xSafetySwData(SafetySwData &c9x)
{
  swtch = open9xStockFromSwitch(c9x.swtch);
  val = c9x.val;
}

t_Open9xSafetySwData::operator SafetySwData ()
{
  SafetySwData c9x;
  c9x.swtch = open9xStockToSwitch(swtch);
  c9x.val = val;
  return c9x;
}

t_Open9xSwashRingData_v208::t_Open9xSwashRingData_v208(SwashRingData &c9x)
{
  invertELE = c9x.invertELE;
  invertAIL = c9x.invertAIL;
  invertCOL = c9x.invertCOL;
  type = c9x.type;
  collectiveSource = open9x208FromSource(c9x.collectiveSource);
  value = c9x.value;
}

t_Open9xSwashRingData_v208::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.invertELE = invertELE;
  c9x.invertAIL = invertAIL;
  c9x.invertCOL = invertCOL;
  c9x.type = type;
  c9x.collectiveSource = open9x208ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xSwashRingData_v209::t_Open9xSwashRingData_v209(SwashRingData &c9x)
{
  invertELE = c9x.invertELE;
  invertAIL = c9x.invertAIL;
  invertCOL = c9x.invertCOL;
  type = c9x.type;
  collectiveSource = open9xStock209FromSource(c9x.collectiveSource);
  value = c9x.value;
}

t_Open9xSwashRingData_v209::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.invertELE = invertELE;
  c9x.invertAIL = invertAIL;
  c9x.invertCOL = invertCOL;
  c9x.type = type;
  c9x.collectiveSource = open9xStock209ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xPhaseData_v201::operator PhaseData ()
{
  PhaseData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = (((int16_t)trim[i]) << 2) + ((trim_ext >> (2*i)) & 0x03);
  c9x.swtch = open9xStockToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  return c9x;
}

t_Open9xPhaseData_v201::t_Open9xPhaseData_v201(PhaseData &c9x)
{
  trim_ext = 0;
  for (int i=0; i<NUM_STICKS; i++) {
    trim[i] = (int8_t)(c9x.trim[i] >> 2);
    trim_ext = (trim_ext & ~(0x03 << (2*i))) + (((c9x.trim[i] & 0x03) << (2*i)));
  }
  swtch = open9xStockFromSwitch(c9x.swtch);
  setEEPROMZString(name, c9x.name, sizeof(name));
  fadeIn = c9x.fadeIn;
  fadeOut = c9x.fadeOut;
}

t_Open9xTimerData_v201::operator TimerData ()
{
  TimerData c9x;
  c9x.mode = TMRMODE_OFF;
  c9x.val = val;
  c9x.persistent = persistent;
  c9x.dir = dir;
  return c9x;
}

t_Open9xTimerData_v202::operator TimerData ()
{
  TimerData c9x;

  if (mode <= -22)
    c9x.mode = TimerMode(TMRMODE_FIRST_NEG_MOMENT_SWITCH+(mode+22));
  else if (mode <= -1)
    c9x.mode = TimerMode(TMRMODE_FIRST_NEG_SWITCH+(mode+1));
  else if (mode < 5)
    c9x.mode = TimerMode(mode);
  else if (mode < 5+21)
    c9x.mode = TimerMode(TMRMODE_FIRST_SWITCH+(mode-5));
  else
    c9x.mode = TimerMode(TMRMODE_FIRST_MOMENT_SWITCH+(mode-5-21));

  c9x.val = val;
  c9x.persistent = false;
  c9x.dir = (val == 0);
  return c9x;
}

t_Open9xTimerData_v202::t_Open9xTimerData_v202(TimerData &c9x)
{
  val = c9x.val;

  if (c9x.mode >= TMRMODE_OFF && c9x.mode <= TMRMODE_THt)
    mode = 0+c9x.mode-TMRMODE_OFF;
  else if (c9x.mode >= TMRMODE_FIRST_MOMENT_SWITCH)
    mode = 26+c9x.mode-TMRMODE_FIRST_MOMENT_SWITCH;
  else if (c9x.mode >= TMRMODE_FIRST_SWITCH)
    mode = 5+c9x.mode-TMRMODE_FIRST_SWITCH;
  else if (c9x.mode <= TMRMODE_FIRST_NEG_MOMENT_SWITCH)
    mode = -22+c9x.mode-TMRMODE_FIRST_NEG_MOMENT_SWITCH;
  else if (c9x.mode <= TMRMODE_FIRST_NEG_SWITCH)
    mode = -1+c9x.mode-TMRMODE_FIRST_NEG_SWITCH;
  else
    mode = 0;
}

FrSkyRSSIAlarm t_Open9xFrSkyRSSIAlarm::get(int index)
{
  FrSkyRSSIAlarm c9x;
  c9x.level = (2+index+level) % 4;
  c9x.value = value + 50;
  return c9x;
}

t_Open9xFrSkyRSSIAlarm::t_Open9xFrSkyRSSIAlarm(int index, FrSkyRSSIAlarm &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyRSSIAlarm));
  level = (2+c9x.level-index) % 4;
  value = c9x.value - 50;
}

t_Open9xFrSkyChannelData_v201::t_Open9xFrSkyChannelData_v201(FrSkyChannelData &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyChannelData_v201));
  ratio = c9x.ratio;
  alarms_value[0] = c9x.alarms[0].value;
  alarms_value[1] = c9x.alarms[1].value;
  alarms_level = (c9x.alarms[1].level << 2) + c9x.alarms[0].level;
  alarms_greater = (c9x.alarms[1].greater << 1) + c9x.alarms[0].greater;
  type = c9x.type;
  offset = LIMIT<int8_t>(-8, c9x.offset, +7);
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

t_Open9xFrSkyChannelData_v203::t_Open9xFrSkyChannelData_v203(FrSkyChannelData &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyChannelData_v203));
  ratio = c9x.ratio;
  alarms_value[0] = c9x.alarms[0].value;
  alarms_value[1] = c9x.alarms[1].value;
  alarms_level = (c9x.alarms[1].level << 2) + c9x.alarms[0].level;
  alarms_greater = (c9x.alarms[1].greater << 1) + c9x.alarms[0].greater;
  type = c9x.type;
  offset = c9x.offset;
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

t_Open9xFrSkyChannelData_v204::t_Open9xFrSkyChannelData_v204(FrSkyChannelData &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyChannelData_v204));
  ratio = c9x.ratio;
  type = c9x.type;
  multiplier = c9x.multiplier;
  alarms_value[0] = c9x.alarms[0].value;
  alarms_value[1] = c9x.alarms[1].value;
  alarms_level = (c9x.alarms[1].level << 2) + c9x.alarms[0].level;
  alarms_greater = (c9x.alarms[1].greater << 1) + c9x.alarms[0].greater;
  // TODO multiplier = c9x.multiplier;
  offset = c9x.offset;
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

t_Open9xFrSkyChannelData_v208::t_Open9xFrSkyChannelData_v208(FrSkyChannelData &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyChannelData_v208));
  ratio = c9x.ratio;
  type = c9x.type;
  multiplier = c9x.multiplier;
  alarms_value[0] = c9x.alarms[0].value;
  alarms_value[1] = c9x.alarms[1].value;
  alarms_level = (c9x.alarms[1].level << 2) + c9x.alarms[0].level;
  alarms_greater = (c9x.alarms[1].greater << 1) + c9x.alarms[0].greater;
  // TODO multiplier = c9x.multiplier;
  offset = c9x.offset;
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

t_Open9xFrSkyData_v202::t_Open9xFrSkyData_v202(FrSkyData &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyData_v202));
  channels[0] = c9x.channels[0];
  channels[1] = c9x.channels[1];
  usrProto = c9x.usrProto;
}

t_Open9xFrSkyData_v203::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  return c9x;
}

t_Open9xFrSkyData_v203::t_Open9xFrSkyData_v203(FrSkyData &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyData_v203));
  channels[0] = c9x.channels[0];
  channels[1] = c9x.channels[1];
  usrProto = c9x.usrProto;
}

t_Open9xFrSkyBarData_v204::operator FrSkyBarData ()
{
  FrSkyBarData c9x;
  c9x.source = source;
  c9x.barMin = barMin;
  c9x.barMax = barMax;
  return c9x;
}

t_Open9xFrSkyBarData_v204::t_Open9xFrSkyBarData_v204(FrSkyBarData &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyBarData_v204));
  source = c9x.source;
  barMin = c9x.barMin;
  barMax = c9x.barMax;
}

t_Open9xFrSkyData_v204::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  c9x.imperial = imperial;
  c9x.blades = blades;
  c9x.screens[0].type = 1;
  for (int i=0; i<4; i++)
    c9x.screens[0].body.bars[i] = bars[i];
  return c9x;
}

t_Open9xFrSkyData_v204::t_Open9xFrSkyData_v204(FrSkyData &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyData_v204));
  channels[0] = c9x.channels[0];
  channels[1] = c9x.channels[1];
  usrProto = c9x.usrProto;
  imperial = c9x.imperial;
  blades = c9x.blades;
  for (int i=0; i<4; i++)
    bars[i] = c9x.screens[0].body.bars[i];
}

t_Open9xFrSkyData_v205::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  c9x.imperial = imperial;
  c9x.blades = blades;
  c9x.screens[0].type = 1;
  for (int i=0; i<4; i++)
    c9x.screens[0].body.bars[i] = bars[i];
  c9x.rssiAlarms[0] = rssiAlarms[0].get(0);
  c9x.rssiAlarms[1] = rssiAlarms[1].get(1);
  return c9x;
}

t_Open9xFrSkyData_v205::t_Open9xFrSkyData_v205(FrSkyData &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyData_v205));
  channels[0] = c9x.channels[0];
  channels[1] = c9x.channels[1];
  usrProto = c9x.usrProto;
  imperial = c9x.imperial;
  blades = c9x.blades;
  for (int i=0; i<4; i++)
    bars[i] = c9x.screens[0].body.bars[i];
  rssiAlarms[0] = Open9xFrSkyRSSIAlarm(0, c9x.rssiAlarms[0]);
  rssiAlarms[1] = Open9xFrSkyRSSIAlarm(1, c9x.rssiAlarms[1]);
}


t_Open9xFrSkyData_v208::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  c9x.voltsSource = voltsSource;
  c9x.blades = blades;
  c9x.currentSource=currentSource;
  c9x.screens[0].type = 1;
  for (int i=0; i<4; i++)
    c9x.screens[0].body.bars[i] = bars[i];
  c9x.rssiAlarms[0] = rssiAlarms[0].get(0);
  c9x.rssiAlarms[1] = rssiAlarms[1].get(1);
  return c9x;
}

t_Open9xFrSkyData_v208::t_Open9xFrSkyData_v208(FrSkyData &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyData_v208));
  channels[0] = c9x.channels[0];
  channels[1] = c9x.channels[1];
  usrProto = c9x.usrProto;
  voltsSource = c9x.voltsSource;
  blades = c9x.blades;
  currentSource=c9x.currentSource;
  for (int i=0; i<4; i++)
    bars[i] = c9x.screens[0].body.bars[i];
  rssiAlarms[0] = Open9xFrSkyRSSIAlarm(0, c9x.rssiAlarms[0]);
  rssiAlarms[1] = Open9xFrSkyRSSIAlarm(1, c9x.rssiAlarms[1]);
}

t_Open9xFrSkyData_v210::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  c9x.usrProto = usrProto;
  c9x.voltsSource = voltsSource;
  c9x.blades = blades;
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
  for (int line=0; line<4; line++) {
    for (int col=0; col<2; col++) {
      c9x.screens[lines_screen_index].body.lines[line].source[col] = (col==0 ? (lines[line] & 0x0f) : ((lines[line] & 0xf0) / 16));
      c9x.screens[lines_screen_index].body.lines[line].source[col] += (((linesXtra >> (4*line+2*col)) & 0x03) * 16);
    }
  }

  c9x.rssiAlarms[0] = rssiAlarms[0].get(0);
  c9x.rssiAlarms[1] = rssiAlarms[1].get(1);
  c9x.varioSource = varioSource;
  c9x.varioCenterMax = varioSpeedUpMin;
  c9x.varioCenterMin = varioSpeedDownMin;
  return c9x;
}

t_Open9xFrSkyData_v210::t_Open9xFrSkyData_v210(FrSkyData &c9x)
{
  memset(this, 0, sizeof(t_Open9xFrSkyData_v210));
  channels[0] = c9x.channels[0];
  channels[1] = c9x.channels[1];
  usrProto = c9x.usrProto;
  voltsSource = c9x.voltsSource;
  blades = c9x.blades;
  currentSource=c9x.currentSource;

  for (int scr=0; scr<2; scr++) {
    if (c9x.screens[scr].type == 1) {
      for (int i=0; i<4; i++)
        bars[i] = c9x.screens[scr].body.bars[i];
    }
    else {
      linesXtra=0;
      for (int j=0; j<4; j++) {
        lines[j] = 0;
        for (int k=0; k<2; k++) {
          int value = c9x.screens[scr].body.lines[j].source[k];
          lines[j] |= (k==0 ? (value & 0x0f) : ((value & 0x0f) << 4));
          linesXtra |= (value / 16) << (4*j+2*k);
        }
      }
    }
  }
  rssiAlarms[0] = Open9xFrSkyRSSIAlarm(0, c9x.rssiAlarms[0]);
  rssiAlarms[1] = Open9xFrSkyRSSIAlarm(1, c9x.rssiAlarms[1]);
  varioSource = c9x.varioSource;
  varioSpeedUpMin = c9x.varioCenterMax;
  varioSpeedDownMin = c9x.varioCenterMin;
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
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
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

  for (int i=0; i<12; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<12; i++)
    c9x.funcSw[i] = funcSw[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];
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
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter,
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
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

  for (int i=0; i<12; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<12; i++)
    c9x.funcSw[i] = funcSw[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId=modelId;
  return c9x;
}

t_Open9xModelData_v202::t_Open9xModelData_v202(ModelData &c9x)
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
      case PPM16:
        protocol = 3;
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
    spare1 = 0;
    pulsePol = c9x.moduleData[0].ppmPulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.moduleData[0].ppmDelay - 300) / 50;
    beepANACenter = (uint8_t)(c9x.beepANACenter & 0x7F);
    timer2 = c9x.timers[1];
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";

    /* TODO for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j]; */

    for (int i=0; i<12; i++)
      customSw[i] = c9x.customSw[i];
    for (int i=0; i<12; i++)
      funcSw[i] = c9x.funcSw[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      safetySw[i] = c9x.safetySw[i];
    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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
    ppmFrameLength = c9x.moduleData[0].ppmFrameLength;
    thrTraceSrc = c9x.thrTraceSrc;
    modelId=c9x.modelId;
  }
  else {
    memset(this, 0, sizeof(t_Open9xModelData_v202));
  }
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
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
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

  for (int i=0; i<O9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;
  return c9x;
}

t_Open9xModelData_v203::t_Open9xModelData_v203(ModelData &c9x)
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
      case PPM16:
        protocol = 3;
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
    spare1 = 0;
    pulsePol = c9x.moduleData[0].ppmPulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.moduleData[0].ppmDelay - 300) / 50;
    beepANACenter = (uint8_t)(c9x.beepANACenter & 0x7F);
    timer2 = c9x.timers[1];
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";

    /* for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j]; */

    for (int i=0; i<O9X_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xStockFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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
    ppmFrameLength = c9x.moduleData[0].ppmFrameLength;
    thrTraceSrc = c9x.thrTraceSrc;
    modelId = c9x.modelId;
  }
  else {
    memset(this, 0, sizeof(t_Open9xModelData_v203));
  }
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
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
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

  for (int i=0; i<O9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.frsky.rssiAlarms[0] = frskyRssiAlarms[0].get(0);
  c9x.frsky.rssiAlarms[1] = frskyRssiAlarms[1].get(1);
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;
  return c9x;
}

t_Open9xModelData_v204::t_Open9xModelData_v204(ModelData &c9x)
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
      case PPM16:
        protocol = 3;
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
    spare1 = 0;
    pulsePol = c9x.moduleData[0].ppmPulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.moduleData[0].ppmDelay - 300) / 50;
    beepANACenter = (uint8_t)(c9x.beepANACenter & 0x7F);
    timer2 = c9x.timers[1];
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";

    /* TODO    for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j]; */

    for (int i=0; i<O9X_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xStockFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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
    ppmFrameLength = c9x.moduleData[0].ppmFrameLength;
    thrTraceSrc = c9x.thrTraceSrc;
    modelId = c9x.modelId;
    frskyRssiAlarms[0] = Open9xFrSkyRSSIAlarm(0, c9x.frsky.rssiAlarms[0]);
    frskyRssiAlarms[1] = Open9xFrSkyRSSIAlarm(1, c9x.frsky.rssiAlarms[1]);
  }
  else {
    memset(this, 0, sizeof(t_Open9xModelData_v204));
  }
}

t_Open9xModelData_v205::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<O9X_MAX_TIMERS; i++)
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
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
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

  for (int i=0; i<O9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;
  c9x.frsky.screens[1].type = 0;
  for (int line=0; line<4; line++) {
    for (int col=0; col<2; col++) {
      c9x.frsky.screens[1].body.lines[line].source[col] = (col==0 ? (frskyLines[line] & 0x0f) : ((frskyLines[line] & 0xf0) / 16));
      c9x.frsky.screens[1].body.lines[line].source[col] += (((frskyLinesXtra >> (4*line+2*col)) & 0x03) * 16);
    }
  }

  return c9x;
}

#define MODEL_DATA_SIZE_205 756
t_Open9xModelData_v205::t_Open9xModelData_v205(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_205) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_205));
  }

  if (c9x.used) {
    setEEPROMZString(name, c9x.name, sizeof(name));
    for (int i=0; i<O9X_MAX_TIMERS; i++)
      timers[i] = c9x.timers[i];
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
      case PPM16:
        protocol = 3;
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
    spare1 = 0;
    pulsePol = c9x.moduleData[0].ppmPulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.moduleData[0].ppmDelay - 300) / 50;
    beepANACenter = (uint8_t)(c9x.beepANACenter & 0x7F);
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";
/* TODO    for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j]; */
    for (int i=0; i<O9X_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xStockFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  }
  else {
    memset(this, 0, sizeof(t_Open9xModelData_v205));
  }
}


t_Open9xModelData_v208::operator ModelData ()
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
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
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

  for (int i=0; i<O9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.frsky.varioSource = varioSource;
  c9x.frsky.varioCenterMax = varioSpeedUpMin;
  c9x.frsky.varioCenterMin = varioSpeedDownMin;

  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;
  c9x.frsky.screens[1].type = 0;
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

#define MODEL_DATA_SIZE_208 758
t_Open9xModelData_v208::t_Open9xModelData_v208(ModelData &c9x)
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
    beepANACenter = (uint8_t)(c9x.beepANACenter & 0x7F);
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";
/* TODO    for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j]; */
    for (int i=0; i<O9X_NUM_CSW; i++) {
      customSw[i] = c9x.customSw[i];
    }
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xStockFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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
    memset(this, 0, sizeof(t_Open9xModelData_v208));
  }
}


t_Open9xModelData_v209::operator ModelData ()
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
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
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

  for (int i=0; i<O9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_NUM_FSW; i++)
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
  c9x.frsky.screens[1].type = 0;
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

#define MODEL_DATA_SIZE_209 759
t_Open9xModelData_v209::t_Open9xModelData_v209(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_209) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_209));
  }

  memset(this, 0, sizeof(t_Open9xModelData_v209));

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
    beepANACenter = (uint8_t)(c9x.beepANACenter & 0x7F);
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";
/* TODO    for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j]; */
    for (int i=0; i<O9X_NUM_CSW; i++) {
      customSw[i] = c9x.customSw[i];
    }
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xStockFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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

t_Open9xModelData_v210::operator ModelData ()
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
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];

  for (int i=0; i<O9X_MAX_CURVES; i++) {
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

  for (int i=0; i<O9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.switchWarningStates = switchWarningStates;

  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;

  for (int i=0; i<O9X_NUM_CHNOUT; i++) {
    c9x.limitData[i].ppmCenter=servoCenter[i];
  }

  return c9x;
}

#define MODEL_DATA_SIZE_210 760
t_Open9xModelData_v210::t_Open9xModelData_v210(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_210) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_210));
  }

  memset(this, 0, sizeof(t_Open9xModelData_v210));

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
    beepANACenter = (uint8_t)(c9x.beepANACenter & 0x7F);
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";

    int8_t * cur = &points[0];
    int offset = 0;
    for (int i=0; i<O9X_MAX_CURVES; i++) {
      offset += (c9x.curves[i].custom ? c9x.curves[i].count * 2 - 2 : c9x.curves[i].count) - 5;
      if (offset > O9X_NUM_POINTS - 5 * O9X_MAX_CURVES) {
        EEPROMWarnings += ::QObject::tr("open9x only accepts %1 points in all curves").arg(O9X_NUM_POINTS) + "\n";
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

    for (int i=0; i<O9X_NUM_CSW; i++) {
      customSw[i] = c9x.customSw[i];
    }
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xStockFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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

t_Open9xModelData_v211::operator ModelData ()
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
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.trimInc = trimInc;
  c9x.moduleData[0].ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.moduleData[0].ppmPulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];

  for (int i=0; i<O9X_MAX_CURVES; i++) {
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

  for (int i=0; i<O9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.switchWarningStates = switchWarningStates;

  c9x.moduleData[0].ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;

  return c9x;
}

#define MODEL_DATA_SIZE_211 744
t_Open9xModelData_v211::t_Open9xModelData_v211(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_211) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_211));
  }

  memset(this, 0, sizeof(t_Open9xModelData_v211));

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
    spare1 = 0;
    ppmDelay = (c9x.moduleData[0].ppmDelay - 300) / 50;
    beepANACenter = (uint8_t)(c9x.beepANACenter & 0x7F);
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";

    int8_t * cur = &points[0];
    int offset = 0;
    for (int i=0; i<O9X_MAX_CURVES; i++) {
      offset += (c9x.curves[i].custom ? c9x.curves[i].count * 2 - 2 : c9x.curves[i].count) - 5;
      if (offset > O9X_NUM_POINTS - 5 * O9X_MAX_CURVES) {
        EEPROMWarnings += ::QObject::tr("open9x only accepts %1 points in all curves").arg(O9X_NUM_POINTS) + "\n";
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

    for (int i=0; i<O9X_NUM_CSW; i++) {
      customSw[i] = c9x.customSw[i];
    }
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xStockFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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
