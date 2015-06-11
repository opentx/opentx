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

#include "firmwares/er9x/er9xsimulator.h"
#include "firmwares/er9x/er9xinterface.h"

#define SIMU
#define SIMU_EXCEPTIONS
#define FRSKY
#define PHASES
#undef min
#undef max

#define NUM_POTS  3
#define SWSRC_SW1 DSW_SW1

#include <exception>

namespace Er9x {

#include "simpgmspace.h"
#include "er9x.cpp"
#include "stamp.cpp"
#include "menus.cpp"
#include "pers.cpp"
#include "file.cpp"
#include "lcd.cpp"
#include "drivers.cpp"
#include "simpgmspace.cpp"
#include "templates.cpp"
#include "frsky.cpp"
#include "audio.cpp"

uint8_t pxxFlag = 0;
int16_t g_anas[7];

#define isBacklightEnable() (PORTB & (1<<OUT_B_LIGHT))

uint16_t anaIn(uint8_t chan)
{
  if (chan == 7)
    return 1500;
  else
    return g_anas[chan];
}

bool hasExtendedTrims()
{
  return false;
}

void setTrim(uint8_t idx, int8_t value)
{
  g_model.trim[idx] = value;
}

void getTrims(int16_t values[4])
{
  for (int i=0; i<4; i++)
    values[i] = g_model.trim[i];
}

#define TRIMS_GPIO_REG_LHL         pind
#define TRIMS_GPIO_REG_LVD        pind
#define TRIMS_GPIO_REG_RVU        pind
#define TRIMS_GPIO_REG_RHL         pind
#define TRIMS_GPIO_REG_LHR         pind
#define TRIMS_GPIO_REG_LVU        pind
#define TRIMS_GPIO_REG_RVD        pind
#define TRIMS_GPIO_REG_RHR         pind
#define PIN_TRIM_LH_L          (1<<INP_D_TRM_LH_DWN)
#define PIN_TRIM_LV_DN         (1<<INP_D_TRM_LV_DWN)
#define PIN_TRIM_RV_UP         (1<<INP_D_TRM_RV_UP)
#define PIN_TRIM_RH_L          (1<<INP_D_TRM_RH_DWN)
#define PIN_TRIM_LH_R          (1<<INP_D_TRM_LH_UP)
#define PIN_TRIM_LV_UP         (1<<INP_D_TRM_LV_UP)
#define PIN_TRIM_RV_DN         (1<<INP_D_TRM_RV_DWN)
#define PIN_TRIM_RH_R          (1<<INP_D_TRM_RH_UP)

#define TRIM_CASE(key, pin, mask) \
    case key: \
      if (state) pin |= mask; else pin &= ~mask;\
      break;

void simuSetTrim(uint8_t trim, bool state)
{
  switch (trim) {
    TRIM_CASE(0, TRIMS_GPIO_REG_LHL, PIN_TRIM_LH_L)
    TRIM_CASE(1, TRIMS_GPIO_REG_LHR, PIN_TRIM_LH_R)
    TRIM_CASE(2, TRIMS_GPIO_REG_LVD, PIN_TRIM_LV_DN)
    TRIM_CASE(3, TRIMS_GPIO_REG_LVU, PIN_TRIM_LV_UP)
    TRIM_CASE(4, TRIMS_GPIO_REG_RVD, PIN_TRIM_RV_DN)
    TRIM_CASE(5, TRIMS_GPIO_REG_RVU, PIN_TRIM_RV_UP)
    TRIM_CASE(6, TRIMS_GPIO_REG_RHL, PIN_TRIM_RH_L)
    TRIM_CASE(7, TRIMS_GPIO_REG_RHR, PIN_TRIM_RH_R)
  }
}

#define SWITCH_CASE(swtch, pin, mask) \
    case swtch: \
      if (state) pin &= ~(mask); else pin |= (mask); \
      break;
#define SWITCH_3_CASE(swtch, pin1, pin2, mask1, mask2) \
    case swtch: \
      if (state >= 0) pin1 &= ~(mask1); else pin1 |= (mask1); \
      if (state <= 0) pin2 &= ~(mask2); else pin2 |= (mask2); \
      break;

void simuSetSwitch(uint8_t swtch, int8_t state)
{
  switch (swtch) {
    SWITCH_CASE(0, pinc, 1<<INP_C_ThrCt)
    SWITCH_CASE(4, pinc, 1<<INP_C_AileDR)
    SWITCH_3_CASE(3, ping, pine, (1<<INP_G_ID1), (1<<INP_E_ID2))
    SWITCH_CASE(1, ping, 1<<INP_G_RuddDR)
    SWITCH_CASE(2, pine, 1<<INP_E_ElevDR)
    SWITCH_CASE(5, pine, 1<<INP_E_Gear)
    SWITCH_CASE(6, pine, 1<<INP_E_Trainer)
    default:
      break;
  }
}

#define KEY_CASE(key, pin, mask) \
    case key: \
      if (state) pin |= mask; else pin &= ~mask;\
      break;

void simuSetKey(uint8_t key, bool state)
{
  switch (key) {
    KEY_CASE(KEY_MENU, pinb, (1<<INP_B_KEY_MEN))
    KEY_CASE(KEY_EXIT, pinb, (1<<INP_B_KEY_EXT))
    KEY_CASE(KEY_RIGHT, pinb, (1<<INP_B_KEY_RGT))
    KEY_CASE(KEY_LEFT, pinb, (1<<INP_B_KEY_LFT))
    KEY_CASE(KEY_UP, pinb, (1<<INP_B_KEY_UP))
    KEY_CASE(KEY_DOWN, pinb, (1<<INP_B_KEY_DWN))
  }
}

#define isBacklightEnable() (PORTB & (1<<OUT_B_LIGHT))

}

using namespace Er9x;

Er9xSimulator::Er9xSimulator(Er9xInterface * er9xInterface):
  er9xInterface(er9xInterface)
{
}

bool Er9xSimulator::timer10ms()
{
  AUDIO_DRIVER();  // the tone generator
  AUDIO_HEARTBEAT();  // the queue processing
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Er9xSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Er9xSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Er9xSimulator::start(RadioData &radioData, bool tests)
{
  er9xInterface->save(&eeprom[0], radioData);
  StartMainThread(tests);
}

void Er9xSimulator::stop()
{
  StopMainThread();
}

void Er9xSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
  outputs.beep = audio.busy();
}

void Er9xSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Er9xSimulator::setTrim(unsigned int idx, int value)
{
  Er9x::setTrim(idx, value);
}

void Er9xSimulator::getTrims(Trims & trims)
{
  trims.extended = false;
  Er9x::getTrims(trims.values);
}

const char * Er9xSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
