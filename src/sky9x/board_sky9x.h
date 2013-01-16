/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
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

#ifndef board_sky9x_h
#define board_sky9x_h

#include <stdio.h>
#include "board.h"
#include "audio_driver.h"

extern "C" {
extern void init_SDcard();
extern void sdInit();
}

#if defined(REVA)
#define GPIO_BUTTON_MENU                PIOB->PIO_PDSR
#define GPIO_BUTTON_EXIT                PIOA->PIO_PDSR
#define GPIO_BUTTON_UP                  PIOC->PIO_PDSR
#define GPIO_BUTTON_DOWN                PIOC->PIO_PDSR
#define GPIO_BUTTON_RIGHT               PIOC->PIO_PDSR
#define GPIO_BUTTON_LEFT                PIOC->PIO_PDSR
#define PIN_BUTTON_MENU                 0x00000040
#define PIN_BUTTON_EXIT                 0x80000000
#define PIN_BUTTON_UP                   0x00000004
#define PIN_BUTTON_DOWN                 0x00000008
#define PIN_BUTTON_RIGHT                0x00000010
#define PIN_BUTTON_LEFT                 0x00000020
#else
#define GPIO_BUTTON_MENU                PIOB->PIO_PDSR
#define GPIO_BUTTON_EXIT                PIOC->PIO_PDSR
#define GPIO_BUTTON_UP                  PIOC->PIO_PDSR
#define GPIO_BUTTON_DOWN                PIOC->PIO_PDSR
#define GPIO_BUTTON_RIGHT               PIOC->PIO_PDSR
#define GPIO_BUTTON_LEFT                PIOC->PIO_PDSR
#define PIN_BUTTON_MENU                 0x00000020
#define PIN_BUTTON_EXIT                 0x01000000
#define PIN_BUTTON_UP                   0x00000002
#define PIN_BUTTON_DOWN                 0x00000020
#define PIN_BUTTON_RIGHT                0x00000010
#define PIN_BUTTON_LEFT                 0x00000008
#endif

#define sdPoll10ms()

void usbMassStorage();

#define PIN_ENABLE           0x001
#define PIN_PERIPHERAL       0x000
#define PIN_INPUT            0x002
#define PIN_OUTPUT           0x000
#define PIN_PULLUP           0x004
#define PIN_NO_PULLUP        0x000
#define PIN_PULLDOWN         0x008
#define PIN_NO_PULLDOWN      0x000
#define PIN_PERI_MASK_L      0x010
#define PIN_PERI_MASK_H      0x020
#define PIN_PER_A            0x000
#define PIN_PER_B            0x010
#define PIN_PER_C            0x020
#define PIN_PER_D            0x030
#define PIN_PORT_MASK        0x0C0
#define PIN_PORTA            0x000
#define PIN_PORTB            0x040
#define PIN_PORTC            0x080
#define PIN_LOW              0x000
#define PIN_HIGH             0x100

void configure_pins( uint32_t pins, uint16_t config );
uint16_t getCurrent();

extern uint8_t temperature ;              // Raw temp reading
extern uint8_t maxTemperature ;           // Raw temp reading
uint8_t getTemperature();

#define strcpy_P strcpy
#define strcat_P strcat

extern uint32_t readKeys();
#define KEYS_PRESSED() (~readKeys())
#define DBLKEYS_PRESSED_RGT_LFT(i) ((in & (0x20 + 0x40)) == (0x20 + 0x40))
#define DBLKEYS_PRESSED_UP_DWN(i)  ((in & (0x10 + 0x08)) == (0x10 + 0x08))
#define DBLKEYS_PRESSED_RGT_UP(i)  ((in & (0x20 + 0x10)) == (0x20 + 0x10))
#define DBLKEYS_PRESSED_LFT_DWN(i) ((in & (0x40 + 0x08)) == (0x40 + 0x08))

#if !defined(REVA)
extern uint16_t Current_analogue;
extern uint16_t Current_max;
extern uint32_t Current_accumulator;
extern uint32_t Current_used;
extern uint16_t sessionTimer;
#endif

#if defined(REVC)
#define BOOTLOADER_REQUEST() (usbPlugged())
#else
#define BOOTLOADER_REQUEST() (pwrCheck() == e_power_usb)
#endif

#define SLAVE_MODE() (pwrCheck() == e_power_trainer)
#define JACK_PPM_OUT() PIOC->PIO_PDR = PIO_PC22
#define JACK_PPM_IN() PIOC->PIO_PER = PIO_PC22

void setSticksGain(uint8_t gains);

// WDT driver
#if !defined(SIMU)
#define wdt_disable()
#define wdt_enable(x) WDT->WDT_MR = 0x3FFF217F;
#define wdt_reset()   WDT->WDT_CR = 0xA5000001
#endif

// Backlight driver
#define setBacklight(xx)  PWM->PWM_CH_NUM[0].PWM_CDTYUPD = xx
#define __BACKLIGHT_ON    (PWM->PWM_CH_NUM[0].PWM_CDTY = g_eeGeneral.backlightBright)
#define __BACKLIGHT_OFF   (PWM->PWM_CH_NUM[0].PWM_CDTY = 100)
#define IS_BACKLIGHT_ON() (PWM->PWM_CH_NUM[0].PWM_CDTY != 100)

// ADC driver
void adcInit();
void adcRead(void);
extern uint16_t Analog_values[];

// Buzzer driver
void buzzerSound(uint8_t duration);
void buzzerHeartbeat();
#define BUZZER_HEARTBEAT buzzerHeartbeat

// Coproc driver
void coprocInit();
void coprocWriteData(uint8_t *data, uint32_t size);
void coprocReadData(bool onlytemp=false);
void coprocCheck();
extern int8_t coprocVolumeRequired;
extern uint8_t coprocVolumeReadPending;
extern uint8_t Coproc_read;
extern int8_t Coproc_valid;
extern int8_t Coproc_temp;
extern int8_t Coproc_maxtemp;

// Haptic driver
void hapticOff(void) ;
void hapticOn(uint32_t pwmPercent);

// BlueTooth driver
#if defined(BLUETOOTH)
void btInit();
void btTask(void* pdata);
void btPushByte(uint8_t data);
#endif

// Power driver
void pwrInit();
void pwrOff();
uint32_t pwrCheck();

// EEPROM driver
void eepromInit();

// Rotary Encoder driver
void rotencInit();
void rotencEnd();

#endif
