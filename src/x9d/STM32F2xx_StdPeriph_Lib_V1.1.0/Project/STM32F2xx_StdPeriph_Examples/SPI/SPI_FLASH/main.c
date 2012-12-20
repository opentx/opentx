/**
  ******************************************************************************
  * @file    SPI/SPI_FLASH/main.c 
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
#include "stm322xg_eval.h"
#include "spi_flash.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_FLASH
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define  FLASH_WRITE_ADDRESS      0x700000
#define  FLASH_READ_ADDRESS       FLASH_WRITE_ADDRESS
#define  FLASH_SECTOR_TO_ERASE    FLASH_WRITE_ADDRESS

#define  BufferSize (countof(Tx_Buffer)-1)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t Tx_Buffer[] = "STM32F2xx SPI Firmware Library Example: communication with an M25P SPI FLASH";
uint8_t  Rx_Buffer[BufferSize];
__IO uint8_t Index = 0x0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = PASSED;
__IO uint32_t FlashID = 0;

/* Private functions ---------------------------------------------------------*/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

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
       
  /* Initialize Leds mounted on STM322xG-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);

  /* Initialize the SPI FLASH driver */
  sFLASH_Init();

  /* Get SPI Flash ID */
  FlashID = sFLASH_ReadID();
  
  /* Check the SPI Flash ID */
  if (FlashID == sFLASH_M25P64_ID)
  {
    /* OK: Turn on LD1 */
    STM_EVAL_LEDOn(LED1);

    /* Perform a write in the Flash followed by a read of the written data */
    /* Erase SPI FLASH Sector to write on */
    sFLASH_EraseSector(FLASH_SECTOR_TO_ERASE);

    /* Write Tx_Buffer data to SPI FLASH memory */
    sFLASH_WriteBuffer(Tx_Buffer, FLASH_WRITE_ADDRESS, BufferSize);

    /* Read data from SPI FLASH memory */
    sFLASH_ReadBuffer(Rx_Buffer, FLASH_READ_ADDRESS, BufferSize);

    /* Check the correctness of written dada */
    TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
    /* TransferStatus1 = PASSED, if the transmitted and received data by SPI1
       are the same */
    /* TransferStatus1 = FAILED, if the transmitted and received data by SPI1
       are different */

    /* Perform an erase in the Flash followed by a read of the written data */
    /* Erase SPI FLASH Sector to write on */
    sFLASH_EraseSector(FLASH_SECTOR_TO_ERASE);

    /* Read data from SPI FLASH memory */
    sFLASH_ReadBuffer(Rx_Buffer, FLASH_READ_ADDRESS, BufferSize);

    /* Check the correctness of erasing operation dada */
    for (Index = 0; Index < BufferSize; Index++)
    {
      if (Rx_Buffer[Index] != 0xFF)
      {
        TransferStatus2 = FAILED;
      }
    }
    /* TransferStatus2 = PASSED, if the specified sector part is erased */
    /* TransferStatus2 = FAILED, if the specified sector part is not well erased */
  }
  else
  {
    /* Error: Turn on LD2 */
    STM_EVAL_LEDOn(LED2);
  }
  
  while (1)
  {}
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
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
