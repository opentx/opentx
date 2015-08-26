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

#ifndef _BOARD_TARANIS_H_
#define _BOARD_TARANIS_H_

#include "stddef.h"

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

#if defined(REV9E)
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rcc.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_gpio.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_adc.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_spi.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_i2c.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rtc.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_pwr.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dma.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_usart.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_flash.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dbgmcu.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/misc.h"
#else
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include/stm32f2xx.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_rcc.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_gpio.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_adc.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_spi.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_i2c.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_rtc.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_pwr.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_dma.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_usart.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_flash.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_dbgmcu.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/misc.h"
#endif

#if !defined(SIMU)
  #include "STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/inc/usbd_msc_core.h"
  #include "STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/hid/inc/usbd_hid_core.h"
  #include "STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/inc/usbd_usr.h"
  #include "usbd_desc.h"
  #include "usb_conf.h"
  #include "usbd_conf.h"
#endif

#include "hal.h"
#include "aspi.h"

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

#define FLASHSIZE          0x80000
#define BOOTLOADER_SIZE    0x8000
#define FIRMWARE_ADDRESS   0x08000000

#if defined(REV9E)
  #define PERI1_FREQUENCY  42000000
  #define PERI2_FREQUENCY  84000000
#else
  #define PERI1_FREQUENCY  30000000
  #define PERI2_FREQUENCY  60000000
#endif

#define TIMER_MULT_APB1    2
#define TIMER_MULT_APB2    2

#define PIN_MODE_MASK      0x0003
#define PIN_INPUT          0x0000
#define PIN_OUTPUT         0x0001
#define PIN_PERIPHERAL     0x0002
#define PIN_ANALOG         0x0003

#define PIN_PULL_MASK      0x000C
#define PIN_PULLUP         0x0004

#define PIN_PORT_MASK      0x0700
#define PIN_PORTA          0x0000
#define PIN_PORTB          0x0100
#define PIN_PORTC          0x0200
#define PIN_PORTD          0x0300
#define PIN_PORTE          0x0400
#define PIN_PORTF          0x0500

#define PIN_PERI_MASK      0x00F0
#define PIN_PER_1          0x0010
#define PIN_PER_2          0x0020
#define PIN_PER_3          0x0030
#define PIN_PER_5          0x0050
#define PIN_PER_6          0x0060
#define PIN_PER_8          0x0080

#define PIN_SPEED_MASK     0x6000
#define PIN_OS25           0x2000
#define PIN_OS50           0x4000
#define PIN_OS100          0x6000

void configure_pins( uint32_t pins, uint16_t config );

#define strcpy_P strcpy
#define strcat_P strcat

extern uint16_t sessionTimer;

#define SLAVE_MODE()         (g_model.trainerMode == TRAINER_MODE_SLAVE)
#define TRAINER_CONNECTED()  (GPIO_ReadInputDataBit(TRAINER_GPIO_DETECT, TRAINER_GPIO_PIN_DETECT) == Bit_RESET)

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
  void sdPoll10ms(void);
  #define sdMountPoll()
  uint32_t sdMounted(void);
  #define SD_CARD_PRESENT()       (~SD_GPIO_PRESENT->IDR & SD_GPIO_PIN_PRESENT)
#endif

// Flash Write driver
#define FLASH_PAGESIZE 256
void unlockFlash(void);
void lockFlash(void);
void writeFlash(uint32_t * address, uint32_t * buffer);
uint32_t isFirmwareStart(const void * buffer);
uint32_t isBootloaderStart(const void * buffer);

// Pulses driver
#define INTERNAL_MODULE_ON()      GPIO_SetBits(INTMODULE_GPIO_PWR, INTMODULE_GPIO_PIN_PWR)
#define INTERNAL_MODULE_OFF()     GPIO_ResetBits(INTMODULE_GPIO_PWR, INTMODULE_GPIO_PIN_PWR)
#define EXTERNAL_MODULE_ON()      GPIO_SetBits(EXTMODULE_GPIO_PWR, EXTMODULE_GPIO_PIN_PWR)
#define EXTERNAL_MODULE_OFF()     GPIO_ResetBits(EXTMODULE_GPIO_PWR, EXTMODULE_GPIO_PIN_PWR)
#define IS_INTERNAL_MODULE_ON()   (GPIO_ReadInputDataBit(INTMODULE_GPIO_PWR, INTMODULE_GPIO_PIN_PWR) == Bit_SET)
#define IS_EXTERNAL_MODULE_ON()   (GPIO_ReadInputDataBit(EXTMODULE_GPIO_PWR, EXTMODULE_GPIO_PIN_PWR) == Bit_SET)
void init_no_pulses(uint32_t port);
void disable_no_pulses(uint32_t port);
void init_ppm( uint32_t module_index );
void disable_ppm( uint32_t module_index );
void set_external_ppm_parameters(uint32_t idleTime, uint32_t delay, uint32_t positive);
#if defined(TARANIS_INTERNAL_PPM)
  void set_internal_ppm_parameters(uint32_t idleTime, uint32_t delay, uint32_t positive);
#endif
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
#define TRIMS_PRESSED()            (readTrims())
#define KEYS_PRESSED()             (readKeys())

#if defined(REV9E)
// Rotary Encoder driver
void rotencInit(void);
void rotencEnd(void);
void checkRotaryEncoder(void);
#endif

// WDT driver
#if !defined(SIMU)
#define wdt_disable()
void watchdogInit(unsigned int duration);
#define wdt_enable(x)   watchdogInit(1500)
#define wdt_reset()     IWDG->KR = 0xAAAA
#define WAS_RESET_BY_SOFTWARE()               (RCC->CSR & RCC_CSR_SFTRSTF)
#define WAS_RESET_BY_WATCHDOG()               (RCC->CSR & (RCC_CSR_WDGRSTF | RCC_CSR_WWDGRSTF))
#define WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()   (RCC->CSR & (RCC_CSR_WDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_SFTRSTF))
#endif

// ADC driver
void adcInit(void);
void adcRead(void);
inline uint16_t getAnalogValue(uint32_t value);

#define BATT_SCALE    150

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

// Power driver
void pwrInit(void);
uint32_t pwrCheck(void);
void pwrOn(void);
void pwrOff(void);
#if defined(REV9E)
uint32_t pwrPressed(void);
uint32_t pwrPressedDuration(void);
#endif
#define UNEXPECTED_SHUTDOWN()   (g_eeGeneral.unexpectedShutdown)

// Backlight driver
void backlightInit(void);
#if defined(REVPLUS)
  void turnBacklightOn(uint8_t level, uint8_t color);
  void turnBacklightOff(void);
  #define setBacklight(xx)         turnBacklightOn(xx, g_eeGeneral.backlightColor)
  #define backlightEnable()        turnBacklightOn(g_eeGeneral.backlightBright, g_eeGeneral.backlightColor)
  #define backlightDisable()       turnBacklightOff()
  #define isBacklightEnable()      ((BACKLIGHT_TIMER->CCR4 != 0) || (BACKLIGHT_TIMER->CCR2 != 0))
#else
  #define setBacklight(xx)         BACKLIGHT_TIMER->CCR1 = 100-xx
  #define backlightEnable()        BACKLIGHT_TIMER->CCR1 = 100-g_eeGeneral.backlightBright
  #define backlightDisable()       BACKLIGHT_TIMER->CCR1 = 0
  #define isBacklightEnable()      (BACKLIGHT_TIMER->CCR1 != 0)
#endif

// USB driver
int usbPlugged(void);
void usbInit(void);
void usbStart(void);
#if defined(USB_JOYSTICK)
void usbStop(void);
#endif

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

// I2C driver: EEPROM + Audio Volume
void i2cInit(void);
void eepromReadBlock(uint8_t * buffer, uint32_t address, uint32_t size);
void eepromWriteBlock(uint8_t * buffer, uint32_t address, uint32_t size);

// Debug driver
void debugPutc(const char c);

// Telemetry driver
void telemetryPortInit(uint32_t baudrate);
void sportSendBuffer(uint8_t *buffer, uint32_t count);

// Audio driver
void audioInit(void) ;
void audioEnd(void) ;
void dacStart(void);
void dacStop(void);
void setSampleRate(uint32_t frequency);
extern const int8_t volumeScale[];
#define VOLUME_LEVEL_MAX  23
#define VOLUME_LEVEL_DEF  12
void setVolume(uint8_t volume);

// Haptic driver
void hapticInit(void);
void hapticOff(void);
#if defined(REVPLUS)
  void hapticOn(uint32_t pwmPercent);
#else
  void hapticOn(void);
#endif

// SERIAL_USART driver
#define DEBUG_BAUDRATE                 115200
void uart3Init(unsigned int mode, unsigned int protocol);
void uart3Putc(const char c);
#define telemetrySecondPortInit(protocol) uart3Init(UART_MODE_TELEMETRY, protocol)
void uart3SbusInit(void);
void uart3Stop(void);

// BT driver
#define BLUETOOTH_DEFAULT_BAUDRATE     115200
#define BLUETOOTH_FACTORY_BAUDRATE     9600
uint8_t bluetoothReady(void);
void bluetoothInit(uint32_t baudrate);
void bluetoothWrite(const void * buffer, int len);
void bluetoothWriteString(const char * str);
int bluetoothRead(void * buffer, int len);
void bluetoothWakeup(void);

// LCD driver
void lcdInit(void);
void lcdInitFinish(void);
void lcdOff(void);

// Top LCD driver
#if defined(REV9E)
void topLcdInit(void);
void topLcdOff(void);
void topLcdRefreshStart(void);
void topLcdRefreshEnd(void);
void setTopFirstTimer(int32_t value);
void setTopSecondTimer(uint32_t value);
void setTopRssi(uint32_t rssi);
void setTopBatteryState(int state, uint8_t blinking);
void setTopBatteryValue(uint32_t volts);
#endif

#define USART_FLAG_ERRORS (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)

#if defined(USB_JOYSTICK) && !defined(SIMU)
void usbJoystickUpdate(void);
#endif

void checkTrainerSettings(void);

#endif
