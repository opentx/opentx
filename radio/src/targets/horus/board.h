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

#ifndef _BOARD_HORUS_H_
#define _BOARD_HORUS_H_

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

#define FLASHSIZE                      0x80000
#define BOOTLOADER_SIZE                0x8000
#define FIRMWARE_ADDRESS               0x08000000

// HSI is at 168Mhz (over-drive is not enabled!)
#define PERI1_FREQUENCY                42000000
#define PERI2_FREQUENCY                84000000
#define TIMER_MULT_APB1                2
#define TIMER_MULT_APB2                2

#define strcpy_P strcpy
#define strcat_P strcat

extern uint16_t sessionTimer;

#define SLAVE_MODE()                   (g_model.trainerMode == TRAINER_MODE_SLAVE)
#define TRAINER_CONNECTED()            (GPIO_ReadInputDataBit(TRAINER_DETECT_GPIO, TRAINER_DETECT_GPIO_PIN) == Bit_RESET)

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
#define LEN_CPU_UID                    (3*8+2)
void getCPUUniqueID(char * s);

// SD driver
#define BLOCK_SIZE                     512 /* Block Size in Bytes */
#if !defined(SIMU) || defined(SIMU_DISKIO)
uint32_t sdIsHC(void);
uint32_t sdGetSpeed(void);
#define SD_IS_HC()                     (sdIsHC())
#define SD_GET_SPEED()                 (sdGetSpeed())
#define SD_GET_FREE_BLOCKNR()          (sdGetFreeSectors())
#define SD_CARD_PRESENT()              (~SD_PRESENT_GPIO->IDR & SD_PRESENT_GPIO_PIN)
void sdInit(void);
void sdMount(void);
void sdDone(void);
#define sdPoll10ms()
#define sdMountPoll()
uint32_t sdMounted(void);
#else
#define SD_IS_HC()                     (0)
#define SD_GET_SPEED()                 (0)
#define sdInit()
#define sdMount()
#define sdDone()
#define SD_CARD_PRESENT()              true
#endif
#if defined(DISK_CACHE)
#include "diskio.h"
DRESULT __disk_read(BYTE drv, BYTE * buff, DWORD sector, UINT count);
DRESULT __disk_write(BYTE drv, const BYTE * buff, DWORD sector, UINT count);
#else
#define __disk_read                    disk_read
#define __disk_write                   disk_write
#endif

// Flash Write driver
#define FLASH_PAGESIZE                 256
void unlockFlash(void);
void lockFlash(void);
void writeFlash(uint32_t * address, uint32_t * buffer);
uint32_t isFirmwareStart(const void * buffer);
uint32_t isBootloaderStart(const void * buffer);

// SDRAM driver
void SDRAM_Init(void);

// Pulses driver
#define INTERNAL_MODULE_ON()           GPIO_SetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN)
#define INTERNAL_MODULE_OFF()          GPIO_ResetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN)
#define EXTERNAL_MODULE_ON()           GPIO_SetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN)
#define EXTERNAL_MODULE_OFF()          GPIO_ResetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN)
#define IS_INTERNAL_MODULE_ON()        (GPIO_ReadInputDataBit(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN) == Bit_SET)
#define IS_EXTERNAL_MODULE_ON()        (GPIO_ReadInputDataBit(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN) == Bit_SET)
#define IS_UART_MODULE(port)           (port == INTERNAL_MODULE)

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

// Trainer driver
void init_trainer_ppm(void);
void stop_trainer_ppm(void);
void init_trainer_capture(void);
void stop_trainer_capture(void);

// Keys driver
enum EnumKeys
{
  KEY_PGUP,
  KEY_PGDN,
  KEY_ENTER,
  KEY_MODEL,
  KEY_UP = KEY_MODEL,
  KEY_EXIT,
  KEY_DOWN = KEY_EXIT,
  KEY_TELEM,
  KEY_RIGHT = KEY_TELEM,
  KEY_RADIO,
  KEY_LEFT = KEY_RADIO,
  
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
#define IS_3POS(x)                     ((x) != SW_SF && (x) != SW_SH)

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
void keysInit(void);
uint8_t keyState(uint8_t index);
uint32_t switchState(uint8_t index);
uint32_t readKeys(void);
uint32_t readTrims(void);
#define TRIMS_PRESSED()                (readTrims())
#define KEYS_PRESSED()                 (readKeys())
#define DBLKEYS_PRESSED_RGT_LFT(in)    ((in & (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_LEFT)) == (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_LEFT))
#define DBLKEYS_PRESSED_UP_DWN(in)     ((in & (KEYS_GPIO_PIN_UP + KEYS_GPIO_PIN_DOWN)) == (KEYS_GPIO_PIN_UP + KEYS_GPIO_PIN_DOWN))
#define DBLKEYS_PRESSED_RGT_UP(in)     ((in & (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_UP))  == (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_UP))
#define DBLKEYS_PRESSED_LFT_DWN(in)    ((in & (KEYS_GPIO_PIN_LEFT + KEYS_GPIO_PIN_DOWN)) == (KEYS_GPIO_PIN_LEFT + KEYS_GPIO_PIN_DOWN))

// Rotary encoder driver
extern int32_t rotencValue;
extern uint32_t rotencSpeed;
#define ROTENC_LOWSPEED                1
#define ROTENC_MIDSPEED                5
#define ROTENC_HIGHSPEED               50
void checkRotaryEncoder(void);

// WDT driver
#define WDTO_500MS                     500
#define wdt_disable()
void watchdogInit(unsigned int duration);
#if defined(WATCHDOG_DISABLED) || defined(SIMU)
  #define wdt_enable(x)
  #define wdt_reset()
#else
  #define wdt_enable(x)                watchdogInit(x)
  #define wdt_reset()                  IWDG->KR = 0xAAAA
#endif
#define WAS_RESET_BY_WATCHDOG()               (RCC->CSR & (RCC_CSR_WDGRSTF | RCC_CSR_WWDGRSTF))
#define WAS_RESET_BY_SOFTWARE()               (RCC->CSR & RCC_CSR_SFTRSTF)
#define WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()   (RCC->CSR & (RCC_CSR_WDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_SFTRSTF))

// ADC driver
#define NUM_POTS                       3
#define NUM_SLIDERS                    4
#define NUM_XPOTS                      3
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
  SLIDER1,
  SLIDER2,
  SLIDER3,
  SLIDER4,
  TX_VOLTAGE,
  MOUSE1,
  MOUSE2,
  NUMBER_ANALOG
};
#define IS_POT(x)                      ((x)>=POT_FIRST && (x)<=POT_LAST)
#define IS_SLIDER(x)                   ((x)>=SLIDER1 && (x)<=SLIDER4)
extern uint16_t adcValues[NUMBER_ANALOG];
void adcInit(void);
void adcRead(void);
uint16_t getAnalogValue(uint8_t index);
uint16_t getBatteryVoltage();   // returns current battery voltage in 10mV steps

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

// Power driver
void pwrInit(void);
uint32_t pwrCheck(void);
void pwrOn(void);
void pwrOff(void);
void pwrResetHandler(void);
uint32_t pwrPressed(void);
uint32_t pwrPressedDuration(void);
#define pwroffPressed()                pwrPressed()
#define UNEXPECTED_SHUTDOWN()          (WAS_RESET_BY_WATCHDOG())

// Led driver
void ledOff(void);
void ledRed(void);
void ledBlue(void);

// LCD driver
#define LCD_W                          480
#define LCD_H                          272
#define LCD_DEPTH                      16
void lcdInit(void);
void lcdRefresh(void);
void DMAFillRect(uint16_t * dest, uint16_t destw, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void DMACopyBitmap(uint16_t * dest, uint16_t destw, uint16_t x, uint16_t y, const uint16_t * src, uint16_t srcw, uint16_t srcx, uint16_t srcy, uint16_t w, uint16_t h);
void DMACopyAlphaBitmap(uint16_t * dest, uint16_t destw, uint16_t x, uint16_t y, const uint16_t * src, uint16_t srcw, uint16_t srcx, uint16_t srcy, uint16_t w, uint16_t h);
void DMABitmapConvert(uint16_t * dest, const uint8_t * src, uint16_t w, uint16_t h, uint32_t format);
void lcdStoreBackupBuffer(void);
int lcdRestoreBackupBuffer(void);
void lcdSetContrast();
#define lcdOff(...)
#define lcdSetRefVolt(...)
#define lcdRefreshWait(...)

// Backlight driver
void backlightInit(void);
#if defined(SIMU)
#define backlightEnable(...)
#else
void backlightEnable(uint8_t dutyCycle);
#endif
#define BACKLIGHT_ENABLE()    backlightEnable(UNEXPECTED_SHUTDOWN() ? 100 : 100-g_eeGeneral.backlightBright)
#define BACKLIGHT_DISABLE()   backlightEnable(UNEXPECTED_SHUTDOWN() ? 100 : g_eeGeneral.blOffBright)
#define isBacklightEnabled()  true

// USB driver
int usbPlugged(void);
void usbInit(void);
void usbDeInit(void);
void usbSerialPutc(uint8_t c);
#define USB_NAME                       "FrSky Horus"
#define USB_MANUFACTURER               'F', 'r', 'S', 'k', 'y', ' ', ' ', ' '  /* 8 bytes */
#define USB_PRODUCT                    'H', 'o', 'r', 'u', 's', ' ', ' ', ' '  /* 8 Bytes */

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

// Audio driver
void audioInit(void);
void audioConsumeCurrentBuffer(void);
#define setSampleRate(freq)
void setScaledVolume(uint8_t volume);
void setVolume(uint8_t volume);
int32_t getVolume(void);
#define VOLUME_LEVEL_MAX               23
#define VOLUME_LEVEL_DEF               12

// Telemetry driver
void telemetryPortInit(uint32_t baudrate, int mode);
void telemetryPortSetDirectionOutput(void);
void sportSendBuffer(uint8_t * buffer, uint32_t count);
int telemetryGetByte(uint8_t * byte);

// Haptic driver
void hapticInit(void);
void hapticDone(void);
void hapticOff(void);
#define HAPTIC_OFF()                   hapticOff()
void hapticOn(uint32_t pwmPercent);

// GPS driver
#if PCBREV >= 13
#define INTERNAL_GPS                   1
#else
#define INTERNAL_GPS                   0
#endif
void gpsInit(void);
uint8_t gpsGetByte(uint8_t * byte);

// Second serial port driver
#define DEBUG_BAUDRATE                 115200
extern uint8_t serial2Mode;
void serial2Init(unsigned int mode, unsigned int protocol);
void serial2Putc(char c);
#define serial2TelemetryInit(protocol) serial2Init(UART_MODE_TELEMETRY, protocol)
void serial2SbusInit(void);
void serial2Stop(void);
#define USART_FLAG_ERRORS              (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)
int sbusGetByte(uint8_t * byte);

#if defined(USB_JOYSTICK) && !defined(SIMU)
void usbJoystickUpdate(void);
#endif

extern uint8_t currentTrainerMode;
void checkTrainerSettings(void);

#if defined(__cplusplus)
#include "fifo.h"
#include "dmafifo.h"
extern DMAFifo<512> telemetryFifo;
extern DMAFifo<32> serial2RxFifo;
#endif


#endif // _BOARD_HORUS_H_
