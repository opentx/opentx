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

// Starts TIMER at 2MHz
void init2MhzTimer()
{
  TIMER_2MHz_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1 ;       // 0.5 uS, 2 MHz
  TIMER_2MHz_TIMER->ARR = 65535;
  TIMER_2MHz_TIMER->CR2 = 0;
  TIMER_2MHz_TIMER->CR1 = TIM_CR1_CEN;
}

// Starts TIMER at 200Hz (5ms)
void init5msTimer()
{
  INTERRUPT_xMS_TIMER->ARR = 4999 ; // 5mS in uS
  INTERRUPT_xMS_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 1000000 - 1 ; // 1uS
  INTERRUPT_xMS_TIMER->CCER = 0 ;
  INTERRUPT_xMS_TIMER->CCMR1 = 0 ;
  INTERRUPT_xMS_TIMER->EGR = 0 ;
  INTERRUPT_xMS_TIMER->CR1 = 5 ;
  INTERRUPT_xMS_TIMER->DIER |= 1 ;
  NVIC_EnableIRQ(INTERRUPT_xMS_IRQn) ;
  NVIC_SetPriority(INTERRUPT_xMS_IRQn, 7);
}

void stop5msTimer( void )
{
  INTERRUPT_xMS_TIMER->CR1 = 0 ;        // stop timer
  NVIC_DisableIRQ(INTERRUPT_xMS_IRQn) ;
}

// TODO use the same than board_sky9x.cpp
void interrupt5ms()
{
  static uint32_t pre_scale ;       // Used to get 10 Hz counter

  AUDIO_HEARTBEAT();

#if defined(HAPTIC)
  HAPTIC_HEARTBEAT();
#endif

  if (++pre_scale >= 2) {
    pre_scale = 0 ;
    DEBUG_TIMER_START(debugTimerPer10ms);
    DEBUG_TIMER_SAMPLE(debugTimerPer10msPeriod);
    per10ms();
    DEBUG_TIMER_STOP(debugTimerPer10ms);
  }

#if defined(ROTARY_ENCODER_NAVIGATION)
  checkRotaryEncoder();
#endif
}

#if !defined(SIMU)
extern "C" void INTERRUPT_xMS_IRQHandler()
{
  INTERRUPT_xMS_TIMER->SR &= ~TIM_SR_UIF ;
  interrupt5ms() ;
  DEBUG_INTERRUPT(INT_5MS);
}
#endif

#if defined(PWR_BUTTON_PRESS) && !defined(SIMU)
  #define PWR_PRESS_DURATION_MIN        100 // 1s
  #define PWR_PRESS_DURATION_MAX        500 // 5s
#endif

#if (defined(PCBX9E) && !defined(SIMU))
const pm_uchar bmp_startup[] PROGMEM = {
  #include "startup.lbm"
};
const pm_uchar bmp_lock[] PROGMEM = {
  #include "lock.lbm"
};
#endif

#if defined(SPORT_UPDATE_PWR_GPIO)
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
  GPIO_SPORT_UPDATE_PWR_GPIO_ON(SPORT_UPDATE_PWR_GPIO, SPORT_UPDATE_PWR_GPIO_PIN);
}

void sportUpdatePowerOff()
{
  GPIO_SPORT_UPDATE_PWR_GPIO_OFF(SPORT_UPDATE_PWR_GPIO, SPORT_UPDATE_PWR_GPIO_PIN);
}
#endif

void boardInit()
{
#if !defined(SIMU)
  RCC_AHB1PeriphClockCmd(PWR_RCC_AHB1Periph | PCBREV_RCC_AHB1Periph |
                         KEYS_RCC_AHB1Periph | LCD_RCC_AHB1Periph |
                         AUDIO_RCC_AHB1Periph | BACKLIGHT_RCC_AHB1Periph |
                         ADC_RCC_AHB1Periph | I2C_RCC_AHB1Periph |
                         SD_RCC_AHB1Periph | HAPTIC_RCC_AHB1Periph |
                         INTMODULE_RCC_AHB1Periph | EXTMODULE_RCC_AHB1Periph |
                         TELEMETRY_RCC_AHB1Periph | SPORT_UPDATE_RCC_AHB1Periph |
                         SERIAL_RCC_AHB1Periph | TRAINER_RCC_AHB1Periph |
                         HEARTBEAT_RCC_AHB1Periph | BT_RCC_AHB1Periph, ENABLE);

  RCC_APB1PeriphClockCmd(LCD_RCC_APB1Periph | AUDIO_RCC_APB1Periph | ADC_RCC_APB1Periph |
                         BACKLIGHT_RCC_APB1Periph | HAPTIC_RCC_APB1Periph | INTERRUPT_xMS_RCC_APB1Periph |
                         TIMER_2MHz_RCC_APB1Periph | I2C_RCC_APB1Periph |
                         SD_RCC_APB1Periph | TRAINER_RCC_APB1Periph |
                         TELEMETRY_RCC_APB1Periph | SERIAL_RCC_APB1Periph |
                         INTMODULE_RCC_APB1Periph | BT_RCC_APB1Periph, ENABLE);

  RCC_APB2PeriphClockCmd(BACKLIGHT_RCC_APB2Periph | ADC_RCC_APB2Periph |
                         HAPTIC_RCC_APB2Periph | INTMODULE_RCC_APB2Periph |
                         EXTMODULE_RCC_APB2Periph | HEARTBEAT_RCC_APB2Periph |
                         BT_RCC_APB2Periph, ENABLE);

#if !defined(PCBX9E)
  // some X9E boards need that the pwrInit() is moved a little bit later
  pwrInit();
#endif

#if defined(STATUS_LEDS)
  ledInit();
  ledGreen();
#endif

  keysInit();
  delaysInit();

#if NUM_PWMSTICKS > 0
  sticksPwmInit();
  delay_ms(20);
  if (pwm_interrupt_count < 32) {
    sticks_pwm_disabled = true;
  }
#endif

  adcInit();
  lcdInit(); // delaysInit() must be called before
  audioInit();
  init2MhzTimer();
  init5msTimer();
  __enable_irq();
  i2cInit();
  usbInit();

#if defined(DEBUG) && defined(SERIAL_GPIO)
  serial2Init(0, 0); // default serial mode (None if DEBUG not defined)
  TRACE("\nTaranis board started :)");
#endif

#if defined(HAPTIC)
  hapticInit();
#endif

#if defined(BLUETOOTH)
  bluetoothInit(BLUETOOTH_DEFAULT_BAUDRATE);
#endif

#if defined(DEBUG)
  DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP|DBGMCU_TIM1_STOP|DBGMCU_TIM2_STOP|DBGMCU_TIM3_STOP|DBGMCU_TIM6_STOP|DBGMCU_TIM8_STOP|DBGMCU_TIM10_STOP|DBGMCU_TIM13_STOP|DBGMCU_TIM14_STOP, ENABLE);
#endif

#if defined(PWR_BUTTON_PRESS)
  if (!WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()) {
    lcdClear();
#if defined(PCBX9E)
    lcdDrawBitmap(76, 2, bmp_lock, 0, 60);
#else
    lcdDrawFilledRect(LCD_W / 2 - 18, LCD_H / 2 - 3, 6, 6, SOLID, 0);
#endif
    lcdRefresh();
    lcdRefreshWait();

    tmr10ms_t start = get_tmr10ms();
    tmr10ms_t duration = 0;
    uint8_t pwr_on = 0;
    while (pwrPressed()) {
      duration = get_tmr10ms() - start;
      if (duration < PWR_PRESS_DURATION_MIN) {
        unsigned index = duration / (PWR_PRESS_DURATION_MIN / 4);
        lcdClear();
#if defined(PCBX9E)
        lcdDrawBitmap(76, 2, bmp_startup, index*60, 60);
#else
        for(uint8_t i= 0; i < 4; i++) {
          if (index >= i) {
            lcdDrawFilledRect(LCD_W / 2 - 18 + 10 * i, LCD_H / 2 - 3, 6, 6, SOLID, 0);
          }
        }
#endif
      }
      else if (duration >= PWR_PRESS_DURATION_MAX) {
        drawSleepBitmap();
        backlightDisable();
      }
      else {
        if (pwr_on != 1) {
          pwr_on = 1;
          pwrInit();
          backlightInit();
          haptic.play(15, 3, PLAY_NOW);
        }
      }
      lcdRefresh();
      lcdRefreshWait();
    }
    if (duration < PWR_PRESS_DURATION_MIN || duration >= PWR_PRESS_DURATION_MAX) {
      boardOff();
    }
  }
  else {
    pwrInit();
    backlightInit();
  }
#if defined(TOPLCD_GPIO)
  toplcdInit();
#endif
#else // defined(PWR_BUTTON_PRESS)
  backlightInit();
#endif

  if (HAS_SPORT_UPDATE_CONNECTOR()) {
    sportUpdateInit();
  }
#endif // !defined(SIMU)
}

void boardOff()
{
#if defined(STATUS_LEDS)
  ledOff();
#endif

  BACKLIGHT_DISABLE();

#if defined(TOPLCD_GPIO)
  toplcdOff();
#endif

#if defined(PWR_BUTTON_PRESS)
  while (pwrPressed()) {
    wdt_reset();
  }
#endif

  lcdOff();
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
      case TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE:
        stop_cppm_on_heartbeat_capture() ;
        break;
      case TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE:
        stop_sbus_on_heartbeat_capture() ;
        break;
#if defined(TRAINER_BATTERY_COMPARTMENT)
      case TRAINER_MODE_MASTER_BATTERY_COMPARTMENT:
        serial2Stop();
#endif
    }

    currentTrainerMode = requiredTrainerMode;
    switch (requiredTrainerMode) {
      case TRAINER_MODE_SLAVE:
        init_trainer_ppm();
        break;
      case TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE:
         init_cppm_on_heartbeat_capture();
         break;
      case TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE:
         init_sbus_on_heartbeat_capture();
         break;
#if defined(TRAINER_BATTERY_COMPARTMENT)
      case TRAINER_MODE_MASTER_BATTERY_COMPARTMENT:
        if (g_eeGeneral.serial2Mode == UART_MODE_SBUS_TRAINER) {
          serial2SbusInit();
          break;
        }
        // no break
#endif
      default:
        // master is default
        init_trainer_capture();
        break;
    }
  }
}

uint16_t getBatteryVoltage()
{
  int32_t instant_vbat = anaIn(TX_VOLTAGE); // using filtered ADC value on purpose
  instant_vbat = (instant_vbat * BATT_SCALE * (128 + g_eeGeneral.txVoltageCalibration) ) / 26214;
  instant_vbat += 20; // add 0.2V because of the diode TODO check if this is needed, but removal will beak existing calibrations!!!
  return (uint16_t)instant_vbat;
}
