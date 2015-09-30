/*
 * Authors (alphabetical order)
 * - Aguerre Franck 
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

#include "../../opentx.h"

#ifndef SIMU
inline void boardInit()
{
  // Set up I/O port data directions and initial states (unused pin setting : input, pull-up on)
  DDRA = 0b11111111;  PORTA = 0b00000000; // LCD data
  DDRB = 0b01110111;  PORTB = 0b00101111; // 7:PPM_IN 6:PPM_OUT, 5:SimCTRL, 4:Buzzer, SDCARD[3:MISO 2:MOSI 1:SCK 0:CS]
  DDRC = 0b11111100;  PORTC = 0b00000011; // 7-3:LCD, 2:BackLight, 1:ID2_SW, 0:ID1_SW
  DDRD = 0b00000000;  PORTD = 0b11111100; // 7:AilDR_SW, 6:N/A, 5:N/A, 4:N/A, 3:RENC2_B, 2:RENC2_A, 1:I2C_SDA, 0:I2C_SCL
  DDRE = 0b00000010;  PORTE = 0b11111100; // 7:N/A, 6:N/A, 5:RENC1_B, 4:RENC1_A, 3:N/A, 2:N/A, 1:TELEM_TX, 0:TELEM_RX
  DDRF = 0b00000000;  PORTF = 0b11111111; // 7-0:Trim switch inputs
  DDRG = 0b00000000;  PORTG = 0b11111111; // 7:N/A, 6:N/A, 5:N/A, 4:N/A, 3:N/A, 2:TCut_SW, 1:Gear_SW, 0: RudDr_SW
#if defined(PCBMEGA2560) && defined(DEBUG)  
  DDRH = 0b01011000;  PORTH = 0b11110110; // 7:N/A, 6:RF_Activated, 5:DSC_Activated, 4:Hold_Power, 3:Speaker, 2:N/A, 1:N/A, 0:Haptic
#else
  DDRH = 0b00011000;  PORTH = 0b11110110; // 7:N/A, 6:RF_Activated, 5:DSC_Activated, 4:Hold_Power, 3:Speaker, 2:N/A, 1:N/A, 0:Haptic
#endif  
  DDRJ = 0b00000000;  PORTJ = 0b11111111; // 7:N/A, 6:N/A, 5:N/A, 4:N/A, 3:N/A, 2:N/A, 1:RENC2_push, 0:RENC1_push
  DDRK = 0b00000000;  PORTK = 0b00000000; // Analogic input (no pull-ups)
  DDRL = 0b00000000;  PORTL = 0b11111111; // 7:TRN_SW 6:EleDR_SW, 5:ESC, 4:MENU 3:Keyb_Left, 2:Keyb_Right, 1:Keyb_Up, 0:Keyb_Down
  
  adcInit();

  /**** Set up timer/counter 0 ****/
  // TCNT0  10ms = 16MHz/1024/156(.25) periodic timer (100ms interval)
  //        cycles at 9.984ms but includes 1:4 duty cycle correction to /157 to average at 10.0ms
  TCCR2B  = (0b111 << CS20); // Norm mode, clk/1024 (differs from ATmega64 chip)
  OCR2A   = 156;
  TIMSK2 |= (1<<OCIE2A) |  (1<<TOIE2); // Enable Output-Compare and Overflow interrrupts

  // TIMER4 set into CTC mode, prescaler 16MHz/64=250 kHz 
  // Used for audio tone generation
  TCCR4B  = (1<<WGM42) | (0b011 << CS00);
  TCCR4A  = 0x00;
  
  #if defined (VOICE)     // OLD FROM GRUVIN9X, TO REWRITE
  // SOMO set-up
  OCR4A = 0x1F4; //2ms
  TCCR4B = (1 << WGM42) | (0b011 << CS40); // CTC OCR1A, 16MHz / 64 (4us ticks)
  TIMSK4 |= (1<<OCIE4A); // Start the interrupt so the unit reset can occur
  #endif

  /* Rotary encoder interrupt set-up                 */
  EIMSK = 0; // disable ALL external interrupts.
  // encoder 1
  EICRB = (1<<ISC50) | (1<<ISC40); // 01 = interrupt on any edge
  EIFR = (3<<INTF4); // clear the int. flag in case it got set when changing modes 
  // encoder 2
  EICRA = (1<<ISC30) | (1<<ISC20);
  EIFR = (3<<INTF2);
  EIMSK = (3<<INT4) | (3<<INT2); // enable the two rot. enc. ext. int. pairs.
}              
#endif // !SIMU

uint8_t pwrCheck()
{
#if !defined(SIMU)
  if ((~PINH & 0b00100000) && (~PINH & 0b01000000))
  return e_power_off;
#endif          
  return e_power_on;  
}      

void pwrOff()
{
  PORTH &= ~0x10;   // PortH-4 set to 0
}

FORCEINLINE uint8_t keyDown()
{
  return ((~PINL) & 0x3F) || ROTENC_DOWN();
}

bool switchState(EnumKeys enuk)
{
  uint8_t result = 0 ;

  if (enuk < (int)DIM(keys))
    return keys[enuk].state() ? 1 : 0;

  switch(enuk){
    case SW_ELE:
      result = !(PINL & (1<<INP_L_ElevDR));
      break;

    case SW_AIL:
      result = !(PIND & (1<<INP_D_AileDR));
      break;

    case SW_RUD:
      result = !(PING & (1<<INP_G_RuddDR));
      break;

    //       INP_C_ID1  INP_C_ID2
    // ID0      0          1
    // ID1      1          1
    // ID2      1          0
    case SW_ID0:
      result = !(PINC & (1<<INP_C_ID1));
      break;

    case SW_ID1:
      result = (PINC & (1<<INP_C_ID1))&& (PINC & (1<<INP_C_ID2));
      break;

    case SW_ID2:
      result = !(PINC & (1<<INP_C_ID2));
      break;

    case SW_GEA:
      result = !(PING & (1<<INP_G_Gear));
      break;

    case SW_THR:
      result = !(PING & (1<<INP_G_ThrCt));
      break;

    case SW_TRN:
      result = !(PINL & (1<<INP_L_Trainer));
      break;

    default:
      break;
  }

  return result;
}

// Trim switches
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
  uint8_t in = ~PINF;                //was PIND
  return (in & pgm_read_byte(crossTrim+idx));
}

FORCEINLINE void readKeysAndTrims()
{
  uint8_t enuk = KEY_MENU;

  keys[BTN_REa].input(~PINJ & 0x01);
  keys[BTN_REb].input(~PINJ & 0x02);

  uint8_t tin = ~PINL;
  uint8_t in;
  in = (tin & 0x0f) << 3;
  in |= (tin & 0x30) >> 3;

  for (int i=1; i<7; i++) {
    keys[enuk].input(in & (1<<i));
    ++enuk;
  }

  // Trims
  in = ~PINF;
  for (int i=0; i<8; i++) {
    // INP_D_TRM_RH_UP   0 .. INP_D_TRM_LH_UP   7
    keys[enuk].input(in & pgm_read_byte(crossTrim+i));
    ++enuk;
  }
}

// Rotary encoders increment/decrement (0 = rotary 1, 1 = rotary 2)
ISR(INT4_vect)     // Arduino2560 IO02 (portE pin4)    
{
  uint8_t input = (PINE & 0x30);
  if (input == 0 || input == 0x30) incRotaryEncoder(0, -1);
}
                 
ISR(INT5_vect)     // Arduino2560 IO03 (portE pin5)
{
  uint8_t input = (PINE & 0x30);
  if (input == 0 || input == 0x30) incRotaryEncoder(0, +1);
}

ISR(INT2_vect)     // Arduino2560 IO19 (portD pin2)
{
  uint8_t input = (PIND & 0x0C);
  if (input == 0 || input == 0x0C) incRotaryEncoder(1, -1);
}

ISR(INT3_vect)     // Arduino2560 IO18 (portD pin3)
{
  uint8_t input = (PIND & 0x0C);
  if (input == 0 || input == 0x0C) incRotaryEncoder(1, +1);
}  
       
