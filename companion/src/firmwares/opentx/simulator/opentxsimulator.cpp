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
#include <QDebug>
#include <stdio.h>
#include <exception>
#include <map>
#include <string>

#define COMPANION
#define SIMU
#define SIMU_EXCEPTIONS
#define GUI
#define SPLASH
#define FRSKY
#define FRSKY_HUB
#define GPS
#define VARIO
#define GAUGES
#define PPM_UNIT_PERCENT_PREC1
#define AUDIO
#define PXX
#define DSM2
#define DSM2_PPM
#define DBLKEYS
#define AUTOSWITCH
#define GRAPHICS
#define CURVES
#if defined(PCBTARANIS)
  #define RTCLOCK
  #define XCURVES
  #define VIRTUALINPUTS
  #define LUA
  #define LUA_MODEL_SCRIPTS
  #define LUAINPUTS 
#else
  #define BUZZER
  #define TEMPLATES
#endif
#define BOLD_FONT
#define HAPTIC
#define THRTRACE

#if defined(CPUARM)
#define TIMERS 3
#else
#define TIMERS 2
#endif

#if defined(PCBSKY9X) && !defined(REVX)
  #define ROTARY_ENCODERS 1
#elif defined(PCBGRUVIN9X)
  #define ROTARY_ENCODERS 2
#endif

#if defined(CPUARM)
  #define FRSKY_SPORT
#endif

#if defined (PCBTARANIS)
  #define MIXERS_MONITOR
#endif

#undef min
#undef max

namespace NAMESPACE {

#if defined(CPUARM)
#include "radio/src/pulses/ppm_arm.cpp"
#include "radio/src/pulses/pxx_arm.cpp"
#include "radio/src/pulses/dsm2_arm.cpp"
#include "radio/src/main_arm.cpp"
#include "radio/src/pulses/pulses_arm.cpp"
#include "radio/src/tasks_arm.cpp"
#include "radio/src/audio_arm.cpp"
#include "radio/src/telemetry/telemetry.cpp"
#include "radio/src/telemetry/frsky_sport.cpp"
#include "radio/src/sbus.cpp"
#else
#include "radio/src/main_avr.cpp"
#include "radio/src/audio_avr.cpp"
#include "radio/src/pulses/pulses_avr.cpp"
#endif

#include "radio/src/eeprom_common.cpp"
#if defined(EEPROM_RLC)
#include "radio/src/eeprom_rlc.cpp"
#else
#include "radio/src/eeprom_raw.cpp"
#endif

#if defined(PCBTARANIS) || defined(PCBSKY9X)
#include "radio/src/eeprom_conversions.cpp"
#endif

#include "radio/src/opentx.cpp"
#include "radio/src/debug.cpp"
#include "radio/src/strhelpers.cpp"
#include "radio/src/switches.cpp"
#include "radio/src/functions.cpp"
#include "radio/src/curves.cpp"
#include "radio/src/mixer.cpp"
#if defined(SDCARD)
#include "radio/src/sdcard.cpp"
#endif
#include "radio/src/timers.cpp"
#include "radio/src/stamp.cpp"
#include "radio/src/maths.cpp"
#include "radio/src/vario.cpp"
#include "radio/src/trainer_input.cpp"

#if defined(PCBTARANIS)
#include "radio/src/bmp.cpp"
#include "radio/src/gui/Taranis/fonts.cpp"
#include "radio/src/gui/Taranis/widgets.cpp"
#include "radio/src/gui/Taranis/navigation.cpp"
#include "radio/src/gui/Taranis/popups.cpp"
#include "radio/src/gui/Taranis/helpers.cpp"
#include "radio/src/gui/Taranis/menus.cpp"
#include "radio/src/gui/Taranis/menu_model.cpp"
#include "radio/src/gui/Taranis/menu_model_select.cpp"
#include "radio/src/gui/Taranis/menu_model_setup.cpp"
#include "radio/src/gui/Taranis/menu_model_heli.cpp"
#include "radio/src/gui/Taranis/menu_model_flightmodes.cpp"
#include "radio/src/gui/Taranis/menu_model_inputs_mixes.cpp"
#include "radio/src/gui/Taranis/menu_model_curves.cpp"
#include "radio/src/gui/Taranis/menu_model_logical_switches.cpp"
#include "radio/src/gui/Taranis/menu_model_custom_functions.cpp"
#include "radio/src/gui/Taranis/menu_model_custom_scripts.cpp"
#include "radio/src/gui/Taranis/menu_model_gvars.cpp"
#include "radio/src/gui/Taranis/menu_model_limits.cpp"
#include "radio/src/gui/Taranis/menu_model_telemetry.cpp"
#include "radio/src/gui/Taranis/menu_general.cpp"
#include "radio/src/gui/Taranis/menu_general_setup.cpp"
#include "radio/src/gui/Taranis/menu_general_sdmanager.cpp"
#include "radio/src/gui/Taranis/menu_general_trainer.cpp"
#include "radio/src/gui/Taranis/menu_general_version.cpp"
#include "radio/src/gui/Taranis/menu_general_diagkeys.cpp"
#include "radio/src/gui/Taranis/menu_general_diaganas.cpp"
#include "radio/src/gui/Taranis/menu_general_hardware.cpp"
#include "radio/src/gui/Taranis/menu_general_calib.cpp"
#include "radio/src/gui/Taranis/view_main.cpp"
#include "radio/src/gui/Taranis/view_statistics.cpp"
#include "radio/src/gui/Taranis/view_channels.cpp"
#include "radio/src/gui/Taranis/view_telemetry.cpp"
#include "radio/src/gui/Taranis/view_text.cpp"
#include "radio/src/gui/Taranis/view_about.cpp"
#include "radio/src/gui/Taranis/lcd.cpp"
#include "radio/src/gui/Taranis/splash.cpp"
#else
#include "radio/src/gui/9X/fonts.cpp"
#include "radio/src/gui/9X/widgets.cpp"
#include "radio/src/gui/9X/navigation.cpp"
#include "radio/src/gui/9X/popups.cpp"
#include "radio/src/gui/9X/helpers.cpp"
#include "radio/src/gui/9X/menus.cpp"
#include "radio/src/gui/9X/menu_model.cpp"
#include "radio/src/gui/9X/menu_model_select.cpp"
#include "radio/src/gui/9X/menu_model_setup.cpp"
#include "radio/src/gui/9X/menu_model_heli.cpp"
#include "radio/src/gui/9X/menu_model_flightmodes.cpp"
#include "radio/src/gui/9X/menu_model_inputs_mixes.cpp"
#include "radio/src/gui/9X/menu_model_curves.cpp"
#include "radio/src/gui/9X/menu_model_logical_switches.cpp"
#include "radio/src/gui/9X/menu_model_custom_functions.cpp"
#include "radio/src/gui/9X/menu_model_limits.cpp"
#include "radio/src/gui/9X/menu_model_telemetry.cpp"
#include "radio/src/gui/9X/menu_model_templates.cpp"
#include "radio/src/gui/9X/menu_general.cpp"
#include "radio/src/gui/9X/menu_general_setup.cpp"
#if defined(SDCARD)
#include "radio/src/gui/9X/menu_general_sdmanager.cpp"
#include "radio/src/gui/9X/view_text.cpp"
#endif
#include "radio/src/gui/9X/menu_general_trainer.cpp"
#include "radio/src/gui/9X/menu_general_version.cpp"
#include "radio/src/gui/9X/menu_general_diagkeys.cpp"
#include "radio/src/gui/9X/menu_general_diaganas.cpp"
#if defined(CPUARM)
#include "radio/src/gui/9X/menu_general_hardware.cpp"
#endif
#include "radio/src/gui/9X/menu_general_calib.cpp"
#include "radio/src/gui/9X/view_main.cpp"
#include "radio/src/gui/9X/view_statistics.cpp"
#include "radio/src/gui/9X/view_telemetry.cpp"
#if !defined(CPUM64)
#include "radio/src/gui/9X/view_about.cpp"
#endif
#include "radio/src/gui/9X/lcd.cpp"
#include "radio/src/gui/9X/splash.cpp"
#endif

#if !defined(VIRTUALINPUTS)
#include "radio/src/templates.cpp"
#endif

#if defined(SDCARD)
#include "radio/src/logs.cpp"
#endif

#if defined(RTCLOCK)
#include "radio/src/rtc.cpp"
#endif

#if defined(BUZZER)
#include "radio/src/buzzer.cpp"
#endif

#if defined(PCBTARANIS)
#include "radio/src/targets/taranis/board_taranis.cpp"
#include "radio/src/targets/taranis/keys_driver.cpp"
#include "radio/src/targets/taranis/audio_driver.cpp"
#include "radio/src/targets/taranis/telemetry_driver.cpp"
#include "radio/src/targets/taranis/haptic_driver.cpp"
#if defined(REV9E)
#include "radio/src/targets/taranis/top_lcd_driver.cpp"
#endif
#include "radio/src/targets/taranis/pulses_driver.cpp"
#include "radio/src/targets/taranis/rtc_driver.cpp"
#include "radio/src/targets/taranis/trainer_driver.cpp"
#include "radio/src/targets/taranis/uart3_driver.cpp"
#elif defined(PCBSKY9X)
#include "radio/src/targets/sky9x/board_sky9x.cpp"
#include "radio/src/targets/sky9x/telemetry_driver.cpp"
#include "radio/src/targets/sky9x/pwr_driver.cpp"
#include "radio/src/targets/sky9x/eeprom_driver.cpp"
#include "radio/src/targets/sky9x/keys_driver.cpp"
#include "radio/src/targets/sky9x/audio_driver.cpp"
#include "radio/src/targets/sky9x/sdcard_driver.cpp"
#include "radio/src/targets/sky9x/coproc_driver.cpp"
#include "radio/src/targets/sky9x/haptic_driver.cpp"
#include "radio/src/targets/sky9x/second_serial_driver.cpp"
#include "radio/src/targets/sky9x/pulses_driver.cpp"
#elif defined(PCBGRUVIN9X)
#include "radio/src/targets/gruvin9x/board_gruvin9x.cpp"
#include "radio/src/targets/gruvin9x/somo14d.cpp"
#elif defined(PCBMEGA2560)
#include "radio/src/targets/mega2560/board_mega2560.cpp"
#else
#include "radio/src/targets/stock/board_stock.cpp"
#endif

#include "radio/src/keys.cpp"
#include "radio/src/haptic.cpp"
#include "radio/src/targets/simu/simpgmspace.cpp"
#include "radio/src/translations.cpp"
#include "radio/src/telemetry/frsky.cpp"
#include "radio/src/telemetry/frsky_d.cpp"
#include "radio/src/translations/tts_en.cpp"

#if defined(CPUARM)
#include "radio/src/translations/tts_cz.cpp"
#include "radio/src/translations/tts_de.cpp"
#include "radio/src/translations/tts_es.cpp"
#include "radio/src/translations/tts_fr.cpp"
#include "radio/src/translations/tts_hu.cpp"
#include "radio/src/translations/tts_it.cpp"
#include "radio/src/translations/tts_nl.cpp"
#include "radio/src/translations/tts_pl.cpp"
#include "radio/src/translations/tts_pt.cpp"
#include "radio/src/translations/tts_se.cpp"
#include "radio/src/translations/tts_sk.cpp"
#endif

#if defined(LUA)
#include "radio/src/lua_api.cpp"
#include "radio/src/thirdparty/Lua/src/lapi.c"
#include "radio/src/thirdparty/Lua/src/lcode.c"
#include "radio/src/thirdparty/Lua/src/lctype.c"
#include "radio/src/thirdparty/Lua/src/ldebug.c"
#include "radio/src/thirdparty/Lua/src/ldo.c"
#include "radio/src/thirdparty/Lua/src/ldump.c"
#include "radio/src/thirdparty/Lua/src/lfunc.c"
#include "radio/src/thirdparty/Lua/src/lgc.c"
#include "radio/src/thirdparty/Lua/src/llex.c"
#include "radio/src/thirdparty/Lua/src/lmem.c"
#include "radio/src/thirdparty/Lua/src/lobject.c"
#include "radio/src/thirdparty/Lua/src/lopcodes.c"
#include "radio/src/thirdparty/Lua/src/lparser.c"
#include "radio/src/thirdparty/Lua/src/lstate.c"
#include "radio/src/thirdparty/Lua/src/lstring.c"
#include "radio/src/thirdparty/Lua/src/ltable.c"
#include "radio/src/thirdparty/Lua/src/lrotable.c"
#include "radio/src/thirdparty/Lua/src/ltm.c"
#include "radio/src/thirdparty/Lua/src/lundump.c"
#include "radio/src/thirdparty/Lua/src/lvm.c"
#include "radio/src/thirdparty/Lua/src/lzio.c"
#include "radio/src/thirdparty/Lua/src/lbaselib.c"
#include "radio/src/thirdparty/Lua/src/linit.c"
#include "radio/src/thirdparty/Lua/src/lmathlib.c"
#include "radio/src/thirdparty/Lua/src/lbitlib.c"
#include "radio/src/thirdparty/Lua/src/loadlib.c"
#include "radio/src/thirdparty/Lua/src/lauxlib.c"
#include "radio/src/thirdparty/Lua/src/ltablib.c"
#include "radio/src/thirdparty/Lua/src/lcorolib.c"
#include "radio/src/thirdparty/Lua/src/liolib.c"
#endif

int16_t g_anas[NUM_STICKS+NUM_POTS];

uint16_t anaIn(uint8_t chan)
{
    return g_anas[chan];
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

void delay_01us(::uint16_t nb)
{
}

OpenTxSimulator::OpenTxSimulator()
{
}

void OpenTxSimulator::setSdPath(const QString &sdPath)
{
#if defined(SDCARD)
  strncpy(simuSdDirectory, sdPath.toAscii().constData(), sizeof(simuSdDirectory)-1);
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

::uint8_t * OpenTxSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool OpenTxSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void OpenTxSimulator::start(QByteArray & eeprom, bool tests)
{
  memcpy(NAMESPACE::eeprom, eeprom.data(), std::min<int>(sizeof(NAMESPACE::eeprom), eeprom.size()));
  start((const char *)0, tests);
}

void OpenTxSimulator::start(const char * filename, bool tests)
{
#if defined(PCBSKY9X) && !defined(REVX)
  g_rotenc[0] = 0;
#elif defined(PCBGRUVIN9X)
  g_rotenc[0] = 0;
  g_rotenc[1] = 0;
#endif

  StartEepromThread(filename);
  StartAudioThread(volumeGain);
  StartMainThread(tests);
}

void OpenTxSimulator::stop()
{
  StopMainThread();
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
#if defined(PCBSKY9X)
  outputs.beep = g_beepCnt;
  g_beepCnt = 0;
#endif
}

void OpenTxSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void OpenTxSimulator::setTrim(unsigned int idx, int value)
{
  idx = NAMESPACE::modn12x3[4*getStickMode() + idx];
  ::uint8_t phase = getTrimFlightPhase(getFlightMode(), idx);
  setTrimValue(phase, idx, value);
}

void OpenTxSimulator::getTrims(Trims & trims)
{
  ::uint8_t phase = getFlightMode();
  trims.extended = hasExtendedTrims();
  for (::uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(phase, idx), idx);
  }

  for (int i=0; i<2; i++) {
    ::uint8_t idx = NAMESPACE::modn12x3[4*getStickMode() + i];
    ::int16_t tmp = trims.values[i];
    trims.values[i] = trims.values[idx];
    trims.values[idx] = tmp;
  }
}

void OpenTxSimulator::wheelEvent(int steps)
{
#if defined(REV9E)
  if (steps == 255)
    x9de_rotenc -= 2;
  else
    x9de_rotenc += 2;
#elif defined(PCBSKY9X) && !defined(REVX)
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

void OpenTxSimulator::sendTelemetry(::uint8_t * data, unsigned int len)
{
#if defined(FRSKY_SPORT)
  processSportPacket(data);
#endif
}

void OpenTxSimulator::setTrainerInput(unsigned int inputNumber, ::int16_t value)
{
#define SETTRAINER_IMPORT
#include "simulatorimport.h"
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
      return QString("opentx-" FLAVOUR SUFFIX);
    }

    virtual BoardEnum type()
    {
#if defined(PCBTARANIS)
      return BOARD_TARANIS;
#else
      return BOARD_STOCK;
#endif
    }
};

}

extern "C" DLLEXPORT SimulatorFactory *registerSimu()
{
  return new NAMESPACE::OpenTxSimulatorFactory();
}
