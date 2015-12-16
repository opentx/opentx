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


// MULTI protocol definition (from mbtx:radio/er9x/src/pulses.cpp)
/*
  Serial: 125000 Baud 8n1      _ xxxx xxxx - ---
  Channels:
    Nbr=8
    10bits=0..1023
	0		-125%
    96		-100%
	512		   0%
	928		+100%
	1023	+125%
  Stream[0]   = sub_protocol|BindBit|RangeCheckBit|AutoBindBit;
   sub_protocol is 0..31 (bits 0..4)
				=>	Reserved	0
					Flysky		1
					Hubsan		2
					Frsky		3
					Hisky		4
					V2x2		5
					DSM2		6
					Devo		7
					YD717		8
					KN			9
					SymaX		10
					SLT			11
					CX10		12
					CG023		13
					Bayang		14
   BindBit=>		0x80	1=Bind/0=No
   AutoBindBit=>	0x40	1=Yes /0=No
   RangeCheck=>		0x20	1=Yes /0=No
  Stream[1]   = RxNum | Power | Type;
   RxNum value is 0..15 (bits 0..3)
   Type is 0..7 <<4     (bit 4..6)
		sub_protocol==Flysky
			Flysky	0
			V9x9	1
			V6x6	2
			V912	3
		sub_protocol==Hisky
			Hisky	0
			HK310	1
		sub_protocol==DSM2
			DSM2	0
			DSMX	1
		sub_protocol==YD717
			YD717	0
			SKYWLKR	1
			SYMAX2	2
			XINXUN	3
			NIHUI	4
		sub_protocol==SYMAX
			SYMAX	0
			SYMAX5C	1
		sub_protocol==CX10
			CX10_GREEN	0
			CX10_BLUE	1	// also compatible with CX10-A, CX12
			DM007		2
		sub_protocol==CG023
			CG023		0
			YD829		1
   Power value => 0x80	0=High/1=Low
  Stream[2]   = option_protocol;
   option_protocol value is -127..127
  Stream[i+3] = lowByte(channel[i])		// with i[0..7]
  Stream[11]  = highByte(channel[0])<<6 | highByte(channel[1])<<4 | highByte(channel[2])<<2 | highByte(channel[3])
  Stream[12]  = highByte(channel[4])<<6 | highByte(channel[5])<<4 | highByte(channel[6])<<2 | highByte(channel[7])
  Stream[13]  = lowByte(CRC16(Stream[0..12])
*/

/* Lenght 3 (header) + 8 (channel low bytes) + 2 (channel high bits) + 1 (crc) */
#define MULTI_CHANS           8
#define MULTI_FRAME_LENGTH 2+MULTI_CHANS+2+1

/* The protocol is heavily inspired by the DSM2 protocol, so reuse some the definitions where they are identical */

#define MULTI_SEND_BIND                     (1 << 7)
#define MULTI_SEND_RANGECHECK               (1 << 5)

void sendByteMulti(uint8_t b)
{
    sendByteDsm2(b);
}

extern const uint16_t CRCTable[];
inline void sendByteMultiCRC(uint16_t & crcsum, uint8_t b)
{
    crcsum = (crcsum << 8) ^ (CRCTable[((crcsum>>8)^b) & 0xff]);
    sendByteMulti(b);
}


// This is the data stream to send, prepare after 19.5 mS
// Send after 22.5 mS

//static uint8_t *Dsm2_pulsePtr = pulses2MHz.pbyte ;
void setupPulsesMultimodule(unsigned int port)
{

    uint16_t crcsum =0;
#if defined(PCBSKY9X)
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

  uint8_t protoByte=0;


  if (moduleFlag[port] == MODULE_BIND)
    protoByte |= MULTI_SEND_BIND;
  else if (moduleFlag[port] == MODULE_RANGECHECK)
    protoByte |= MULTI_SEND_RANGECHECK;

  // rfProtocol = -8..7 => 1..15
  protoByte |= ((g_model.moduleData[port].rfProtocol+9) & 0xf);
  sendByteMultiCRC(crcsum, protoByte);


  // power always set to high (0 << 7)
  int subtype = g_model.moduleData[port].subType;
  sendByteMultiCRC(crcsum, (g_model.header.modelId[port] & 0x0f)
                   | (0 << 7) | ((subtype & 0x7)  << 4));


  //TODO: option_protocol, use same default as multi module
  int8_t option_protocol=0xd7;
  sendByteMultiCRC (crcsum, option_protocol);

  uint16_t highbits=0;
  for (int i=0; i<MULTI_CHANS; i++) {
    int channel = g_model.moduleData[port].channelsStart+i;
    int value = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
    uint16_t pulse = limit(0, ((value*13)>>5)+512, 1023);
    sendByteMultiCRC(crcsum,pulse & 0xff);
    highbits = (highbits << 2) | ( (pulse >> 8) & 0x03);
  }

  // Send high bits
  sendByteMultiCRC(crcsum, (highbits >> 8) & 0xff);
  sendByteMultiCRC(crcsum, highbits & 0xff);


  //send CRC

  sendByteMulti(crcsum & 0xff);

  putDsm2Flush();
}
