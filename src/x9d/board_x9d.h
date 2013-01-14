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

#ifndef board_x9d_h
#define board_x9d_h

#include <stdio.h>

#include "./STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_rcc.h"
#include "./STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_gpio.h"
#include "./STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_spi.h"
#include "./STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_rtc.h"
#include "./STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_pwr.h"
#include "./STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_usart.h"
#include "./STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include/stm32f2xx.h"

#include "hal.h"
#include "aspi.h"
#include "i2c_ee.h"
#include "audio_driver.h"

#define PERI1_FREQUENCY 30000000
#define PERI2_FREQUENCY 60000000
#define TIMER_MULT_APB1 2
#define TIMER_MULT_APB2 1

// TODO elsewhere
#if !defined(SIMU)
#define SD_IS_HC()                     (1)
#endif

void sdInit();
void sdPoll10ms();

void usbMassStorage();

void setVolume( register uint8_t volume );
#define JACK_PPM_OUT()
#define JACK_PPM_IN()

void configure_pins( uint32_t pins, uint16_t config );
uint16_t getCurrent();

extern uint8_t temperature ;              // Raw temp reading
extern uint8_t maxTemperature ;           // Raw temp reading
uint8_t getTemperature();

#define SD_GET_SIZE_MB()        (0)
#define SD_GET_BLOCKNR()        (0)
#define SD_GET_SPEED()          (0)

void soft_power_off();

#define strcpy_P strcpy
#define strcat_P strcat

extern uint32_t readKeys();
#define KEYS_PRESSED() (~readKeys())
#define DBLKEYS_PRESSED_RGT_LFT(i) ((in & (0x20 + 0x40)) == (0x20 + 0x40))
#define DBLKEYS_PRESSED_UP_DWN(i)  ((in & (0x10 + 0x08)) == (0x10 + 0x08))
#define DBLKEYS_PRESSED_RGT_UP(i)  ((in & (0x20 + 0x10)) == (0x20 + 0x10))
#define DBLKEYS_PRESSED_LFT_DWN(i) ((in & (0x40 + 0x08)) == (0x40 + 0x08))

extern uint16_t sessionTimer;

#define BOOTLOADER_REQUEST() (0/*usbPlugged()*/)

#define SLAVE_MODE() (0/*check_soft_power() == e_power_trainer*/)

#if !defined(SIMU)
#define wdt_disable()
#define wdt_enable(x)
#define wdt_reset()
#endif

uint32_t check_soft_power();

#define setBacklight(xx)
#define __BACKLIGHT_ON    GPIO_SetBits(GPIOB, GPIO_Pin_BL)
#define __BACKLIGHT_OFF   GPIO_ResetBits(GPIOB, GPIO_Pin_BL)
#define IS_BACKLIGHT_ON() GPIO_IsSet(GPIOB, GPIO_Pin_BL)

#if !defined(SIMU)
#define eeprom_read_block I2C_EE_BufferRead
#define eeWriteBlockCmp   I2C_EE_BufferWrite
#else
void eeWriteBlockCmp(const void *pointer_ram, uint16_t pointer_eeprom, size_t size);
#endif

void keysInit();
void pwrInit();
void eepromInit(); // TODO check it's not in another include
void delaysInit();
void adcInit();
void audioInit();
void debugInit();

void adcRead();
void init_trainer_ppm();
void init_trainer_capture();
extern volatile uint16_t Analog_values[];

#define DEBUG_UART_BAUDRATE 115200

void uartInit(void);
void uartSendChar(uint8_t c);

void delaysInit(void);
void delay_01us(uint16_t nb);

#endif
