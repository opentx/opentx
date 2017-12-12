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

#ifndef _TASKS_ARM_H_
#define _TASKS_ARM_H_

#if !defined(SIMU)
extern "C" {
#include <CoOS.h>
}
#endif

#define MENUS_STACK_SIZE       2000
#define MIXER_STACK_SIZE       500
#define AUDIO_STACK_SIZE       500
#define BLUETOOTH_STACK_SIZE   500

#if defined(_MSC_VER)
#define _ALIGNED(x) __declspec(align(x))
#elif defined(__GNUC__)
#define _ALIGNED(x) __attribute__ ((aligned(x)))
#endif

uint16_t getStackAvailable(void * address, uint16_t size);

template<int SIZE>
class TaskStack
{
  public:
    TaskStack() { }
    void paint();
    uint16_t size()
    {
      return SIZE * 4;
    }
    uint16_t available()
    {
      return getStackAvailable(stack, SIZE);
    }
    OS_STK stack[SIZE];
};

void stackPaint();
uint16_t stackSize();
uint16_t stackAvailable();

extern OS_TID menusTaskId;
// menus stack must be aligned to 8 bytes otherwise printf for %f does not work!
extern TaskStack<MENUS_STACK_SIZE> _ALIGNED(8) menusStack;

extern OS_TID mixerTaskId;
extern TaskStack<MIXER_STACK_SIZE> mixerStack;

extern OS_TID audioTaskId;
extern TaskStack<AUDIO_STACK_SIZE> audioStack;

void tasksStart();

extern volatile uint16_t timeForcePowerOffPressed;
inline void resetForcePowerOffRequest() {timeForcePowerOffPressed = 0; }

#endif // _TASKS_ARM_H_
