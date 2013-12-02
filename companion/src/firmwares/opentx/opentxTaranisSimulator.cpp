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

#include "opentxTaranisSimulator.h"
#include "open9xinterface.h"

#define SIMU
#define SIMU_EXCEPTIONS
#define PCBTARANIS
#define CPUARM
#define HELI
#define TEMPLATES
#define SPLASH
#define FLIGHT_MODES
#define FRSKY
#define FRSKY_HUB
#define FRSKY_SPORT
#define GPS
#define VARIO
#define GAUGES
#define PPM_UNIT_PERCENT_PREC1
#define AUDIO
#define VOICE
#define PXX
#define DBLKEYS
#define AUTOSWITCH
#define GRAPHICS
#define SDCARD
#define RTCLOCK
#define CURVES
#define XCURVES
#define GVARS
#define BOLD_FONT
#define PPM_CENTER_ADJUSTABLE
#define PPM_LIMITS_SYMETRICAL
#define FAI_CHOICE
#define EEPROM_VARIANT 3

#undef min
#undef max

#ifndef __GNUC__
#include "../winbuild/winbuild.h"
#endif

#include <exception>

namespace Open9xX9D {

int taranisSimulatorBoard = 0;
inline int geteepromsize() {
  return taranisSimulatorBoard==BOARD_TARANIS_REV4a ? 64*1024 : 32*1024;
}
#define EESIZE geteepromsize()

#include "radio/src/targets/taranis/board_taranis.cpp"
#include "radio/src/protocols/ppm_arm.cpp"
#include "radio/src/protocols/pxx_arm.cpp"
#include "radio/src/targets/taranis/pwr_driver.cpp"
#include "radio/src/eeprom_common.cpp"
#include "radio/src/eeprom_conversions.cpp"
#include "radio/src/eeprom_rlc.cpp"
#include "radio/src/opentx.cpp"
#include "radio/src/targets/taranis/pulses_driver.cpp"
#include "radio/src/targets/taranis/rtc_driver.cpp"
#include "radio/src/targets/taranis/trainer_driver.cpp"
#include "radio/src/protocols/pulses_arm.cpp"
#include "radio/src/stamp.cpp"
#include "radio/src/maths.cpp"
#include "radio/src/gui/menus.cpp"
#include "radio/src/gui/menu_model.cpp"
#include "radio/src/gui/menu_general.cpp"
#include "radio/src/gui/view_main.cpp"
#include "radio/src/gui/view_statistics.cpp"
#include "radio/src/gui/view_channels.cpp"
#include "radio/src/gui/view_telemetry.cpp"
#include "radio/src/gui/view_text.cpp"
#include "radio/src/gui/view_about.cpp"
#include "radio/src/lcd.cpp"
#include "radio/src/logs.cpp"
#include "radio/src/rtc.cpp"
#include "radio/src/targets/taranis/keys_driver.cpp"
#include "radio/src/keys.cpp"
#include "radio/src/bmp.cpp"
// TODO Because FatFS in not C++ there cannot be namespaces there and the functions are defined several times!
#undef SDCARD
#include "radio/src/simpgmspace.cpp"
#define SDCARD
#include "radio/src/templates.cpp"
#include "radio/src/translations.cpp"
#include "radio/src/telemetry/frsky_sport.cpp"
#include "radio/src/targets/taranis/audio_driver.cpp"
#include "radio/src/audio_arm.cpp"
#include "radio/src/translations/tts_cz.cpp"
#include "radio/src/translations/tts_de.cpp"
#include "radio/src/translations/tts_en.cpp"
#include "radio/src/translations/tts_es.cpp"
#include "radio/src/translations/tts_se.cpp"
#include "radio/src/translations/tts_it.cpp"
#include "radio/src/translations/tts_fr.cpp"
#include "radio/src/translations/tts_pt.cpp"
#include "radio/src/translations/tts_sk.cpp"
#include "radio/src/translations/tts_pl.cpp"
  
int16_t g_anas[NUM_STICKS+BOARD_X9D_NUM_POTS];

uint16_t anaIn(uint8_t chan)
{
  if (chan == 8)
    return 1800;
  else
    return g_anas[chan];
}

bool hasExtendedTrims()
{
  return g_model.extendedTrims;
}

uint8_t getStickMode()
{
  return g_eeGeneral.stickMode;
}

void resetTrims()
{
  GPIOE->IDR |= PIN_TRIM_LH_L | PIN_TRIM_LH_R | PIN_TRIM_LV_DN | PIN_TRIM_LV_UP;
  GPIOC->IDR |= PIN_TRIM_RV_DN | PIN_TRIM_RV_UP | PIN_TRIM_RH_L | PIN_TRIM_RH_R;
}

}

using namespace Open9xX9D;

OpentxTaranisSimulator::OpentxTaranisSimulator(Open9xInterface * open9xInterface):
  open9xInterface(open9xInterface)
{
  taranisSimulatorBoard = GetEepromInterface()->getBoard();
  QSettings settings("companion9x", "companion9x");
  QString path=settings.value("sdPath", ".").toString()+"/";
  int i=0;
  for (i=0; i< std::min(path.length(),1022); i++) {
    simuSdDirectory[i]=path.at(i).toAscii();
  }
  simuSdDirectory[i]=0;  
}

bool OpentxTaranisSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * OpentxTaranisSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool OpentxTaranisSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void OpentxTaranisSimulator::start(RadioData &radioData, bool tests)
{
  open9xInterface->save(Open9xX9D::eeprom, radioData);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void OpentxTaranisSimulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void OpentxTaranisSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#define g_chans512 channelOutputs
#include "simulatorimport.h"
}

void OpentxTaranisSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void OpentxTaranisSimulator::setTrim(unsigned int idx, int value)
{
  idx = Open9xX9D::modn12x3[4*getStickMode() + idx] - 1;
  uint8_t phase = getTrimFlightPhase(getFlightPhase(), idx);
  setTrimValue(phase, idx, value);
}

void OpentxTaranisSimulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightPhase();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(phase, idx), idx);
  }

  for (int i=0; i<2; i++) {
    uint8_t idx = Open9xX9D::modn12x3[4*getStickMode() + i] - 1;
    int16_t tmp = trims.values[i];
    trims.values[i] = trims.values[idx];
    trims.values[idx] = tmp;
  }
}

void OpentxTaranisSimulator::wheelEvent(uint8_t steps)
{
  // g_rotenc[0] += steps*4;
}

unsigned int OpentxTaranisSimulator::getPhase()
{
  return getFlightPhase();
}

const char * OpentxTaranisSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
