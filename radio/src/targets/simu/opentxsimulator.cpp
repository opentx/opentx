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
#include "simulcd.h"

int16_t g_anas[NUM_STICKS+NUM_POTS+NUM_SLIDERS];

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

void OpenTxSimulator::setSdPath(const QString & sdPath, const QString & settingsPath)
{
  simuSdDirectory = sdPath;
  simuSettingsDirectory = settingsPath;
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
#if defined(EEPROM_SIZE)
  memcpy(eeprom, ee.data(), std::min<int>(EEPROM_SIZE, ee.size()));
#endif
  start((const char *)0, tests);
}

void OpenTxSimulator::start(const char * filename, bool tests)
{
#if defined(ROTARY_ENCODER_NAVIGATION)
  for (uint8_t i=0; i<DIM(rotencValue); i++) {
    rotencValue[i] = 0;
  }
#endif

  StartEepromThread(filename);
  StartAudioThread(volumeGain);
  StartSimu(tests, simuSdDirectory.toLatin1().constData(), simuSettingsDirectory.toLatin1().constData());
}

void OpenTxSimulator::stop()
{
  StopSimu();
#if defined(CPUARM)
  StopAudioThread();
#endif
  StopEepromThread();
}

void OpenTxSimulator::readEepromData(QByteArray & dest)
{
#if defined(EEPROM_SIZE)
  memcpy(dest.data(), eeprom, std::min<int>(EEPROM_SIZE, dest.size()));
#endif
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
  uint8_t phase = getTrimFlightMode(getFlightMode(), idx);
  setTrimValue(phase, idx, value);
}

void OpenTxSimulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightMode();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightMode(phase, idx), idx);
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
#if defined(ROTARY_ENCODER_NAVIGATION)
  ROTARY_ENCODER_NAVIGATION_VALUE += steps * ROTARY_ENCODER_GRANULARITY;
#else
  if (steps > 0)
    simuSetKey(KEY_MINUS, 1);
  else if (steps < 0)
    simuSetKey(KEY_PLUS, 1);
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
#if defined(TELEMETRY_FRSKY_SPORT)
  sportProcessTelemetryPacket(data);
#endif
}

uint8_t OpenTxSimulator::getSensorInstance(uint16_t id, uint8_t defaultValue)
{
#if defined(TELEMETRY_FRSKY_SPORT)
  for (int i = 0; i<MAX_TELEMETRY_SENSORS; i++) {
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
#if defined(TELEMETRY_FRSKY_SPORT)
  for (int i = 0; i<MAX_TELEMETRY_SENSORS; i++) {
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

    virtual SimulatorInterface * create()
    {
      return new OpenTxSimulator();
    }

    virtual QString name()
    {
      return QString(SIMULATOR_FLAVOUR);
    }

    virtual Board::Type type()
    {
#if defined(PCBX12S)
      return Board::BOARD_X12S;
#elif defined(PCBX10)
      return Board::BOARD_X10;
#elif defined(PCBFLAMENCO)
      return Board::BOARD_FLAMENCO;
#elif defined(PCBX7)
      return Board::BOARD_TARANIS_X7;
#elif defined(PCBTARANIS)
      return Board::BOARD_TARANIS_X9D;
#else
      return Board::BOARD_STOCK;
#endif
    }
};

extern "C" DLLEXPORT SimulatorFactory * registerSimu()
{
  return new OpenTxSimulatorFactory();
}
