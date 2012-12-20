/**
  ******************************************************************************
  * @file    DMA/FLASH_RAM/main.c 
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

/** @addtogroup DMA_FLASH_RAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO TestStatus  TransferStatus = FAILED;

const uint32_t SRC_Const_Buffer[BUFFER_SIZE]= {
                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};
uint32_t DST_Buffer[BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
void DMA_Config(void);
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
    
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

  /* Configure LEDs to monitor program status */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  STM_EVAL_LEDOn(LED1); /* Turn LED1 on: start of configuration */
  
  /* Configure and enable the DMA Stream for Memory to Memory transfer */
  DMA_Config();

  STM_EVAL_LEDOn(LED2); /* Turn LED2 on: start of Transfer */
  
  /* Wait the end of transmission (the DMA Stream is disabled by Hardware at the 
     end of the transfer) .
     There is also another way to check on end of transfer by monitoring the 
     number of remaining data to be transferred. */
  /* while (DMA_GetCurrentMemoryTarget(DMA_STREAM) != 0) */  /* First method */
  while (DMA_GetCmdStatus(DMA_STREAM) != DISABLE)            /* Second method */
  {
    /*
       Since this code present a simple example of how to use DMA, it is just
       waiting on the end of transfer.
       But, while DMA Stream is transferring data, the CPU is free to perform 
       other tasks in parallel to the DMA transfer.
    */
  }
   
  /* Check if the transmitted and received data are equal */
  TransferStatus = Buffercmp(SRC_Const_Buffer, DST_Buffer, BUFFER_SIZE);
  /* TransferStatus = PASSED, if the transmitted and received data 
     are the same */
  /* TransferStatus = FAILED, if the transmitted and received data 
     are different */

  if (TransferStatus != FAILED)
  {
    /* Turn LED4 on: Transfer correct */
    STM_EVAL_LEDOn(LED4);
  }
  
  while (1)
  {
  }
}

/**
  * @brief  Configure the DMA controller according to the Stream parameters
  *         defined in main.h file 
  * @param  None
  * @retval None
  */
void DMA_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
  __IO uint32_t    Timeout = TIMEOUT_MAX;
    
  /* Enable DMA clock */
  RCC_AHB1PeriphClockCmd(DMA_STREAM_CLOCK, ENABLE);
  
  /* Reset DMA Stream registers (for debug purpose) */
  DMA_DeInit(DMA_STREAM);

  /* Check if the DMA Stream is disabled before enabling it.
     Note that this step is useful when the same Stream is used multiple times:
     enabled, then disabled then re-enabled... In this case, the DMA Stream disable
     will be effective only at the end of the ongoing data transfer and it will 
     not be possible to re-configure it before making sure that the Enable bit 
     has been cleared by hardware. If the Stream is used only once, this step might 
     be bypassed. */
  while (DMA_GetCmdStatus(DMA_STREAM) != DISABLE)
  {
  }
  
  /* Configure DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_CHANNEL;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SRC_Const_Buffer;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DST_Buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA_STREAM, &DMA_InitStructure);
    
  /* Enable DMA Stream Transfer Complete interrupt */
  DMA_ITConfig(DMA_STREAM, DMA_IT_TC, ENABLE);

  /* DMA Stream enable */
  DMA_Cmd(DMA_STREAM, ENABLE);

  /* Check if the DMA Stream has been effectively enabled.
     The DMA Stream Enable bit is cleared immediately by hardware if there is an 
     error in the configuration parameters and the transfer is no started (ie. when
     wrong FIFO threshold is configured ...) */
  Timeout = TIMEOUT_MAX;
  while ((DMA_GetCmdStatus(DMA_STREAM) != ENABLE) && (Timeout-- > 0))
  {
  }
   
  /* Check if a timeout condition occurred */
  if (Timeout == 0)
  {
    /* Manage the error: to simplify the code enter an infinite loop */
    while (1)
    {
    }
  }

  /* Enable the DMA Stream IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel = DMA_STREAM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);     
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer, pBuffer1: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer identical to pBuffer1
  *         FAILED: pBuffer differs from pBuffer1
  */
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer != *pBuffer1)
    {
      return FAILED;
    }
    
    pBuffer++;
    pBuffer1++;
  }

  return PASSED;  
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
