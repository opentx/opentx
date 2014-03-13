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
  sourceDlg = -1;
  input1 = NOINPUT;
  input2 = NOINPUT;
  weight1 = 0;  
  weight2 = 0;
}

WizMix::WizMix(const unsigned int modelId):
  complete(false),
  modelId(modelId),
  vehicle(NOVEHICLE)
{
  strcpy(name, "            ");
}

WizMix::operator ModelData()
{
  ModelData model;

  model.used = true;
  model.modelId = modelId;

  // Safe copy model name
  strncpy(model.name, name, WIZ_MODEL_NAME_LENGTH);
  model.name[WIZ_MODEL_NAME_LENGTH] = 0;

  for (int i=0; i<WIZ_MAX_CHANNELS; i++ ) {
    Channel ch = channel[i];
    if (ch.sourceDlg > 0) {
      //**** INSERT MIXER HERE ****
    }
  }

  return model;
}


