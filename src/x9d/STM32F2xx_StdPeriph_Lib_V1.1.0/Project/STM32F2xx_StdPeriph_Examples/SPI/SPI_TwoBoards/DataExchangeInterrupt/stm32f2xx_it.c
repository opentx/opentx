/**
  ******************************************************************************
  * @file    SPI/SPI_TwoBoards/DataExchangeInterrupt/stm32f2xx_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_it.h"
#include "main.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_DataExchangeInterrupt
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef SPI_SLAVE
 extern  __IO uint8_t RxBuffer[];
 extern __IO uint8_t Rx_Idx;
#endif
#ifdef SPI_MASTER
 extern __IO uint8_t CmdTransmitted;
 extern __IO uint8_t Tx_Idx;
 extern __IO uint8_t CmdStatus;
#endif

extern uint8_t TxBuffer[];
extern __IO uint32_t TimeOut;
__IO uint8_t Counter = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  /* Decrement the timeout value */
  if (TimeOut != 0)
  {
    TimeOut--;
  }
    
  if (Counter < 10)
  {
    Counter++;
  }
  else
  {
    Counter = 0;
    STM_EVAL_LEDToggle(LED1);
  }
}

/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @brief  This function handles SPI interrupt request.
  * @param  None
  * @retval None
  */
void SPIx_IRQHANDLER(void)
{
#ifdef SPI_SLAVE
  /* SPI in Slave Receiver mode--------------------------------------- */
  if (SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_RXNE) == SET)
  {
    RxBuffer[Rx_Idx++] = SPI_I2S_ReceiveData(SPIx);
  }
#endif 
#ifdef SPI_MASTER
  /* SPI in Master Tramitter mode--------------------------------------- */
  if (SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_TXE) == SET)
  {
    if (CmdStatus == 0x00)
    {
	    /* Send Transaction code */
      SPI_I2S_SendData(SPIx, CmdTransmitted);
      CmdStatus = 0x01;
    }
    else
    {
      if (Tx_Idx < GetVar_NbrOfData())
      {
	      /* Send Transaction data */
        SPI_I2S_SendData(SPIx, TxBuffer[Tx_Idx++]);
      }
      else
      {
        /* Disable the Tx buffer empty interrupt */
        SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
      }
    }
  }
#endif /* SPI_SLAVE */
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
