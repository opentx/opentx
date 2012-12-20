/**
  ******************************************************************************
  * @file    NVIC/DMA_WFIMode/main.c 
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
#include "stm32f2xx.h"
#include "stm322xg_eval.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup DMA_WFIMode
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USART3_DR_ADDRESS    ((uint32_t)0x40004804)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LowPowerMode = 0;
uint16_t DstBuffer[10]= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* Private function prototypes -----------------------------------------------*/
void DMA_Config(void);
void USART_Config(void);
uint8_t Buffercmp16(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);
void Delay(__IO uint32_t nCount);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
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

  /* Initialize Leds and Key Button mounted on STM322xG-EVAL board */       
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_EXTI); 

  /* DMA1 Stream1 channel4 (connected to USART3_RX) configuration ------------*/
  DMA_Config();

  /* EVAL_COM1 (USART3) configuration ----------------------------------------*/
  USART_Config();  
    
  while (1)
  {
    if(LowPowerMode == 1)
    {
      /* Turn Off LED2 and LED3 */
      STM_EVAL_LEDOff(LED2);
      STM_EVAL_LEDOff(LED3);

      /* Request to enter WFI mode */
      __WFI();
      LowPowerMode = 0;
    }

    Delay(0xFFFFF);
    STM_EVAL_LEDToggle(LED1);
  }
}

/**
  * @brief  Configures the EVAL_COM1 (USART3).
  * @param  None
  * @retval None
  */
void USART_Config(void)
{
  /* EVAL_COM1 configuration ---------------------------------------------------*/
  /* EVAL_COM1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitTypeDef  USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);

  /* Enable DMA Rx request */
  USART_DMACmd(EVAL_COM1, USART_DMAReq_Rx, ENABLE);
}

/**
  * @brief  Configures DMA1 Stream1 channel4 to handle USART3_RX DMA request.
  * @param  None
  * @retval None
  */
void DMA_Config(void)
{
  DMA_InitTypeDef  DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable DMA1 clock ********************************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

  /* DMA1 Stream1 channel4 configuration **************************************/
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART3_DR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DstBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 10;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream1, &DMA_InitStructure);
  DMA_Cmd(DMA1_Stream1, ENABLE);

  /* Enable DMA1_Stream1 Transfer complete interrupt */
  DMA_ITConfig(DMA1_Stream1, DMA_IT_TC, ENABLE);
  
  /* Enable DMA1_Stream1 */
  DMA_Cmd(DMA1_Stream1, ENABLE);
    
  /* Enable DMA1_Stream1 IRQn Interrupt ***************************************/
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0: pBuffer1 identical to pBuffer2
  *         1: pBuffer1 differs from pBuffer2
  */
uint8_t Buffercmp16(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return 0;
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
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
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
