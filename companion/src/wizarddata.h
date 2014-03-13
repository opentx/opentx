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

#ifndef WIZARDDATA_H
#define WIZARDDATA_H

#include "eeprominterface.h"

#define WIZ_MAX_CHANNELS 8

// TODO use a constant common to the whole companion
// TODO when in the wizard use the getCapacity(...) to know how long the name can be
#define WIZ_MODEL_NAME_LENGTH 12

enum Input {
  NOINPUT,
  THROTTLE,
  RUDDER,
  ELEVATOR,
  AILERON,
  FLAP,
  AIRBREAK
};

enum Vehicle {
  NOVEHICLE,
  PLANE,
  MULTICOPTER,
  HELICOPTER
};

class Channel
{
  public:
    int sourceDlg;     // Originating dialog, only of interest for producer
    Input input1;
    Input input2;
    int weight1;
    int weight2;

    Channel();
    void clear();
};

class WizMix
{
  public:
    bool complete;
    char name[WIZ_MODEL_NAME_LENGTH + 1];
    unsigned int modelId;
    Vehicle vehicle;
    Channel channel[WIZ_MAX_CHANNELS];

    explicit WizMix(const unsigned int modelId);
    operator ModelData();

  private:
    WizMix();
};

#endif // WIZARDDATA_H
