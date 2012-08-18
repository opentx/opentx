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

#ifdef SDCARD
uint8_t g_ms100 = 0; // global to allow time set function to reset to zero
#endif

#ifndef SIMU
inline void board_init()
{
  // Set up I/O port data directions and initial states
  DDRA = 0xff;  PORTA = 0x00; // LCD data
  DDRB = 0b11000111;  PORTB = 0b00111111; // 7:SPKR, 6:PPM_OUT,  5:TrainSW,  4:IDL2_SW, SDCARD[3:MISO 2:MOSI 1:SCK 0:CS]
  DDRC = 0x3f;  PORTC = 0xc0; // 7:AilDR, 6:EleDR, LCD[5,4,3,2,1], 0:BackLight
  DDRD = 0b11000000;  PORTD = 0b11111100; // 7:VIB, 6:LED BL, 5:RENC2_PUSH, 4:RENC1_PUSH, 3:RENC2_B, 2:RENC2_A, 1:I2C_SDA, 0:I2C_SCL
  DDRE = 0b00001010;  PORTE = 0b11110100; // 7:PPM_IN, 6: RENC1_B, 5:RENC1_A, 4:USB_DNEG, 3:BUZZER, 2:USB_DPOS, 1:TELEM_TX, 0:TELEM_RX(pull-up off)
  DDRF = 0x00;  PORTF = 0x00; // 7-4:JTAG, 3:ADC_REF_1.2V input, 2-0:ADC_SPARE_2-0
  DDRG = 0b00010000;  PORTG = 0xff; // 7-6:N/A, 5:GearSW, 4: Sim_Ctrl[out], 3:IDL1_Sw, 2:TCut_Sw, 1:RF_Power[in], 0: RudDr_Sw
  DDRH = 0b00110000;  PORTH = 0b11011111; // 7:0 Spare port [6:SOMO14D-BUSY 5:SOMO14D-DATA 4:SOMO14D-CLK] [2:VIB_OPTION -- setting to input for now]
  DDRJ = 0x00;  PORTJ = 0xff; // 7-0:Trim switch inputs
  DDRK = 0x00;  PORTK = 0x00; // anain. No pull-ups!
#ifdef REV0
  DDRL = 0x80;  PORTL = 0x7f; // 7: Hold_PWR_On (1=On, default Off), 6:Jack_Presence_TTL, 5-0: User Button inputs
#else
  DDRL = 0x80;  PORTL = 0xff; // 7: Hold_PWR_On (1=On, default Off), 6:Jack_Presence_TTL, 5-0: User Button inputs
#endif

  ADMUX=ADC_VREF_TYPE;
  ADCSRA=0x85; // ADC enabled, pre-scaler division=32 (no interrupt, no auto-triggering)
  ADCSRB=(1<<MUX5);

  /**** Set up timer/counter 0 ****/
  /** Move old 64A Timer0 functions to Timer2 and use WGM on OC0(A) (PB7) for spkear tone output **/

  // TCNT0  10ms = 16MHz/1024/156(.25) periodic timer (100ms interval)
  //        cycles at 9.984ms but includes 1:4 duty cycle correction to /157 to average at 10.0ms
  TCCR2B  = (0b111 << CS20); // Norm mode, clk/1024 (differs from ATmega64 chip)
  OCR2A   = 156;
  TIMSK2 |= (1<<OCIE2A) |  (1<<TOIE2); // Enable Output-Compare and Overflow interrrupts

  // Set up Phase correct Waveform Gen. mode, at clk/64 = 250,000 counts/second
  // (Higher speed allows for finer control of frquencies in the audio range.)
  // Used for audio tone generation
  TCCR0B  = (1<<WGM02) | (0b011 << CS00);
  TCCR0A  = (0b01<<WGM00);

#if defined(SDCARD)
  // Initialise global unix timestamp with current time from RTC chip on SD card interface
  RTC rtc = {0,0,0,0,0,0,0};
  struct gtm utm;
  rtc_gettime(&rtc);
  utm.tm_year = rtc.year - 1900;
  utm.tm_mon =  rtc.month - 1;
  utm.tm_mday = rtc.mday;
  utm.tm_hour = rtc.hour;
  utm.tm_min =  rtc.min;
  utm.tm_sec =  rtc.sec;
  utm.tm_wday = rtc.wday - 1;
  g_unixTime = gmktime(&utm);
#endif

  /***************************************************/
  /* Rotary encoder interrupt set-up (V4 board only) */

  // All external interrupts initialise to disabled. But maybe not after 
  // a WDT or BOD event? So to be safe ...
  EIMSK = 0; // disable ALL external interrupts.

  // encoder 1
  EICRB = (1<<ISC60) | (1<<ISC50); // 01 = interrupt on any edge
  EIFR = (3<<INTF5); // clear the int. flag in case it got set when changing modes

  // encoder 2
  EICRA = (1<<ISC30) | (1<<ISC20); // do the same for encoder 1
  EIFR = (3<<INTF2);

#if defined(EXTRA_ROTARY_ENCODERS)
  EIMSK = (3<<INT5); // enable the ONE rot. enc. ext. int. pairs.
#else
  EIMSK = (3<<INT5) | (3<<INT2); // enable the two rot. enc. ext. int. pairs.
#endif
  /***************************************************/

  /*
   * SOMO set-up (V4 board only)
   */
  OCR4A = 0x7d;
  TCCR4B = (1 << WGM42) | (3<<CS40); // CTC OCR1A, 16MHz / 64 (4us ticks)

  /* Mixer interrupt */
  OCR5A = 0x7d * 40; /* 20ms */
  TCCR5B = (1 << WGM52) | (3<<CS50); // CTC OCR1A, 16MHz / 64 (4us ticks)
  TIMSK5 |= (1<<OCIE5A);

#if defined(EXTRA_ROTARY_ENCODERS)
  //configure uart1 here
  DDRD &= ~(1 << 2);
  PORTD &= ~(1 << 2);
  #define EXTRA_ROTARY_ENCODERS_USART_BAUD 9600UL
  UBRR1 = F_CPU/(16*EXTRA_ROTARY_ENCODERS_USART_BAUD)-1;
  //9 bit mode
  UCSR1C = (1<<USBS1)|(3<<UCSZ10);
  UCSR1B = (1<<RXEN1)|(0<<TXEN1)|(1<<UCSZ12);
  UCSR1B |= 1 << RXCIE1; //enable interrupt on rx
#endif //EXTRA_ROTARY_ENCODERS
}

#if defined(EXTRA_ROTARY_ENCODERS)

uint8_t vpotToChange = 0;
uint8_t vpot_mod_state = 0;

ISR(USART1_RX_vect)
{
  //receive data from extension board
  //bit 9 = 1 mean encoder number, following byte with bit 9 = 0 is increment value
  /* Get status and 9th bit, then data */
  /* from buffer */
  uint16_t resh = UCSR1B;
  uint16_t resl = UDR1;
  uint16_t res = 0;
  
  /* Filter the 9th bit, then return */
  resh = (resh >> 1) & 0x01;
  res = ((resh << 8) | resl);  
  if((res == 0x180) | (res == 0x1C0)){         //button REb filter
    keys[BTN_REa].input((res & 0x1C0) == 0x1C0, BTN_REa);
  } else if((res & 0x100) == 0x100){  //rotary filter
    vpotToChange = res & 0xEF;
    vpot_mod_state = 1;
  }
  else {
    if(vpot_mod_state  & (vpotToChange < NUM_ROTARY_ENCODERS))
    {
      int8_t vpot_inc = res & 0xFF;
      if(vpot_inc){
        incRotaryEncoder(vpotToChange, vpot_inc);
      }    
      vpot_mod_state = 0;
    }
  }
}
#endif //EXTRA_ROTARY_ENCODERS

#endif

uint8_t check_soft_power()
{
#if !defined(SIMU) && !defined(REV0)
  if ((PING & 0b00000010) && (~PINL & 0b01000000))
    return e_power_off;
#endif
  return e_power_on;
}

void soft_power_off()
{
#ifndef REV0
  PORTL = 0x7f;
#endif
}

#ifndef SIMU
FORCEINLINE
#endif
uint8_t keyDown()
{
  return (~PINL) & 0x3F;
}

bool keyState(EnumKeys enuk)
{
  uint8_t result = 0 ;

  if (enuk < (int)DIM(keys))
    return keys[enuk].state() ? 1 : 0;

  switch(enuk){
    case SW_ElevDR:
      result = PINC & (1<<INP_C_ElevDR);
      break;

    case SW_AileDR:
      result = PINC & (1<<INP_C_AileDR);
      break;

    case SW_RuddDR:
      result = PING & (1<<INP_G_RuddDR);
      break;
      //     INP_G_ID1 INP_B_ID2
      // id0    0        1
      // id1    1        1
      // id2    1        0
    case SW_ID0:
      result = !(PING & (1<<INP_G_ID1));
      break;

    case SW_ID1:
      result = (PING & (1<<INP_G_ID1))&& (PINB & (1<<INP_B_ID2));
      break;

    case SW_ID2:
      result = !(PINB & (1<<INP_B_ID2));
      break;

    case SW_Gear:
      result = PING & (1<<INP_G_Gear);
      break;

    case SW_ThrCt:
      result = PING & (1<<INP_G_ThrCt);
      break;

    case SW_Trainer:
      result = PINB & (1<<INP_B_Trainer);
      break;

    default:
      break;
  }

  return result;
}

#ifndef SIMU
FORCEINLINE
#endif
void readKeysAndTrims()
{
  /* Original keys were connected to PORTB as follows:

     Bit  Key
      7   other use
      6   LEFT
      5   RIGHT
      4   UP
      3   DOWN
      2   EXIT
      1   MENU
      0   other use
  */

  uint8_t enuk = KEY_MENU;

#if !defined(EXTRA_ROTARY_ENCODERS)
  keys[BTN_REa].input(~PIND & 0x20, BTN_REa);
#endif //!EXTRA_ROTARY_ENCODERS
  keys[BTN_REb].input(~PIND & 0x10, BTN_REb);

  uint8_t tin = ~PINL;
  uint8_t in;
  in = (tin & 0x0f) << 3;
  in |= (tin & 0x30) >> 3;

  for(int i=1; i<7; i++)
  {
    //INP_B_KEY_MEN 1  .. INP_B_KEY_LFT 6
    keys[enuk].input(in & (1<<i),(EnumKeys)enuk);
    ++enuk;
  }

  // Trim switches ...
  static const pm_uchar crossTrim[] PROGMEM ={
    1<<INP_J_TRM_LH_DWN,
    1<<INP_J_TRM_LH_UP,
    1<<INP_J_TRM_LV_DWN,
    1<<INP_J_TRM_LV_UP,
    1<<INP_J_TRM_RV_DWN,
    1<<INP_J_TRM_RV_UP,
    1<<INP_J_TRM_RH_DWN,
    1<<INP_J_TRM_RH_UP
  };

  in = ~PINJ;

  for (int i=0; i<8; i++) {
    // INP_D_TRM_RH_UP   0 .. INP_D_TRM_LH_UP   7
    keys[enuk].input(in & pgm_read_byte(crossTrim+i),(EnumKeys)enuk);
    ++enuk;
  }
}
