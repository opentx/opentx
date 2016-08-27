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

#ifndef _JETI_H_
#define _JETI_H_

#define JETI_KEY_LEFT		0x70
#define JETI_KEY_RIGHT		0xe0
#define JETI_KEY_UP		0xd0
#define JETI_KEY_DOWN		0xb0
#define JETI_KEY_NOCHANGE	0xf0

void JETI_Init(void);
void JETI_EnableRXD (void);
void JETI_DisableRXD (void);
void JETI_EnableTXD (void);
void JETI_DisableTXD (void);

void telemetryInterrupt10ms();
void menuViewTelemetryJeti(event_t event);

#endif // _JETI_H_

