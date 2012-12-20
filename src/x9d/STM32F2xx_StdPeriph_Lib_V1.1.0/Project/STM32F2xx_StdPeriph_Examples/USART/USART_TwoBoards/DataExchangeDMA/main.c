/**
  ******************************************************************************
  * @file    USART/USART_TwoBoards/DataExchangeDMA/main.c
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

/** @addtogroup USART_DataExchangeDMA
* @{
*/

/* Private typedef -----------------------------------------------------------*/
DMA_InitTypeDef  DMA_InitStructure;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t TxBuffer[] = "USART DMA Example: Communication between two USART using DMA";
uint8_t CmdBuffer [2] = {0x00, 0x00}; /* {Command, Number of byte to receive or to transmit} */
__IO uint32_t TimeOut = 0x0;   

#ifdef USART_TRANSMITTER_MODE
 JOYState_TypeDef PressedButton = JOY_NONE;
 JOYState_TypeDef ReleasedButton   = JOY_NONE;
#endif /* USART_TRANSMITTER_MODE */

#ifdef USART_RECEIVER_MODE
uint8_t RxBuffer [RXBUFFERSIZE];
#endif /* USART_RECEIVER_MODE */


/* Private function prototypes -----------------------------------------------*/
static void TimeOut_UserCallback(void);
static void USART_Config(void);
static void SysTickConfig(void);
static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLength);

#ifdef USART_RECEIVER_MODE
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
#endif /* USART_RECEIVER_MODE */

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
  
  /* LEDs configuration ------------------------------------------------------*/
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* IO Expanderconfiguration ------------------------------------------------*/
#ifdef USART_TRANSMITTER_MODE  
  TimeOut = USER_TIMEOUT;
  while ((IOE_Config() != IOE_OK) && (TimeOut != 0))
  {
  }
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
#endif /* USART_TRANSMITTER_MODE */
 
  while (1)
  {
/******************************************************************************/
/*                      USART in Transmitter Mode                             */           
/******************************************************************************/
#ifdef USART_TRANSMITTER_MODE  
    
    /* Clear Buffers */
    Fill_Buffer(CmdBuffer, 2);
    
    DMA_DeInit(USARTx_TX_DMA_STREAM);
    DMA_InitStructure.DMA_Channel = USARTx_TX_DMA_CHANNEL;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  
    
    /****************** USART will Transmit Specific Command ******************/ 
    /* Prepare the DMA to transfer the transaction command (2bytes) from the
       memory to the USART */  
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)CmdBuffer;
    DMA_InitStructure.DMA_BufferSize = (uint16_t)2;
    DMA_Init(USARTx_TX_DMA_STREAM, &DMA_InitStructure); 
   
    /* Prepare Command to be transmitted */
    /* Waiting joystick pressed */  
    PressedButton = JOY_NONE;  
    while (PressedButton == JOY_NONE)
    {
      PressedButton = IOE_JoyStickGetState();
    }
    
    /* Waiting joystick released */  
    ReleasedButton = IOE_JoyStickGetState();  
    while ((PressedButton == ReleasedButton) && (ReleasedButton != JOY_NONE))
    {
      ReleasedButton = IOE_JoyStickGetState();    
    }

    
    if(PressedButton != JOY_NONE)
    {
      /* For each joystick state correspond a command to be sent through USART */      
      switch (PressedButton)
      {
        /* JOY_RIGHT button pressed */
        case JOY_RIGHT:
          CmdBuffer[0] = CMD_RIGHT;
          CmdBuffer[1] = CMD_RIGHT_SIZE;
          break;
        /* JOY_LEFT button pressed */
        case JOY_LEFT:
          CmdBuffer[0] = CMD_LEFT;
          CmdBuffer[1]  = CMD_LEFT_SIZE;
          break;
        /* JOY_UP button pressed */
        case JOY_UP:
          CmdBuffer[0] = CMD_UP;
          CmdBuffer[1] = CMD_UP_SIZE;
          break;
        /* JOY_DOWN button pressed */          
        case JOY_DOWN:
          CmdBuffer[0] = CMD_DOWN;
          CmdBuffer[1] = CMD_DOWN_SIZE;
          break;
        /* JOY_SEL button pressed */
        case JOY_SEL:
          CmdBuffer[0] = CMD_SEL;
          CmdBuffer[1] = CMD_SEL_SIZE;
          break;
        default:
          break;
      }
      
      /* Enable the USART DMA requests */
      USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
      
      /* Clear the TC bit in the SR register by writing 0 to it */
      USART_ClearFlag(USARTx, USART_FLAG_TC);

      /* Enable the DMA TX Stream, USART will start sending the command code (2bytes) */
      DMA_Cmd(USARTx_TX_DMA_STREAM, ENABLE);
      
      /* Wait the USART DMA Tx transfer complete or time out */
      TimeOut = USER_TIMEOUT; 
      while ((DMA_GetFlagStatus(USARTx_TX_DMA_STREAM, USARTx_TX_DMA_FLAG_TCIF) == RESET)&&(TimeOut != 0))
      {
      }
      
      if(TimeOut == 0)
      {
        TimeOut_UserCallback();
      } 
      
      /* The software must wait until TC=1. The TC flag remains cleared during all data
         transfers and it is set by hardware at the last frame’s end of transmission*/
      TimeOut = USER_TIMEOUT;
      while ((USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)&&(TimeOut != 0))
      {
      }
      if(TimeOut == 0)
      {
        TimeOut_UserCallback();
      }      
      
      /* Clear DMA Streams flags */
      DMA_ClearFlag(USARTx_TX_DMA_STREAM, USARTx_TX_DMA_FLAG_HTIF | USARTx_TX_DMA_FLAG_TCIF);                                    
                                          
      /* Disable the DMA Streams */
      DMA_Cmd(USARTx_TX_DMA_STREAM, DISABLE);
      
      /* Disable the USART Tx DMA request */
      USART_DMACmd(USARTx, USART_DMAReq_Tx, DISABLE);
      
      /******************* USART will Transmit Data Buffer ********************/
      /* Prepare the DMA to transfer the transaction data (length defined by 
         CmdBuffer[1] variable) from the memory to the USART */   
      DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TxBuffer;
      DMA_InitStructure.DMA_BufferSize = (uint16_t)CmdBuffer[1];
      DMA_Init(USARTx_TX_DMA_STREAM, &DMA_InitStructure); 
      
      /* Enable the USART Tx DMA request */                
      USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
      
      /* Clear the TC bit in the SR register by writing 0 to it */
      USART_ClearFlag(USARTx, USART_FLAG_TC);
      
      /* Enable DMA USART Tx Stream */
      DMA_Cmd(USARTx_TX_DMA_STREAM, ENABLE);
      
      /* Wait the USART DMA Tx transfer complete or time out */
      TimeOut = USER_TIMEOUT;
      while ((DMA_GetFlagStatus(USARTx_TX_DMA_STREAM, USARTx_TX_DMA_FLAG_TCIF) == RESET)&&(TimeOut != 0))
      {
      }
      if(TimeOut == 0)
      {
        TimeOut_UserCallback();
      }
      
      /* The software must wait until TC=1, The TC flag remains cleared during all data
         transfers and it is set by hardware at the last frame’s end of transmission */
      TimeOut = USER_TIMEOUT;
      while ((USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)&&(TimeOut != 0))
      {
      }
      if(TimeOut == 0)
      {
        TimeOut_UserCallback();
      }
       
      /* Clear all DMA Streams flags */
      DMA_ClearFlag(USARTx_TX_DMA_STREAM, USARTx_TX_DMA_FLAG_HTIF | USARTx_TX_DMA_FLAG_TCIF);
                                          
      /* Disable the DMA Stream */
      DMA_Cmd(USARTx_TX_DMA_STREAM, DISABLE);
      
      /* Disable the USART Tx DMA request */
      USART_DMACmd(USARTx, USART_DMAReq_Tx, DISABLE);
    }
  
#endif /* USART_TRANSMITTER_MODE */

/******************************************************************************/
/*                      USART in Receiver Mode                                */           
/******************************************************************************/
#ifdef USART_RECEIVER_MODE
    /* Clear Buffers */
    Fill_Buffer(RxBuffer, TXBUFFERSIZE);
    Fill_Buffer(CmdBuffer, 2);
    
    DMA_DeInit(USARTx_RX_DMA_STREAM);
    DMA_InitStructure.DMA_Channel = USARTx_RX_DMA_CHANNEL;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; 
    /****************** USART will Receive Specific Command *******************/
    /* Configure the DMA to receive 2 bytes (transaction command), in case of USART receiver */  
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)CmdBuffer;
    DMA_InitStructure.DMA_BufferSize = (uint16_t)2;
    DMA_Init(USARTx_RX_DMA_STREAM, &DMA_InitStructure);
    
    /* Enable the USART Rx DMA request */
    USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);   
    /* Enable the DMA RX Stream */
    DMA_Cmd(USARTx_RX_DMA_STREAM, ENABLE);
    
    /* Wait the USART DMA Rx transfer complete (to receive the transaction command) */
    while (DMA_GetFlagStatus(USARTx_RX_DMA_STREAM, USARTx_RX_DMA_FLAG_TCIF) == RESET)
    {      
    }
      
    /* Clear all DMA Streams flags */
    DMA_ClearFlag(USARTx_RX_DMA_STREAM, USARTx_RX_DMA_FLAG_HTIF | USARTx_RX_DMA_FLAG_TCIF); 
                                        
    /* Disable the DMA Rx Stream */
    DMA_Cmd(USARTx_RX_DMA_STREAM, DISABLE);
      
    /* Disable the USART Rx DMA requests */
    USART_DMACmd(USARTx, USART_DMAReq_Rx, DISABLE);
   
    /************* USART will receive the the transaction data ****************/
    /* Transaction data (length defined by CmdBuffer[1] variable) */       
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)RxBuffer;
    DMA_InitStructure.DMA_BufferSize = (uint16_t)CmdBuffer[1];
    DMA_Init(USARTx_RX_DMA_STREAM, &DMA_InitStructure);

    /* Enable the USART Rx DMA requests */
    USART_DMACmd(USARTx, USART_DMAReq_Rx , ENABLE);
      
    /* Enable the DMA Stream */
    DMA_Cmd(USARTx_RX_DMA_STREAM, ENABLE);
      
    /* Wait the USART DMA Rx transfer complete or time out */
    TimeOut = USER_TIMEOUT;     
    while ((DMA_GetFlagStatus(USARTx_RX_DMA_STREAM, USARTx_RX_DMA_FLAG_TCIF) == RESET)&&(TimeOut != 0))
    {
    }      
    if(TimeOut == 0)
    {
      TimeOut_UserCallback();
    }
      
    /* Clear all DMA Streams flags */
    DMA_ClearFlag(USARTx_RX_DMA_STREAM, USARTx_RX_DMA_FLAG_HTIF | USARTx_RX_DMA_FLAG_TCIF);
      
    /* Disable the DMA Stream */
    DMA_Cmd(USARTx_RX_DMA_STREAM, DISABLE);
      
    /* Disable the USART Rx DMA requests */
    USART_DMACmd(USARTx, USART_DMAReq_Rx, DISABLE);
      
    switch (CmdBuffer[1])
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
#endif /* USART_RECEIVER_MODE */      
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
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(USARTx_TX_GPIO_CLK | USARTx_RX_GPIO_CLK, ENABLE);
  
  /* Enable USART clock */
  USARTx_CLK_INIT(USARTx_CLK, ENABLE);
  
  /* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(USARTx_DMAx_CLK, ENABLE);
  
  /* USARTx GPIO configuration -----------------------------------------------*/ 
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
 
  /* USARTx configuration ----------------------------------------------------*/
  /* Enable the USART OverSampling by 8 */
  USART_OverSampling8Cmd(USARTx, ENABLE); 
  
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
  /* When using Parity the word length must be configured to 9 bits */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);

  /* Configure DMA controller to manage USART TX and RX DMA request ----------*/  
  DMA_InitStructure.DMA_PeripheralBaseAddr = USARTx_DR_ADDRESS;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  /* Here only the unchanged parameters of the DMA initialization structure are
     configured. During the program operation, the DMA will be configured with 
     different parameters according to the operation phase */
         
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
  /* Set SysTick Timer for 10ms interrupts  */
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
#ifdef USART_RECEIVER_MODE
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
#endif /* USART_RECEIVER_MODE */

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
