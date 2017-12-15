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

#include "radiodata.h"
#include "radiodataconversionstate.h"
#include "eeprominterface.h"

// TODO here we will move a lot of functions from eeprominterface.cpp when no merge risk

RawSource RawSource::convert(RadioDataConversionState & cstate)
{
  cstate.setItemType("SRC", 1);
  RadioDataConversionState::EventType evt = RadioDataConversionState::EVT_NONE;
  RadioDataConversionState::LogField oldData(index, toString(cstate.fromModel(), cstate.fromGS(), cstate.fromType));

  if (type == SOURCE_TYPE_STICK) {
    if (cstate.toBoard.getCapability(Board::Sliders)) {
      if (index >= cstate.fromBoard.getCapability(Board::Sticks) + cstate.fromBoard.getCapability(Board::Pots)) {
        // 1st slider alignment
        index += cstate.toBoard.getCapability(Board::Pots) - cstate.fromBoard.getCapability(Board::Pots);
      }

      if (isSlider(0, cstate.fromType)) {
        // LS and RS sliders are after 2 aux sliders on X12 and X9E
        if ((IS_HORUS_X12S(cstate.toType) || IS_TARANIS_X9E(cstate.toType)) && !IS_HORUS_X12S(cstate.fromType) && !IS_TARANIS_X9E(cstate.fromType)) {
          if (index >= 7) {
            index += 2;  // LS/RS to LS/RS
          }
        }
        else if (!IS_TARANIS_X9E(cstate.toType) && !IS_HORUS_X12S(cstate.toType) && (IS_HORUS_X12S(cstate.fromType) || IS_TARANIS_X9E(cstate.fromType))) {
          if (index >= 7 && index <= 8) {
            index += 2;   // aux sliders to spare analogs (which may not exist, this is validated later)
            evt = RadioDataConversionState::EVT_CVRT;
          }
          else if (index >= 9 && index <= 10) {
            index -= 2;  // LS/RS to LS/RS
          }
        }
      }
    }

    if (IS_TARANIS(cstate.toType) && IS_HORUS(cstate.fromType)) {
      if (index == 6)
        index = 5;  // pot S2 to S2
      else if (index == 5)
        index = -1;  //  6P on Horus doesn't exist on Taranis
    }
    else  if (IS_HORUS(cstate.toType) && IS_TARANIS(cstate.fromType) && index == 5)
    {
      index = 6;  // pot S2 to S2
    }

  }  // SOURCE_TYPE_STICK

  if (type == SOURCE_TYPE_SWITCH) {
    // SWI to SWR don't exist on !X9E board
    if (!IS_TARANIS_X9E(cstate.toType) && IS_TARANIS_X9E(cstate.fromType)) {
      if (index >= 8) {
        index = index % 8;
        evt = RadioDataConversionState::EVT_CVRT;
      }
    }

    if (IS_TARANIS_X7(cstate.toType) && (IS_TARANIS_X9(cstate.fromType) || IS_HORUS(cstate.fromType))) {
      // No SE and SG on X7 board
      if (index == 4 || index == 6) {
        index = 3;  // SG and SE to SD
        evt = RadioDataConversionState::EVT_CVRT;
      }
      else if (index == 5) {
        index = 4;  // SF to SF
      }
      else if (index == 7) {
        index = 5;  // SH to SH
      }
    }
    // Compensate for SE and SG on X9/Horus board if converting from X7
    else if ((IS_TARANIS_X9(cstate.toType) || IS_HORUS(cstate.toType)) && IS_TARANIS_X7(cstate.fromType)) {
      if (index == 4) {
        index = 5;  // SF to SF
      }
      else if (index == 5) {
        index = 7;  // SH to SH
      }
    }
  }  // SOURCE_TYPE_SWITCH

  // final validation (we do not pass model to isAvailable() because we don't know what has or hasn't been converted)
  if (!isAvailable(NULL, cstate.toGS(), cstate.toType)) {
    cstate.setInvalid(oldData);
    index = -1;  // TODO: better way to flag invalid sources?
    type = MAX_SOURCE_TYPE;
  }
  else if (evt == RadioDataConversionState::EVT_CVRT) {
    cstate.setConverted(oldData, RadioDataConversionState::LogField(index, toString(cstate.toModel(), cstate.toGS(), cstate.toType)));
  }
  else if (oldData.id != index) {
    // provide info by default if anything changed
    cstate.setMoved(oldData, RadioDataConversionState::LogField(index, toString(cstate.toModel(), cstate.toGS(), cstate.toType)));
  }

  return *this;
}

RawSwitch RawSwitch::convert(RadioDataConversionState & cstate)
{
  if (!index)
    return *this;

  cstate.setItemType("SW", 2);
  RadioDataConversionState::EventType evt = RadioDataConversionState::EVT_NONE;
  RadioDataConversionState::LogField oldData(index, toString(cstate.fromType, cstate.fromGS(), cstate.fromModel()));

  if (type == SWITCH_TYPE_SWITCH) {
    int srcIdx = div(abs(index)-1, 3).quot;  // raw source index
    int delta = 0;

    // SWI to SWR don't exist on !X9E board
    if (!IS_TARANIS_X9E(cstate.toType) && IS_TARANIS_X9E(cstate.fromType)) {
      if (srcIdx > 7) {
        index %= 24;
        evt = RadioDataConversionState::EVT_CVRT;
      }
    }

    // No SE and SG on X7 board
    if (IS_TARANIS_X7(cstate.toType) && (IS_TARANIS_X9(cstate.fromType) || IS_HORUS(cstate.fromType))) {
      if (srcIdx == 4 || srcIdx == 5) {
        delta = 3;  // SE to SD & SF to SF
        if (srcIdx == 4)
          evt = RadioDataConversionState::EVT_CVRT;
      }
      else if (srcIdx == 6) {
        delta = 9;  // SG to SD
        evt = RadioDataConversionState::EVT_CVRT;
      }
      else if (srcIdx == 7) {
        delta = 6;  // SH to SH
      }
    }
    // Compensate for SE and SG on X9/Horus board if converting from X7
    else if ((IS_TARANIS_X9(cstate.toType) || IS_HORUS(cstate.toType)) && IS_TARANIS_X7(cstate.fromType)) {
      if (srcIdx == 4) {
        delta = -3;  // SF to SF
      }
      else if (srcIdx == 5) {
        delta = -6;  // SH to SH
      }
    }

    if (index < 0) {
      delta = -delta;  // invert for !switch
    }

    index -= delta;

  }  // SWITCH_TYPE_SWITCH

  // final validation (we do not pass model to isAvailable() because we don't know what has or hasn't been converted)
  if (!isAvailable(NULL, cstate.toGS(), cstate.toType)) {
    cstate.setInvalid(oldData);
    type = MAX_SWITCH_TYPE;  // TODO: better way to flag invalid switches?
  }
  else if (evt == RadioDataConversionState::EVT_CVRT) {
    cstate.setConverted(oldData, RadioDataConversionState::LogField(index, toString(cstate.toType, cstate.toGS(), cstate.toModel())));
  }
  else if (oldData.id != index) {
    // provide info by default if anything changed
    cstate.setMoved(oldData, RadioDataConversionState::LogField(index, toString(cstate.toType, cstate.toGS(), cstate.toModel())));
  }

  return *this;
}

void ExpoData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent(QObject::tr("INP"), 3);
  cstate.setSubComp(RawSource(SOURCE_TYPE_VIRTUAL_INPUT, chn).toString(cstate.fromModel(), cstate.fromGS(), cstate.fromType) % QObject::tr(" (@%1)").arg(cstate.subCompIdx));
  srcRaw.convert(cstate);
  swtch.convert(cstate);
}

void MixData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent(QObject::tr("MIX"), 4);
  cstate.setSubComp(RawSource(SOURCE_TYPE_CH, destCh-1).toString(cstate.fromModel(), cstate.fromGS(), cstate.fromType) % QObject::tr(" (@%1)").arg(cstate.subCompIdx));
  srcRaw.convert(cstate);
  swtch.convert(cstate);
}

void LogicalSwitchData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent("LSW", 7);
  cstate.setSubComp(RawSwitch(SWITCH_TYPE_VIRTUAL, cstate.subCompIdx + 1).toString(cstate.fromType, cstate.fromGS(), cstate.fromModel()));
  CSFunctionFamily family = getFunctionFamily();
  switch(family) {
    case LS_FAMILY_VOFS:
      val1 = RawSource(val1).convert(cstate.withComponentField("V1")).toValue();
      break;
    case LS_FAMILY_STICKY:
    case LS_FAMILY_VBOOL:
      val1 = RawSwitch(val1).convert(cstate.withComponentField("V1")).toValue();
      val2 = RawSwitch(val2).convert(cstate.withComponentField("V2")).toValue();
      break;
    case LS_FAMILY_EDGE:
      val1 = RawSwitch(val1).convert(cstate.withComponentField("V1")).toValue();
      break;
    case LS_FAMILY_VCOMP:
      val1 = RawSource(val1).convert(cstate.withComponentField("V1")).toValue();
      val2 = RawSource(val2).convert(cstate.withComponentField("V2")).toValue();
      break;
    default:
      break;
  }

  andsw = RawSwitch(andsw).convert(cstate.withComponentField("AND")).toValue();
}

void CustomFunctionData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent("CFN", 8);
  cstate.setSubComp(toString(cstate.subCompIdx, (cstate.toModel() ? false : true)));
  swtch.convert(cstate);
  if (func == FuncVolume || func == FuncPlayValue || (func >= FuncAdjustGV1 && func <= FuncAdjustGVLast && adjustMode == 1)) {
    param = RawSource(param).convert(cstate.withComponentField("PARAM")).toValue();
  }
}

void FlightModeData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent("FMD", 2);
  cstate.setSubComp(toString(cstate.subCompIdx));
  swtch.convert(cstate);
}

void TimerData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent("TMR", 1);
  cstate.setSubComp(QObject::tr("Timer %1").arg(cstate.subCompIdx + 1));
  mode.convert(cstate);
}

void ModelData::convert(RadioDataConversionState & cstate)
{
  // Here we can add explicit conversions when moving from one board to another

  QString origin = QString(name);
  if (origin.isEmpty())
    origin = QString::number(cstate.modelIdx+1);
  cstate.setOrigin(QObject::tr("Model: ") % origin);

  cstate.setComponent("SET", 0);
  if (thrTraceSrc && (int)thrTraceSrc < cstate.fromBoard.getCapability(Board::Pots) + cstate.fromBoard.getCapability(Board::Sliders)) {
    cstate.setSubComp(QObject::tr("Throttle Source"));
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
}

void GeneralSettings::convert(RadioDataConversionState & cstate)
{
  // Here we can add explicit conversions when moving from one board to another

  cstate.setOrigin(QObject::tr("Radio Settings"));

  setDefaultControlTypes(cstate.toType);  // start with default switches/pots/sliders

  // Try to intelligently copy any custom control names

  // SE and SG are skipped on X7 board
  if (IS_TARANIS_X7(cstate.toType)) {
    if (IS_TARANIS_X9(cstate.fromType) || IS_HORUS(cstate.fromType)) {
      strncpy(switchName[4], switchName[5], sizeof(switchName[0]));
      strncpy(switchName[5], switchName[7], sizeof(switchName[0]));
    }
  }
  else if (IS_TARANIS_X7(cstate.fromType)) {
    if (IS_TARANIS_X9(cstate.toType) || IS_HORUS(cstate.toType)) {
      strncpy(switchName[5], switchName[4], sizeof(switchName[0]));
      strncpy(switchName[7], switchName[5], sizeof(switchName[0]));
    }
  }

  // LS and RS sliders are after 2 aux sliders on X12 and X9E
  if ((IS_HORUS_X12S(cstate.toType) || IS_TARANIS_X9E(cstate.toType)) && !IS_HORUS_X12S(cstate.fromType) && !IS_TARANIS_X9E(cstate.fromType)) {
    strncpy(sliderName[0], sliderName[2], sizeof(sliderName[0]));
    strncpy(sliderName[1], sliderName[3], sizeof(sliderName[0]));
  }
  else if (!IS_TARANIS_X9E(cstate.toType) && !IS_HORUS_X12S(cstate.toType) && (IS_HORUS_X12S(cstate.fromType) || IS_TARANIS_X9E(cstate.fromType))) {
    strncpy(sliderName[2], sliderName[0], sizeof(sliderName[0]));
    strncpy(sliderName[3], sliderName[1], sizeof(sliderName[0]));
  }

  if (IS_HORUS(cstate.toType)) {
    // 6P switch is only on Horus (by default)
    if (cstate.fromBoard.getCapability(Board::FactoryInstalledPots) == 2) {
      strncpy(potName[2], potName[1], sizeof(potName[0]));
      potName[1][0] = '\0';
    }
  }

  if (IS_TARANIS(cstate.toType)) {
    // No S3 pot on Taranis boards by default
    if (cstate.fromBoard.getCapability(Board::FactoryInstalledPots) > 2)
      strncpy(potName[1], potName[2], sizeof(potName[0]));

    contrast = qBound<int>(getCurrentFirmware()->getCapability(MinContrast), contrast, getCurrentFirmware()->getCapability(MaxContrast));
  }

  // TODO: Would be nice at this point to have GUI pause and ask the user to set up any custom hardware they have on the destination radio.

  // Convert all global functions (do this after HW adjustments)
  for (int i=0; i<CPN_MAX_SPECIAL_FUNCTIONS; i++) {
    customFn[i].convert(cstate.withComponentIndex(i));
  }

}


/*
 * RadioData
*/

RadioData::RadioData()
{
  models.resize(getCurrentFirmware()->getCapability(Models));
}

void RadioData::setCurrentModel(unsigned int index)
{
  generalSettings.currModelIndex = index;
  if (index < models.size()) {
    strcpy(generalSettings.currModelFilename, models[index].filename);
  }
}

void RadioData::fixModelFilename(unsigned int index)
{
  ModelData & model = models[index];
  QString filename(model.filename);
  bool ok = filename.endsWith(".bin");
  if (ok) {
    if (filename.startsWith("model") && filename.mid(5, filename.length()-9).toInt() > 0) {
      ok = false;
    }
  }
  if (ok) {
    for (unsigned i=0; i<index; i++) {
      if (strcmp(models[i].filename, model.filename) == 0) {
        ok = false;
        break;
      }
    }
  }
  if (!ok) {
    sprintf(model.filename, "model%d.bin", index+1);
  }
}

void RadioData::fixModelFilenames()
{
  for (unsigned int i=0; i<models.size(); i++) {
    fixModelFilename(i);
  }
  setCurrentModel(generalSettings.currModelIndex);
}

QString RadioData::getNextModelFilename()
{
  char filename[sizeof(ModelData::filename)];
  int index = 0;
  bool found = true;
  while (found) {
    sprintf(filename, "model%d.bin", ++index);
    found = false;
    for (unsigned int i=0; i<models.size(); i++) {
      if (strcmp(filename, models[i].filename) == 0) {
        found = true;
        break;
      }
    }
  }
  return filename;
}

void RadioData::convert(RadioDataConversionState & cstate)
{
  generalSettings.convert(cstate.withModelIndex(-1));

  for (unsigned i=0; i<models.size(); i++) {
    models[i].convert(cstate.withModelIndex(i));
  }

  if (categories.size() == 0) {
    categories.push_back(CategoryData(qPrintable(QObject::tr("Models"))));
    for (unsigned i=0; i<models.size(); i++) {
      models[i].category = 0;
    }
  }

  if (IS_HORUS(cstate.toType)) {
    fixModelFilenames();
  }

  // ensure proper number of model slots
  if (getCurrentFirmware()->getCapability(Models) && getCurrentFirmware()->getCapability(Models) != (int)models.size()) {
    models.resize(getCurrentFirmware()->getCapability(Models));
  }
}
