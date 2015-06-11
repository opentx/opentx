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

#include "firmwares/th9x/th9xsimulator.h"
#include "firmwares/th9x/th9xinterface.h"

#define SIM
#define SIMU
#define SIMU_EXCEPTIONS

#undef min
#undef max

#define NUM_POTS  3
#define NUM_CSW   12
#define SWSRC_SW1 10

#include <exception>
#include <algorithm>

namespace Th9x {

#include "simpgmspace.h"
#include "th9x.cpp"
#include "stamp.cpp"
#include "menus.cpp"
#include "pers.cpp"
#include "file.cpp"
#include "lcd.cpp"
#include "drivers.cpp"
#include "foldedlist.cpp"
#include "simpgmspace.cpp"
#include "pulses.cpp"

int16_t g_anas[7];
char g_title[80] = "";

uint16_t anaIn(uint8_t chan)
{
  if (chan == 7)
    return 750;
  else
    return 512 + g_anas[chan];
}

bool hasExtendedTrims()
{
  return false;
}

void setTrim(uint8_t idx, int8_t value)
{
  g_model.trimData[idx].itrim = trimRevertM(value, g_model.trimData[idx].tmode);
}

void getTrims(int16_t values[4])
{
  for (int i=0; i<4; i++) {
    values[i] = trimVal(i);
  }
}

void eeprom_RESV_mismatch(void)
{
  assert(!"Should never been called. Only needed by VC++ (debug mode)");
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
    SWITCH_CASE(0, pine, 1<<INP_E_ThrCt)
    SWITCH_CASE(4, pine, 1<<INP_E_AileDR)
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

using namespace Th9x;

Th9xSimulator::Th9xSimulator(Th9xInterface * th9xInterface):
  th9xInterface(th9xInterface)
{
}

bool Th9xSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Th9xSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Th9xSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Th9xSimulator::start(RadioData &radioData, bool tests)
{
  th9xInterface->save(&eeprom[0], radioData);
  StartMainThread(tests);
}

void Th9xSimulator::stop()
{
  StopMainThread();
}

void Th9xSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
  for (int i=0; i<8; i++)
    outputs.chans[i] *= 2;
}

void Th9xSimulator::setValues(TxInputs &_inputs)
{
  TxInputs inputs = _inputs;
  for (int i=0; i<4; i++)
    inputs.sticks[i] /= 2;
  for (int i=0; i<3; i++)
    inputs.pots[i] /= 2;  
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Th9xSimulator::setTrim(unsigned int idx, int value)
{
  Th9x::setTrim(idx, value);
}

void Th9xSimulator::getTrims(Trims & trims)
{
  trims.extended = false;
  Th9x::getTrims(trims.values);
}

const char * Th9xSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
