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

#include "modeldata.h"

#include "eeprominterface.h"
#include "generalsettings.h"
#include "macros.h"
#include "radiodataconversionstate.h"


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


/*
 * TimerData
 */

void TimerData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent(tr("TMR"), 1);
  cstate.setSubComp(tr("Timer %1").arg(cstate.subCompIdx + 1));
  mode.convert(cstate);
}


/*
 * ModelData
 */

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

void ModelData::removeInput(const int idx, bool clearName)
{
  unsigned int chn = expoData[idx].chn;

  memmove(&expoData[idx], &expoData[idx+1], (CPN_MAX_EXPOS-(idx+1))*sizeof(ExpoData));
  expoData[CPN_MAX_EXPOS-1].clear();

  //also remove input name if removing last line for this input
  if (clearName && !expos(chn).size())
    inputNames[chn][0] = 0;
}

void ModelData::clearInputs()
{
  for (int i=0; i<CPN_MAX_EXPOS; i++)
    expoData[i].clear();

  //clear all input names
  if (getCurrentFirmware()->getCapability(VirtualInputs)) {
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

void ModelData::clear()
{
  memset(this, 0, sizeof(ModelData));
  modelIndex = -1;  // an invalid index, this is managed by the TreeView data model
  moduleData[0].channelsCount = 8;
  moduleData[1].channelsStart = 0;
  moduleData[1].channelsCount = 8;
  moduleData[0].ppm.delay = 300;
  moduleData[1].ppm.delay = 300;
  moduleData[2].ppm.delay = 300;
  int board = getCurrentBoard();
  if (IS_HORUS_OR_TARANIS(board)) {
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
  for (int i=0; i<CPN_MAX_GVARS; i++) {
    gvarData[i].clear();
  }
  clearInputs();
  clearMixes();
  for (int i=0; i<CPN_MAX_CHNOUT; i++)
    limitData[i].clear();
  for (int i=0; i<CPN_MAX_STICKS; i++)
    expoData[i].clear();
  for (int i=0; i<CPN_MAX_LOGICAL_SWITCHES; i++)
    logicalSw[i].clear();
  for (int i=0; i<CPN_MAX_SPECIAL_FUNCTIONS; i++)
    customFn[i].clear();
  for (int i=0; i<CPN_MAX_CURVES; i++)
    curves[i].clear(5);
  for (int i=0; i<CPN_MAX_TIMERS; i++)
    timers[i].clear();
  swashRingData.clear();
  frsky.clear();
  rssiAlarms.clear();
  for (int i=0; i<CPN_MAX_SENSORS; i++)
    sensorData[i].clear();

  static const uint8_t blob[] = { 0x4c, 0x61, 0x79, 0x6f, 0x75, 0x74, 0x32, 0x50, 0x31, 0x00, 0x4d, 0x6f, 0x64, 0x65, 0x6c, 0x42, 0x6d, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  memcpy(customScreenData[0], blob, sizeof(blob));
}

bool ModelData::isEmpty() const
{
  return !used;
}

void ModelData::setDefaultInputs(const GeneralSettings & settings)
{
  Board::Type board = getCurrentBoard();
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
  Board::Type board = getCurrentBoard();
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
  for (int i=0; idx>GVAR_MAX_VALUE && i<CPN_MAX_FLIGHT_MODES; i++) {
    int nextPhase = idx - GVAR_MAX_VALUE - 1;
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
    return IS_HORUS_OR_TARANIS(getCurrentBoard()) ? 150 : 125;
  else
    return 100;
}

bool ModelData::isAvailable(const RawSwitch & swtch) const
{
  unsigned index = abs(swtch.index) - 1;

  if (swtch.type == SWITCH_TYPE_VIRTUAL) {
    return logicalSw[index].func != LS_FN_OFF;
  }
  else if (swtch.type == SWITCH_TYPE_FLIGHT_MODE) {
    return index == 0 || flightModeData[index].swtch.type != SWITCH_TYPE_NONE;
  }
  else if (swtch.type == SWITCH_TYPE_SENSOR) {
    return strlen(sensorData[index].label) > 0;
  }
  else {
    return true;
  }
}

float ModelData::getGVarFieldValuePrec(int phaseIdx, int gvarIdx)
{
  return getGVarFieldValue(phaseIdx, gvarIdx) * gvarData[gvarIdx].multiplierGet();
}

void ModelData::convert(RadioDataConversionState & cstate)
{
  // Here we can add explicit conversions when moving from one board to another

  QString origin = QString(name);
  if (origin.isEmpty())
    origin = QString::number(cstate.modelIdx+1);
  cstate.setOrigin(tr("Model: ") % origin);

  cstate.setComponent("SET", 0);
  if (thrTraceSrc && (int)thrTraceSrc < cstate.fromBoard.getCapability(Board::Pots) + cstate.fromBoard.getCapability(Board::Sliders)) {
    cstate.setSubComp(tr("Throttle Source"));
    thrTraceSrc = RawSource(SOURCE_TYPE_STICK, (int)thrTraceSrc + 3).convert(cstate).index - 3;
  }

  for (int i=0; i<CPN_MAX_TIMERS; i++) {
    timers[i].convert(cstate.withComponentIndex(i));
  }

  for (int i=0; i<CPN_MAX_MIXERS; i++) {
    mixData[i].convert(cstate.withComponentIndex(i));
  }

  for (int i=0; i<CPN_MAX_EXPOS; i++) {
    expoData[i].convert(cstate.withComponentIndex(i));
  }

  for (int i=0; i<CPN_MAX_LOGICAL_SWITCHES; i++) {
    logicalSw[i].convert(cstate.withComponentIndex(i));
  }

  for (int i=0; i<CPN_MAX_SPECIAL_FUNCTIONS; i++) {
    customFn[i].convert(cstate.withComponentIndex(i));
  }

  for (int i=0; i<CPN_MAX_FLIGHT_MODES; i++) {
    flightModeData[i].convert(cstate.withComponentIndex(i));
  }

  for (int i=0; i<CPN_MAX_MODULES; i++) {
    moduleData[i].convert(cstate.withComponentIndex(i));
  }
}
