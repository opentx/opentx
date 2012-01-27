/*
 * Author - Philip Moss
 * Adapted from frsky.h code by Jean-Pierre PARISY
 * 
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

#ifndef nmea_h
#define nmea_h

void NMEA_Init(void);
void NMEA_EnableRXD (void);
void menuProcNMEA(uint8_t event);

#endif

