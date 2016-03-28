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

#ifndef _CLI_H_
#define _CLI_H_

#include "serial.h"

extern uint8_t cliTracesEnabled;

#ifdef __cplusplus
#include "fifo.h"
extern Fifo<uint8_t, 256> cliRxFifo;
#include "tasks_arm.h"
#define CLI_STACK_SIZE                 1000
extern OS_TID cliTaskId;
extern TaskStack<CLI_STACK_SIZE> cliStack;
#endif

void cliStart();
void cliStackPaint();
uint32_t cliStackFree();

#endif // _CLI_H_
