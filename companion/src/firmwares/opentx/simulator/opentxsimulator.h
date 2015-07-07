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

#if defined(PCBTARANIS) && defined(REV9E)
  #define FLAVOUR "taranisx9e"
  #define NAMESPACE TaranisX9E
  #define EEPROM_VARIANT 0x8000
#elif defined(PCBTARANIS) && defined(REVPLUS)
  #define FLAVOUR "taranisplus"
  #define NAMESPACE TaranisPlus
  #define EEPROM_VARIANT 0
#elif defined(PCBTARANIS)
  #define FLAVOUR "taranis"
  #define NAMESPACE Taranis
  #define EEPROM_VARIANT 0
#elif defined(PCBSKY9X) && defined(REVX)
  #define FLAVOUR "9xrpro"
  #define NAMESPACE Sky9xPro
  #define EEPROM_VARIANT 0
#elif defined(PCBSKY9X)
  #define FLAVOUR "sky9x"
  #define NAMESPACE Sky9X
  #define EEPROM_VARIANT 0
#elif defined(PCBGRUVIN9X)
  #define FLAVOUR "gruvin9x"
  #define NAMESPACE Gruvin9X
  #define EEPROM_VARIANT 0
#elif defined(PCBMEGA2560)
  #define FLAVOUR "mega2560"
  #define NAMESPACE Mega2560
  #define EEPROM_VARIANT 0
#elif defined(PCB9XR) && defined(CPUM128)
  #define FLAVOUR "9xr128"
  #define NAMESPACE M128XR
  #define EEPROM_VARIANT 0x8003
#elif defined(PCB9X) && defined(CPUM128)
  #define FLAVOUR "9x128"
  #define NAMESPACE M128
  #define EEPROM_VARIANT 0x8003
#elif defined(PCB9XR) && defined(CPUM64)
  #define FLAVOUR "9xr"
  #define NAMESPACE M64XR
  #define EEPROM_VARIANT 0x0003
#elif defined(PCB9X) && defined(CPUM64)
  #define FLAVOUR "9x"
  #define NAMESPACE M64
  #define EEPROM_VARIANT 0x0003
#else
#error "Unknown board"
#endif

#if defined __GNUC__
  #define DLLEXPORT
#else
  #define DLLEXPORT __declspec(dllexport)
#endif

namespace NAMESPACE {

class DLLEXPORT OpenTxSimulator : public SimulatorInterface {

  private:
    int volumeGain;

  public:

    OpenTxSimulator();

    virtual void setSdPath(const QString &sdPath);

    virtual void setVolumeGain(int value);

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

    virtual void wheelEvent(int steps);

    virtual const char * getError();

    virtual void sendTelemetry(uint8_t * data, unsigned int len);

    virtual void setTrainerInput(unsigned int inputNumber, int16_t value);

    virtual void installTraceHook(void (*callback)(const char *));
};

}

#endif
