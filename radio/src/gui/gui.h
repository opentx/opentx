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

#ifndef _GUI_H_
#define _GUI_H_

#if defined(CPUARM)
#include "gui_helpers.h"
#endif

#if defined(PCBHORUS)
  #include "horus/gui.h"
#elif defined(PCBFLAMENCO)
  #include "flamenco/gui.h"
#elif defined(PCBTARANIS)
  #include "taranis/gui.h"
#else
  #include "9x/gui.h"
#endif

#if defined(SIMU)
extern bool simuLcdRefresh;
extern display_t simuLcdBuf[DISPLAY_BUFFER_SIZE];
#endif

#endif // _GUI_H_
