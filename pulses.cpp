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

#include "gruvin9x.h"

#ifndef SIMU

#ifdef CTP1009
uint16_t pulses2MHz[50] = {0};
#else
uint16_t pulses2MHz[40] = {0};
#endif

uint16_t *pulses2MHzRPtr = pulses2MHz;
uint16_t *pulses2MHzWPtr = pulses2MHz;

#define CTRL_END 0
#define CTRL_CNT 1
#define CTRL_REP_1CMD -3
#define CTRL_REP_2CMD -6

ISR(TIMER1_COMPA_vect) //2MHz pulse generation
{
  static uint8_t   pulsePol; // TODO strange, it's always 0 at first, shouldn't it be initialized properly in setupPulses?

  // Latency -- how far further on from interrupt trigger has the timer counted?
  // (or -- how long did it take to get to this function)
  uint8_t dt = TCNT1L;
  if (dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if (dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;

  // vinceofdrink@gmail harwared ppm
  // Orginal bitbang for PPM
#if !defined (DPPMPB7_HARDWARE) && !defined (PCBV4)
  if (pulsePol) {
    PORTB |=  (1<<OUT_B_PPM); // GCC optimisation should result in a single SBI instruction
    pulsePol = 0;
  }
  else {
    PORTB &= ~(1<<OUT_B_PPM);
    pulsePol = 1;
  }
#endif

  OCR1A = *pulses2MHzRPtr; // Schedule next interrupt vector (to this handler)

#if defined (PCBV4)
  OCR1B = *pulses2MHzRPtr; /* G: Using timer in CTC mode, restricted to using OCR1A for interrupt triggering.
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
#elif defined (DPPMPB7_HARDWARE)
  OCR1C = *pulses2MHzRPtr;  // just copy the value of the OCR1A to OCR1C to test PPM out without too
                      // much change in the code not optimum but will not alter ppm precision
#endif

  ++pulses2MHzRPtr;

  if (pulses2MHzRPtr == pulses2MHzWPtr) {
    //currpulse=0;
    pulsePol = g_model.pulsePol;//0;
    //    channel = 0 ;
    //    PulseTotal = 0 ;

    // cli(); // TODO I remove this cli, we are in a blocking interrupt
#if defined (PCBV3)
    TIMSK1 &= ~(1<<OCIE1A); //stop reentrance
#else
    TIMSK &= ~(1<<OCIE1A); //stop reentrance
#endif
    sei();
    setupPulses();

#if !defined (PCBV3) && defined (DPPMPB7_HARDWARE)
    // G: NOTE: This strategy does not work on the '2560 becasue you can't
    //          read the PPM out pin when connected to OC1B. Vincent says
    //          it works on the '64A. I haven't personally tested it.
    if (PINB & (1<<OUT_B_PPM) && g_model.pulsePol)
      TCCR1C=(1<<FOC1C);
#endif

    cli();
#if defined (PCBV3)
    TIMSK1 |= (1<<OCIE1A);
#else
    TIMSK |= (1<<OCIE1A);
#endif
    // sei(); TODO I remove this sei
  }
  heartbeat |= HEART_TIMER2Mhz;
}

//inline int16_t reduceRange(int16_t x)  // for in case we want to have room for subtrims
//{
//    return x-(x/4);  //512+128 =? 640,  640 - 640/4  == 640 * 3/4 => 480 (just below 500msec - it can still reach 500 with offset)
//}
//int16_t PPM_range = 512*2;   //range of 0.7..1.7msec
//uint16_t PPM_gap = 300 * 2; //Stoplen *2
//uint16_t PPM_frame ;

inline void __attribute__ ((always_inline)) setupPulsesPPM() // changed 10/05/2010 by dino Issue 128
{
#define PPM_CENTER 1200*2
    int16_t PPM_range = g_model.extendedLimits ? 640*2 : 512*2;   //range of 0.7..1.7msec

    //Total frame length = 22.5msec
    //each pulse is 0.7..1.7ms long with a 0.3ms stop tail
    //The pulse ISR is 2mhz that's why everything is multiplied by 2

    // G: Found the following reference at th9x. The below code does not seem
    // to produce quite exactly this, to my eye. *shrug*
    //   http://www.aerodesign.de/peter/2000/PCM/frame_ppm.gif
    uint16_t *ptr = pulses2MHz ; // TODO check this saves flash
    uint8_t p = 8+(g_model.ppmNCH*2); // channels count
    uint16_t q = (g_model.ppmDelay*50+300)*2; //Stoplen *2
    uint16_t rest = 22500u*2-q; //Minimum Framelen=22.5 ms
    // TODO in er9x this line replaces the next one: rest += (int16_t(g_model.ppmFrameLength))*1000;
    if(p>9) rest=p*(1720u*2 + q) + 4000u*2; //for more than 9 channels, frame must be longer
    for (uint8_t i=0; i<p; i++) {
      int16_t v = max(min(g_chans512[i], PPM_range), -PPM_range) + PPM_CENTER;
      rest -= (v+q);
      *ptr++ = q;
      *ptr++ = v - q + 600; /* as Pat MacKenzie suggests */
    }
    *ptr = q;
    *(ptr+1) = rest;
    pulses2MHzWPtr = ptr+2;
}

#ifdef PXX

inline void __attribute__ ((always_inline)) setupPulsesPXX()
{

}

#endif

#ifdef DSM2

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

inline void __attribute__ ((always_inline)) _send_1(uint16_t v)
{
  *pulses2MHzWPtr++ = v;
}

#define BITLEN_DSM2 (8*2) //125000 Baud
inline void __attribute__ ((always_inline)) sendByteDsm2(uint8_t b) //max 10changes 0 10 10 10 10 1
{
    bool    lev = 0;
    uint8_t len = BITLEN_DSM2; //max val: 9*16 < 256
    for( uint8_t i=0; i<=8; i++){ //8Bits + Stop=1
        bool nlev = b & 1; //lsb first
        if(lev == nlev){
            len += BITLEN_DSM2;
        }else{
            _send_1(len-1);
            len = BITLEN_DSM2;
            lev = nlev;
        }
        b = (b>>1) | 0x80; //shift in stop bit
    }
    _send_1(len + 10*BITLEN_DSM2 -1); //some more space-time for security
}


inline void __attribute__ ((always_inline)) setupPulsesDsm2(uint8_t chns)
{
    static uint8_t dsmDat[2+6*2]={0x80,0,  0x00,0xAA,  0x05,0xFF,  0x09,0xFF,  0x0D,0xFF,  0x13,0x54,  0x14,0xAA};

    static uint8_t state = 0;

    if(state==0){

        if((dsmDat[0] == 0) || ! keyState(SW_Trainer) ){ //init - bind!
            dsmDat[0]=0; dsmDat[1]=0;  //DSM2_Header = 0,0;
            for(uint8_t i=0; i<chns; i++){
                uint16_t pulse = limit(0, g_chans512[i]+512,1023);
                dsmDat[2+2*i] = (i<<2) | ((pulse>>8)&0x03);
                dsmDat[3+2*i] = pulse & 0xff;
            }
        }
    }
    sendByteDsm2(dsmDat[state++]);
    sendByteDsm2(dsmDat[state++]);
    if(state >= 2+chns*2){
        pulses2MHzWPtr--; //remove last stopbits and
        _send_1(20000u*2 -1); //prolong them
        state=0;
    }
}

#endif

#if defined(SILVER) || defined(CTP1009)
void _send_hilo(uint16_t hi,uint16_t lo)
{
  *pulses2MHzWPtr++=hi;
  *pulses2MHzWPtr++=lo;
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
  pulses2MHzWPtr--;
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
  if((pulses2MHzWPtr-pulses2MHz) >= (signed)DIM(pulses2MHz)) alert(PSTR("pulse tab overflow"));
}

#endif

void setupPulses()
{
  pulses2MHzWPtr = pulses2MHz;
  pulses2MHzRPtr = pulses2MHz;

  switch(g_model.protocol) {
    case PROTO_PPM:
      setupPulsesPPM();
      break;
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
      setupPulsesPXX();
      break;
#endif
#ifdef DSM2
    case PROTO_DSM2:
      setupPulsesDsm2(6);
      break;
#endif
  }
}
#endif

