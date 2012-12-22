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

#include "../open9x.h"

#ifndef SIMU
inline void boardInit()
{
  // Set up I/O port data directions and initial states
  DDRA = 0xff;  PORTA = 0x00; // LCD data
  DDRB = 0x81;  PORTB = 0x7e; //pullups keys+nc
  DDRC = 0x3e;  PORTC = 0xc1; //pullups nc
  DDRD = 0x00;  PORTD = 0xff; //pullups keys
  DDRE = (1<<OUT_E_BUZZER);  PORTE = 0xff-(1<<OUT_E_BUZZER); //pullups + buzzer 0
  DDRF = 0x00;  PORTF = 0x00; //anain
  DDRG = 0x14;  PORTG = 0xfb; //pullups + SIM_CTL=1 = phonejack = ppm_in, Haptic output and off (0)

  ADMUX=ADC_VREF_TYPE;
  ADCSRA=0x85; // ADC enabled, pre-scaler division=32 (no interrupt, no auto-triggering)

  /**** Set up timer/counter 0 ****/
#if defined (AUDIO)
  // TCNT0  10ms = 16MHz/1024/2(/78) periodic timer (for speaker tone generation)
  //        Capture ISR 7812.5/second -- runs per-10ms code segment once every 78
  //        cycles (9.984ms). Timer overflows at about 61Hz or once every 16ms.
  TCCR0  = (0b111 << CS00);//  Norm mode, clk/1024
  OCR0 = 2;
#else
  // TCNT0  10ms = 16MHz/1024/156 periodic timer (9.984ms)
  // (with 1:4 duty at 157 to average 10.0ms)
  // Timer overflows at about 61Hz or once every 16ms.
  TCCR0  = (0b111 << CS00);//  Norm mode, clk/1024
  OCR0 = 156;
#endif

  TIMSK |= (1<<OCIE0) |  (1<<TOIE0); // Enable Output-Compare and Overflow interrrupts
  /********************************/
}
#endif

#ifndef SIMU
FORCEINLINE
#endif
uint8_t keyDown()
{
  // printf("PINB=%x\n", PINB & 0x7E); fflush(stdout);
  return (~PINB) & 0x7E;
}

bool keyState(EnumKeys enuk)
{
  uint8_t result = 0 ;

  if (enuk < (int)DIM(keys))
    return keys[enuk].state();

  switch(enuk){
    case SW_ELE:
      result = PINE & (1<<INP_E_ElevDR);
      break;

#if defined(JETI) || defined(FRSKY) || defined(ARDUPILOT) || defined(NMEA) || defined(MAVLINK)
    case SW_AIL:
      result = PINC & (1<<INP_C_AileDR); //shad974: rerouted inputs to free up UART0
      break;
#else
    case SW_AIL:
      result = PINE & (1<<INP_E_AileDR);
      break;
#endif

    case SW_RUD:
      result = PING & (1<<INP_G_RuddDR);
      break;
      //     INP_G_ID1 INP_E_ID2
      // id0    0        1
      // id1    1        1
      // id2    1        0
    case SW_ID0:
      result = !(PING & (1<<INP_G_ID1));
      break;

    case SW_ID1:
      result = (PING & (1<<INP_G_ID1)) && (PINE & (1<<INP_E_ID2));
      break;

    case SW_ID2:
      result = !(PINE & (1<<INP_E_ID2));
      break;

    case SW_GEA:
      result = PINE & (1<<INP_E_Gear);
      break;

    //case SW_THR  : return PINE & (1<<INP_E_ThrCt);

#if defined(JETI) || defined(FRSKY) || defined(ARDUPILOT) || defined(NMEA) || defined(MAVLINK)
    case SW_THR:
      result = PINC & (1<<INP_C_ThrCt); //shad974: rerouted inputs to free up UART0
      break;

#else
    case SW_THR:
      result = PINE & (1<<INP_E_ThrCt);
      break;
#endif

    case SW_TRN:
      result = PINE & (1<<INP_E_Trainer);
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
  uint8_t enuk = KEY_MENU;

  // User buttons ...
  uint8_t in = ~PINB;

  for(int i=1; i<7; i++)
  {
    //INP_B_KEY_MEN 1  .. INP_B_KEY_LFT 6
    keys[enuk].input(in & (1<<i),(EnumKeys)enuk);
    ++enuk;
  }

  // Trim switches ...
  static const pm_uchar crossTrim[] PROGMEM ={
    1<<INP_D_TRM_LH_DWN,  // bit 7
    1<<INP_D_TRM_LH_UP,
    1<<INP_D_TRM_LV_DWN,
    1<<INP_D_TRM_LV_UP,
    1<<INP_D_TRM_RV_DWN,
    1<<INP_D_TRM_RV_UP,
    1<<INP_D_TRM_RH_DWN,
    1<<INP_D_TRM_RH_UP    // bit 0
  };

  in = ~PIND;

  for (int i=0; i<8; i++) {
    // INP_D_TRM_RH_UP   0 .. INP_D_TRM_LH_UP   7
    keys[enuk].input(in & pgm_read_byte(crossTrim+i),(EnumKeys)enuk);
    ++enuk;
  }
}

bool checkSlaveMode()
{
  // no power -> only phone jack = slave mode
  static bool lastSlaveMode = false;
  static uint8_t checkDelay = 0;
  if (IS_AUDIO_BUSY()) {
    checkDelay = 20;
  }
  else if (checkDelay) {
    --checkDelay;
  }
  else {
    lastSlaveMode = (PING & (1<<INP_G_RF_POW));
  }
  return lastSlaveMode;
}
