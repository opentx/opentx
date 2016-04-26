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

#ifndef _CROSSFIRE_H_
#define _CROSSFIRE_H_

// Device address
#define BROADCAST_ADDRESS              0x00

// Frame id
#define GPS_ID                         0x02
#define BATTERY_ID                     0x08
#define LINK_ID                        0x14
#define CHANNELS_ID                    0x16

void processCrossfireTelemetryData(uint8_t data);
void crossfireSetDefault(int index, uint8_t id, uint8_t subId);

#endif // _CROSSFIRE_H_
