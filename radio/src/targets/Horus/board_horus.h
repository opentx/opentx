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

#ifndef _BOARD_FLAMENCO_H_
#define _BOARD_FLAMENCO_H_

#include "stddef.h"

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rcc.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_gpio.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_spi.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_i2c.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rtc.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_pwr.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dma.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_usart.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_flash.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_sdio.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dbgmcu.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_ltdc.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_fmc.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_tim.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dma2d.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/misc.h"

#if !defined(SIMU)
  #include "usbd_cdc_core.h"
  #include "usbd_msc_core.h"
  #include "usbd_hid_core.h"
  #include "usbd_usr.h"
  #include "usbd_desc.h"
  #include "usb_conf.h"
  #include "usbd_conf.h"
#endif

#include "hal.h"

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

#define FLASHSIZE          0x80000
#define BOOTLOADER_SIZE    0x8000
#define FIRMWARE_ADDRESS   0x08000000

#define PERI1_FREQUENCY    45000000
#define PERI2_FREQUENCY    90000000
#define TIMER_MULT_APB1    4
#define TIMER_MULT_APB2    2

#define PIN_MODE_MASK      0x0003
#define PIN_INPUT          0x0000
#define PIN_OUTPUT         0x0001
#define PIN_PERIPHERAL     0x0002
#define PIN_ANALOG         0x0003
#define PIN_PULL_MASK      0x000C
#define PIN_PULLUP         0x0004
#define PIN_NO_PULLUP      0x0000
#define PIN_PULLDOWN       0x0008
#define PIN_NO_PULLDOWN    0x0000
#define PIN_PERI_MASK      0x00F0
#define PIN_PUSHPULL       0x0000
#define PIN_ODRAIN         0x8000
#define PIN_PORT_MASK      0x0700
#define PIN_SPEED_MASK     0x6000
#define PIN_PORTA                       0x0000
#define PIN_PORTB                       0x0100
#define PIN_PORTC                       0x0200
#define PIN_PORTD                       0x0300
#define PIN_PORTE                       0x0400
#define PIN_PORTF                       0x0500
#define PIN_PER_1                       0x0010
#define PIN_PER_2                       0x0020
#define PIN_PER_3                       0x0030
#define PIN_PER_5                       0x0050
#define PIN_PER_6                       0x0060
#define PIN_PER_8                       0x0080
#define PIN_OS25                        0x2000
#define PIN_OS50                        0x4000
#define PIN_OS100                       0x6000

void configure_pins( uint32_t pins, uint16_t config );

#define strcpy_P strcpy
#define strcat_P strcat

extern uint16_t sessionTimer;

#define SLAVE_MODE()         (g_model.trainerMode == TRAINER_MODE_SLAVE)

#ifdef __cplusplus
extern "C" {
#endif
void delaysInit(void);
void delay_01us(uint16_t nb);
#ifdef __cplusplus
}
#endif

// SD driver
#if !defined(SIMU) || defined(SIMU_DISKIO)
  uint32_t sdIsHC(void);
  uint32_t sdGetSpeed(void);
  #define SD_IS_HC()              (sdIsHC())
  #define SD_GET_SPEED()          (sdGetSpeed())
  #define SD_GET_FREE_BLOCKNR()   (sdGetFreeSectors())
#else
  #define SD_IS_HC()              (0)
  #define SD_GET_SPEED()          (0)
#endif

#if defined(SIMU)
  #define sdInit()
  #define sdDone()
#else
  void sdInit(void);
  void sdDone(void);
  #define sdPoll10ms()
  #define sdMountPoll()
  uint32_t sdMounted(void);
  #define SD_CARD_PRESENT()       (~SD_PRESENT_GPIO->IDR & SD_PRESENT_GPIO_PIN)
#endif

// Flash Write driver
#define FLASH_PAGESIZE 256
void unlockFlash(void);
void lockFlash(void);
void writeFlash(uint32_t * address, uint32_t * buffer);
uint32_t isFirmwareStart(const void * buffer);
uint32_t isBootloaderStart(const void * buffer);

// SDRAM driver
#define SDRAM_BANK_ADDR     ((uint32_t)0xD0000000)
void SDRAM_Init(void);
void SDRAM_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize);
void SDRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize);

// Pulses driver
#define EXTERNAL_MODULE_ON()      GPIO_SetBits(EXTMODULE_GPIO_PWR, EXTMODULE_GPIO_PIN_PWR)
#define EXTERNAL_MODULE_OFF()     GPIO_ResetBits(EXTMODULE_GPIO_PWR, EXTMODULE_GPIO_PIN_PWR)
void init_no_pulses(uint32_t port);
void disable_no_pulses(uint32_t port);
void init_ppm( uint32_t module_index );
void disable_ppm( uint32_t module_index );
void set_external_ppm_parameters(uint32_t idleTime, uint32_t delay, uint32_t positive);
void init_pxx( uint32_t module_index );
void disable_pxx( uint32_t module_index );
void init_dsm2( uint32_t module_index );
void disable_dsm2( uint32_t module_index );

// Trainer driver
void init_trainer_ppm(void);
void stop_trainer_ppm(void);
void init_trainer_capture(void);
void stop_trainer_capture(void);
void init_cppm_on_heartbeat_capture(void);
void stop_cppm_on_heartbeat_capture(void);
void init_sbus_on_heartbeat_capture(void);
void stop_sbus_on_heartbeat_capture(void);
void set_trainer_ppm_parameters(uint32_t idleTime, uint32_t delay, uint32_t positive);

// Keys driver
void keysInit(void);
uint32_t readKeys(void);
uint32_t readTrims(void);
#define TRIMS_PRESSED() (readTrims())
#define KEYS_PRESSED()  (readKeys())
#define DBLKEYS_PRESSED_RGT_LFT(i) ((in & ((2<<KEY_PLUS) + (2<<KEY_MINUS))) == ((2<<KEY_PLUS) + (2<<KEY_MINUS)))
#define DBLKEYS_PRESSED_UP_DWN(i)  ((in & ((2<<KEY_MENU) + (2<<KEY_PAGE))) == ((2<<KEY_MENU) + (2<<KEY_PAGE)))
#define DBLKEYS_PRESSED_RGT_UP(i)  ((in & ((2<<KEY_ENTER) + (2<<KEY_MINUS))) == ((2<<KEY_ENTER) + (2<<KEY_MINUS)))
#define DBLKEYS_PRESSED_LFT_DWN(i) ((in & ((2<<KEY_PAGE) + (2<<KEY_EXIT))) == ((2<<KEY_PAGE) + (2<<KEY_EXIT)))
void checkRotaryEncoder(void);

// WDT driver
#if !defined(SIMU)
#define wdt_disable()
void watchdogInit(unsigned int duration);
#define wdt_enable(x) //   watchdogInit(1500)
#define wdt_reset()   //  IWDG->KR = 0xAAAA
#define WAS_RESET_BY_WATCHDOG()   (RCC->CSR & (RCC_CSR_WDGRSTF | RCC_CSR_WWDGRSTF))
#define WAS_RESET_BY_SOFTWARE()   (RCC->CSR & RCC_CSR_SFTRSTF)
#endif

// ADC driver
void adcInit(void);
void adcRead(void);
inline uint16_t getAnalogValue(uint32_t value);

#if defined(REV3)
  #define BATT_SCALE    120
#else
  #define BATT_SCALE    150
#endif

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

// Power driver
void pwrInit(void);
uint32_t pwrCheck(void);
void pwrOn(void);
void pwrOff(void);
uint32_t pwrPressed(void);
uint32_t pwrPressedDuration(void);
#define UNEXPECTED_SHUTDOWN()   (g_eeGeneral.unexpectedShutdown)

// Led driver
void ledInit(void);
void ledOff(void);
void ledRed(void);
void ledBlue(void);

// LCD driver

// Backlight driver
#define setBacklight(xx)
#define backlightEnable()
#define backlightDisable()
#define isBacklightEnable()   true

// USB driver
int usbPlugged(void);
void usbInit(void);
void usbDeInit(void);
void usbSerialPutc(uint8_t c);

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

// Audio driver
void audioInit(void);
void audioEnd(void);
void dacStart(void);
void dacStop(void);
void setSampleRate(uint32_t frequency);

// I2C driver
void i2cInit(void);
#define VOLUME_LEVEL_MAX  23
#define VOLUME_LEVEL_DEF  12
extern const int8_t volumeScale[];
void setVolume(uint8_t volume);

// Telemetry driver
void telemetryPortInit(uint32_t baudrate);
void sportSendBuffer(uint8_t *buffer, uint32_t count);

// Haptic driver
void hapticInit(void);
void hapticOff(void);
#define HAPTIC_OFF()    hapticOff()
void hapticOn(uint32_t pwmPercent);

// Second serial port driver
#define DEBUG_BAUDRATE      115200
void serial2Init(unsigned int mode, unsigned int protocol);
void serial2Putc(char c);
#define serial2TelemetryInit(protocol) serial2Init(UART_MODE_TELEMETRY, protocol)
void serial2SbusInit(void);
void serial2Stop(void);

#define USART_FLAG_ERRORS (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)

#if defined(USB_JOYSTICK) && !defined(SIMU)
void usbJoystickUpdate(void);
#endif

void checkTrainerSettings(void);

#endif
