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
  NO_INPUT,
  RUDDER_INPUT,
  ELEVATOR_INPUT,
  THROTTLE_INPUT,
  AILERONS_INPUT,
  FLAPS_INPUT,
  AIRBRAKES_INPUT
};

enum Vehicle {
  NOVEHICLE,
  PLANE,
  MULTICOPTER,
  HELICOPTER
};

enum WizardPage {
  Page_None = -1,
  Page_Models,
  Page_Throttle,
  Page_Wingtypes,
  Page_Ailerons,
  Page_Flaps,
  Page_Airbrakes,
  Page_Bank,
  Page_Rudder,
  Page_Tails,
  Page_Tail,
  Page_Vtail,
  Page_Simpletail,
  Page_Cyclic,
  Page_Gyro,
  Page_Flybar,
  Page_Fblheli,
  Page_Helictrl,
  Page_Multirotor,
  Page_Conclusion
};

class Channel
{
  public:
    WizardPage page;     // Originating dialog, only of interest for producer
    bool prebooked;     // Temporary lock variable 
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
    const GeneralSettings & settings;
    Vehicle vehicle;
    Channel channel[WIZ_MAX_CHANNELS];

    WizMix(const GeneralSettings & settings, const unsigned int modelId);
    operator ModelData();

  private:
    WizMix();
    void addMix(ModelData & model, Input input, int weight, int channel, int & mixerIndex);
    void maxMixSwitch(MixData &mix, int destCh, int sw, int weight);

};

#endif // WIZARDDATA_H
