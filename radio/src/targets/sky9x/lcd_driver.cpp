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

#define LCD_DATA  0x000000FFL
#if !defined(REVA)
#define LCD_A0    0x00000080L
#else
#define LCD_A0    0x00008000L
#endif
#define LCD_RnW   0x00002000L
#define LCD_E     0x00001000L
#define LCD_CS1   0x04000000L
#define LCD_RES   0x08000000L

// Lookup table for prototype board
#if defined(REVA)
const uint8_t Lcd_lookup[] =
{
  0x00,0x01,0x80,0x81,0x40,0x41,0xC0,0xC1,0x20,0x21,0xA0,0xA1,0x60,0x61,0xE0,0xE1,
  0x10,0x11,0x90,0x91,0x50,0x51,0xD0,0xD1,0x30,0x31,0xB0,0xB1,0x70,0x71,0xF0,0xF1,
  0x08,0x09,0x88,0x89,0x48,0x49,0xC8,0xC9,0x28,0x29,0xA8,0xA9,0x68,0x69,0xE8,0xE9,
  0x18,0x19,0x98,0x99,0x58,0x59,0xD8,0xD9,0x38,0x39,0xB8,0xB9,0x78,0x79,0xF8,0xF9,
  0x04,0x05,0x84,0x85,0x44,0x45,0xC4,0xC5,0x24,0x25,0xA4,0xA5,0x64,0x65,0xE4,0xE5,
  0x14,0x15,0x94,0x95,0x54,0x55,0xD4,0xD5,0x34,0x35,0xB4,0xB5,0x74,0x75,0xF4,0xF5,
  0x0C,0x0D,0x8C,0x8D,0x4C,0x4D,0xCC,0xCD,0x2C,0x2D,0xAC,0xAD,0x6C,0x6D,0xEC,0xED,
  0x1C,0x1D,0x9C,0x9D,0x5C,0x5D,0xDC,0xDD,0x3C,0x3D,0xBC,0xBD,0x7C,0x7D,0xFC,0xFD,
  0x02,0x03,0x82,0x83,0x42,0x43,0xC2,0xC3,0x22,0x23,0xA2,0xA3,0x62,0x63,0xE2,0xE3,
  0x12,0x13,0x92,0x93,0x52,0x53,0xD2,0xD3,0x32,0x33,0xB2,0xB3,0x72,0x73,0xF2,0xF3,
  0x0A,0x0B,0x8A,0x8B,0x4A,0x4B,0xCA,0xCB,0x2A,0x2B,0xAA,0xAB,0x6A,0x6B,0xEA,0xEB,
  0x1A,0x1B,0x9A,0x9B,0x5A,0x5B,0xDA,0xDB,0x3A,0x3B,0xBA,0xBB,0x7A,0x7B,0xFA,0xFB,
  0x06,0x07,0x86,0x87,0x46,0x47,0xC6,0xC7,0x26,0x27,0xA6,0xA7,0x66,0x67,0xE6,0xE7,
  0x16,0x17,0x96,0x97,0x56,0x57,0xD6,0xD7,0x36,0x37,0xB6,0xB7,0x76,0x77,0xF6,0xF7,
  0x0E,0x0F,0x8E,0x8F,0x4E,0x4F,0xCE,0xCF,0x2E,0x2F,0xAE,0xAF,0x6E,0x6F,0xEE,0xEF,
  0x1E,0x1F,0x9E,0x9F,0x5E,0x5F,0xDE,0xDF,0x3E,0x3F,0xBE,0xBF,0x7E,0x7F,0xFE,0xFF
};
#endif

void lcdSendCtl(uint8_t val)
{
  Pio *pioptr;

#if defined(REVA)
  pioptr = PIOC;
  pioptr->PIO_CODR = LCD_CS1;            // Select LCD
  pioptr->PIO_CODR = LCD_A0;                     // Control
  pioptr->PIO_CODR = LCD_RnW;            // Write
  pioptr->PIO_ODSR = Lcd_lookup[val];
#else
  pioptr = PIOC;
  #if !defined(REVX)
    pioptr->PIO_CODR = LCD_CS1;            // Select LCD
  #endif
  PIOA->PIO_CODR = LCD_A0;
  #if !defined(REVX)
    pioptr->PIO_CODR = LCD_RnW;            // Write
  #endif
  pioptr->PIO_ODSR = val;
#endif

  pioptr->PIO_SODR = LCD_E;                      // Start E pulse
  // Need a delay here (250nS)
  TC0->TC_CHANNEL[0].TC_CCR = 5; // Enable clock and trigger it (may only need trigger)
  while ( TC0->TC_CHANNEL[0].TC_CV < 9 )          // Value depends on MCK/2 (used 18MHz)
  {
    // Wait
  }
  pioptr->PIO_CODR = LCD_E;                      // End E pulse
#if defined(REVA)
  pioptr->PIO_SODR = LCD_A0;                     // Data
#else
  PIOA->PIO_SODR = LCD_A0;                       // Data
#endif

#if !defined(REVX)
  pioptr->PIO_SODR = LCD_CS1;                    // Deselect LCD
#endif
}

void lcdInit()
{
  Pio *pioptr;
  // /home/thus/txt/datasheets/lcd/KS0713.pdf
  // ~/txt/flieger/ST7565RV17.pdf  from http://www.glyn.de/content.asp?wdid=132&sid=

  // read the inputs, and lock the LCD lines
  lcdInputs = PIOC->PIO_PDSR; // 6 LEFT, 5 RIGHT, 4 DOWN, 3 UP ()
  lcdLock = 1;

#if defined(REVA)
  pioptr = PIOC;
  pioptr->PIO_PER = 0x0C00B0FFL;         // Enable bits 27,26,15,13,12,7-0
  pioptr->PIO_CODR = LCD_E | LCD_RnW | LCD_A0;
  pioptr->PIO_SODR = LCD_RES | LCD_CS1;
  pioptr->PIO_OER = 0x0C00B0FFL;         // Set bits 27,26,15,13,12,7-0 output
  pioptr->PIO_OWER = 0x000000FFL;                // Allow write to ls 8 bits in ODSR
#else
  configure_pins( LCD_A0, PIN_ENABLE | PIN_LOW | PIN_OUTPUT | PIN_PORTA | PIN_NO_PULLUP );
  pioptr = PIOC;
#if defined(REVX)
  pioptr->PIO_PER = PIO_PC27 | PIO_PC12 | 0xFF;
  pioptr->PIO_CODR = LCD_E;
  pioptr->PIO_CODR = LCD_RnW | LCD_CS1;  // No longer needed, used elsewhere
#else
  pioptr->PIO_PER = PIO_PC27 | PIO_PC26 | PIO_PC13 | PIO_PC12 | 0xFF;
  pioptr->PIO_CODR = LCD_E | LCD_RnW | LCD_CS1;
#endif
  pioptr->PIO_SODR = LCD_RES;
  pioptr->PIO_OER = PIO_PC27 | PIO_PC26 | PIO_PC13 | PIO_PC12 | 0xFF;          // Set bits 27,26,13,12,7-0 output
  pioptr->PIO_OWER = 0x000000FFL;                // Allow write to ls 8 bits in ODSR
#endif

  TC0->TC_CHANNEL[0].TC_CCR = 5;       // Enable clock and trigger it (may only need trigger)
  pioptr->PIO_CODR = LCD_RES;            // Reset LCD
  while ( TC0->TC_CHANNEL[0].TC_CV < 500 ) ;        // > 10us, Value depends on MCK/2 (used 18MHz)
  TC0->TC_CHANNEL[0].TC_CCR = 5; // Enable clock and trigger it (may only need trigger)
  pioptr->PIO_SODR = LCD_RES;            // Remove LCD reset
  while ( TC0->TC_CHANNEL[0].TC_CV < 27000 ) ;              // 1500us, Value depends on MCK/2 (used 18MHz)
  lcdSendCtl(0xe2); //Initialize the internal functions
  lcdSendCtl(0xae); //DON = 0: display OFF
  lcdSendCtl(0xa1); //ADC = 1: reverse direction(SEG132->SEG1)
  lcdSendCtl(0xA6); //REV = 0: non-reverse display
  lcdSendCtl(0xA4); //EON = 0: normal display. non-entire
  lcdSendCtl(0xA2); // Select LCD bias=0
  lcdSendCtl(0xC0); //SHL = 0: normal direction (COM1->COM64)
  lcdSendCtl(0x2F); //Control power circuit operation VC=VR=VF=1
  lcdSendCtl(0x25); //Select int resistance ratio R2 R1 R0 =5
  lcdSendCtl(0x81); //Set reference voltage Mode
  lcdSendCtl(0x22); // 24 SV5 SV4 SV3 SV2 SV1 SV0 = 0x18

#if defined(REVX)
  // 200mS delay (only if not wdt reset)
  if ( ( ( ResetReason & RSTC_SR_RSTTYP ) != (2 << 8) ) && !unexpectedShutdown ) {
    for (uint32_t j = 0; j < 100; j += 1 ) {
      TC0->TC_CHANNEL[0].TC_CCR = 5; // Enable clock and trigger it (may only need trigger)
      while ( TC0->TC_CHANNEL[0].TC_CV < 36000 ) {
        // Value depends on MCK/2 (used 18MHz) give 2mS delay
        wdt_reset(); // Wait
      }
    }
  }
#endif

  lcdSendCtl(0xAF); // DON = 1: display ON

#if defined(REVA)
  pioptr->PIO_ODR = 0x0000003CL;         // Set bits 2, 3, 4, 5 input
  pioptr->PIO_PUER = 0x0000003CL;                // Set bits 2, 3, 4, 5 with pullups
  pioptr->PIO_ODSR = 0;                                                  // Drive D0 low
#else
  pioptr->PIO_ODR = 0x0000003AL;         // Set bits 1, 3, 4, 5 input
  pioptr->PIO_PUER = 0x0000003AL;                // Set bits 1, 3, 4, 5 with pullups
  pioptr->PIO_ODSR = 0;                                                  // Drive D0 low
#endif

  lcdLock = 0;
}

void lcdSetRefVolt(uint8_t val)
{
  Pio *pioptr;
  pioptr = PIOC;

  // read the inputs, and lock the LCD lines
  lcdInputs = PIOC->PIO_PDSR; // 6 LEFT, 5 RIGHT, 4 DOWN, 3 UP ()
  lcdLock = 1;

  pioptr->PIO_OER = 0x0C00B0FFL;         // Set bits 27,26,15,13,12,7-0 output

  lcdSendCtl(0x81);
  if ( val == 0 )
  {
    val = 0x22;
  }
  lcdSendCtl(val);

#if defined(REVA)
  pioptr->PIO_ODR = 0x000000FEL;         // Set bits 2, 3, 4, 5 input
  pioptr->PIO_PUER = 0x000000FEL;                // Set bits 2, 3, 4, 5 with pullups
#else
  pioptr->PIO_ODR = 0x000000FEL;         // Set bits 1, 3, 4, 5 input
  pioptr->PIO_PUER = 0x000000FEL;                // Set bits 1, 3, 4, 5 with pullups
#endif

  pioptr->PIO_ODSR = 0;                                                  // Drive D0 low
  lcdLock = 0;
}

void lcdRefresh()
{
  Pio *pioptr;
  uint8_t *p = displayBuf;
  uint32_t y;
  uint32_t x;
  uint32_t z;
  uint32_t ebit;
#if defined(REVA)
  uint8_t *lookup;
  lookup = (uint8_t *) Lcd_lookup;
#endif

  ebit = LCD_E;

#if !defined(REVA)
  pioptr = PIOA;
  pioptr->PIO_PER = 0x00000080; // Enable bit 7 (LCD-A0)
  pioptr->PIO_OER = 0x00000080;// Set bit 7 output
#endif

  // read the inputs, and lock the LCD lines
  lcdInputs = PIOC->PIO_PDSR; // 6 LEFT, 5 RIGHT, 4 DOWN, 3 UP ()
  lcdLock = 1;

  pioptr = PIOC;
#if defined(REVA)
  pioptr->PIO_OER = 0x0C00B0FFL; // Set bits 27,26,15,13,12,7-0 output
#else
  pioptr->PIO_OER = 0x0C0030FFL; // Set bits 27,26,15,13,12,7-0 output
#endif
  for (y = 0; y < 8; y++) {
    lcdSendCtl(g_eeGeneral.optrexDisplay ? 0 : 0x04);
    lcdSendCtl(0x10); //column addr 0
    lcdSendCtl(y | 0xB0); //page addr y

#if !defined(REVX)
    pioptr->PIO_CODR = LCD_CS1; // Select LCD
#endif
    PIOA->PIO_SODR = LCD_A0; // Data
#if !defined(REVX)
    pioptr->PIO_CODR = LCD_RnW; // Write
#endif

#if defined(REVA)
    x = lookup[*p];
#else
    x = *p;
#endif
    for (z = 0; z < LCD_W; z += 1) {
      pioptr->PIO_ODSR = x;
      pioptr->PIO_SODR = ebit; // Start E pulse
      // Need a delay here (250nS)
      p += 1;
#if defined(REVA)
      x = lookup[*p];
#else
      x = *p;
#endif
//                      TC0->TC_CHANNEL[0].TC_CCR = 5; // Enable clock and trigger it (may only need trigger)
//                      while ( TC0->TC_CHANNEL[0].TC_CV < 3 )          // Value depends on MCK/2 (used 6MHz)
//                      {
//                              // Wait
//                      }
      pioptr->PIO_CODR = ebit; // End E pulse
    }
#if !defined(REVX)
    pioptr->PIO_SODR = LCD_CS1; // Deselect LCD
#endif
  }

  pioptr->PIO_ODSR = 0xFF;                                       // Drive lines high
#if defined(REVA)
  pioptr->PIO_PUER = 0x000000FEL;        // Set bits 2, 3, 4, 5 with pullups
  pioptr->PIO_ODR = 0x000000FEL;         // Set bits 2, 3, 4, 5 input
#else
  pioptr->PIO_PUER = 0x000000FEL;        // Set bits 1, 3, 4, 5 with pullups
  pioptr->PIO_ODR = 0x000000FEL;         // Set bits 1, 3, 4, 5 input
#endif

  pioptr->PIO_ODSR = 0xFE;                                       // Drive D0 low
  lcdLock = 0;
}
