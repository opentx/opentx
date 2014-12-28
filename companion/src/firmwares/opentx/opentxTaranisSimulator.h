/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 * 
 * Based on th9x -> http://code.google.com/p/th9x/
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

#ifndef opentx_taranis_simulator_h
#define opentx_taranis_simulator_h

#include "simulatorinterface.h"

class RadioData;
class OpenTxEepromInterface;

class OpentxTaranisSimulator : public SimulatorInterface {

  public:

    OpentxTaranisSimulator();

    virtual void start(QByteArray & eeprom, bool tests=true);

    virtual void start(const char * filename, bool tests=true);

    virtual void stop();

    virtual bool timer10ms();

    virtual uint8_t * getLcd();

    virtual bool lcdChanged(bool & lightEnable);

    virtual void setValues(TxInputs &inputs);

    virtual void getValues(TxOutputs &outputs);

    virtual void setTrim(unsigned int idx, int value);

    virtual void getTrims(Trims & trims);

    virtual unsigned int getPhase();

    virtual const char * getPhaseName(unsigned int phase);

    virtual void wheelEvent(uint8_t steps);

    virtual const char * getError();

    virtual void sendTelemetry(uint8_t * data, unsigned int len);

    virtual void setTrainerInput(unsigned int inputNumber, int16_t value);
};

#endif
