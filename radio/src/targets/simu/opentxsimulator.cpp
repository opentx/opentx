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

#include "opentxsimulator.h"
#include "opentx.h"

int16_t g_anas[NUM_STICKS+NUM_POTS];

uint16_t anaIn(uint8_t chan)
{
  return g_anas[chan];
}

uint16_t getAnalogValue(uint8_t index)
{
  return anaIn(index);
}

bool hasExtendedTrims()
{
  return g_model.extendedTrims;
}

uint8_t getStickMode()
{
  return limit<uint8_t>(0, g_eeGeneral.stickMode, 3);
}

#if defined(PCBTARANIS)
void resetTrims()
{
  TRIMS_GPIO_REG_RVD |= TRIMS_GPIO_PIN_RVD;
  TRIMS_GPIO_REG_RVU |= TRIMS_GPIO_PIN_RVU;
  TRIMS_GPIO_REG_RHL |= TRIMS_GPIO_PIN_RHL;
  TRIMS_GPIO_REG_RHR |= TRIMS_GPIO_PIN_RHR;
  TRIMS_GPIO_REG_LVD |= TRIMS_GPIO_PIN_LVD;
  TRIMS_GPIO_REG_LVU |= TRIMS_GPIO_PIN_LVU;
  TRIMS_GPIO_REG_LHL |= TRIMS_GPIO_PIN_LHL;
  TRIMS_GPIO_REG_LHR |= TRIMS_GPIO_PIN_LHR;
}
#endif

OpenTxSimulator::OpenTxSimulator()
{
}

void OpenTxSimulator::setSdPath(const QString &sdPath)
{
#if defined(SDCARD)
  strncpy(simuSdDirectory, sdPath.toLatin1().constData(), sizeof(simuSdDirectory)-1);
  simuSdDirectory[sizeof(simuSdDirectory)-1] = '\0';
#endif
}

void OpenTxSimulator::setVolumeGain(int value)
{
  volumeGain = value;
}

bool OpenTxSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * OpenTxSimulator::getLcd()
{
  return (uint8_t *)simuLcdBuf;
}

bool OpenTxSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void OpenTxSimulator::start(QByteArray & ee, bool tests)
{
  memcpy(eeprom, ee.data(), std::min<int>(EESIZE_SIMU, ee.size()));
  start((const char *)0, tests);
}

void OpenTxSimulator::start(const char * filename, bool tests)
{
#if defined(PCBSKY9X) && !defined(REVX) && !defined(AR9X)
  g_rotenc[0] = 0;
#elif defined(PCBGRUVIN9X)
  g_rotenc[0] = 0;
  g_rotenc[1] = 0;
#endif

  StartEepromThread(filename);
  StartAudioThread(volumeGain);
  StartSimu(tests);
}

void OpenTxSimulator::stop()
{
  StopSimu();
#if defined(CPUARM)
  StopAudioThread();
#endif
  StopEepromThread();
}

void OpenTxSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#define g_chans512 channelOutputs
#include "simulatorimport.h"
}

void OpenTxSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void OpenTxSimulator::setTrim(unsigned int idx, int value)
{
  idx = modn12x3[4*getStickMode() + idx];
  uint8_t phase = getTrimFlightPhase(getFlightMode(), idx);
  setTrimValue(phase, idx, value);
}

void OpenTxSimulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightMode();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(phase, idx), idx);
  }

  for (int i=0; i<2; i++) {
    uint8_t idx = modn12x3[4*getStickMode() + i];
    int16_t tmp = trims.values[i];
    trims.values[i] = trims.values[idx];
    trims.values[idx] = tmp;
  }
}

void OpenTxSimulator::wheelEvent(int steps)
{
#if defined(PCBHORUS) || defined(PCBFLAMENCO)
  if (steps > 0)
    rotencValue -= 2;
  else
    rotencValue += 2;
#elif defined(REV9E)
  if (steps == 255)
    rotencValue -= 2;
  else
    rotencValue += 2;
#elif defined(PCBSKY9X) && !defined(REVX) && !defined(AR9X)
  g_rotenc[0] += steps*4;
#elif defined(PCBGRUVIN9X)
  g_rotenc[0] += steps;
#endif
}

unsigned int OpenTxSimulator::getPhase()
{
  return getFlightMode();
}

const char * OpenTxSimulator::getPhaseName(unsigned int phase)
{
  static char buff[sizeof(g_model.flightModeData[0].name)+1];
  zchar2str(buff, g_model.flightModeData[phase].name, sizeof(g_model.flightModeData[0].name));
  return buff;
}

const char * OpenTxSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}

void OpenTxSimulator::sendTelemetry(uint8_t * data, unsigned int len)
{
#if defined(FRSKY_SPORT)
  processSportPacket(data);
#endif
}

uint8_t OpenTxSimulator::getSensorInstance(uint16_t id, uint8_t defaultValue)
{
#if defined(FRSKY_SPORT)
  for (int i = 0; i<MAX_SENSORS; i++) {
    if (isTelemetryFieldAvailable(i)) {
      TelemetrySensor * sensor = &g_model.telemetrySensors[i];
      if (sensor->id == id) {
        return sensor->instance;
      }
    }
  }
#endif
  return defaultValue;
}

uint16_t OpenTxSimulator::getSensorRatio(uint16_t id)
{
#if defined(FRSKY_SPORT)
  for (int i = 0; i<MAX_SENSORS; i++) {
    if (isTelemetryFieldAvailable(i)) {
      TelemetrySensor * sensor = &g_model.telemetrySensors[i];
      if (sensor->id == id) {
        return sensor->custom.ratio;
      }
    }
  }
#endif
  return 0;
}

void OpenTxSimulator::setTrainerInput(unsigned int inputNumber, int16_t value)
{
#define SETTRAINER_IMPORT
#include "simulatorimport.h"
}

void OpenTxSimulator::setLuaStateReloadPermanentScripts()
{
#if defined(LUA)
    luaState = INTERPRETER_RELOAD_PERMANENT_SCRIPTS;
#endif
}

void OpenTxSimulator::installTraceHook(void (*callback)(const char *))
{
  traceCallback = callback;
}

class OpenTxSimulatorFactory: public SimulatorFactory
{
  public:
    OpenTxSimulatorFactory()
    {
    }

    virtual SimulatorInterface *create()
    {
      return new OpenTxSimulator();
    }

    virtual QString name()
    {
      return QString(SIMULATOR_FLAVOUR);
    }

    virtual BoardEnum type()
    {
#if defined(PCBHORUS)
      return BOARD_HORUS;
#elif defined(PCBFLAMENCO)
      return BOARD_FLAMENCO;
#elif defined(PCBTARANIS)
      return BOARD_TARANIS;
#else
      return BOARD_STOCK;
#endif
    }
};

extern "C" DLLEXPORT SimulatorFactory * registerSimu()
{
  return new OpenTxSimulatorFactory();
}
