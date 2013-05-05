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

#include "opentx.h"

#if NUM_MODULES == 2
  #define MODULES_INIT(x) x, x
#else
  #define MODULES_INIT(x) x
#endif

uint8_t s_pulses_paused = 0;
uint8_t s_current_protocol[NUM_MODULES] = { MODULES_INIT(255) };
uint32_t failsafeCounter[NUM_MODULES] = { MODULES_INIT(100) };

uint8_t pxxFlag[NUM_MODULES] = { MODULES_INIT(0) };
uint16_t pxxStream[NUM_MODULES][400] ;                // Transitions
uint16_t *pxxStreamPtr[NUM_MODULES] ;
uint16_t PxxValue[NUM_MODULES] ;
uint16_t PcmCrc[NUM_MODULES] ;
uint8_t  PcmOnesCount[NUM_MODULES] ;

#if NUM_MODULES == 2
uint16_t ppmStream[NUM_MODULES][20]  = { { 2000, 2200, 2400, 2600, 2800, 3000, 3200, 3400, 9000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                         { 2000, 2200, 2400, 2600, 2800, 3000, 3200, 3400, 9000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
#else
uint16_t ppmStream[NUM_MODULES][20]  = { { 2000, 2200, 2400, 2600, 2800, 3000, 3200, 3400, 9000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
#endif

#if defined(PCBSKY9X)
volatile uint32_t ppmStreamIndex[NUM_MODULES] = { MODULES_INIT(0) };  // Modified in interrupt routine
uint8_t Bit_pulses[64] ;                          // Likely more than we need
uint8_t *Pulses2MHzptr ;
#endif

// DSM2 control bits
#define DSM2_CHANS     6
#define BIND_BIT       0x80
#define RANGECHECK_BIT 0x20
#define FRANCE_BIT     0x10
#define DSMX_BIT       0x08
#define BAD_DATA       0x47

uint8_t Serial_byte ;
uint8_t Serial_bit_count;
uint8_t Serial_byte_count ;

void setupPulsesPPM(unsigned int port)                   // Don't enable interrupts through here
{
#if defined(PCBSKY9X)
  register Pwm *pwmptr = PWM;
#endif

  int16_t PPM_range = g_model.extendedLimits ? 640 * 2 : 512 * 2; //range of 0.7..1.7msec

  // Total frame length = 22.5msec
  // each pulse is 0.7..1.7ms long with a 0.3ms stop tail
  // The pulse ISR is 2mhz that's why everything is multiplied by 2

  uint32_t firstCh = g_model.moduleData[port].channelsStart;
  uint32_t lastCh = min<unsigned int>(NUM_CHNOUT, firstCh + 8 + g_model.moduleData[port].channelsCount);

#if defined(PCBSKY9X)
  uint32_t pwmCh;
  if (port == 0) {
    pwmCh = 3;
  }
  else {
    pwmCh = 1;
  }
#endif
  uint16_t * ptr = ppmStream[port];

#if defined(PCBSKY9X)
  pwmptr->PWM_CH_NUM[pwmCh].PWM_CDTYUPD = (g_model.moduleData[port].ppmDelay * 50 + 300) * 2; //Stoplen *2
  if (g_model.moduleData[port].ppmPulsePol)
    pwmptr->PWM_CH_NUM[pwmCh].PWM_CMR |= 0x00000200 ;   // CPOL
  else
    pwmptr->PWM_CH_NUM[pwmCh].PWM_CMR &= ~0x00000200 ;  // CPOL
#endif

  uint32_t rest = 22500u * 2; //Minimum Framelen=22.5 ms
  rest += (int32_t(g_model.moduleData[port].ppmFrameLength)) * 1000;
  for (uint32_t i=firstCh; i<lastCh; i++) {
    int16_t v = limit((int16_t)-PPM_range, channelOutputs[i], (int16_t)PPM_range) + 2*PPM_CH_CENTER(i);
    rest -= v;
    *ptr++ = v; /* as Pat MacKenzie suggests */
  }
  rest = (rest > 65535) ? 65535 : rest;
  *ptr = rest;
  *(ptr + 1) = 0;

#if defined(PCBTARANIS)
  if (port == EXTERNAL_MODULE) {
    TIM8->CCR2 = rest - 1000;             // Update time
    TIM8->CCR1 = (g_model.moduleData[port].ppmDelay*50+300)*2;
  }
  else {
    TIM1->CCR2 = rest - 1000;             // Update time
    TIM1->CCR3 = (g_model.moduleData[port].ppmDelay*50+300)*2;
  }
#endif
}

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
  PcmCrc[port]=(PcmCrc[port]<<8)^(CRCTable[((PcmCrc[port]>>8)^data) & 0xFF]);
}

void putPcmPart(uint8_t value, unsigned int port)
{
  PxxValue[port] += 18 ;                                        // Output 1 for this time
  *pxxStreamPtr[port]++ = PxxValue[port] ;
  PxxValue[port] += 14 ;
  if (value) {
    PxxValue[port] += 16 ;
  }
  *pxxStreamPtr[port]++ = PxxValue[port] ;  // Output 0 for this time
}

void putPcmFlush(unsigned int port)
{
  *pxxStreamPtr[port]++ = 18010 ;             // Past the 18000 of the ARR
}

void putPcmBit(uint8_t bit, unsigned int port)
{
  if (bit)
  {
    PcmOnesCount[port] += 1 ;
    putPcmPart(1, port);
  }
  else
  {
    PcmOnesCount[port] = 0 ;
    putPcmPart(0, port);
  }
  if (PcmOnesCount[port] >= 5)
  {
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

#if defined(PCBSKY9X)
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
#endif

void setupPulsesPXX(unsigned int port)
{
  uint16_t chan=0, chan_low=0;

#if defined(PCBTARANIS)
  pxxStreamPtr[port] = pxxStream[port];
  PxxValue[port] = 0 ;
#endif

  PcmCrc[port] = 0;
  PcmOnesCount[port] = 0;

  /* Sync */
  putPcmHead(port);

  /* Rx Number */
  putPcmByte(g_model.header.modelId, port);

  /* FLAG1 */
  uint8_t flag1;
  if (pxxFlag[port] & PXX_SEND_RXNUM) {
    flag1 = (g_model.moduleData[port].rfProtocol << 6) | (g_eeGeneral.countryCode << 1) | pxxFlag[port];
  }
  else {
    flag1 = (g_model.moduleData[port].rfProtocol << 6) | pxxFlag[port];

    if (g_model.moduleData[port].failsafeMode != FAILSAFE_HOLD) {
      if (failsafeCounter[port]-- == 0) {
        failsafeCounter[port] = 1000;
        flag1 |= PXX_SEND_FAILSAFE;
      }
      if (failsafeCounter[port] == 0 && g_model.moduleData[port].channelsCount > 0) {
        flag1 |= PXX_SEND_FAILSAFE;
      }
    }
  }

  putPcmByte(flag1, port);

  /* FLAG2 */
  putPcmByte(0, port);

  /* PPM */
  static uint32_t pass[NUM_MODULES] = {0};
  uint32_t sendUpperChannels = 0;
  if (pass[port]++ & 0x01) {
    sendUpperChannels = g_model.moduleData[port].channelsCount;
  }
  for (uint32_t i=0; i<8; i++) {
    if (flag1 & PXX_SEND_FAILSAFE) {
      if (g_model.moduleData[port].failsafeMode == FAILSAFE_NOPULSES) {
        if (i < sendUpperChannels)
          chan = 3072;
        else
          chan = 1024;
      }
      else {
        if (i < sendUpperChannels) {
          chan =  limit(2048, PPM_CH_CENTER(8+g_model.moduleData[port].channelsStart+i) - PPM_CENTER + (g_model.moduleData[port].failsafeChannels[8+g_model.moduleData[port].channelsStart+i] * 512 / 682) + 3072, 4095);
          if (chan == 3072) chan = 3073;
        }
        else {
          chan = limit(0, PPM_CH_CENTER(g_model.moduleData[port].channelsStart+i) - PPM_CENTER + (g_model.moduleData[port].failsafeChannels[g_model.moduleData[port].channelsStart+i] * 512 / 682) + 1024, 2047);
          if (chan == 1024) chan = 1025;
        }
      }
    }
    else {
      if (i < sendUpperChannels)
        chan =  limit(2048, PPM_CH_CENTER(8+g_model.moduleData[port].channelsStart+i) - PPM_CENTER + (channelOutputs[8+g_model.moduleData[port].channelsStart+i] * 512 / 682) + 3072, 4095);
      else
        chan = limit(0, PPM_CH_CENTER(g_model.moduleData[port].channelsStart+i) - PPM_CENTER + (channelOutputs[g_model.moduleData[port].channelsStart+i] * 512 / 682) + 1024, 2047);
    }

    if (i & 1) {
      putPcmByte(chan_low, port); // Low byte of channel
      putPcmByte(((chan_low >> 8) & 0x0F) | (chan << 4), port);  // 4 bits each from 2 channels
      putPcmByte(chan >> 4, port);  // High byte of channel
    }
    else {
      chan_low = chan;
    }
  }

  /* CRC16 */
  putPcmByte(0, port);
  chan = PcmCrc[port];
  putPcmByte(chan>>8, port);
  putPcmByte(chan, port);

  /* Sync */
  putPcmHead(port);

  putPcmFlush(port);
}

#if defined(DSM2)
#define BITLEN_DSM2 (8*2) //125000 Baud => 8uS per bit
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
#endif

#if defined(PCBSKY9X) && !defined(SIMU)
extern "C" void PWM_IRQHandler(void)
{
  register Pwm *pwmptr;
  register Ssc *sscptr;
  uint32_t period;
  uint32_t reason;

  pwmptr = PWM;
  reason = pwmptr->PWM_ISR1 ;
  if (reason & PWM_ISR1_CHID3) {
    switch (s_current_protocol[0]) // Use the current, don't switch until set_up_pulses
    {
      case PROTO_PXX:
        // Alternate periods of 15.5mS and 2.5 mS
        period = pwmptr->PWM_CH_NUM[3].PWM_CPDR;
        if (period == 5000) { // 2.5 mS
          period = 15500 * 2;
        }
        else {
          period = 5000;
        }
        pwmptr->PWM_CH_NUM[3].PWM_CPDRUPD = period; // Period in half uS
        if (period != 5000) { // 2.5 mS
          setupPulses(0);
        }
        else {
          // Kick off serial output here
          sscptr = SSC;
          sscptr->SSC_TPR = (uint32_t) Bit_pulses;
          sscptr->SSC_TCR = Serial_byte_count;
          sscptr->SSC_PTCR = SSC_PTCR_TXTEN; // Start transfers
        }
        break;

      case PROTO_DSM2_LP45:
      case PROTO_DSM2_DSM2:
      case PROTO_DSM2_DSMX:
        // Alternate periods of 19.5mS and 2.5 mS
        period = pwmptr->PWM_CH_NUM[3].PWM_CPDR;
        if (period == 5000) // 2.5 mS
            {
          period = 19500 * 2;
        }
        else {
          period = 5000;
        }
        pwmptr->PWM_CH_NUM[3].PWM_CPDRUPD = period; // Period in half uS
        if (period != 5000) // 2.5 mS
        {
          setupPulses(0);
        }
        else {
          // Kick off serial output here
          sscptr = SSC;
          sscptr->SSC_TPR = (uint32_t) Bit_pulses;
          sscptr->SSC_TCR = Serial_byte_count;
          sscptr->SSC_PTCR = SSC_PTCR_TXTEN; // Start transfers
        }
        break;

      default:
        pwmptr->PWM_CH_NUM[3].PWM_CPDRUPD = ppmStream[0][ppmStreamIndex[0]++]; // Period in half uS
        if (ppmStream[0][ppmStreamIndex[0]] == 0) {
          ppmStreamIndex[0] = 0;
          setupPulses(0);
        }
        break;

    }
  }

  if (reason & PWM_ISR1_CHID1) {
    pwmptr->PWM_CH_NUM[1].PWM_CPDRUPD = ppmStream[1][ppmStreamIndex[1]++] ;  // Period in half uS
    if (ppmStream[1][ppmStreamIndex[1]] == 0) {
      ppmStreamIndex[1] = 0 ;
      setupPulsesPPM(1) ;
    }
  }
}
#endif

void setupPulses(unsigned int port)
{
  heartbeat |= (HEART_TIMER_PULSES << port);

#if defined(PCBTARANIS)
  if (port) port = 1;			// Ensure is 0 or 1 only
  uint8_t required_protocol;
  
  if (port == INTERNAL_MODULE) 
  	required_protocol = g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF ? PROTO_NONE : PROTO_PXX;
  else {
  	switch (g_model.externalModule) {
  	  case MODULE_TYPE_PPM:
  		  required_protocol = PROTO_PPM;
  		  break;
  	  case MODULE_TYPE_XJT:
      case MODULE_TYPE_DJT:
      	required_protocol = PROTO_PXX;
        break;
      default:
        required_protocol = PROTO_NONE;
    }
  }
#else
  uint8_t required_protocol = g_model.protocol;
#endif

  if (s_pulses_paused)
    required_protocol = PROTO_NONE;

  if (s_current_protocol[port] != required_protocol) {

    switch (s_current_protocol[port]) { // stop existing protocol hardware
      case PROTO_PXX:
#if defined(PCBTARANIS)
        disable_pxx(port);
#else
        disable_pxx();
#endif
        break;
#if defined(DSM2)
      case PROTO_DSM2_LP45:
      case PROTO_DSM2_DSM2:
      case PROTO_DSM2_DSMX:
        disable_ssc();
        break;
#endif
      default:
#if defined(PCBTARANIS)
        disable_ppm(port);
#else
        disable_main_ppm();
#endif
        break;
    }

    s_current_protocol[port] = required_protocol;

#if defined(PCBTARANIS)
    switch (required_protocol) {
      case PROTO_PXX:
        init_pxx(port);
        break;
      case PROTO_PPM:
        init_ppm(port);
        break;
    }
#elif defined(PCBSKY9X)
    switch (required_protocol) { // Start new protocol hardware here
      case PROTO_PXX:
        init_main_ppm(5000, 0); // Initial period 2.5 mS, output off
        init_ssc();
        break;
      case PROTO_DSM2_LP45:
      case PROTO_DSM2_DSM2:
      case PROTO_DSM2_DSMX:
        init_main_ppm(5000, 0); // Initial period 2.5 mS, output off
        init_ssc();
        break;
      case PROTO_NONE:
        init_main_ppm(3000, 0); // Initial period 1.5 mS, output off
        break;
      default:
        init_main_ppm(3000, 1); // Initial period 1.5 mS, output on
        break;
    }
#endif
  }

  // Set up output data here
  switch (required_protocol) {
    case PROTO_PXX:
      setupPulsesPXX(port);
      break;
#if defined(DSM2)
    case PROTO_DSM2_LP45:
    case PROTO_DSM2_DSM2:
    case PROTO_DSM2_DSMX:
      setupPulsesDsm2(6);
      break;
#endif
#if defined(PCBTARANIS)
    case PROTO_PPM:
#else
    default:
#endif
      setupPulsesPPM(port); // Don't enable interrupts through here
      break ;
  }
}
