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

#ifndef simulator_interface_h
#define simulator_interface_h

#include "eeprominterface.h"

struct TxInputs {
    int  sticks[NUM_STICKS]; /* lh lv rv rh */
    int  pots[C9X_NUM_POTS];
    int  switches[C9X_NUM_SWITCHES];
    bool keys[C9X_NUM_KEYS];
    bool rotenc;
    bool trims[8];
};

class TxOutputs {
  public:
    TxOutputs() {
      memset(this, 0, sizeof(TxOutputs));
    }

    int16_t chans[C9X_NUM_CHNOUT];
    bool vsw[C9X_NUM_CSW];
    int16_t gvars[C9X_MAX_FLIGHT_MODES][C9X_MAX_GVARS];
    unsigned int beep;
    // uint8_t phase;
};

struct Trims {
    int16_t values[NUM_STICKS]; /* lh lv rv rh */
    bool extended;
};

class SimulatorInterface {

  public:

    virtual ~SimulatorInterface() {};

    virtual void start(QByteArray & eeprom, bool tests=true) = 0;

    virtual void start(const char * filename, bool tests=true) = 0;

    virtual void stop() = 0;

    virtual bool timer10ms() = 0;

    virtual uint8_t * getLcd() = 0;

    virtual bool lcdChanged(bool & lightEnable) = 0;

    virtual void setValues(TxInputs &inputs) = 0;

    virtual void getValues(TxOutputs &outputs) = 0;

    virtual void setTrim(unsigned int idx, int value) = 0;

    virtual void getTrims(Trims & trims) = 0;

    virtual unsigned int getPhase() = 0;
    
    virtual const char * getPhaseName(unsigned int phase) = 0;

    virtual void wheelEvent(uint8_t steps) = 0;

    virtual const char * getError() = 0;

    virtual void sendTelemetry(uint8_t * data, unsigned int len) = 0;
};

#endif
