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

// Start TIMER7 at 2000000Hz
void init2MhzTimer()
{
  // Now for timer 7
  RCC->APB1ENR |= RCC_APB1ENR_TIM7EN ;            // Enable clock

  TIM7->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1 ;       // 0.5 uS, 2 MHz
  TIM7->ARR = 65535;
  TIM7->CR2 = 0;
  TIM7->CR1 = TIM_CR1_CEN;
}

// Starts TIMER at 200Hz, 5mS period
void init5msTimer()
{
  // Timer14
  RCC->APB1ENR |= RCC_APB1ENR_TIM14EN ;           // Enable clock
  TIM14->ARR = 4999 ;     // 5mS
  TIM14->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 1000000 - 1 ;                // 1uS from 30MHz
  TIM14->CCER = 0 ;
  TIM14->CCMR1 = 0 ;
  TIM14->EGR = 0 ;
  TIM14->CR1 = 5 ;
  TIM14->DIER |= 1 ;
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

  AUDIO_HEARTBEAT();

#if defined(HAPTIC)
  HAPTIC_HEARTBEAT();
#endif

  if ( ++pre_scale >= 2 ) {
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

void boardInit()
{
  pwrInit();
  ledInit();
  delaysInit();

  ledRed();


  if (0) {
   // pinCheck(SERIAL_GPIO, SERIAL_GPIO_PIN_TX, SERIAL_RCC_AHB1Periph_GPIO);
   // pinCheck(EEPROM_GPIO, EEPROM_GPIO_PIN_SCK, RCC_AHB1Periph_GPIOB);
   // pinCheck(EEPROM_GPIO, EEPROM_GPIO_PIN_MISO, RCC_AHB1Periph_GPIOB);
   // pinCheck(EEPROM_GPIO, EEPROM_GPIO_PIN_MOSI, RCC_AHB1Periph_GPIOB);
  }

  serial2Init(0, 0); // default serial mode (None if DEBUG not defined)

  ledBlue();

  __enable_irq();

  TRACE("Horus started :)");

  keysInit();
  // adcInit();
  lcdInit();
  audioInit();
  i2cInit();
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

// // TODO
// void eeDirty(uint8_t msk) { }
// uint8_t   s_eeDirtyMsk;
// tmr10ms_t s_eeDirtyTime10ms;
// void eeCheck(bool) { }
// ModelHeader modelHeaders[MAX_MODELS];
// void eeLoadModelHeaders() { }
// void eeLoadModelHeader(uint8_t id, ModelHeader *header) { }
// bool eeCopyModel(uint8_t dst, uint8_t src) { }
// void eeSwapModels(uint8_t id1, uint8_t id2) { }
// void eeDeleteModel(uint8_t idx) { }
// uint8_t eeFindEmptyModel(uint8_t id, bool down) { return 0; };
// void eeLoadModel(uint8_t id) { }
// bool eeModelExists(uint8_t id) { return true; }
// const pm_char * eeBackupModel(uint8_t i_fileSrc) { return NULL; }
// const pm_char * eeRestoreModel(uint8_t i_fileDst, char *model_name) { return NULL; }
// void selectModel(uint8_t sub) { }
