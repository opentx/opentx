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

#include "opentx.h"

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif
#include "usb_dcd_int.h"
#include "usb_bsp.h"
#if defined(__cplusplus) && !defined(SIMU)
}
#endif

uint32_t shutdownRequest;          // Stores intentional shutdown to avoid reboot loop
uint32_t shutdownReason;           // Used for detecting unexpected reboots regardless of reason
uint32_t powerupReason __NOINIT;   // Stores power up reason beyond initialization for emergency mode activation

HardwareOptions hardwareOptions;

void watchdogInit(unsigned int duration)
{
  IWDG->KR = 0x5555;      // Unlock registers
  IWDG->PR = 3;           // Divide by 32 => 1kHz clock
  IWDG->KR = 0x5555;      // Unlock registers
  IWDG->RLR = duration;       // 1.5 seconds nominal
  IWDG->KR = 0xAAAA;      // reload
  IWDG->KR = 0xCCCC;      // start
}

#if defined(SEMIHOSTING)
extern "C" void initialise_monitor_handles();
#endif

#if defined(PCBX10)
void sportUpdateInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = SPORT_UPDATE_PWR_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(SPORT_UPDATE_PWR_GPIO, &GPIO_InitStructure);
}

void sportUpdatePowerOn()
{
  GPIO_SetBits(SPORT_UPDATE_PWR_GPIO, SPORT_UPDATE_PWR_GPIO_PIN);
}

void sportUpdatePowerOff()
{
  GPIO_ResetBits(SPORT_UPDATE_PWR_GPIO, SPORT_UPDATE_PWR_GPIO_PIN);
}
#endif

void boardInit()
{
#if defined(SEMIHOSTING)
  initialise_monitor_handles();
#endif

#if !defined(SIMU)
  RCC_AHB1PeriphClockCmd(PWR_RCC_AHB1Periph |
                         PCBREV_RCC_AHB1Periph |
                         LED_RCC_AHB1Periph |
                         LCD_RCC_AHB1Periph |
                         BACKLIGHT_RCC_AHB1Periph |
                         SD_RCC_AHB1Periph |
                         AUDIO_RCC_AHB1Periph |
                         KEYS_RCC_AHB1Periph |
                         ADC_RCC_AHB1Periph |
                         AUX_SERIAL_RCC_AHB1Periph |
                         TELEMETRY_RCC_AHB1Periph |
                         TRAINER_RCC_AHB1Periph |
                         BT_RCC_AHB1Periph |
                         AUDIO_RCC_AHB1Periph |
                         HAPTIC_RCC_AHB1Periph |
                         INTMODULE_RCC_AHB1Periph |
                         EXTMODULE_RCC_AHB1Periph |
                         GPS_RCC_AHB1Periph |
                         SPORT_UPDATE_RCC_AHB1Periph,
                         ENABLE);

  RCC_APB1PeriphClockCmd(ROTARY_ENCODER_RCC_APB1Periph |
                         INTERRUPT_xMS_RCC_APB1Periph |
                         ADC_RCC_APB1Periph |
                         TIMER_2MHz_RCC_APB1Periph |
                         AUDIO_RCC_APB1Periph |
                         AUX_SERIAL_RCC_APB1Periph |
                         TELEMETRY_RCC_APB1Periph |
                         TRAINER_RCC_APB1Periph |
                         AUDIO_RCC_APB1Periph |
                         INTMODULE_RCC_APB1Periph |
                         EXTMODULE_RCC_APB1Periph |
                         GPS_RCC_APB1Periph |
                         BACKLIGHT_RCC_APB1Periph,
                         ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG |
                         LCD_RCC_APB2Periph |
                         ADC_RCC_APB2Periph |
                         HAPTIC_RCC_APB2Periph |
                         INTMODULE_RCC_APB2Periph |
                         EXTMODULE_RCC_APB2Periph |
                         BT_RCC_APB2Periph |
                         BACKLIGHT_RCC_APB2Periph,
                         ENABLE);

  pwrInit();
  pwrOn();
  delaysInit();

#if defined(DEBUG)
  auxSerialInit(0, 0); // default serial mode (None if DEBUG not defined)
#endif

  __enable_irq();

  TRACE("\nHorus board started :)");
  TRACE("RCC->CSR = %08x", RCC->CSR);

  audioInit();

  // we need to initialize g_FATFS_Obj here, because it is in .ram section (because of DMA access)
  // and this section is un-initialized
  memset(&g_FATFS_Obj, 0, sizeof(g_FATFS_Obj));

  keysInit();
  rotaryEncoderInit();

#if NUM_PWMSTICKS > 0
  sticksPwmInit();
  delay_ms(20);
  if (pwm_interrupt_count < 32) {
    hardwareOptions.sticksPwmDisabled = true;
  }
#endif

  adcInit();
  lcdInit();
  backlightInit();

  init2MhzTimer();
  init5msTimer();
  usbInit();
  hapticInit();

#if defined(BLUETOOTH)
  bluetoothInit(BLUETOOTH_DEFAULT_BAUDRATE, true);
#endif

#if defined(INTERNAL_GPS)
  gpsInit(GPS_USART_BAUDRATE);
#endif

#if defined(DEBUG)
  DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP|DBGMCU_TIM1_STOP|DBGMCU_TIM2_STOP|DBGMCU_TIM3_STOP|DBGMCU_TIM4_STOP|DBGMCU_TIM5_STOP|DBGMCU_TIM6_STOP|DBGMCU_TIM7_STOP|DBGMCU_TIM8_STOP|DBGMCU_TIM9_STOP|DBGMCU_TIM10_STOP|DBGMCU_TIM11_STOP|DBGMCU_TIM12_STOP|DBGMCU_TIM13_STOP|DBGMCU_TIM14_STOP, ENABLE);
#endif

#if defined(PCBX10)
  ledInit();
  sportUpdateInit();
#endif

  ledBlue();

  vbattRTC = getRTCBattVoltage();
#endif
}

void boardOff()
{
  BACKLIGHT_DISABLE();

  while (pwrPressed()) {
    wdt_reset();
  }

  SysTick->CTRL = 0; // turn off systick

#if defined(PCBX12S)
  // Shutdown the Audio amp
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = AUDIO_SHUTDOWN_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(AUDIO_SHUTDOWN_GPIO, &GPIO_InitStructure);
  GPIO_ResetBits(AUDIO_SHUTDOWN_GPIO, AUDIO_SHUTDOWN_GPIO_PIN);
#endif

  // Shutdown the Haptic
  hapticDone();

  shutdownRequest = SHUTDOWN_REQUEST;
  shutdownReason = NORMAL_POWER_OFF;

  pwrOff();
}

uint16_t getBatteryVoltage()
{
  int32_t instant_vbat = anaIn(TX_VOLTAGE);  // using filtered ADC value on purpose
  return (uint16_t)((instant_vbat * (1000 + g_eeGeneral.txVoltageCalibration)) / 1629);
}
