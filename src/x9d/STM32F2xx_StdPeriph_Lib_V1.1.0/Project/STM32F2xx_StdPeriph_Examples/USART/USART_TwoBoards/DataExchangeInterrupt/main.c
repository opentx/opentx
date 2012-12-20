/**
  ******************************************************************************
  * @file    USART/USART_TwoBoards/DataExchangeInterrupt/main.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main program body
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
#include "main.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_DataExchangeInterrupt
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t TxBuffer[] = "USART Interrupt Example: Communication between two USART using Interrupt";
uint8_t RxBuffer [RXBUFFERSIZE];
__IO uint8_t RxIndex = 0x00;
__IO uint8_t TxIndex = 0x00;

__IO JOYState_TypeDef PressedButton  = JOY_NONE;
__IO uint8_t UsartMode = USART_MODE_TRANSMITTER;
__IO uint8_t UsartTransactionType = USART_TRANSACTIONTYPE_CMD;

uint8_t CmdBuffer [0x02] = {0x00, 0x00}; /* {Transaction Command, 
Number of byte to receive or to transmit} */
uint8_t AckBuffer [0x02] = {0x00, 0x00};  /* {Transaction Command, ACK command} */

__IO uint32_t TimeOut = 0x00;  
/* Private function prototypes -----------------------------------------------*/
static void USART_Config(void);
static void SysTickConfig(void);
static void TimeOut_UserCallback(void);
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLength);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f2xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f2xx.c file
     */
  
  /* USART configuration -----------------------------------------------------*/
  USART_Config();
  
  /* SysTick configuration ---------------------------------------------------*/
  SysTickConfig();
  
  /* Initialize LEDs mounted on STM322xG-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  /* Configure the IO Expander mounted on STM322xG-EVAL board */
  TimeOut = USER_TIMEOUT;
  while ((IOE_Config() != IOE_OK) && (TimeOut != 0x00))
  {}
  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Enable the USARTx Receive interrupt: this interrupt is generated when the
  USARTx receive data register is not empty */
  USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
  
  while (1)
  {
    TxIndex = 0x00;
    RxIndex = 0x00;
    UsartTransactionType = USART_TRANSACTIONTYPE_CMD; 
    UsartMode = USART_MODE_RECEIVER;
    
    Fill_Buffer(CmdBuffer, 0x02);
    Fill_Buffer(AckBuffer, 0x02);
    
    /* Clear the RxBuffer */
    Fill_Buffer(RxBuffer, TXBUFFERSIZE);
    
    PressedButton = IOE_JoyStickGetState();
    
    /* Waiting Joystick is pressed or transaction command is received */ 
    while ((PressedButton == JOY_NONE) && (CmdBuffer[0x00] == 0x00))
    {
      PressedButton = IOE_JoyStickGetState();
    }

    /* 
      If the Joystick is pressed go to transmitter mode, otherwise (the transaction 
      command is received) go to receiver mode 
    */
    
/******************************************************************************/
/*                      USART in Mode Transmitter                             */           
/******************************************************************************/
    if ((PressedButton != JOY_NONE) && (CmdBuffer[0x00] == 0x00))
    {
      UsartMode = USART_MODE_TRANSMITTER;
      switch (PressedButton)
      {
        /* JOY_RIGHT button pressed */
        case JOY_RIGHT:
          CmdBuffer[0x00] = CMD_RIGHT;
          CmdBuffer[0x01] = CMD_RIGHT_SIZE;
          break;
        /* JOY_LEFT button pressed */
        case JOY_LEFT:
          CmdBuffer[0x00] = CMD_LEFT;
          CmdBuffer[0x01]  = CMD_LEFT_SIZE;
          break;
        /* JOY_UP button pressed */
        case JOY_UP:
          CmdBuffer[0x00] = CMD_UP;
          CmdBuffer[0x01] = CMD_UP_SIZE;
          break;
        /* JOY_DOWN button pressed */          
        case JOY_DOWN:
          CmdBuffer[0x00] = CMD_DOWN;
          CmdBuffer[0x01] = CMD_DOWN_SIZE;
          break;
        /* JOY_SEL button pressed */
        case JOY_SEL:
          CmdBuffer[0x00] = CMD_SEL;
          CmdBuffer[0x01] = CMD_SEL_SIZE;
          break;
        default:
          break;
      }
      
      if (CmdBuffer[0x00]!= 0x00)
      {
        /* Enable the USARTx transmit data register empty interrupt */
        USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
        
        /* Wait until USART sends the command or time out */
        TimeOut = USER_TIMEOUT;
        while ((TxIndex < 0x02)&&(TimeOut != 0x00))
        {}
        if(TimeOut == 0)
        {
          TimeOut_UserCallback();
        }
        
        /* The software must wait until TC=1. The TC flag remains cleared during all data
           transfers and it is set by hardware at the last frame’s end of transmission*/
        TimeOut = USER_TIMEOUT;
        while ((USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)&&(TimeOut != 0x00))
        {
        }
        if(TimeOut == 0)
        {
          TimeOut_UserCallback();
        }
        
        /* Wait until USART receives the Ack command  or time out*/
        TimeOut = USER_TIMEOUT;
        while ((RxIndex < 0x02)&&(TimeOut != 0x00))
        {}
        if(TimeOut == 0)
        {
          TimeOut_UserCallback();
        } 
        /* USART sends the data */
        UsartTransactionType = USART_TRANSACTIONTYPE_DATA; 
        TxIndex = 0x00;
        RxIndex = 0x00;
        
        /* Enable the USARTx transmit data register empty interrupt */
        USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
        
        /* Wait until end of data transfer */
        TimeOut = USER_TIMEOUT;
        while ((TxIndex < GetVar_NbrOfData())&&(TimeOut != 0x00))
        {}
        if(TimeOut == 0)
        {
          TimeOut_UserCallback();
        } 
        
        /* The software must wait until TC=1. The TC flag remains cleared during all data
           transfers and it is set by hardware at the last frame’s end of transmission*/
        TimeOut = USER_TIMEOUT;
        while ((USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)&&(TimeOut != 0x00))
        {
        }
        if(TimeOut == 0)
        {
          TimeOut_UserCallback();
        }
      }
      CmdBuffer[0x00] = 0x00;
    }
    
/******************************************************************************/
/*                      USART in Receiver Mode                                */           
/******************************************************************************/
    if (CmdBuffer[0x00] != 0x00)
    {
      /* Wait until USART receives the command  or time out */
      TimeOut = USER_TIMEOUT;
      while ((RxIndex < 0x02)&&(TimeOut != 0x00))
      {}
      if(TimeOut == 0)
      {
        TimeOut_UserCallback();
      } 
      UsartMode = USART_MODE_RECEIVER;
      
      /* Enable the USARTx transmit data register empty interrupt */
      USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
      
      /* Wait until USART sends the ACK command or time out*/
      TimeOut = USER_TIMEOUT;
      while ((TxIndex < 0x02)&&(TimeOut != 0x00))
      {}
      if(TimeOut == 0)
      {
        TimeOut_UserCallback();
      } 
      
      /* The software must wait until TC=1. The TC flag remains cleared during all data
         transfers and it is set by hardware at the last frame’s end of transmission*/
      TimeOut = USER_TIMEOUT;
      while ((USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)&&(TimeOut != 0x00))
      {
      }
      if(TimeOut == 0)
      {
        TimeOut_UserCallback();
      }
      
      /* USART receives the data */
      UsartTransactionType = USART_TRANSACTIONTYPE_DATA; 
      TxIndex = 0x00;
      RxIndex = 0x00;
      
      /* Wait until end of data transfer or time out */
      TimeOut = USER_TIMEOUT;
      while ((RxIndex < GetVar_NbrOfData())&&(TimeOut != 0x00))
      {}
      if(TimeOut == 0)
      {
        TimeOut_UserCallback();
      } 
      switch (CmdBuffer[0x01])
      {
        /* CMD_RIGHT command received */
        case CMD_RIGHT_SIZE:
          if (Buffercmp(TxBuffer, RxBuffer, CMD_RIGHT_SIZE) != FAILED)
          {
            /* Turn ON LED2 and LED3 */
            STM_EVAL_LEDOn(LED2);
            STM_EVAL_LEDOn(LED3);
            /* Turn OFF LED4 */
            STM_EVAL_LEDOff(LED4);
          }
          break;
        /* CMD_LEFT command received */
        case CMD_LEFT_SIZE:
          if (Buffercmp(TxBuffer, RxBuffer, CMD_LEFT_SIZE) != FAILED)
          {
            /* Turn ON LED4 */
            STM_EVAL_LEDOn(LED4);
            /* Turn OFF LED2 and LED3 */
            STM_EVAL_LEDOff(LED2);
            STM_EVAL_LEDOff(LED3);
          }
          break;
        /* CMD_UP command received */
        case CMD_UP_SIZE:
          if (Buffercmp(TxBuffer, RxBuffer, CMD_UP_SIZE) != FAILED)
          {
            /* Turn ON LED2 */
            STM_EVAL_LEDOn(LED2);
            /* Turn OFF LED3 and LED4 */
            STM_EVAL_LEDOff(LED3);
            STM_EVAL_LEDOff(LED4);
          }
          break;
        /* CMD_DOWN command received */
        case CMD_DOWN_SIZE:
          if (Buffercmp(TxBuffer, RxBuffer, CMD_DOWN_SIZE) != FAILED)
          {
            /* Turn ON LED3 */
            STM_EVAL_LEDOn(LED3);
            /* Turn OFF LED2 and LED4 */
            STM_EVAL_LEDOff(LED2);
            STM_EVAL_LEDOff(LED4);
          }
          break;
        /* CMD_SEL command received */
        case CMD_SEL_SIZE:
          if (Buffercmp(TxBuffer, RxBuffer, CMD_SEL_SIZE) != FAILED) 
          {
            /* Turn ON all LED2, LED3 and LED4 */
            STM_EVAL_LEDOn(LED2);
            STM_EVAL_LEDOn(LED3);
            STM_EVAL_LEDOn(LED4);
          }
          break;
        default:
          break;
      }
      CmdBuffer[0x00] = 0x00;
    }
  }
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
static void TimeOut_UserCallback(void)
{
  /* User can add his own implementation to manage TimeOut Communication failure */
  /* Block communication and all processes */
  while (1)
  {   
  }
}

/**
  * @brief  Configures the USART Peripheral.
  * @param  None
  * @retval None
  */
static void USART_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(USARTx_TX_GPIO_CLK | USARTx_RX_GPIO_CLK, ENABLE);
  
  /* Enable USART clock */
  USARTx_CLK_INIT(USARTx_CLK, ENABLE);
  
  /* Connect USART pins to AF7 */
  GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);
  GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);
  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
  GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
  GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

  /* Enable the USART OverSampling by 8 */
  USART_OverSampling8Cmd(USARTx, ENABLE);  

  /* USARTx configuration ----------------------------------------------------*/
  /* USARTx configured as follow:
        - BaudRate = 3750000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 8
		     is: (USART APB Clock / 8) 
			 Example: 
			    - (USART3 APB1 Clock / 8) = (30 MHz / 8) = 3750000 baud
			    - (USART1 APB2 Clock / 8) = (60 MHz / 8) = 7500000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 16
		     is: (USART APB Clock / 16) 
			 Example: (USART3 APB1 Clock / 16) = (30 MHz / 16) = 1875000 baud
			 Example: (USART1 APB2 Clock / 16) = (60 MHz / 16) = 3750000 baud
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 3750000;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);
  
  /* NVIC configuration */
  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable USART */
  USART_Cmd(USARTx, ENABLE);
}

/**
  * @brief  Configures the SysTick Base time to 10 ms.
  * @param  None
  * @retval None
  */
static void SysTickConfig(void)
{
  /* Setup SysTick Timer for 10ms interrupts  */
  if (SysTick_Config(SystemCoreClock / 100))
  {
    /* Capture error */
    while (1);
  }
  /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    pBuffer1++;
    pBuffer2++;
  }
  
  return PASSED;
}
/**
  * @brief  Returns NbrOfData value.
  * @param  None
  * @retval Tx Buffer Size (NbrOfDataToTransfer).
  */
uint8_t GetVar_NbrOfData(void)
{
  return CmdBuffer[0x01];
}
/**
  * @brief  Fills buffer.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferLength: size of the buffer to fill
  * @retval None
  */
static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLength)
{
  uint16_t index = 0;
  
  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++ )
  {
    pBuffer[index] = 0x00;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
