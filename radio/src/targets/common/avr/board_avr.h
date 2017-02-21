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

#if defined(SIMU)
extern volatile unsigned char pina, pinb, pinc, pind, pine, pinf, ping, pinh, pinj, pinl;
#define PINA  ~pina
#define PINB  ~pinb
#define PINC  ~pinc
#define PIND  ~pind
#define PINE  ~pine
#define PINF  ~pinf
#define PING  ~ping
#define PINH  ~pinh
#define PINJ  ~pinj
#define PINL  ~pinl
#else
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "pgmtypes.h"
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#define F_CPU                        16000000UL  // 16 MHz
#include <util/delay.h>
#define pgm_read_adr(address_short)  pgm_read_word(address_short)
#include <avr/wdt.h>
#endif

// ADC driver
#define NUM_POTS                       3
#define NUM_SLIDERS                    0
#define NUM_XPOTS                      0

enum Analogs {
  STICK1,
  STICK2,
  STICK3,
  STICK4,
  POT_FIRST,
  POT1 = POT_FIRST,
  POT2,
  POT3,
  POT_LAST = POT3,
#if defined(TELEMETRY_MOD_14051) || defined(TELEMETRY_MOD_14051_SWAPPED)
  // When the mod is applied, ADC7 is connected to 14051's X pin and TX_VOLTAGE
  // is connected to 14051's X0 pin (one of the multiplexed inputs). TX_VOLTAGE
  // value is filled in by processMultiplexAna().

  // This shifts TX_VOLTAGE from 7 to 8 and makes X14051 take the 7th position
  // corresponding to ADC7.
  X14051,
#endif
  TX_VOLTAGE,
  NUM_ANALOGS
};

enum CalibratedAnalogs {
  CALIBRATED_STICK1,
  CALIBRATED_STICK2,
  CALIBRATED_STICK3,
  CALIBRATED_STICK4,
  CALIBRATED_POT_FIRST,
  CALIBRATED_POT1 = CALIBRATED_POT_FIRST,
  CALIBRATED_POT2,
  CALIBRATED_POT3,
  CALIBRATED_POT_LAST = CALIBRATED_POT3,
  NUM_CALIBRATED_ANALOGS
};

void adcInit();
void adcPrepareBandgap();
void getADC();
void getADC_bandgap();

// Keys driver
#define NUM_SWITCHES                   7
enum EnumKeys
{
  KEY_MENU,
  KEY_ENTER=KEY_MENU,
  KEY_EXIT,
  KEY_DOWN,
  KEY_MINUS = KEY_DOWN,
  KEY_UP,
  KEY_PLUS = KEY_UP,
  KEY_RIGHT,
  KEY_LEFT,
  
  TRM_BASE,
  TRM_LH_DWN = TRM_BASE,
  TRM_LH_UP,
  TRM_LV_DWN,
  TRM_LV_UP,
  TRM_RV_DWN,
  TRM_RV_UP,
  TRM_RH_DWN,
  TRM_RH_UP,
  TRM_LAST = TRM_RH_UP,

#if ROTARY_ENCODERS > 0 || defined(ROTARY_ENCODER_NAVIGATION)
  BTN_REa,
#endif
#if ROTARY_ENCODERS > 0
  BTN_REb,
#endif
  
  NUM_KEYS
};

enum EnumSwitches
{
  SW_ID0,
  SW_ID1,
  SW_ID2,
  SW_THR,
  SW_RUD,
  SW_ELE,
  SW_AIL,
  SW_GEA,
  SW_TRN,
};
#define IS_3POS(sw)                    ((sw) == 0)
#define IS_TOGGLE(sw)                  ((sw) == SWSRC_TRN)
uint8_t keyState(uint8_t index);
uint8_t switchState(uint8_t index);

// LCD driver
#define LCD_W                          128
#define LCD_H                          64
#define LCD_DEPTH                      1
#define LCD_CONTRAST_MIN               10
#define LCD_CONTRAST_MAX               45
#define LCD_CONTRAST_DEFAULT           25
void lcdRefresh(void);
#define lcdRefreshWait()
void lcdSetRefVolt(unsigned char val);
void lcdSetContrast(void);
void lcdInit(void);
#define lcdOff()

#if defined(LCD_KS108)
void lcdRefreshSide();
#endif

#if defined(LCD_ST7920)
uint8_t	lcdRefresh_ST7920(uint8_t full);
#endif

// EEPROM driver
#if defined(CPUM2560) || defined(CPUM2561) || defined(CPUM128)
  #define EEPROM_SIZE                  4096
#else
  #define EEPROM_SIZE                  2048
#endif
#if defined(SIMU)
void eepromReadBlock(uint8_t * buffer, size_t address, size_t size);
#else
#define eepromReadBlock(buffer, address, size)  eeprom_read_block(buffer, (const void *)address, size)
#endif
uint8_t eepromIsTransferComplete();

// USART driver (static register dispatcher)
#define RXD_DDR1 DDRD
#define RXD_DDR_PIN1 DDD2
#define RXD_PORT1 PORTD
#define RXD_PORT_PIN1 PORTD2
#define RXD_DDR0 DDRE
#define RXD_DDR_PIN0 DDE0
#define RXD_PORT0 PORTE
#define RXD_PORT_PIN0 PORTE0

#define _DOR_N(usart_no) DOR ## usart_no
#define _FE_N(usart_no) FE ## usart_no
#define _RXCIE_N(usart_no) RXCIE ## usart_no
#define _RXC_N(usart_no) RXC ## usart_no
#define _RXD_DDR_N(usart_no) RXD_DDR ## usart_no
#define _RXD_DDR_PIN_N(usart_no) RXD_DDR ## usart_no
#define _RXD_PORT_N(usart_no) RXD_DDR ## usart_no
#define _RXD_PORT_PIN_N(usart_no) RXD_DDR ## usart_no
#define _RXEN_N(usart_no) RXEN ## usart_no
#define _TXCIE_N(usart_no) TXCIE ## usart_no
#define _TXEN_N(usart_no) TXEN ## usart_no
#define _U2X_N(usart_no) U2X ## usart_no
#define _UBRRH_N(usart_no) UBRR ## usart_no ## H
#define _UBRRL_N(usart_no) UBRR ## usart_no ## L
#define _UCSRA_N(usart_no) UCSR ## usart_no ## A
#define _UCSRB_N(usart_no) UCSR ## usart_no ## B
#define _UCSRC_N(usart_no) UCSR ## usart_no ## C
#define _UCSZ0_N(usart_no) UCSZ ## usart_no ## 0
#define _UCSZ1_N(usart_no) UCSZ ## usart_no ## 1
#define _UCSZ2_N(usart_no) UCSZ ## usart_no ## 2
#define _UDRIE_N(usart_no) UDRIE ## usart_no
#define _UDR_N(usart_no) UDR ## usart_no
#define _UPE_N(usart_no) UPE ## usart_no
#define _USART_RX_vect_N(usart_no) USART ## usart_no ## _RX_vect
#define _USART_UDRE_vect_N(usart_no) USART ## usart_no ## _UDRE_vect

#define DOR_N(usart_no) _DOR_N(usart_no)
#define FE_N(usart_no) _FE_N(usart_no)
#define RXCIE_N(usart_no) _RXCIE_N(usart_no)
#define RXC_N(usart_no) _RXC_N(usart_no)
#define RXD_DDR_N(usart_no) _RXD_DDR_N(usart_no)
#define RXD_DDR_PIN_N(usart_no) _RXD_DDR_PIN_N(usart_no)
#define RXD_PORT_N(usart_no) _RXD_PORT_N(usart_no)
#define RXD_PORT_PIN_N(usart_no) _RXD_PORT_PIN_N(usart_no)
#define RXEN_N(usart_no) _RXEN_N(usart_no)
#define TXCIE_N(usart_no) _TXCIE_N(usart_no)
#define TXEN_N(usart_no) _TXEN_N(usart_no)
#define U2X_N(usart_no) _U2X_N(usart_no)
#define UBRRH_N(usart_no) _UBRRH_N(usart_no)
#define UBRRL_N(usart_no) _UBRRL_N(usart_no)
#define UCSRA_N(usart_no) _UCSRA_N(usart_no)
#define UCSRB_N(usart_no) _UCSRB_N(usart_no)
#define UCSRC_N(usart_no) _UCSRC_N(usart_no)
#define UCSZ0_N(usart_no) _UCSZ0_N(usart_no)
#define UCSZ1_N(usart_no) _UCSZ1_N(usart_no)
#define UCSZ2_N(usart_no) _UCSZ2_N(usart_no)
#define UDRIE_N(usart_no) _UDRIE_N(usart_no)
#define UDR_N(usart_no) _UDR_N(usart_no)
#define UPE_N(usart_no) _UPE_N(usart_no)
#define USART_RX_vect_N(usart_no) _USART_RX_vect_N(usart_no)
#define USART_UDRE_vect_N(usart_no) _USART_UDRE_vect_N(usart_no)

// Telemetry driver
#if defined(TELEMETRY_MOD_14051_SWAPPED)
#define TLM_USART 1
#else
#define TLM_USART 0
#endif
void telemetryPortInit();
void telemetryPortInit(uint8_t baudrate);
void telemetryTransmitBuffer();
