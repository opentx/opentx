/**
  ******************************************************************************
  * @file    FLASH/Write_Protection/main.c 
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

/** @addtogroup FLASH_Write_Protection
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_WRP_SECTORS   (OB_WRP_Sector_2 | OB_WRP_Sector_3) /* sectors 2 and 3  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t SectorsWRPStatus = 0xFFF;

/* Private function prototypes -----------------------------------------------*/
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

  /* Initialize LEDs mounted on STM322xG-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* Initialize Key Button mounted on STM322xG-EVAL board */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);

  /* Test if Key push-button on STM322xG-EVAL board is pressed */
  if (STM_EVAL_PBGetState(BUTTON_KEY) == 0x00)
  {
    /* Get FLASH_WRP_SECTORS write protection status */
    SectorsWRPStatus = FLASH_OB_GetWRP() & FLASH_WRP_SECTORS;

    if (SectorsWRPStatus == 0x00)
    {
     /* If FLASH_WRP_SECTORS are write protected, disable the write protection */

      /* Enable the Flash option control register access */
      FLASH_OB_Unlock();

      /* Disable FLASH_WRP_SECTORS write protection */
      FLASH_OB_WRPConfig(FLASH_WRP_SECTORS, DISABLE); 

      /* Start the Option Bytes programming process */  
      if (FLASH_OB_Launch() != FLASH_COMPLETE)
      {
        /* User can add here some code to deal with this error */
        while (1)
        {
        }
      }
      /* Disable the Flash option control register access (recommended to protect 
        the option Bytes against possible unwanted operations) */
      FLASH_OB_Lock();

      /* Get FLASH_WRP_SECTORS write protection status */
      SectorsWRPStatus = FLASH_OB_GetWRP() & FLASH_WRP_SECTORS;
  
      /* Check if FLASH_WRP_SECTORS write protection is disabled */
      if (SectorsWRPStatus == FLASH_WRP_SECTORS)
      {
        /* OK, turn ON LED1 */
        STM_EVAL_LEDOn(LED1); 
      }
      else
      {
        /* KO, turn ON LED3 */
        STM_EVAL_LEDOn(LED3); 
      }
    }
    else
    { /* If FLASH_WRP_SECTORS are not write protected, enable the write protection */

      /* Enable the Flash option control register access */
      FLASH_OB_Unlock();

      /* Enable FLASH_WRP_SECTORS write protection */
      FLASH_OB_WRPConfig(FLASH_WRP_SECTORS, ENABLE); 

      /* Start the Option Bytes programming process */  
      if (FLASH_OB_Launch() != FLASH_COMPLETE)
      {
        /* User can add here some code to deal with this error */
        while (1)
        {
        }
      }

      /* Disable the Flash option control register access (recommended to protect 
        the option Bytes against possible unwanted operations) */
      FLASH_OB_Lock();

      /* Get FLASH_WRP_SECTORS write protection status */
      SectorsWRPStatus = FLASH_OB_GetWRP() & FLASH_WRP_SECTORS;

      /* Check if FLASH_WRP_SECTORS are write protected */
      if (SectorsWRPStatus == 0x00)
      {
        /* OK, turn ON LED4 */
        STM_EVAL_LEDOn(LED4); 
      }
      else
      {
        /* KO, turn ON LED3 */
        STM_EVAL_LEDOn(LED3); 
      }
    }
  }

  /* Turn ON LED2 */
  STM_EVAL_LEDOn(LED2); 

  while (1)
  {
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
