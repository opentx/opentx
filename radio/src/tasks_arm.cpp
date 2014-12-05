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
uint8_t requiredSpeakerVolume;

extern void batteryCheck();

void perMainArm()
{
#if defined(PCBSKY9X)
  Current_accumulator += Current_analogue ;
  static uint32_t OneSecTimer;
  if (++OneSecTimer >= 100) {
    OneSecTimer -= 100 ;
    Current_used += Current_accumulator / 100 ;                     // milliAmpSeconds (but scaled)
    Current_accumulator = 0 ;
  }
#endif

  if (currentSpeakerVolume != requiredSpeakerVolume) {
    currentSpeakerVolume = requiredSpeakerVolume;
    setVolume(currentSpeakerVolume);
  }

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

  sdMountPoll();
  writeLogs();

  uint8_t evt = getEvent(false);
  if (evt && (g_eeGeneral.backlightMode & e_backlight_mode_keys)) backlightOn(); // on keypress turn the light on
  checkBacklight();

#if defined(PCBTARANIS)
  checkTrainerSettings();
#endif

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

#if defined(NAVIGATION_STICKS)
  uint8_t sticks_evt = getSticksNavigationEvent();
  if (sticks_evt) evt = sticks_evt;
#endif

#if defined(USB_MASS_STORAGE)
  if (usbPlugged()) {
    lcd_clear();
    menuMainView(0);
  }
  else 
#endif
  {
    const char *warn = s_warning;
    uint8_t menu = s_menu_count;

    if (!LUA_STANDALONE_SCRIPT_RUNNING()) {
      lcd_clear();
      if (menuEvent) {
        m_posVert = menuEvent == EVT_ENTRY_UP ? g_menuPos[g_menuStackPtr] : 0;
        m_posHorz = 0;
        evt = menuEvent;
        menuEvent = 0;
        AUDIO_MENUS();
      }
      g_menuStack[g_menuStackPtr]((warn || menu) ? 0 : evt);
    }

#if defined(LUA)
    luaTask(evt);
#endif

    if (!LUA_STANDALONE_SCRIPT_RUNNING()) {
      if (warn) DISPLAY_WARNING(evt);
      if (menu) {
        const char * result = displayMenu(evt);
        if (result) {
          menuHandler(result);
          putEvent(EVT_MENU_UP);
        }
      }
    }
  }

  drawStatusLine();

  lcdRefresh();

  if (SLAVE_MODE()) {
    JACK_PPM_OUT();
  }
  else {
    JACK_PPM_IN();
  }

  batteryCheck();
}

#define MENUS_STACK_SIZE    2000
#define MIXER_STACK_SIZE    500
#define AUDIO_STACK_SIZE    500
#define BT_STACK_SIZE       500
#define DEBUG_STACK_SIZE    500

#if defined(_MSC_VER)
  #define _ALIGNED(x) __declspec(align(x))
#elif defined(__GNUC__)
  #define _ALIGNED(x) __attribute__ ((aligned(x)))
#endif

OS_TID menusTaskId;
// stack must be aligned to 8 bytes otherwise printf for %f does not work!
OS_STK _ALIGNED(8) menusStack[MENUS_STACK_SIZE];

OS_TID mixerTaskId;
OS_STK mixerStack[MIXER_STACK_SIZE];

OS_TID audioTaskId;
OS_STK audioStack[AUDIO_STACK_SIZE];

#if defined(BLUETOOTH)
OS_TID btTaskId;
OS_STK btStack[BT_STACK_SIZE];
#endif

#if defined(DEBUG)
OS_TID debugTaskId;
OS_STK debugStack[DEBUG_STACK_SIZE];
#endif

OS_MutexID audioMutex;
OS_MutexID mixerMutex;

void stack_paint()
{
  for (uint32_t i=0; i<MENUS_STACK_SIZE; i++)
    menusStack[i] = 0x55555555;
  for (uint32_t i=0; i<MIXER_STACK_SIZE; i++)
    mixerStack[i] = 0x55555555;
  for (uint32_t i=0; i<AUDIO_STACK_SIZE; i++)
    audioStack[i] = 0x55555555;
}

uint32_t stack_free(uint32_t tid)
{
  OS_STK *stack;
  uint32_t size;

  switch(tid) {
    case 0:
      stack = menusStack;
      size = MENUS_STACK_SIZE;
      break;
    case 1:
      stack = mixerStack;
      size = MIXER_STACK_SIZE;
      break;
    case 2:
      stack = audioStack;
      size = AUDIO_STACK_SIZE;
      break;
#if 0 // defined(PCBTARANIS) && !defined(SIMU)
    case 255:
      // main stack
      stack = (OS_STK *)&_main_stack_start;
      size = ((unsigned char *)&_estack - (unsigned char *)&_main_stack_start) / 4;
      break;
#endif
    default:
      return 0;
  }

  uint32_t i=0;
  for (; i<size; i++)
    if (stack[i] != 0x55555555)
      break;
  return i*4;
}

#if !defined(SIMU)

void mixerTask(void * pdata)
{
  s_pulses_paused = true;

  while(1) {

    if (!s_pulses_paused) {
      uint16_t t0 = getTmr2MHz();

      CoEnterMutexSection(mixerMutex);
      doMixerCalculations();
      CoLeaveMutexSection(mixerMutex);

#if defined(FRSKY) || defined(MAVLINK)
      telemetryWakeup();
#endif

      if (heartbeat == HEART_WDT_CHECK) {
        wdt_reset();
        heartbeat = 0;
      }

      t0 = getTmr2MHz() - t0;
      if (t0 > maxMixerDuration) maxMixerDuration = t0 ;
    }

    CoTickDelay(1);  // 2ms for now
  }
}

#define MENU_TASK_PERIOD_TICKS      10    // 20ms

extern void opentxClose();
extern void opentxInit();

void menusTask(void * pdata)
{
  opentxInit();

  while (pwrCheck() != e_power_off) {
    U64 start = CoGetOSTime();
    perMainArm();
    // TODO remove completely massstorage from sky9x firmware
    U32 runtime = (U32)(CoGetOSTime() - start);
    if (runtime >= MENU_TASK_PERIOD_TICKS) {
      //no delay
    }
    else {
      CoTickDelay(MENU_TASK_PERIOD_TICKS - runtime);  // 5*2ms for now
    }
  }

  lcd_clear();
  displayPopup(STR_SHUTDOWN);

  opentxClose();

  lcd_clear();
  lcdRefresh();
  lcdOff();

#if !defined(SIMU)
  SysTick->CTRL = 0; // turn off systick
#endif

  pwrOff(); // Only turn power off if necessary
}

extern void audioTask(void* pdata);

void tasksStart()
{
  CoInitOS();

#if defined(CPUARM) && defined(DEBUG) && !defined(SIMU)
  debugTaskId = CoCreateTaskEx(debugTask, NULL, 10, &debugStack[DEBUG_STACK_SIZE-1], DEBUG_STACK_SIZE, 1, false);
#endif

#if defined(BLUETOOTH)
  btTaskId = CoCreateTask(btTask, NULL, 15, &btStack[BT_STACK_SIZE-1], BT_STACK_SIZE);
#endif

  mixerTaskId = CoCreateTask(mixerTask, NULL, 5, &mixerStack[MIXER_STACK_SIZE-1], MIXER_STACK_SIZE);
  menusTaskId = CoCreateTask(menusTask, NULL, 10, &menusStack[MENUS_STACK_SIZE-1], MENUS_STACK_SIZE);
  audioTaskId = CoCreateTask(audioTask, NULL, 7, &audioStack[AUDIO_STACK_SIZE-1], AUDIO_STACK_SIZE);

#if !defined(SIMU)
  audioMutex = CoCreateMutex();
  mixerMutex = CoCreateMutex();
#endif

  CoStartOS();
}

#endif // #if !defined(SIMU)