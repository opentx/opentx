/**
  ******************************************************************************
  * @file    I2C/I2C_TwoBoards/MasterReceiverInterrupt/stm32f2xx_it.c 
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

/** @addtogroup MasterReceiverInterrupt
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern uint8_t TxBuffer[];
__IO uint8_t Counter = 0x00;
__IO uint32_t Event = 0x00;

#if defined (I2C_MASTER)
 uint8_t HEADER_ADDRESS_Write = (((SLAVE_ADDRESS & 0xFF00) >> 7) | 0xF0);
 uint8_t HEADER_ADDRESS_Read = (((SLAVE_ADDRESS & 0xFF00) >> 7) | 0xF1); 

 extern __IO uint32_t TimeOut;
 extern __IO uint8_t Rx_Idx;
 extern __IO uint8_t NumberOfByteToReceive;
 extern  uint8_t RxBuffer[];
 __IO uint8_t GenerateStartStatus = 0x00;
 #ifdef I2C_10BITS_ADDRESS  
  __IO uint8_t Send_HeaderStatus = 0x00;
 #endif /* I2C_10BITS_ADDRESS */
#endif /* I2C_MASTER */

#if defined (I2C_SLAVE)
 extern __IO uint8_t Tx_Idx;
 extern uint8_t NbrOfDataToSend;
#endif /* I2C_SLAVE */

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
    /* Clears error flags */
    I2Cx->SR1 &= 0x00FF;
    Tx_Idx = 0;
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
    Rx_Idx = 0;
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
#if defined (I2C_SLAVE)
  /* Get Last I2C Event */
  Event = I2C_GetLastEvent(I2Cx);
  switch (Event)
  { 
    /*****************************************************************************/
    /*                          Slave Transmitter Events                         */
    /*                                                                           */
    /* ***************************************************************************/  
    
    /* Check on EV1 */
  case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:  
      I2C_SendData(I2Cx, TxBuffer[Tx_Idx++]);
      /* Enable I2C event interrupt */
      I2C_ITConfig(I2Cx, I2C_IT_BUF, ENABLE);
    break;
    
    /* Check on EV3 */
  case I2C_EVENT_SLAVE_BYTE_TRANSMITTING:  
  case I2C_EVENT_SLAVE_BYTE_TRANSMITTED:  
    if (Tx_Idx < NbrOfDataToSend)
      I2C_SendData(I2Cx, TxBuffer[Tx_Idx++]);
    else
      /* Disable I2C event interrupt */
      I2C_ITConfig(I2Cx, I2C_IT_EVT  | I2C_IT_BUF, DISABLE);
    break;
 
  default:
    break;  
  }
#endif /* I2C_SLAVE*/
  
#if defined (I2C_MASTER)
/*****************************************************************************/
/*                               Master Receiver                             */
/*                                                                           */
/* ***************************************************************************/  
#ifdef I2C_10BITS_ADDRESS  
  /* Check on SB Flag and clear it */
  if(I2C_GetITStatus(I2Cx, I2C_IT_SB)== SET)
  {
    if (Send_HeaderStatus == 0x00) 
    {
      /* Send Header to Slave for write */
      I2C_SendData(I2Cx, HEADER_ADDRESS_Write);
      Send_HeaderStatus = 0x01;
      GenerateStartStatus = 0x01;
    }
    else
    {
      /* Send Header to Slave for Read */
      I2C_SendData(I2Cx, HEADER_ADDRESS_Read);
      Send_HeaderStatus = 0x00;
      if (NumberOfByteToReceive == 0x03)
      {
        /* Disable buffer Interrupts */
        I2C_ITConfig(I2Cx, I2C_IT_BUF , DISABLE);
      }
      else
      {
        /* Enable buffer Interrupts */
        I2C_ITConfig(I2Cx, I2C_IT_BUF , ENABLE);
      }
    }
  }
  /* Check on ADD10 Flag */
  else if(I2C_GetITStatus(I2Cx, I2C_IT_ADD10)== SET)
  {
    /* Send slave Address */
    I2C_Send7bitAddress(I2Cx, (uint8_t)SLAVE_ADDRESS, I2C_Direction_Transmitter);   
  }   
  
#else /* I2C_7BITS_ADDRESS */
  
  /* Check on EV5 */
  if(I2C_GetITStatus(I2Cx, I2C_IT_SB)== SET)
  {
    /* Send slave Address for read */
    I2C_Send7bitAddress(I2Cx, SLAVE_ADDRESS, I2C_Direction_Receiver);
    if (NumberOfByteToReceive == 0x03)
    {
      /* Disable buffer Interrupts */
      I2C_ITConfig(I2Cx, I2C_IT_BUF , DISABLE);
    }
    else
    {
      /* Enable buffer Interrupts */
      I2C_ITConfig(I2Cx, I2C_IT_BUF , ENABLE);
    }
  }  
#endif /* I2C_10BITS_ADDRESS */
  
  else if(I2C_GetITStatus(I2Cx, I2C_IT_ADDR)== SET)
  {
    if (NumberOfByteToReceive == 1)
    {
      I2C_AcknowledgeConfig(I2Cx, DISABLE);
    }
    
    /* Clear ADDR Register */
    (void)(I2Cx->SR1);
    (void)(I2Cx->SR2);  
    if (GenerateStartStatus == 0x00)
    { 
      if (NumberOfByteToReceive == 1)
      {
         I2C_GenerateSTOP(I2Cx, ENABLE);  
      }  
      
      if (NumberOfByteToReceive == 2)
      {
        I2C_AcknowledgeConfig(I2Cx, DISABLE);
        I2C_PECPositionConfig(I2Cx, I2C_PECPosition_Next);
        /* Disable buffer Interrupts */
        I2C_ITConfig(I2Cx, I2C_IT_BUF , DISABLE);
      }
    }
    
#ifdef I2C_10BITS_ADDRESS   
    if (GenerateStartStatus == 0x01)
    {
      /* Repeated Start */
      I2C_GenerateSTART(I2Cx, ENABLE);
      GenerateStartStatus = 0x00;
    }
    
#endif /* I2C_10BITS_ADDRESS */
  } 
  
  else if((I2C_GetITStatus(I2Cx, I2C_IT_RXNE)== SET)&&(I2C_GetITStatus(I2Cx, I2C_IT_BTF)== RESET))
  {
    /* Store I2C received data */
    RxBuffer[Rx_Idx++] = I2C_ReceiveData (I2Cx);
    NumberOfByteToReceive--;
    
    if (NumberOfByteToReceive == 0x03)
    {
      /* Disable buffer Interrupts */
      I2C_ITConfig(I2Cx, I2C_IT_BUF , DISABLE);
    }

    if (NumberOfByteToReceive == 0x00)
    {
      /* Disable Error and Buffer Interrupts */
      I2C_ITConfig(I2Cx, (I2C_IT_EVT | I2C_IT_BUF), DISABLE);            
    }
  }    
  /* BUSY, MSL and RXNE flags */
  else if(I2C_GetITStatus(I2Cx, I2C_IT_BTF)== SET)
  {
    /* if Three bytes remaining for reception */
    if (NumberOfByteToReceive == 3)
    {
      I2C_AcknowledgeConfig(I2Cx, DISABLE);
      /* Store I2C received data */
      RxBuffer[Rx_Idx++] = I2C_ReceiveData (I2Cx);
      NumberOfByteToReceive--;        
    } 
    else if (NumberOfByteToReceive == 2)
    {           
      I2C_GenerateSTOP(I2Cx, ENABLE);    
      
      /* Store I2C received data */
      RxBuffer[Rx_Idx++] = I2C_ReceiveData (I2Cx);
      NumberOfByteToReceive--;
      /* Store I2C received data */
      RxBuffer[Rx_Idx++] = I2C_ReceiveData (I2Cx);
      NumberOfByteToReceive--;        
      /* Disable Error and Buffer Interrupts */
      I2C_ITConfig(I2Cx, (I2C_IT_EVT | I2C_IT_BUF), DISABLE);            
    }
    else 
    {
      /* Store I2C received data */
      RxBuffer[Rx_Idx++] = I2C_ReceiveData (I2Cx);
      NumberOfByteToReceive--;
    }
  } 
#endif /* I2C_MASTER*/
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
