/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
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

#include "opentx.h"

static uint8_t currentSpeakerVolume = 255;
uint8_t requiredSpeakerVolume = 255;

extern void checkBattery();

void handleUsbConnection()
{
#if defined(PCBTARANIS) && !defined(SIMU)
  static bool usbStarted = false;
  if (!usbStarted && usbPlugged()) {
#if defined(USB_MASS_STORAGE)
    opentxClose();
#endif
    usbStart();
#if defined(USB_MASS_STORAGE)
    usbPluggedIn();
#endif
    usbStarted = true;
  }
  
#if defined(USB_JOYSTICK)
  if (usbStarted) {
    if (!usbPlugged()) {
      //disable USB
      usbStop();
      usbStarted = false;
    }
    else {
      usbJoystickUpdate();
    }
  }
#endif
#endif //#if defined(PCBTARANIS) && !defined(SIMU)
}

void checkSpeakerVolume()
{
  if (currentSpeakerVolume != requiredSpeakerVolume) {
    currentSpeakerVolume = requiredSpeakerVolume;
    setVolume(currentSpeakerVolume);
  }
}

void checkEeprom()
{
  if (!usbPlugged()) {
    // TODO merge these 2 branches
#if defined(PCBSKY9X)
    if (Eeprom32_process_state != E32_IDLE)
      ee32_process();
    else if (TIME_TO_WRITE())
      eeCheck(false);
#else
    if (theFile.isWriting())
      theFile.nextWriteStep();
    else if (TIME_TO_WRITE())
      eeCheck(false);
#endif
  }
}

void perMainArm()
{
#if defined(PCBSKY9X) && !defined(REVA)
  calcConsumption();
#endif
  checkSpeakerVolume();
  checkEeprom();
  sdMountPoll();
  writeLogs();
  handleUsbConnection();
  checkTrainerSettings();
  checkBattery();

  uint8_t evt = getEvent(false);
  if (evt && (g_eeGeneral.backlightMode & e_backlight_mode_keys)) backlightOn(); // on keypress turn the light on
  checkBacklight();
#if defined(NAVIGATION_STICKS)
  uint8_t sticks_evt = getSticksNavigationEvent();
  if (sticks_evt) evt = sticks_evt;
#endif

#if defined(USB_MASS_STORAGE)
  if (usbPlugged()) {
    // disable access to menus
    lcd_clear();
    menuMainView(0);
    lcdRefresh();
    return;
  }
#endif

  // run Lua scripts that don't use LCD (to use CPU time while LCD DMA is running)
  luaRunNonGuiScripts();

  // draw LCD from menus or from Lua script
  if (luaRunGuiScripts(evt)) {   // either stand-alone or telemetry scripts
    // let Lua manage LCD fully
  }
  else {
    // normal GUI from menus
    const char *warn = s_warning;
    uint8_t menu = s_menu_count;
    lcd_clear();
    if (menuEvent) {
      m_posVert = menuEvent == EVT_ENTRY_UP ? g_menuPos[g_menuStackPtr] : 0;
      m_posHorz = 0;
      evt = menuEvent;
      menuEvent = 0;
      AUDIO_MENUS();
    }
    g_menuStack[g_menuStackPtr]((warn || menu) ? 0 : evt);
    if (warn) DISPLAY_WARNING(evt);
    if (menu) {
      const char * result = displayMenu(evt);
      if (result) {
        menuHandler(result);
        putEvent(EVT_MENU_UP);
      }
    }
    drawStatusLine();
  }
  lcdRefresh();
}
