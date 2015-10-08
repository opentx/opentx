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

#define MENUS_STACK_SIZE       2000
#define MIXER_STACK_SIZE       500
#define AUDIO_STACK_SIZE       500
#define BLUETOOTH_STACK_SIZE   500

#if defined(_MSC_VER)
  #define _ALIGNED(x) __declspec(align(x))
#elif defined(__GNUC__)
  #define _ALIGNED(x) __attribute__ ((aligned(x)))
#endif

OS_TID menusTaskId;
// menus stack must be aligned to 8 bytes otherwise printf for %f does not work!
TaskStack<MENUS_STACK_SIZE> _ALIGNED(8) menusStack;

OS_TID mixerTaskId;
TaskStack<MENUS_STACK_SIZE> mixerStack;

OS_TID audioTaskId;
TaskStack<AUDIO_STACK_SIZE> audioStack;

#if defined(BLUETOOTH)
OS_TID btTaskId;
TaskStack<BLUETOOTH_STACK_SIZE> bluetoothStack;
#endif

OS_MutexID audioMutex;
OS_MutexID mixerMutex;

enum TaskIndex {
  MENU_TASK_INDEX,
  MIXER_TASK_INDEX,
  AUDIO_TASK_INDEX,
  CLI_TASK_INDEX,
  BLUETOOTH_TASK_INDEX,
  TASK_INDEX_COUNT,
  MAIN_TASK_INDEX = 255
};

template<int SIZE>
void TaskStack<SIZE>::paint()
{
  for (uint32_t i=0; i<SIZE; i++) {
    stack[i] = 0x55555555;
  }
}

template<int SIZE>
uint16_t TaskStack<SIZE>::size()
{
  return SIZE*4;
}

uint16_t getStackAvailable(void * address, uint16_t size)
{
  uint32_t * array = (uint32_t *)address;
  uint16_t i = 0;
  while (i < size && array[i] == 0x55555555) {
    i++;
  }
  return i*4;
#if defined(CLI)
  cliStackPaint();
#endif
}

template<int SIZE>
uint16_t TaskStack<SIZE>::available()
{
  return getStackAvailable(stack, SIZE);
}

void stackPaint()
{
  menusStack.paint();
  mixerStack.paint();
  audioStack.paint();
#if defined(CLI)
  cliStack.paint();
#endif
}

#if defined(CPUSTM32) && !defined(SIMU)
uint16_t stackSize()
{
  return ((unsigned char *)&_estack - (unsigned char *)&_main_stack_start) / 4;
}

uint16_t stackAvailable()
{
  return getStackAvailable(&_main_stack_start, stackSize());
}
#endif

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

void menusTask(void * pdata)
{
  opentxInit();

#if defined(PCBTARANIS) && defined(REV9E)
  while (1) {
    uint32_t pwr_check = pwrCheck();
    if (pwr_check == e_power_off) {
      break;
    }
    else if (pwr_check == e_power_press) {
      continue;
    }
#else
  while (pwrCheck() != e_power_off) {
#endif
    U64 start = CoGetOSTime();
    perMain();
    // TODO remove completely massstorage from sky9x firmware
    U32 runtime = (U32)(CoGetOSTime() - start);
    // deduct the thread run-time from the wait, if run-time was more than 
    // desired period, then skip the wait all together
    if (runtime < MENU_TASK_PERIOD_TICKS) {
      CoTickDelay(MENU_TASK_PERIOD_TICKS - runtime);
    }
  }

#if defined(PCBTARANIS) && defined(REV9E)
  topLcdOff();
#endif

  BACKLIGHT_OFF();

#if defined(COLORLCD)
#elif defined(PCBTARANIS)
  displaySleepBitmap();
#else
  lcd_clear();
  displayPopup(STR_SHUTDOWN);
#endif

  opentxClose();
  boardOff(); // Only turn power off if necessary
}

extern void audioTask(void* pdata);

void tasksStart()
{
  CoInitOS();

#if defined(CLI)
  cliStart();
#endif

#if defined(BLUETOOTH)
  btTaskId = CoCreateTask(btTask, NULL, 15, &bluetoothStack.stack[BLUETOOTH_STACK_SIZE-1], BLUETOOTH_STACK_SIZE);
#endif

  mixerTaskId = CoCreateTask(mixerTask, NULL, 5, &mixerStack.stack[MIXER_STACK_SIZE-1], MIXER_STACK_SIZE);
  menusTaskId = CoCreateTask(menusTask, NULL, 10, &menusStack.stack[MENUS_STACK_SIZE-1], MENUS_STACK_SIZE);
  audioTaskId = CoCreateTask(audioTask, NULL, 7, &audioStack.stack[AUDIO_STACK_SIZE-1], AUDIO_STACK_SIZE);

#if !defined(SIMU)
  audioMutex = CoCreateMutex();
  mixerMutex = CoCreateMutex();
#endif

  CoStartOS();
}

#endif // #if !defined(SIMU)
