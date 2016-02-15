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

// for the  MULTI protocol definition
// see https://github.com/pascallanger/DIY-Multiprotocol-TX-Module
// file Multiprotocol/multiprotocol.h



/* The protocol is heavily inspired by the DSM2 protocol, so reuse some the definitions where they are identical */

#define MULTI_SEND_BIND                     (1 << 7)
#define MULTI_SEND_RANGECHECK               (1 << 5)


#define BITLEN_MULTI          (10*2) //100000 Baud => 10uS per bit

static void _send_level(uint8_t v)
{
    /* Copied over from DSM, this looks doubious and in my logic analyzer
       output the low->high is about 2 ns late */
  if (modulePulsesData[EXTERNAL_MODULE].dsm2.index == 0)
    v -= 2;
  else
    v += 2;

  modulePulsesData[EXTERNAL_MODULE].dsm2.value += v;
  *modulePulsesData[EXTERNAL_MODULE].dsm2.ptr++ = modulePulsesData[EXTERNAL_MODULE].dsm2.value;
  modulePulsesData[EXTERNAL_MODULE].dsm2.index = (modulePulsesData[EXTERNAL_MODULE].dsm2.index+1) % 2;
}


static void sendByteMulti(uint8_t b) //max 11 changes 0 10 10 10 10 P 1
{
  bool    lev = 0;
  uint8_t parity = 1;

  uint8_t len = BITLEN_MULTI; //max val: 10*20 < 256
  for (uint8_t i=0; i<=9; i++) { //8Bits + 1Parity + Stop=1
    bool nlev = b & 1; //lsb first
    parity = parity ^ nlev;
    if (lev == nlev) {
      len += BITLEN_MULTI;
    }
    else {
      _send_level(len);
      len  = BITLEN_MULTI;
      lev  = nlev;
    }
    b = (b>>1) | 0x80; //shift in ones for stop bit and parity
    if (i==7)
        b = b ^ parity; // lowest bit is one from previous line
  }
  _send_level(len+2*BITLEN_MULTI); // 2Stop bits
}

// This is the data stream to send, prepare after 19.5 mS
// Send after 22.5 mS

//static uint8_t *Dsm2_pulsePtr = pulses2MHz.pbyte ;


#define MULTI_CHANS           16
#define MULTI_CHAN_BITS       11
void setupPulsesMultimodule(unsigned int port)
{

#if defined(PPM_PIN_HW_SERIAL)
  modulePulsesData[EXTERNAL_MODULE].dsm2.serialByte = 0 ;
  modulePulsesData[EXTERNAL_MODULE].dsm2.serialBitCount = 0 ;
#else
  modulePulsesData[EXTERNAL_MODULE].dsm2.value = 0;
  modulePulsesData[EXTERNAL_MODULE].dsm2.index = 1;
#endif

  modulePulsesData[EXTERNAL_MODULE].dsm2.ptr = modulePulsesData[EXTERNAL_MODULE].dsm2.pulses;

#if defined(PCBTARANIS)
  modulePulsesData[EXTERNAL_MODULE].dsm2.value = 100;
  *modulePulsesData[EXTERNAL_MODULE].dsm2.ptr++ = modulePulsesData[EXTERNAL_MODULE].dsm2.value;
#endif

  // header, byte 0, always 0x55
  sendByteMulti(0x55);

  // byte 1+2, protocol information

  int type = g_model.moduleData[port].multi.rfProtocol +1;
  int subtype = g_model.moduleData[port].subType;

  uint8_t protoByte=0;
  if (moduleFlag[port] == MODULE_BIND)
    protoByte |= MULTI_SEND_BIND;
  else if (moduleFlag[port] == MODULE_RANGECHECK)
    protoByte |= MULTI_SEND_RANGECHECK;

  // rfProtocol
  // 15 is FrskyX or D16 which we map as a subprotocol of 3 (FrSky)
  // all protos > frskyx are therefore also off by one
  if (type >=15)
    type= type +1;

  if (g_model.moduleData[port].multi.rfProtocol == MM_RF_PROTO_FRSKY) {
    if(subtype == 1)
      type = 3;
     else
      type = 15;
    subtype = 0;
  }

  protoByte |= (type & 0x1f);
  sendByteMulti(protoByte);


  // power always set to high (0 << 7)
  sendByteMulti((g_model.header.modelId[port] & 0x0f)
                   | (0 << 7) | ((subtype & 0x7)  << 4));


  //TODO: option_protocol, use same default as multi module
  // byte 3
  sendByteMulti(g_model.moduleData[port].multi.optionValue);

  uint32_t bits = 0;
  uint8_t bitsavailable = 0;

  // byte 4-25, channels 0..2047
  // ?? Range for pulses (channelsOutputs) is [-1024:+1024]
  for (int i=0; i<MULTI_CHANS; i++) {
    int channel = g_model.moduleData[port].channelsStart+i;
    int value = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;

    bits |= limit(0, 1024 + value, 2047) << bitsavailable;
    bitsavailable += MULTI_CHAN_BITS;
    while (bitsavailable >= 8) {
      sendByteMulti((uint8_t) (bits & 0xff));
      bits >>= 8;
      bitsavailable -= 8;
    }
  }

  putDsm2Flush();
}
