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

uint8_t currentSpeakerVolume = 255;
uint8_t requiredSpeakerVolume = 255;
uint8_t requestScreenshot = false;

void handleUsbConnection()
{
#if defined(PCBTARANIS) && !defined(SIMU)
  static bool usbStarted = false;

  if (!usbStarted && usbPlugged()) {
    usbStarted = true;

    /*
      We used to initialize USB peripheral and driver here.
      According to my tests this is way too late. The USB peripheral
      therefore does not have enough information to start responding to 
      USB host request, which causes very slow USB device recognition, 
      multiple USB device resets, etc...

      If we want to change the USB profile, the procedure is simple:
        * USB cable must be disconnected
        * call usbDeInit();
        * call usbUnit(); which initializes USB with the new profile. 
          Obviously the usbInit() should be modified to have a runtime
          selection of the USB profile.
    */

#if defined(USB_MASS_STORAGE)
    opentxClose();
    usbPluggedIn();
#endif
  }
  if (usbStarted && !usbPlugged()) {
    usbStarted = false;
  }
  
#if defined(USB_JOYSTICK)
  if (usbStarted ) {
    usbJoystickUpdate();
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
    if (eepromIsWriting())
      eepromWriteProcess();
    else if (TIME_TO_WRITE())
      eeCheck(false);
  }
}

void handleGui(uint8_t event) {
  // if Lua standalone, run it and don't clear the screen (Lua will do it)
  // else if Lua telemetry view, run it and don't clear the screen
  // else clear scren and show normal menus 
#if defined(LUA)
  if (luaTask(event, RUN_STNDAL_SCRIPT, true)) {
    // standalone script is active
  }
  else if (luaTask(event, RUN_TELEM_FG_SCRIPT, true)) {
    // the telemetry screen is active
    // prevent events from keys MENU, UP, DOWN, ENT(short) and EXIT(short) from reaching the normal menus,
    // so Lua telemetry script can fully use them
    if (event) {
      uint8_t key = EVT_KEY_MASK(event);
      // no need to filter out MENU and ENT(short), because they are not used by menuTelemetryFrsky()
      if (key == KEY_PLUS || key == KEY_MINUS || (!IS_KEY_LONG(event) && key == KEY_EXIT)) {
        // TRACE("Telemetry script event 0x%02x killed", event);
        event = 0;
      }
    }
    menuHandlers[menuLevel](event);
    // todo     drawStatusLine(); here???
  }
  else 
#endif
  {
    lcd_clear();
    menuHandlers[menuLevel](event);
    drawStatusLine();
  }
}

bool inPopupMenu = false;

void perMain()
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

#if defined(USB_MASS_STORAGE)
  if (usbPlugged()) {
    // disable access to menus
    lcdRefreshWait();
    lcd_clear();
    menuMainView(0);
    lcdRefresh();
    return;
  }
#endif

#if defined(LUA)
  // TODO better lua stopwatch
  uint32_t t0 = get_tmr10ms();
  static uint32_t lastLuaTime = 0;
  uint16_t interval = (lastLuaTime == 0 ? 0 : (t0 - lastLuaTime));
  lastLuaTime = t0;
  if (interval > maxLuaInterval) {
    maxLuaInterval = interval;
  }

  // run Lua scripts that don't use LCD (to use CPU time while LCD DMA is running)
  luaTask(0, RUN_MIX_SCRIPT | RUN_FUNC_SCRIPT | RUN_TELEM_BG_SCRIPT, false);

  t0 = get_tmr10ms() - t0;
  if (t0 > maxLuaDuration) {
    maxLuaDuration = t0;
  }
#endif //#if defined(LUA)

  // wait for LCD DMA to finish before continuing, because code from this point 
  // is allowed to change the contents of LCD buffer
  // 
  // WARNING: make sure no code above this line does any change to the LCD display buffer!
  //
  lcdRefreshWait();

  // get event
  uint8_t evt;
  if (menuEvent) {
    // we have a popupMenuActive entry or exit event 
    menuVerticalPosition = (menuEvent == EVT_ENTRY_UP) ? menuVerticalPositions[menuLevel] : 0;
    menuHorizontalPosition = 0;
    evt = menuEvent;
    if (menuEvent == EVT_ENTRY_UP) {
      TRACE("menuEvent EVT_ENTRY_UP");
    }
    else if (menuEvent == EVT_MENU_UP) {
      TRACE("menuEvent EVT_MENU_UP");
    }
    else if (menuEvent == EVT_ENTRY) {
      TRACE("menuEvent EVT_ENTRY");
    }
    else {
      TRACE("menuEvent 0x%02x", menuEvent);
    }
    menuEvent = 0;
    AUDIO_MENUS();
  }
  else {
    evt = getEvent(false);
    if (evt && (g_eeGeneral.backlightMode & e_backlight_mode_keys)) backlightOn(); // on keypress turn the light on
    checkBacklight();
#if defined(NAVIGATION_STICKS)
    uint8_t sticks_evt = getSticksNavigationEvent();
    if (sticks_evt) evt = sticks_evt;
#endif
  }

  if (warningText) {
    // show warning on top of the normal menus
    handleGui(0); // suppress events, they are handled by the warning
    DISPLAY_WARNING(evt);
  }
  else if (popupMenuNoItems > 0) {
    // popup menu is active display it on top of normal menus 
    handleGui(0); // suppress events, they are handled by the popup
    if (!inPopupMenu) {
      TRACE("Popup Menu started");
      inPopupMenu = true;
    }
    const char * result = displayPopupMenu(evt);
    if (result) {
      TRACE("popupMenuHandler(%s)", result);
      popupMenuHandler(result);
      putEvent(EVT_MENU_UP);
      // todo should we handle this event immediately??
      // handleGui(EVT_MENU_UP)
    }
  }
  else {
    // normal menus
    if (inPopupMenu) {
      TRACE("Popup Menu ended");
      inPopupMenu = false;
    }
    handleGui(evt);
  }

  lcdRefresh();

#if defined(REV9E) && !defined(SIMU)
  topLcdRefreshStart();
  setTopFirstTimer(getValue(MIXSRC_FIRST_TIMER+g_model.topLcdTimer));
  setTopSecondTimer(g_eeGeneral.globalTimer + sessionTimer);
  setTopRssi(TELEMETRY_RSSI());
  setTopBatteryValue(g_vbat100mV);
  setTopBatteryState(GET_TXBATT_BARS(), IS_TXBATT_WARNING());
  topLcdRefreshEnd();
#endif

#if defined(REV9E) && !defined(SIMU)
  bluetoothWakeup();
#endif

#if defined(PCBTARANIS)
  if (requestScreenshot) {
    requestScreenshot = false;
    writeScreenshot();
  }
#endif

}
