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

#if defined(ROTARY_ENCODER_NAVIGATION)

#if defined(TELEMETREZ)
uint8_t TrotCount;             // TeZ version
uint8_t LastTrotCount;         // TeZ version
uint8_t RotEncoder;
#else
uint8_t RotPosition;
int8_t LastRotaryValue;
int8_t Rotary_diff;
int8_t RotaryControl;
uint8_t RotEncoder;
#endif

void rotencPoll()
{
#if defined(TELEMETREZ)
  if (TrotCount != LastTrotCount) {
    rotencValue[0] = LastTrotCount = TrotCount;
  }
#else
  // Rotary Encoder polling
  PORTA = 0;                     // No pullups
  DDRA = 0x1F;           // Top 3 bits input

  asm(" rjmp 1f");
  asm("1:");

  uint8_t rotary;
  rotary = PINA;
  DDRA = 0xFF;           // Back to all outputs
  rotary &= 0xE0;

  RotEncoder = rotary; // just read the lcd pin

  rotary &= 0xDF;
  if ( rotary != RotPosition ) {
    uint8_t x;
    x = RotPosition & 0x40;
    x <<= 1;
    x ^= rotary & 0x80;
    if ( x ) {
      rotencValue[0] -= 1;
    }
    else {
      rotencValue[0] += 1;
    }
    RotPosition = rotary;
  }
#endif // TELEMETREZ
}
#else
#define ROTENC_DOWN() (0)
#endif // ROTARY_ENCODER_NAVIGATION

void boardInit()
{
#if !defined(SIMU)
  // Set up I/O port data directions and initial states
  DDRA = 0xff;  PORTA = 0x00; // LCD data
  DDRB = 0x81;  PORTB = 0x7e; //pullups keys+nc
#if defined(TELEMETRY_MOD_14051) || defined(TELEMETRY_MOD_14051_SWAPPED)
  DDRC = 0xff;  PORTC = 0x00;
#else
  DDRC = 0x3e;  PORTC = 0xc1; //pullups nc
#endif
  DDRD = 0x00;  PORTD = 0xff; //pullups keys
  DDRE = (1<<OUT_E_BUZZER); PORTE = 0xff-(1<<OUT_E_BUZZER); //pullups + buzzer 0
  DDRF = 0x00;  PORTF = 0x00; //anain
  DDRG = 0x14;  PORTG = 0xfb; //pullups + SIM_CTL=1 = phonejack = ppm_in, Haptic output and off (0)

  adcInit();

#if defined(CPUM2561)
  TCCR2B  = (0b111 << CS20); // Norm mode, clk/1024 (differs from ATmega64 chip)
  OCR2A   = 156;
  TIMSK2 |= (1<<OCIE2A) |  (1<<TOIE2); // Enable Output-Compare and Overflow interrrupts
#else
  // TCNT0  10ms = 16MHz/1024/156 periodic timer (9.984ms)
  // (with 1:4 duty at 157 to average 10.0ms)
  // Timer overflows at about 61Hz or once every 16ms.
  TCCR0  = (0b111 << CS00); // Norm mode, clk/1024
  OCR0   = 156;
#endif

#if defined(AUDIO) || defined(VOICE)
  SET_TIMER_AUDIO_CTRL();
  #if defined(CPUM2561)
    OCR4A = 0xFF;
    RESUME_AUDIO_INTERRUPT();
  #else
    TIMSK |= (1<<OCIE0) | (1<<TOIE0) | (1<<TOIE2); // Enable Output-Compare and Overflow interrrupts
  #endif
#elif defined(PWM_BACKLIGHT)
  /** Smartieparts LED Backlight is connected to PORTB/pin7, which can be used as pwm output of timer2 **/
  #if defined(CPUM2561)
    #if defined(SP22)
      TCCR0A = (1<<WGM00)|(1<<COM0A1)|(1<<COM0A0); // inv. pwm mode, clk/64
    #else
      TCCR0A = (1<<WGM00)|(1<<COM0A1); // pwm mode, clk/64
    #endif
    TCCR0B = (0b011<<CS00);
  #else
    #if defined(SP22)
      TCCR2  = (0b011<<CS20)|(1<<WGM20)|(1<<COM21)|(1<<COM20); // inv. pwm mode, clk/64
    #else
      TCCR2  = (0b011<<CS20)|(1<<WGM20)|(1<<COM21); // pwm mode, clk/64
    #endif
    TIMSK |= (1<<OCIE0) | (1<<TOIE0); // Enable Output-Compare and Overflow interrrupts
  #endif
#else
  #if !defined(CPUM2561)
    TIMSK |= (1<<OCIE0) | (1<<TOIE0); // Enable Output-Compare and Overflow interrrupts
  #endif
#endif
#endif // !defined(SIMU)
}

#if !defined(SIMU) && (defined(TELEMETRY_MOD_14051) || defined(TELEMETRY_MOD_14051_SWAPPED))
uint8_t pf7_digital[MUX_PF7_DIGITAL_MAX - MUX_PF7_DIGITAL_MIN + 1];
/**
 * Update ADC PF7 using 14051 multiplexer
 * X0 : Battery voltage
 * X1 : AIL SW
 * X2 : THR SW
 * X3 : TRIM LEFT VERTICAL UP
 * X4 : TRIM LEFT VERTICAL DOWN
 */
void processMultiplexAna()
{
  static uint8_t muxNum = MUX_BATT;
  uint8_t nextMuxNum = muxNum-1;

  switch (muxNum) {
    case MUX_BATT:
      s_anaFilt[TX_VOLTAGE] = s_anaFilt[X14051];
      nextMuxNum = MUX_MAX;
      break;
    case MUX_AIL:
    case MUX_THR:
    case MUX_TRM_LV_UP:
    case MUX_TRM_LV_DWN:
      // Digital switch depend from input voltage
      // take half voltage to determine digital state
      pf7_digital[muxNum - MUX_PF7_DIGITAL_MIN] = (s_anaFilt[X14051] >= (s_anaFilt[TX_VOLTAGE] / 2)) ? 1 : 0;
      break;
  }

  // set the mux number for the next ADC convert,
  // stabilize voltage before ADC read.
  muxNum = nextMuxNum;
  PORTC &= ~((1 << PC7) | (1 << PC6) | (1 << PC0));
  if(muxNum & 1) PORTC |= (1 << PC7); // Mux CTRL A
  if(muxNum & 2) PORTC |= (1 << PC6); // Mux CTRL B
  if(muxNum & 4) PORTC |= (1 << PC0); // Mux CTRL C
}
#endif

#if !defined(SIMU) && (defined(TELEMETRY_MOD_14051) || defined(TELEMETRY_MOD_14051_SWAPPED))
  #define THR_STATE()   pf7_digital[PF7_THR]
  #define AIL_STATE()   pf7_digital[PF7_AIL]
#elif defined(TELEMETRY_JETI) || defined(TELEMETRY_FRSKY) || defined(TELEMETRY_ARDUPILOT) || defined(TELEMETRY_NMEA) || defined(TELEMETRY_MAVLINK)
  #define THR_STATE()   (PINC & (1<<INP_C_ThrCt))
  #define AIL_STATE()   (PINC & (1<<INP_C_AileDR))
#else
  #define THR_STATE()   (PINE & (1<<INP_E_ThrCt))
  #define AIL_STATE()   (PINE & (1<<INP_E_AileDR))
#endif

uint8_t keyState(uint8_t index)
{
  return keys[index].state();
}

uint8_t switchState(uint8_t index)
{
  uint8_t result = 0;

  switch (index) {
    case SW_ELE:
      result = PINE & (1<<INP_E_ElevDR);
      break;

    case SW_AIL:
      result = AIL_STATE();
      break;

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

    case SW_THR:
      result = THR_STATE();
      break;

    case SW_TRN:
      result = PINE & (1<<INP_E_Trainer);
      break;

    default:
      break;
  }

  return result;
}

// Trim switches ...
uint8_t trimHelper(uint8_t negpin, uint8_t idx)
{
  switch(idx){
    case 0: return negpin & TRIMS_GPIO_PIN_LHL;
    case 1: return negpin & TRIMS_GPIO_PIN_LHR;
#if !defined(SIMU) && defined(TELEMETRY_MOD_14051_SWAPPED)
    case 2: return !pf7_digital[PF7_TRM_LV_DWN];
    case 3: return !pf7_digital[PF7_TRM_LV_UP];
#else
    case 2: return negpin & TRIMS_GPIO_PIN_LVD;
    case 3: return negpin & TRIMS_GPIO_PIN_LVU;
#endif
    case 4: return negpin & TRIMS_GPIO_PIN_RVD;
    case 5: return negpin & TRIMS_GPIO_PIN_RVU;
    case 6: return negpin & TRIMS_GPIO_PIN_RHL;
    case 7: return negpin & TRIMS_GPIO_PIN_RHR;
    default: return 0;
  }
}

uint8_t trimDown(uint8_t idx)
{
  return trimHelper(~PIND, idx);
}

void readKeysAndTrims()
{
  uint8_t index = KEY_MENU;

  // User buttons ...
  uint8_t in = ~PINB;
  for (int i=1; i<7; i++) {
    keys[index].input(in & (1<<i));
    ++index;
  }

  // Trims ...
  in = ~PIND;
  for (int i=0; i<8; i++) {
    // INP_D_TRM_RH_UP   0 .. INP_D_TRM_LH_UP   7
    keys[index].input(trimHelper(in, i));
    ++index;
  }

#if defined(ROTARY_ENCODER_NAVIGATION)
  keys[index].input(ROTENC_DOWN()); // Rotary Enc. Switch
#endif

#if defined(NAVIGATION_STICKS)
  if (~PINB & 0x7E) {
    StickScrollTimer = STICK_SCROLL_TIMEOUT;
  }
#endif
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

#if defined(PWM_BACKLIGHT)

// exponential PWM table for linear brightness
static const uint8_t pwmtable[16] PROGMEM =
{
    0, 2, 3, 4, 6, 8, 11, 16, 23, 32, 45, 64, 90, 128, 181, 255
};

static uint8_t bl_target;
static uint8_t bl_current;

void backlightEnable()
{
  bl_target = 15 - g_eeGeneral.blOnBright;
}

void backlightDisable()
{
  bl_target = g_eeGeneral.blOffBright;
}

bool isBacklightEnabled()
{
  return (bl_target==g_eeGeneral.blOnBright);
}

void backlightFade() // called from per10ms()
{
  if (bl_target != bl_current) {
#if defined(CPUM2561)
    if (bl_target > bl_current)
      OCR0A = pgm_read_byte(&pwmtable[++bl_current]);
    else
      OCR0A = pgm_read_byte(&pwmtable[--bl_current]);
#else
    if (bl_target > bl_current)
      OCR2 = pgm_read_byte(&pwmtable[++bl_current]);
    else
      OCR2 = pgm_read_byte(&pwmtable[--bl_current]);
#endif
  }
}

#endif
