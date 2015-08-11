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

#include "../../opentx.h"

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif
#include "STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/inc/usb_dcd_int.h"
#include "STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/inc/usb_bsp.h"
#if defined(__cplusplus) && !defined(SIMU)
}
#endif

void watchdogInit(unsigned int duration)
{
  IWDG->KR = 0x5555 ;      // Unlock registers
  IWDG->PR = 3 ;           // Divide by 32 => 1kHz clock
  IWDG->KR = 0x5555 ;      // Unlock registers
  IWDG->RLR = duration ;       // 1.5 seconds nominal
  IWDG->KR = 0xAAAA ;      // reload
  IWDG->KR = 0xCCCC ;      // start
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
  INTERRUPT_5MS_TIMER->ARR = 4999 ;     // 5mS
  INTERRUPT_5MS_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 1000000 - 1 ;                // 1uS from 30MHz
  INTERRUPT_5MS_TIMER->CCER = 0 ;
  INTERRUPT_5MS_TIMER->CCMR1 = 0 ;
  INTERRUPT_5MS_TIMER->EGR = 0 ;
  INTERRUPT_5MS_TIMER->CR1 = 5 ;
  INTERRUPT_5MS_TIMER->DIER |= 1 ;
  NVIC_EnableIRQ(INTERRUPT_5MS_IRQn) ;
  NVIC_SetPriority(INTERRUPT_5MS_IRQn, 7);
}

void stop5msTimer( void )
{
  INTERRUPT_5MS_TIMER->CR1 = 0 ;        // stop timer
  NVIC_DisableIRQ(INTERRUPT_5MS_IRQn) ;
}

// TODO use the same than board_sky9x.cpp
void interrupt5ms()
{
  static uint32_t pre_scale ;       // Used to get 10 Hz counter

  AUDIO_HEARTBEAT();

#if defined(HAPTIC)
  HAPTIC_HEARTBEAT();
#endif

  if ( ++pre_scale >= 2 ) {
    pre_scale = 0 ;
    per10ms();
  }

#if defined(REV9E)
  checkRotaryEncoder();
#endif
}

#if !defined(SIMU)
extern "C" void INTERRUPT_5MS_IRQHandler()
{
  INTERRUPT_5MS_TIMER->SR &= ~TIM_SR_UIF ;
  interrupt5ms() ;
}

#if defined(REV9E)
  #define PWR_PRESS_DURATION_MIN       200 // 2s
  #define PWR_PRESS_DURATION_MAX       500 // 5s

  const pm_uchar bmp_startup[] PROGMEM = {
    #include "../../bitmaps/Taranis/startup.lbm"
  };

  const pm_uchar bmp_lock[] PROGMEM = {
    #include "../../bitmaps/Taranis/lock.lbm"
  };
#endif

void boardInit()
{
  RCC_AHB1PeriphClockCmd(PWR_RCC_AHB1Periph | KEYS_RCC_AHB1Periph | LCD_RCC_AHB1Periph | BACKLIGHT_RCC_AHB1Periph | ADC_RCC_AHB1Periph | I2C_RCC_AHB1Periph | SD_RCC_AHB1Periph | HAPTIC_RCC_AHB1Periph | INTMODULE_RCC_AHB1Periph | EXTMODULE_RCC_AHB1Periph | TELEMETRY_RCC_AHB1Periph | SERIAL_RCC_AHB1Periph | TRAINER_RCC_AHB1Periph | HEARTBEAT_RCC_AHB1Periph, ENABLE);
  RCC_APB1PeriphClockCmd(LCD_RCC_APB1Periph | BACKLIGHT_RCC_APB1Periph | INTERRUPT_5MS_APB1Periph | TIMER_2MHz_APB1Periph | I2C_RCC_APB1Periph | SD_RCC_APB1Periph | TRAINER_RCC_APB1Periph | TELEMETRY_RCC_APB1Periph | SERIAL_RCC_APB1Periph, ENABLE);
  RCC_APB2PeriphClockCmd(BACKLIGHT_RCC_APB2Periph | ADC_RCC_APB2Periph | HAPTIC_RCC_APB2Periph | INTMODULE_RCC_APB2Periph | EXTMODULE_RCC_APB2Periph | HEARTBEAT_RCC_APB2Periph, ENABLE);

  pwrInit();
  keysInit();
  adcInit();
  delaysInit();
  lcdInit();    // delaysInit() must be called before
  audioInit();
  init2MhzTimer();
  init5msTimer();
  __enable_irq();
  i2cInit();
  usbInit();
  
#if defined(HAPTIC)  
  hapticInit();
#endif

#if defined(REV9E)
  bluetoothInit(BLUETOOTH_DEFAULT_BAUDRATE);
#endif

#if defined(DEBUG)
  DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP|DBGMCU_TIM1_STOP|DBGMCU_TIM2_STOP|DBGMCU_TIM3_STOP|DBGMCU_TIM6_STOP|DBGMCU_TIM8_STOP|DBGMCU_TIM10_STOP|DBGMCU_TIM13_STOP|DBGMCU_TIM14_STOP, ENABLE);
#endif

#if defined(REV9E)
  if (!(RCC->CSR & (RCC_CSR_SFTRSTF | RCC_CSR_WDGRSTF))) {
    lcd_clear();
    lcd_bmp(76, 2, bmp_lock, 0, 60);
    lcdRefresh();
    lcdRefreshWait();

    tmr10ms_t start = get_tmr10ms();
    tmr10ms_t duration = 0;
    uint8_t pwr_on = 0;
    while (pwrPressed()) {
      duration = get_tmr10ms() - start;
      if (duration < PWR_PRESS_DURATION_MIN) {
        unsigned index = duration / (PWR_PRESS_DURATION_MIN / 4);
        lcd_clear();
        lcd_bmp(76, 2, bmp_startup, index*60, 60);
      }
      else if (duration >= PWR_PRESS_DURATION_MAX) {
        displaySleepBitmap();
        turnBacklightOff();
      }
      else {
        if (pwr_on != 1) {
          pwr_on = 1;
          backlightInit();
          haptic.play(15, 3, PLAY_NOW);
        }
      }
      lcdRefresh();
      lcdRefreshWait();
    }
    if (duration < PWR_PRESS_DURATION_MIN || duration >= PWR_PRESS_DURATION_MAX) {
      pwrOff();
    }
  }
  else {
    backlightInit();
  }
  topLcdInit();
#else
  backlightInit();
#endif
}
#endif


#if defined(USB_JOYSTICK) && !defined(SIMU)
extern USB_OTG_CORE_HANDLE USB_OTG_dev;

/*
  Prepare and send new USB data packet

  The format of HID_Buffer is defined by
  USB endpoint description can be found in 
  file usb_hid_joystick.c, variable HID_JOYSTICK_ReportDesc
*/
void usbJoystickUpdate(void)
{
  static uint8_t HID_Buffer[HID_IN_PACKET];
  
  //buttons
  HID_Buffer[0] = 0; //buttons
  for (int i = 0; i < 8; ++i) {
    if ( channelOutputs[i+8] > 0 ) {
      HID_Buffer[0] |= (1 << i);
    } 
  }

  //analog values
  //uint8_t * p = HID_Buffer + 1;
  for (int i = 0; i < 8; ++i) {
    int16_t value = channelOutputs[i] / 8;
    if ( value > 127 ) value = 127;
    else if ( value < -127 ) value = -127;
    HID_Buffer[i+1] = static_cast<int8_t>(value);  
  }

  USBD_HID_SendReport (&USB_OTG_dev, HID_Buffer, HID_IN_PACKET );
}

#endif //#if defined(USB_JOYSTICK) && defined(PCBTARANIS) && !defined(SIMU)


uint8_t currentTrainerMode = 0xff;

void checkTrainerSettings()
{
  uint8_t requiredTrainerMode = g_model.trainerMode;
  if (requiredTrainerMode != currentTrainerMode) {
    switch (currentTrainerMode) {
      case TRAINER_MODE_MASTER:
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
      case TRAINER_MODE_MASTER_BATTERY_COMPARTMENT:
        uart3Stop();
    }

    currentTrainerMode = requiredTrainerMode;
    switch (requiredTrainerMode) {
      case TRAINER_MODE_SLAVE:
        init_trainer_ppm();
        break;
      case TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE:
         init_cppm_on_heartbeat_capture() ;
         break;
      case TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE:
         init_sbus_on_heartbeat_capture() ;
         break;
      case TRAINER_MODE_MASTER_BATTERY_COMPARTMENT:
        if (g_eeGeneral.uart3Mode == UART_MODE_SBUS_TRAINER) {
          uart3SbusInit();
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
