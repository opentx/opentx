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

#include <QTimer>
#include <QDebug>
#include <QElapsedTimer>

#if !defined(MAX_LOGICAL_SWITCHES) && defined(NUM_CSW)
  #define MAX_LOGICAL_SWITCHES    NUM_CSW
#endif

#if defined(CPUARM)
  #define GET_SWITCH_BOOL(sw__)    getSwitch((sw__), 0);
#else
  #define GET_SWITCH_BOOL(sw__)    getSwitch(sw__);
#endif

int16_t g_anas[NUM_STICKS+NUM_POTS+NUM_SLIDERS];

uint16_t anaIn(uint8_t chan)
{
  return g_anas[chan];
}

uint16_t getAnalogValue(uint8_t index)
{
  return anaIn(index);
}

OpenTxSimulator::OpenTxSimulator() :
  SimulatorInterface(),
  m_timer10ms(NULL),
  m_resetOutputsData(true)
{
}

OpenTxSimulator::~OpenTxSimulator()
{
  if (m_timer10ms)
    m_timer10ms->deleteLater();
  //qDebug() << "Deleting OpenTxSimulator";
}

bool OpenTxSimulator::hasExtendedTrims()
{
  return g_model.extendedTrims;
}

uint8_t OpenTxSimulator::getStickMode()
{
  return limit<uint8_t>(0, g_eeGeneral.stickMode, 3);
}

void OpenTxSimulator::setSdPath(const QString & sdPath, const QString & settingsPath)
{
  simuSdDirectory = sdPath;
  simuSettingsDirectory = settingsPath;
}

void OpenTxSimulator::setVolumeGain(const int value)
{
  volumeGain = value;
}

void OpenTxSimulator::timer10ms()
{
  static uint32_t loops = 0;
  static QElapsedTimer ts;

  if (!loops)
    ts.start();

  if (!isRunning()) {
    QString err(getError());
    emit runtimeError(err);
    stop();
    return;
  }

  ++loops;

  per10ms();

  checkLcdChanged();

  if (!(loops % 5)) {
    checkOutputsChanged();
  }

  if (!(loops % (SIMULATOR_INTERFACE_HEARTBEAT_PERIOD / 10))) {
    emit heartbeat(loops, ts.elapsed());
  }

}

uint8_t * OpenTxSimulator::getLcd()
{
  return (uint8_t *)simuLcdBuf;
}

bool OpenTxSimulator::checkLcdChanged()
{
  if (simuLcdRefresh) {
    simuLcdRefresh = false;
    emit lcdChange(isBacklightEnabled());
    return true;
  }
  return false;
}

bool OpenTxSimulator::lcdChanged(bool & lightEnable)
{
  if (checkLcdChanged()) {
    lightEnable = isBacklightEnabled();
    return true;
  }
  return false;
}


void OpenTxSimulator::checkOutputsChanged()
{
  static TxOutputs lastOutputs;
  static size_t chansDim = DIM(channelOutputs);
  qint32 tmpVal;
  uint8_t i, idx;
  uint8_t phase = getPhase();
  uint8_t mode = getStickMode();

  for (i=0; i < chansDim; i++) {
    if (lastOutputs.chans[i] != channelOutputs[i] || m_resetOutputsData) {
      emit channelOutValueChange(i, channelOutputs[i]);
      emit channelMixValueChange(i, ex_chans[i]);
      emit outputValueChange(OUTPUT_SRC_CHAN_OUT, i, channelOutputs[i]);
      emit outputValueChange(OUTPUT_SRC_CHAN_MIX, i, ex_chans[i]);
      lastOutputs.chans[i] = channelOutputs[i];
    }
  }

  for (i=0; i < MAX_LOGICAL_SWITCHES; i++) {
    tmpVal = (qint32)GET_SWITCH_BOOL(SWSRC_SW1+i);
    if (lastOutputs.vsw[i] != (bool)tmpVal || m_resetOutputsData) {
      emit virtualSwValueChange(i, tmpVal);
      emit outputValueChange(OUTPUT_SRC_VIRTUAL_SW, i, tmpVal);
      lastOutputs.vsw[i] = tmpVal;
    }
  }

  for (i=0; i < CPN_MAX_TRIMS; i++) {
    if (i < 4)  // swap axes
      idx = modn12x3[4 * mode + i];
    else
      idx = i;

    tmpVal = getTrimValue(getTrimFlightMode(phase, idx), idx);
    if (lastOutputs.trims[i] != tmpVal || m_resetOutputsData) {
      emit trimValueChange(i, tmpVal);
      emit outputValueChange(OUTPUT_SRC_TRIM_VALUE, i, tmpVal);
      lastOutputs.trims[i] = tmpVal;
    }
  }

  tmpVal = (qint32)hasExtendedTrims();
  if (lastOutputs.trimRange != (bool)tmpVal || m_resetOutputsData) {
    emit trimRangeChange(0, tmpVal);
    emit outputValueChange(OUTPUT_SRC_TRIM_RANGE, 0, tmpVal);
    lastOutputs.trimRange = tmpVal;
  }

  if (lastOutputs.phase != phase || m_resetOutputsData) {
    emit phaseChanged(phase, getCurrentPhaseName());
    emit outputValueChange(OUTPUT_SRC_PHASE, 0, qint16(phase));
    lastOutputs.phase = phase;
  }

#if defined(GVAR_VALUE) && defined(GVARS)
  gVarMode_t gvar;
  for (uint8_t fm=0; fm < MAX_FLIGHT_MODES; fm++) {
    gvar.mode = fm;
    for (uint8_t gv=0; gv < MAX_GVARS; gv++) {
      tmpVal = GVAR_VALUE(gv, getGVarFlightMode(fm, gv));
      if (lastOutputs.gvars[fm][gv] != tmpVal || m_resetOutputsData) {
        lastOutputs.gvars[fm][gv] = tmpVal;
        gvar.value = (int16_t)tmpVal;
        tmpVal = gvar;
        emit gVarValueChange(gv, tmpVal);
        emit outputValueChange(OUTPUT_SRC_GVAR, gv, tmpVal);
      }
    }
  }
#endif

  m_resetOutputsData = false;
}

void OpenTxSimulator::setRadioData(const QByteArray & data)
{
#if defined(EEPROM_SIZE)
  memcpy(eeprom, data.data(), qMin<int>(EEPROM_SIZE, data.size()));
#endif
}

void OpenTxSimulator::start(const char * filename, bool tests)
{
  if (isRunning())
    return;

  if (!m_timer10ms) {
    m_timer10ms = new QTimer(this);
    m_timer10ms->setInterval(10);
    connect(m_timer10ms, &QTimer::timeout, this, &OpenTxSimulator::timer10ms);
  }
  simuInit();
  StartEepromThread(filename);
  StartAudioThread(volumeGain);
  StartSimu(tests, simuSdDirectory.toLatin1().constData(), simuSettingsDirectory.toLatin1().constData());
  m_resetOutputsData = true;
  m_timer10ms->start();
  timer10ms();
  emit started();
}

void OpenTxSimulator::stop()
{
  if (!isRunning())
    return;

  if (m_timer10ms)
    m_timer10ms->stop();
  StopSimu();
#if defined(CPUARM)
  StopAudioThread();
#endif
  StopEepromThread();
  emit stopped();
}

bool OpenTxSimulator::isRunning()
{
  return (bool)main_thread_running;
}

void OpenTxSimulator::readEepromData(QByteArray & dest)
{
#if defined(EEPROM_SIZE)
  memcpy(dest.data(), eeprom, std::min<int>(EEPROM_SIZE, dest.size()));
#endif
}

void OpenTxSimulator::setAnalogValue(uint8_t index, int16_t value)
{
  if (index < NUM_STICKS + NUM_POTS + NUM_SLIDERS) {
#if defined(PCBTARANIS) && !defined(PCBX7)
    // this needs to follow the exception in radio/src/mixer.cpp:evalInputs()
    if (index == POT1 || index == SLIDER1)
      value = -value;
#endif
    g_anas[index] = value;
  }
}

void OpenTxSimulator::setSwitch(uint8_t swtch, int8_t state)
{
  simuSetSwitch(swtch, state);
}

void OpenTxSimulator::setKey(uint8_t key, bool state)
{
  simuSetKey(key, state);
}

void OpenTxSimulator::setTrimSwitch(uint8_t trim, bool state)
{
  simuSetTrim(trim, state);
}

void OpenTxSimulator::setTrim(unsigned int idx, int value)
{
  idx = modn12x3[4*getStickMode() + idx];
  uint8_t phase = getTrimFlightMode(getFlightMode(), idx);
  setTrimValue(phase, idx, value);
}

void OpenTxSimulator::setInputValue(int type, uint8_t index, int16_t value)
{
  //qDebug() << type << index << value;
  switch (type) {
    case INPUT_SRC_STICK :
      setAnalogValue(index, value);
      break;
    case INPUT_SRC_KNOB :
      setAnalogValue(index + NUM_STICKS, value);
      break;
    case INPUT_SRC_SLIDER :
      setAnalogValue(index + NUM_STICKS + NUM_POTS, value);
      break;
    case INPUT_SRC_SWITCH :
      setSwitch(index, (int8_t)value);
      break;
    case INPUT_SRC_TRIM_SW :
      setTrimSwitch(index, (bool)value);
      break;
    case INPUT_SRC_TRIM :
      setTrim(index, value);
      break;
    case INPUT_SRC_KEY :
      setKey(index, (bool)value);
      break;
    case INPUT_SRC_TRAINER :
      setTrainerInput(index, value);
      break;
    case INPUT_SRC_ROTENC :  // TODO
    default:
      return;
  }
}

void OpenTxSimulator::rotaryEncoderEvent(int steps)
{
#if defined(ROTARY_ENCODER_NAVIGATION)
  ROTARY_ENCODER_NAVIGATION_VALUE += steps * ROTARY_ENCODER_GRANULARITY;
#else
  int key;
  if (steps > 0)
    key = KEY_MINUS;
  else if (steps < 0)
    key = KEY_PLUS;

  setKey(key, 1);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
  QTimer::singleShot(10, [this, key]() { setKey(key, 0); });
#else
  QTimer *timer = new QTimer(this);
  timer->setSingleShot(true);
  connect(timer, &QTimer::timeout, [=]() {
    setKey(key, 0);
    timer->deleteLater();
  } );
  timer->start(10);
#endif
#endif  // defined(ROTARY_ENCODER_NAVIGATION)
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

const QString OpenTxSimulator::getCurrentPhaseName()
{
  QString name(getPhaseName(getPhase()));
  if (name.isEmpty())
    name = QString::number(getPhase());
  return name;
}

const char * OpenTxSimulator::getError()
{
  return main_thread_error;
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

void OpenTxSimulator::setTrainerTimeout(uint16_t ms)
{
  ppmInputValidityTimer = ms;
}

void OpenTxSimulator::setTrainerInput(unsigned int inputNumber, int16_t value)
{
  //setTrainerTimeout(100);
  ppmInput[inputNumber] = qMin(qMax((int16_t)-512, value), (int16_t)512);
}

void OpenTxSimulator::setLuaStateReloadPermanentScripts()
{
#if defined(LUA)
  luaState = INTERPRETER_RELOAD_PERMANENT_SCRIPTS;
#endif
}

QString OpenTxSimulator::name()
{
  return QString(SIMULATOR_FLAVOUR);
}

void OpenTxSimulator::installTraceHook(void (*callback)(const char *))
{
  traceCallback = callback;
}


/*
 * OpenTxSimulatorFactory
 */

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
