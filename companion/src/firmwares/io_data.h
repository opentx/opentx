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

#ifndef IO_DATA_H
#define IO_DATA_H

#include "constants.h"
#include "curvereference.h"
#include "rawsource.h"
#include "rawswitch.h"

#include <QtCore>

class RadioDataConversionState;

enum InputMode {
  INPUT_MODE_NONE,
  INPUT_MODE_POS,
  INPUT_MODE_NEG,
  INPUT_MODE_BOTH
};

class ExpoData {
  Q_DECLARE_TR_FUNCTIONS(ExpoData)

  public:
    ExpoData() { clear(); }
    RawSource srcRaw;
    unsigned int scale;
    unsigned int mode;   // InputMode
    unsigned int chn;
    RawSwitch swtch;
    unsigned int flightModes;        // -5=!FP4, 0=normal, 5=FP4
    int  weight;
    int offset;
    CurveReference curve;
    int carryTrim;
    char name[10+1];
    void clear() { memset(this, 0, sizeof(ExpoData)); }
    void convert(RadioDataConversionState & cstate);
};

enum MltpxValue {
  MLTPX_ADD=0,
  MLTPX_MUL=1,
  MLTPX_REP=2
};

#define MIXDATA_NAME_LEN  10

class MixData {
  Q_DECLARE_TR_FUNCTIONS(MixData)

  public:
    MixData() { clear(); }
    void convert(RadioDataConversionState & cstate);

    unsigned int destCh;            //        1..CPN_MAX_CHNOUT
    RawSource srcRaw;
    int     weight;
    RawSwitch swtch;
    CurveReference     curve;             //0=symmetrisch
    unsigned int delayUp;
    unsigned int delayDown;
    unsigned int speedUp;           // Servogeschwindigkeit aus Tabelle (10ms Cycle)
    unsigned int speedDown;         // 0 nichts
    int  carryTrim;
    bool noExpo;
    MltpxValue mltpx;          // multiplex method 0=+ 1=* 2=replace
    unsigned int mixWarn;           // mixer warning
    unsigned int flightModes;             // -5=!FP4, 0=normal, 5=FP4
    int    sOffset;
    char   name[MIXDATA_NAME_LEN+1];

    void clear() { memset(this, 0, sizeof(MixData)); }
};

class LimitData {
  Q_DECLARE_TR_FUNCTIONS(LimitData)

  public:
    LimitData() { clear(); }
    int   min;
    int   max;
    bool  revert;
    int   offset;
    int   ppmCenter;
    bool  symetrical;
    char  name[6+1];
    CurveReference curve;
    QString minToString() const;
    QString maxToString() const;
    QString offsetToString() const;
    QString revertToString() const;
    QString nameToString(int index) const;
    void clear();
};

class CurvePoint {
  public:
    int8_t x;
    int8_t y;
};

class CurveData {
  Q_DECLARE_TR_FUNCTIONS(CurveData)

  public:
    enum CurveType {
      CURVE_TYPE_STANDARD,
      CURVE_TYPE_CUSTOM,
      CURVE_TYPE_LAST = CURVE_TYPE_CUSTOM
    };

    CurveData();
    void clear(int count);
    bool isEmpty() const;
    QString nameToString(const int idx) const;

    CurveType type;
    bool smooth;
    int  count;
    CurvePoint points[CPN_MAX_POINTS];
    char name[6+1];
};

class FlightModeData {
  Q_DECLARE_TR_FUNCTIONS(FlightModeData)

  public:
    FlightModeData() { clear(0); }
    int trimMode[CPN_MAX_TRIMS];
    int trimRef[CPN_MAX_TRIMS];
    int trim[CPN_MAX_TRIMS];
    RawSwitch swtch;
    char name[10+1];
    unsigned int fadeIn;
    unsigned int fadeOut;
    int rotaryEncoders[CPN_MAX_ENCODERS];
    int gvars[CPN_MAX_GVARS];
    void clear(const int phase);
    QString nameToString(int index) const;
    void convert(RadioDataConversionState & cstate);
};

class SwashRingData {
  Q_DECLARE_TR_FUNCTIONS(SwashRingData)

  public:
    SwashRingData() { clear(); }
    int elevatorWeight;
    int aileronWeight;
    int collectiveWeight;
    unsigned int  type;
    RawSource collectiveSource;
    RawSource aileronSource;
    RawSource elevatorSource;
    unsigned int  value;
    void clear() { memset(this, 0, sizeof(SwashRingData)); }
};

#endif // IO_DATA_H
