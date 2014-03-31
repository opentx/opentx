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
  input1 = NOINPUT;
  input2 = NOINPUT;
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

void WizMix::addMix(ModelData &model, Input input, int weight, int channel, int & mixerIndex)
{
  if (input != NOINPUT) 
  {
    MixData & mix = model.mixData[mixerIndex++];
    mix.destCh = channel+1;
    if (input >= RUDDER && input <= AILERON) 
    {
      if (IS_TARANIS(GetEepromInterface()->getBoard()))
        mix.srcRaw = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, input-1, &model);
      else
        mix.srcRaw = RawSource(SOURCE_TYPE_STICK, input-1);
    }
    if (input==FLAP){
    }
    if (input==AIRBREAK){
    }
    mix.weight = weight;
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


