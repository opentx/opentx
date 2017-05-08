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

#if defined(PCBSTD) && defined(VOICE)
volatile uint8_t LcdLock;
#define LCD_LOCK() LcdLock = 1
#define LCD_UNLOCK() LcdLock = 0
#else
#define LCD_LOCK()
#define LCD_UNLOCK()
#endif

#if defined(LCD_KS108)
#include "targets/9x/lcd_ks108_driver.cpp"
#elif defined(LCD_ST7920)
#include "targets/9x/lcd_st7920_driver.cpp"
#else
#include "targets/9x/lcd_default_driver.cpp"
#endif
