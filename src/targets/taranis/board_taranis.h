/*
 * Authors (alphabetical order)
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

#ifndef board_taranis_h
#define board_taranis_h

#include <stdio.h>

#include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_rcc.h"
#include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_gpio.h"
#include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_spi.h"
#include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_rtc.h"
#include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_pwr.h"
#include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_dma.h"
#include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_usart.h"
#include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include/stm32f2xx.h"
extern "C" {
#include "STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/inc/usbd_msc_core.h"
#include "STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/inc/usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "usbd_conf.h"
}
#include "hal.h"
#include "aspi.h"
#include "i2c.h"
#include "audio_driver.h"
#include "a7105_driver.h"

#define PERI1_FREQUENCY 30000000
#define PERI2_FREQUENCY 60000000
#define TIMER_MULT_APB1 2
#define TIMER_MULT_APB2 1

#define JACK_PPM_OUT()
#define JACK_PPM_IN()

void configure_pins( uint32_t pins, uint16_t config );
uint16_t getCurrent();

extern uint8_t temperature ;              // Raw temp reading
extern uint8_t maxTemperature ;           // Raw temp reading
uint8_t getTemperature();

#define strcpy_P strcpy
#define strcat_P strcat

extern uint16_t sessionTimer;

/* USB driver */
uint8_t usbPlugged(void);
#define BOOTLOADER_REQUEST() (0)
#define usbBootloader()

#define SLAVE_MODE()         (g_model.trainerMode == 1)
#define TRAINER_CONNECTED()  (GPIO_ReadInputDataBit(GPIOTRNDET, PIN_TRNDET) == Bit_RESET)

void delaysInit();

#define DEBUG_UART_BAUDRATE 115200
#define SPORT_BAUDRATE 57600

void uartInit(uint32_t baudrate);
void uartPutc(const char c);

void sportInit(void);

void delaysInit(void);
void delay_01us(uint16_t nb);

// SD driver
#if !defined(SIMU)
#define SD_IS_HC()              (1)
#define SD_GET_SIZE_MB()        (0)
#define SD_GET_BLOCKNR()        (0)
#define SD_GET_SPEED()          (0)
void sdInit();
void sdPoll10ms();
#define sdMountPoll()
uint32_t sdMounted();
#define SD_CARD_PRESENT()       (~SD_PRESENT_GPIO->IDR & SD_PRESENT_GPIO_Pin)
#endif

// Pulses driver
void init_ppm( uint32_t module_index );
void disable_ppm( uint32_t module_index );
void init_pxx( uint32_t module_index );
void disable_pxx( uint32_t module_index );

// Trainer driver
void init_trainer_ppm();
void stop_trainer_ppm();
void init_trainer_capture();
void stop_trainer_capture();

// Keys driver
void keysInit();
uint32_t readKeys();
#define KEYS_PRESSED() (~readKeys())
#define DBLKEYS_PRESSED_RGT_LFT(i) ((in & ((2<<KEY_PLUS) + (2<<KEY_MINUS))) == ((2<<KEY_PLUS) + (2<<KEY_MINUS)))
#define DBLKEYS_PRESSED_UP_DWN(i)  ((in & ((2<<KEY_MENU) + (2<<KEY_PAGE))) == ((2<<KEY_MENU) + (2<<KEY_PAGE)))
#define DBLKEYS_PRESSED_RGT_UP(i)  ((in & ((2<<KEY_ENTER) + (2<<KEY_MINUS))) == ((2<<KEY_ENTER) + (2<<KEY_MINUS)))
#define DBLKEYS_PRESSED_LFT_DWN(i) ((in & ((2<<KEY_PAGE) + (2<<KEY_EXIT))) == ((2<<KEY_PAGE) + (2<<KEY_EXIT)))

// WDT driver
#if !defined(SIMU)
#define wdt_disable()
void watchdogInit();
#define wdt_enable(x)   watchdogInit()
#define wdt_reset()     IWDG->KR = 0xAAAA
#endif

// ADC driver
void adcInit();
void adcRead();
extern volatile uint16_t Analog_values[];
#if defined(REV3)
  #define BATT_SCALE    120
#else
  #define BATT_SCALE    150
#endif

// Power driver
void pwrInit();
uint32_t pwrCheck();
void pwrOff();
#define UNEXPECTED_SHUTDOWN() (g_eeGeneral.unexpectedShutdown)
#define INTERNAL_RF_ON()      GPIO_SetBits(GPIOPWR, PIN_INT_RF_PWR)
#define INTERNAL_RF_OFF()     GPIO_ResetBits(GPIOPWR, PIN_INT_RF_PWR)
#define EXTERNAL_RF_ON()      GPIO_SetBits(GPIOPWR, PIN_EXT_RF_PWR)
#define EXTERNAL_RF_OFF()     GPIO_ResetBits(GPIOPWR, PIN_EXT_RF_PWR)

// Backlight driver
#define setBacklight(xx)
#define __BACKLIGHT_ON        GPIO_SetBits(GPIOB, GPIO_Pin_BL)
#define __BACKLIGHT_OFF       GPIO_ResetBits(GPIOB, GPIO_Pin_BL)
#define IS_BACKLIGHT_ON()     GPIO_IsSet(GPIOB, GPIO_Pin_BL)

// USB driver
void usbInit(void);
extern uint8_t usb_connected;

// EEPROM driver
#if !defined(SIMU)
#define eepromInit()          I2C_EE_Init()
#define eeprom_read_block     I2C_EE_BufferRead
#define eeWriteBlockCmp       I2C_EE_BufferWrite
#else
#define eepromInit()
void eeWriteBlockCmp(const void *pointer_ram, uint16_t pointer_eeprom, size_t size);
#endif
#define EEPROM_MASSSTORAGE()  (usb_connected)

extern uint8_t currentTrainerMode;

#endif
