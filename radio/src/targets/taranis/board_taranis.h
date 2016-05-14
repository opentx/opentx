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
  #include "usbd_cdc_core.h"
  #include "usbd_msc_core.h"
  #include "usbd_hid_core.h"
  #include "usbd_usr.h"
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

#define SLAVE_MODE()                   (g_model.trainerMode == TRAINER_MODE_SLAVE)

#if defined(REV9E)
#define TRAINER_CONNECTED()            (true)
#else
#define TRAINER_CONNECTED()            (GPIO_ReadInputDataBit(TRAINER_GPIO_DETECT, TRAINER_GPIO_PIN_DETECT) == Bit_RESET)
#endif

#ifdef __cplusplus
extern "C" {
#endif
void delaysInit(void);
void delay_01us(uint16_t nb);
void delay_ms(uint32_t ms);
#ifdef __cplusplus
}
#endif

// CPU Unique ID
#define LEN_CPU_UID                    (3*8+2)
void getCPUUniqueID(char * s);

// SD driver
#if !defined(SIMU) || defined(SIMU_DISKIO)
  uint32_t sdIsHC(void);
  uint32_t sdGetSpeed(void);
  #define SD_IS_HC()                   (sdIsHC())
  #define SD_GET_SPEED()               (sdGetSpeed())
  #define SD_GET_FREE_BLOCKNR()        (sdGetFreeSectors())
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
  #define SD_CARD_PRESENT()            (~SD_GPIO_PRESENT->IDR & SD_GPIO_PIN_PRESENT)
#endif

// Flash Write driver
#define FLASH_PAGESIZE 256
void unlockFlash(void);
void lockFlash(void);
void writeFlash(uint32_t * address, uint32_t * buffer);
uint32_t isFirmwareStart(const void * buffer);
uint32_t isBootloaderStart(const void * buffer);

// Pulses driver
#define INTERNAL_MODULE_ON()      GPIO_SetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN)
#define INTERNAL_MODULE_OFF()     GPIO_ResetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN)
#define EXTERNAL_MODULE_ON()      GPIO_SetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN)
#define EXTERNAL_MODULE_OFF()     GPIO_ResetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN)
#define IS_INTERNAL_MODULE_ON()   (GPIO_ReadInputDataBit(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN) == Bit_SET)
#define IS_EXTERNAL_MODULE_ON()   (GPIO_ReadInputDataBit(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN) == Bit_SET)
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
void init_crossfire( uint32_t module_index );
void disable_crossfire( uint32_t module_index );

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
int sbusGetByte(uint8_t * byte);

// Keys driver
void keysInit(void);
uint32_t readKeys(void);
uint32_t readTrims(void);
#define TRIMS_PRESSED()            (readTrims())
#define KEYS_PRESSED()             (readKeys())

#if defined(REV9E)
// Rotary Encoder driver
extern int32_t rotencValue;
void rotencInit(void);
void rotencEnd(void);
void checkRotaryEncoder(void);
#endif

// WDT driver
#if !defined(SIMU)
#define wdt_disable()
void watchdogInit(unsigned int duration);
#define wdt_enable(x)                         watchdogInit(x)
#define wdt_reset()                           IWDG->KR = 0xAAAA
#define WAS_RESET_BY_SOFTWARE()               (RCC->CSR & RCC_CSR_SFTRSTF)
#define WAS_RESET_BY_WATCHDOG()               (RCC->CSR & (RCC_CSR_WDGRSTF | RCC_CSR_WWDGRSTF))
#define WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()   (RCC->CSR & (RCC_CSR_WDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_SFTRSTF))
#endif

// ADC driver
enum Analogs {
  STICK1,
  STICK2,
  STICK3,
  STICK4,
  POT1,
  POT2,
  POT3,
  #if defined(REV9E)
    POT4,
    POT_LAST = POT4,
  #else
    POT_LAST = POT3,
  #endif
  SLIDER1,
  SLIDER2,
  #if defined(REV9E)
    SLIDER3,
    SLIDER4,
  #endif
  TX_VOLTAGE,
  NUMBER_ANALOG
};
void adcInit(void);
void adcRead(void);
extern uint16_t adcValues[NUMBER_ANALOG];
uint16_t getAnalogValue(uint8_t index);
uint16_t getBatteryVoltage();   // returns current battery voltage in 10mV steps

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
void usbDeInit(void);
void usbSerialPutc(uint8_t c);

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
void telemetryPortSetDirectionOutput(void);
void sportSendBuffer(uint8_t * buffer, uint32_t count);
int telemetryGetByte(uint8_t * byte);
extern uint32_t telemetryErrors;

// Audio driver
void audioInit(void) ;
void audioEnd(void) ;
void dacStart(void);
void dacStop(void);
void setSampleRate(uint32_t frequency);
#define VOLUME_LEVEL_MAX  23
#define VOLUME_LEVEL_DEF  12
void setScaledVolume(uint8_t volume);
void setVolume(uint8_t volume);
int32_t getVolume(void);
#define audioConsumeCurrentBuffer()

// Haptic driver
void hapticInit(void);
void hapticOff(void);
#if defined(REVPLUS)
  void hapticOn(uint32_t pwmPercent);
#else
  void hapticOn(void);
#endif

// Second serial port driver
#define DEBUG_BAUDRATE                 115200
extern uint8_t serial2Mode;
void serial2Init(unsigned int mode, unsigned int protocol);
void serial2Putc(char c);
#define serial2TelemetryInit(protocol) serial2Init(UART_MODE_TELEMETRY, protocol)
void serial2SbusInit(void);
void serial2Stop(void);

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
void toplcdInit(void);
void toplcdOff(void);
void toplcdRefreshStart(void);
void toplcdRefreshEnd(void);
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

extern uint8_t currentTrainerMode;
void checkTrainerSettings(void);

#if defined(__cplusplus)
#include "fifo.h"
#include "dmafifo.h"
extern Fifo<uint8_t, 32> telemetryFifo;
extern DMAFifo<32> serial2RxFifo;
extern Fifo<uint8_t, 32> sbusFifo;
#endif

#endif // _BOARD_TARANIS_H_
