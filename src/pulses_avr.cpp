/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#include "open9x.h"

#if defined(DSM2)
// DSM2 control bits
#define DSM2_CHANS     6
#define BIND_BIT       0x80
#define RANGECHECK_BIT 0x20
#define FRANCE_BIT     0x10
#define DSMX_BIT       0x08
#define BAD_DATA       0x47
#endif

uint8_t s_current_protocol = 255;
uint8_t s_pulses_paused = 0;

uint16_t B3_comp_value;

#ifdef DSM2_SERIAL
inline void DSM2_EnableTXD(void)
{
  UCSR0B |= (1 << TXEN0); // enable TX
  // UCSR0B |= (1 << UDRIE0); // don't enable UDRE0 interrupt, it will be enabled during next setupPulses
}
#endif

void set_timer3_capture( void ) ;
void set_timer3_ppm( void ) ;

void startPulses()
{
#ifdef SIMU
  s_current_protocol = g_model.protocol;
#else
  setupPulses();

#ifdef DSM2_SERIAL
  if (g_model.protocol != PROTO_DSM2)
#endif

  {
#if defined(PCBV4)
    OCR1B = 0xffff; /* Prevent any PPM_PUT pin toggle before the TCNT1 interrupt
                      fires for the first time and sets up the pulse period. */
    // TCCR1A |= (1<<COM1B0); // (COM1B1=0 and COM1B0=1 in TCCR1A)  toogle the state of PB6(OC1B) on each TCNT1==OCR1B
    TCCR1A = (3<<COM1B0); // Connect OC1B to PPM_OUT pin (SET the state of PB6(OC1B) on next TCNT1==OCR1B)
#endif
  }

#if defined(PCBV4)
  TIMSK1 |= (1<<OCIE1A); // Pulse generator enable immediately before mainloop
#else
  TIMSK |= (1<<OCIE1A);  // Pulse generator enable immediately before mainloop
#endif

#endif // SIMU
}

#define PULSES_SIZE       144
uint8_t pulses2MHz[PULSES_SIZE] = {0}; // TODO check this length, pulled from er9x, perhaps too big
uint8_t *pulses2MHzRPtr = pulses2MHz;

#if defined(DSM2) || defined(PXX) || defined(IRPROTOS)
uint8_t *pulses2MHzWPtr = pulses2MHz;
#endif

#define CTRL_END 0
#define CTRL_CNT 1
#define CTRL_REP_1CMD -3
#define CTRL_REP_2CMD -6

#ifndef SIMU

ISR(TIMER1_COMPA_vect) //2MHz pulse generation
{
  static uint8_t pulsePol; // TODO strange, it's always 0 at first, shouldn't it be initialized properly in setupPulses?

  // Latency -- how far further on from interrupt trigger has the timer counted?
  // (or -- how long did it take to get to this function)
  uint8_t dt = TCNT1L;
  
#ifdef DSM2_SERIAL
  if (g_model.protocol == PROTO_DSM2) {
    OCR1A = 40000;
    // sei will be called inside setupPulses()
    setupPulses();
    cli();
    UCSR0B |= (1 << UDRIE0); // enable  UDRE0 interrupt
  }
  else
#endif
  {
    // Original bitbang for PPM
#if !defined(PCBV4)
    if (s_current_protocol != PROTO_NONE) {
      if (pulsePol) {
        PORTB |=  (1<<OUT_B_PPM); // GCC optimisation should result in a single SBI instruction
        pulsePol = 0;
      }
      else {
        PORTB &= ~(1<<OUT_B_PPM);
        pulsePol = 1;
      }
    }
#endif

    OCR1A = *((uint16_t*)pulses2MHzRPtr); // Schedule next interrupt vector (to this handler)

#if defined(PCBV4)
    OCR1B = *((uint16_t*)pulses2MHzRPtr); /* G: Using timer in CTC mode, restricted to using OCR1A for interrupt triggering.
                                                So we actually have to handle the OCR1B register separately in this way. */

    // We cannot read the status of the PPM_OUT pin when OC1B is connected to it on the ATmega2560.
    // So the only way to set polarity is to manually control set/reset mode in COM1B0/1
    if (s_current_protocol != PROTO_NONE) {
      if (pulsePol) {
        TCCR1A = (3<<COM1B0); // SET the state of PB6(OC1B) on next TCNT1==OCR1B
        pulsePol = 0;
      }
      else {
        TCCR1A = (2<<COM1B0); // CLEAR the state of PB6(OC1B) on next TCNT1==OCR1B
        pulsePol = 1;
      }
    }
#endif

    pulses2MHzRPtr += sizeof(uint16_t);
    if (*((uint16_t*)pulses2MHzRPtr) == 0) {

      pulsePol = g_model.pulsePol;

#if defined(PCBV4)
      TIMSK1 &= ~(1<<OCIE1A); //stop reentrance
#else
      TIMSK &= ~(1<<OCIE1A); //stop reentrance
#endif

      // sei will be called inside setupPulses()

      setupPulses();

      if (!IS_PXX_PROTOCOL(s_current_protocol) && !IS_DSM2_PROTOCOL(s_current_protocol)) {

        // cli is not needed because for these 2 protocols interrupts are not enabled when entering here

#if defined(PCBV4)
        TIMSK1 |= (1<<OCIE1A);
#else
        TIMSK |= (1<<OCIE1A);
#endif
        sei();
      }
    }
  }
  
  if (dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if (dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;
    
  heartbeat |= HEART_TIMER_PULSES;
}

#endif

void setupPulsesPPM(uint8_t proto)
{
    int16_t PPM_range = g_model.extendedLimits ? 640*2 : 512*2;   //range of 0.7..1.7msec

    //Total frame length = 22.5msec
    //each pulse is 0.7..1.7ms long with a 0.3ms stop tail
    //The pulse ISR is 2mhz that's why everything is multiplied by 2
    uint16_t *ptr = (proto == PROTO_PPM ? (uint16_t *)pulses2MHz : (uint16_t *) &pulses2MHz[PULSES_SIZE/2]);
    uint8_t p = (proto == PROTO_PPM16 ? 16 : 8) + (g_model.ppmNCH * 2); //Channels *2
    uint16_t q = (g_model.ppmDelay*50+300)*2; // Stoplen *2
    uint32_t rest = 22500u*2 - q; // Minimum Framelen=22.5ms
    
#if defined(PCBV4)
    OCR5A = (uint16_t)0x7d * (45+g_model.ppmFrameLength-g_timeMainLast-2/*1ms*/);
    TCNT5 = 0;
#endif

    rest += (int32_t(g_model.ppmFrameLength))*1000;
    for (uint8_t i=(proto==PROTO_PPM16) ? p-8 : 0; i<p; i++) {
#ifdef PPM_CENTER_ADJUSTABLE
      int16_t v = limit((int16_t)-PPM_range, g_chans512[i], (int16_t)PPM_range) + 2*(PPM_CENTER+limitaddress(i)->ppmCenter);
#else
      int16_t v = limit((int16_t)-PPM_range, g_chans512[i], (int16_t)PPM_range) + 2*PPM_CENTER;
#endif
      rest -= v;
      *ptr++ = q;
      *ptr++ = v - q; /* as Pat MacKenzie suggests, reviewed and modified by Cam */
    }

    *ptr = q;       //reverse these two assignments
    *(ptr+1) = rest;

    if (proto == PROTO_PPM) {
      pulses2MHzRPtr = pulses2MHz;
    }
    else {
      B3_comp_value = rest - 1000 ;               // 500uS before end of sync pulse
    }

    *(ptr+2) = 0;
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

uint8_t PcmByte ;
uint8_t PcmBitCount ;
uint16_t PcmCrc ;
uint8_t PcmOnesCount ;
uint8_t pxxFlag = 0;

void crc( uint8_t data )
{
    PcmCrc=(PcmCrc>>8)^pgm_read_word(&CRCTable[(PcmCrc^data) & 0xFF]);
}


void putPcmPart( uint8_t value )
{
    PcmByte >>= 2 ;
    PcmByte |= value ;
    if ( ++PcmBitCount >= 4 )
    {
        *pulses2MHzWPtr++ = PcmByte ;
        PcmBitCount = PcmByte = 0 ;
    }
}


void putPcmFlush()
{
    while ( PcmBitCount != 0 )
    {
        putPcmPart( 0 ) ; // Empty
    }
    *pulses2MHzWPtr = 0 ;                               // Mark end
}

void putPcmBit( uint8_t bit )
{
    if ( bit )
    {
        PcmOnesCount += 1 ;
        putPcmPart( 0x80 ) ;
    }
    else
    {
        PcmOnesCount = 0 ;
        putPcmPart( 0xC0 ) ;
    }
    if ( PcmOnesCount >= 5 )
    {
        putPcmBit( 0 ) ;                                // Stuff a 0 bit in
    }
}

void putPcmByte( uint8_t byte )
{
    uint8_t i ;

    crc( byte ) ;

    for (i=0; i<8; i++)
    {
        putPcmBit( byte & 0x80 ) ;
        byte <<= 1 ;
    }
}


void putPcmHead()
{
  // send 7E, do not CRC
  // 01111110
  putPcmPart( 0xC0 ) ;
  putPcmPart( 0x80 ) ;
  putPcmPart( 0x80 ) ;
  putPcmPart( 0x80 ) ;
  putPcmPart( 0x80 ) ;
  putPcmPart( 0x80 ) ;
  putPcmPart( 0x80 ) ;
  putPcmPart( 0xC0 ) ;
}

void setupPulsesPXX()
{
    uint8_t i ;
    uint16_t chan ;
    uint16_t chan_1 ;

    pulses2MHzWPtr = pulses2MHz;
    pulses2MHzRPtr = pulses2MHz;

    PcmCrc = 0 ;
    PcmBitCount = PcmByte = 0 ;
    PcmOnesCount = 0 ;
    putPcmHead() ;
    putPcmByte( g_model.modelId ) ;     // putPcmByte( g_model.rxnum ) ;  //
    putPcmByte( pxxFlag ) ;     // First byte of flags
    putPcmByte( 0 ) ;     // Second byte of flags
    pxxFlag = 0;          // reset flag after send
    for ( i = 0 ; i < 8 ; i += 2 )              // First 8 channels only
    {
        chan = g_chans512[i] * 3 / 4 + 2250 ;
        chan_1 = g_chans512[i+1] * 3 / 4 + 2250 ;
        putPcmByte( chan ) ; // Low byte of channel
        putPcmByte( ( ( chan >> 8 ) & 0x0F ) | ( chan_1 << 4) ) ;  // 4 bits each from 2 channels
        putPcmByte( chan_1 >> 4 ) ;  // High byte of channel
    }
    chan = PcmCrc ;                     // get the crc
    putPcmByte( chan ) ;                        // Checksum lo
    putPcmByte( chan >> 8 ) ; // Checksum hi
    putPcmHead( ) ;
    putPcmFlush() ;
    OCR1C += 40000 ;            // 20mS on
    PORTB |= (1<<OUT_B_PPM);
}
#endif

#ifdef DSM2_SERIAL

// DSM2 protocol pulled from th9x - Thanks thus!!!

//http://www.rclineforum.de/forum/board49-zubeh-r-elektronik-usw/fernsteuerungen-sender-und-emp/neuer-9-kanal-sender-f-r-70-au/Beitrag_3897736#post3897736
//(dsm2( LP4DSM aus den RTF ( Ready To Fly ) Sendern von Spektrum.
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

bool s_bind_allowed = true;

FORCEINLINE void setupPulsesDsm2()
{
  uint16_t *ptr = (uint16_t *)pulses2MHz;
  if (s_bind_allowed) {
    *ptr++ = (keyState(SW_Trainer) ? BIND_BIT : 0x00);
    s_bind_allowed = false;
  }
  else {
    *ptr++ = 0x00;
  }
  *ptr++ = g_model.modelId;
  for (uint8_t i=0; i<DSM2_CHANS; i++) {
    uint16_t pulse = limit(0, ((g_chans512[i]*13)>>5)+512,1023);
    *ptr++ = (i<<2) | ((pulse>>8)&0x03);
    *ptr++ = pulse & 0xff;
  }

  pulses2MHzWPtr = (uint8_t *)ptr;
  pulses2MHzRPtr = pulses2MHz;
}

void DSM2_Done()
{
  UCSR0B &= ~((1 << TXEN0) | (1 << UDRIE0)); // disable UART TX and interrupt
}

void DSM2_Init(void)
{
#ifndef SIMU

  DDRE &= ~(1 << DDE0);    // set RXD0 pin as input
  PORTE |= (1 << PORTE0);  // enable pullup on RXD0 pin

#undef BAUD
#define BAUD 125000

#include <util/setbaud.h>

  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0A &= ~(1 << U2X0); // disable double speed operation.

  // set 8N1 (leave TX and RX disabled for now)
  UCSR0B = 0 | (0 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) | (0 << RXEN0) | (0 << TXEN0) | (0 << UCSZ02);
  UCSR0C = 0 | (1 << UCSZ01) | (1 << UCSZ00);

  while (UCSR0A & (1 << RXC0)) UDR0; // flush receive buffer

  // These should be running right from power up on a FrSky enabled '9X.
  DSM2_EnableTXD(); // enable DSM2 UART transmitter

#endif // SIMU
}
#endif

#if defined(DSM2_PPM)
inline void _send_1(uint8_t v)
{
  *pulses2MHzWPtr++ = v;
}

#define BITLEN_DSM2 (8*2) //125000 Baud
void sendByteDsm2(uint8_t b) //max 10changes 0 10 10 10 10 1
{
    bool    lev = 0;
    uint8_t len = BITLEN_DSM2; //max val: 9*16 < 256
    for( uint8_t i=0; i<=8; i++){ //8Bits + Stop=1
        bool nlev = b & 1; //lsb first
        if(lev == nlev){
            len += BITLEN_DSM2;
        }else{
            _send_1(len -1);
            len  = BITLEN_DSM2;
            lev  = nlev;
        }
        b = (b>>1) | 0x80; //shift in stop bit
    }
    _send_1(len+BITLEN_DSM2-1); // 2 stop bits
}

void setupPulsesDsm2()
{
  static uint8_t dsmDat[2+6*2] = {0xFF,0x00, 0x00,0xAA, 0x05,0xFF, 0x09,0xFF, 0x0D,0xFF, 0x13,0x54, 0x14,0xAA};
  uint8_t counter;

  pulses2MHzWPtr = pulses2MHz;

  // If more channels needed make sure the pulses union/array is large enough
  if (dsmDat[0] & BAD_DATA) // first time through, setup header
  {
    switch(g_model.ppmNCH)
    {
      case LPXDSM2:
        dsmDat[0] = BIND_BIT;
        break;
      case DSM2only:
        dsmDat[0] = 0x90;
        break;
      default:
        dsmDat[0] = 0x98; // DSMX bind mode
        break;
    }
  }
  if ((dsmDat[0] & BIND_BIT) && (!keyState(SW_Trainer))) dsmDat[0] &= ~BIND_BIT; // clear bind bit if trainer not pulled
  // TODO find a way to do that, FUNC SWITCH: if ((!(dsmDat[0] & BIND_BIT)) && getSwitch(MAX_DRSWITCH-1, 0, 0)) dsmDat[0] |= RANGECHECK_BIT;   // range check function
  // else dsmDat[0] &= ~RANGECHECK_BIT;
  dsmDat[1] = g_model.modelId; // DSM2 Header second byte for model match
  for (uint8_t i=0; i<DSM2_CHANS; i++)
  {
    uint16_t pulse = limit(0, ((g_chans512[i]*13)>>5)+512,1023);
    dsmDat[2+2*i] = (i<<2) | ((pulse>>8)&0x03);
    dsmDat[3+2*i] = pulse & 0xff;
  }

  for (counter=0; counter<14; counter++)
    sendByteDsm2(dsmDat[counter]);

  pulses2MHzWPtr -= 1; //remove last stopbits and
  _send_1(255); //prolong them
  _send_1(0); //end of pulse stream

  pulses2MHzRPtr = pulses2MHz;
}
#endif

#if defined(IRPROTOS)
static void _send_u8(uint8_t u8)
{
#ifdef SIMU
  *(pulses2MHzPtr)++=u8;
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
  *(*(uint16_t**)&pulses2MHzWPtr)++=u16;
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
  *pulses2MHzWPtr++=CTRL_CNT;
  //_send_u8(CTRL_CNT);
}

static void _send_rep1(uint16_t t0,uint8_t cnt)
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
  while (n--) picco_sendB1(bits & (1<<n));
}


#define BITS  10
#define BITS2 (BITS-1)

NOINLINE uint8_t reduce7u(int16_t v, uint8_t sfr)
{
  v += (1<<BITS2);
  if(v <  0) v=0;
  if(v >= (1<<BITS)) v=(1<<BITS)-1;
  return v>>sfr;
}

NOINLINE int8_t reduce7s(int16_t v, uint8_t sfr, uint8_t sf2, int8_t ofs2)
{
  v += (1<<BITS2)+sf2;
  if(v&(1<<BITS)) v = (1<<BITS)-1; //no overflow
  int8_t  i8 = (uint16_t)v>>sfr;
  if(i8<=0) i8=1;
  i8-=ofs2;
  if(i8>=ofs2) i8=ofs2-1;//no overflow
  return i8;
}

//these defines allow the compiler to preclculate constants
#define getChan7u(i,bitsRes) reduce7u(g_chans512[i],(BITS-bitsRes))
#define getChan7s(i,bitsRes) reduce7s(g_chans512[i],(BITS-bitsRes),1<<((BITS-bitsRes)-1),1<<(bitsRes-1))

static void setupPulsesPiccoZ(uint8_t chn)
{
  // 1900  650 650         1226             650 0bit  1226 1bit      Stop
  // ----   __  -- __ -- __----__----__     --__      ----____       --__
  static bool    state = 0;
  static uint8_t pow;
  static int8_t  trim;
  static int8_t  dir;
  static uint8_t chk;
  if (state == 0)  {
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
  if (s_pulses_paused)
    required_protocol = PROTO_NONE;

  if (s_current_protocol != required_protocol) {

    s_current_protocol = required_protocol;

    switch (required_protocol) {

#if defined(DSM2_PPM)
      case PROTO_DSM2:
        set_timer3_capture() ;
        TCCR1B = 0;            // Stop counter
        OCR1C = 200;           // 100 uS
        TCNT1 = 300;           // Past the OCR1C value
        ICR1 = 44000;          // Next frame starts in 22 mS
#if defined(PCBV4)
        TIMSK1 &= ~0x3C;       // All interrupts off
        TIFR1 = 0x2F;
        TIMSK1 |= 0x28;        // Enable CAPT and COMPC
#else
        TIMSK &= ~0x3C;        // All interrupts off
        TIFR = 0x3C;
        ETIFR = 0x3F ;
        TIMSK |= 0x20;         // Enable CAPT
        ETIMSK |= (1<<OCIE1C); // Enable COMPC
#endif
        TCCR1A = (0 << WGM10);
        TCCR1B = (3 << WGM12) | (2 << CS10); // CTC ICR, 16MHz / 8
        break;
#endif

#if defined(PXX)
      case PROTO_PXX:
        set_timer3_capture() ;
        TCCR1B = 0 ;           // Stop counter
        TCNT1 = 0 ;
        OCR1B = 6000 ;         // Next frame starts in 3 mS
        OCR1C = 4000 ;         // Next frame setup in 2 mS
#if defined(PCBV4)
        TIMSK1 &= ~0x3C; // All interrupts off
        TIFR1 = 0x2F;
        TIMSK1 |= (1<<OCIE1B) ; // Enable COMPB
        TIMSK1 |= (1<<OCIE1C); // Enable COMPC
#else
        TIMSK &= ~0x3C;        // All interrupts off
        TIFR = 0x3C ;
        ETIFR = 0x3F ;
        TIMSK |= (1<<OCIE1B) ; // Enable COMPB
        ETIMSK |= (1<<OCIE1C); // Enable COMPC
#endif
        TCCR1A  = 0;
        TCCR1B  = (2<<CS10);   //ICNC3 16MHz / 8
        break;
#endif

      case PROTO_PPM16 :
        TCCR1B = 0 ;            // Stop counter
        OCR1A = 40000 ;         // Next frame starts in 20 mS
        TCNT1 = 0 ;
#if defined(PCBV4)
        TIMSK1 &= ~0x3C; // All interrupts off
        TIMSK1 &= ~(1<<OCIE1C) ;            // COMPC1 off
        TIFR1 = 0x2F;
        TIMSK1 |= 0x10; // Enable COMPA
#else
        TIMSK &= ~0x3C ;    // All interrupts off
        ETIMSK &= ~(1<<OCIE1C) ;            // COMPC1 off
        TIFR = 0x3C ;                       // Clear all pending interrupts
        ETIFR = 0x3F ;                      // Clear all pending interrupts
        TIMSK |= 0x10 ;         // Enable COMPA
#endif
        TCCR1A = (0<<WGM10) ;
        TCCR1B = (1 << WGM12) | (2<<CS10) ; // CTC OCRA, 16MHz / 8
        setupPulsesPPM(PROTO_PPM16);
        OCR3A = 50000 ;
        OCR3B = 5000 ;
        set_timer3_ppm() ;
        break ;

      case PROTO_PPMSIM :
        TCCR1B = 0 ;                        // Stop counter
        TCNT1 = 0 ;
#if defined(PCBV4)
        TIMSK1 &= ~0x3C; // All interrupts off
        TIMSK1 &= ~(1<<OCIE1C) ;            // COMPC1 off
        TIFR1 = 0x2F;
#else
        TIMSK &= ~0x3C ;    // All interrupts off
        ETIMSK &= ~(1<<OCIE1C) ;            // COMPC1 off
        TIFR = 0x3C ;                       // Clear all pending interrupts
        ETIFR = 0x3F ;                      // Clear all pending interrupts
#endif
        setupPulsesPPM(PROTO_PPMSIM);
        OCR3A = 50000 ;
        OCR3B = 5000 ;
        set_timer3_ppm() ;
        PORTB &= ~(1<<OUT_B_PPM);                       // Hold PPM output low
        break ;

      default:
        set_timer3_capture() ;
        TCCR1B = 0;    // Stop counter
        OCR1A = 40000; // Next frame starts in 20 mS
        TCNT1 = 0;
#if defined(PCBV4)
        TIMSK1 &= ~0x3C; // All interrupts off
        TIFR1 = 0x2F;
        TIMSK1 |= 0x10; // Enable COMPA
#else
        TIMSK &= ~0x3C; // All interrupts off
        TIFR = 0x3C;
        ETIFR = 0x3F ; // Clear all pending interrupts
        TIMSK |= 0x10; // Enable COMPA
#endif
        // TCNT1 2MHz counter (auto-cleared) plus Capture Compare int.
        //       Used for PPM pulse generator
        TCCR1A = (0 << WGM10);
        TCCR1B = (1 << WGM12) | (2 << CS10); // CTC OCRA, 16MHz / 8
        break;
    }
  }

  switch(required_protocol) {

#ifdef PXX
    case PROTO_PXX:
      sei();
      setupPulsesPXX();
      break;
#endif

#ifdef DSM2
    case PROTO_DSM2:
      sei();
      setupPulsesDsm2();
      break;
#endif

#ifdef IRPROTOS
    case PROTO_PICZ:
      setupPulsesPiccoZ(g_model.ppmNCH);
      // TODO BSS stbyLevel = 0; //start with 1
      break;
#endif

    default:
      // no sei here
      setupPulsesPPM(PROTO_PPM);
      // if PPM16, PPM16 pulses are set up automatically within the interrupts
      break;
  }
}

#ifndef SIMU

#if defined(DSM2_PPM) || defined(PXX)
ISR(TIMER1_CAPT_vect) // 2MHz pulse generation
{
  uint8_t x ;
  PORTB ^= (1<<OUT_B_PPM);
  x = *pulses2MHzRPtr++;      // Byte size
  ICR1 = x ;
  if (x > 200) PORTB |= (1<<OUT_B_PPM); // Make sure pulses are the correct way up
  heartbeat |= HEART_TIMER_PULSES; // TODO why not in TIMER1_COMPB_vect (in setupPulses)?
}

#if defined(PXX)
ISR(TIMER1_COMPB_vect) // PXX main interrupt
{
    uint8_t x ;
    PORTB ^= (1<<OUT_B_PPM) ;
    x = *pulses2MHzRPtr;      // Byte size
    if ( ( x & 1 ) == 0 )
    {
        OCR1B += 32 ;
    }
    else
    {
        OCR1B += 16 ;
    }
    if ( (x >>= 1) == 0 )
    {
        if ( *(++pulses2MHzRPtr) == 0 )
        {
            OCR1B = OCR1C + 2000 ;              // 1mS on from OCR1B
        }
    }
    else
    {
        *pulses2MHzRPtr = x;
    }

    heartbeat |= HEART_TIMER_PULSES;
}
#endif

ISR(TIMER1_COMPC_vect) // DSM2 or PXX end of frame
{
#if defined(DSM2_PPM) && defined(PXX)
  if ( g_model.protocol == PROTO_DSM2 ) {
#endif

#if defined(DSM2_PPM)
    ICR1 = 41536 ; // next frame starts in 22ms 41536 = 2*(22000 - 14*11*8)
    if (OCR1C < 255) {
      OCR1C = 39000;  // delay setup pulses by 19.5ms to reduce system latency
    }
    else {
      OCR1C = 200;
      // sei will be called inside setupPulses()
      setupPulses();
    }
#endif

#if defined(DSM2_PPM) && defined(PXX)
  }
  else {
#endif

#if defined(PXX)
    // must be PXX
    setupPulses() ;
#endif

#if defined(DSM2_PPM) && defined(PXX)
  }
#endif
}
#endif

#endif


void set_timer3_capture()
{
#ifndef SIMU
#if defined (PCBV4)
    TIMSK3 &= ~( (1<<OCIE3A) | (1<<OCIE3B) | (1<<OCIE3C) ) ;    // Stop compare interrupts
#else
    ETIMSK &= ~( (1<<OCIE3A) | (1<<OCIE3B) | (1<<OCIE3C) ) ;    // Stop compare interrupts
#endif
    DDRE &= ~0x80;  PORTE |= 0x80 ;     // Bit 7 input + pullup

    TCCR3B = 0 ;                        // Stop counter
    TCCR3A = 0;
    // Noise Canceller enabled, neg. edge, clock at 16MHz / 8 (2MHz) (Correct for PCB V4.x+ also)
    TCCR3B  = (1<<ICNC3) | (0b010 << CS30);
#if defined (PCBV4)
    TIMSK3 |= (1<<ICIE3);
#else
    ETIMSK |= (1<<TICIE3);
#endif
#endif
}

void set_timer3_ppm()
{
#ifndef SIMU
#if defined (PCBV4)
    TIMSK3 &= ~(1<<ICIE3);
#else
    ETIMSK &= ~(1<<TICIE3) ;   // Stop capture interrupt
#endif
    DDRE |= 0x80;                                       // Bit 7 output

    TCCR3B = 0 ;                        // Stop counter
    TCCR3A = (0<<WGM10);
    TCCR3B = (1 << WGM12) | (2<<CS10); // CTC OCR1A, 16MHz / 8

#if defined (PCBV4)
    TIMSK3 |= ( (1<<OCIE3A) | (1<<OCIE3B) );                    // enable immediately before mainloop
#else
    ETIMSK |= ( (1<<OCIE3A) | (1<<OCIE3B) );                    // enable immediately before mainloop
#endif
#endif
}

#ifndef SIMU

ISR(TIMER3_COMPA_vect) //2MHz pulse generation
{
    static uint8_t   pulsePol;
    static uint16_t *pulse2MHzPPM16RPtr = (uint16_t*) &pulses2MHz[PULSES_SIZE/2];

    if (pulsePol) {
        PORTE |= 0x80 ; // (1<<OUT_B_PPM);
        pulsePol = 0;
    }
    else {
        PORTE &= ~0x80; // (1<<OUT_B_PPM);
        pulsePol = 1;
    }

    OCR3A = *pulse2MHzPPM16RPtr++;
    OCR3B = B3_comp_value ;

    if (*pulse2MHzPPM16RPtr == 0) {
        pulse2MHzPPM16RPtr = (uint16_t*) &pulses2MHz[PULSES_SIZE/2];
        pulsePol = g_model.pulsePol;
    }

    heartbeat |= HEART_TIMER_PULSES;
}

ISR(TIMER3_COMPB_vect) //2MHz pulse generation
{
  sei() ;
  if (s_current_protocol != g_model.protocol) {
    if (s_current_protocol == PROTO_PPMSIM) {
      setupPulses();
    }
  }
  else {
    setupPulsesPPM(g_model.protocol) ;
  }
}

#endif // SIMU

