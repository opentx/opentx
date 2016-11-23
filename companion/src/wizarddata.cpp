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

WizMix::WizMix(const GeneralSettings & settings, const unsigned int modelId):
  complete(false),
  modelId(modelId),
  settings(settings),
  vehicle(NOVEHICLE)
{
  strcpy(name, "            ");
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
    bool isTaranis = IS_TARANIS(GetEepromInterface()->getBoard());

    if (input >= RUDDER_INPUT && input <= AILERONS_INPUT) {
      MixData & mix = model.mixData[mixIndex++];
      mix.destCh = channel+1;
      if (isTaranis){
        int channel = settings.getDefaultChannel(input-1);
        mix.srcRaw = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, channel);
      }
      else
        mix.srcRaw = RawSource(SOURCE_TYPE_STICK, input-1);
      mix.weight = weight;
    }
    else if (input==FLAPS_INPUT){
      // There ought to be some kind of constants for switches somewhere...
      maxMixSwitch((char *)"Flaps Up",   model.mixData[mixIndex++], channel+1, isTaranis ? SWITCH_SA0 :-SWITCH_ELE ,  weight); //Taranis SA-UP, 9X ELE-UP
      maxMixSwitch((char *)"Flaps Dn", model.mixData[mixIndex++], channel+1, isTaranis ? SWITCH_SA2 : SWITCH_ELE , -weight); //Taranis SA-DOWN, 9X ELE-DOWN

    }
    else if (input==AIRBRAKES_INPUT){ 
      maxMixSwitch((char *)"AirbkOff", model.mixData[mixIndex++], channel+1, isTaranis ? SWITCH_SE0 :-SWITCH_RUD , -weight); //Taranis SE-UP, 9X RUD-UP
      maxMixSwitch((char *)"AirbkOn",  model.mixData[mixIndex++], channel+1, isTaranis ? SWITCH_SE2 : SWITCH_RUD , weight); //Tatanis SE-DOWN, 9X RUD-DOWN
    }
  }
}

WizMix::operator ModelData()
{
  int throttleChannel = -1;
  bool isTaranis = IS_TARANIS(GetEepromInterface()->getBoard());

  ModelData model;
  model.used = true;
  model.moduleData[0].modelId = modelId;
  model.setDefaultInputs(settings);

  int mixIndex = 0;
  int switchIndex = 0;
  int timerIndex = 0;

  // Safe copy model name
  strncpy(model.name, name, WIZ_MODEL_NAME_LENGTH);
  model.name[WIZ_MODEL_NAME_LENGTH] = 0;

  // Add the channel mixes
  for (int i=0; i<WIZ_MAX_CHANNELS; i++ ) 
  {
    Channel ch = channel[i];
    if (ch.input1 == THROTTLE_INPUT || ch.input2 == THROTTLE_INPUT)
      throttleChannel = i;

    addMix(model, ch.input1, ch.weight1, i, mixIndex);
    addMix(model, ch.input2, ch.weight2, i, mixIndex);
  }

  // Add the Throttle Cut option
  if( options[THROTTLE_CUT_OPTION] && throttleChannel >=0 ){
    model.customFn[switchIndex].swtch.type = SWITCH_TYPE_SWITCH;
    model.customFn[switchIndex].swtch.index = isTaranis ? SWITCH_SF0 : SWITCH_THR;
    model.customFn[switchIndex].enabled = 1;
    model.customFn[switchIndex].func = (AssignFunc)throttleChannel;
    model.customFn[switchIndex].param = -100;
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


