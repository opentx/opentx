#include "../open9x.h"

#define PIN_MODE_MASK           0x0003
#define PIN_INPUT               0x0000
#define PIN_OUTPUT              0x0001
#define PIN_PERIPHERAL          0x0002
#define PIN_ANALOG              0x0003
#define PIN_PULL_MASK           0x000C
#define PIN_PULLUP              0x0004
#define PIN_NO_PULLUP           0x0000
#define PIN_PULLDOWN            0x0008
#define PIN_NO_PULLDOWN         0x0000
#define PIN_PERI_MASK           0x00F0
#define PIN_PUSHPULL            0x0000
#define PIN_ODRAIN              0x8000
#define PIN_PORT_MASK           0x0700
#define PIN_SPEED_MASK          0x6000


#if !defined(SIMU)
void configure_pins( uint32_t pins, uint16_t config )
{
  uint32_t address ;
  GPIO_TypeDef *pgpio ;
  uint32_t thispin ;
  uint32_t pos ;

  address = ( config & PIN_PORT_MASK ) >> 8 ;
  address *= (GPIOB_BASE-GPIOA_BASE) ;
  address += GPIOA_BASE ;
  pgpio = (GPIO_TypeDef* ) address ;

  /* -------------------------Configure the port pins---------------- */
  /*-- GPIO Mode Configuration --*/
  for (thispin = 0x0001, pos = 0; thispin < 0x10000; thispin <<= 1, pos +=1 )
  {
    if ( pins & thispin)
    {
      pgpio->MODER  &= ~(GPIO_MODER_MODER0 << (pos * 2)) ;
      pgpio->MODER |= (config & PIN_MODE_MASK) << (pos * 2) ;

      if ( ( (config & PIN_MODE_MASK ) == PIN_OUTPUT) || ( (config & PIN_MODE_MASK) == PIN_PERIPHERAL) )
      {
        /* Speed mode configuration */
        pgpio->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR0 << (pos * 2) ;
        pgpio->OSPEEDR |= ((config & PIN_SPEED_MASK) >> 13 ) << (pos * 2) ;

        /* Output mode configuration*/
        pgpio->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)pos)) ;
        if ( config & PIN_ODRAIN )
        {
          pgpio->OTYPER |= (GPIO_OTYPER_OT_0) << pos ;
        }
      }
      /* Pull-up Pull down resistor configuration*/
      pgpio->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)pos * 2));
      pgpio->PUPDR |= ((config & PIN_PULL_MASK) >> 2) << (pos * 2) ;

      pgpio->AFR[pos >> 3] &= ~(0x000F << (pos & 7)) ;
      pgpio->AFR[pos >> 3] |= ((config & PIN_PERI_MASK) >> 4) << (pos & 7) ;
    }
  }
}
#endif




#ifndef SIMU
inline void boardInit()
{
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

  /***************************************************/
  /* Rotary encoder interrupt set-up */

  // All external interrupts initialise to disabled.
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

#if defined (VOICE)
  /*
   * SOMO set-up (V4 board only)
   */
  OCR4A = 0x1F4; //2ms
  TCCR4B = (1 << WGM42) | (3<<CS40); // CTC OCR1A, 16MHz / 64 (4us ticks)
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
#endif
#endif // !SIMU

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

#if ROTARY_ENCODERS <= 2
#define ROTENC_DOWN() ((~PIND & 0x20) || (~PIND & 0x10))
#else
#define ROTENC_DOWN() (0)
#endif

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

#if 0
    case SW_ID3:
      result = (calibratedStick[POT1+EXTRA_3POS-1] < 0);
      break;

    case SW_ID4:
      result = (calibratedStick[POT1+EXTRA_3POS-1] == 0);
      break;

    case SW_ID5:
      result = (calibratedStick[POT1+EXTRA_3POS-1] > 0);
      break;
#endif

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

FORCEINLINE void readKeysAndTrims()
{
  uint8_t enuk = KEY_MENU;

#if ROTARY_ENCODERS <= 2
  keys[BTN_REa].input(~PIND & 0x20, BTN_REa);
#endif
  keys[BTN_REb].input(~PIND & 0x10, BTN_REb);

  uint8_t tin = ~PINL;
  uint8_t in;
  in = (tin & 0x0f) << 3;
  in |= (tin & 0x30) >> 3;

  for (int i=1; i<7; i++)
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

#if ROTARY_ENCODERS <= 2
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
#endif

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
