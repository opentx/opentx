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

#include "opentxM128simulator.h"
#include "opentxinterface.h"
#include "opentxeeprom.h"

#define SIMU
#define SIMU_EXCEPTIONS
#define PCBSTD
#define CPUM128
#define HELI
#define TEMPLATES
#define SPLASH
#define FLIGHT_MODES
#define FRSKY
#define FRSKY_HUB
#define WS_HOW_HIGH
#define VARIO
#define PPM_UNIT_PERCENT_PREC1
#define AUDIO
#define HAPTIC
#define AUTOSWITCH
#define GRAPHICS
#define CURVES
#define XCURVES
#define GVARS
#define BOLD_FONT
#define VOICE
#define PPM_CENTER_ADJUSTABLE
#define PPM_LIMITS_SYMETRICAL
#define GAUGES
#define GPS
#define FAI_CHOICE

#define EEPROM_VARIANT SIMU_M128_VARIANTS
#define GAUGES

#undef min
#undef max

#include <exception>

namespace Open9xM128 {
#include "radio/src/targets/stock/board_stock.cpp"
#include "radio/src/eeprom_common.cpp"
#include "radio/src/eeprom_rlc.cpp"
#include "radio/src/opentx.cpp"
#include "radio/src/protocols/pulses_avr.cpp"
#include "radio/src/stamp.cpp"
#include "radio/src/maths.cpp"
#include "radio/src/gui/menus.cpp"
#include "radio/src/gui/menu_model.cpp"
#include "radio/src/gui/menu_general.cpp"
#include "radio/src/gui/view_main.cpp"
#include "radio/src/gui/view_statistics.cpp"
#include "radio/src/gui/view_telemetry.cpp"
#include "radio/src/lcd.cpp"
#include "radio/src/keys.cpp"
#include "radio/src/simpgmspace.cpp"
#include "radio/src/templates.cpp"
#include "radio/src/translations.cpp"
#include "radio/src/fonts.cpp"
#include "radio/src/audio_avr.cpp"
#include "radio/src/targets/stock/voice.cpp"
#include "radio/src/telemetry/frsky.cpp"
#include "radio/src/translations/tts_en.cpp"
#include "radio/src/haptic.cpp"

int16_t g_anas[NUM_STICKS+BOARD_9X_NUM_POTS];

uint16_t anaIn(uint8_t chan)
{
  if (chan == 7)
    return 150;
  else
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

}

using namespace Open9xM128;

Open9xM128Simulator::Open9xM128Simulator(OpenTxInterface * open9xInterface):
  open9xInterface(open9xInterface)
{
}

bool Open9xM128Simulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Open9xM128Simulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Open9xM128Simulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Open9xM128Simulator::start(QByteArray & eeprom, bool tests)
{
  memcpy(Open9xM128::eeprom, eeprom.data(), std::min<int>(sizeof(Open9xM128::eeprom), eeprom.size()));
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void Open9xM128Simulator::start(const char * filename, bool tests)
{
  StartEepromThread(filename);
  StartMainThread(tests);
}

void Open9xM128Simulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void Open9xM128Simulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#define g_chans512 channelOutputs
#include "simulatorimport.h"
}

void Open9xM128Simulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xM128Simulator::setTrim(unsigned int idx, int value)
{
  idx = Open9xM128::modn12x3[4*getStickMode() + idx];
  uint8_t phase = getTrimFlightPhase(getFlightPhase(), idx);
  setTrimValue(phase, idx, value);
}

void Open9xM128Simulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightPhase();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(phase, idx), idx);
  }

  for (int i=0; i<2; i++) {
    uint8_t idx = Open9xM128::modn12x3[4*getStickMode() + i];
    int16_t tmp = trims.values[i];
    trims.values[i] = trims.values[idx];
    trims.values[idx] = tmp;
  }
}

void Open9xM128Simulator::wheelEvent(uint8_t steps)
{
}

unsigned int Open9xM128Simulator::getPhase()
{
  return getFlightPhase();
}

const char * Open9xM128Simulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
