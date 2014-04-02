/*
 * Author - Kjell Kernen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <string.h>
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


void WizMix::maxMixSwitch(MixData &mix, int channel, int sw, int weight)
{
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
      if (isTaranis)
        mix.srcRaw = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, input-1, &model);
      else
        mix.srcRaw = RawSource(SOURCE_TYPE_STICK, input-1);
      mix.weight = weight;
    }
    else if (input==FLAPS_INPUT){
      // There must be some kind of constants for switches somewhere...
      maxMixSwitch( model.mixData[mixIndex++], channel+1, isTaranis ? 1 :-3 , -100); //SA-UP or ELE-UP
      maxMixSwitch( model.mixData[mixIndex++], channel+1, isTaranis ? 3 : 3 ,  100); //SA-DOWN or ELE-DOWN
    }
    else if (input==AIRBRAKES_INPUT){ 
      maxMixSwitch( model.mixData[mixIndex++], channel+1, isTaranis ? 13 :-2 , -100); //SE-UP or RUD-UP
      maxMixSwitch( model.mixData[mixIndex++], channel+1, isTaranis ? 15 : 2 ,  100); //SE-DOWN or RUD-DOWN
    }
  }
}

WizMix::operator ModelData()
{
  ModelData model;
  model.used = true;
  model.modelId = modelId;
  model.setDefaultInputs(settings);

  // Safe copy model name
  strncpy(model.name, name, WIZ_MODEL_NAME_LENGTH);
  model.name[WIZ_MODEL_NAME_LENGTH] = 0;

  int mixIndex = 0;
  for (int i=0; i<WIZ_MAX_CHANNELS; i++ ) 
  {
    Channel ch = channel[i];
    addMix(model, ch.input1, ch.weight1, i, mixIndex);
    addMix(model, ch.input2, ch.weight2, i, mixIndex);
  }
  return model;
}


