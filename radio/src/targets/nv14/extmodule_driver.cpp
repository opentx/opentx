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
//fix it
#if defined(EXTMODULE_USART)
ModuleFifo extmoduleFifo;
#endif
bool stopOnTxDone = false;

void extmoduleSendNextFrame();

void EXTERNAL_MODULE_ON()
{
  GPIO_SetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN);
  GPIO_ResetBits(EXTMODULE_PWR_FIX_GPIO, EXTMODULE_PWR_FIX_GPIO_PIN);
}
void EXTERNAL_MODULE_OFF()
{
  GPIO_ResetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN);
  GPIO_SetBits(EXTMODULE_PWR_FIX_GPIO, EXTMODULE_PWR_FIX_GPIO_PIN);
}
void extModuleInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PWR_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_ResetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN);
  GPIO_Init(EXTMODULE_PWR_GPIO, &GPIO_InitStructure);

  //for additional transistor to ensuring module is completely disabled
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  //pin must be pulled to V+ (voltage of board - VCC is not enough to fully close transistor)
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PWR_FIX_GPIO_PIN;
  GPIO_SetBits(EXTMODULE_PWR_FIX_GPIO, EXTMODULE_PWR_FIX_GPIO_PIN);
  GPIO_Init(EXTMODULE_PWR_FIX_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_INVERT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(EXTMODULE_TX_INVERT_GPIO, &GPIO_InitStructure);

  GPIO_SetBits(EXTMODULE_TX_INVERT_GPIO, EXTMODULE_TX_INVERT_GPIO_PIN);
}

void extmoduleStop()
{
  EXTERNAL_MODULE_OFF();

  NVIC_DisableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_DisableIRQ(EXTMODULE_USART_TX_DMA_IRQn);

  USART_DeInit(EXTMODULE_USART);

  DMA_Cmd(EXTMODULE_USART_RX_DMA_STREAM, DISABLE);
  USART_DMACmd(EXTMODULE_USART, USART_DMAReq_Rx, DISABLE);
  DMA_DeInit(EXTMODULE_USART_RX_DMA_STREAM);

  DMA_Cmd(EXTMODULE_USART_TX_DMA_STREAM, DISABLE);
  USART_DMACmd(EXTMODULE_USART, USART_DMAReq_Tx, DISABLE);
  DMA_DeInit(EXTMODULE_USART_TX_DMA_STREAM);

  EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
  EXTMODULE_USART_TX_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable UART DMA

  EXTMODULE_TIMER->DIER &= ~(TIM_DIER_CC2IE | TIM_DIER_UDE);
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
}

void extmoduleNoneStart()
{
  EXTERNAL_MODULE_OFF();

  USART_DeInit(EXTMODULE_USART);

  DMA_Cmd(EXTMODULE_USART_RX_DMA_STREAM, DISABLE);
  USART_DMACmd(EXTMODULE_USART, USART_DMAReq_Rx, DISABLE);
  DMA_DeInit(EXTMODULE_USART_RX_DMA_STREAM);

  DMA_Cmd(EXTMODULE_USART_TX_DMA_STREAM, DISABLE);
  USART_DMACmd(EXTMODULE_USART, USART_DMAReq_Tx, DISABLE);
  DMA_DeInit(EXTMODULE_USART_TX_DMA_STREAM);

  GPIO_PinAFConfig(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PinSource, 0);
  GPIO_PinAFConfig(EXTMODULE_RX_GPIO, EXTMODULE_RX_GPIO_PinSource, 0);

  extmoduleFifo.clear();

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_TX_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PIN); // Set high

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 36000; // 18mS
  EXTMODULE_TIMER->CCR2 = 32000; // Update time
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE; // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);

  GPIO_SetBits(EXTMODULE_TX_INVERT_GPIO, EXTMODULE_TX_INVERT_GPIO_PIN);
}

void extmodulePpmStart()
{
  EXTERNAL_MODULE_ON();
  GPIO_SetBits(EXTMODULE_TX_INVERT_GPIO, EXTMODULE_TX_INVERT_GPIO_PIN);
  GPIO_PinAFConfig(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PinSource, EXTMODULE_TX_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_TX_GPIO, &GPIO_InitStructure);

  // PPM generation principle:
  //
  // Hardware timer in PWM mode is used for PPM generation
  // Output is OFF if CNT<CCR1(delay) and ON if bigger
  // CCR1 register defines duration of pulse length and is constant
  // AAR register defines duration of each pulse, it is
  // updated after every pulse in Update interrupt handler.
  // CCR2 register defines duration of no pulses (time between two pulse trains)
  // it is calculated every round to have PPM period constant.
  // CC2 interrupt is then used to setup new PPM values for the
  // next PPM pulses train.

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN; // Stop timer
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 45000;

  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // PWM mode 1
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE;
  EXTMODULE_TIMER->EGR = 1; // Reloads register values now
  EXTMODULE_TIMER->DIER = TIM_DIER_UDE; // Update DMA request
  EXTMODULE_TIMER->CR1 = TIM_CR1_CEN; // Start timer


  extmoduleSendNextFrame();

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

void extmoduleSerialStart(uint32_t baudrate, uint32_t period_half_us, bool inverted)
{

}

void extmodulePxxStart()
{

  EXTERNAL_MODULE_ON();
  GPIO_SetBits(EXTMODULE_TX_INVERT_GPIO, EXTMODULE_TX_INVERT_GPIO_PIN);
  GPIO_PinAFConfig(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PinSource, EXTMODULE_TX_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_TX_GPIO, &GPIO_InitStructure);

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 18000;


  EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | TIM_CCER_CC1NE;
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE; // Enable outputs
  EXTMODULE_TIMER->CCR1 = 18;
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0; // Force O/P high
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Enable DMA on update
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  extmoduleSendNextFrame();

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}



void ConfigureRxDMA(){
  extmoduleFifo.clear();
  /*
  DMA_Cmd(EXTMODULE_USART_RX_DMA_STREAM, DISABLE);
  USART_DMACmd(EXTMODULE_USART, USART_DMAReq_Rx, DISABLE);
  DMA_DeInit(EXTMODULE_USART_RX_DMA_STREAM);

  DMA_InitTypeDef DMA_InitStructure;
  DMA_InitStructure.DMA_Channel = EXTMODULE_USART_RX_DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&EXTMODULE_USART->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(extmoduleFifo.buffer());
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = extmoduleFifo.size();
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(EXTMODULE_USART_RX_DMA_STREAM, &DMA_InitStructure);
  USART_DMACmd(EXTMODULE_USART, USART_DMAReq_Rx, ENABLE);
  DMA_Cmd(EXTMODULE_USART_RX_DMA_STREAM, ENABLE);
  */
}
#if defined(DSM2)
void extmoduleDsm2Start()
{
  /*
  EXTERNAL_MODULE_ON();
  GPIO_SetBits(EXTMODULE_TX_INVERT_GPIO, EXTMODULE_TX_INVERT_GPIO_PIN);
  GPIO_PinAFConfig(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PinSource, EXTMODULE_TX_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_TX_GPIO, &GPIO_InitStructure);

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 44000; // 22mS

  EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | TIM_CCER_CC1P;
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE; // Enable outputs
  EXTMODULE_TIMER->CCR1 = 0;
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0; // Force O/P high
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Enable DMA on update
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0;
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  extmoduleSendNextFrame();

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
  */
}
#endif

void extmoduleCrossfireStart()
{
  EXTERNAL_MODULE_ON();

  GPIO_PinAFConfig(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PinSource, 0);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_TX_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PIN); // Set high

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = (2000 * CROSSFIRE_PERIOD);
  EXTMODULE_TIMER->CCR2 = (2000 * CROSSFIRE_PERIOD) - 1000;
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE; // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}


void extmoduleSendNextFrame()
{
  /*
  uint32_t start = 0;
  uint32_t count = 0;
  if (moduleState[EXTERNAL_MODULE].protocol == PROTOCOL_CHANNELS_PPM) {
    EXTMODULE_TIMER->CCR1 = GET_MODULE_PPM_DELAY(EXTERNAL_MODULE)*2;
    EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | (GET_MODULE_PPM_POLARITY(EXTERNAL_MODULE) ? TIM_CCER_CC1P : 0);
    EXTMODULE_TIMER->CCR2 = *(extmodulePulsesData.ppm.ptr - 1) - 4000; // 2mS in advance
    EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
    EXTMODULE_DMA_STREAM->CR |= EXTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_0 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
    EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
    start = CONVERT_PTR_UINT(extmodulePulsesData.ppm.pulses);
    count = extmodulePulsesData.ppm.ptr - extmodulePulsesData.ppm.pulses;
    EXTMODULE_DMA_STREAM->M0AR = start;
    EXTMODULE_DMA_STREAM->NDTR = count;
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA | DMA_SxCR_TEIE | DMA_SxCR_DMEIE
  }
  else if (moduleState[EXTERNAL_MODULE].protocol == PROTOCOL_CHANNELS_PXX1_PULSES) {
    EXTMODULE_TIMER->CCR2 = *(extmodulePulsesData.pxx.ptr - 1) - 4000; // 2mS in advance
    EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
    EXTMODULE_DMA_STREAM->CR |= EXTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_0 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
    EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
    start = CONVERT_PTR_UINT(extmodulePulsesData.pxx.pulses);
    count = extmodulePulsesData.pxx.ptr - extmodulePulsesData.pxx.pulses;
    EXTMODULE_DMA_STREAM->M0AR = start;
    EXTMODULE_DMA_STREAM->NDTR = count;
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA | DMA_SxCR_TEIE | DMA_SxCR_DMEIE
  }
  else if (IS_DSM2_PROTOCOL(moduleState[EXTERNAL_MODULE].protocol) || IS_MULTIMODULE_PROTOCOL(moduleState[EXTERNAL_MODULE].protocol) || IS_SBUS_PROTOCOL(moduleState[EXTERNAL_MODULE].protocol)) {
    EXTMODULE_TIMER->CCR2 = *(extmodulePulsesData.dsm2.ptr - 1) - 4000; // 2mS in advance
    EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
    EXTMODULE_DMA_STREAM->CR |= EXTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_0 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
    if (IS_SBUS_PROTOCOL(moduleState[EXTERNAL_MODULE].protocol)) EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | (GET_SBUS_POLARITY(EXTERNAL_MODULE) ? TIM_CCER_CC1P : 0); // reverse polarity for Sbus if needed
    EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
    start = CONVERT_PTR_UINT(extmodulePulsesData.dsm2.pulses);
    count = extmodulePulsesData.dsm2.ptr - extmodulePulsesData.dsm2.pulses;
    EXTMODULE_DMA_STREAM->M0AR = start;
    EXTMODULE_DMA_STREAM->NDTR = count;
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE; // Enable DMA | DMA_SxCR_TEIE | DMA_SxCR_DMEIE
  }
  else if (moduleState[EXTERNAL_MODULE].protocol == PROTO_AFHDS3) {
    count = extmodulePulsesData.flysky.ptr - extmodulePulsesData.flysky.pulses;
    start = CONVERT_PTR_UINT(extmodulePulsesData.flysky.pulses);

    if(count == 0) {
      EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;
    }
    else {
      DMA_DeInit (EXTMODULE_USART_TX_DMA_STREAM);
      DMA_InitTypeDef DMA_InitStructure;
      DMA_InitStructure.DMA_Channel = EXTMODULE_USART_TX_DMA_CHANNEL;
      DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&EXTMODULE_USART->DR);
      DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
      DMA_InitStructure.DMA_Memory0BaseAddr = start;
      DMA_InitStructure.DMA_BufferSize = count;
      DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
      DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
      DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
      DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
      DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
      DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
      DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
      DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
      DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
      DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
      DMA_Init(EXTMODULE_USART_TX_DMA_STREAM, &DMA_InitStructure);
      USART_DMACmd(EXTMODULE_USART, USART_DMAReq_Tx, ENABLE);
      EXTMODULE_USART_TX_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE; // Enable DMA | DMA_SxCR_TEIE | DMA_SxCR_DMEIE
    }
  }
  else {
    EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;
  }
   */
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;
}

extern "C" void EXTMODULE_USART_TX_DMA_IRQHandler(void)
{

  bool startTimer = false;
  if (DMA_GetITStatus(EXTMODULE_USART_TX_DMA_STREAM, EXTMODULE_USART_TX_DMA_FLAG_TC)) {
    // TODO we could send the 8 next channels here (when needed)
    DMA_ClearITPendingBit(EXTMODULE_USART_TX_DMA_STREAM, EXTMODULE_USART_TX_DMA_FLAG_TC);
    startTimer = true;
  }

  if (DMA_GetITStatus(EXTMODULE_USART_TX_DMA_STREAM, DMA_IT_TEIF7)) { //ERROR
    DMA_ClearITPendingBit(EXTMODULE_USART_TX_DMA_STREAM, DMA_IT_TEIF7);
    startTimer = true;
  }
  if (DMA_GetITStatus(EXTMODULE_USART_TX_DMA_STREAM, DMA_IT_DMEIF7)) { //ERROR
    DMA_ClearITPendingBit(EXTMODULE_USART_TX_DMA_STREAM, DMA_IT_DMEIF7);
    startTimer = true;
  }
  if(startTimer) {
    EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF; // Clear flag
    EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE; // Enable this interrupt
  }
  if(stopOnTxDone) {
    TRACE("STOP EXT MODULE AFTER SEND");
    stopOnTxDone = false;
    extmoduleStop();
  }
}

extern "C" void EXTMODULE_DMA_IRQHandler()
{
  bool startTimer = false;
  if (DMA_GetITStatus(EXTMODULE_DMA_STREAM, EXTMODULE_DMA_FLAG_TC)) {
    DMA_ClearITPendingBit(EXTMODULE_DMA_STREAM, EXTMODULE_DMA_FLAG_TC);
    startTimer = true;
  }
  if (DMA_GetITStatus(EXTMODULE_DMA_STREAM, DMA_IT_TEIF1)) { //ERROR
    DMA_ClearITPendingBit(EXTMODULE_DMA_STREAM, DMA_IT_TEIF1);
    startTimer = true;
  }
  if (DMA_GetITStatus(EXTMODULE_DMA_STREAM, DMA_IT_DMEIF1)) { //ERROR
    DMA_ClearITPendingBit(EXTMODULE_DMA_STREAM, DMA_IT_DMEIF1);
    startTimer = true;
  }

  if(startTimer) {
    EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF; // Clear flag
    EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE; // Enable this interrupt
  }
}

extern "C" void EXTMODULE_TIMER_IRQHandler()
{
  EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE; // Stop this interrupt
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;
  setupPulsesExternalModule();
  extmoduleSendNextFrame();
}
//hack for afhds3
void sendExtModuleNow() {
  NVIC_SetPendingIRQ(EXTMODULE_TIMER_IRQn);
}

uint8_t heartbeatTelemetryGetByte(uint8_t * byte)
{
  return extmoduleFifo.pop(*byte);
}

