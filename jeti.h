/*
 * Author - Karl Szmutny <shadow@privy.de>
 *
 * Based on the Code from Peter "woggle" Mack, mac@denich.net
 * -> http://svn.mikrokopter.de/filedetails.php?repname=Projects&path=/Transportables_Koptertool/trunk/jeti.c
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

#ifndef jeti_h
#define jeti_h


#include "gruvin9x.h"

#define JETI_KEY_LEFT		0x70
#define JETI_KEY_RIGHT		0xe0
#define JETI_KEY_UP		0xd0
#define JETI_KEY_DOWN		0xb0
#define JETI_KEY_NOCHANGE	0xf0

extern uint16_t jeti_keys;
extern uint8_t JetiBuffer[32]; // 32 characters
extern uint8_t JetiBufferReady;

void JETI_Init(void);
void JETI_DisableTXD (void);
void JETI_EnableTXD (void);
void JETI_DisableRXD (void);
void JETI_EnableRXD (void);

void JETI_putw (uint16_t c);
void JETI_putc (uint8_t c);
void JETI_puts (char *s);
void JETI_put_start (void);
void JETI_put_stop (void);

void menuProcJeti(uint8_t event);

#endif

