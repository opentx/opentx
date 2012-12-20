/**
  ******************************************************************************
  * @file    I2C/I2C_TwoBoards/MasterTransmitterInterrupt/stm32f2xx_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
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

/** @addtogroup MasterTransmitterInterrupt
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t TxBuffer[];
__IO uint8_t Counter = 0x00;
extern __IO uint8_t NumberOfByte;

#if defined (I2C_SLAVE)
 __IO uint32_t Event = 0x00;
 extern  uint8_t RxBuffer[];
 extern __IO uint8_t Rx_Idx;
 extern __IO uint8_t CmdReceived;
#endif /* I2C_SLAVE */

#if defined (I2C_MASTER)
 uint8_t HEADER_ADDRESS_Write = (((SLAVE_ADDRESS & 0xFF00) >> 7) | 0xF0);
 extern __IO uint8_t PressedButton;
 extern __IO uint8_t Tx_Idx;
 extern __IO uint32_t TimeOut;
 extern __IO uint8_t CmdTransmitted;
#endif /* I2C_MASTER */

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
#if defined (I2C_MASTER)  
  /* Decrement the time out value */
  if (TimeOut != 0x0)
  {
    TimeOut--;
  }
#endif /* I2C_MASTER*/
  
  if (Counter < 10)
  {
    Counter++;
  }
  else
  {
    Counter = 0x00;
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
  * @brief  This function handles I2Cx Error interrupt request.
  * @param  None
  * @retval None
  */
void I2Cx_ER_IRQHandler(void)
{
#if defined (I2C_SLAVE)

  /* Read SR1 register to get I2C error */
  if ((I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0xFF00) != 0x00)
  {
    /* Clears erreur flags */
    I2Cx->SR1 &= 0x00FF;
  }
#endif /* I2C_SLAVE*/
  
#if defined (I2C_MASTER)
  
  /* Read SR1 register to get I2C error */
  if ((I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0xFF00) != 0x00)
  {
    /* Clears erreur flags */
    I2Cx->SR1 &= 0x00FF;
    
    /* Set LED3 and LED4 */
    STM_EVAL_LEDOn(LED3);
    STM_EVAL_LEDOn(LED4);
    STM_EVAL_LEDOff(LED2);
  }
#endif /* I2C_MASTER*/
}

/**
  * @brief  This function handles I2Cx event interrupt request.
  * @param  None
  * @retval None
  */
void I2Cx_EV_IRQHandler(void)
{
#if defined (I2C_MASTER)

  switch (I2C_GetLastEvent(I2Cx))
  {
    /* EV5 */
    case I2C_EVENT_MASTER_MODE_SELECT :
#ifdef I2C_10BITS_ADDRESS
      /* Send Header to Slave for write */
      I2C_SendData(I2Cx, HEADER_ADDRESS_Write);
      break;

    /* EV9 */
    case I2C_EVENT_MASTER_MODE_ADDRESS10:
#endif /* I2C_10BITS_ADDRESS */

      /* Send slave Address for write */
      I2C_Send7bitAddress(I2Cx, (uint8_t)SLAVE_ADDRESS, I2C_Direction_Transmitter);
      break;

    /* EV6 */
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
      /* Send the I2C transaction code */
      I2C_SendData(I2Cx, CmdTransmitted);
      break;

    /* EV8 */
    case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
    case I2C_EVENT_MASTER_BYTE_TRANSMITTED:      
     if (Tx_Idx == GetVar_NbrOfDataToTransfer())
      {
        /* Send STOP condition */
        I2C_GenerateSTOP(I2Cx, ENABLE);
        I2C_ITConfig(I2Cx, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
      }
      else
      {
        /* Transmit Data TxBuffer */
        I2C_SendData(I2Cx, TxBuffer[Tx_Idx++]); 
      }
      break;

    default:
      break;
  }

#endif /* I2C_MASTER*/
  
#if defined (I2C_SLAVE)

  /* Get Last I2C Event */
  Event = I2C_GetLastEvent(I2Cx);

  switch (Event)
  {
    /* Check on EV1*/
    case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:
    Rx_Idx = 0x00;
    break;

    /* Check on EV2*/
   case I2C_EVENT_SLAVE_BYTE_RECEIVED:  
   case (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_SR1_BTF):  
   if (CmdReceived == 0x00)
    {
      CmdReceived = I2C_ReceiveData(I2Cx);
    }
    else
    {
      RxBuffer[Rx_Idx++] = I2C_ReceiveData(I2Cx);
    }
      break;

    /* Check on EV4 */
    case (I2C_EVENT_SLAVE_STOP_DETECTED):
      I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF);
      I2C_Cmd(I2Cx, ENABLE);
      break;

    default:
      break;
  }
#endif /* I2C_SLAVE*/  
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
