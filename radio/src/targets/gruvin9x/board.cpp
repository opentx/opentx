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

void boardInit()
{
#if !defined(SIMU)
  // Set up I/O port data directions and initial states
  DDRA = 0xff;  PORTA = 0x00; // LCD data
  DDRB = 0b11000111;  PORTB = 0b00111111; // 7:SPKR, 6:PPM_OUT,  5:TrainSW,  4:IDL2_SW, SDCARD[3:MISO 2:MOSI 1:SCK 0:CS]
  DDRC = 0x3f;  PORTC = 0xc0; // 7:AilDR, 6:EleDR, LCD[5,4,3,2,1], 0:BackLight
  DDRD = 0b11000000;  PORTD = 0b11111100; // 7:VIB, 6:LED BL, 5:RENC2_PUSH, 4:RENC1_PUSH, 3:RENC2_B, 2:RENC2_A, 1:I2C_SDA, 0:I2C_SCL
  DDRE = 0b00001010;  PORTE = 0b11110100; // 7:PPM_IN, 6: RENC1_B, 5:RENC1_A, 4:USB_DNEG, 3:BUZZER, 2:USB_DPOS, 1:TELEM_TX, 0:TELEM_RX(pull-up off)
  DDRF = 0x00;  PORTF = 0x00; // 7-4:JTAG, 3:ADC_REF_1.2V input, 2-0:ADC_SPARE_2-0
  DDRG = 0b00010000;  PORTG = 0xff; // 7-6:N/A, 5:GearSW, 4: Sim_Ctrl[out], 3:IDL1_Sw, 2:TCut_Sw, 1:RF_Power[in], 0: RudDr_Sw

#if defined(DEBUG) && !defined(VOICE)
  DDRH = 0b11111000;  PORTH = 0b11010111; // PORTH:7-6 enabled for timing analysis output ... see below ...
#else
  DDRH = 0b10111000;  PORTH = 0b11010111; // [7:0 DSM/PPM TX-caddy control. 1=PPM, 0=DSM ]
                                          // [6:SOMO14D-BUSY 5:SOMO14D-DATA 4:SOMO14D-CLK 3:SOMO14D-RESET]
                                          // [2:VIB_OPTION -- setting to input for now]
                                          // [1:TxD 0:RxD Spare serial port]
#endif

  DDRJ = 0x00;  PORTJ = 0xff; // 7-0:Trim switch inputs
  DDRK = 0x00;  PORTK = 0x00; // anain. No pull-ups!
#ifdef REV0
  DDRL = 0x80;  PORTL = 0x7f; // 7: Hold_PWR_On (1=On, default Off), 6:Jack_Presence_TTL, 5-0: User Button inputs
#else
  DDRL = 0x80;  PORTL = 0xff; // 7: Hold_PWR_On (1=On, default Off), 6:Jack_Presence_TTL, 5-0: User Button inputs
#endif

  adcInit();

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

#if ROTARY_ENCODERS > 2
  EIMSK = (3<<INT5); // enable the ONE rot. enc. ext. int. pairs.
#else
  EIMSK = (3<<INT5) | (3<<INT2); // enable the two rot. enc. ext. int. pairs.
#endif
  /***************************************************/

#if defined(VOICE)
  /*
   * SOMO set-up (V4 board only)
   */
  OCR4A = 0x1F4; //2ms
  TCCR4B = (1 << WGM42) | (0b011 << CS40); // CTC OCR1A, 16MHz / 64 (4us ticks)
  TIMSK4 |= (1<<OCIE4A); // Start the interrupt so the unit reset can occur
#endif

#if ROTARY_ENCODERS > 2
  //configure uart1 here
  DDRD &= ~(1 << 2);
  PORTD &= ~(1 << 2);
  #define EXTRA_ROTARY_ENCODERS_USART_BAUD 9600UL
  UBRR1 = F_CPU/(16*EXTRA_ROTARY_ENCODERS_USART_BAUD)-1;
  //9 bit mode
  UCSR1C = (1<<USBS1)|(3<<UCSZ10);
  UCSR1B = (1<<RXEN1)|(0<<TXEN1)|(1<<UCSZ12);
  UCSR1B |= 1 << RXCIE1; //enable interrupt on rx
#endif
#endif // !defined(SIMU)
}

#if ROTARY_ENCODERS > 2
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
    keys[BTN_REa].input((res & 0x1C0) == 0x1C0);
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
#endif

uint8_t pwrCheck()
{
#if !defined(SIMU) && !defined(REV0)
  if ((PING & 0b00000010) && (~PINL & 0b01000000))
    return e_power_off;
#endif
  return e_power_on;
}

void pwrOff()
{
#ifndef REV0
  PORTL = 0x7f;
#endif
}

uint8_t keyDown()
{
  return ((~PINL) & 0x3F) || ROTENC_DOWN();
}

uint8_t keyState(uint8_t index)
{
  return keys[index].state();
}

uint8_t switchState(uint8_t index)
{
  uint8_t result = 0;
  
  switch (index) {
    case SW_ELE:
      result = PINC & (1<<INP_C_ElevDR);
      break;

    case SW_AIL:
      result = PINC & (1<<INP_C_AileDR);
      break;

    case SW_RUD:
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

    case SW_GEA:
      result = PING & (1<<INP_G_Gear);
      break;

    case SW_THR:
      result = PING & (1<<INP_G_ThrCt);
      break;

    case SW_TRN:
      result = PINB & (1<<INP_B_Trainer);
      break;

    default:
      break;
  }

  return result;
}

static const pm_uchar crossTrim[] PROGMEM = {
  TRIMS_GPIO_PIN_LHL,
  TRIMS_GPIO_PIN_LHR,
  TRIMS_GPIO_PIN_LVD,
  TRIMS_GPIO_PIN_LVU,
  TRIMS_GPIO_PIN_RVD,
  TRIMS_GPIO_PIN_RVU,
  TRIMS_GPIO_PIN_RHL,
  TRIMS_GPIO_PIN_RHR
};

uint8_t trimDown(uint8_t idx)
{
  uint8_t in = ~PIND;
  return (in & pgm_read_byte(crossTrim+idx));
}

void readKeysAndTrims()
{
  uint8_t index = KEY_MENU;

  // User buttons ...
  uint8_t tin = ~PINL;
  uint8_t in;
  in = (tin & 0x0f) << 3;
  in |= (tin & 0x30) >> 3;
  for (int i=1; i<7; i++) {
    keys[index].input(in & (1<<i));
    ++index;
  }

  // Trims ...
  in = ~PINJ;
  for (int i=0; i<8; i++) {
    // INP_D_TRM_RH_UP   0 .. INP_D_TRM_LH_UP   7
    keys[index].input(in & pgm_read_byte(crossTrim+i));
    ++index;
  }

#if defined(ROTARY_ENCODERS)
  keys[BTN_REa].input(REA_DOWN());
  keys[BTN_REb].input(REB_DOWN());
#endif
}

ISR(INT2_vect)
{
  uint8_t input = (PIND & 0x0C);
  if (input == 0 || input == 0x0C) incRotaryEncoder(0, -1);
}

ISR(INT3_vect)
{
  uint8_t input = (PIND & 0x0C);
  if (input == 0 || input == 0x0C) incRotaryEncoder(0, +1);
}

ISR(INT5_vect)
{
  uint8_t input = (PINE & 0x60);
  if (input == 0 || input == 0x60) incRotaryEncoder(1, +1);
}

ISR(INT6_vect)
{
  uint8_t input = (PINE & 0x60);
  if (input == 0 || input == 0x60) incRotaryEncoder(1, -1);
}
