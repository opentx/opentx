/**
  ******************************************************************************
  * @file    GPIO/JTAG_Remap/main.c 
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

/** @addtogroup GPIO_JTAG_Remap
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;

/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
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
           
  /* Initialize LEDs and Key Button mounted on STM322xG-EVAL board */       
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);

  /* Test if Key push-button on STM322xG-EVAL board is pressed */
  if (STM_EVAL_PBGetState(BUTTON_KEY) == 0x00)
  { /* Key is pressed */

    /* Enable GPIOA and GPIOB clocks */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB , ENABLE);

    /* Turn on LED1 */
    STM_EVAL_LEDOn(LED1);

    /* Configure PA.13 (JTMS/SWDIO), PA.14 (JTCK/SWCLK) and PA.15 (JTDI) as 
       output push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure PB.03 (JTDO) and PB.04 (JTRST) as output push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    while (1)
    {
      /* Toggle JTMS/SWDAT pin */
      GPIO_WriteBit(GPIOA, GPIO_Pin_13, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_13)));
      /* Insert delay */
      Delay(0x5FFFF);

      /* Toggle JTCK/SWCLK pin */
      GPIO_WriteBit(GPIOA, GPIO_Pin_14, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_14)));
      /* Insert delay */
      Delay(0x5FFFF);

      /* Toggle JTDI pin */
      GPIO_WriteBit(GPIOA, GPIO_Pin_15, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_15)));
      /* Insert delay */
      Delay(0x5FFFF);

      /* Toggle JTDO pin */
      GPIO_WriteBit(GPIOB, GPIO_Pin_3, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_3)));
      /* Insert delay */
      Delay(0x5FFFF);

      /* Toggle JTRST pin */
      GPIO_WriteBit(GPIOB, GPIO_Pin_4, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_4)));
      /* Insert delay */
      Delay(0x5FFFF);
    }
  }
  else
  {
    /* Turn on LED2 */
    STM_EVAL_LEDOn(LED2);

    while (1)
    {
    }
  }
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
