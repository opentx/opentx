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

#include "opentxM64simulator.h"
#include "opentxinterface.h"
#include "opentxeeprom.h"

#define HELI
#define SIMU
#define SIMU_EXCEPTIONS
#define PCBSTD
#define CPUM64
#define SPLASH
#define FLIGHT_MODES
#define PPM_UNIT_PERCENT_PREC1
#define HELI
#define TEMPLATES
#define FRSKY
#define FRSKY_HUB
#define WS_HOW_HIGH
#define PXX
#define DSM2
#define DSM2_PPM
#define VARIO
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
#define BUZZER
#define GAUGES
#define GPS
#define FAI_CHOICE
#define FRSKY_STICKS
#define OVERRIDE_CHANNEL_FUNCTION
#define FAS_OFFSET
#define EEPROM_VARIANT SIMU_STOCK_VARIANTS
#define GAUGES
#define FLAVOUR "9x"
#define NUM_POTS  3

#undef min
#undef max

#include <exception>

namespace OpenTxM64 {
#include "radio/src/targets/stock/board_stock.cpp"
#include "radio/src/targets/common_avr/telemetry_driver.cpp"
#include "radio/src/eeprom_common.cpp"
#include "radio/src/eeprom_rlc.cpp"
#include "radio/src/opentx.cpp"
#include "radio/src/main_avr.cpp"
#include "radio/src/strhelpers.cpp"
#include "radio/src/switches.cpp"
#include "radio/src/functions.cpp"
#include "radio/src/curves.cpp"
#include "radio/src/mixer.cpp"
#include "radio/src/pulses/pulses_avr.cpp"
#include "radio/src/stamp.cpp"
#include "radio/src/maths.cpp"
#include "radio/src/vario.cpp"
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
#include "radio/src/gui/9X/menu_general_trainer.cpp"
#include "radio/src/gui/9X/menu_general_version.cpp"
#include "radio/src/gui/9X/menu_general_diagkeys.cpp"
#include "radio/src/gui/9X/menu_general_diaganas.cpp"
#include "radio/src/gui/9X/menu_general_calib.cpp"
#include "radio/src/gui/9X/view_main.cpp"
#include "radio/src/gui/9X/view_statistics.cpp"
#include "radio/src/gui/9X/view_telemetry.cpp"
#include "radio/src/lcd_common.cpp"
#include "radio/src/lcd_default.cpp"
#include "radio/src/keys.cpp"
#include "radio/src/targets/simu/simpgmspace.cpp"
#include "radio/src/telemetry/frsky.cpp"
#include "radio/src/telemetry/frsky_d.cpp"
#include "radio/src/templates.cpp"
#include "radio/src/translations.cpp"
#include "radio/src/gui/9X/fonts.cpp"
#include "radio/src/targets/stock/voice.cpp"
#include "radio/src/buzzer.cpp"
#include "radio/src/translations/tts_en.cpp"
#include "radio/src/haptic.cpp"

int16_t g_anas[NUM_STICKS+BOARD_9X_NUM_POTS];

uint16_t anaIn(uint8_t chan)
{
  if (chan == 7)
    return 1500;
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

using namespace OpenTxM64;

OpenTxM64Simulator::OpenTxM64Simulator()
{
#define INIT_IMPORT
#include "simulatorimport.h"
}

bool OpenTxM64Simulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * OpenTxM64Simulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool OpenTxM64Simulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void OpenTxM64Simulator::start(QByteArray & eeprom, bool tests)
{
  memcpy(&OpenTxM64::eeprom[0], eeprom.data(), 2048);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void OpenTxM64Simulator::start(const char * filename, bool tests)
{
  StartEepromThread(filename);
  StartMainThread(tests);
}

void OpenTxM64Simulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void OpenTxM64Simulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#define g_chans512 channelOutputs
#include "simulatorimport.h"
  outputs.beep = g_beepCnt;
  g_beepCnt = 0;
}

void OpenTxM64Simulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void OpenTxM64Simulator::setTrim(unsigned int idx, int value)
{
  idx = OpenTxM64::modn12x3[4*getStickMode() + idx];
  uint8_t phase = getTrimFlightPhase(getFlightMode(), idx);
  setTrimValue(phase, idx, value);
}

void OpenTxM64Simulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightMode();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(phase, idx), idx);
  }

  for (int i=0; i<2; i++) {
    uint8_t idx = OpenTxM64::modn12x3[4*getStickMode() + i];
    int16_t tmp = trims.values[i];
    trims.values[i] = trims.values[idx];
    trims.values[idx] = tmp;
  }
}

unsigned int OpenTxM64Simulator::getPhase()
{
  return getFlightMode();
}

const char * OpenTxM64Simulator::getPhaseName(unsigned int phase)
{
  static char buff[sizeof(g_model.flightModeData[0].name)+1];
  zchar2str(buff, g_model.flightModeData[phase].name, sizeof(g_model.flightModeData[0].name));
  return buff;
}

const char * OpenTxM64Simulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
