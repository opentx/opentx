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

#include "customfunctiondata.h"

#include "eeprominterface.h"
#include "radiodata.h"
#include "radiodataconversionstate.h"

void CustomFunctionData::clear()
{
  memset(this, 0, sizeof(CustomFunctionData));
  if (!getCurrentFirmware()->getCapability(SafetyChannelCustomFunction)) {
    func = FuncTrainer;
  }
}

bool CustomFunctionData::isEmpty() const
{
  return (swtch.type == SWITCH_TYPE_NONE);
}

QString CustomFunctionData::nameToString(int index, bool globalContext) const
{
  return RadioData::getElementName((globalContext ? tr("GF") : tr("SF")), index+1, 0, true);
}

QString CustomFunctionData::funcToString(const ModelData * model) const
{
  if (func >= FuncOverrideCH1 && func <= FuncOverrideCH32)
    return tr("Override %1").arg(RawSource(SOURCE_TYPE_CH, func).toString(model));
  else if (func == FuncTrainer)
    return tr("Trainer");
  else if (func == FuncTrainerRUD)
    return tr("Trainer RUD");
  else if (func == FuncTrainerELE)
    return tr("Trainer ELE");
  else if (func == FuncTrainerTHR)
    return tr("Trainer THR");
  else if (func == FuncTrainerAIL)
    return tr("Trainer AIL");
  else if (func == FuncInstantTrim)
    return tr("Instant Trim");
  else if (func == FuncPlaySound)
    return tr("Play Sound");
  else if (func == FuncPlayHaptic)
    return tr("Haptic");
  else if (func == FuncReset)
    return tr("Reset");
  else if (func >= FuncSetTimer1 && func <= FuncSetTimer3)
    return tr("Set Timer %1").arg(func-FuncSetTimer1+1);
  else if (func == FuncVario)
    return tr("Vario");
  else if (func == FuncPlayPrompt)
    return tr("Play Track");
  else if (func == FuncPlayBoth)
    return tr("Play Both");
  else if (func == FuncPlayValue)
    return tr("Play Value");
  else if (func == FuncPlayScript)
    return tr("Play Script");
  else if (func == FuncLogs)
    return tr("SD Logs");
  else if (func == FuncVolume)
    return tr("Volume");
  else if (func == FuncBacklight)
    return tr("Backlight");
  else if (func == FuncScreenshot)
    return tr("Screenshot");
  else if (func == FuncBackgroundMusic)
    return tr("Background Music");
  else if (func == FuncBackgroundMusicPause)
    return tr("Background Music Pause");
  else if (func >= FuncAdjustGV1 && func <= FuncAdjustGVLast)
    return tr("Adjust %1").arg(RawSource(SOURCE_TYPE_GVAR, func-FuncAdjustGV1).toString(model));
  else if (func == FuncSetFailsafeInternalModule)
    return tr("SetFailsafe Int. Module");
  else if (func == FuncSetFailsafeExternalModule)
    return tr("SetFailsafe Ext. Module");
  else if (func == FuncRangeCheckInternalModule)
    return tr("RangeCheck Int. Module");
  else if (func == FuncRangeCheckExternalModule)
    return tr("RangeCheck Ext. Module");
  else if (func == FuncBindInternalModule)
    return tr("Bind Int. Module");
  else if (func == FuncBindExternalModule)
    return tr("Bind Ext. Module");
  else {
    return QString("???"); // Highlight unknown functions with output of question marks.(BTW should not happen that we do not know what a function is)
  }
}

void CustomFunctionData::populateResetParams(const ModelData * model, QComboBox * b, unsigned int value = 0)
{
  int val = 0;
  Firmware * firmware = Firmware::getCurrentVariant();
  Board::Type board = firmware->getBoard();

  b->addItem(tr("Timer1"), val++);
  b->addItem(tr("Timer2"), val++);
  if (IS_ARM(board)) {
    b->addItem( tr("Timer3"), val++);
  }
  b->addItem(tr("Flight"), val++);
  b->addItem(tr("Telemetry"), val++);
  int reCount = firmware->getCapability(RotaryEncoders);
  if (reCount == 1) {
    b->addItem(tr("Rotary Encoder"), val++);
  }
  else if (reCount == 2) {
    b->addItem(tr("REa"), val++);
    b->addItem(tr("REb"), val++);
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
    return QString("%1").arg(param/10.0) + tr("s");
  }
  else if (func==FuncPlaySound) {
    CustomFunctionData::populatePlaySoundParams(qs);
    if (param>=0 && param<(int)qs.count())
      return qs.at(param);
    else
      return tr("<font color=red><b>Inconsistent parameter</b></font>");
  }
  else if (func==FuncPlayHaptic) {
    CustomFunctionData::populateHapticParams(qs);
    if (param>=0 && param<(int)qs.count())
      return qs.at(param);
    else
      return tr("<font color=red><b>Inconsistent parameter</b></font>");
  }
  else if (func==FuncReset) {
    QComboBox cb;
    CustomFunctionData::populateResetParams(model, &cb);
    int pos = cb.findData(param);
    if (pos >= 0)
      return cb.itemText(pos);
    else
      return tr("<font color=red><b>Inconsistent parameter</b></font>");
  }
  else if ((func==FuncVolume)|| (func==FuncPlayValue)) {
    RawSource item(param);
    return item.toString(model);
  }
  else if ((func==FuncPlayPrompt) || (func==FuncPlayBoth)) {
    if ( getCurrentFirmware()->getCapability(VoicesAsNumbers)) {
      return QString("%1").arg(param);
    }
    else {
      return paramarm;
    }
  }
  else if ((func>=FuncAdjustGV1) && (func<FuncCount)) {
    switch (adjustMode) {
      case FUNC_ADJUST_GVAR_CONSTANT:
        return tr("Value ")+QString("%1").arg(param);
      case FUNC_ADJUST_GVAR_SOURCE:
      case FUNC_ADJUST_GVAR_GVAR:
        return RawSource(param).toString();
      case FUNC_ADJUST_GVAR_INCDEC:
        if (param==0) return tr("Decr:") + " -1";
        else          return tr("Incr:") + " +1";
    }
  }
  return "";
}

QString CustomFunctionData::repeatToString() const
{
  if (repeatParam == -1) {
    return tr("played once, not during startup");
  }
  else if (repeatParam == 0) {
    return "";
  }
  else {
    unsigned int step = IS_ARM(getCurrentBoard()) ? 1 : 10;
    return tr("repeat(%1s)").arg(step*repeatParam);
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
      return tr("DISABLED");
    }
  }
  return "";
}

void CustomFunctionData::convert(RadioDataConversionState & cstate)
{
  cstate.setComponent(tr("CFN"), 8);
  cstate.setSubComp(nameToString(cstate.subCompIdx, (cstate.toModel() ? false : true)));
  swtch.convert(cstate);
  if (func == FuncVolume || func == FuncPlayValue || (func >= FuncAdjustGV1 && func <= FuncAdjustGVLast && adjustMode == 1)) {
    param = RawSource(param).convert(cstate.withComponentField("PARAM")).toValue();
  }
}
