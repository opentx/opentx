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

#include <string.h>
#include "eeprominterface.h"
#include "wizarddata.h"

Channel::Channel()
{
  clear();
}

void Channel::clear()
{
  page = Page_None;
  prebooked = false;
  input1 = NO_INPUT;
  input2 = NO_INPUT;
  weight1 = 0;
  weight2 = 0;
}

WizMix::WizMix(const GeneralSettings & settings, unsigned int modelId, const ModelData & modelData):
  complete(false),
  modelId(modelId),
  settings(settings),
  originalModelData(modelData),
  vehicle(NOVEHICLE)
{
  strncpy(name, originalModelData.name, WIZ_MODEL_NAME_LENGTH);
}


void WizMix::maxMixSwitch(char *name, MixData &mix, int channel, int sw, int weight)
{
  strncpy(mix.name, name, MIXDATA_NAME_LEN);
  mix.name[MIXDATA_NAME_LEN] = '\0';
  mix.destCh = channel;
  mix.srcRaw = RawSource(SOURCE_TYPE_MAX);
  mix.swtch  = RawSwitch(SWITCH_TYPE_SWITCH, sw);
  mix.weight = weight;
}

void WizMix::addMix(ModelData &model, Input input, int weight, int channel, int & mixIndex)
{
  if (input != NO_INPUT)  {
    bool isHorusOrTaranis = IS_ARM(getCurrentBoard());
    if (input >= RUDDER_INPUT && input <= AILERONS_INPUT) {
      MixData & mix = model.mixData[mixIndex++];
      mix.destCh = channel+1;
      if (isHorusOrTaranis){
        int channel = settings.getDefaultChannel(input-1);
        mix.srcRaw = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, channel);
      }
      else
        mix.srcRaw = RawSource(SOURCE_TYPE_STICK, input-1);
      mix.weight = weight;
    }
    else if (input==FLAPS_INPUT){
      // There ought to be some kind of constants for switches somewhere...
      maxMixSwitch((char *)"Flaps Up",   model.mixData[mixIndex++], channel+1, isHorusOrTaranis ? SWITCH_SA0 :-SWITCH_ELE ,  weight); //Taranis-Horus SA-UP, 9X ELE-UP
      maxMixSwitch((char *)"Flaps Dn", model.mixData[mixIndex++], channel+1, isHorusOrTaranis ? SWITCH_SA2 : SWITCH_ELE , -weight); //Taranis-Horus SA-DOWN, 9X ELE-DOWN

    }
    else if (input==AIRBRAKES_INPUT){
      maxMixSwitch((char *)"AirbkOff", model.mixData[mixIndex++], channel+1, isHorusOrTaranis ? SWITCH_SE0 :-SWITCH_RUD , -weight); //Taranis-Horus SE-UP, 9X RUD-UP
      maxMixSwitch((char *)"AirbkOn",  model.mixData[mixIndex++], channel+1, isHorusOrTaranis ? SWITCH_SE2 : SWITCH_RUD , weight); //Tatanis-Horus SE-DOWN, 9X RUD-DOWN
    }
  }
}

WizMix::operator ModelData()
{
  int throttleChannel = -1;

  ModelData model;
  //ModelData model(originalModelData);
  model.category = originalModelData.category;
  model.used = true;
  model.moduleData[0].modelId = modelId;
  model.setDefaultInputs(settings);

  int mixIndex = 0;
  int timerIndex = 0;

  // Safe copy model name
  strncpy(model.name, name, WIZ_MODEL_NAME_LENGTH);
  model.name[WIZ_MODEL_NAME_LENGTH] = 0;

  // Add the channel mixes
  for (int i=0; i<WIZ_MAX_CHANNELS; i++ )
  {
    Channel ch = channel[i];

    addMix(model, ch.input1, ch.weight1, i, mixIndex);
    addMix(model, ch.input2, ch.weight2, i, mixIndex);

    if ((ch.input1 == THROTTLE_INPUT || ch.input2 == THROTTLE_INPUT) &&  options[THROTTLE_CUT_OPTION]) {
      // Add the Throttle Cut option
      MixData & mix = model.mixData[mixIndex++];
      mix.destCh = i+1;
      mix.srcRaw = SOURCE_TYPE_MAX;
      mix.weight = -100;
      mix.swtch.type = SWITCH_TYPE_SWITCH;
      mix.swtch.index = IS_ARM(getCurrentBoard()) ? SWITCH_SF0 : SWITCH_THR;
      mix.mltpx = MLTPX_REP;
      strncpy(mix.name, "Cut", MIXDATA_NAME_LEN);
      mix.name[MIXDATA_NAME_LEN] = '\0';
    }
  }

  // Add the Flight Timer option
  if (options[FLIGHT_TIMER_OPTION] ){
    model.timers[timerIndex].mode.type = SWITCH_TYPE_TIMER_MODE;
    model.timers[timerIndex].mode.index = TMRMODE_THR_TRG;
    timerIndex++;
  }

  // Add the Throttle Timer option
  if (options[THROTTLE_TIMER_OPTION] && throttleChannel >=0){
    model.timers[timerIndex].mode.type = SWITCH_TYPE_TIMER_MODE;
    model.timers[timerIndex].mode.index = TMRMODE_THR;
    timerIndex++;
  }

  return model;
}
