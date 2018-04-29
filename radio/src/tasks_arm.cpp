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

OS_TID menusTaskId;
// menus stack must be aligned to 8 bytes otherwise printf for %f does not work!
TaskStack<MENUS_STACK_SIZE> _ALIGNED(8) menusStack;

OS_TID mixerTaskId;
TaskStack<MIXER_STACK_SIZE> mixerStack;

OS_TID audioTaskId;
TaskStack<AUDIO_STACK_SIZE> audioStack;

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

void stackPaint()
{
  menusStack.paint();
  mixerStack.paint();
  audioStack.paint();
#if defined(CLI)
  cliStack.paint();
#endif
}

#if defined(STM32) && !defined(SIMU)
uint16_t stackSize()
{
  return ((unsigned char *)&_estack - (unsigned char *)&_main_stack_start) / 4;
}

uint16_t stackAvailable()
{
  return getStackAvailable(&_main_stack_start, stackSize());
}
#endif

volatile uint16_t timeForcePowerOffPressed = 0;

bool isForcePowerOffRequested()
{
  if (pwrOffPressed()) {
    if (timeForcePowerOffPressed == 0) {
      timeForcePowerOffPressed = get_tmr10ms();
    }
    else {
      uint16_t delay = (uint16_t)get_tmr10ms() - timeForcePowerOffPressed;
      if (delay > 1000/*10s*/) {
        return true;
      }
    }
  }
  else {
    resetForcePowerOffRequest();
  }
  return false;
}

uint32_t nextMixerTime[NUM_MODULES];

void mixerTask(void * pdata)
{
  static uint32_t lastRunTime;
  s_pulses_paused = true;

  while(1) {

#if defined(SIMU)
    if (main_thread_running == 0)
      return;
#endif

#if defined(SBUS)
    processSbusInput();
#endif

    CoTickDelay(1);

    if (isForcePowerOffRequested()) {
      pwrOff();
    }

    uint32_t now = CoGetOSTime();
    bool run = false;
#if !defined(SIMU) && defined(STM32)
    if ((now - lastRunTime) >= (usbStarted() ? 5 : 10)) {     // run at least every 20ms (every 10ms if USB is active)
#else
    if ((now - lastRunTime) >= 10) {     // run at least every 20ms
#endif
      run = true;
    }
    else if (now == nextMixerTime[0]) {
      run = true;
    }
#if NUM_MODULES >= 2
    else if (now == nextMixerTime[1]) {
      run = true;
    }
#endif
    if (!run) {
      continue;  // go back to sleep
    }

    lastRunTime = now;

    if (!s_pulses_paused) {
      uint16_t t0 = getTmr2MHz();

      DEBUG_TIMER_START(debugTimerMixer);
      CoEnterMutexSection(mixerMutex);
      doMixerCalculations();
      DEBUG_TIMER_START(debugTimerMixerCalcToUsage);
      DEBUG_TIMER_SAMPLE(debugTimerMixerIterval);
      CoLeaveMutexSection(mixerMutex);
      DEBUG_TIMER_STOP(debugTimerMixer);

#if defined(STM32) && !defined(SIMU)
      if (getSelectedUsbMode() == USB_JOYSTICK_MODE) {
        usbJoystickUpdate();
      }
#endif

#if defined(TELEMETRY_FRSKY) || defined(TELEMETRY_MAVLINK)
      DEBUG_TIMER_START(debugTimerTelemetryWakeup);
      telemetryWakeup();
      DEBUG_TIMER_STOP(debugTimerTelemetryWakeup);
#endif

#if defined(BLUETOOTH)
      bluetoothWakeup();
#endif

      if (heartbeat == HEART_WDT_CHECK) {
        wdt_reset();
        heartbeat = 0;
      }

      t0 = getTmr2MHz() - t0;
      if (t0 > maxMixerDuration) maxMixerDuration = t0 ;
    }
  }
}

void scheduleNextMixerCalculation(uint8_t module, uint16_t period_ms)
{
  // Schedule next mixer calculation time,
  // for now assume mixer calculation takes 2 ms.
  nextMixerTime[module] = (uint32_t)CoGetOSTime() + period_ms / 2 - 1/*2ms*/;
  DEBUG_TIMER_STOP(debugTimerMixerCalcToUsage);
}

#define MENU_TASK_PERIOD_TICKS      25    // 50ms

#if defined(COLORLCD) && defined(CLI)
bool perMainEnabled = true;
#endif

void menusTask(void * pdata)
{
  opentxInit();

#if defined(PWR_BUTTON_PRESS)
  while (1) {
    uint32_t pwr_check = pwrCheck();
    if (pwr_check == e_power_off) {
      break;
    }
    else if (pwr_check == e_power_press) {
      CoTickDelay(MENU_TASK_PERIOD_TICKS);
      continue;
    }
#else
  while (pwrCheck() != e_power_off) {
#endif
    uint32_t start = (uint32_t)CoGetOSTime();
    DEBUG_TIMER_START(debugTimerPerMain);
#if defined(COLORLCD) && defined(CLI)
    if (perMainEnabled) {
      perMain();
    }
#else
    perMain();
#endif
    DEBUG_TIMER_STOP(debugTimerPerMain);
    // TODO remove completely massstorage from sky9x firmware
    uint32_t runtime = ((uint32_t)CoGetOSTime() - start);
    // deduct the thread run-time from the wait, if run-time was more than
    // desired period, then skip the wait all together
    if (runtime < MENU_TASK_PERIOD_TICKS) {
      CoTickDelay(MENU_TASK_PERIOD_TICKS - runtime);
    }

    resetForcePowerOffRequest();

#if defined(SIMU)
    if (main_thread_running == 0)
      break;
#endif
  }

#if defined(PCBX9E)
  toplcdOff();
#endif

#if defined(PCBHORUS)
  ledOff();
#endif

  drawSleepBitmap();
  opentxClose();
  boardOff(); // Only turn power off if necessary
}

void tasksStart()
{
  CoInitOS();

#if defined(CLI)
  cliStart();
#endif

  mixerTaskId = CoCreateTask(mixerTask, NULL, 5, &mixerStack.stack[MIXER_STACK_SIZE-1], MIXER_STACK_SIZE);
  menusTaskId = CoCreateTask(menusTask, NULL, 10, &menusStack.stack[MENUS_STACK_SIZE-1], MENUS_STACK_SIZE);

#if !defined(SIMU)
  // TODO move the SIMU audio in this task
  audioTaskId = CoCreateTask(audioTask, NULL, 7, &audioStack.stack[AUDIO_STACK_SIZE-1], AUDIO_STACK_SIZE);
#endif

  audioMutex = CoCreateMutex();
  mixerMutex = CoCreateMutex();

  CoStartOS();
}
