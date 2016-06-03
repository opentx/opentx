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

void setupPulses(uint8_t port);
void setupPulsesPPM(uint8_t port);
void setupPulsesPXX(uint8_t port);

static void extmoduleStop(void);

static void intmodulePxxStart(void);
static void intmodulePxxStop(void);
static void extmodulePxxStart(void);
#if defined(DSM2)
static void extmoduleDsm2Start(void);
#endif
static void extmodulePpmStart(void);
static void intmoduleNoneStart(void);
static void intmoduleNoneStop(void);
static void extmoduleNoneStart(void);
static void extmoduleCrossfireStart(void);

void init_pxx(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmodulePxxStart();
  else
    extmodulePxxStart();
}

void disable_pxx(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmodulePxxStop();
  else
    extmoduleStop();
}

#if defined(DSM2)
void init_dsm2(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleDsm2Start();
  }
}

void disable_dsm2(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleStop();
  }
}
#endif

void init_ppm(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmodulePpmStart();
  }
}

void disable_ppm(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleStop();
  }
}

void set_external_ppm_parameters(uint32_t idleTime, uint32_t delay, uint32_t positive)
{
  EXTMODULE_TIMER->CCR2 = idleTime;
  EXTMODULE_TIMER->CCR1 = delay;
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | (positive ? 0 : TIM_CCER_CC1P);
}

void init_no_pulses(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmoduleNoneStart();
  else
    extmoduleNoneStart();
}

void disable_no_pulses(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmoduleNoneStop();
  else
    extmoduleStop();
}

void init_crossfire(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleCrossfireStart();
  }
}

void disable_crossfire(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleStop();
  }
}

static void intmoduleNoneStart()
{
  INTERNAL_MODULE_OFF();

  // Timer1, channel 3
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = INTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(INTMODULE_TX_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(INTMODULE_TX_GPIO, INTMODULE_TX_GPIO_PIN); // Set high

  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  INTMODULE_TIMER->ARR = 36000;             // 18mS
  INTMODULE_TIMER->CCR2 = 32000;            // Update time
  INTMODULE_TIMER->PSC = INTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)

  INTMODULE_TIMER->CCER = TIM_CCER_CC3E;

  INTMODULE_TIMER->CCMR2 = 0;
  INTMODULE_TIMER->EGR = 1;                                                         // Restart

  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0;                     // Toggle CC1 o/p
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;                             // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;  // Enable this interrupt
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN;
  NVIC_EnableIRQ(INTMODULE_TIMER_IRQn);
  NVIC_SetPriority(INTMODULE_TIMER_IRQn, 7);
}

static void intmoduleNoneStop()
{
  NVIC_DisableIRQ(INTMODULE_TIMER_IRQn);
  INTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE;
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
}

extern "C" void INTMODULE_DMA_STREAM_IRQHandler(void)
{
  DEBUG_INTERRUPT(INT_DMA2S7);
  if (DMA_GetITStatus(INTMODULE_DMA_STREAM, INTMODULE_DMA_FLAG_TC)) {
    // TODO we could send the 8 next channels here (when needed)
    DMA_ClearITPendingBit(INTMODULE_DMA_STREAM, INTMODULE_DMA_FLAG_TC);
  }
}

extern "C" void INTMODULE_TIMER_CC_IRQHandler()
{
  DEBUG_INTERRUPT(INT_TIM1CC);
  DEBUG_TIMER_SAMPLE(debugTimerIntPulses);
  DEBUG_TIMER_START(debugTimerIntPulsesDuration);
  INTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE;       // stop this interrupt
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;           // clear flag

  setupPulses(INTERNAL_MODULE);

  if (s_current_protocol[INTERNAL_MODULE] == PROTO_PXX) {
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(INTMODULE_DMA_STREAM);
    DMA_InitStructure.DMA_Channel = INTMODULE_DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&INTMODULE_USART->DR);
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(modulePulsesData[INTERNAL_MODULE].pxx_uart.pulses);
    DMA_InitStructure.DMA_BufferSize = (uint8_t *)modulePulsesData[INTERNAL_MODULE].pxx_uart.ptr - (uint8_t *)modulePulsesData[INTERNAL_MODULE].pxx_uart.pulses;
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
    DMA_Init(INTMODULE_DMA_STREAM, &DMA_InitStructure);

    DMA_Cmd(INTMODULE_DMA_STREAM, ENABLE);
    USART_DMACmd(INTMODULE_USART, USART_DMAReq_Tx, ENABLE);
  }

  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;
  DEBUG_TIMER_STOP(debugTimerIntPulsesDuration);
}

static void extmoduleNoneStart()
{
  if (!IS_TRAINER_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }

  GPIO_PinAFConfig(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PinSource, 0);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PPM_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_PPM_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PIN); // Set high

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 36000; // 18mS
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->DIER |= TIM_DIER_UIE;  // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

static void extmoduleStop()
{
  NVIC_DisableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn);
  EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
  EXTMODULE_TIMER->DIER &= ~(TIM_DIER_UIE | TIM_DIER_UDE);
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;

  if (!IS_TRAINER_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }
}

static void extmoduleCrossfireStart()
{
  EXTERNAL_MODULE_ON();

  GPIO_PinAFConfig(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PinSource, 0);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PPM_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_PPM_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PIN); // Set high

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = (2000 * CROSSFIRE_FRAME_PERIOD);
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->DIER |= TIM_DIER_UIE; // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

void intmodulePxxStart()
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  INTERNAL_MODULE_ON();

  setupPulsesPXX(INTERNAL_MODULE);

  NVIC_InitStructure.NVIC_IRQChannel = INTMODULE_DMA_STREAM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; /* Not used as 4 bits are used for the pre-emption priority. */;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // TX Pin
  GPIO_PinAFConfig(INTMODULE_TX_GPIO, INTMODULE_TX_GPIO_PinSource, INTMODULE_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(INTMODULE_TX_GPIO, &GPIO_InitStructure);

  // RX Pin
  GPIO_PinAFConfig(INTMODULE_RX_GPIO, INTMODULE_RX_GPIO_PinSource, INTMODULE_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_RX_GPIO_PIN;
  GPIO_Init(INTMODULE_RX_GPIO, &GPIO_InitStructure);

  // UART config
  USART_DeInit(INTMODULE_USART);
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(INTMODULE_USART, &USART_InitStructure);
  USART_Cmd(INTMODULE_USART, ENABLE);

  // Timer
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  INTMODULE_TIMER->ARR = 18000;                     // 9mS
  INTMODULE_TIMER->CCR2 = 15000;            // Update time
  INTMODULE_TIMER->PSC = INTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  INTMODULE_TIMER->CCER = TIM_CCER_CC3E;
  INTMODULE_TIMER->CCER = TIM_CCER_CC3E;

  INTMODULE_TIMER->CCMR2 = 0;
  INTMODULE_TIMER->EGR = 1;                                                         // Restart

  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0;                     // Toggle CC1 o/p
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;                             // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;  // Enable this interrupt
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN;
  NVIC_EnableIRQ(INTMODULE_TIMER_IRQn);
  NVIC_SetPriority(INTMODULE_TIMER_IRQn, 7);
}

static void intmodulePxxStop()
{
  INTERNAL_MODULE_OFF();
}

void extmodulePxxStart()
{
  EXTERNAL_MODULE_ON();

  setupPulsesPXX(EXTERNAL_MODULE);

  GPIO_PinAFConfig(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PinSource, EXTMODULE_PPM_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PPM_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_PPM_GPIO, &GPIO_InitStructure);

  EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
  EXTMODULE_DMA_STREAM->CR |= EXTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_1 | DMA_SxCR_MSIZE_1 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
  EXTMODULE_DMA_STREAM->NDTR = modulePulsesData[EXTERNAL_MODULE].pxx.ptr - modulePulsesData[EXTERNAL_MODULE].pxx.pulses;
  EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
  EXTMODULE_DMA_STREAM->M0AR = CONVERT_PTR_UINT(modulePulsesData[EXTERNAL_MODULE].pxx.pulses);

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 18000;
  EXTMODULE_TIMER->CCER = TIM_OCPolarity_Low | TIM_OCNPolarity_Low | TIM_OutputState_Enable | TIM_OutputNState_Enable; //  TIM_CCER_CC1E | TIM_CCER_CC1P;
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE; // Enable outputs
  EXTMODULE_TIMER->CCR1 = 18;
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0; // Force O/P high
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Enable DMA on update
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
}

extern "C" void EXTMODULE_DMA_IRQHandler()
{
  if (!DMA_GetITStatus(EXTMODULE_DMA_STREAM, EXTMODULE_DMA_FLAG_TC))
    return;

  DMA_ClearITPendingBit(EXTMODULE_DMA_STREAM, EXTMODULE_DMA_FLAG_TC);

  setupPulses(EXTERNAL_MODULE);

  if (s_current_protocol[EXTERNAL_MODULE] == PROTO_PPM) {
    EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_1 | DMA_SxCR_MSIZE_1 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
    EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
    EXTMODULE_DMA_STREAM->M0AR = CONVERT_PTR_UINT(modulePulsesData[EXTERNAL_MODULE].ppm.pulses);
    EXTMODULE_DMA_STREAM->NDTR = modulePulsesData[EXTERNAL_MODULE].ppm.ptr - modulePulsesData[EXTERNAL_MODULE].ppm.pulses;
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA
  }
  else if (s_current_protocol[EXTERNAL_MODULE] == PROTO_PXX) {
    EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_1 | DMA_SxCR_MSIZE_1 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
    EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
    EXTMODULE_DMA_STREAM->M0AR = CONVERT_PTR_UINT(modulePulsesData[EXTERNAL_MODULE].pxx.pulses);
    EXTMODULE_DMA_STREAM->NDTR = modulePulsesData[EXTERNAL_MODULE].pxx.ptr - modulePulsesData[EXTERNAL_MODULE].pxx.pulses;
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA
  }
  else if (s_current_protocol[EXTERNAL_MODULE] >= PROTO_DSM2_LP45 && s_current_protocol[EXTERNAL_MODULE] <= PROTO_DSM2_DSMX) {
    EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_1 | DMA_SxCR_MSIZE_1 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
    EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
    EXTMODULE_DMA_STREAM->M0AR = CONVERT_PTR_UINT(modulePulsesData[EXTERNAL_MODULE].dsm2.pulses);
    EXTMODULE_DMA_STREAM->NDTR = modulePulsesData[EXTERNAL_MODULE].dsm2.ptr - modulePulsesData[EXTERNAL_MODULE].dsm2.pulses;
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA
  }
}

#if defined(DSM2)
static void extmoduleDsm2Start()
{
  EXTERNAL_MODULE_ON();

  setupPulsesDSM2(EXTERNAL_MODULE);

  GPIO_PinAFConfig(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PinSource, EXTMODULE_PPM_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PPM_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_PPM_GPIO, &GPIO_InitStructure);

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 44000;
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | TIM_CCER_CC1P;
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE; // Enable outputs
  EXTMODULE_TIMER->CCR1 = 0;
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0; // Force O/P high
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Enable DMA on update
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0;
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
  EXTMODULE_DMA_STREAM->CR |= EXTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_1 | DMA_SxCR_MSIZE_1 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
  EXTMODULE_DMA_STREAM->NDTR = modulePulsesData[EXTERNAL_MODULE].dsm2.ptr - modulePulsesData[EXTERNAL_MODULE].dsm2.pulses;
  EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
  EXTMODULE_DMA_STREAM->M0AR = CONVERT_PTR_UINT(modulePulsesData[EXTERNAL_MODULE].dsm2.pulses);
  EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA and TC interrupt

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
}
#endif

static void extmodulePpmStart()
{
  EXTERNAL_MODULE_ON();

  GPIO_PinAFConfig(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PinSource, EXTMODULE_PPM_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PPM_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_PPM_GPIO, &GPIO_InitStructure);

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
  // stop timer
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;

  // setupPulsesPPM() is also configuring registers,
  // so it has to be called after the peripheral is enabled
  setupPulsesPPM(EXTERNAL_MODULE);

  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 41000;

  // CH1 PWM mode 1, CH1 out is OFF if CNT<CCR1(delay) and ON if bigger (up to AAR,which is pulse length)
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2PE;        // PWM mode 1

  // Reloads register vales now
  EXTMODULE_TIMER->EGR = 1;

  // Update DMA request
  EXTMODULE_TIMER->DIER |= TIM_DIER_UDE;

  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2PE;
  // enable timer
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
  EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_1 | DMA_SxCR_MSIZE_1 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
  EXTMODULE_DMA_STREAM->NDTR = modulePulsesData[EXTERNAL_MODULE].ppm.ptr - modulePulsesData[EXTERNAL_MODULE].ppm.pulses;
  EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
  EXTMODULE_DMA_STREAM->M0AR = CONVERT_PTR_UINT(modulePulsesData[EXTERNAL_MODULE].ppm.pulses);
  EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA and interrupt

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
}

extern "C" void EXTMODULE_TIMER_IRQHandler()
{
  EXTMODULE_TIMER->SR &= ~TIM_SR_UIF; // Clear flag
  setupPulses(EXTERNAL_MODULE);
}

