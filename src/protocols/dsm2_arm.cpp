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

uint8_t Bit_pulses[64] ;                          // Likely more than we need
uint8_t *Pulses2MHzptr ;
uint8_t Serial_byte ;
uint8_t Serial_bit_count;
uint8_t Serial_byte_count ;

// DSM2 control bits
#define DSM2_CHANS     6
#define BIND_BIT       0x80
#define RANGECHECK_BIT 0x20
#define FRANCE_BIT     0x10
#define DSMX_BIT       0x08
#define BAD_DATA       0x47

#define BITLEN_DSM2    (8*2) //125000 Baud => 8uS per bit

void put_serial_bit(uint8_t bit)
{
  Serial_byte >>= 1 ;
  if (bit & 1)
  {
    Serial_byte |= 0x80 ;
  }
  if (++Serial_bit_count >= 8)
  {
    *Pulses2MHzptr++ = Serial_byte ;
    Serial_bit_count = 0 ;
    Serial_byte_count += 1 ;
  }
}

void sendByteDsm2(uint8_t b) //max 10changes 0 10 10 10 10 1
{
  put_serial_bit(0) ;           // Start bit
  for(uint8_t i=0; i<8; i++)    // 8 data Bits
  {
    put_serial_bit(b & 1) ;
    b >>= 1 ;
  }

  put_serial_bit(1) ;           // Stop bit
  put_serial_bit(1) ;           // Stop bit
}

// This is the data stream to send, prepare after 19.5 mS
// Send after 22.5 mS

//static uint8_t *Dsm2_pulsePtr = pulses2MHz.pbyte ;
void setupPulsesDsm2(uint8_t chns)
{
  static uint8_t dsmDat[2+6*2]={0xFF,0x00,  0x00,0xAA,  0x05,0xFF,  0x09,0xFF,  0x0D,0xFF,  0x13,0x54,  0x14,0xAA};
  uint8_t counter ;
  //    CSwData &cs = g_model.customSw[NUM_CSW-1];

  Serial_byte = 0 ;
  Serial_bit_count = 0 ;
  Serial_byte_count = 0 ;
  Pulses2MHzptr = Bit_pulses ;

  // If more channels needed make sure the pulses union/array is large enough
  if (dsmDat[0]&BAD_DATA)  //first time through, setup header
  {
    switch(s_current_protocol[0])
    {
      case PROTO_DSM2_LP45:
        dsmDat[0]= 0x80;
        break;
      case PROTO_DSM2_DSM2:
        dsmDat[0]=0x90;
        break;
      default:
        dsmDat[0]=0x98;  //dsmx, bind mode
        break;
    }
  }
  if ((dsmDat[0] & BIND_BIT) && (!switchState(SW_TRN))) dsmDat[0] &= ~BIND_BIT; // clear bind bit if trainer not pulled

  // TODO find a way to do that, FUNC SWITCH: if ((!(dsmDat[0] & BIND_BIT)) && getSwitch(MAX_DRSWITCH-1, 0, 0)) dsmDat[0] |= RANGECHECK_BIT;   // range check function
  // else dsmDat[0] &= ~RANGECHECK_BIT;
  dsmDat[1]=g_eeGeneral.currModel+1;  //DSM2 Header second byte for model match
  for(uint8_t i=0; i<chns; i++)
  {
    uint16_t pulse = limit(0, ((channelOutputs[i]*13)>>5)+512,1023);
    dsmDat[2+2*i] = (i<<2) | ((pulse>>8)&0x03);
    dsmDat[3+2*i] = pulse & 0xff;
  }

  for (counter = 0 ; counter < 14 ; counter += 1)
  {
    sendByteDsm2(dsmDat[counter]);
  }
  for (counter = 0 ; counter < 16 ; counter += 1)
  {
    put_serial_bit(1) ;           // 16 extra stop bits
  }
}
