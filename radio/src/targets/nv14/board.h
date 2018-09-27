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

#ifndef _BOARD_H_
#define _BOARD_H_

#include "stddef.h"
#include "stdbool.h"
#include "opentx_constants.h"
#include "shutdown_animation.h"

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

#if __clang__
// clang is very picky about the use of "register"
// Tell clang to ignore the warnings for the following files
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
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
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_adc.h"
#include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/misc.h"

#if __clang__
// Restore warnings about registers
#pragma clang diagnostic pop
#endif

#include "usb_driver.h"

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

#include "touch_driver.h"
#include "hallStick_driver.h"
#include "lcd_driver.h"
#include "battery_driver.h"

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

#define FLASHSIZE                       0x200000
#define BOOTLOADER_SIZE                 0x20000
#define FIRMWARE_ADDRESS                0x08000000

#define MB                              *1024*1024
#define LUA_MEM_EXTRA_MAX               (2 MB)    // max allowed memory usage for Lua bitmaps (in bytes)
#define LUA_MEM_MAX                     (6 MB)    // max allowed memory usage for complete Lua  (in bytes), 0 means unlimited

// HSI is at 168Mhz (over-drive is not enabled!)
#define PERI1_FREQUENCY                 42000000
#define PERI2_FREQUENCY                 84000000
#define TIMER_MULT_APB1                 2
#define TIMER_MULT_APB2                 2

#define strcpy_P strcpy

extern uint16_t sessionTimer;

#define SLAVE_MODE()                    (g_model.trainerMode == TRAINER_MODE_SLAVE)

#define TRAINER_CONNECTED()             (GPIO_ReadInputDataBit(TRAINER_DETECT_GPIO, TRAINER_DETECT_GPIO_PIN) == Bit_SET)

// Board driver
void boardInit(void);
void boardOff(void);

// Delays driver
#ifdef __cplusplus
extern "C" {
#endif
void delaysInit(void);
void delay_01us(uint16_t nb);
void delay_us(uint16_t nb);
void delay_ms(uint32_t ms);
#ifdef __cplusplus
}
#endif

// CPU Unique ID
#define LEN_CPU_UID                     (3*8+2)
void getCPUUniqueID(char * s);

// SD driver
#define BLOCK_SIZE                      512 /* Block Size in Bytes */
#if !defined(SIMU) || defined(SIMU_DISKIO)
uint32_t sdIsHC(void);
uint32_t sdGetSpeed(void);
#define SD_IS_HC()                     (sdIsHC())
#define SD_GET_SPEED()                 (sdGetSpeed())
#define SD_GET_FREE_BLOCKNR()          (sdGetFreeSectors())
#define SD_CARD_PRESENT()              (true)
void sdInit(void);
void sdMount(void);
void sdDone(void);
#define sdPoll10ms()
uint32_t sdMounted(void);
#else
#define SD_IS_HC()                      (0)
#define SD_GET_SPEED()                  (0)
#define sdInit()
#define sdMount()
#define sdDone()
#define SD_CARD_PRESENT()               true
#endif

#if defined(DISK_CACHE)
#include "diskio.h"
DRESULT __disk_read(BYTE drv, BYTE * buff, DWORD sector, UINT count);
DRESULT __disk_write(BYTE drv, const BYTE * buff, DWORD sector, UINT count);
#else
#define __disk_read                     disk_read
#define __disk_write                    disk_write
#endif

// Flash Write driver
#define FLASH_PAGESIZE 256
void unlockFlash(void);
void lockFlash(void);
void flashWrite(uint32_t * address, uint32_t * buffer);
uint32_t isFirmwareStart(const uint8_t * buffer);
uint32_t isBootloaderStart(const uint8_t * buffer);

// SDRAM driver
void sdramInit(void);

// Pulses driver
#define INTERNAL_MODULE_OFF()           GPIO_SetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN)
#define INTERNAL_MODULE_ON()            GPIO_ResetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN)
#define EXTERNAL_MODULE_ON()            GPIO_SetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN)
#define EXTERNAL_MODULE_OFF()           GPIO_ResetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN)
#define BLUETOOTH_MODULE_ON()           GPIO_ResetBits(BLUETOOTH_ON_GPIO, BLUETOOTH_ON_GPIO_PIN)
#define BLUETOOTH_MODULE_OFF()          GPIO_SetBits(BLUETOOTH_ON_GPIO, BLUETOOTH_ON_GPIO_PIN)
#define IS_INTERNAL_MODULE_ON()         (GPIO_ReadInputDataBit(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN) == Bit_SET)
#define IS_EXTERNAL_MODULE_ON()         (GPIO_ReadInputDataBit(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN) == Bit_SET)
#define IS_UART_MODULE(port)            (port == INTERNAL_MODULE)

void init_no_pulses(uint32_t port);
void disable_no_pulses(uint32_t port);
void init_ppm(uint32_t module_index);
void disable_ppm(uint32_t module_index);
void init_pxx(uint32_t module_index);
void disable_pxx(uint32_t module_index);
void init_dsm2(uint32_t module_index);
void disable_dsm2(uint32_t module_index);
void init_crossfire(uint32_t module_index);
void disable_crossfire(uint32_t module_index);
void init_sbusOut(uint32_t module_index);
void disable_sbusOut(uint32_t module_index);
void init_serial(uint32_t module_index, uint32_t baudrate, uint32_t period_half_us);
void disable_serial(uint32_t port);

// Trainer driver
void init_trainer_ppm(void);
void stop_trainer_ppm(void);
void init_trainer_capture(void);
void stop_trainer_capture(void);

// Keys driver
enum EnumKeys
{
  TRM_BASE,
  TRM_LH_DWN = TRM_BASE,
  TRM_LH_UP,
  TRM_LV_DWN,
  TRM_LV_UP,
  TRM_RV_DWN,
  TRM_RV_UP,
  TRM_RH_DWN,
  TRM_RH_UP,
  TRM_LS_DWN,
  TRM_LS_UP,
  TRM_RS_DWN,
  TRM_RS_UP,
  TRM_LAST = TRM_RS_UP,

  NUM_KEYS
};

#define IS_SHIFT_KEY(index)             (false)
#define IS_SHIFT_PRESSED()              (false)

enum EnumSwitches
{
  SW_SA,
  SW_SB,
  SW_SC,
  SW_SD,
  SW_SE,
  SW_SF,
  SW_SG,
  SW_SH,
  NUM_SWITCHES
};

#define DEFAULT_SWITCH_CONFIG (SWITCH_TOGGLE << 14) + (SWITCH_3POS << 12) + (SWITCH_3POS << 10) + (SWITCH_TOGGLE << 8) + (SWITCH_2POS << 6) + (SWITCH_TOGGLE << 4) + (SWITCH_3POS << 2) + (SWITCH_2POS << 0);

enum EnumSwitchesPositions
{
  SW_SA0,
  SW_SA1,
  SW_SA2,
  SW_SB0,
  SW_SB1,
  SW_SB2,
  SW_SC0,
  SW_SC1,
  SW_SC2,
  SW_SD0,
  SW_SD1,
  SW_SD2,
  SW_SE0,
  SW_SE1,
  SW_SE2,
  SW_SF0,
  SW_SF1,
  SW_SF2,
  SW_SG0,
  SW_SG1,
  SW_SG2,
  SW_SH0,
  SW_SH1,
  SW_SH2,
};

void monitorInit(void);
void keysInit(void);
uint8_t keyState(uint8_t index);
uint32_t switchState(uint8_t index);
uint32_t readKeys(void);
uint32_t readTrims(void);
#define NUM_TRIMS                       NUM_STICKS
#define TRIMS_PRESSED()                 (readTrims())
#define KEYS_PRESSED()                  (readKeys())
#define DBLKEYS_PRESSED_RGT_LFT(in)     (false)
#define DBLKEYS_PRESSED_UP_DWN(in)      (false)
#define DBLKEYS_PRESSED_RGT_UP(in)      (false)
#define DBLKEYS_PRESSED_LFT_DWN(in)     (false)

// WDT driver
#define WDTO_500MS                      500
extern uint32_t powerupReason;

#define SHUTDOWN_REQUEST                0xDEADBEEF
#define NO_SHUTDOWN_REQUEST             ~SHUTDOWN_REQUEST
#define DIRTY_SHUTDOWN                  0xCAFEDEAD
#define NORMAL_POWER_OFF                ~DIRTY_SHUTDOWN

#define wdt_disable()
void watchdogInit(unsigned int duration);
#if defined(SIMU)
  #define WAS_RESET_BY_WATCHDOG()               (false)
  #define WAS_RESET_BY_SOFTWARE()               (false)
  #define WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()   (false)
  #define wdt_enable(x)
  #define wdt_reset()
#else
  #if defined(WATCHDOG_DISABLED)
    #define wdt_enable(x)
    #define wdt_reset()
  #else
    #define wdt_enable(x)                       watchdogInit(x)
    #define wdt_reset()                         IWDG->KR = 0xAAAA
  #endif
  #define WAS_RESET_BY_WATCHDOG()               (RCC->CSR & (RCC_CSR_WDGRSTF | RCC_CSR_WWDGRSTF))
  #define WAS_RESET_BY_SOFTWARE()               (RCC->CSR & RCC_CSR_SFTRSTF)
  #define WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()   (RCC->CSR & (RCC_CSR_WDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_SFTRSTF))
#endif

// ADC driver
#define NUM_POTS                        2
#define NUM_XPOTS                       0 // NUM_POTS
#define NUM_SLIDERS                     0
#define NUM_PWMANALOGS                  0
#define NUM_MOUSE_ANALOGS               0
#define NUM_DUMMY_ANAS                  0

enum Analogs {
  STICK1,
  STICK2,
  STICK3,
  STICK4,
  POT_FIRST,
  POT1 = POT_FIRST,
  POT2,
  POT_LAST = POT2,
  SWITCH_FIRST,
  SWA = SWITCH_FIRST,
  SWB,
  SWC,
  SWD,
  SWE,
  SWF,
  SWG,
  SWH,
  SUB_ANALOG_POS = SWH,
  SWITCH_END = SWH,
  TX_VOLTAGE,
  NUM_ANALOGS
};

#define DEFAULT_POTS_CONFIG (POT_WITHOUT_DETENT << 0) + (POT_WITHOUT_DETENT << 2) // 2 pots without detent

enum CalibratedAnalogs {
  CALIBRATED_STICK1,
  CALIBRATED_STICK2,
  CALIBRATED_STICK3,
  CALIBRATED_STICK4,
  CALIBRATED_POT1,
  CALIBRATED_POT2,
  CALIBRATED_SWA,
  CALIBRATED_SWB,
  CALIBRATED_SWC,
  CALIBRATED_SWD,
  CALIBRATED_SWE,
  CALIBRATED_SWF,
  CALIBRATED_SWG,
  CALIBRATED_SWH,
  NUM_CALIBRATED_ANALOGS
};

#define IS_POT(x)                       ((x)>=POT_FIRST && (x)<=POT_LAST)
#define IS_SLIDER(x)                    (false)

extern uint16_t adcValues[NUM_ANALOGS];

void adcInit(void);
void adcRead(void);
uint16_t getAnalogValue(uint8_t index);
uint16_t getBatteryVoltage();   // returns current battery voltage in 10mV steps

#define BATTERY_WARN                  37 // 3.7V
#define BATTERY_MIN                   35 // 3.5V
#define BATTERY_MAX                   43 // 4.3V

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

// Power driver
#define SOFT_PWR_CTRL
void pwrInit(void);
uint32_t pwrCheck(void);
#if defined(PCBFLYSKY)
uint32_t lowPowerCheck(void);
#endif
void pwrOn(void);
void pwrOff(void);
void pwrResetHandler(void);
uint32_t pwrPressed(void);
uint32_t pwrPressedDuration(void);
#if defined(SIMU) || defined(NO_UNEXPECTED_SHUTDOWN)
  #define UNEXPECTED_SHUTDOWN()         (false)
#else
  #define UNEXPECTED_SHUTDOWN()         ((powerupReason == DIRTY_SHUTDOWN) || WAS_RESET_BY_WATCHDOG_OR_SOFTWARE())
#endif

// LCD driver
#define LCD_W                           320
#define LCD_H                           480
#define LCD_DEPTH                       16
#define LCD_CONTRAST_DEFAULT            20
void lcdInit(void);
void lcdNextLayer(void);
void lcdRefresh(void);
void DMACopy(void * src, void * dest, unsigned size);
void DMAFillRect(uint16_t * dest, uint16_t destw, uint16_t desth, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void DMACopyBitmap(uint16_t * dest, uint16_t destw, uint16_t desth, uint16_t x, uint16_t y, const uint16_t * src, uint16_t srcw, uint16_t srch, uint16_t srcx, uint16_t srcy, uint16_t w, uint16_t h);
void DMACopyAlphaBitmap(uint16_t * dest, uint16_t destw, uint16_t desth, uint16_t x, uint16_t y, const uint16_t * src, uint16_t srcw, uint16_t srch, uint16_t srcx, uint16_t srcy, uint16_t w, uint16_t h);
void DMABitmapConvert(uint16_t * dest, const uint8_t * src, uint16_t w, uint16_t h, uint32_t format);
void lcdSetContrast();
#define lcdOff()                        backlightEnable(0) /* just disable the backlight */
#define lcdSetRefVolt(...)
#define lcdRefreshWait(...)

// Backlight driver
void backlightInit(void);
#if defined(SIMU)
#define backlightEnable(...)
#else
void backlightEnable(uint8_t dutyCycle);
#endif

#define BACKLIGHT_LEVEL_MAX             100
#define BACKLIGHT_LEVEL_MIN             15

#define BACKLIGHT_ENABLE()              backlightEnable(unexpectedShutdown ? BACKLIGHT_LEVEL_MAX : BACKLIGHT_LEVEL_MAX-g_eeGeneral.backlightBright)
#define BACKLIGHT_DISABLE()             backlightEnable(unexpectedShutdown ? BACKLIGHT_LEVEL_MAX : ((g_eeGeneral.blOffBright == BACKLIGHT_LEVEL_MIN) && (g_eeGeneral.backlightMode != e_backlight_mode_off)) ? 0 : g_eeGeneral.blOffBright)
#define isBacklightEnabled()            true

#if !defined(SIMU)
void usbJoystickUpdate();
#endif
#define USBD_MANUFACTURER_STRING        "FlySky"
#define USB_NAME                        "FlySky NV14"
#define USB_MANUFACTURER                'F', 'l', 'y', 'S', 'k', 'y', ' ', ' '  /* 8 bytes */
#define USB_PRODUCT                     'N', 'V', '1', '4', ' ', ' ', ' ', ' '  /* 8 Bytes */

#if defined(__cplusplus) && !defined(SIMU)
}
#endif


// Audio driver
void audioInit(void);
void audioConsumeCurrentBuffer(void);
void Audio_Sine_Test(void);
void audioSpiWriteBuffer(const uint8_t * buffer, uint32_t size);
void audioSpiSetSpeed(uint8_t speed);
uint8_t audioHardReset(void);
uint8_t audioSoftReset(void);
void audioSendRiffHeader();

#define SPI_SPEED_2                    0
#define SPI_SPEED_4                    1
#define SPI_SPEED_8                    2
#define SPI_SPEED_16                   3
#define SPI_SPEED_32                   4
#define SPI_SPEED_64                   5
#define SPI_SPEED_128                  6
#define SPI_SPEED_256                  7

#define audioDisableIrq()             // interrupts must stay enabled on Horus
#define audioEnableIrq()              // interrupts must stay enabled on Horus
#if defined(PCBNV14)
#define setSampleRate(freq)
#else
void setSampleRate(uint32_t frequency);
#endif
void setScaledVolume(uint8_t volume);
void setVolume(uint8_t volume);
int32_t getVolume(void);
#define VOLUME_LEVEL_MAX               23
#define VOLUME_LEVEL_DEF               12

// Telemetry driver
#define TELEMETRY_FIFO_SIZE             512
void telemetryPortInit(uint32_t baudrate, uint8_t mode);
void telemetryPortSetDirectionOutput(void);
void sportSendBuffer(uint8_t * buffer, uint32_t count);
uint8_t telemetryGetByte(uint8_t * byte);
extern uint32_t telemetryErrors;

// Sport update driver
#define SPORT_UPDATE_POWER_ON()
#define SPORT_UPDATE_POWER_OFF()

// Haptic driver
void hapticInit(void);
void hapticDone(void);
void hapticOff(void);
#define HAPTIC_OFF()                    hapticOff()
void hapticOn(uint32_t pwmPercent);

// Second serial port driver
#define AUX_SERIAL
#define DEBUG_BAUDRATE                  115200
extern uint8_t auxSerialMode;
void auxSerialInit(unsigned int mode, unsigned int protocol);
void auxSerialPutc(char c);
#define auxSerialTelemetryInit(protocol) auxSerialInit(UART_MODE_TELEMETRY, protocol)
void auxSerialSbusInit(void);
void auxSerialStop(void);
#define USART_FLAG_ERRORS               (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)

extern uint8_t currentTrainerMode;
void checkTrainerSettings(void);

#if defined(__cplusplus)
#include "fifo.h"
#include "dmafifo.h"
extern DMAFifo<512> telemetryFifo;
extern Fifo<uint8_t, 32> auxSerialRxFifo;
#endif

uint8_t touchPressed(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

#endif // _BOARD_H_
