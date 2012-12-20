/**
  ******************************************************************************
  * @file    GPIO/IOToggle/main.c 
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

/** @addtogroup IOToggle
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef  GPIO_InitStructure;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
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

  /* GPIOG Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

  /* Configure PG6 and PG8 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* To achieve GPIO toggling maximum frequency, the following  sequence is mandatory. 
     You can monitor PG6 or PG8 on the scope to measure the output signal. 
     If you need to fine tune this frequency, you can add more GPIO set/reset 
     cycles to minimize more the infinite loop timing.
     This code needs to be compiled with high speed optimization option.  */
  while (1)
  {
    /* Set PG6 and PG8 */
    GPIOG->BSRRL = GPIO_Pin_6 | GPIO_Pin_8;
    /* Reset PG6 and PG8 */
    GPIOG->BSRRH = GPIO_Pin_6 | GPIO_Pin_8;

    /* Set PG6 and PG8 */
    GPIOG->BSRRL = GPIO_Pin_6 | GPIO_Pin_8;
    /* Reset PG6 and PG8 */
    GPIOG->BSRRH = GPIO_Pin_6 | GPIO_Pin_8;

    /* Set PG6 and PG8 */
    GPIOG->BSRRL = GPIO_Pin_6 | GPIO_Pin_8;
    /* Reset PG6 and PG8 */
    GPIOG->BSRRH = GPIO_Pin_6 | GPIO_Pin_8;

    /* Set PG6 and PG8 */
    GPIOG->BSRRL = GPIO_Pin_6 | GPIO_Pin_8;
    /* Reset PG6 and PG8 */
    GPIOG->BSRRH = GPIO_Pin_6 | GPIO_Pin_8;

    /* Set PG6 and PG8 */
    GPIOG->BSRRL = GPIO_Pin_6 | GPIO_Pin_8;
    /* Reset PG6 and PG8 */
    GPIOG->BSRRH = GPIO_Pin_6 | GPIO_Pin_8;

    /* Set PG6 and PG8 */
    GPIOG->BSRRL = GPIO_Pin_6 | GPIO_Pin_8;
    /* Reset PG6 and PG8 */
    GPIOG->BSRRH = GPIO_Pin_6 | GPIO_Pin_8;

    /* Set PG6 and PG8 */
    GPIOG->BSRRL = GPIO_Pin_6 | GPIO_Pin_8;
    /* Reset PG6 and PG8 */
    GPIOG->BSRRH = GPIO_Pin_6 | GPIO_Pin_8;

    /* Set PG6 and PG8 */
    GPIOG->BSRRL = GPIO_Pin_6 | GPIO_Pin_8;
    /* Reset PG6 and PG8 */
    GPIOG->BSRRH = GPIO_Pin_6 | GPIO_Pin_8;

    /* Set PG6 and PG8 */
    GPIOG->BSRRL = GPIO_Pin_6 | GPIO_Pin_8;
    /* Reset PG6 and PG8 */
    GPIOG->BSRRH = GPIO_Pin_6 | GPIO_Pin_8;

    /* Set PG6 and PG8 */
    GPIOG->BSRRL = GPIO_Pin_6 | GPIO_Pin_8;
    /* Reset PG6 and PG8 */
    GPIOG->BSRRH = GPIO_Pin_6 | GPIO_Pin_8;
  }
}

/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
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
