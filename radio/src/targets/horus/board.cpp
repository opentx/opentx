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

void watchdogInit(unsigned int duration)
{
  IWDG->KR = 0x5555;      // Unlock registers
  IWDG->PR = 3;           // Divide by 32 => 1kHz clock
  IWDG->KR = 0x5555;      // Unlock registers
  IWDG->RLR = duration;       // 1.5 seconds nominal
  IWDG->KR = 0xAAAA;      // reload
  IWDG->KR = 0xCCCC;      // start
}

// Start TIMER7 at 2000000Hz
void init2MhzTimer()
{
  TIMER_2MHz_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1; // 0.5 uS, 2 MHz
  TIMER_2MHz_TIMER->ARR = 65535;
  TIMER_2MHz_TIMER->CR2 = 0;
  TIMER_2MHz_TIMER->CR1 = TIM_CR1_CEN;
}

// Starts TIMER at 1000Hz
void init1msTimer()
{
  INTERRUPT_xMS_TIMER->ARR = 999; // 1mS in uS
  INTERRUPT_xMS_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 1000000 - 1;  // 1uS
  INTERRUPT_xMS_TIMER->CCER = 0;
  INTERRUPT_xMS_TIMER->CCMR1 = 0;
  INTERRUPT_xMS_TIMER->EGR = 0;
  INTERRUPT_xMS_TIMER->CR1 = 5;
  INTERRUPT_xMS_TIMER->DIER |= 1;
  NVIC_EnableIRQ(INTERRUPT_xMS_IRQn);
  NVIC_SetPriority(INTERRUPT_xMS_IRQn, 7);
}

// TODO use the same than board_sky9x.cpp
void interrupt1ms()
{
  static uint8_t pre_scale;       // Used to get 10 Hz counter

  ++pre_scale;

#if defined(HAPTIC)
  if (pre_scale == 5 || pre_scale == 10) {
    DEBUG_TIMER_START(debugTimerHaptic);
    HAPTIC_HEARTBEAT();
    DEBUG_TIMER_STOP(debugTimerHaptic);
  }
#endif

  if (pre_scale == 10) {
    pre_scale = 0;
    DEBUG_TIMER_START(debugTimerPer10ms);
    DEBUG_TIMER_SAMPLE(debugTimerPer10msPeriod);
    per10ms();
    DEBUG_TIMER_STOP(debugTimerPer10ms);
  }

  DEBUG_TIMER_START(debugTimerRotEnc);
  checkRotaryEncoder();
  DEBUG_TIMER_STOP(debugTimerRotEnc);
}

extern "C" void INTERRUPT_xMS_IRQHandler()
{
  INTERRUPT_xMS_TIMER->SR &= ~TIM_SR_UIF;
  interrupt1ms();
  DEBUG_INTERRUPT(INT_1MS);
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
                         SERIAL_RCC_AHB1Periph |
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

  RCC_APB1PeriphClockCmd(INTERRUPT_xMS_RCC_APB1Periph |
                         ADC_RCC_APB1Periph |
                         TIMER_2MHz_RCC_APB1Periph |
                         AUDIO_RCC_APB1Periph |
                         SERIAL_RCC_APB1Periph |
                         TELEMETRY_RCC_APB1Periph |
                         TRAINER_RCC_APB1Periph |
                         AUDIO_RCC_APB1Periph |
                         INTMODULE_RCC_APB1Periph |
                         EXTMODULE_RCC_APB1Periph |
                         GPS_RCC_APB1Periph |
                         BACKLIGHT_RCC_APB1Periph,
                         ENABLE);

  RCC_APB2PeriphClockCmd(LCD_RCC_APB2Periph |
                         ADC_RCC_APB2Periph |
                         HAPTIC_RCC_APB2Periph |
                         INTMODULE_RCC_APB2Periph |
                         EXTMODULE_RCC_APB2Periph |
                         BT_RCC_APB2Periph |
                         BACKLIGHT_RCC_APB2Periph,
                         ENABLE);

  pwrInit();
  delaysInit();

  // FrSky removed the volume chip in latest board, that's why it doesn't answer!
  // i2cInit();

#if defined(DEBUG)
  serial2Init(0, 0); // default serial mode (None if DEBUG not defined)
#endif

  __enable_irq();

  TRACE("\nHorus board started :)");
  TRACE("RCC->CSR = %08x", RCC->CSR);

  audioInit();

  // we need to initialize g_FATFS_Obj here, because it is in .ram section (because of DMA access) 
  // and this section is un-initialized
  memset(&g_FATFS_Obj, 0, sizeof(g_FATFS_Obj));

  keysInit();

#if NUM_PWMSTICKS > 0
  sticksPwmInit();
  delay_ms(20);
  if (pwm_interrupt_count < 32) {
    sticks_pwm_disabled = true;
  }
#endif

  adcInit();
  lcdInit();
  backlightInit();

  init2MhzTimer();
  init1msTimer();
  usbInit();
  hapticInit();

#if defined(BLUETOOTH)
  bluetoothInit(BLUETOOTH_DEFAULT_BAUDRATE);
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
#endif
}

void boardOff()
{
  BACKLIGHT_DISABLE();

  while (pwrPressed()) {
    wdt_reset();
  }

  SysTick->CTRL = 0; // turn off systick
  pwrOff();
}

uint8_t currentTrainerMode = 0xff;

void checkTrainerSettings()
{
  uint8_t requiredTrainerMode = g_model.trainerMode;
  if (requiredTrainerMode != currentTrainerMode) {
    switch (currentTrainerMode) {
      case TRAINER_MODE_MASTER_TRAINER_JACK:
        stop_trainer_capture();
        break;
      case TRAINER_MODE_SLAVE:
        stop_trainer_ppm();
        break;
      case TRAINER_MODE_MASTER_BATTERY_COMPARTMENT:
        serial2Stop();
    }

    currentTrainerMode = requiredTrainerMode;
    switch (requiredTrainerMode) {
      case TRAINER_MODE_SLAVE:
        init_trainer_ppm();
        break;
      case TRAINER_MODE_MASTER_BATTERY_COMPARTMENT:
        if (g_eeGeneral.serial2Mode == UART_MODE_SBUS_TRAINER) {
          serial2SbusInit();
          break;
        }
        // no break
      default:
        // master is default
        init_trainer_capture();
        break;
    }
  }
}

uint16_t getBatteryVoltage()
{
  int32_t instant_vbat = anaIn(TX_VOLTAGE);  // using filtered ADC value on purpose
  return (uint16_t)((instant_vbat * (1000 + g_eeGeneral.txVoltageCalibration)) / 1629);
}
