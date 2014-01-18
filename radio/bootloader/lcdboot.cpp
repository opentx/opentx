/*
 * Author - Mike Blandford
 *
 * Based on er9x by Erez Raviv <erezraviv@gmail.com>
 *
 * Based on th9x -> http://code.google.com/p/th9x/
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

// This version for ARM based ERSKY9X board

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef PCBSKY
#include "AT91SAM3S4.h"
#include "radio.h"
//#include "myeeprom.h"
#endif

#ifdef PCBX9D
 #include "radio.h"
#endif

#include "lcd.h"
#include "drivers.h"
#include "logicio.h"

#include "font.lbm"
#define font_5x8_x20_x7f (font)

#include "font_dblsize.lbm"
#define font_10x16_x20_x7f (font_dblsize)

// Local data
uint8_t OptrexDisplay = 0 ;
uint8_t Lcd_lastPos ;
uint8_t DisplayBuf[DISPLAY_W*DISPLAY_H/8] ;
#define DISPLAY_END (DisplayBuf+sizeof(DisplayBuf))

#ifdef PCBX9D
#define X9D_OFFSET		0
#define DISPLAY_START (DisplayBuf + X9D_OFFSET)
#else
#define DISPLAY_START (DisplayBuf + 0)
#endif 



/// invers: 0 no 1=yes 2=blink
void lcd_putc(uint8_t x,uint8_t y,const char c )
{
  lcd_putcAtt(x,y,c,0);
}

uint8_t lcd_putcAtt(uint8_t x,uint8_t y,const char c,uint8_t mode)
{
	register int32_t i ;
	register uint8_t *p    = &DISPLAY_START[ y / 8 * DISPLAY_W + x ];
    //uint8_t *pmax = &displayBuf[ DISPLAY_H/8 * DISPLAY_W ];
	if ( c < 22 )		// Move to specific x position (c)*FW
	{
		x = c*FW ;
  	if(mode&DBLSIZE)
		{
			x += x ;
		}
		return x ;
	}
	x += FW ;
  register uint8_t    *q ;
	if( c < 0xC0 )
	{
		q = (uint8_t *) &font_5x8_x20_x7f[(c-0x20)*5] ;
	}
	else
	{
			q = (uint8_t *) &font_5x8_x20_x7f[0] ;
	}
  register bool   inv = (mode & INVERS) ? true : false ;
  
	if(mode&DBLSIZE)
  {
		if ( (c!=0x2E)) x+=FW; //check for decimal point
	/* each letter consists of ten top bytes followed by
	 * five bottom by ten bottom bytes (20 bytes per 
	 * char) */
	  unsigned char c_mapped ;
		if( c < 0xC0 )
		{
			if ( c == 'B' )
			{
				c_mapped = 1 ;
			}
			else if ( c == 'S' )
			{
				c_mapped = 2 ;
			}
			else if ( c == 'U' )
			{
				c_mapped = 3 ;
			}
			else
			{
				c_mapped = 0 ;
			}
			q = (uint8_t *) &font_10x16_x20_x7f[(c_mapped)*20] ;
//  	  q = (uint8_t *) &font_10x16_x20_x7f[(c-0x20)*10 + ((c-0x20)/16)*160];
		}
		else
		{
				q = (uint8_t *) &font_10x16_x20_x7f[0] ;
		}
    for( i=5 ; i>=0 ; i-- )
		{
			uint8_t b1 ;
			uint8_t b3 ;
			uint8_t b2 ;
			uint8_t b4 ;

			if( c < 0xC0 )
			{
	    	/*top byte*/
      	b1 = i>0 ? *q : 0;
	    	/*bottom byte*/
      	b3 = i>0 ? *(q+10) : 0;
	    	/*top byte*/
      	b2 = i>0 ? *(++q) : 0;
	    	/*bottom byte*/
      	b4 = i>0 ? *(q+10) : 0;
      	q++;
			}
			else
			{
	    	/*top byte*/
      	b1 = i>0 ? *q++ : 0 ;
	    	/*bottom byte*/
      	b3 = i>0 ? *q++ : 0 ;
	    	/*top byte*/
      	b2 = i>0 ? *q++ : 0 ;
	    	/*bottom byte*/
      	b4 = i>0 ? *q++ : 0 ;
			}
      if(inv)
			{
		    b1=~b1;
		    b2=~b2;
		    b3=~b3;
		    b4=~b4;
      }

      if(&p[DISPLAY_W+1] < DISPLAY_END)
			{
        p[0]=b1;
        p[1]=b2;
        p[DISPLAY_W] = b3;
        p[DISPLAY_W+1] = b4;
        p+=2;
      }
    }
  }
  else
  {
    uint8_t condense=0;

    if (mode & CONDENSED)
		{
      *p++ = inv ? ~0 : 0;
      condense=1;
    	x += FWNUM-FW ;
		}

    for( i=5 ; i!=0 ; i-- )
		{
      uint8_t b = *q++ ;
  	  if (condense && i==4)
			{
        /*condense the letter by skipping column 4 */
        continue;
      }
      if(p<DISPLAY_END) *p++ = inv ? ~b : b;
    }
    if(p<DISPLAY_END) *p++ = inv ? ~0 : 0;
  }
	return x ;
}

void lcd_putsnAtt(uint8_t x,uint8_t y, const char * s,uint8_t len,uint8_t mode)
{
	register char c ;
//	register uint8_t size ;
//	size = mode & DBLSIZE ;
  while(len!=0) {
    c = *s++ ;
		if ( c == 0 )
		{
			break ;			
		}

    x = lcd_putcAtt(x,y,c,mode);
    len--;
  }
}

void lcd_putsn_P(uint8_t x,uint8_t y,const char * s,uint8_t len)
{
  lcd_putsnAtt( x,y,s,len,0);
}

uint8_t lcd_putsAtt( uint8_t x, uint8_t y, const char *s, uint8_t mode )
{
  while(1)
	{
    char c = *s++ ;
    if(!c) break ;
    x = lcd_putcAtt(x,y,c,mode) ;
  }
  return x;
}

void lcd_puts_Pleft( uint8_t y, const char *s )
{
  lcd_putsAtt( 0, y, s, 0);
}

void lcd_puts_P( uint8_t x, uint8_t y, const char *s )
{
  lcd_putsAtt( x, y, s, 0);
}


void lcd_outhex4(uint8_t x,uint8_t y,uint16_t val)
{
  register int i ;
  x+=FWNUM*4;
  for(i=0; i<4; i++)
  {
    x-=FWNUM;
    char c = val & 0xf;
    c = c>9 ? c+'A'-10 : c+'0';
    lcd_putcAtt(x,y,c,c>='A'?CONDENSED:0);
    val>>=4;
  }
}
uint8_t plotType = PLOT_XOR ;

void lcd_write_bits( uint8_t *p, uint8_t mask )
{
  if(p<DISPLAY_END)
	{
		uint8_t temp = *p ;
		if ( plotType != PLOT_XOR )
		{
			temp |= mask ;
		}
		if ( plotType != PLOT_BLACK )
		{
			temp ^= mask ;
		}
		*p = temp ;
	}
}

void lcd_plot( register uint8_t x, register uint8_t y )
{
  register uint8_t *p   = &DISPLAY_START[ y / 8 * DISPLAY_W + x ];
	lcd_write_bits( p, BITMASK(y%8) ) ;
}

void lcd_hlineStip( unsigned char x, unsigned char y, signed char w, uint8_t pat )
{
  if(w<0) {x+=w; w=-w;}
  register uint8_t *p  = &DISPLAY_START[ y / 8 * DISPLAY_W + x ];
  register uint8_t msk = BITMASK(y%8);
  while(w)
	{
    if ( p>=DISPLAY_END)
    {
      break ;			
    }
    if(pat&1)
		{
			lcd_write_bits( p, msk ) ;
      pat = (pat >> 1) | 0x80;
    }
		else
		{
      pat = pat >> 1;
    }
    w--;
    p++;
  }
}


// Reverse video 8 pixels high, w pixels wide
// Vertically on an 8 pixel high boundary
void lcd_char_inverse( uint8_t x, uint8_t y, uint8_t w, uint8_t blink )
{
//	if ( blink && BLINK_ON_PHASE )
//	{
//		return ;
//	}
	uint8_t end = x + w ;
  uint8_t *p = &DISPLAY_START[ y / 8 * DISPLAY_W + x ];

	while ( x < end )
	{
		*p++ ^= 0xFF ;
		x += 1 ;
	}
}

void lcd_hline( uint8_t x, uint8_t y, int8_t w )
{
  lcd_hlineStip(x,y,w,0xff);
}

void lcd_vline( uint8_t x, uint8_t y, int8_t h )
{
  if (h<0) { y+=h; h=-h; }
  register uint8_t *p  = &DISPLAY_START[ y / 8 * DISPLAY_W + x ];

  y &= 0x07 ;
	if ( y )
	{
    uint8_t msk = ~(BITMASK(y)-1) ;
    h -= 8-y ;
    if (h < 0)
      msk -= ~(BITMASK(8+h)-1) ;
		lcd_write_bits( p, msk ) ;
    p += DISPLAY_W ;
	}
    
  while( h >= 8 )
	{
		h -= 8 ;
		lcd_write_bits( p, 0xFF ) ;
    p += DISPLAY_W ;
  }
	if ( h > 0 )
	{
  	lcd_write_bits( p, (BITMASK(h)-1) ) ;
	}
}

#ifdef SIMU
bool lcd_refresh = true;
uint8_t lcd_buf[DISPLAY_W*DISPLAY_H/8];
#endif


void lcd_clear()
{
  //for(unsigned i=0; i<sizeof(displayBuf); i++) displayBuf[i]=0;
  memset( DisplayBuf, 0, sizeof( DisplayBuf) ) ;
}

// LCD i/o pins
// LCD_RES     PC27
// LCD_CS1     PC26
// LCD_E       PC12
// LCD_RnW     PC13
// LCD_A0      PC15
// LCD_D0      PC0
// LCD_D1      PC7
// LCD_D2      PC6
// LCD_D3      PC5
// LCD_D4      PC4
// LCD_D5      PC3
// LCD_D6      PC2
// LCD_D7      PC1

#define LCD_DATA	0x000000FFL
#define LCD_A0    0x00000080L
#define LCD_RnW   0x00002000L		// bit 13
#define LCD_E     0x00001000L
#define LCD_CS1   0x04000000L		// bit 26
#define LCD_RES   0x08000000L


uint8_t LcdLock ;
uint8_t LcdInputs ;



#ifdef PCBSKY

const static uint8_t Lcdinit[] =
{
	0xe2, 0xae, 0xa1, 0xA6, 0xA4, 0xA2, 0xC0, 0x2F, 0x25, 0x81, 0x22, 0xAF
} ;	


void lcd_init()
{
	register Pio *pioptr ;
	uint32_t i ;
  // /home/thus/txt/datasheets/lcd/KS0713.pdf
  // ~/txt/flieger/ST7565RV17.pdf  from http://www.glyn.de/content.asp?wdid=132&sid=


	configure_pins( LCD_A0, PIN_ENABLE | PIN_LOW | PIN_OUTPUT | PIN_PORTA | PIN_NO_PULLUP ) ;

// read the inputs, and lock the LCD lines
	LcdInputs = PIOC->PIO_PDSR << 1 ; // 6 LEFT, 5 RIGHT, 4 DOWN, 3 UP ()
	LcdLock = 1 ;
//	pioptr = PIOA ;
//	pioptr->PIO_PER = LCD_A0 ;		// Enable bit 7 (LCD-A0)
//	pioptr->PIO_CODR = LCD_A0 ;
//	pioptr->PIO_OER = LCD_A0 ;		// Set bit 7 output
	pioptr = PIOC ;
	
#ifdef REVX
	pioptr->PIO_PER = PIO_PC27 | PIO_PC12 | 0xFF ;		// Enable bits 27,26,13,12,7-0
#else
	pioptr->PIO_PER = PIO_PC27 | PIO_PC26 | PIO_PC13 | PIO_PC12 | 0xFF ;		// Enable bits 27,26,13,12,7-0
#endif // REVX

//#ifndef REVX
//	pioptr->PIO_CODR = LCD_E | LCD_RnW ;
//	pioptr->PIO_SODR = LCD_RES | LCD_CS1 ;
//#else 
#ifdef REVX
	pioptr->PIO_CODR = LCD_E ;
	pioptr->PIO_CODR = LCD_RnW | LCD_CS1 ;	// No longer needed, used elsewhere
#else
	pioptr->PIO_CODR = LCD_E | LCD_RnW | LCD_CS1 ;
#endif // REVX
	pioptr->PIO_SODR = LCD_RES ;
//#endif 
	pioptr->PIO_OER = PIO_PC27 | PIO_PC26 | PIO_PC13 | PIO_PC12 | 0xFF ;		// Set bits 27,26,13,12,7-0 output
	pioptr->PIO_OWER = 0x000000FFL ;		// Allow write to ls 8 bits in ODSR

	pioptr->PIO_CODR = LCD_RES ;		// Reset LCD
	TC0->TC_CHANNEL[0].TC_CCR = 5 ;	// Enable clock and trigger it (may only need trigger)
	while ( TC0->TC_CHANNEL[0].TC_CV < 200 )		// >10 uS, Value depends on MCK/2 (used 18MHz)
	{
		// Wait
	}
	pioptr->PIO_SODR = LCD_RES ;		// Remove LCD reset
	TC0->TC_CHANNEL[0].TC_CCR = 5 ;	// Enable clock and trigger it (may only need trigger)
	while ( TC0->TC_CHANNEL[0].TC_CV < 27000 )	// 1500 uS, Value depends on MCK/2 (used 18MHz)
	{
		// Wait
	}
	for ( i = 0 ; i < 12 ; i += 1 )
	{
	  lcdSendCtl( Lcdinit[i] ) ;
	}

	pioptr->PIO_ODR = 0x0000003AL ;		// Set bits 1, 3, 4, 5 input
	pioptr->PIO_PUER = 0x0000003AL ;		// Set bits 1, 3, 4, 5 with pullups
	pioptr->PIO_ODSR = 0 ;							// Drive D0 low
	LcdLock = 0 ;

#ifdef REVX
	lcdSetRefVolt( 23 ) ;
#else
	lcdSetRefVolt( 30 ) ;
#endif

}


void lcdSetRefVolt(uint8_t val)
{
#ifndef SIMU
	register Pio *pioptr ;
	pioptr = PIOC ;

// read the inputs, and lock the LCD lines
	LcdInputs = PIOC->PIO_PDSR << 1 ; // 6 LEFT, 5 RIGHT, 4 DOWN, 3 UP ()
	LcdLock = 1 ;

	pioptr->PIO_OER = 0x0C00B0FFL ;		// Set bits 27,26,15,13,12,7-0 output

  lcdSendCtl(0x81);
	if ( val == 0 )
	{
		val = 0x22 ;		
	}
  lcdSendCtl(val);
	
	pioptr->PIO_ODR = 0x0000003AL ;		// Set bits 1, 3, 4, 5 input
	pioptr->PIO_PUER = 0x0000003AL ;		// Set bits 1, 3, 4, 5 with pullups
	pioptr->PIO_ODSR = 0 ;							// Drive D0 low
#endif // SIMU
	LcdLock = 0 ;
}



void lcdSendCtl(uint8_t val)
{
	register Pio *pioptr ;
	
	pioptr = PIOC ;
#ifndef REVX
	pioptr->PIO_CODR = LCD_CS1 ;		// Select LCD
#endif 
	PIOA->PIO_CODR = LCD_A0 ;
#ifndef REVX
	pioptr->PIO_CODR = LCD_RnW ;		// Write
#endif
	pioptr->PIO_ODSR = val ;

	pioptr->PIO_SODR = LCD_E ;			// Start E pulse
	// Need a delay here (250nS)
	TC0->TC_CHANNEL[0].TC_CCR = 5 ;	// Enable clock and trigger it (may only need trigger)
	while ( TC0->TC_CHANNEL[0].TC_CV < 9 )		// Value depends on MCK/2 (used 18MHz)
	{
		// Wait
	}
	pioptr->PIO_CODR = LCD_E ;			// End E pulse
	PIOA->PIO_SODR = LCD_A0 ;				// Data
}

#ifdef SIMU
void refreshDisplay()
{
  memcpy(lcd_buf, DisplayBuf, sizeof(DisplayBuf));
  lcd_refresh = true;
}
#else
void refreshDisplay()
{
	register Pio *pioptr ;
  register uint8_t *p=DisplayBuf;
	register uint32_t y ;
	register uint32_t x ;
	register uint32_t z ;
	register uint32_t ebit ;

	ebit = LCD_E ;

	pioptr = PIOA ;
	pioptr->PIO_PER = 0x00000080 ;		// Enable bit 7 (LCD-A0)
	pioptr->PIO_OER = 0x00000080 ;		// Set bit 7 output

// read the inputs, and lock the LCD lines
	LcdInputs = PIOC->PIO_PDSR << 1 ; // 6 LEFT, 5 RIGHT, 4 DOWN, 3 UP ()
	LcdLock = 1 ;

	pioptr = PIOC ;
	pioptr->PIO_OER = 0x0C0030FFL ;		// Set bits 27,26,15,13,12,7-0 output
  for( y=0; y < 8; y++) {
    lcdSendCtl( OptrexDisplay ? 0 : 0x04 ) ;
    lcdSendCtl(0x10); //column addr 0
    lcdSendCtl( y | 0xB0); //page addr y
    
#ifndef REVX
		pioptr->PIO_CODR = LCD_CS1 ;		// Select LCD
#endif // REVX

		PIOA->PIO_SODR = LCD_A0 ;			// Data
#ifndef REVX
		pioptr->PIO_CODR = LCD_RnW ;		// Write
#endif // REVX
		 
		x =	*p ;
    for( z=0; z<128; z+=1)
		{
			pioptr->PIO_ODSR = x ;
			pioptr->PIO_SODR = ebit ;			// Start E pulse
			// Need a delay here (250nS)
			p += 1 ;
			x =	*p ;
			pioptr->PIO_CODR = ebit ;			// End E pulse
    }
  }
	pioptr->PIO_ODSR = 0xFF ;					// Drive lines high
	pioptr->PIO_PUER = 0x0000003AL ;	// Set bits 1, 3, 4, 5 with pullups
	pioptr->PIO_ODR = 0x0000003AL ;		// Set bits 1, 3, 4, 5 input
	pioptr->PIO_ODSR = 0xFE ;					// Drive D0 low

	LcdLock = 0 ;

}

#endif // PCBSKY
#endif // PCBSKY

#ifdef PCBSKY
// This specially for the 9XR
extern void start_timer0( void ) ;
extern void stop_timer0( void ) ;

extern "C" void dispUSB( void ) ;
void dispUSB()
{
	// Put USB on display here?
	start_timer0() ;
	lcd_init() ;
	OptrexDisplay = 1 ;
	lcd_clear() ;
	refreshDisplay() ;
	OptrexDisplay = 0 ;
	lcd_clear() ;
	lcd_putcAtt( 48, 24, 'U', DBLSIZE ) ;
	lcd_putcAtt( 60, 24, 'S', DBLSIZE ) ;
	lcd_putcAtt( 72, 24, 'B', DBLSIZE ) ;
	refreshDisplay() ;
	stop_timer0() ;
}

#endif

