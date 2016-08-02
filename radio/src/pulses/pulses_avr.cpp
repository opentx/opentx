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

uint16_t nextMixerEndTime = 0;
#define SCHEDULE_MIXER_END(delay) nextMixerEndTime = getTmr16KHz() + (delay) - 2*16/*2ms*/

#if defined(DSM2)
  // DSM2 control bits
  #define DSM2_CHANS                         6
  #define FRANCE_BIT                         0x10
  #define DSMX_BIT                           0x08
  #define BAD_DATA                           0x47
  #define DSM2_SEND_BIND                     (1 << 7)
  #define DSM2_SEND_RANGECHECK               (1 << 5)
  uint8_t  dsm2BindTimer = DSM2_BIND_TIMEOUT;
#endif

#if defined(PXX)
  #define PXX_SEND_BIND                      0x01
  #define PXX_SEND_FAILSAFE                  (1 << 4)
  #define PXX_SEND_RANGECHECK                (1 << 5)
#endif

#if defined(DSM2) || defined(PXX)
uint8_t moduleFlag[NUM_MODULES] = { 0 };
#endif

uint8_t s_current_protocol[1] = { 255 };
uint8_t s_pulses_paused = 0;

uint16_t B3_comp_value;

#if defined(DSM2_SERIAL)
inline void DSM2_EnableTXD(void)
{
  UCSR0B |= (1 << TXEN0); // enable TX
  // don't enable UDRE0 interrupt now, it will be enabled during next setupPulses
}
#endif

void set_timer3_capture(void);
void set_timer3_ppm(void);

void startPulses()
{
#if defined(CPUM2560)
#if defined(DSM2_SERIAL)
  if (!IS_DSM2_PROTOCOL(g_model.protocol))
#endif
  {
    // TODO g: There has to be a better place for this bug fix
    OCR1B = 0xffff; /* Prevent any PPM_OUT pin toggle before the TCNT1 interrupt
                       fires for the first time and sets up the pulse period. 
                       *** Prevents WDT reset loop. */
  }
#endif

#if defined(SIMU)
  s_current_protocol[0] = g_model.protocol;
#else
  setupPulses();
#endif // SIMU
}

#define PULSES_SIZE       144
uint8_t pulses2MHz[PULSES_SIZE] = {0}; // TODO check this length, pulled from er9x, perhaps too big. 
uint8_t *pulses2MHzRPtr = pulses2MHz;

#if defined(DSM2) || defined(PXX) || defined(IRPROTOS)
uint8_t *pulses2MHzWPtr = pulses2MHz;
#endif

#define CTRL_END 0
#define CTRL_CNT 1
#define CTRL_REP_1CMD -3
#define CTRL_REP_2CMD -6

#define SETUP_PULSES_DURATION 1000 // 500us
uint8_t g_ppmPulsePolarity = 0; // Needed for Bit-bang PPM.

// TIMER1_COMPA_vect used for PPM and DSM2=SERIAL.
ISR(TIMER1_COMPA_vect) // 2MHz pulse generation (BLOCKING ISR).
{
  uint8_t dt = TCNT1L; // Record Timer1 latency for DEBUG stats display.

  // Call setupPulses only after "rest" period has elapsed.
  // Must do this before toggle PORTB to keep timing accurate.
  if (IS_DSM2_SERIAL_PROTOCOL(s_current_protocol[0]) || *((uint16_t*)pulses2MHzRPtr) == 0) {
    if (!IS_DSM2_SERIAL_PROTOCOL(s_current_protocol[0])) {
      OCR1A = SETUP_PULSES_DURATION;
#if defined(CPUM2560) // CPUM2560 hardware toggled PPM out.
      OCR1B = OCR1A;
      if (g_model.pulsePol) {
        TCCR1A = (TCCR1A | (1<<COM1B1)) & ~(1<<COM1B0); // Set idle level.
      }
      else {
        TCCR1A |= 3<<COM1B0;
      }
      TCCR1C = 1<<FOC1B; // Strobe FOC1B.
      TCCR1A = (TCCR1A | (1<<COM1B0)) & ~(1<<COM1B1); // Toggle OC1B on next match.
#endif
    }
    setupPulses(); // Does not sei() for setupPulsesPPM.
    heartbeat |= HEART_TIMER_PULSES;
    return;
  }

  if (s_current_protocol[0] != PROTO_NONE) {
#if !defined(CPUM2560)
    // Original Bit-bang for PPM.
    if (g_ppmPulsePolarity) {
      PORTB |= (1<<OUT_B_PPM); // GCC optimisation should result in a single SBI instruction
      g_ppmPulsePolarity = 0;
    }
    else {
      PORTB &= ~(1<<OUT_B_PPM);
      g_ppmPulsePolarity = 1;
    }
#else // defined(CPUM2560)
    // CPUM2560 hardware toggled PPM out.
    if (*(uint16_t*)(pulses2MHzRPtr + sizeof(uint16_t)) == 0) {
      // Look one step ahead to see if we are currently the "rest" period.
      OCR1B = 0xffff; // Prevent next compare match hence toggle.
    }
    else {
      OCR1B = *((uint16_t*) pulses2MHzRPtr);
    }
#endif
  }

  OCR1A = *((uint16_t*) pulses2MHzRPtr); // Schedule next Timer1 interrupt vector (to this function).
  pulses2MHzRPtr += sizeof(uint16_t); // Non PPM protocols use uint8_t pulse buffer.

  if (dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if (dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;
}

void setupPulsesPPM(uint8_t proto)
{
  // Total frame length is a fixed 22.5msec (more than 9 channels is non-standard and requires this to be extended.)
  // Each channel's pulse is 0.7 to 1.7ms long, with a 0.3ms stop tail, making each compelte cycle 1 to 2ms.

  int16_t PPM_range = g_model.extendedLimits ? 640*2 : 512*2;   //range of 0.7..1.7msec

  uint16_t *ptr = (proto == PROTO_PPM ? (uint16_t *)pulses2MHz : (uint16_t *) &pulses2MHz[PULSES_SIZE/2]);

  //The pulse ISR is 2mhz that's why everything is multiplied by 2
  uint8_t p = (proto == PROTO_PPM16 ? 16 : 8) + (g_model.ppmNCH * 2); //Channels *2
  uint16_t q = (g_model.ppmDelay*50+300)*2; // Stoplen *2
  int32_t rest = 22500u*2 - q;

  rest += (int32_t(g_model.ppmFrameLength))*1000;
  for (uint8_t i=(proto==PROTO_PPM16) ? p-8 : 0; i<p; i++) {
    int16_t v = limit((int16_t)-PPM_range, channelOutputs[i], (int16_t)PPM_range) + 2*PPM_CH_CENTER(i);
    rest -= v;
    *ptr++ = q;
    *ptr++ = v - q; // total pulse width includes stop phase
  }

  *ptr++ = q;  
  if (rest > 65535) rest = 65535; /* prevents overflows */
  if (rest < 9000)  rest = 9000;

  if (proto == PROTO_PPM) {
    *ptr++ = rest - SETUP_PULSES_DURATION;
    pulses2MHzRPtr = pulses2MHz;
  }
  else {
    *ptr++ = rest;    
    B3_comp_value = rest - SETUP_PULSES_DURATION;               // 500uS before end of sync pulse
  }

  *ptr = 0;
}


#if defined(PXX)
const pm_uint16_t CRCTable[] PROGMEM =
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

uint8_t PcmByte;
uint8_t PcmBitCount;
uint16_t PcmCrc;
uint8_t PcmOnesCount;

void crc(uint8_t data)
{
  PcmCrc = (PcmCrc<<8) ^ pgm_read_word(&CRCTable[((PcmCrc >> 8) ^ data) & 0xFF]);
}


void putPcmPart(uint8_t value)
{
  PcmByte >>= 2;
  PcmByte |= value;
  if (++PcmBitCount >= 4) {
    *pulses2MHzWPtr++ = PcmByte;
    PcmBitCount = PcmByte = 0;
  }
}


void putPcmFlush()
{
  while (PcmBitCount != 0) {
    putPcmPart(0); // Empty
  }
  *pulses2MHzWPtr = 0; // Mark end
}

void putPcmBit(uint8_t bit)
{
  if (bit) {
    PcmOnesCount += 1;
    putPcmPart(0x80);
  }
  else {
    PcmOnesCount = 0;
    putPcmPart(0xC0);
  }
  if (PcmOnesCount >= 5) {
    putPcmBit(0);                                // Stuff a 0 bit in
  }
}

void putPcmByte(uint8_t byte)
{
  uint8_t i;

  crc(byte);

  for (i=0; i<8; i++) {
    putPcmBit(byte & 0x80);
    byte <<= 1;
  }
}


void putPcmHead()
{
  // send 7E, do not CRC
  // 01111110
  putPcmPart(0xC0);
  putPcmPart(0x80);
  putPcmPart(0x80);
  putPcmPart(0x80);
  putPcmPart(0x80);
  putPcmPart(0x80);
  putPcmPart(0x80);
  putPcmPart(0xC0);
}

uint16_t scaleForPXX(uint8_t i)
{
  int16_t value = ((i < 16) ? channelOutputs[i] * 3 / 4 : 0) + 1024;
  return limit<uint16_t>(1, value, 2046);
}

void setupPulsesPXX()
{
  uint16_t chan;
  uint16_t chan_1;

  pulses2MHzWPtr = pulses2MHz;
  pulses2MHzRPtr = pulses2MHz;

  PcmCrc = 0;
  PcmBitCount = PcmByte = 0;
  PcmOnesCount = 0;
  putPcmHead();
  putPcmByte(g_model.header.modelId[0]);
  uint8_t flag1 = 0;
  if (moduleFlag[0] == MODULE_BIND) {
    flag1 |= (g_eeGeneral.countryCode << 1) | PXX_SEND_BIND;
  }
  else if (moduleFlag[0] == MODULE_RANGECHECK) {
    flag1 |= PXX_SEND_RANGECHECK;
  }
  putPcmByte(flag1);     // First byte of flags
  putPcmByte(0);     // Second byte of flags
  for (uint8_t i=0; i<8; i+=2) {              // First 8 channels only
    chan = scaleForPXX(i);
    chan_1 = scaleForPXX(i+1);
    putPcmByte(chan); // Low byte of channel
    putPcmByte(((chan >> 8) & 0x0F) | (chan_1 << 4));  // 4 bits each from 2 channels
    putPcmByte(chan_1 >> 4);  // High byte of channel
  }
  putPcmByte(0);
  chan = PcmCrc;                     // get the crc
  putPcmByte(chan>>8);                        // Checksum lo
  putPcmByte(chan); // Checksum hi
  putPcmHead();
  putPcmFlush();
  OCR1C += 40000;            // 20mS on
  PORTB |= (1<<OUT_B_PPM);
}
#endif

#if defined(DSM2_SERIAL)

// DSM2 protocol pulled from th9x - Thanks thus!!!

//http://www.rclineforum.de/forum/board49-zubeh-r-elektronik-usw/fernsteuerungen-sender-und-emp/neuer-9-kanal-sender-f-r-70-au/Beitrag_3897736#post3897736
//(dsm2(LP4DSM aus den RTF (Ready To Fly) Sendern von Spektrum.
//http://www.rcgroups.com/forums/showpost.php?p=18554028&postcount=237
// /home/thus/txt/flieger/PPMtoDSM.c
/*
  125000 Baud 8n1      _ xxxx xxxx - ---
#define DSM2_CHANNELS      6                // Max number of DSM2 Channels transmitted
#define DSM2_BIT (8*2)
bind:
  DSM2_Header = 0x80,0
static byte DSM2_Channel[DSM2_CHANNELS*2] = {
                ch
  0x00,0xAA,     0 0aa
  0x05,0xFF,     1 1ff
  0x09,0xFF,     2 1ff
  0x0D,0xFF,     3 1ff
  0x13,0x54,     4 354
  0x14,0xAA      5 0aa
};

normal:
  DSM2_Header = 0,0;
  DSM2_Channel[i*2]   = (byte)(i<<2) | highByte(pulse);
  DSM2_Channel[i*2+1] = lowByte(pulse);

 */

// DSM2=SERIAL mode
FORCEINLINE void setupPulsesDSM2()
{
  uint16_t *ptr = (uint16_t *)pulses2MHz;
  switch (s_current_protocol[0])
  {
    case PROTO_DSM2_LP45:
      *ptr = 0x00;
      break;
    case PROTO_DSM2_DSM2:
      *ptr = 0x10;
      break;
    default: // DSMX
      *ptr = 0x18;
      break;
  }

  if (dsm2BindTimer > 0) {
    dsm2BindTimer--;
    if (switchState(SW_DSM2_BIND)) {
      moduleFlag[0] = MODULE_BIND;
      *ptr |= DSM2_SEND_BIND;
    }
  }
  else if (moduleFlag[0] == MODULE_RANGECHECK) {
    *ptr |= DSM2_SEND_RANGECHECK;
  }
  else {
    moduleFlag[0] = 0;
  }

  ptr++;
  *ptr++ = g_model.header.modelId[0];

  for (uint8_t i=0; i<DSM2_CHANS; i++) {
    uint16_t pulse = limit(0, ((channelOutputs[i]*13)>>5)+512,1023);
    *ptr++ = (i<<2) | ((pulse>>8)&0x03); // encoded channel + upper 2 bits pulse width
    *ptr++ = pulse & 0xff; // low byte
  }

  pulses2MHzWPtr = (uint8_t *)ptr;
  pulses2MHzRPtr = pulses2MHz;

  UCSR0B |= (1 << UDRIE0); // enable UDRE0 interrupt to start transmitting DSM2 data bytes from buffer
}

// DSM2=SERIAL mode
void DSM2_Done()
{
  UCSR0B &= ~((1 << TXEN0) | (1 << UDRIE0)); // disable UART TX and interrupt
}

// DSM2=SERIAL mode
void DSM2_Init(void)
{
#ifndef SIMU

#undef BAUD
#define BAUD 125000

#include <util/setbaud.h>

  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0A &= ~(1 << U2X0); // disable double speed operation.

  // set 8N1 (leave TX and RX disabled for now)
  UCSR0B = 0 | (0 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) | (0 << RXEN0) | (0 << TXEN0) | (0 << UCSZ02);
  UCSR0C = 0 | (1 << UCSZ01) | (1 << UCSZ00);

  while (UCSR0A & (1 << RXC0)) {
    UDR0;  // flush receive buffer
  }

  // These should be running right from power up on a FrSky enabled '9X.
  DSM2_EnableTXD(); // enable DSM2 UART transmitter

#endif // SIMU
}
#endif // defined(DSM2_SERIAL)

/****** END DSM2=SERIAL ********/



/////////////////////////////////////////////////////////////



/******* BEGIN DSM2=PPM ********/

#if defined(DSM2_PPM)
inline void _send_1(uint8_t v)
{
  *pulses2MHzWPtr++ = v;
}

#define BITLEN_DSM2 (8*2) //125000 Baud
void sendByteDsm2(uint8_t b) //max 10changes 0 10 10 10 10 1
{
  bool lev = 0;
  uint8_t len = BITLEN_DSM2; //max val: 9*16 < 256
  for (uint8_t i=0; i<=8; i++) { //8Bits + Stop=1
    bool nlev = b & 1; //lsb first
    if (lev == nlev) {
      len += BITLEN_DSM2;
    }
    else {
#if defined(CPUM2560)
      // G: Compensate for main clock synchronisation -- to get accurate 8us bit length
      // NOTE: This has now been tested as NOT required on the stock board, with the ATmega64A chip.
      _send_1(nlev ? len-5 : len+3);
#else
      _send_1(len-1);
#endif
      len  = BITLEN_DSM2;
      lev  = nlev;
    }
    b = (b>>1) | 0x80; //shift in stop bit
  }
#if defined (CPUM2560)
  _send_1(len+BITLEN_DSM2+3); // 2 stop bits
#else
  _send_1(len+BITLEN_DSM2-1); // 2 stop bits
#endif
}

// DSM2=PPM mode
void setupPulsesDSM2()
{
  static uint8_t dsmDat[2+6*2] = {0xFF,0x00, 0x00,0xAA, 0x05,0xFF, 0x09,0xFF, 0x0D,0xFF, 0x13,0x54, 0x14,0xAA};

  pulses2MHzWPtr = pulses2MHz;

  // If more channels needed make sure the pulses union/array is large enough
  switch (s_current_protocol[0]) {
    case PROTO_DSM2_LP45:
      dsmDat[0] = 0x00;
      break;
    case PROTO_DSM2_DSM2:
      dsmDat[0] = 0x10;
      break;
    default: // DSMX
      dsmDat[0] = 0x18;
      break;
  }

  if (dsm2BindTimer > 0) {
    dsm2BindTimer--;
    if (switchState(SW_DSM2_BIND)) {
      moduleFlag[0] = MODULE_BIND;
      dsmDat[0] |= DSM2_SEND_BIND;
    }
  }
  else if (moduleFlag[0] == MODULE_RANGECHECK) {
    dsmDat[0] |= DSM2_SEND_RANGECHECK;
  }
  else {
    moduleFlag[0] = 0;
  }

  dsmDat[1] = g_model.header.modelId[0];

  for (uint8_t i=0; i<DSM2_CHANS; i++) {
    uint16_t pulse = limit(0, ((channelOutputs[i]*13)>>5)+512, 1023);
    dsmDat[2+2*i] = (i<<2) | ((pulse>>8)&0x03); // encoded channel + upper 2 bits pulse width
    dsmDat[3+2*i] = pulse & 0xff; // low byte
  }

  for (uint8_t counter=0; counter<14; counter++) {
    sendByteDsm2(dsmDat[counter]);
  }

  pulses2MHzWPtr -= 1; //remove last stopbits and

#if !defined(CPUM2560)
//G: Removed to get waveform correct on analyser. Leave in for stock board until tests can be done.
  _send_1(255); // prolong them
#endif
  _send_1(0); //end of pulse stream
  
  pulses2MHzRPtr = pulses2MHz;
}
#endif

/****** END DSM2=PPM ********/

#if defined(IRPROTOS)
static void _send_u8(uint8_t u8)
{
#ifdef SIMU
  *(pulses2MHzPtr)++ = u8;
#else
  asm volatile(
    " st   Z+,%A[u]        \n\t"

    : [p]"=z"(pulses2MHzWPtr)
    :    "%[p]"(pulses2MHzWPtr)
    , [u]"r"(u8)
 );

#endif
}

static void _send_u16(uint16_t u16)
{
#ifdef  SIMU
  *(*(uint16_t**)&pulses2MHzWPtr)++ = u16;
#else
  asm volatile(
    " st   Z+,%A[t0]        \n\t"
    " st   Z+,%B[t0]        \n\t"

    : [p]"=z"(pulses2MHzWPtr)
    :    "%[p]"(pulses2MHzWPtr)
    , [t0]"r"(u16)
 );

#endif
}

static void _send_1(uint16_t t0)
{
  //  *(*(uint16_t**)&pulses2MHzPtr)++=t0;
  _send_u16(t0);
  *pulses2MHzWPtr++ = CTRL_CNT;
  //_send_u8(CTRL_CNT);
}

static void _send_rep1(uint16_t t0, uint8_t cnt)
{
  //  *(*(uint16_t**)&pulses2MHzPtr)++=t0;
  _send_u16(t0);
  _send_u8(CTRL_REP_1CMD);
  _send_u8(cnt);
  _send_u8(CTRL_CNT);
}

//picco z
//http://home.versanet.de/~b-konze/uni_fb/uni_fb.htm
// /home/husteret/txt/flieger/protokolle/m168fb_ufo_v08/picooz.c
//
// 1900  650 650         1226             650       1226           Stop
// ----   __  -- __ -- __----__----__     --__      ----____       --__
/*
  chn:2 a=00 b=01 c=10
  pow:4u msb first
  trim:4s  -2,0,1
  direction:3s
  -chk[0]
  -chk[1]
  0
-------
2-bit sum = 0

chk:2 = chn:2 + pow>>2:2 +pow:2 +  trim>>2:2 +trim:2 dir + direction>>1:2 + direction<<1:2

*/

#define PICOOZ_RC_HIGH   93 //(1226/13)
#define PICOOZ_RC_LOW    49 //(650/13)

#define LEN_38KHZ (13*2) //= 38,46KHz

void picco_sendB1(bool bit)
{
  if (bit) {
    _send_rep1(LEN_38KHZ-1, PICOOZ_RC_HIGH);     //ungerade anzahl 10101
    _send_1   (1226*2 - 1);   //lange 0
  }
  else {
    _send_rep1(LEN_38KHZ-1, PICOOZ_RC_LOW);      //ungerade anzahl 10101
    _send_1   (650*2  - 1);   //lange 0
  }
}

void picco_sendBn(uint8_t bits, uint8_t n)
{
  while (n--) {
    picco_sendB1(bits & (1<<n));
  }
}

#define BITS  10
#define BITS2 (BITS-1)

NOINLINE uint8_t reduce7u(int16_t v, uint8_t sfr)
{
  v += (1<<BITS2);
  if (v < 0) v = 0;
  if (v >= (1<<BITS)) v=(1<<BITS)-1;
  return v >> sfr;
}

NOINLINE int8_t reduce7s(int16_t v, uint8_t sfr, uint8_t sf2, int8_t ofs2)
{
  v += (1<<BITS2) + sf2;
  if (v & (1<<BITS)) {
    v = (1<<BITS)-1; // no overflow
  }
  int8_t  i8 = (uint16_t)v>>sfr;
  if (i8 <= 0) {
    i8 = 1;
  }
  i8 -= ofs2;
  if (i8 >= ofs2) {
    i8 = ofs2-1; //no overflow
  }
  return i8;
}

//these defines allow the compiler to preclculate constants
#define getChan7u(i,bitsRes) reduce7u(channelOutputs[i],(BITS-bitsRes))
#define getChan7s(i,bitsRes) reduce7s(channelOutputs[i],(BITS-bitsRes),1<<((BITS-bitsRes)-1),1<<(bitsRes-1))

static void setupPulsesPiccoZ(uint8_t chn)
{
  // 1900  650 650         1226             650 0bit  1226 1bit      Stop
  // ----   __  -- __ -- __----__----__     --__      ----____       --__
  static bool    state = 0;
  static uint8_t pow;
  static int8_t  trim;
  static int8_t  dir;
  static uint8_t chk;
  if (state == 0) {
    _send_rep1(LEN_38KHZ-1, 147);   // 1900/13  !! must be odd
    _send_1   (650*2 - 1);//
    picco_sendBn(0,2);
    _send_rep1(LEN_38KHZ-1, PICOOZ_RC_HIGH);
    _send_1   (650*2 - 1);//
    _send_rep1(LEN_38KHZ-1, PICOOZ_RC_HIGH);
    _send_1   (650*2 - 1);//
    //   chn:2 a=00 b=01 c=10
    //   pow:4u msb first
    //   trim:4s  -2,0,1
    //   dir:3s
    //   -chk[0]
    //   -chk[1]
    //   0
    pow  = getChan7u(2,4);
    trim = getChan7s(1,4);
    dir  = getChan7s(0,3);
    chk    = - (chn+ (pow>>2) + pow + (trim>>2) + trim + (dir>>1) + (dir<<1));
  }
  else {
    picco_sendBn(chn,2);
    picco_sendBn(pow,4);
    picco_sendBn(trim,4);
    picco_sendBn(dir,3);
    picco_sendB1(chk & (1<<0)); //lsb first, because we are here on a odd bit (dir is only 3 bits)
    picco_sendB1(chk & (1<<1));
    _send_rep1(LEN_38KHZ-1, PICOOZ_RC_LOW); //0-bit pulses
    _send_1   (20000u*2  - 1); //20ms gap ?
  }
  state = !state;
}
#endif

void setupPulses()
{
  uint8_t required_protocol = g_model.protocol;

#if defined(DEBUG) && !defined(VOICE)
  PORTH |= 0x80; // PORTH:7 LOW->HIGH signals start of setupPulses()
#endif

  if (s_pulses_paused) {
    required_protocol = PROTO_NONE;
  }

#if defined(CPUM2560) && defined(DSM2_PPM) && defined(TX_CADDY)
// This should be here, executed on every loop, to ensure re-setting of the 
// TX moudle power control output register, in case of electrical glitch.
// (Following advice of Atmel for MCU's used  in industrial / mission cricital 
// applications.)
  if (IS_DSM2_PROTOCOL(required_protocol))
    PORTH &= ~0x80;
  else
    PORTH |= 0x80;
#endif

  if (s_current_protocol[0] != required_protocol) {

#if defined(DSM2_SERIAL) && defined(TELEMETRY_FRSKY)
    if (s_current_protocol[0] == 255 || IS_DSM2_PROTOCOL(s_current_protocol[0])) {
      telemetryInit();
    }
#endif

    s_current_protocol[0] = required_protocol;

    TCCR1B = 0;                           // Stop counter
    TCNT1 = 0;

#if defined(CPUM2560) || defined(CPUM2561)
    TIMSK1 &= ~0x2F;                      // All Timer1 interrupts off
    TIMSK1 &= ~(1<<OCIE1C);               // COMPC1 off
    TIFR1 = 0x2F;
#else
    TIMSK &= ~0x3C;            // All interrupts off
    ETIMSK &= ~(1<<OCIE1C);    // COMPC1 off
    TIFR = 0x3C;               // Clear all pending interrupts
    ETIFR = 0x3F;              // Clear all pending interrupts
#endif

    switch (required_protocol) {
#if defined(DSM2_PPM) // For DSM2=SERIAL, the default: case is executed, below
      case PROTO_DSM2_LP45:
      case PROTO_DSM2_DSM2:
      case PROTO_DSM2_DSMX:
        set_timer3_capture(); 
        OCR1C = 200;                          // 100 uS
        TCNT1 = 300;                          // Past the OCR1C value
        ICR1 = 44000;                         // Next frame starts in 22 mS
#if defined(CPUM2560) || defined(CPUM2561)
        TIMSK1 |= 0x28;                       // Enable Timer1 COMPC and CAPT interrupts
        TCCR1A = (0 << WGM10);                // Set output waveform mode to normal, for now. Note that
                                              // WGM will be changed to toggle OCR1B pin on compare capture, 
                                              // in next switch(required_protocol) {...}, below
#else
        TIMSK |= 0x20;                        // Enable CAPT
        ETIMSK |= (1<<OCIE1C);                // Enable COMPC
        TCCR1A = (0 << WGM10);
#endif
        TCCR1B = (3 << WGM12) | (2 << CS10);  // CTC ICR, 16MHz / 8
        break;
#endif // defined(DSM2_PPM)

#if defined(PXX)
      case PROTO_PXX:
        set_timer3_capture(); 
        OCR1B = 6000;                         // Next frame starts in 3 mS
        OCR1C = 4000;                         // Next frame setup in 2 mS
#if defined(CPUM2560) || defined(CPUM2561)
        TIMSK1 |= (1<<OCIE1B);                // Enable COMPB
        TIMSK1 |= (1<<OCIE1C);                // Enable COMPC
        TCCR1A = (3 << COM1B0);               // Connect OC1B for hardware PPM switching
#else
        TIMSK |= (1<<OCIE1B);                 // Enable COMPB
        ETIMSK |= (1<<OCIE1C);                // Enable COMPC
        TCCR1A  = 0;
#endif
        TCCR1B  = (2<<CS10);                  // ICNC3 16MHz / 8
        break;
#endif

      case PROTO_PPM16:
        OCR1A = 40000;                        // Next frame starts in 20 mS
#if defined(CPUM2560) || defined(CPUM2561)
        TIMSK1 |= (1<<OCIE1A);                // Enable COMPA
        TCCR1A = (3 << COM1B0);               // Connect OC1B for hardware PPM switching
#else
        TIMSK |= 0x10;                        // Enable COMPA
        TCCR1A = (0<<WGM10);
#endif
        TCCR1B = (1 << WGM12) | (2<<CS10);   // CTC OCRA, 16MHz / 8
        setupPulsesPPM(PROTO_PPM16);
        OCR3A = 50000;
        OCR3B = 5000;
        set_timer3_ppm();
        break;

      case PROTO_PPMSIM:
#if defined(CPUM2560) || defined(CPUM2561)
        TCCR1A = 0;                           // Disconnect OC1B for bit-bang PPM switching
#endif
        setupPulsesPPM(PROTO_PPMSIM);
        OCR3A = 50000; 
        OCR3B = 5000; 
        set_timer3_ppm(); 
        PORTB &= ~(1<<OUT_B_PPM);             // Hold PPM output low
        break;

#if defined(DSM2_SERIAL) && defined(TELEMETRY_FRSKY)
      case PROTO_DSM2_LP45:
      case PROTO_DSM2_DSM2:
      case PROTO_DSM2_DSMX:
        DSM2_Init();
        // no break
#endif

      default: // PPM and DSM2=SERIAL modes
        set_timer3_capture(); 
        OCR1A = 44000;                        // Next frame starts in 22ms -- DSM mode. 
                                              //    This is arbitrary and for the first frame only. In fact, ... 
                                              //    DSM2 mode will set frame timing again at each ISR(TIMER1_COMPC_vect)    
                                              //       and
                                              //    PPM mode will dynamically adjust to the frame rate set in model SETUP menu, 
                                              //    from within setupPulsesPPM().
#if defined(CPUM2560) || defined(CPUM2561)
        TIMSK1 |= (1<<OCIE1A);                // Enable COMPA
        TCCR1A = (3 << COM1B0);               // Connect OC1B for hardware PPM switching. G: Not needed
                                              // for DSM2=SERIAL. But OK.
#else
        TIMSK |= 0x10;                        // Enable COMPA
        TCCR1A = (0 << WGM10);
#endif
        TCCR1B = (1 << WGM12) | (2 << CS10);  // CTC OCRA, 16MHz / 8
        break;
    }
  }

  switch(required_protocol) {
#if defined(PXX)
    case PROTO_PXX:
      // schedule next Mixer calculations
      SCHEDULE_MIXER_END(20*16);
      sei();
      setupPulsesPXX();
      break;
#endif

#if defined(DSM2)
    case PROTO_DSM2_LP45:
    case PROTO_DSM2_DSM2:
    case PROTO_DSM2_DSMX:
      // schedule next Mixer calculations
      SCHEDULE_MIXER_END(22*16);
#if defined(DSM2_PPM)
      sei();
#endif
      setupPulsesDSM2(); // Different versions for DSM2=SERIAL vs. DSM2=PPM
#if defined(CPUM2560) && defined(DSM2_PPM)
      // Ensure each DSM2=PPM serial packet starts out with the correct bit polarity
      TCCR1A = (0 << WGM10) | (3<<COM1B1);  // Make Waveform Generator 'SET' OCR1B pin on next compare event and ...
      TCCR1C = (1<<FOC1B);                  // ... force compare event, to set OCR1B pin high.
      TCCR1A = (1<<COM1B0);                 // Output is ready. Now configure OCR1B pin into 'TOGGLE' mode.
#endif
      break;
#endif

#if defined(IRPROTOS)
    case PROTO_PICZ:
      setupPulsesPiccoZ(g_model.ppmNCH);
      // TODO BSS stbyLevel = 0; //start with 1
      break;
#endif

    default: // standard PPM protocol
#if !defined(SIMU)
      g_ppmPulsePolarity = g_model.pulsePol;
#endif
      // schedule next Mixer calculations
      SCHEDULE_MIXER_END(45*8+g_model.ppmFrameLength*8);
      // no sei here
      setupPulsesPPM(PROTO_PPM);
      // if PPM16, PPM16 pulses are set up automatically within the interrupts
      break;
  }

#if defined(DEBUG) && !defined(VOICE)
  PORTH &= ~0x80; // PORTH:7 HIGH->LOW signals end of setupPulses()
#endif
}

#ifndef SIMU

#if defined(DSM2_PPM) || defined(PXX)
ISR(TIMER1_CAPT_vect) // 2MHz pulse generation
{
#if defined (CPUM2560)
  /*** G9X V4 hardware toggled PPM_out avoids any chance of output timing jitter ***/

  // OCR1B output pin (PPM_OUT) is pre-SET in setupPulses -- on every new 
  // frame, for safety -- and then configured to toggle on each OCR1B compare match.
  // Thus, all we need do here is update the compare regisiter(s) ...
  uint8_t x;
  x = *pulses2MHzRPtr++;  // Byte size
  ICR1 = x;
  OCR1B = (uint16_t)x;    // Duplicate capture compare value for OCR1B, because Timer1 is in CTC mode
                          // and thus we cannot use the OCR1B int. vector. (Should have put PPM_OUT 
                          // pin on OCR1A. Oh well.)

#else // manual bit-bang mode
  uint8_t x;
  PORTB ^= (1<<OUT_B_PPM);    // Toggle PPM_OUT
  x = *pulses2MHzRPtr++;      // Byte size
  ICR1 = x;
  if (x > 200) PORTB |= (1<<OUT_B_PPM); // Make sure pulses are the correct way up.
#endif
}

#if defined(PXX)
ISR(TIMER1_COMPB_vect) // PXX main interrupt
{
  uint8_t x;
  PORTB ^= (1<<OUT_B_PPM);
  x = *pulses2MHzRPtr;      // Byte size
  if ((x & 1) == 0) {
    OCR1B += 32;
  }
  else {
    OCR1B += 16;
  }
  if ((x >>= 1) == 0) {
    if (*(++pulses2MHzRPtr) == 0) {
      OCR1B = OCR1C + 2000;              // 1mS on from OCR1B
    }
  }
  else {
    *pulses2MHzRPtr = x;
  }

  heartbeat |= HEART_TIMER_PULSES;
}
#endif

ISR(TIMER1_COMPC_vect) // DSM2_PPM or PXX end of frame
{
#if defined(DSM2_PPM) && defined(PXX)
  if (IS_DSM2_PROTOCOL(s_current_protocol[0])) {
#endif

#if defined(DSM2_PPM)
    ICR1 = 41536; // next frame starts in 22ms 41536 = 2*(22000 - 14*11*8)
    if (OCR1C < 255) {
      OCR1C = 39000;  // delay setup pulses by 19.5ms to reduce system latency
    }
    else {
      OCR1C = 200;
      // sei will be called inside setupPulses()
      setupPulses();
    }

    heartbeat |= HEART_TIMER_PULSES;
#endif

#if defined(DSM2_PPM) && defined(PXX)
  }
  else {
#endif

#if defined(PXX)
    // must be PXX
    setupPulses();
#endif

#if defined(DSM2_PPM) && defined(PXX)
  }
#endif
}
#endif // defined(DSM2_PPM) || defined(PXX)

#endif // ifndef SIMU


void set_timer3_capture()
{
#ifndef SIMU
#if defined (CPUM2560) || defined(CPUM2561) // TODO TIMSK3 in #define!
  TIMSK3 &= ~((1<<OCIE3A) | (1<<OCIE3B) | (1<<OCIE3C));    // Stop compare interrupts
#else
  ETIMSK &= ~((1<<OCIE3A) | (1<<OCIE3B) | (1<<OCIE3C));    // Stop compare interrupts
#endif
  // TODO G: This can't work with V3.2/4.x boards. Select and use a different pin
  //         for secondary 8-ch PPM output (PORTH or Spare1 or Spare2 maybe?)
  DDRE &= ~0x80;  PORTE |= 0x80;     // Bit 7 input + pullup

  TCCR3B = 0;                        // Stop counter
  TCCR3A = 0;
  // Noise Canceller enabled, neg. edge, clock at 16MHz / 8 (2MHz) (Correct for PCB V4.x+ also)
  TCCR3B  = (1<<ICNC3) | (0b010 << CS30);

  RESUME_PPMIN_INTERRUPT();
#endif
}

void set_timer3_ppm()
{
#ifndef SIMU
  PAUSE_PPMIN_INTERRUPT();

  DDRE |= 0x80;                                       // Bit 7 output

  TCCR3B = 0;                        // Stop counter
  TCCR3A = (0<<WGM10);
  TCCR3B = (1 << WGM12) | (2<<CS10); // CTC OCR1A, 16MHz / 8

#if defined (CPUM2560) || defined(CPUM2561)
  TIMSK3 |= ((1<<OCIE3A) | (1<<OCIE3B));                    // enable immediately before mainloop
#else
  ETIMSK |= ((1<<OCIE3A) | (1<<OCIE3B));                    // enable immediately before mainloop
#endif
#endif
}

#ifndef SIMU

// G: TIMER3_COMPA and COMPB int. vectors are used for PPM16 (8+8, actually)  
// and PPMSIM modes
ISR(TIMER3_COMPA_vect) //2MHz pulse generation
{
  static uint8_t pulsePol;
  static uint16_t * pulse2MHzPPM16RPtr = (uint16_t*) &pulses2MHz[PULSES_SIZE/2];

  if (pulsePol) {
    PORTE |= 0x80; // (1<<OUT_B_PPM);
    pulsePol = 0;
  }
  else {
    PORTE &= ~0x80; // (1<<OUT_B_PPM);
    pulsePol = 1;
  }

  OCR3A = *pulse2MHzPPM16RPtr++;
  OCR3B = B3_comp_value;

  if (*pulse2MHzPPM16RPtr == 0) {
    pulse2MHzPPM16RPtr = (uint16_t*) &pulses2MHz[PULSES_SIZE/2];
    pulsePol = g_model.pulsePol;
  }

  heartbeat |= HEART_TIMER_PULSES;
}

ISR(TIMER3_COMPB_vect) //2MHz pulse generation
{
  sei();
  if (s_current_protocol[0] != g_model.protocol) {
    if (s_current_protocol[0] == PROTO_PPMSIM) {
      setupPulses();
    }
  }
  else {
    setupPulsesPPM(g_model.protocol);
  }
}

#endif // SIMU

