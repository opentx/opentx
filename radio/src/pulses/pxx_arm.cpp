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

#define PXX_SEND_BIND                      0x01
#define PXX_SEND_FAILSAFE                  (1 << 4)
#define PXX_SEND_RANGECHECK                (1 << 5)

const uint16_t CRCTable[]=
{
  0x0000,0x1189,0x2312,0x329b,0x4624,0x57ad,0x6536,0x74bf,
  0x8c48,0x9dc1,0xaf5a,0xbed3,0xca6c,0xdbe5,0xe97e,0xf8f7,
  0x1081,0x0108,0x3393,0x221a,0x56a5,0x472c,0x75b7,0x643e,
  0x9cc9,0x8d40,0xbfdb,0xae52,0xdaed,0xcb64,0xf9ff,0xe876,
  0x2102,0x308b,0x0210,0x1399,0x6726,0x76af,0x4434,0x55bd,
  0xad4a,0xbcc3,0x8e58,0x9fd1,0xeb6e,0xfae7,0xc87c,0xd9f5,
  0x3183,0x200a,0x1291,0x0318,0x77a7,0x662e,0x54b5,0x453c,
  0xbdcb,0xac42,0x9ed9,0x8f50,0xfbef,0xea66,0xd8fd,0xc974,
  0x4204,0x538d,0x6116,0x709f,0x0420,0x15a9,0x2732,0x36bb,
  0xce4c,0xdfc5,0xed5e,0xfcd7,0x8868,0x99e1,0xab7a,0xbaf3,
  0x5285,0x430c,0x7197,0x601e,0x14a1,0x0528,0x37b3,0x263a,
  0xdecd,0xcf44,0xfddf,0xec56,0x98e9,0x8960,0xbbfb,0xaa72,
  0x6306,0x728f,0x4014,0x519d,0x2522,0x34ab,0x0630,0x17b9,
  0xef4e,0xfec7,0xcc5c,0xddd5,0xa96a,0xb8e3,0x8a78,0x9bf1,
  0x7387,0x620e,0x5095,0x411c,0x35a3,0x242a,0x16b1,0x0738,
  0xffcf,0xee46,0xdcdd,0xcd54,0xb9eb,0xa862,0x9af9,0x8b70,
  0x8408,0x9581,0xa71a,0xb693,0xc22c,0xd3a5,0xe13e,0xf0b7,
  0x0840,0x19c9,0x2b52,0x3adb,0x4e64,0x5fed,0x6d76,0x7cff,
  0x9489,0x8500,0xb79b,0xa612,0xd2ad,0xc324,0xf1bf,0xe036,
  0x18c1,0x0948,0x3bd3,0x2a5a,0x5ee5,0x4f6c,0x7df7,0x6c7e,
  0xa50a,0xb483,0x8618,0x9791,0xe32e,0xf2a7,0xc03c,0xd1b5,
  0x2942,0x38cb,0x0a50,0x1bd9,0x6f66,0x7eef,0x4c74,0x5dfd,
  0xb58b,0xa402,0x9699,0x8710,0xf3af,0xe226,0xd0bd,0xc134,
  0x39c3,0x284a,0x1ad1,0x0b58,0x7fe7,0x6e6e,0x5cf5,0x4d7c,
  0xc60c,0xd785,0xe51e,0xf497,0x8028,0x91a1,0xa33a,0xb2b3,
  0x4a44,0x5bcd,0x6956,0x78df,0x0c60,0x1de9,0x2f72,0x3efb,
  0xd68d,0xc704,0xf59f,0xe416,0x90a9,0x8120,0xb3bb,0xa232,
  0x5ac5,0x4b4c,0x79d7,0x685e,0x1ce1,0x0d68,0x3ff3,0x2e7a,
  0xe70e,0xf687,0xc41c,0xd595,0xa12a,0xb0a3,0x8238,0x93b1,
  0x6b46,0x7acf,0x4854,0x59dd,0x2d62,0x3ceb,0x0e70,0x1ff9,
  0xf78f,0xe606,0xd49d,0xc514,0xb1ab,0xa022,0x92b9,0x8330,
  0x7bc7,0x6a4e,0x58d5,0x495c,0x3de3,0x2c6a,0x1ef1,0x0f78
};

void crc(uint8_t data, unsigned int port)
{
  modulePulsesData[port].pxx.pcmCrc = (modulePulsesData[port].pxx.pcmCrc<<8) ^ (CRCTable[((modulePulsesData[port].pxx.pcmCrc>>8)^data) & 0xFF]);
}

#if defined(PCBTARANIS)

void putPcmPart(uint8_t value, unsigned int port)
{
  modulePulsesData[port].pxx.pcmValue += 18 ;                                        // Output 1 for this time
  *modulePulsesData[port].pxx.ptr++ = modulePulsesData[port].pxx.pcmValue ;
  modulePulsesData[port].pxx.pcmValue += 14 ;
  if (value) {
    modulePulsesData[port].pxx.pcmValue += 16 ;
  }
  *modulePulsesData[port].pxx.ptr++ = modulePulsesData[port].pxx.pcmValue ;  // Output 0 for this time
}

void putPcmFlush(unsigned int port)
{
  *modulePulsesData[port].pxx.ptr++ = 18010 ;             // Past the 18000 of the ARR
}

#else

void putPcmSerialBit(uint8_t bit, unsigned int port)
{
  modulePulsesData[port].pxx.serialByte >>= 1;
  if (bit & 1) {
    modulePulsesData[port].pxx.serialByte |= 0x80;
  }
  if (++modulePulsesData[port].pxx.serialBitCount >= 8) {
    *modulePulsesData[port].pxx.ptr++ = modulePulsesData[port].pxx.serialByte;
    modulePulsesData[port].pxx.serialBitCount = 0;
  }
}

// 8uS/bit 01 = 0, 001 = 1
void putPcmPart(uint8_t value, unsigned int port)
{
  putPcmSerialBit(0, port);
  if (value) {
    putPcmSerialBit(0, port);
  }
  putPcmSerialBit(1, port);
}

void putPcmFlush(unsigned int port)
{
  while (modulePulsesData[port].pxx.serialBitCount != 0) {
    putPcmSerialBit(1, port);
  }
}

#endif

void putPcmBit(uint8_t bit, unsigned int port)
{
  if (bit) {
    modulePulsesData[port].pxx.pcmOnesCount += 1;
    putPcmPart(1, port);
  }
  else {
    modulePulsesData[port].pxx.pcmOnesCount = 0;
    putPcmPart(0, port);
  }
  if (modulePulsesData[port].pxx.pcmOnesCount >= 5) {
    putPcmBit(0, port);                                // Stuff a 0 bit in
  }
}

void putPcmByte(uint8_t byte, unsigned int port)
{
  crc(byte, port);

  for (uint8_t i=0; i<8; i++) {
    putPcmBit(byte & 0x80, port);
    byte <<= 1;
  }
}

void putPcmHead(unsigned int port)
{
  // send 7E, do not CRC
  // 01111110
  putPcmPart(0, port);
  putPcmPart(1, port);
  putPcmPart(1, port);
  putPcmPart(1, port);
  putPcmPart(1, port);
  putPcmPart(1, port);
  putPcmPart(1, port);
  putPcmPart(0, port);
}

void setupPulsesPXX(unsigned int port)
{
  uint16_t pulseValue=0, pulseValueLow=0;

  modulePulsesData[port].pxx.ptr = modulePulsesData[port].pxx.pulses;
  modulePulsesData[port].pxx.pcmValue = 0 ;
  modulePulsesData[port].pxx.pcmCrc = 0;
  modulePulsesData[port].pxx.pcmOnesCount = 0;

  /* Preamble */
  putPcmPart(0, port);
  putPcmPart(0, port);
  putPcmPart(0, port);
  putPcmPart(0, port);

  /* Sync */
  putPcmHead(port);

  /* Rx Number */
  putPcmByte(g_model.header.modelId[port], port);

  /* FLAG1 */
  uint8_t flag1 = (g_model.moduleData[port].rfProtocol << 6);
  if (moduleFlag[port] == MODULE_BIND) {
    flag1 |= (g_eeGeneral.countryCode << 1) | PXX_SEND_BIND;
  }
  else if (moduleFlag[port] == MODULE_RANGECHECK) {
    flag1 |= PXX_SEND_RANGECHECK;
  }
  else if (g_model.moduleData[port].failsafeMode != FAILSAFE_NOT_SET && g_model.moduleData[port].failsafeMode != FAILSAFE_RECEIVER) {
    if (failsafeCounter[port]-- == 0) {
      failsafeCounter[port] = 1000;
      flag1 |= PXX_SEND_FAILSAFE;
    }
    if (failsafeCounter[port] == 0 && g_model.moduleData[port].channelsCount > 0) {
      flag1 |= PXX_SEND_FAILSAFE;
    }
  }

  putPcmByte(flag1, port);

  /* FLAG2 */
  putPcmByte(0, port);

  /* PPM */
  static uint32_t pass[NUM_MODULES] = { MODULES_INIT(0) };
  int sendUpperChannels = 0;
  if (pass[port]++ & 0x01) {
    sendUpperChannels = g_model.moduleData[port].channelsCount;
  }
  for (int i=0; i<8; i++) {
    if (flag1 & PXX_SEND_FAILSAFE) {
      if (g_model.moduleData[port].failsafeMode == FAILSAFE_HOLD) {
        pulseValue = (i < sendUpperChannels ? 4095 : 2047);
      }
      else if (g_model.moduleData[port].failsafeMode == FAILSAFE_NOPULSES) {
        pulseValue = (i < sendUpperChannels ? 2048 : 0);
      }
      else {
        if (i < sendUpperChannels) {
          int channel = 8 + g_model.moduleData[port].channelsStart + i;
          int16_t failsafeValue = g_model.moduleData[port].failsafeChannels[channel];
          if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
            pulseValue = 4095;
          }
          else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
            pulseValue = 2048;
          }
          else {
            failsafeValue += 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
            pulseValue = limit(2049, (failsafeValue * 512 / 682) + 3072, 4094);
          }
        }
        else {
          int channel = g_model.moduleData[port].channelsStart + i;
          int16_t failsafeValue = g_model.moduleData[port].failsafeChannels[channel];
          if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
            pulseValue = 2047;
          }
          else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
            pulseValue = 0;
          }
          else {
            failsafeValue += 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
            pulseValue = limit(1, (failsafeValue * 512 / 682) + 1024, 2046);
          }
        }
      }
    }
    else {
      if (i < sendUpperChannels) {
        int channel = 8 + g_model.moduleData[port].channelsStart + i;
        int value = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
        pulseValue = limit(2049, (value * 512 / 682) + 3072, 4094);
      }
      else if (i < NUM_CHANNELS(port)) {
        int channel = g_model.moduleData[port].channelsStart + i;
        int value = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
        pulseValue = limit(1, (value * 512 / 682) + 1024, 2046);
      }
      else {
        pulseValue = 1024;
      }
    }

    if (i & 1) {
      putPcmByte(pulseValueLow, port); // Low byte of channel
      putPcmByte(((pulseValueLow >> 8) & 0x0F) | (pulseValue << 4), port);  // 4 bits each from 2 channels
      putPcmByte(pulseValue >> 4, port);  // High byte of channel
    }
    else {
      pulseValueLow = pulseValue;
    }
  }

  /* CRC16 */
  putPcmByte(0, port);
  pulseValue = modulePulsesData[port].pxx.pcmCrc;
  putPcmByte(pulseValue>>8, port);
  putPcmByte(pulseValue, port);

  /* Sync */
  putPcmHead(port);

  putPcmFlush(port);
}
