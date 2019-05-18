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

#include <inttypes.h>
#include "../definitions.h"
#include "cpu_id.h"

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

#if __clang__
// clang is very picky about the use of "register"
// tell it to ignore for the STM32 includes instead of modyfing the orginal files
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

#if defined(STM32F4)
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rcc.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_syscfg.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_gpio.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_exti.h"
  #include "STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_tim.h"
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
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_syscfg.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_gpio.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_exti.h"
  #include "STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_tim.h"
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

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

#define FLASHSIZE                       0x80000
#define BOOTLOADER_SIZE                 0x8000
#define FIRMWARE_ADDRESS                0x08000000

#define LUA_MEM_MAX                     (0)    // max allowed memory usage for complete Lua  (in bytes), 0 means unlimited

#if defined(STM32F4)
  #define PERI1_FREQUENCY               42000000
  #define PERI2_FREQUENCY               84000000
#else
  #define PERI1_FREQUENCY               30000000
  #define PERI2_FREQUENCY               60000000
#endif

#define TIMER_MULT_APB1                 2
#define TIMER_MULT_APB2                 2

extern uint16_t sessionTimer;

// Board driver
void boardPreInit(void);
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

// SD driver
#define BLOCK_SIZE                      512 /* Block Size in Bytes */
#if !defined(SIMU) || defined(SIMU_DISKIO)
uint32_t sdIsHC(void);
uint32_t sdGetSpeed(void);
#define SD_IS_HC()                      (sdIsHC())
#define SD_GET_SPEED()                  (sdGetSpeed())
#define SD_GET_FREE_BLOCKNR()           (sdGetFreeSectors())
#else
#define SD_IS_HC()                      (0)
#define SD_GET_SPEED()                  (0)
#endif
#define __disk_read                     disk_read
#define __disk_write                    disk_write
#if defined(SIMU)
  #if !defined(SIMU_DISKIO)
    #define sdInit()
    #define sdDone()
  #endif
  #define sdMount()
  #define SD_CARD_PRESENT()               true
#else
void sdInit(void);
void sdMount(void);
void sdDone(void);
void sdPoll10ms(void);
uint32_t sdMounted(void);
#define SD_CARD_PRESENT()               ((SD_GPIO_PRESENT_GPIO->IDR & SD_GPIO_PRESENT_GPIO_PIN) == 0)
#endif

// Flash Write driver
#define FLASH_PAGESIZE 256
void unlockFlash(void);
void lockFlash(void);
void flashWrite(uint32_t * address, uint32_t * buffer);
uint32_t isFirmwareStart(const uint8_t * buffer);
uint32_t isBootloaderStart(const uint8_t * buffer);

// Pulses driver
#define INTERNAL_MODULE_ON()            GPIO_SetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN)
#if defined(INTMODULE_USART)
  #define INTERNAL_MODULE_OFF()         intmoduleStop()
#else
  #define INTERNAL_MODULE_OFF()         GPIO_ResetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN)
#endif

#define EXTERNAL_MODULE_ON()            EXTERNAL_MODULE_PWR_ON()

#if defined(EXTMODULE_USART)
#define EXTERNAL_MODULE_OFF()         extmoduleStop()
#else
#define EXTERNAL_MODULE_OFF()         EXTERNAL_MODULE_PWR_OFF()
#endif

#define IS_INTERNAL_MODULE_ON()         (GPIO_ReadInputDataBit(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN) == Bit_SET)

void init_ppm(uint8_t module);
void disable_ppm(uint8_t module);
void init_pxx2(uint8_t module);
void disable_pxx2(uint8_t module);
void init_pxx1_pulses(uint8_t module);
void init_pxx1_serial(uint8_t module);
void disable_pxx1_pulses(uint8_t module);
void disable_pxx1_serial(uint8_t module);
void disable_serial(uint8_t module);
void intmoduleStop();
void intmodulePxxStart();
void intmoduleSerialStart(uint32_t baudrate, uint8_t rxEnable);
#if defined(INTERNAL_MODULE_PPM)
void intmodulePpmStart(void);
#endif
void intmoduleSendBuffer(const uint8_t * data, uint8_t size);
void intmoduleSendNextFrame();

void extmoduleStop();
void extmodulePpmStart();
void extmodulePxxPulsesStart();
void extmodulePxxSerialStart();
void extmodulePxx2Start();
void extmoduleSerialStart(uint32_t baudrate, uint32_t period_half_us, bool inverted);
void extmoduleInvertedSerialStart(uint32_t baudrate);
void extmoduleSendBuffer(const uint8_t * data, uint8_t size);
void extmoduleSendNextFrame();

// Trainer driver
#define SLAVE_MODE()                    (g_model.trainerData.mode == TRAINER_MODE_SLAVE)
#if defined(PCBX9E)
  #define TRAINER_CONNECTED()           (true)
#elif defined(PCBX7) || defined(PCBX9LITE)
  #define TRAINER_CONNECTED()           (GPIO_ReadInputDataBit(TRAINER_DETECT_GPIO, TRAINER_DETECT_GPIO_PIN) == Bit_SET)
#elif defined(PCBXLITES)
  enum JackState
  {
    SPEAKER_ACTIVE,
    HEADPHONE_ACTIVE,
    TRAINER_ACTIVE,
  };
  extern uint8_t jackState;
  #define TRAINER_CONNECTED()           (jackState == TRAINER_ACTIVE)
#elif defined(PCBXLITE)
  #define TRAINER_CONNECTED()           false // there is no Trainer jack on Taranis X-Lite
#else
  #define TRAINER_CONNECTED()           (GPIO_ReadInputDataBit(TRAINER_DETECT_GPIO, TRAINER_DETECT_GPIO_PIN) == Bit_RESET)
#endif
#if defined(TRAINER_GPIO)
  void init_trainer_ppm(void);
  void stop_trainer_ppm(void);
  void init_trainer_capture(void);
  void stop_trainer_capture(void);
#else
  #define init_trainer_ppm()
  #define stop_trainer_ppm()
  #define init_trainer_capture()
  #define stop_trainer_capture()
#endif
#if defined(TRAINER_MODULE_HEARTBEAT)
  void init_cppm_on_heartbeat_capture(void);
  void stop_cppm_on_heartbeat_capture(void);
  void init_sbus_on_heartbeat_capture(void);
  void stop_sbus_on_heartbeat_capture(void);
#else
  #define init_cppm_on_heartbeat_capture()
  #define stop_cppm_on_heartbeat_capture()
  #define init_sbus_on_heartbeat_capture()
  #define stop_sbus_on_heartbeat_capture()
#endif

// SBUS
int sbusGetByte(uint8_t * byte);

// Keys driver
enum EnumKeys
{
#if defined(PCBXLITE)
  KEY_SHIFT,
#else
  KEY_MENU,
#endif
  KEY_EXIT,
  KEY_ENTER,
#if defined(PCBXLITE)
  KEY_DOWN,
  KEY_UP,
  KEY_RIGHT,
  KEY_LEFT,
#else
  KEY_PAGE,
  KEY_PLUS,
  KEY_MINUS,
#endif

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

  NUM_KEYS
};

#if defined(PCBX9E) && !defined(SIMU)
  #define KEY_UP                        KEY_MINUS
  #define KEY_DOWN                      KEY_PLUS
  #define KEY_RIGHT                     KEY_PLUS
  #define KEY_LEFT                      KEY_MINUS
#elif defined(PCBXLITE)
  #define KEY_PLUS                      KEY_RIGHT
  #define KEY_MINUS                     KEY_LEFT
#else
  #define KEY_UP                        KEY_PLUS
  #define KEY_DOWN                      KEY_MINUS
  #define KEY_RIGHT                     KEY_MINUS
  #define KEY_LEFT                      KEY_PLUS
#endif

#if defined(KEYS_GPIO_PIN_SHIFT)
#define IS_SHIFT_KEY(index)             (index == KEY_SHIFT)
#define IS_SHIFT_PRESSED()              (~KEYS_GPIO_REG_SHIFT & KEYS_GPIO_PIN_SHIFT)
#else
#define IS_SHIFT_KEY(index)             (false)
#define IS_SHIFT_PRESSED()              (false)
#endif

enum EnumSwitches
{
  SW_SA,
  SW_SB,
  SW_SC,
  SW_SD,
  SW_SE,
  SW_SF,
  SW_SG,
  SW_SH
};
#define IS_3POS(x)                      ((x) != SW_SF && (x) != SW_SH)

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
#if !defined(PCBX9LITE)
  SW_SD0,
  SW_SD1,
  SW_SD2,
#endif
#if defined(PCBX9) || defined(PCBXLITES)
  SW_SE0,
  SW_SE1,
  SW_SE2,
#endif
#if defined(PCBX9) || defined(PCBX7) || defined(PCBXLITES)
  SW_SF0,
  SW_SF1,
  SW_SF2,
#endif
#if defined(PCBX9D) || defined(PCBX9DP) || defined(PCBX9E)
  SW_SG0,
  SW_SG1,
  SW_SG2,
#endif
#if defined(PCBX9D) || defined(PCBX9DP) || defined(PCBX9E) || defined(PCBX7)
  SW_SH0,
  SW_SH1,
  SW_SH2,
#endif
#if defined(PCBX7)
  SW_SI0,
  SW_SI1,
  SW_SI2,
  SW_SJ0,
  SW_SJ1,
  SW_SJ2,
#endif
#if defined(PCBX9E)
  SW_SI0,
  SW_SI1,
  SW_SI2,
  SW_SJ0,
  SW_SJ1,
  SW_SJ2,
  SW_SK0,
  SW_SK1,
  SW_SK2,
  SW_SL0,
  SW_SL1,
  SW_SL2,
  SW_SM0,
  SW_SM1,
  SW_SM2,
  SW_SN0,
  SW_SN1,
  SW_SN2,
  SW_SO0,
  SW_SO1,
  SW_SO2,
  SW_SP0,
  SW_SP1,
  SW_SP2,
  SW_SQ0,
  SW_SQ1,
  SW_SQ2,
  SW_SR0,
  SW_SR1,
  SW_SR2,
#endif
  NUM_SWITCHES_POSITIONS
};
#if defined(PCBXLITES)
  #define NUM_SWITCHES                  6
#elif defined(PCBXLITE)
  #define NUM_SWITCHES                  4
#elif defined(PCBX7)
  #define NUM_SWITCHES                  8
#elif defined(PCBX9LITE)
  #define NUM_SWITCHES                  5
#elif defined(PCBX9E)
  #define NUM_SWITCHES                  18 // yes, it's a lot!
#else
  #define NUM_SWITCHES                  8
#endif

#if defined(__cplusplus)
static_assert(NUM_SWITCHES_POSITIONS == NUM_SWITCHES * 3, "Wrong switches positions count");
#endif

void keysInit(void);
uint8_t keyState(uint8_t index);
uint32_t switchState(uint8_t index);
uint32_t readKeys(void);
uint32_t readTrims(void);
#define TRIMS_PRESSED()                 (readTrims())
#define KEYS_PRESSED()                  (readKeys())

#if defined(PCBX9E) || defined(PCBX7) || defined(PCBX9LITE)
// Rotary Encoder driver
#define ROTARY_ENCODER_NAVIGATION
void rotaryEncoderInit(void);
void rotaryEncoderCheck(void);
#endif

// WDT driver
#define WDTO_500MS                      500
#if defined(WATCHDOG_DISABLED) || defined(SIMU)
  #define wdt_enable(x)
  #define wdt_reset()
#else
  #define wdt_enable(x)                 watchdogInit(x)
  #define wdt_reset()                   IWDG->KR = 0xAAAA
#endif
#define wdt_disable()
void watchdogInit(unsigned int duration);
#define WAS_RESET_BY_SOFTWARE()             (RCC->CSR & RCC_CSR_SFTRSTF)
#define WAS_RESET_BY_WATCHDOG()             (RCC->CSR & (RCC_CSR_WDGRSTF | RCC_CSR_WWDGRSTF))
#define WAS_RESET_BY_WATCHDOG_OR_SOFTWARE() (RCC->CSR & (RCC_CSR_WDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_SFTRSTF))

// ADC driver
enum Analogs {
  STICK1,
  STICK2,
  STICK3,
  STICK4,
  POT_FIRST,
  POT1 = POT_FIRST,
#if defined(PCBX9LITE)
  POT_LAST = POT1,
#elif defined(PCBXLITE) || defined(PCBX7)
  POT2,
  POT_LAST = POT2,
#elif defined(PCBX9E)
  POT2,
  POT3,
  POT4,
  POT_LAST = POT4,
  SLIDER1,
  SLIDER2,
  SLIDER3,
  SLIDER4,
#else
  POT2,
  POT3,
  POT_LAST = POT3,
  SLIDER1,
  SLIDER2,
#endif
  TX_VOLTAGE,
  NUM_ANALOGS,
  TX_RTC = NUM_ANALOGS
};

#define NUM_POTS                        (POT_LAST-POT_FIRST+1)
#define NUM_XPOTS                       NUM_POTS
#define STORAGE_NUM_POTS                NUM_POTS
#define NUM_SLIDERS                     (TX_VOLTAGE-POT_LAST-1)
#define STORAGE_NUM_SLIDERS             NUM_SLIDERS
#define NUM_TRIMS                       4
#define NUM_MOUSE_ANALOGS               0
#define STORAGE_NUM_MOUSE_ANALOGS       0

#if defined(STICKS_PWM)
  #define NUM_PWMSTICKS                 4
  #define STICKS_PWM_ENABLED()          (!hardwareOptions.sticksPwmDisabled)
  void sticksPwmInit(void);
  void sticksPwmRead(uint16_t * values);
  extern volatile uint32_t pwm_interrupt_count; // TODO => reusable buffer (boot section)
  #define NUM_TRIMS_KEYS                4
#else
  #define NUM_TRIMS_KEYS                8
  #define STICKS_PWM_ENABLED()          false
#endif

#if NUM_PWMSTICKS > 0
PACK(typedef struct {
  uint8_t sticksPwmDisabled:1;
  uint8_t pxx2Enabled:1;
}) HardwareOptions;
#else
PACK(typedef struct {
  uint8_t pxx2Enabled:1;
}) HardwareOptions;
#endif

extern HardwareOptions hardwareOptions;

#if !defined(PXX2)
  #define IS_PXX2_INTERNAL_ENABLED()            (false)
  #define IS_PXX1_INTERNAL_ENABLED()            (true)
#elif !defined(PXX1) || defined(PCBXLITES) || defined(PCBX9LITE)
  #define IS_PXX2_INTERNAL_ENABLED()            (true)
  #define IS_PXX1_INTERNAL_ENABLED()            (false)
#else
  // TODO #define PXX2_PROBE
  // TODO #define IS_PXX2_INTERNAL_ENABLED()            (hardwareOptions.pxx2Enabled)
  #define IS_PXX2_INTERNAL_ENABLED()            (true)
  #define IS_PXX1_INTERNAL_ENABLED()            (true)
#endif

enum CalibratedAnalogs {
  CALIBRATED_STICK1,
  CALIBRATED_STICK2,
  CALIBRATED_STICK3,
  CALIBRATED_STICK4,
  CALIBRATED_POT_FIRST,
  CALIBRATED_POT_LAST = CALIBRATED_POT_FIRST + NUM_POTS - 1,
  CALIBRATED_SLIDER_FIRST,
  CALIBRATED_SLIDER_LAST = CALIBRATED_SLIDER_FIRST + NUM_SLIDERS - 1,
  NUM_CALIBRATED_ANALOGS
};

#if defined(PCBX9D)
  #define IS_POT(x)                     ((x)>=POT_FIRST && (x)<=POT2) // POT3 is only defined in software
#else
  #define IS_POT(x)                     ((x)>=POT_FIRST && (x)<=POT_LAST)
#endif

#define IS_SLIDER(x)                    ((x)>POT_LAST && (x)<TX_VOLTAGE)

void adcInit(void);
void adcRead(void);
extern uint16_t adcValues[NUM_ANALOGS + 1/*RTC*/];
uint16_t getAnalogValue(uint8_t index);
uint16_t getRTCBattVoltage();

// Battery driver
uint16_t getBatteryVoltage();   // returns current battery voltage in 10mV steps
#if defined(PCBX9E)
  // NI-MH 9.6V
  #define BATTERY_WARN                  87 // 8.7V
  #define BATTERY_MIN                   85 // 8.5V
  #define BATTERY_MAX                   115 // 11.5V
#elif defined(PCBXLITE)
  // 2 x Li-Ion
  #define BATTERY_WARN                  66 // 6.6V
  #define BATTERY_MIN                   67 // 6.7V
  #define BATTERY_MAX                   83 // 8.3V
#else
  // NI-MH 7.2V
  #define BATTERY_WARN                  65 // 6.5V
  #define BATTERY_MIN                   60 // 6.0V
  #define BATTERY_MAX                   80 // 8.0V
#endif
#if defined(PCBXLITE)
  #define BATT_SCALE                    131
#elif defined(PCBX7)
  #define BATT_SCALE                    123
#elif defined(PCBX9LITE)
  #define BATT_SCALE                    117
#else
  #define BATT_SCALE                    150
#endif

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

// Power driver
#define SOFT_PWR_CTRL
void pwrInit(void);
uint32_t pwrCheck(void);
void pwrOn(void);
void pwrOff(void);
uint32_t pwrPressed(void);
#if defined(PWR_BUTTON_PRESS)
uint32_t pwrPressedDuration(void);
#endif
void pwrResetHandler(void);

#if defined(SIMU)
#define UNEXPECTED_SHUTDOWN()           false
#else
#define UNEXPECTED_SHUTDOWN()           (WAS_RESET_BY_WATCHDOG() || g_eeGeneral.unexpectedShutdown)
#endif

// Backlight driver
void backlightInit(void);
void backlightDisable(void);
#define BACKLIGHT_DISABLE()             backlightDisable()
uint8_t isBacklightEnabled(void);
#if defined(PCBX9E) || defined(PCBX9DP)
  void backlightEnable(uint8_t level, uint8_t color);
  #define BACKLIGHT_ENABLE()            backlightEnable(g_eeGeneral.backlightBright, g_eeGeneral.backlightColor)
#else
  void backlightEnable(uint8_t level);
  #define BACKLIGHT_ENABLE()            backlightEnable(g_eeGeneral.backlightBright)
#endif

#if !defined(SIMU)
  void usbJoystickUpdate();
#endif
#define USB_NAME                        "FrSky Taranis"
#define USB_MANUFACTURER                'F', 'r', 'S', 'k', 'y', ' ', ' ', ' '  /* 8 bytes */
#define USB_PRODUCT                     'T', 'a', 'r', 'a', 'n', 'i', 's', ' '  /* 8 Bytes */

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

// I2C driver: EEPROM + Audio Volume
#if defined(REV4a)
  #define EEPROM_SIZE                   (64*1024)
#else
  #define EEPROM_SIZE                   (32*1024)
#endif

void i2cInit(void);
void eepromReadBlock(uint8_t * buffer, size_t address, size_t size);
void eepromStartWrite(uint8_t * buffer, size_t address, size_t size);
uint8_t eepromIsTransferComplete();

// Debug driver
void debugPutc(const char c);

// Telemetry driver
void telemetryPortInit(uint32_t baudrate, uint8_t mode);
void telemetryPortSetDirectionOutput(void);
void sportSendBuffer(const uint8_t * buffer, uint32_t count);
uint8_t telemetryGetByte(uint8_t * byte);
extern uint32_t telemetryErrors;

// PCBREV driver
#if defined(PCBXLITE) || defined(PCBX9LITE)
  #define HAS_SPORT_UPDATE_CONNECTOR()  true
#elif defined(PCBX7)
  #define IS_PCBREV_40()                (GPIO_ReadInputDataBit(PCBREV_GPIO, PCBREV_GPIO_PIN) == Bit_SET)
  #define HAS_SPORT_UPDATE_CONNECTOR()  IS_PCBREV_40()
#else
  #define HAS_SPORT_UPDATE_CONNECTOR()  false
#endif

// Sport update driver
#if defined(SPORT_UPDATE_PWR_GPIO)
void sportUpdateInit(void);
void sportUpdatePowerOn(void);
void sportUpdatePowerOff(void);
#define SPORT_UPDATE_POWER_ON()         sportUpdatePowerOn()
#define SPORT_UPDATE_POWER_OFF()        sportUpdatePowerOff()
#else
#define sportUpdateInit()
#define SPORT_UPDATE_POWER_ON()
#define SPORT_UPDATE_POWER_OFF()
#endif

// Audio driver
void audioInit(void) ;
void audioEnd(void) ;
void dacStart(void);
void dacStop(void);
void setSampleRate(uint32_t frequency);
#define VOLUME_LEVEL_MAX  23
#define VOLUME_LEVEL_DEF  12
#if !defined(SOFTWARE_VOLUME)
void setScaledVolume(uint8_t volume);
void setVolume(uint8_t volume);
int32_t getVolume(void);
#endif
#if defined(AUDIO_SPEAKER_ENABLE_GPIO)
void initSpeakerEnable(void);
void enableSpeaker();
void disableSpeaker();
#else
static inline void initSpeakerEnable(void) { }
static inline void enableSpeaker(void) { }
static inline void disableSpeaker(void) { }
#endif
#if defined(HEADPHONE_TRAINER_SWITCH_GPIO)
void initHeadphoneTrainerSwitch(void);
void enableHeadphone(void);
void enableTrainer(void);
#else
static inline void initHeadphoneTrainerSwitch(void) { }
static inline void enableHeadphone(void) { }
static inline void enableTrainer(void) { }
#endif
#if defined(JACK_DETECT_GPIO)
void initJackDetect(void);
bool isJackPlugged();
#endif
void audioConsumeCurrentBuffer();
#define audioDisableIrq()               __disable_irq()
#define audioEnableIrq()                __enable_irq()

// Haptic driver
void hapticInit(void);
void hapticOff(void);
#if defined(HAPTIC_PWM)
  void hapticOn(uint32_t pwmPercent);
#else
  void hapticOn(void);
#endif

// Second serial port driver
#if defined(SERIAL_GPIO)
#define DEBUG_BAUDRATE                  115200
#define SERIAL2
extern uint8_t serial2Mode;
void serial2Init(unsigned int mode, unsigned int protocol);
void serial2Putc(char c);
#define serial2TelemetryInit(protocol) serial2Init(UART_MODE_TELEMETRY, protocol)
void serial2SbusInit(void);
void serial2Stop(void);
#endif

// BT driver
#define BLUETOOTH_BOOTLOADER_BAUDRATE   230400
#define BLUETOOTH_DEFAULT_BAUDRATE      115200
#if defined(PCBX9E) && !defined(USEHORUSBT)
#define BLUETOOTH_FACTORY_BAUDRATE      9600
#else
#define BLUETOOTH_FACTORY_BAUDRATE      57600
#endif
#define BT_TX_FIFO_SIZE    64
#define BT_RX_FIFO_SIZE    128
void bluetoothInit(uint32_t baudrate, bool enable);
void bluetoothWriteWakeup(void);
uint8_t bluetoothIsWriting(void);
void bluetoothDisable(void);
#if defined(PCBX9LITE)
  #define IS_BLUETOOTH_CHIP_PRESENT()     (false)
#elif (defined(PCBX7) || defined(PCBXLITE)) && !defined(SIMU)
  extern volatile uint8_t btChipPresent;
  #define IS_BLUETOOTH_CHIP_PRESENT()     (btChipPresent)
#else
  #define IS_BLUETOOTH_CHIP_PRESENT()     (true)
#endif

// LED driver
void ledInit(void);
void ledOff(void);
void ledRed(void);
void ledGreen(void);
void ledBlue(void);

// LCD driver
#if defined(STATUS_LEDS)
#define LCD_W                           128
#define LCD_H                           64
#define LCD_DEPTH                       1
#define IS_LCD_RESET_NEEDED()           true
#define LCD_CONTRAST_MIN                10
#define LCD_CONTRAST_MAX                30
#define LCD_CONTRAST_DEFAULT            20
#else
#define LCD_W                           212
#define LCD_H                           64
#define LCD_DEPTH                       4
#define IS_LCD_RESET_NEEDED()           (!WAS_RESET_BY_WATCHDOG_OR_SOFTWARE())
#define LCD_CONTRAST_MIN                0
#define LCD_CONTRAST_MAX                45
#define LCD_CONTRAST_DEFAULT            25
#endif
void lcdInit(void);
void lcdInitFinish(void);
void lcdOff(void);

// TODO lcdRefreshWait() stub in simpgmspace and remove LCD_DUAL_BUFFER
#if defined(LCD_DMA) && !defined(LCD_DUAL_BUFFER) && !defined(SIMU)
void lcdRefreshWait();
#else
#define lcdRefreshWait()
#endif
#if defined(PCBX9D) || defined(SIMU) || !defined(__cplusplus)
void lcdRefresh(void);
#else
void lcdRefresh(bool wait=true); // TODO uint8_t wait to simplify this
#endif
void lcdSetRefVolt(unsigned char val);
void lcdSetContrast(void);

// Top LCD driver
#if defined(TOPLCD_GPIO)
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

extern uint8_t currentTrainerMode;
void checkTrainerSettings(void);

#if defined(__cplusplus)
#include "fifo.h"
#include "dmafifo.h"

#if defined(CROSSFIRE)
#define TELEMETRY_FIFO_SIZE             128
#else
#define TELEMETRY_FIFO_SIZE             64
#endif

extern Fifo<uint8_t, TELEMETRY_FIFO_SIZE> telemetryFifo;
extern DMAFifo<32> serial2RxFifo;
#endif

// Gyro driver
#define GYRO_VALUES_COUNT               6
#define GYRO_BUFFER_LENGTH              (GYRO_VALUES_COUNT * sizeof(int16_t))
int gyroInit();
int gyroRead(uint8_t buffer[GYRO_BUFFER_LENGTH]);
#define GYRO_MAX_DEFAULT                30
#define GYRO_MAX_RANGE                  60
#define GYRO_OFFSET_MIN                 -30
#define GYRO_OFFSET_MAX                 10

#endif // _BOARD_H_
