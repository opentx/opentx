/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"

uint8_t currentSpeakerVolume = 255;
uint8_t requiredSpeakerVolume = 255;
uint8_t requestScreenshot = false;

void handleUsbConnection()
{
#if defined(CPUSTM32) && !defined(SIMU)
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
  
#endif // defined(CPUSTM32) && !defined(SIMU)
}

void checkSpeakerVolume()
{
  if (currentSpeakerVolume != requiredSpeakerVolume) {
    currentSpeakerVolume = requiredSpeakerVolume;
    setScaledVolume(currentSpeakerVolume);
  }
}

#if defined(EEPROM)
void checkEeprom()
{
  if (!usbPlugged()) {
    if (eepromIsWriting())
      eepromWriteProcess();
    else if (TIME_TO_WRITE())
      storageCheck(false);
  }
}
#else
void checkEeprom()
{
  if (TIME_TO_WRITE())
    storageCheck(false);
}
#endif

#if defined(GUI) && defined(COLORLCD)
void guiMain(evt_t evt)
{
  bool refreshNeeded = false;

#if defined(LUA)
  uint32_t t0 = get_tmr10ms();
  static uint32_t lastLuaTime = 0;
  uint16_t interval = (lastLuaTime == 0 ? 0 : (t0 - lastLuaTime));
  lastLuaTime = t0;
  if (interval > maxLuaInterval) {
    maxLuaInterval = interval;
  }

  // run Lua scripts that don't use LCD (to use CPU time while LCD DMA is running)
  luaTask(0, RUN_MIX_SCRIPT | RUN_FUNC_SCRIPT | RUN_TELEM_BG_SCRIPT, false);

  // wait for LCD DMA to finish before continuing, because code from this point 
  // is allowed to change the contents of LCD buffer
  // 
  // WARNING: make sure no code above this line does any change to the LCD display buffer!
  //
  lcdRefreshWait();

  // draw LCD from menus or from Lua script
  // run Lua scripts that use LCD

  bool standaloneScriptWasRun = luaTask(evt, RUN_STNDAL_SCRIPT, true);
  if (!standaloneScriptWasRun) {
    luaTask(evt, RUN_TELEM_FG_SCRIPT, true);
    refreshNeeded = true;
  }

  t0 = get_tmr10ms() - t0;
  if (t0 > maxLuaDuration) {
    maxLuaDuration = t0;
  }
#else
  lcdRefreshWait();   // WARNING: make sure no code above this line does any change to the LCD display buffer!
  const bool standaloneScriptWasRun = false;
#endif

  if (!standaloneScriptWasRun) {
    while (1) {
      // normal GUI from menus
      const char * warn = warningText;
      uint8_t menu = popupMenuNoItems;

      static bool popupDisplayed = false;
      if (warn || menu) {
        if (popupDisplayed == false) {
          menuHandlers[menuLevel](EVT_REFRESH);
          lcdDrawBlackOverlay();
          TIME_MEASURE_START(storebackup);
          lcdStoreBackupBuffer();
          TIME_MEASURE_STOP(storebackup);
        }
        if (popupDisplayed == false || evt) {
          popupDisplayed = lcdRestoreBackupBuffer();
          if (warn) DISPLAY_WARNING(evt);
          if (menu) {
            const char * result = displayPopupMenu(evt);
            if (result) {
              popupMenuHandler(result);
              evt = EVT_REFRESH;
              continue;
            }
          }
          refreshNeeded = true;
        }
      }
      else {
        if (popupDisplayed) {
          if (evt == 0) {
            evt = EVT_REFRESH;
          }
          popupDisplayed = false;
        }
        refreshNeeded = menuHandlers[menuLevel](evt);
      }

      if (menuEvent == EVT_ENTRY) {
        menuVerticalPosition = -1;
        menuHorizontalPosition = 0;
        evt = menuEvent;
        menuEvent = 0;
      }
      else if (menuEvent == EVT_ENTRY_UP) {
        menuVerticalPosition = menuVerticalPositions[menuLevel];
        menuHorizontalPosition = 0;
        evt = menuEvent;
        menuEvent = 0;
      }
      else {
        break;
      }
    }
  }

  if (refreshNeeded) {
    lcdRefresh();
  }
}
#elif defined(GUI)

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
    lcdClear();
    menuHandlers[menuLevel](event);
    drawStatusLine();
  }
}

bool inPopupMenu = false;

void guiMain(evt_t evt)
{
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

  if (menuEvent) {
    // we have a popupMenuActive entry or exit event 
    menuVerticalPosition = (menuEvent == EVT_ENTRY_UP) ? menuVerticalPositions[menuLevel] : 0;
    menuHorizontalPosition = 0;
    evt = menuEvent;
    if (menuEvent == EVT_ENTRY_UP) {
      TRACE("menuEvent EVT_ENTRY_UP");
    }
    // else if (menuEvent == EVT_MENU_UP) {
    //   TRACE("menuEvent EVT_MENU_UP");
    // }
    else if (menuEvent == EVT_ENTRY) {
      TRACE("menuEvent EVT_ENTRY");
    }
    else {
      TRACE("menuEvent 0x%02x", menuEvent);
    }
    menuEvent = 0;
    AUDIO_MENUS();
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
}
#endif

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

  evt_t evt = getEvent(false);
  if (evt && (g_eeGeneral.backlightMode & e_backlight_mode_keys)) backlightOn(); // on keypress turn the light on
  doLoopCommonActions();
#if defined(NAVIGATION_STICKS)
  uint8_t sticks_evt = getSticksNavigationEvent();
  if (sticks_evt) evt = sticks_evt;
#endif

#if defined(USB_MASS_STORAGE)
  if (usbPlugged()) {
    // disable access to menus
    lcdClear();
    menuMainView(0);
    lcdRefresh();
    return;
  }
#endif

#if defined(GUI)
  guiMain(evt);
#endif

#if defined(PCBTARANIS)
  if (requestScreenshot) {
    requestScreenshot = false;
    writeScreenshot();
  }
#endif

#if defined(PCBTARANIS) && defined(REV9E) && !defined(SIMU)
  toplcdRefreshStart();
  setTopFirstTimer(getValue(MIXSRC_FIRST_TIMER+g_model.toplcdTimer));
  setTopSecondTimer(g_eeGeneral.globalTimer + sessionTimer);
  setTopRssi(TELEMETRY_RSSI());
  setTopBatteryValue(g_vbat100mV);
  setTopBatteryState(GET_TXBATT_BARS(), IS_TXBATT_WARNING());
  toplcdRefreshEnd();
#endif

#if defined(PCBTARANIS) && defined(REV9E) && !defined(SIMU)
  bluetoothWakeup();
#endif

}
