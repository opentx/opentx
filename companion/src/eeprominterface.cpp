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

#include <stdio.h>
#include <list>
#include <float.h>
#include <QtWidgets>
#include <stdlib.h>
#include "eeprominterface.h"
#include "firmwares/er9x/er9xinterface.h"
#include "firmwares/ersky9x/ersky9xinterface.h"
#include "firmwares/opentx/opentxinterface.h"
#include "firmwares/opentx/opentxeeprom.h"
#include "appdata.h"
#include "helpers.h"
#include "wizarddata.h"
#include "firmwareinterface.h"

std::list<QString> EEPROMWarnings;

const uint8_t chout_ar[] = { // First number is 0..23 -> template setup,  Second is relevant channel out
  1,2,3,4 , 1,2,4,3 , 1,3,2,4 , 1,3,4,2 , 1,4,2,3 , 1,4,3,2,
  2,1,3,4 , 2,1,4,3 , 2,3,1,4 , 2,3,4,1 , 2,4,1,3 , 2,4,3,1,
  3,1,2,4 , 3,1,4,2 , 3,2,1,4 , 3,2,4,1 , 3,4,1,2 , 3,4,2,1,
  4,1,2,3 , 4,1,3,2 , 4,2,1,3 , 4,2,3,1 , 4,3,1,2 , 4,3,2,1
};

static const char specialCharsTab[] = "_-.,";

void setEEPROMString(char *dst, const char *src, int size)
{
  memcpy(dst, src, size);
  for (int i=size-1; i>=0; i--) {
    if (dst[i] == '\0')
      dst[i] = ' ';
    else
      break;
  }
}

void getEEPROMString(char *dst, const char *src, int size)
{
  memcpy(dst, src, size);
  dst[size] = '\0';
  for (int i=size-1; i>=0; i--) {
    if (dst[i] == ' ')
      dst[i] = '\0';
    else
      break;
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

float ValToTim(int value)
{
   return ((value < -109 ? 129+value : (value < 7 ? (113+value)*5 : (53+value)*10))/10.0);
}

int TimToVal(float value)
{
  int temp;
  if (value>60) {
    temp=136+round((value-60));
  }
  else if (value>2) {
    temp=20+round((value-2.0)*2.0);
  }
  else {
    temp=round(value*10.0);
  }
  return (temp-129);
}

QString getSignedStr(int value)
{
  return value > 0 ? QString("+%1").arg(value) : QString("%1").arg(value);
}

QString getGVarString(int16_t val, bool sign)
{
  if (val >= -10000 && val <= 10000) {
    if (sign)
      return QString("%1%").arg(getSignedStr(val));
    else
      return QString("%1%").arg(val);
  }
  else {
    if (val<0)
      return QObject::tr("-GV%1").arg(-val-10000);
    else
      return QObject::tr("GV%1").arg(val-10000);
  }
}

void SensorData::updateUnit()
{
  if (type == TELEM_TYPE_CALCULATED) {
    if (formula == TELEM_FORMULA_CONSUMPTION)
      unit = UNIT_MAH;
  }
}

QString SensorData::unitString() const
{
  switch (unit) {
    case UNIT_VOLTS:
      return QObject::tr("V");
    case UNIT_AMPS:
      return QObject::tr("A");
    case UNIT_MILLIAMPS:
      return QObject::tr("mA");
    case UNIT_KTS:
      return QObject::tr("kts");
    case UNIT_METERS_PER_SECOND:
      return QObject::tr("m/s");
    case UNIT_KMH:
      return QObject::tr("km/h");
    case UNIT_MPH:
      return QObject::tr("mph");
    case UNIT_METERS:
      return QObject::tr("m");
    case UNIT_FEET:
      return QObject::tr("f");
    case UNIT_CELSIUS:
      return QObject::trUtf8("°C");
    case UNIT_FAHRENHEIT:
      return QObject::trUtf8("°F");
    case UNIT_PERCENT:
      return QObject::tr("%");
    case UNIT_MAH:
      return QObject::tr("mAh");
    case UNIT_WATTS:
      return QObject::tr("W");
    case UNIT_MILLIWATTS:
        return QObject::tr("mW");
    case UNIT_DB:
      return QObject::tr("dB");
    case UNIT_RPMS:
      return QObject::tr("rpms");
    case UNIT_G:
      return QObject::tr("g");
    case UNIT_DEGREE:
      return QObject::trUtf8("°");
    case UNIT_RADIANS:
      return QObject::trUtf8("Rad");
    case UNIT_HOURS:
      return QObject::tr("hours");
    case UNIT_MINUTES:
      return QObject::tr("minutes");
    case UNIT_SECONDS:
      return QObject::tr("seconds");
    case UNIT_CELLS:
      return QObject::tr("V");
    default:
      return "";
  }
}

bool RawSource::isTimeBased() const
{
  if (IS_ARM(GetCurrentFirmware()->getBoard()))
    return (type == SOURCE_TYPE_SPECIAL && index > 0);
  else
    return (type==SOURCE_TYPE_TELEMETRY && (index==TELEMETRY_SOURCE_TX_TIME || index==TELEMETRY_SOURCE_TIMER1 || index==TELEMETRY_SOURCE_TIMER2 || index==TELEMETRY_SOURCE_TIMER3));
}

float RawSourceRange::getValue(int value)
{
  if (IS_ARM(GetCurrentFirmware()->getBoard()))
    return float(value) * step;
  else
    return min + float(value) * step;
}

RawSourceRange RawSource::getRange(const ModelData * model, const GeneralSettings & settings, unsigned int flags) const
{
  RawSourceRange result;

  Firmware * firmware = GetCurrentFirmware();
  int board = firmware->getBoard();
  bool singleprec = (flags & RANGE_SINGLE_PRECISION);

  if (!singleprec && !IS_ARM(board)) {
    singleprec = true;
  }

  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      if (IS_ARM(board)) {
        div_t qr = div(index, 3);
        const SensorData & sensor = model->sensorData[qr.quot];
        if (sensor.prec == 2)
          result.step = 0.01;
        else if (sensor.prec == 1)
          result.step = 0.1;
        else
          result.step = 1;
        result.min = -30000 * result.step;
        result.max = +30000 * result.step;
        result.decimals = sensor.prec;
        result.unit = sensor.unitString();
      }
      else {
        if (singleprec) {
          result.offset = -DBL_MAX;
        }

        switch (index) {
          case TELEMETRY_SOURCE_TX_BATT:
            result.step = 0.1;
            result.decimals = 1;
            result.max = 25.5;
            result.unit = QObject::tr("V");
            break;
          case TELEMETRY_SOURCE_TX_TIME:
            result.step = 1;
            result.max = 24*60 - 1;
            break;
          case TELEMETRY_SOURCE_TIMER1:
          case TELEMETRY_SOURCE_TIMER2:
          case TELEMETRY_SOURCE_TIMER3:
            result.step = singleprec ? 5 : 1;
            result.max = singleprec ? 255*5 : 60*60;
            result.unit = QObject::tr("s");
            break;
          case TELEMETRY_SOURCE_RSSI_TX:
          case TELEMETRY_SOURCE_RSSI_RX:
            result.max = 100;
            if (singleprec) result.offset = 128;
            break;
          case TELEMETRY_SOURCE_A1_MIN:
          case TELEMETRY_SOURCE_A2_MIN:
          case TELEMETRY_SOURCE_A3_MIN:
          case TELEMETRY_SOURCE_A4_MIN:
            if (model) result = model->frsky.channels[index-TELEMETRY_SOURCE_A1_MIN].getRange();
            break;
          case TELEMETRY_SOURCE_A1:
          case TELEMETRY_SOURCE_A2:
          case TELEMETRY_SOURCE_A3:
          case TELEMETRY_SOURCE_A4:
            if (model) result = model->frsky.channels[index-TELEMETRY_SOURCE_A1].getRange();
            break;
          case TELEMETRY_SOURCE_ALT:
          case TELEMETRY_SOURCE_ALT_MIN:
          case TELEMETRY_SOURCE_ALT_MAX:
          case TELEMETRY_SOURCE_GPS_ALT:
            result.step = singleprec ? 8 : 1;
            result.min = -500;
            result.max = singleprec ? 1540 : 3000;
            if (firmware->getCapability(Imperial) || settings.imperial) {
              result.step = (result.step * 105) / 32;
              result.min = (result.min * 105) / 32;
              result.max = (result.max * 105) / 32;
              result.unit = QObject::tr("ft");
            }
            else {
              result.unit = QObject::tr("m");
            }
            break;
          case TELEMETRY_SOURCE_T1:
          case TELEMETRY_SOURCE_T1_MAX:
          case TELEMETRY_SOURCE_T2:
          case TELEMETRY_SOURCE_T2_MAX:
            result.min = -30;
            result.max = 225;
            result.unit = QObject::trUtf8("°C");
            break;
          case TELEMETRY_SOURCE_HDG:
            result.step = singleprec ? 2 : 1;
            result.max = 360;
            if (singleprec) result.offset = 256;
            result.unit = QObject::trUtf8("°");
            break;
          case TELEMETRY_SOURCE_RPM:
          case TELEMETRY_SOURCE_RPM_MAX:
            result.step = singleprec ? 50 : 1;
            result.max = singleprec ? 12750 : 30000;
            break;
          case TELEMETRY_SOURCE_FUEL:
            result.max = 100;
            result.unit = QObject::tr("%");
            break;
          case TELEMETRY_SOURCE_ASPEED:
          case TELEMETRY_SOURCE_ASPEED_MAX:
            result.decimals = 1;
            result.step = singleprec ? 2.0 : 0.1;
            result.max = singleprec ? (2*255) : 2000;
            if (firmware->getCapability(Imperial) || settings.imperial) {
              result.step *= 1.150779;
              result.max *= 1.150779;
              result.unit = QObject::tr("mph");
            }
            else {
              result.step *= 1.852;
              result.max *= 1.852;
              result.unit = QObject::tr("km/h");
            }
            break;
          case TELEMETRY_SOURCE_SPEED:
          case TELEMETRY_SOURCE_SPEED_MAX:
            result.step = singleprec ? 2 : 1;
            result.max = singleprec ? (2*255) : 2000;
            if (firmware->getCapability(Imperial) || settings.imperial) {
              result.step *= 1.150779;
              result.max *= 1.150779;
              result.unit = QObject::tr("mph");
            }
            else {
              result.step *= 1.852;
              result.max *= 1.852;
              result.unit = QObject::tr("km/h");
            }
            break;
          case TELEMETRY_SOURCE_VERTICAL_SPEED:
            result.step = 0.1;
            result.min = singleprec ? -12.5 : -300.0;
            result.max = singleprec ? 13.0 : 300.0;
            result.decimals = 1;
            result.unit = QObject::tr("m/s");
            break;
          case TELEMETRY_SOURCE_DTE:
            result.max = 30000;
            break;
          case TELEMETRY_SOURCE_DIST:
          case TELEMETRY_SOURCE_DIST_MAX:
            result.step = singleprec ? 8 : 1;
            result.max = singleprec ? 2040 : 10000;
            result.unit = QObject::tr("m");
            break;
          case TELEMETRY_SOURCE_CELL:
          case TELEMETRY_SOURCE_CELL_MIN:
            result.step = singleprec ? 0.02 : 0.01;
            result.max = 5.1;
            result.decimals = 2;
            result.unit = QObject::tr("V");
            break;
          case TELEMETRY_SOURCE_CELLS_SUM:
          case TELEMETRY_SOURCE_CELLS_MIN:
          case TELEMETRY_SOURCE_VFAS:
          case TELEMETRY_SOURCE_VFAS_MIN:
            result.step = 0.1;
            result.max = singleprec ? 25.5 : 100.0;
            result.decimals = 1;
            result.unit = QObject::tr("V");
            break;
          case TELEMETRY_SOURCE_CURRENT:
          case TELEMETRY_SOURCE_CURRENT_MAX:
            result.step = singleprec ? 0.5 : 0.1;
            result.max = singleprec ? 127.5 : 200.0;
            result.decimals = 1;
            result.unit = QObject::tr("A");
            break;
          case TELEMETRY_SOURCE_CONSUMPTION:
            result.step = singleprec ? 100 : 1;
            result.max = singleprec ? 25500 : 30000;
            result.unit = QObject::tr("mAh");
            break;
          case TELEMETRY_SOURCE_POWER:
          case TELEMETRY_SOURCE_POWER_MAX:
            result.step = singleprec ? 5 : 1;
            result.max = singleprec ? 1275 : 2000;
            result.unit = QObject::tr("W");
            break;
          case TELEMETRY_SOURCE_ACCX:
          case TELEMETRY_SOURCE_ACCY:
          case TELEMETRY_SOURCE_ACCZ:
            result.step = 0.01;
            result.decimals = 2;
            result.max = singleprec ? 2.55 : 10.00;
            result.min = singleprec ? 0 : -10.00;
            result.unit = QObject::tr("g");
            break;
          default:
            result.max = 125;
            break;
        }

        if (singleprec && result.offset==-DBL_MAX) {
          result.offset = result.max - (127*result.step);
        }

        if (flags & (RANGE_DELTA_FUNCTION|RANGE_DELTA_ABS_FUNCTION)) {
          if (singleprec) {
            result.offset = 0;
            result.min = result.step * -127;
            result.max = result.step * 127;
          }
          else {
            result.min = -result.max;
          }
        }
      }
      break;

    case SOURCE_TYPE_GVAR:
      result.max = 1024;
      result.min = -result.max;
      break;

    case SOURCE_TYPE_SPECIAL:
      if (index == 0)  {  //Batt
        result.step = 0.1;
        result.decimals = 1;
        result.max = 25.5;
        result.unit = QObject::tr("V");
      }
      else if (index == 1) {   //Time
        result.step = 1;
        result.max = 24*60 - 1;
        result.unit = QObject::tr("h:m");
      }
      else {      // Timers 1 - 3
        result.step = singleprec ? 5 : 1;
        result.max = singleprec ? 255*5 : 60*60;
        result.unit = singleprec ? QObject::tr("m:s") : QObject::tr("h:m:s");
      }
      break;

    default:
      if (model) {
        result.max = model->getChannelsMax(true);
        result.min = -result.max;
      }
      break;
  }

  if (flags & RANGE_DELTA_ABS_FUNCTION) {
    result.min = 0;
  }

  return result;
}

QString RotaryEncoderString(int index)
{
  static const QString rotary[]  = { QObject::tr("REa"), QObject::tr("REb") };
  return CHECK_IN_ARRAY(rotary, index);
}

QString RawSource::toString(const ModelData * model) const
{
  static const QString trims[] = {
    QObject::tr("TrmR"), QObject::tr("TrmE"), QObject::tr("TrmT"), QObject::tr("TrmA")
  };

  static const QString special[] = {
    QObject::tr("Batt"), QObject::tr("Time"), QObject::tr("Timer1"), QObject::tr("Timer2"), QObject::tr("Timer3"),
  };

  static const QString telemetry[] = {
    QObject::tr("Batt"), QObject::tr("Time"), QObject::tr("Timer1"), QObject::tr("Timer2"), QObject::tr("Timer3"),
    QObject::tr("SWR"), QObject::tr("RSSI Tx"), QObject::tr("RSSI Rx"),
    QObject::tr("A1"), QObject::tr("A2"), QObject::tr("A3"), QObject::tr("A4"),
    QObject::tr("Alt"), QObject::tr("Rpm"), QObject::tr("Fuel"), QObject::tr("T1"), QObject::tr("T2"),
    QObject::tr("Speed"), QObject::tr("Dist"), QObject::tr("GPS Alt"),
    QObject::tr("Cell"), QObject::tr("Cells"), QObject::tr("Vfas"), QObject::tr("Curr"), QObject::tr("Cnsp"), QObject::tr("Powr"),
    QObject::tr("AccX"), QObject::tr("AccY"), QObject::tr("AccZ"),
    QObject::tr("Hdg "), QObject::tr("VSpd"), QObject::tr("AirSpeed"), QObject::tr("dTE"),
    QObject::tr("A1-"),  QObject::tr("A2-"), QObject::tr("A3-"),  QObject::tr("A4-"),
    QObject::tr("Alt-"), QObject::tr("Alt+"), QObject::tr("Rpm+"), QObject::tr("T1+"), QObject::tr("T2+"), QObject::tr("Speed+"), QObject::tr("Dist+"), QObject::tr("AirSpeed+"),
    QObject::tr("Cell-"), QObject::tr("Cells-"), QObject::tr("Vfas-"), QObject::tr("Curr+"), QObject::tr("Powr+"),
    QObject::tr("ACC"), QObject::tr("GPS Time"),
  };

  if (index<0) {
    return QObject::tr("----");
  }

  switch (type) {
    case SOURCE_TYPE_VIRTUAL_INPUT:
    {
      QString result = QObject::tr("[I%1]").arg(index+1);
      if (model && strlen(model->inputNames[index]) > 0) {
        result += QString(model->inputNames[index]);
      }
      return result;
    }
    case SOURCE_TYPE_LUA_OUTPUT:
      return QObject::tr("LUA%1%2").arg(index/16+1).arg(QChar('a'+index%16));
    case SOURCE_TYPE_STICK:
      return GetCurrentFirmware()->getAnalogInputName(index);
    case SOURCE_TYPE_TRIM:
      return CHECK_IN_ARRAY(trims, index);
    case SOURCE_TYPE_ROTARY_ENCODER:
      return RotaryEncoderString(index);
    case SOURCE_TYPE_MAX:
      return QObject::tr("MAX");
    case SOURCE_TYPE_SWITCH:
      return GetCurrentFirmware()->getSwitch(index).name;
    case SOURCE_TYPE_CUSTOM_SWITCH:
      return QObject::tr("L%1").arg(index+1);
    case SOURCE_TYPE_CYC:
      return QObject::tr("CYC%1").arg(index+1);
    case SOURCE_TYPE_PPM:
      return QObject::tr("TR%1").arg(index+1);
    case SOURCE_TYPE_CH:
      return QObject::tr("CH%1").arg(index+1);
    case SOURCE_TYPE_SPECIAL:
      return CHECK_IN_ARRAY(special, index);
    case SOURCE_TYPE_TELEMETRY:
      if (IS_ARM(GetEepromInterface()->getBoard())) {
        div_t qr = div(index, 3);
        QString result = QString(model ? model->sensorData[qr.quot].label : QString("[T%1]").arg(qr.quot+1));
        if (qr.rem) result += qr.rem == 1 ? "-" : "+";
        return result;
      }
      else {
        return CHECK_IN_ARRAY(telemetry, index);
      }
    case SOURCE_TYPE_GVAR:
      return QObject::tr("GV%1").arg(index+1);
    default:
      return QObject::tr("----");
  }
}

bool RawSource::isPot() const
{
  return (type == SOURCE_TYPE_STICK &&
          index >= CPN_MAX_STICKS &&
          index < CPN_MAX_STICKS+GetCurrentFirmware()->getCapability(Pots));
}

bool RawSource::isSlider() const
{
  return (type == SOURCE_TYPE_STICK &&
          index >= CPN_MAX_STICKS+GetCurrentFirmware()->getCapability(Pots) &&
          index < CPN_MAX_STICKS+GetCurrentFirmware()->getCapability(Pots)+GetCurrentFirmware()->getCapability(Sliders));
}

QString RawSwitch::toString() const
{
  static const QString switches9X[] = {
    QString("THR"), QString("RUD"), QString("ELE"),
    QString("ID0"), QString("ID1"), QString("ID2"),
    QString("AIL"), QString("GEA"), QString("TRN")
  };

  static const QString flightModes[] = {
    QObject::tr("FM0"), QObject::tr("FM1"), QObject::tr("FM2"), QObject::tr("FM3"), QObject::tr("FM4"), QObject::tr("FM5"), QObject::tr("FM6"), QObject::tr("FM7"), QObject::tr("FM8")
  };

  static const QString multiposPots[] = {
    QObject::tr("S11"), QObject::tr("S12"), QObject::tr("S13"), QObject::tr("S14"), QObject::tr("S15"), QObject::tr("S16"),
    QObject::tr("S21"), QObject::tr("S22"), QObject::tr("S23"), QObject::tr("S24"), QObject::tr("S25"), QObject::tr("S26"),
    QObject::tr("S31"), QObject::tr("S32"), QObject::tr("S33"), QObject::tr("S34"), QObject::tr("S35"), QObject::tr("S36")
  };

  static const QString trimsSwitches[] = {
    QObject::tr("RudTrim Left"), QObject::tr("RudTrim Right"),
    QObject::tr("EleTrim Down"), QObject::tr("EleTrim Up"),
    QObject::tr("ThrTrim Down"), QObject::tr("ThrTrim Up"),
    QObject::tr("AilTrim Left"), QObject::tr("AilTrim Right")
  };

  static const QString rotaryEncoders[] = {
    QObject::tr("REa"), QObject::tr("REb")
  };

  static const QString timerModes[] = {
    QObject::tr("OFF"), QObject::tr("ON"),
    QObject::tr("THs"), QObject::tr("TH%"), QObject::tr("THt")
  };

  if (index < 0) {
    return QString("!") + RawSwitch(type, -index).toString();
  }
  else {
    BoardEnum board = GetEepromInterface()->getBoard();
    switch(type) {
      case SWITCH_TYPE_SWITCH:
        if (IS_HORUS(board) || IS_TARANIS(board)) {
          div_t qr = div(index-1, 3);
          Firmware::Switch sw = GetCurrentFirmware()->getSwitch(qr.quot);
          const char * positions[] = { ARROW_UP, "-", ARROW_DOWN };
          return QString(sw.name) + QString(positions[qr.rem]);
        }
        else {
          return CHECK_IN_ARRAY(switches9X, index - 1);
        }
      case SWITCH_TYPE_VIRTUAL:
        return QObject::tr("L%1").arg(index);
      case SWITCH_TYPE_MULTIPOS_POT:
        return CHECK_IN_ARRAY(multiposPots, index-1);
      case SWITCH_TYPE_TRIM:
        return CHECK_IN_ARRAY(trimsSwitches, index-1);
      case SWITCH_TYPE_ROTARY_ENCODER:
        return CHECK_IN_ARRAY(rotaryEncoders, index-1);
      case SWITCH_TYPE_ON:
        return QObject::tr("ON");
      case SWITCH_TYPE_OFF:
        return QObject::tr("OFF");
      case SWITCH_TYPE_ONE:
        return QObject::tr("One");
      case SWITCH_TYPE_FLIGHT_MODE:
        return CHECK_IN_ARRAY(flightModes, index-1);
      case SWITCH_TYPE_NONE:
        return QObject::tr("----");
      case SWITCH_TYPE_TIMER_MODE:
        return CHECK_IN_ARRAY(timerModes, index);
      default:
        return QObject::tr("???");
    }
  }
}

QString CurveReference::toString() const
{
  if (value == 0) {
    return "----";
  }
  else {
    switch(type) {
      case CURVE_REF_DIFF:
        return QObject::tr("Diff(%1)").arg(getGVarString(value));
      case CURVE_REF_EXPO:
        return QObject::tr("Expo(%1)").arg(getGVarString(value));
      case CURVE_REF_FUNC:
        return QObject::tr("Function(%1)").arg(QString("x>0" "x<0" "|x|" "f>0" "f<0" "|f|").mid(3*(value-1), 3));
      default:
        return QString(value > 0 ? QObject::tr("Curve(%1)") : QObject::tr("!Curve(%1)")).arg(abs(value));
    }
  }
}

CSFunctionFamily LogicalSwitchData::getFunctionFamily() const
{
  if (func == LS_FN_EDGE)
    return LS_FAMILY_EDGE;
  else if (func == LS_FN_TIMER)
    return LS_FAMILY_TIMER;
  else if (func == LS_FN_STICKY)
    return LS_FAMILY_STICKY;
  else if (func < LS_FN_AND || func > LS_FN_ELESS)
    return LS_FAMILY_VOFS;
  else if (func < LS_FN_EQUAL)
    return LS_FAMILY_VBOOL;
  else
    return LS_FAMILY_VCOMP;
}

unsigned int LogicalSwitchData::getRangeFlags() const
{
  if (func == LS_FN_DPOS)
    return RANGE_DELTA_FUNCTION;
  else if (func == LS_FN_DAPOS)
    return RANGE_DELTA_ABS_FUNCTION;
  else
    return 0;
}

QString LogicalSwitchData::funcToString() const
{
  switch (func) {
    case LS_FN_OFF:
      return QObject::tr("---");
    case LS_FN_VPOS:
      return QObject::tr("a>x");
    case LS_FN_VNEG:
      return QObject::tr("a<x");
    case LS_FN_APOS:
      return QObject::tr("|a|>x");
    case LS_FN_ANEG:
      return QObject::tr("|a|<x");
    case LS_FN_AND:
      return QObject::tr("AND");
    case LS_FN_OR:
      return QObject::tr("OR");
    case LS_FN_XOR:
      return QObject::tr("XOR");
    case LS_FN_EQUAL:
      return QObject::tr("a=b");
    case LS_FN_NEQUAL:
      return QObject::tr("a!=b");
    case LS_FN_GREATER:
      return QObject::tr("a>b");
    case LS_FN_LESS:
      return QObject::tr("a<b");
    case LS_FN_EGREATER:
      return QObject::tr("a>=b");
    case LS_FN_ELESS:
      return QObject::tr("a<=b");
    case LS_FN_DPOS:
      return QObject::tr("d>=x");
    case LS_FN_DAPOS:
      return QObject::tr("|d|>=x");
    case LS_FN_VEQUAL:
      return QObject::tr("a=x");
    case LS_FN_VALMOSTEQUAL:
      return QObject::tr("a~x");
    case LS_FN_TIMER:
      return QObject::tr("Timer");
    case LS_FN_STICKY:
      return QObject::tr("Sticky");
    case LS_FN_EDGE:
      return QObject::tr("Edge");
    default:
      return QObject::tr("Unknown");
  }
}

void CustomFunctionData::clear()
{
  memset(this, 0, sizeof(CustomFunctionData));
  if (!GetCurrentFirmware()->getCapability(SafetyChannelCustomFunction)) {
    func = FuncTrainer;
  }
}

QString CustomFunctionData::funcToString() const
{
  if (func >= FuncOverrideCH1 && func <= FuncOverrideCH32)
    return QObject::tr("Override %1").arg(RawSource(SOURCE_TYPE_CH, func).toString());
  else if (func == FuncTrainer)
    return QObject::tr("Trainer");
  else if (func == FuncTrainerRUD)
    return QObject::tr("Trainer RUD");
  else if (func == FuncTrainerELE)
    return QObject::tr("Trainer ELE");
  else if (func == FuncTrainerTHR)
    return QObject::tr("Trainer THR");
  else if (func == FuncTrainerAIL)
    return QObject::tr("Trainer AIL");
  else if (func == FuncInstantTrim)
    return QObject::tr("Instant Trim");
  else if (func == FuncPlaySound)
    return QObject::tr("Play Sound");
  else if (func == FuncPlayHaptic)
    return QObject::tr("Haptic");
  else if (func == FuncReset)
    return QObject::tr("Reset");
  else if (func >= FuncSetTimer1 && func <= FuncSetTimer3)
    return QObject::tr("Set Timer %1").arg(func-FuncSetTimer1+1);
  else if (func == FuncVario)
    return QObject::tr("Vario");
  else if (func == FuncPlayPrompt)
    return QObject::tr("Play Track");
  else if (func == FuncPlayBoth)
    return QObject::tr("Play Both");
  else if (func == FuncPlayValue)
    return QObject::tr("Play Value");
  else if (func == FuncPlayScript)
    return QObject::tr("Play Script");
  else if (func == FuncLogs)
    return QObject::tr("SD Logs");
  else if (func == FuncVolume)
    return QObject::tr("Volume");
  else if (func == FuncBacklight)
    return QObject::tr("Backlight");
  else if (func == FuncScreenshot)
    return QObject::tr("Screenshot");
  else if (func == FuncBackgroundMusic)
    return QObject::tr("Background Music");
  else if (func == FuncBackgroundMusicPause)
    return QObject::tr("Background Music Pause");
  else if (func >= FuncAdjustGV1 && func <= FuncAdjustGVLast)
    return QObject::tr("Adjust GV%1").arg(func-FuncAdjustGV1+1);
  else if (func == FuncSetFailsafeInternalModule)
    return QObject::tr("SetFailsafe Int. Module");
  else if (func == FuncSetFailsafeExternalModule)
    return QObject::tr("SetFailsafe Ext. Module");
  else if (func == FuncRangeCheckInternalModule)
    return QObject::tr("RangeCheck Int. Module");
  else if (func == FuncRangeCheckExternalModule)
    return QObject::tr("RangeCheck Ext. Module");
  else if (func == FuncBindInternalModule)
    return QObject::tr("Bind Int. Module");
  else if (func == FuncBindExternalModule)
    return QObject::tr("Bind Ext. Module");
  else {
    return QString("???"); // Highlight unknown functions with output of question marks.(BTW should not happen that we do not know what a function is)
  }
}

void CustomFunctionData::populateResetParams(const ModelData * model, QComboBox * b, unsigned int value = 0)
{
  int val = 0;
  Firmware * firmware = GetCurrentFirmware();
  BoardEnum board = GetEepromInterface()->getBoard();

  b->addItem(QObject::tr("Timer1"), val++);
  b->addItem(QObject::tr("Timer2"), val++);
  if (IS_ARM(board)) {
    b->addItem( QObject::tr("Timer3"), val++);
  }
  b->addItem(QObject::tr("Flight"), val++);
  b->addItem(QObject::tr("Telemetry"), val++);
  int reCount = firmware->getCapability(RotaryEncoders);
  if (reCount == 1) {
    b->addItem(QObject::tr("Rotary Encoder"), val++);
  }
  else if (reCount == 2) {
    b->addItem(QObject::tr("REa"), val++);
    b->addItem(QObject::tr("REb"), val++);
  }
  if ((int)value < b->count()) {
    b->setCurrentIndex(value);
  }
  if (model && IS_ARM(board)) {
    for (int i=0; i<CPN_MAX_SENSORS; ++i) {
      if (model->sensorData[i].isAvailable()) {
        RawSource item = RawSource(SOURCE_TYPE_TELEMETRY, 3*i);
        b->addItem(item.toString(model), val+i);
        if ((int)value == val+i) {
          b->setCurrentIndex(b->count()-1);
        }
      }
    }
  }
}

void CustomFunctionData::populatePlaySoundParams(QStringList & qs)
{
  qs <<"Beep 1" << "Beep 2" << "Beep 3" << "Warn1" << "Warn2" << "Cheep" << "Ratata" << "Tick" << "Siren" << "Ring" ;
  qs << "SciFi" << "Robot" << "Chirp" << "Tada" << "Crickt"  << "AlmClk"  ;
}

void CustomFunctionData::populateHapticParams(QStringList & qs)
{
  qs << "0" << "1" << "2" << "3";
}

QString CustomFunctionData::paramToString(const ModelData * model) const
{
  QStringList qs;
  if (func <= FuncInstantTrim) {
    return QString("%1").arg(param);
  }
  else if (func==FuncLogs) {
    return QString("%1").arg(param/10.0) + QObject::tr("s");
  }
  else if (func==FuncPlaySound) {
    CustomFunctionData::populatePlaySoundParams(qs);
    if (param>=0 && param<(int)qs.count())
      return qs.at(param);
    else
      return QObject::tr("<font color=red><b>Inconsistent parameter</b></font>");
  }
  else if (func==FuncPlayHaptic) {
    CustomFunctionData::populateHapticParams(qs);
    if (param>=0 && param<(int)qs.count())
      return qs.at(param);
    else
      return QObject::tr("<font color=red><b>Inconsistent parameter</b></font>");
  }
  else if (func==FuncReset) {
    QComboBox cb;
    CustomFunctionData::populateResetParams(model, &cb);
    int pos = cb.findData(param);
    if (pos >= 0)
      return cb.itemText(pos);
    else
      return QObject::tr("<font color=red><b>Inconsistent parameter</b></font>");
  }
  else if ((func==FuncVolume)|| (func==FuncPlayValue)) {
    RawSource item(param);
    return item.toString(model);
  }
  else if ((func==FuncPlayPrompt) || (func==FuncPlayBoth)) {
    if ( GetCurrentFirmware()->getCapability(VoicesAsNumbers)) {
      return QString("%1").arg(param);
    }
    else {
      return paramarm;
    }
  }
  else if ((func>=FuncAdjustGV1) && (func<FuncCount)) {
    switch (adjustMode) {
      case FUNC_ADJUST_GVAR_CONSTANT:
        return QObject::tr("Value ")+QString("%1").arg(param);
      case FUNC_ADJUST_GVAR_SOURCE:
      case FUNC_ADJUST_GVAR_GVAR:
        return RawSource(param).toString();
      case FUNC_ADJUST_GVAR_INCDEC:
        if (param==0) return QObject::tr("Decr:") + " -1";
        else          return QObject::tr("Incr:") + " +1";
    }
  }
  return "";
}

QString CustomFunctionData::repeatToString() const
{
  if (repeatParam == -1) {
    return QObject::tr("played once, not during startup");
  }
  else if (repeatParam == 0) {
    return "";
  }
  else {
    unsigned int step = IS_ARM(GetEepromInterface()->getBoard()) ? 1 : 10;
    return QObject::tr("repeat(%1s)").arg(step*repeatParam);
  }
}

QString CustomFunctionData::enabledToString() const
{
  if ((func>=FuncOverrideCH1 && func<=FuncOverrideCH32) ||
      (func>=FuncAdjustGV1 && func<=FuncAdjustGVLast) ||
      (func==FuncReset) ||
      (func>=FuncSetTimer1 && func<=FuncSetTimer2) ||
      (func==FuncVolume) ||
      (func <= FuncInstantTrim)) {
    if (!enabled) {
      return QObject::tr("DISABLED");
    }
  }
  return "";
}

CurveData::CurveData()
{
  clear(5);
}

void CurveData::clear(int count)
{
  memset(this, 0, sizeof(CurveData));
  this->count = count;
}

bool CurveData::isEmpty() const
{
  for (int i=0; i<count; i++) {
    if (points[i].y != 0) {
      return false;
    }
  }
  return true;
}

QString LimitData::minToString() const
{
  return QString::number((qreal)min/10);
}

QString LimitData::maxToString() const
{
  return QString::number((qreal)max/10);
}

QString LimitData::revertToString() const
{
  return revert ? QObject::tr("INV") : QObject::tr("NOR");
}

QString LimitData::offsetToString() const
{
  return QString::number((qreal)offset/10, 'f', 1);
}

void LimitData::clear()
{
  memset(this, 0, sizeof(LimitData));
  min = -1000;
  max = +1000;
}

GeneralSettings::SwitchInfo GeneralSettings::switchInfoFromSwitchPositionTaranis(unsigned int index)
{
  return SwitchInfo((index-1)/3, (index-1)%3);
}

bool GeneralSettings::switchPositionAllowedTaranis(int index) const
{
  if (index == 0)
    return true;
  SwitchInfo info = switchInfoFromSwitchPositionTaranis(abs(index));
  if (index < 0 && switchConfig[info.index] != Firmware::SWITCH_3POS)
    return false;
  else if (info.position == 1)
    return switchConfig[info.index] == Firmware::SWITCH_3POS;
  else
    return switchConfig[info.index] != Firmware::SWITCH_NONE;
}

bool GeneralSettings::switchSourceAllowedTaranis(int index) const
{
  return switchConfig[index] != Firmware::SWITCH_NONE;
}

bool GeneralSettings::isPotAvailable(int index) const
{
  if (index<0 || index>GetCurrentFirmware()->getCapability(Pots)) return false;
  return potConfig[index] != POT_NONE;
}

bool GeneralSettings::isSliderAvailable(int index) const
{
  if (index<0 || index>GetCurrentFirmware()->getCapability(Sliders)) return false;
  return sliderConfig[index] != SLIDER_NONE;
}

GeneralSettings::GeneralSettings()
{
  memset(this, 0, sizeof(GeneralSettings));

  contrast  = 25;
  vBatWarn  = 90;

  for (int i=0; i<CPN_MAX_STICKS+CPN_MAX_POTS; ++i) {
    calibMid[i]     = 0x200;
    calibSpanNeg[i] = 0x180;
    calibSpanPos[i] = 0x180;
  }

  for (int i=0; i<GetCurrentFirmware()->getCapability(Switches); i++) {
    switchConfig[i] = GetCurrentFirmware()->getSwitch(i).type;
  }

  BoardEnum board = GetEepromInterface()->getBoard();
  if (board == BOARD_HORUS) {
    potConfig[0] = POT_WITH_DETENT;
    potConfig[1] = POT_MULTIPOS_SWITCH;
    potConfig[2] = POT_WITH_DETENT;
    sliderConfig[0] = SLIDER_WITH_DETENT;
    sliderConfig[1] = SLIDER_WITH_DETENT;
    sliderConfig[2] = SLIDER_WITH_DETENT;
    sliderConfig[3] = SLIDER_WITH_DETENT;
  }
  else if (IS_TARANIS(board)) {
    potConfig[0] = POT_WITH_DETENT;
    potConfig[1] = POT_WITH_DETENT;
    sliderConfig[0] = SLIDER_WITH_DETENT;
    sliderConfig[1] = SLIDER_WITH_DETENT;
  }
  else {
    for (int i=0; i<3; i++) {
      potConfig[i] = POT_WITHOUT_DETENT;
    }
  }

  if (IS_ARM(board)) {
    speakerVolume = 12;
  }

  if (IS_TARANIS_X9E(board)) {
    strcpy(bluetoothName, "Taranis");
  }

  templateSetup = g.profile[g.id()].channelOrder();
  stickMode = g.profile[g.id()].defaultMode();

  QString t_calib = g.profile[g.id()].stickPotCalib();
  int potsnum = GetCurrentFirmware()->getCapability(Pots);
  if (!t_calib.isEmpty()) {
    QString t_trainercalib=g.profile[g.id()].trainerCalib();
    int8_t t_txVoltageCalibration=(int8_t)g.profile[g.id()].txVoltageCalibration();
    int8_t t_txCurrentCalibration=(int8_t)g.profile[g.id()].txCurrentCalibration();
    int8_t t_PPM_Multiplier=(int8_t)g.profile[g.id()].ppmMultiplier();
    uint8_t t_stickMode=(uint8_t)g.profile[g.id()].gsStickMode();
    uint8_t t_vBatWarn=(uint8_t)g.profile[g.id()].vBatWarn();
    QString t_DisplaySet=g.profile[g.id()].display();
    QString t_BeeperSet=g.profile[g.id()].beeper();
    QString t_HapticSet=g.profile[g.id()].haptic();
    QString t_SpeakerSet=g.profile[g.id()].speaker();
    QString t_CountrySet=g.profile[g.id()].countryCode();

    if ((t_calib.length()==(CPN_MAX_STICKS+potsnum)*12) && (t_trainercalib.length()==16)) {
      QString Byte;
      int16_t byte16;
      bool ok;
      for (int i=0; i<(CPN_MAX_STICKS+potsnum); i++) {
        Byte=t_calib.mid(i*12,4);
        byte16=(int16_t)Byte.toInt(&ok,16);
        if (ok)
          calibMid[i]=byte16;
        Byte=t_calib.mid(4+i*12,4);
        byte16=(int16_t)Byte.toInt(&ok,16);
        if (ok)
          calibSpanNeg[i]=byte16;
        Byte=t_calib.mid(8+i*12,4);
        byte16=(int16_t)Byte.toInt(&ok,16);
        if (ok)
          calibSpanPos[i]=byte16;
      }
      for (int i=0; i<4; i++) {
        Byte=t_trainercalib.mid(i*4,4);
        byte16=(int16_t)Byte.toInt(&ok,16);
        if (ok)
          trainer.calib[i]=byte16;
      }
      txCurrentCalibration=t_txCurrentCalibration;
      txVoltageCalibration=t_txVoltageCalibration;
      vBatWarn=t_vBatWarn;
      PPM_Multiplier=t_PPM_Multiplier;
      stickMode = t_stickMode;
    }
    if ((t_DisplaySet.length()==6) && (t_BeeperSet.length()==4) && (t_HapticSet.length()==6) && (t_SpeakerSet.length()==6)) {
      uint8_t byte8u;
      int8_t byte8;
      bool ok;
      byte8=(int8_t)t_DisplaySet.mid(0,2).toInt(&ok,16);
      if (ok)
        optrexDisplay=(byte8==1 ? true : false);
      byte8u=(uint8_t)t_DisplaySet.mid(2,2).toUInt(&ok,16);
      if (ok)
        contrast=byte8u;
      byte8u=(uint8_t)t_DisplaySet.mid(4,2).toUInt(&ok,16);
      if (ok)
        backlightBright=byte8u;
      byte8=(int8_t)t_BeeperSet.mid(0,2).toUInt(&ok,16);
      if (ok)
        beeperMode=(BeeperMode)byte8;
      byte8=(int8_t)t_BeeperSet.mid(2,2).toInt(&ok,16);
      if (ok)
        beeperLength=byte8;
      byte8=(int8_t)t_HapticSet.mid(0,2).toUInt(&ok,16);
      if (ok)
        hapticMode=(BeeperMode)byte8;
      byte8=(int8_t)t_HapticSet.mid(2,2).toInt(&ok,16);
      if (ok)
        hapticStrength=byte8;
      byte8=(int8_t)t_HapticSet.mid(4,2).toInt(&ok,16);
      if (ok)
        hapticLength=byte8;
      byte8u=(uint8_t)t_SpeakerSet.mid(0,2).toUInt(&ok,16);
      if (ok)
        speakerMode=byte8u;
      byte8u=(uint8_t)t_SpeakerSet.mid(2,2).toUInt(&ok,16);
      if (ok)
        speakerPitch=byte8u;
      byte8u=(uint8_t)t_SpeakerSet.mid(4,2).toUInt(&ok,16);
      if (ok)
        speakerVolume=byte8u;
      if (t_CountrySet.length()==6) {
        byte8u=(uint8_t)t_CountrySet.mid(0,2).toUInt(&ok,16);
        if (ok)
          countryCode=byte8u;
        byte8u=(uint8_t)t_CountrySet.mid(2,2).toUInt(&ok,16);
        if (ok)
          imperial=byte8u;
        QString chars = t_CountrySet.mid(4, 2);
        ttsLanguage[0] = chars[0].toLatin1();
        ttsLanguage[1] = chars[1].toLatin1();
      }
    }
  }

  strcpy(themeName, "default");
  ThemeOptionData option1 = { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0 };
  memcpy(&themeOptionValue[0], option1, sizeof(ThemeOptionData));
  ThemeOptionData option2 = { 0x03, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0 };
  memcpy(&themeOptionValue[1], option2, sizeof(ThemeOptionData));
}

int GeneralSettings::getDefaultStick(unsigned int channel) const
{
  if (channel >= CPN_MAX_STICKS)
    return -1;
  else
    return chout_ar[4*templateSetup + channel] - 1;
}

RawSource GeneralSettings::getDefaultSource(unsigned int channel) const
{
  int stick = getDefaultStick(channel);
  if (stick >= 0)
    return RawSource(SOURCE_TYPE_STICK, stick);
  else
    return RawSource(SOURCE_TYPE_NONE);
}

int GeneralSettings::getDefaultChannel(unsigned int stick) const
{
  for (int i=0; i<4; i++){
    if (getDefaultStick(i) == (int)stick)
      return i;
  }
  return -1;
}

float FrSkyChannelData::getRatio() const
{
  if (type==0 || type==1 || type==2)
    return float(ratio << multiplier) / 10.0;
  else
    return ratio << multiplier;
}

RawSourceRange FrSkyChannelData::getRange() const
{
  RawSourceRange result;
  float ratio = getRatio();
  if (type==0 || type==1 || type==2)
    result.decimals = 2;
  else
    result.decimals = 0;
  result.step = ratio / 255;
  result.min = offset * result.step;
  result.max = ratio + result.min;
  result.unit = QObject::tr("V");
  return result;
}

void FrSkyScreenData::clear()
{
  memset(this, 0, sizeof(FrSkyScreenData));
  if (!IS_ARM(GetCurrentFirmware()->getBoard())) {
    type = TELEMETRY_SCREEN_NUMBERS;
  }
}

void FrSkyData::clear()
{
  usrProto = 0;
  voltsSource = 0;
  altitudeSource = 0;
  currentSource = 0;
  varioMin = 0;
  varioCenterMin = 0;    // if increment in 0.2m/s = 3.0m/s max
  varioCenterMax = 0;
  varioMax = 0;
  mAhPersistent = 0;
  storedMah = 0;
  fasOffset = 0;
  rssiAlarms[0].clear(2, 45);
  rssiAlarms[1].clear(3, 42);
  for (int i=0; i<4; i++)
    screens[i].clear();
  varioSource = 2/*VARIO*/;
  blades = 2;
}

ModelData::ModelData()
{
  clear();
}

ModelData::ModelData(const ModelData & src)
{
  *this = src;
}

ModelData & ModelData::operator = (const ModelData & src)
{
  memcpy(this, &src, sizeof(ModelData));
  return *this;
}

ExpoData * ModelData::insertInput(const int idx)
{
  memmove(&expoData[idx+1], &expoData[idx], (CPN_MAX_EXPOS-(idx+1))*sizeof(ExpoData));
  expoData[idx].clear();
  return &expoData[idx];
}

bool ModelData::isInputValid(const unsigned int idx) const
{
  for (int i=0; i<CPN_MAX_EXPOS; i++) {
    const ExpoData * expo = &expoData[i];
    if (expo->mode == 0) break;
    if (expo->chn == idx)
      return true;
  }
  return false;
}

bool ModelData::hasExpos(uint8_t inputIdx) const
{
  for (int i=0; i<CPN_MAX_EXPOS; i++) {
    const ExpoData & expo = expoData[i];
    if (expo.chn==inputIdx && expo.mode!=0) {
      return true;
    }
  }
  return false;
}

bool ModelData::hasMixes(uint8_t channelIdx) const
{
  channelIdx += 1;
  for (int i=0; i<CPN_MAX_MIXERS; i++) {
    if (mixData[i].destCh == channelIdx) {
      return true;
    }
  }
  return false;
}

QVector<const ExpoData *> ModelData::expos(int input) const
{
  QVector<const ExpoData *> result;
  for (int i=0; i<CPN_MAX_EXPOS; i++) {
    const ExpoData * ed = &expoData[i];
    if ((int)ed->chn==input && ed->mode!=0) {
      result << ed;
    }
  }
  return result;
}

QVector<const MixData *> ModelData::mixes(int channel) const
{
  QVector<const MixData *> result;
  for (int i=0; i<CPN_MAX_MIXERS; i++) {
    const MixData * md = &mixData[i];
    if ((int)md->destCh == channel+1) {
      result << md;
    }
  }
  return result;
}

void ModelData::removeInput(const int idx)
{
  unsigned int chn = expoData[idx].chn;

  memmove(&expoData[idx], &expoData[idx+1], (CPN_MAX_EXPOS-(idx+1))*sizeof(ExpoData));
  expoData[CPN_MAX_EXPOS-1].clear();

  //also remove input name if removing last line for this input
  bool found = false;
  for (int i=0; i<CPN_MAX_EXPOS; i++) {
    if (expoData[i].mode==0) continue;
    if (expoData[i].chn==chn) {
      found = true;
      break;
    }
  }
  if (!found) inputNames[chn][0] = 0;
}

void ModelData::clearInputs()
{
  for (int i=0; i<CPN_MAX_EXPOS; i++)
    expoData[i].clear();

  //clear all input names
  if (GetCurrentFirmware()->getCapability(VirtualInputs)) {
    for (int i=0; i<CPN_MAX_INPUTS; i++) {
      inputNames[i][0] = 0;
    }
  }
}

void ModelData::clearMixes()
{
  for (int i=0; i<CPN_MAX_MIXERS; i++)
    mixData[i].clear();
}

RadioData::RadioData()
{
  models.resize(GetCurrentFirmware()->getCapability(Models));
}

void ModelData::clear()
{
  memset(this, 0, sizeof(ModelData));
  moduleData[0].channelsCount = 8;
  moduleData[1].channelsStart = 0;
  moduleData[1].channelsCount = 8;
  moduleData[0].ppm.delay = 300;
  moduleData[1].ppm.delay = 300;
  moduleData[2].ppm.delay = 300;
  int board = GetEepromInterface()->getBoard();
  if (IS_TARANIS(board) || IS_HORUS(board)) {
    moduleData[0].protocol = PULSES_PXX_XJT_X16;
    moduleData[1].protocol = PULSES_OFF;
  }
  else if (IS_SKY9X(board)) {
    moduleData[0].protocol = PULSES_PPM;
    moduleData[1].protocol = PULSES_PPM;
  }
  else {
    moduleData[0].protocol = PULSES_PPM;
    moduleData[1].protocol = PULSES_OFF;
  }
  for (int i=0; i<CPN_MAX_FLIGHT_MODES; i++) {
    flightModeData[i].clear(i);
  }
  clearInputs();
  clearMixes();
  for (int i=0; i<CPN_MAX_CHNOUT; i++)
    limitData[i].clear();
  for (int i=0; i<CPN_MAX_STICKS; i++)
    expoData[i].clear();
  for (int i=0; i<CPN_MAX_CSW; i++)
    logicalSw[i].clear();
  for (int i=0; i<CPN_MAX_CUSTOM_FUNCTIONS; i++)
    customFn[i].clear();
  for (int i=0; i<CPN_MAX_CURVES; i++)
    curves[i].clear(5);
  for (int i=0; i<CPN_MAX_TIMERS; i++)
    timers[i].clear();
  swashRingData.clear();
  frsky.clear();
  for (int i=0; i<CPN_MAX_SENSORS; i++)
    sensorData[i].clear();
}

bool ModelData::isEmpty() const
{
  return !used;
}

QString removeAccents(const QString & str)
{
  QString result = str;

  // UTF-8 ASCII Table
  const QString tA[] = { "á", "â", "ã", "à", "ä" };
  const QString tE[] = { "é", "è", "ê", "ě" };
  const QString tI[] = { "í" };
  const QString tO[] = { "ó", "ô", "õ", "ö" };
  const QString tU[] = { "ú", "ü" };
  const QString tC[] = { "ç" };
  const QString tY[] = { "ý" };
  const QString tS[] = { "š" };
  const QString tR[] = { "ř" };

  for (unsigned int i = 0; i < DIM(tA); i++) result.replace(tA[i], "a");
  for (unsigned int i = 0; i < DIM(tE); i++) result.replace(tE[i], "e");
  for (unsigned int i = 0; i < DIM(tI); i++) result.replace(tI[i], "i");
  for (unsigned int i = 0; i < DIM(tO); i++) result.replace(tO[i], "o");
  for (unsigned int i = 0; i < DIM(tU); i++) result.replace(tU[i], "u");
  for (unsigned int i = 0; i < DIM(tC); i++) result.replace(tC[i], "c");
  for (unsigned int i = 0; i < DIM(tY); i++) result.replace(tY[i], "y");
  for (unsigned int i = 0; i < DIM(tS); i++) result.replace(tS[i], "s");
  for (unsigned int i = 0; i < DIM(tR); i++) result.replace(tR[i], "r");

  return result;
}

void ModelData::setDefaultInputs(const GeneralSettings & settings)
{
  BoardEnum board = GetEepromInterface()->getBoard();
  if (IS_ARM(board)) {
    for (int i=0; i<CPN_MAX_STICKS; i++) {
      ExpoData * expo = &expoData[i];
      expo->chn = i;
      expo->mode = INPUT_MODE_BOTH;
      expo->srcRaw = settings.getDefaultSource(i);
      expo->weight = 100;
      strncpy(inputNames[i], removeAccents(expo->srcRaw.toString(this)).toLatin1().constData(), sizeof(inputNames[i])-1);
    }
  }
}

void ModelData::setDefaultMixes(const GeneralSettings & settings)
{
  BoardEnum board = GetEepromInterface()->getBoard();
  if (IS_ARM(board)) {
    setDefaultInputs(settings);
  }

  for (int i=0; i<CPN_MAX_STICKS; i++) {
    MixData * mix = &mixData[i];
    mix->destCh = i+1;
    mix->weight = 100;
    if (IS_ARM(board)) {
      mix->srcRaw = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, i);
    }
    else {
      mix->srcRaw = RawSource(SOURCE_TYPE_STICK, i);
    }
  }
}

void ModelData::setDefaultValues(unsigned int id, const GeneralSettings & settings)
{
  clear();
  used = true;
  sprintf(name, "MODEL%02d", id+1);
  for (int i=0; i<CPN_MAX_MODULES; i++) {
    moduleData[i].modelId = id+1;
  }
  setDefaultMixes(settings);
}

int ModelData::getTrimValue(int phaseIdx, int trimIdx)
{
  int result = 0;
  for (int i=0; i<CPN_MAX_FLIGHT_MODES; i++) {
    FlightModeData & phase = flightModeData[phaseIdx];
    if (phase.trimMode[trimIdx] < 0) {
      return result;
    }
    else {
      if (phase.trimRef[trimIdx] == phaseIdx || phaseIdx == 0) {
        return result + phase.trim[trimIdx];
      }
      else {
        phaseIdx = phase.trimRef[trimIdx];
        if (phase.trimMode[trimIdx] != 0)
          result += phase.trim[trimIdx];
      }
    }
  }
  return 0;
}

bool ModelData::isGVarLinked(int phaseIdx, int gvarIdx)
{
  return flightModeData[phaseIdx].gvars[gvarIdx] > 1024;
}

int ModelData::getGVarFieldValue(int phaseIdx, int gvarIdx)
{
  int idx = flightModeData[phaseIdx].gvars[gvarIdx];
  for (int i=0; idx>1024 && i<CPN_MAX_FLIGHT_MODES; i++) {
    int nextPhase = idx - 1025;
    if (nextPhase >= phaseIdx) nextPhase += 1;
    phaseIdx = nextPhase;
    idx = flightModeData[phaseIdx].gvars[gvarIdx];
  }
  return idx;
}

void ModelData::setTrimValue(int phaseIdx, int trimIdx, int value)
{
  for (uint8_t i=0; i<CPN_MAX_FLIGHT_MODES; i++) {
    FlightModeData & phase = flightModeData[phaseIdx];
    int mode = phase.trimMode[trimIdx];
    int p = phase.trimRef[trimIdx];
    int & trim = phase.trim[trimIdx];
    if (mode < 0)
      return;
    if (p == phaseIdx || phaseIdx == 0) {
      trim = value;
      break;;
    }
    else if (mode == 0) {
      phaseIdx = p;
    }
    else {
      trim = value - getTrimValue(p, trimIdx);
      if (trim < -500)
        trim = -500;
      if (trim > 500)
        trim = 500;
      break;
    }
  }
}

void ModelData::removeGlobalVar(int & var)
{
  if (var >= 126 && var <= 130)
    var = flightModeData[0].gvars[var-126];
  else if (var <= -126 && var >= -130)
    var = - flightModeData[0].gvars[-126-var];
}

ModelData ModelData::removeGlobalVars()
{
  ModelData result = *this;

  for (int i=0; i<CPN_MAX_MIXERS; i++) {
    removeGlobalVar(mixData[i].weight);
    removeGlobalVar(mixData[i].curve.value);
    removeGlobalVar(mixData[i].sOffset);
  }

  for (int i=0; i<CPN_MAX_EXPOS; i++) {
    removeGlobalVar(expoData[i].weight);
    removeGlobalVar(expoData[i].curve.value);
  }

  return result;
}

int ModelData::getChannelsMax(bool forceExtendedLimits) const
{
  if (forceExtendedLimits || extendedLimits)
    return IS_TARANIS(GetCurrentFirmware()->getBoard()) ? 150 : 125;
  else
    return 100;
}

QList<EEPROMInterface *> eepromInterfaces;
void registerEEpromInterfaces()
{
  eepromInterfaces.push_back(new OpenTxEepromInterface(BOARD_STOCK));
  eepromInterfaces.push_back(new OpenTxEepromInterface(BOARD_M128));
  eepromInterfaces.push_back(new OpenTxEepromInterface(BOARD_GRUVIN9X));
  eepromInterfaces.push_back(new OpenTxEepromInterface(BOARD_SKY9X));
  eepromInterfaces.push_back(new OpenTxEepromInterface(BOARD_9XRPRO));
  eepromInterfaces.push_back(new OpenTxEepromInterface(BOARD_TARANIS_X9D));
  eepromInterfaces.push_back(new OpenTxEepromInterface(BOARD_TARANIS_X9DP));
  eepromInterfaces.push_back(new OpenTxEepromInterface(BOARD_TARANIS_X9E));
  // eepromInterfaces.push_back(new Ersky9xInterface());
  // eepromInterfaces.push_back(new Er9xInterface());
}

void unregisterEEpromInterfaces()
{
  foreach(EEPROMInterface * intf, eepromInterfaces) {
    // qDebug() << "UnregisterEepromInterfaces(): deleting " <<  QString::number( reinterpret_cast<uint64_t>(intf), 16 );
    delete intf;
  }
  OpenTxEepromCleanup();
}

QList<Firmware *> firmwares;
Firmware * default_firmware_variant;
Firmware * current_firmware_variant;

void ShowEepromErrors(QWidget *parent, const QString &title, const QString &mainMessage, unsigned long errorsFound)
{
  std::bitset<NUM_ERRORS> errors((unsigned long long)errorsFound);
  QStringList errorsList;

  errorsList << QT_TRANSLATE_NOOP("EepromInterface", "Possible causes for this:");

  if (errors.test(UNSUPPORTED_NEWER_VERSION)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Eeprom is from a newer version of OpenTX"); }
  if (errors.test(NOT_OPENTX)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Eeprom is not from OpenTX"); }
  if (errors.test(NOT_TH9X)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Eeprom is not from Th9X"); }
  if (errors.test(NOT_GRUVIN9X)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Eeprom is not from Gruvin9X"); }
  if (errors.test(NOT_ERSKY9X)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Eeprom is not from ErSky9X"); }
  if (errors.test(NOT_ER9X)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Eeprom is not from Er9X"); }
  if (errors.test(WRONG_SIZE)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Eeprom size is invalid"); }
  if (errors.test(WRONG_FILE_SYSTEM)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Eeprom file system is invalid"); }
  if (errors.test(UNKNOWN_BOARD)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Eeprom is from a unknown board"); }
  if (errors.test(WRONG_BOARD)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Eeprom is from the wrong board"); }
  if (errors.test(BACKUP_NOT_SUPPORTED)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Eeprom backup not supported"); }

  if (errors.test(UNKNOWN_ERROR)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Something that couldn't be guessed, sorry"); }

  if (errors.test(HAS_WARNINGS)) {
    errorsList << QT_TRANSLATE_NOOP("EepromInterface", "Warning:");
    if (errors.test(WARNING_WRONG_FIRMWARE)) { errorsList << QT_TRANSLATE_NOOP("EepromInterface", "- Your radio probably uses a wrong firmware,\n eeprom size is 4096 but only the first 2048 are used"); }
  }

  QMessageBox msgBox(parent);
  msgBox.setWindowTitle(title);
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(mainMessage);
  msgBox.setInformativeText(errorsList.join("\n"));
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

void ShowEepromWarnings(QWidget *parent, const QString &title, unsigned long errorsFound)
{
  std::bitset<NUM_ERRORS> errors((unsigned long long)errorsFound);
  QStringList warningsList;
  if (errors.test(WARNING_WRONG_FIRMWARE)) { warningsList << QT_TRANSLATE_NOOP("EepromInterface", "- Your radio probably uses a wrong firmware,\n eeprom size is 4096 but only the first 2048 are used"); }
  if (errors.test(OLD_VERSION)) { warningsList << QT_TRANSLATE_NOOP("EepromInterface", "- Your eeprom is from an old version of OpenTX, upgrading!\n You should 'save as' to keep the old file as a backup."); }

  QMessageBox msgBox(parent);
  msgBox.setWindowTitle(title);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setText(QT_TRANSLATE_NOOP("EepromInterface", "Warnings!"));
  msgBox.setInformativeText(warningsList.join("\n"));
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

QString getBoardName(BoardEnum board)
{
  switch (board) {
    case BOARD_STOCK:
      return "9X";
    case BOARD_M128:
      return "9X128";
    case BOARD_GRUVIN9X:
      return "Gruvin9x";
    case BOARD_MEGA2560:
      return "MEGA2560";
    case BOARD_TARANIS_X7:
      return "Taranis X7";
    case BOARD_TARANIS_X9D:
      return "Taranis X9D";
    case BOARD_TARANIS_X9DP:
      return "Taranis X9D+";
    case BOARD_TARANIS_X9E:
      return "Taranis X9E";
    case BOARD_SKY9X:
      return "Sky9x";
    case BOARD_9XRPRO:
      return "9XR-PRO";
    case BOARD_AR9X:
      return "AR9X";
    case BOARD_HORUS:
      return "Horus";
    default:
      return "Unknown";
  }
}

const int Firmware::getFlashSize()
{
  switch (board) {
    case BOARD_STOCK:
      return FSIZE_STOCK;
    case BOARD_M128:
      return FSIZE_M128;
    case BOARD_MEGA2560:
    case BOARD_GRUVIN9X:
      return FSIZE_GRUVIN9X;
    case BOARD_SKY9X:
      return FSIZE_SKY9X;
    case BOARD_9XRPRO:
    case BOARD_AR9X:
      return FSIZE_9XRPRO;
    case BOARD_TARANIS_X9D:
    case BOARD_TARANIS_X9DP:
    case BOARD_TARANIS_X9E:
    case BOARD_FLAMENCO:
      return FSIZE_TARANIS;
    case BOARD_HORUS:
      return FSIZE_HORUS;
    default:
      return 0;
  }
}

Firmware * GetFirmware(QString id)
{
  foreach(Firmware * firmware, firmwares) {
    Firmware * result = firmware->getFirmwareVariant(id);
    if (result) {
      return result;
    }
  }

  return default_firmware_variant;
}

void Firmware::addOption(const char *option, QString tooltip, uint32_t variant)
{
  Option options[] = { { option, tooltip, variant }, { NULL } };
  addOptions(options);
}

unsigned int Firmware::getVariantNumber()
{
  unsigned int result = 0;
  const Firmware * base = getFirmwareBase();
  QStringList options = id.mid(base->getId().length()+1).split("-", QString::SkipEmptyParts);
  foreach(QString option, options) {
    foreach(QList<Option> group, base->opts) {
      foreach(Option opt, group) {
        if (opt.name == option) {
          result += opt.variant;
        }
      }
    }
  }
  return result;
}

void Firmware::addLanguage(const char *lang)
{
  languages.push_back(lang);
}

void Firmware::addTTSLanguage(const char *lang)
{
  ttslanguages.push_back(lang);
}

void Firmware::addOptions(Option options[])
{
  QList<Option> opts;
  for (int i=0; options[i].name; i++) {
    opts.push_back(options[i]);
  }
  this->opts.push_back(opts);
}

SimulatorInterface *GetCurrentFirmwareSimulator()
{
  QString firmwareId = GetCurrentFirmware()->getId();
  SimulatorFactory *factory = getSimulatorFactory(firmwareId);
  if (factory)
    return factory->create();
  else
    return NULL;
}

unsigned int getNumSubtypes(MultiModuleRFProtocols type) {
  switch (type) {
    case MM_RF_PROTO_HISKY:
    case MM_RF_PROTO_SYMAX:
    case MM_RF_PROTO_KN:
    case MM_RF_PROTO_SLT:
    case MM_RF_PROTO_Q2X2:
    case MM_RF_PROTO_FY326:
    case MM_RF_PROTO_BAYANG:
    case MM_RF_PROTO_V2X2:
      return 2;

    case MM_RF_PROTO_CG023:
      return 3;

    case MM_RF_PROTO_FRSKY:
    case MM_RF_PROTO_DSM2:
    case MM_RF_PROTO_MT99XX:
    case MM_RF_PROTO_HONTAI:
    case MM_RF_PROTO_AFHDS2A:
      return 4;

    case MM_RF_PROTO_FLYSKY:
    case MM_RF_PROTO_MJXQ:
    case MM_RF_PROTO_YD717:
      return 5;

    case MM_RF_PROTO_CX10:
      return 8;
    default:
      return 1;
  }

}

void FlightModeData::clear(const int phase)
{
  memset(this, 0, sizeof(FlightModeData));
  if (phase != 0) {
    for (int idx=0; idx<CPN_MAX_GVARS; idx++) {
      gvars[idx] = 1025;
    }
    for (int idx=0; idx<CPN_MAX_ENCODERS; idx++) {
      rotaryEncoders[idx] = 1025;
    }
  }
}
