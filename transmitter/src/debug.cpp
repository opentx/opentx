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

#include "../opentx.h"
#include <stdio.h>
#include <stdarg.h>

#if !defined(SIMU)

Fifo<512> debugRxFifo;

// Outputs a string to the UART
void debugPuts(const char *format, ...)
{
  va_list arglist;
  char tmp[256];

  va_start(arglist, format);
  vsnprintf(tmp, 256, format, arglist);
  va_end(arglist);

  const char *t = tmp;
  while (*t) {
    debugPutc(*t++);
  }
}

void dump(unsigned char *data, unsigned int size)
{
  debugPuts("DUMP %d bytes ...\n\r", size);
  unsigned int i = 0, j=0;
  while (i*32+j < size) {
    debugPuts("%.2X ", data[i*32+j]);
    j++;
    if (j==32) {
      i++; j=0;
      debugPuts("\n\r");
    }
  }
  debugPuts("\n\r");
}

uint32_t Mem_address ;
uint32_t Next_mem_address ;
uint32_t Memaddmode ;

void crlf()
{
  debugPutc( 13 ) ;
  debugPutc( 10 ) ;
}

// Send a single 4 bit value to the RS232 port as a hex digit
void hex_digit_send( unsigned char c )
{
	c &= 0x0F ;
	if ( c > 9 )
	{
		c += 7 ;
	}
	c += '0' ;
	debugPutc( c ) ;
}

// Send the 8 bit value to the RS232 port as 2 hex digits
void p2hex( unsigned char c )
{
//	asm("swap %c") ;
	hex_digit_send( c >> 4 ) ;
//	asm("swap %c") ;
	hex_digit_send( c ) ;
}

// Send the 16 bit value to the RS232 port as 4 hex digits
void p4hex( uint16_t value )
{
	p2hex( value >> 8 ) ;
	p2hex( value ) ;
}

// Send the 32 bit value to the RS232 port as 8 hex digits
void p8hex( uint32_t value )
{
	p4hex( value >> 16 ) ;
	p4hex( value ) ;
}


static void dispw_256( register uint32_t address, register uint32_t lines )
{
	register uint32_t i ;
	register uint32_t j ;
	address &= 0xFFFFFFFC ;
	for ( i = 0 ; i < lines ; i += 1 )
	{
		p8hex( address ) ;
		for ( j = 0 ; j < 4 ; j += 1 )
		{
			debugPutc(' ') ;
			p8hex( *( (uint32_t *)address ) ) ;
			address += 4 ;
		}
		crlf() ;
	}
}

void debugTask(void* pdata)
{
  uint8_t rxchar ;

  TRACE("DEBUG Task started");

  crlf() ;
  dispw_256( (uint32_t)USART3, 4 ) ;

  for (;;) {
		
    while ( (USART3->SR & USART_SR_RXNE) == 0 )
      CoTickDelay(5); // 10ms
		
    rxchar = USART3->DR;

    if ( Memaddmode )
    {
      if ( ( rxchar >= 'a' ) && ( rxchar <= 'f' ) )
      {
        rxchar -= 0x20;		// toupper!
      }
      if ( ( ( rxchar >= '0' ) && ( rxchar <= '9' ) ) || ( ( rxchar >= 'A' ) && ( rxchar <= 'F' ) ) )
      {
        debugPutc( rxchar );
        rxchar -= '0';
        if ( rxchar > 9 )
        {
          rxchar -= 7;
        }
        Mem_address <<= 4;
        Mem_address |= rxchar;
      }
      else if ( rxchar == 13 )
      {
        crlf();
        if ( Mem_address == 0 )
        {
          Mem_address = Next_mem_address;
        }
        dispw_256( Mem_address, 4 );
        Next_mem_address = Mem_address + 64;
        Memaddmode = 0;
      }
      else if ( rxchar == 8 )
      {
        debugPutc( rxchar );
        debugPutc( rxchar );
        debugPutc( rxchar );
        Mem_address >>= 4;
      }
      else if ( rxchar == 27 )
      {
        crlf();
        Memaddmode = 0;
      }

    }

    if ( rxchar == '?' )
    {
      Memaddmode = 1;
      Mem_address = 0;
      debugPutc( '>' );
    }

    if ( rxchar == 'm' )
    {
      crlf();
      p8hex( (uint32_t) &g_model.moduleData[0] );
      debugPutc( ' ' );
      p8hex( (uint32_t) &g_model.moduleData[1] );
      crlf();
    }

  }
}
#endif
