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

#ifndef _OPENTX_SIMULATOR_H_
#define _OPENTX_SIMULATOR_H_

#include "simulatorinterface.h"

#if defined __GNUC__
  #define DLLEXPORT
#else
  #define DLLEXPORT __declspec(dllexport)
#endif

class DLLEXPORT OpenTxSimulator : public SimulatorInterface {

  private:
    int volumeGain;
    QString simuSdDirectory;
    QString simuSettingsDirectory;

  public:

    OpenTxSimulator();

    virtual void setSdPath(const QString & sdPath = "", const QString & settingsPath = "");

    virtual void setVolumeGain(int value);

    virtual void start(QByteArray & eeprom, bool tests=true);

    virtual void start(const char * filename, bool tests=true);

    virtual void stop();

    virtual void readEepromData(QByteArray & dest);

    virtual bool timer10ms();

    virtual uint8_t * getLcd();

    virtual bool lcdChanged(bool & lightEnable);

    virtual void setValues(TxInputs &inputs);

    virtual void getValues(TxOutputs &outputs);

    virtual void setTrim(unsigned int idx, int value);

    virtual void getTrims(Trims & trims);

    virtual unsigned int getPhase();

    virtual const char * getPhaseName(unsigned int phase);

    virtual void wheelEvent(int steps);

    virtual const char * getError();

    virtual void sendTelemetry(uint8_t * data, unsigned int len);

    virtual uint8_t getSensorInstance(uint16_t id, uint8_t defaultValue = 0);

    virtual uint16_t getSensorRatio(uint16_t id);

    virtual void setTrainerInput(unsigned int inputNumber, int16_t value);

    virtual void installTraceHook(void (*callback)(const char *));

    virtual void setLuaStateReloadPermanentScripts();
};

#endif // _OPENTX_SIMULATOR_H_
