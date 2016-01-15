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

// Start TIMER7 at 2000000Hz
void init2MhzTimer()
{
  TIMER_2MHz_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1 ;       // 0.5 uS, 2 MHz
  TIMER_2MHz_TIMER->ARR = 65535;
  TIMER_2MHz_TIMER->CR2 = 0;
  TIMER_2MHz_TIMER->CR1 = TIM_CR1_CEN;
}

// Starts TIMER at 200Hz, 5mS period
void init5msTimer()
{
  INTERRUPT_5MS_TIMER->ARR = 999 ;     // 5mS
  INTERRUPT_5MS_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 1000000 - 1 ;                // 1uS from 30MHz
  INTERRUPT_5MS_TIMER->CCER = 0 ;
  INTERRUPT_5MS_TIMER->CCMR1 = 0 ;
  INTERRUPT_5MS_TIMER->EGR = 0 ;
  INTERRUPT_5MS_TIMER->CR1 = 5 ;
  INTERRUPT_5MS_TIMER->DIER |= 1 ;
  NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn) ;
  NVIC_SetPriority(TIM8_TRG_COM_TIM14_IRQn, 7);
}

void stop5msTimer( void )
{
  TIM14->CR1 = 0 ;        // stop timer
  NVIC_DisableIRQ(TIM8_TRG_COM_TIM14_IRQn) ;
  RCC->APB1ENR &= ~RCC_APB1ENR_TIM14EN ;          // Disable clock
}

// TODO use the same than board_sky9x.cpp
void interrupt5ms()
{
  static uint32_t pre_scale ;       // Used to get 10 Hz counter

  ++pre_scale;

  if (pre_scale == 5 || pre_scale == 10) {

#if defined(HAPTIC)
    HAPTIC_HEARTBEAT();
#endif

  }

  if ( pre_scale >= 10 ) {
    pre_scale = 0 ;
    per10ms();
  }

  checkRotaryEncoder();
}

#if !defined(SIMU)
extern "C" void TIM8_TRG_COM_TIM14_IRQHandler()
{
  TIM14->SR &= ~TIM_SR_UIF ;
  interrupt5ms() ;
}

#if 0
void pinCheck(GPIO_TypeDef * gpio, uint32_t pin, uint32_t RCC_AHB1Periph)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph, ENABLE);

  GPIO_InitStruct.GPIO_Pin = pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(gpio, &GPIO_InitStruct);

  for (int i=0; i<10; i++) {
    GPIO_SetBits(gpio, pin);
    delay_01us(10);
    GPIO_ResetBits(gpio, pin);
    delay_01us(10);
  }
}
#endif

void boardInit()
{
  RCC_AHB1PeriphClockCmd(PWR_RCC_AHB1Periph | LCD_RCC_AHB1Periph | KEYS_RCC_AHB1Periph_GPIO | ADC_RCC_AHB1Periph | SERIAL_RCC_AHB1Periph | TELEMETRY_RCC_AHB1Periph | AUDIO_RCC_AHB1Periph | HAPTIC_RCC_AHB1Periph, ENABLE);
  RCC_APB1PeriphClockCmd(INTERRUPT_5MS_APB1Periph | TIMER_2MHz_APB1Periph | SERIAL_RCC_APB1Periph | TELEMETRY_RCC_APB1Periph | AUDIO_RCC_APB1Periph, ENABLE);
  RCC_APB2PeriphClockCmd(LCD_RCC_APB2Periph | ADC_RCC_APB2Periph | HAPTIC_RCC_APB2Periph, ENABLE);

  pwrInit();
  ledInit();
  delaysInit();

  // FrSky removed the volume chip in latest board, that's why it doesn't answer!
  // i2cInit();

  if (0) {
   // pinCheck(SERIAL_GPIO, SERIAL_GPIO_PIN_TX, SERIAL_RCC_AHB1Periph_GPIO);
   // pinCheck(EEPROM_GPIO, EEPROM_GPIO_PIN_SCK, RCC_AHB1Periph_GPIOB);
   // pinCheck(EEPROM_GPIO, EEPROM_GPIO_PIN_MISO, RCC_AHB1Periph_GPIOB);
   // pinCheck(EEPROM_GPIO, EEPROM_GPIO_PIN_MOSI, RCC_AHB1Periph_GPIOB);
  }

  serial2Init(0, 0); // default serial mode (None if DEBUG not defined)

  __enable_irq();

  TRACE("Horus started :)");
  ledBlue();

  keysInit();
  adcInit();
  lcdInit();
  audioInit();
  init2MhzTimer();
  init5msTimer();
  usbInit();
  hapticInit();

  //  rotencInit();
  //  bt_open();

#if defined(DEBUG)
  DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP|DBGMCU_TIM1_STOP|DBGMCU_TIM2_STOP|DBGMCU_TIM3_STOP|DBGMCU_TIM4_STOP|DBGMCU_TIM5_STOP|DBGMCU_TIM6_STOP|DBGMCU_TIM7_STOP|DBGMCU_TIM8_STOP|DBGMCU_TIM9_STOP|DBGMCU_TIM10_STOP|DBGMCU_TIM11_STOP|DBGMCU_TIM12_STOP|DBGMCU_TIM13_STOP|DBGMCU_TIM14_STOP, ENABLE);
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
    }

    currentTrainerMode = requiredTrainerMode;
    switch (requiredTrainerMode) {
      case TRAINER_MODE_SLAVE:
        init_trainer_ppm();
        break;
      default:
        // master is default
        init_trainer_capture();
        break;
    }
  }
}
