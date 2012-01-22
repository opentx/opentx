/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * gruvin9x is based on code named er9x by
 * Author - Erez Raviv <erezraviv@gmail.com>, which is in turn
 * was based on the original (and ongoing) project by Thomas Husterer,
 * th9x -- http://code.google.com/p/th9x/
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

#ifdef DSM2
#define DSM2_CHANS     6
#define BIND_BIT       0x80
#define RANGECHECK_BIT 0x20
#define FRANCE_BIT     0x10
#define DSMX_BIT       0x08
#define BAD_DATA       0x47
#endif

#if defined(DSM2_PPM) || defined(PXX)
uint8_t s_current_protocol = 255;
#endif

#ifdef DSM2_SERIAL
inline void DSM2_EnableTXD(void)
{
  UCSR0B |= (1 << TXEN0); // enable TX
  UCSR0B |= (1 << UDRIE0); // enable  UDRE0 interrupt
}
#endif

void startPulses()
{
#ifndef SIMU
  setupPulses();

#ifdef DSM2_SERIAL
  if (g_model.protocol == PROTO_DSM2) {
    DSM2_EnableTXD();
    OCR1A = 40000;
  }
  else
#endif

  {
#if defined(PCBV4)
    OCR1B = 0xffff; /* Prevent any PPM_PUT pin toggle before the TCNT1 interrupt
                      fires for the first time and sets up the pulse period. */
    // TCCR1A |= (1<<COM1B0); // (COM1B1=0 and COM1B0=1 in TCCR1A)  toogle the state of PB6(OC1B) on each TCNT1==OCR1B
    TCCR1A = (3<<COM1B0); // Connect OC1B to PPM_OUT pin (SET the state of PB6(OC1B) on next TCNT1==OCR1B)
#elif defined(DPPMPB7_HARDWARE) // addon Vinceofdrink@gmail (hardware ppm)
    OCR1C = 0xffff; // See comment for PCBV4, above
    TCCR1A |= (1<<COM1C0); // (COM1C1=0 and COM1C0=1 in TCCR1A)  toogle the state of PB7(OC1C) on each TCNT1==OCR1C
#endif
  }

#if defined(PCBV3)
  TIMSK1 |= (1<<OCIE1A); // Pulse generator enable immediately before mainloop
#else
  TIMSK |= (1<<OCIE1A);  // Pulse generator enable immediately before mainloop
#endif

#endif // SIMU
}

#if defined(DSM2)
uint8_t pulses2MHz[144] = {0}; // TODO check this length, pulled from er9x, perhaps too big
#elif defined(CTP1009)
uint8_t pulses2MHz[50*sizeof(uint16_t)] = {0};
#else
uint8_t pulses2MHz[40*sizeof(uint16_t)] = {0};
#endif

uint8_t *pulses2MHzRPtr = pulses2MHz;
uint8_t *pulses2MHzWPtr = pulses2MHz;

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
    // vinceofdrink@gmail harwared ppm
    // Orginal bitbang for PPM
#if !defined(DPPMPB7_HARDWARE) && !defined(PCBV4)
    if (pulsePol) {
      PORTB |=  (1<<OUT_B_PPM); // GCC optimisation should result in a single SBI instruction
      pulsePol = 0;
    }
    else {
      PORTB &= ~(1<<OUT_B_PPM);
      pulsePol = 1;
    }
#endif

    OCR1A = *((uint16_t*)pulses2MHzRPtr); // Schedule next interrupt vector (to this handler)

#if defined(PCBV4)
    OCR1B = *((uint16_t*)pulses2MHzRPtr); /* G: Using timer in CTC mode, restricted to using OCR1A for interrupt triggering.
                                So we actually have to handle the OCR1B register separately in this way. */

    // We cannot read the status of the PPM_OUT pin when OC1B is connected to it on the ATmega2560.
    // So the only way to set polarity is to manually control set/reset mode in COM1B0/1
    if (pulsePol) {
      TCCR1A = (3<<COM1B0); // SET the state of PB6(OC1B) on next TCNT1==OCR1B
      pulsePol = 0;
    }
    else {
      TCCR1A = (2<<COM1B0); // CLEAR the state of PB6(OC1B) on next TCNT1==OCR1B
      pulsePol = 1;
    }

    //vinceofdrink@gmail harwared ppm
#elif defined(DPPMPB7_HARDWARE)
    OCR1C = *((uint16_t*)pulses2MHzRPtr);  // just copy the value of the OCR1A to OCR1C to test PPM out without too
                              // much change in the code not optimum but will not alter ppm precision
#endif
  
    pulses2MHzRPtr += sizeof(uint16_t);
    if (pulses2MHzRPtr == pulses2MHzWPtr) {

      pulsePol = g_model.pulsePol;

// TODO does it exist PCBV3? If no, replace PCBV3 by PCBV4 everywhere
#if defined(PCBV3)
      TIMSK1 &= ~(1<<OCIE1A); //stop reentrance
#else
      TIMSK &= ~(1<<OCIE1A); //stop reentrance
#endif

      // sei will be called inside setupPulses()

      setupPulses();

#if defined(DPPMPB7_HARDWARE)
      // G: NOTE: This strategy does not work on the '2560 becasue you can't
      //          read the PPM out pin when connected to OC1B. Vincent says
      //          it works on the '64A. I haven't personally tested it.
      if (PINB & (1<<OUT_B_PPM) && g_model.pulsePol)
        TCCR1C=(1<<FOC1C);
#endif

      cli();

#if defined(PCBV3)
      TIMSK1 |= (1<<OCIE1A);
#else
      TIMSK |= (1<<OCIE1A);
#endif
      sei();
    }
  }
  
  if (dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if (dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;
    
  heartbeat |= HEART_TIMER2Mhz;
}

#endif

FORCEINLINE void setupPulsesPPM() // changed 10/05/2010 by dino Issue 128
{
#define PPM_CENTER 1200*2
    int16_t PPM_range = g_model.extendedLimits ? 640*2 : 512*2;   //range of 0.7..1.7msec

    //Total frame length = 22.5msec
    //each pulse is 0.7..1.7ms long with a 0.3ms stop tail
    //The pulse ISR is 2mhz that's why everything is multiplied by 2

    // G: Found the following reference at th9x. The below code does not seem
    // to produce quite exactly this, to my eye. *shrug*
    //   http://www.aerodesign.de/peter/2000/PCM/frame_ppm.gif
    uint16_t *ptr = (uint16_t *)pulses2MHzWPtr;
    uint8_t p = 8+(g_model.ppmNCH*2); // channels count
    uint16_t q = (g_model.ppmDelay*50+300)*2; //Stoplen *2
    uint16_t rest = 22500u*2-q; //Minimum Framelen=22.5 ms
    rest += (int16_t(g_model.ppmFrameLength))*1000;
    for (uint8_t i=0; i<p; i++) {
      int16_t v = limit((int16_t)-PPM_range, g_chans512[i], (int16_t)PPM_range) + PPM_CENTER;
      rest -= (v+q);
      *ptr++ = v - q + 600; /* as Pat MacKenzie suggests */
      *ptr++ = q;
    }
    *ptr = rest;
    *(ptr+1) = q;
    pulses2MHzWPtr = ((uint8_t *)ptr) + (2*sizeof(uint16_t));
}

#ifdef PXX
inline void __attribute__ ((always_inline)) setupPulsesPXX()
{

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

FORCEINLINE void setupPulsesDsm2()
{
  uint16_t *ptr = (uint16_t *)pulses2MHz;
  *ptr++ = (isFunctionActive(FUNC_MODELMATCH) ? BIND_BIT : 0x00);
  *ptr++ = g_eeGeneral.currModel;
  for (uint8_t i=0; i<DSM2_CHANS; i++) {
    uint16_t pulse = limit(0, (g_chans512[i]>>1)+512, 1023);
    *ptr++ = (i<<2) | ((pulse>>8)&0x03);
    *ptr++ = pulse & 0xff;
  }
  pulses2MHzWPtr = (uint8_t *)ptr;
}

void DSM2_Done()
{
  UCSR0B &= ~((1 << TXEN0) | (1 << UDRIE0)); // disable TX pin and interrupt
}

void DSM2_Init(void)
{
#ifndef SIMU

  DDRE &= ~(1 << DDE0);    // set RXD0 pin as input
  PORTE &= ~(1 << PORTE0); // disable pullup on RXD0 pin

#undef BAUD
#define BAUD 125000

#include <util/setbaud.h>

  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0A &= ~(1 << U2X0); // disable double speed operation.

  // set 8N1
  UCSR0B = 0 | (0 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) | (0 << RXEN0) | (0 << TXEN0) | (0 << UCSZ02);
  UCSR0C = 0 | (1 << UCSZ01) | (1 << UCSZ00);

  while (UCSR0A & (1 << RXC0)) UDR0; // flush receive buffer

  setupPulsesDsm2();
  pulses2MHzRPtr = pulses2MHz;

  // These should be running right from power up on a FrSky enabled '9X.
  DSM2_EnableTXD(); // enable FrSky-Telemetry reception

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
  if ((dsmDat[0] & BIND_BIT) && (!isFunctionActive(FUNC_MODELMATCH))) dsmDat[0] &= ~BIND_BIT; //clear bind bit if trainer not pulled
  // TODO add a function key for that: if ((!(dsmDat[0] & BIND_BIT)) && isFunctionActive(FUNC_RANGECHECK)) dsmDat[0] |= RANGECHECK_BIT; //range check function

  else dsmDat[0] &= ~RANGECHECK_BIT;
  dsmDat[1] = g_eeGeneral.currModel+1; //DSM2 Header second byte for model match
  for (uint8_t i=0; i<DSM2_CHANS; i++)
  {
    uint16_t pulse = limit(0, (g_chans512[i]>>1)+512,1023);
    dsmDat[2+2*i] = (i<<2) | ((pulse>>8)&0x03);
    dsmDat[3+2*i] = pulse & 0xff;
  }

  for ( counter = 0; counter < 14; counter += 1 )
  {
    sendByteDsm2(dsmDat[counter]);
  }
  pulses2MHzWPtr -= 1; //remove last stopbits and
  _send_1( 255 ); //prolong them
  _send_1(0); //end of pulse stream
}
#endif

#if defined(SILVER) || defined(CTP1009)
void _send_hilo(uint16_t hi,uint16_t lo)
{
  uint16_t *ptr = (uint16_t *)pulses2MHzWPtr;
  *ptr++ = hi;
  *ptr++ = lo;
  pulses2MHzWPtr = (uint8_t *)ptr;
}
#endif

#ifdef SILVER

#define BITLEN (600u*2)
#define send_hilo_silv( hi, lo) _send_hilo( (hi)*BITLEN,(lo)*BITLEN )

void sendBitSilv(uint8_t val)
{
  send_hilo_silv((val)?2:1,(val)?2:1);
}
void send2BitsSilv(uint8_t val)
{
  sendBitSilv(val&2);sendBitSilv(val&1);
}
// _ oder - je 0.6ms  (gemessen 0.7ms)
//
//____-----_-_-_--_--_   -_--__  -_-_-_-_  -_-_-_-_  --__--__-_______
//         trailer        chan     m1         m2
//
//see /home/thus/txt/silverlit/thus.txt
//m1, m2 most significant bit first |m1-m2| <= 9
//chan: 01=C 10=B
//chk = 0 - chan -m1>>2 -m1 -m2>>2 -m2
//<= 500us Probleme
//>= 650us Probleme
//periode orig: 450ms
inline void __attribute__ ((always_inline)) setupPulsesSilver()
{
  int8_t chan=1; //chan 1=C 2=B 0=A?

  switch(g_model.protocol)
  {
    case PROTO_SILV_A: chan=0; break;
    case PROTO_SILV_B: chan=2; break;
    case PROTO_SILV_C: chan=1; break;
  }

  int8_t m1 = (uint16_t)(g_chans512[0]+1024)*2 / 256;
  int8_t m2 = (uint16_t)(g_chans512[1]+1024)*2 / 256;
  if (m1 < 0)    m1=0;
  if (m2 < 0)    m2=0;
  if (m1 > 15)   m1=15;
  if (m2 > 15)   m2=15;
  if (m2 > m1+9) m1=m2-9;
  if (m1 > m2+9) m2=m1-9;
  //uint8_t i=0;

  send_hilo_silv(5,1); //idx 0 erzeugt pegel=0 am Ausgang, wird  als high gesendet
  send2BitsSilv(0);
  send_hilo_silv(2,1);
  send_hilo_silv(2,1);

  send2BitsSilv(chan); //chan 1=C 2=B 0=A?
  uint8_t sum = 0 - chan;

  send2BitsSilv(m1>>2); //m1
  sum-=m1>>2;
  send2BitsSilv(m1);
  sum-=m1;

  send2BitsSilv(m2>>2); //m2
  sum-=m2>>2;
  send2BitsSilv(m2);
  sum-=m2;

  send2BitsSilv(sum); //chk

  sendBitSilv(0);
  pulses2MHzWPtr -= sizeof(uint16_t);
  send_hilo_silv(50,0); //low-impuls (pegel=1) ueberschreiben
}

#endif

#ifdef CTP1009

/*
  TRACE CTP-1009
   - = send 45MHz
   _ = send nix
    start1       0      1           start2
  -------__     --_    -__         -----__
   7ms   2     .8 .4  .4 .8         5   2

 frame:
  start1  24Bits_1  start2  24_Bits2

 24Bits_1:
  7 x Bits  Throttle lsb first
  1 x 0

  6 x Bits  rotate lsb first
  1 x Bit   1=rechts
  1 x 0

  4 x Bits  chk5 = nib2 ^ nib4
  4 x Bits  chk6 = nib1 ^ nib3

 24Bits_2:
  7 x Bits  Vorwaets lsb first 0x3f = mid
  1 x 1

  7 x Bits  0x0e lsb first
  1 x 1

  4 x Bits  chk5 = nib2 ^ nib4
  4 x Bits  chk6 = nib1 ^ nib3

 */

#define BIT_TRA (400u*2)
void sendBitTra(uint8_t val)
{
  if(val) _send_hilo( BIT_TRA*1 , BIT_TRA*2 );
  else    _send_hilo( BIT_TRA*2 , BIT_TRA*1 );
}
void sendByteTra(uint8_t val)
{
  for(uint8_t i=0; i<8; i++, val>>=1) sendBitTra(val&1);
}

inline void __attribute__ ((always_inline)) setupPulsesTracerCtp1009()
{
  static bool phase;
  if( (phase=!phase) ){
    uint8_t thr = min(127u,(uint16_t)(g_chans512[0]+1024+8) /  16u);
    uint8_t rot;
    if (g_chans512[1] >= 0)
    {
      rot = min(63u,(uint16_t)( g_chans512[1]+16) / 32u) | 0x40;
    }else{
      rot = min(63u,(uint16_t)(-g_chans512[1]+16) / 32u);
    }
    sendByteTra(thr);
    sendByteTra(rot);
    uint8_t chk=thr^rot;
    sendByteTra( (chk>>4) | (chk<<4) );
    _send_hilo( 5000*2, 2000*2 );
  }else{
    uint8_t fwd = min(127u,(uint16_t)(g_chans512[2]+1024) /  16u) | 0x80;
    sendByteTra(fwd);
    sendByteTra(0x8e);
    uint8_t chk=fwd^0x8e;
    sendByteTra( (chk>>4) | (chk<<4) );
    _send_hilo( 7000*2, 2000*2 );
  }
  if((pulses2MHzWPtr-pulses2MHz) >= sizeof(pulses2MHz)) alert(STR_PULSETABOVERFLOW);
}

#endif

void setupPulses()
{
#if defined(DSM2_PPM) || defined(PXX)
  if (s_current_protocol != g_model.protocol) {
    s_current_protocol = g_model.protocol;
    // switch mode here
    switch (g_model.protocol) {
#if defined(DSM2_PPM)
      case PROTO_DSM2:
        TCCR1B = 0; // Stop counter
        OCR1B = 200; // 100 uS
        TCNT1 = 300; // Past the OCR1B value
        ICR1 = 44000; // Next frame starts in 22 mS
#if defined(PCBV3)
        TIMSK1 &= ~0x3C; // All interrupts off
        TIFR1 = 0x2F;
        TIMSK1 |= 0x28; // Enable CAPT and COMPB
#else
        TIMSK &= ~0x3C; // All interrupts off
        TIFR = 0x3C;
        TIMSK |= 0x28; // Enable CAPT and COMPB
#endif
        TCCR1A = (0 << WGM10);
        TCCR1B = (3 << WGM12) | (2 << CS10); // CTC ICR, 16MHz / 8
        break;
#endif

#if defined(PXX)
      case PROTO_PXX:
        // do nothing, not yet implemented
        break;
#endif

      default:
        TCCR1B = 0; // Stop counter
        OCR1A = 40000; // Next frame starts in 20 mS
        TCNT1 = 0;
#if defined(PCBV3)
        TIMSK1 &= ~0x3C; // All interrupts off
        TIFR1 = 0x2F;
        TIMSK1 |= 0x10; // Enable COMPA
#else
        TIMSK &= ~0x3C; // All interrupts off
        TIFR = 0x3C;
        TIMSK |= 0x10; // Enable COMPA
#endif
        TCCR1A = (0 << WGM10);
        TCCR1B = (1 << WGM12) | (2 << CS10); // CTC OCRA, 16MHz / 8
        break;
    }
  }
#endif

  pulses2MHzWPtr = pulses2MHz;

  switch(g_model.protocol) {
#ifdef SILVER
    case PROTO_SILV_A:
    case PROTO_SILV_B:
    case PROTO_SILV_C:
      setupPulsesSilver();
      break;
#endif
#ifdef CTP1009
    case PROTO_CTP1009:
      setupPulsesTracerCtp1009();
      break;
#endif
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
    default:
      // no sei here
      setupPulsesPPM();
      break;
  }

  pulses2MHzRPtr = (uint8_t *)pulses2MHz;
}

#if defined(DSM2_PPM) || defined(PXX)
ISR(TIMER1_CAPT_vect) // 2MHz pulse generation
{
  uint8_t x ;
  PORTB ^= (1<<OUT_B_PPM);
  x = *pulses2MHzRPtr++;      // Byte size
  ICR1 = x ;
  if (x > 200) PORTB |= (1<<OUT_B_PPM); // Make sure pulses are the correct way up
  heartbeat |= HEART_TIMER2Mhz; // TODO why not in TIMER1_COMPB_vect (in setupPulses)?
}

ISR(TIMER1_COMPB_vect) // DSM2 end of frame
{
  ICR1 = 41536 ; // next frame starts in 22ms 41536 = 2*(22000 - 14*11*8)
  if (OCR1B < 255) {
    OCR1B = 39000;  // delay setup pulses by 19.5ms to reduce system latency
  }
  else {
    OCR1B = 200;
    // sei will be called inside setupPulses()
    setupPulses();
  }
}
#endif
