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

#ifndef CUSTOMFUNCTIONDATA_H
#define CUSTOMFUNCTIONDATA_H

#include "boards.h"
#include "constants.h"
#include "rawswitch.h"

#include <QtCore>
#include <QComboBox>

class Firmware;
class ModelData;
class GeneralSettings;
class RadioDataConversionState;

enum AssignFunc {
  FuncOverrideCH1 = 0,
  FuncOverrideCHLast = FuncOverrideCH1 + CPN_MAX_CHNOUT - 1,
  FuncOverrideCH32 = FuncOverrideCHLast,  //  TODO remove
  FuncTrainer,
  FuncTrainerRUD,
  FuncTrainerELE,
  FuncTrainerTHR,
  FuncTrainerAIL,
  FuncTrainerChannels,
  FuncInstantTrim,
  FuncPlaySound,
  FuncPlayHaptic,
  FuncReset,
  FuncSetTimer1,
  FuncSetTimer2,  //  TODO remove
  FuncSetTimer3,  //  TODO remove
  FuncSetTimerLast = FuncSetTimer1 + CPN_MAX_TIMERS - 1,
  FuncVario,
  FuncPlayPrompt,
  FuncPlayBoth,
  FuncPlayValue,
  FuncPlayScript,
  FuncLogs,
  FuncVolume,
  FuncBacklight,
  FuncScreenshot,
  FuncBackgroundMusic,
  FuncBackgroundMusicPause,
  FuncAdjustGV1,
  FuncAdjustGVLast = FuncAdjustGV1 + CPN_MAX_GVARS - 1,
  FuncSetFailsafe,
  FuncRangeCheckInternalModule,
  FuncRangeCheckExternalModule,
  FuncBindInternalModule,
  FuncBindExternalModule,
  FuncCount,
  FuncReserve = -1
};

enum GVarAdjustModes
{
  FUNC_ADJUST_GVAR_CONSTANT,
  FUNC_ADJUST_GVAR_SOURCE,
  FUNC_ADJUST_GVAR_GVAR,
  FUNC_ADJUST_GVAR_INCDEC
};

class CustomFunctionData {
  Q_DECLARE_TR_FUNCTIONS(CustomFunctionData)

  public:
    enum CustomFunctionContext
    {
      GlobalFunctionsContext  = 0x01,
      SpecialFunctionsContext = 0x02,

      AllFunctionContexts     = GlobalFunctionsContext | SpecialFunctionsContext
    };

    CustomFunctionData(AssignFunc func = FuncOverrideCH1) { clear(); this->func = func; }
    RawSwitch    swtch;
    AssignFunc   func;
    int param;
    char paramarm[10];
    unsigned int enabled; // TODO perhaps not any more the right name
    unsigned int adjustMode;
    int repeatParam;

    void clear();
    bool isEmpty() const;
    QString nameToString(int index, bool globalContext = false) const;
    QString funcToString(const ModelData * model = nullptr) const;
    QString paramToString(const ModelData * model) const;
    QString repeatToString() const;
    QString enabledToString() const;

    static void populateResetParams(const ModelData * model, QComboBox * b, unsigned int value);
    static void populatePlaySoundParams(QStringList & qs);
    static void populateHapticParams(QStringList & qs);
    static bool isFuncAvailable(int index);
    static int funcContext(int index);
    static int resetParamCount(const ModelData * model);
    static bool isResetParamAvailable(const ModelData * model, int index);

    void convert(RadioDataConversionState & cstate);

};


#endif // CUSTOMFUNCTIONDATA_H
