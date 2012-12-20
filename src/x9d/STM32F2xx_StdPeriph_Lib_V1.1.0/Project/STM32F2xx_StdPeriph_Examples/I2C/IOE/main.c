/**
  ******************************************************************************
  * @file    I2C/IOE/main.c 
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

/** @addtogroup I2C_IOE
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
RCC_ClocksTypeDef RCC_Clocks;
    
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

  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
         
  /* Initialize LEDs and push-buttons mounted on STM322xG-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* Select the Button test mode (polling or interrupt) BUTTON_MODE in main.h */
  STM_EVAL_PBInit(BUTTON_WAKEUP, BUTTON_MODE);
  STM_EVAL_PBInit(BUTTON_TAMPER, BUTTON_MODE);
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE);

  /* Initialize the LCD */
  STM322xG_LCD_Init();
 
  /* Clear the LCD */ 
  LCD_Clear(White);
  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);
  /* Set the LCD Text Color */
  LCD_SetTextColor(White);    
 
  LCD_DisplayStringLine(Line0, (uint8_t *)"   STM322xG-EVAL    ");
  LCD_DisplayStringLine(Line1, (uint8_t *)"  Example on how to ");
  LCD_DisplayStringLine(Line2, (uint8_t *)" use the IO Expander");
  
  /* Configure the IO Expander */
  if (IOE_Config() == IOE_OK)
  {
    LCD_DisplayStringLine(Line4, (uint8_t *)"   IO Expander OK   ");
  }
  else
  {
    LCD_DisplayStringLine(Line4, (uint8_t *)"IO Expander FAILED ");
    LCD_DisplayStringLine(Line5, (uint8_t *)" Please Reset the  ");
    LCD_DisplayStringLine(Line6, (uint8_t *)"   board and start ");
    LCD_DisplayStringLine(Line7, (uint8_t *)"    again          ");
    while(1);
  }

  /* Leds Control blocks */
  LCD_SetTextColor(Blue);
  LCD_DrawRect(180, 310, 40, 60);
  LCD_SetTextColor(Red);
  LCD_DrawRect(180, 230, 40, 60);
  LCD_SetTextColor(Yellow);
  LCD_DrawRect(180, 150, 40, 60);
  LCD_SetTextColor(Green);
  LCD_DrawRect(180, 70, 40, 60);

#ifdef IOE_INTERRUPT_MODE
  /* Enable the Touch Screen and Joystick interrupts */
  IOE_ITConfig(IOE_ITSRC_JOYSTICK | IOE_ITSRC_TSC); 
#endif /* IOE_INTERRUPT_MODE */
  
  
  while(1)
  {
#ifdef IOE_POLLING_MODE
    static JOY_State_TypeDef JoyState = JOY_NONE;
    static TS_STATE* TS_State;
    
    /* Get the Joystick State */
    JoyState = IOE_JoyStickGetState();
   
    switch (JoyState)
    {
      case JOY_NONE:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY:     ----        ");
        break;
      case JOY_UP:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY:     UP         ");
        break;     
      case JOY_DOWN:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY:    DOWN        ");
        break;          
      case JOY_LEFT:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY:    LEFT        ");
        break;         
      case JOY_RIGHT:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY:    RIGHT        ");
        break;                 
      case JOY_CENTER:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY:   CENTER       ");
        break; 
      default:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY:   ERROR      ");
        break;         
    }

    /* Update the structure with the current position */
    TS_State = IOE_TS_GetState();  
    
    if ((TS_State->TouchDetected) && (TS_State->Y < 220) && (TS_State->Y > 180))
    {
      if ((TS_State->X > 10) && (TS_State->X < 70))
      {
        LCD_DisplayStringLine(Line6, (uint8_t *)" LD4                ");
        STM_EVAL_LEDOn(LED4);
      }
      else if ((TS_State->X > 90) && (TS_State->X < 150))
      {
        LCD_DisplayStringLine(Line6, (uint8_t *)"      LD3           ");
        STM_EVAL_LEDOn(LED3);
      }
      else if ((TS_State->X > 170) && (TS_State->X < 230))
      {
        LCD_DisplayStringLine(Line6, (uint8_t *)"           LD2      ");
        STM_EVAL_LEDOn(LED2);
      }     
      else if ((TS_State->X > 250) && (TS_State->X < 310))
      {
        LCD_DisplayStringLine(Line6, (uint8_t *)"                LD1 ");
        STM_EVAL_LEDOn(LED1);
      }
    }
    else
    {
      STM_EVAL_LEDOff(LED1);
      STM_EVAL_LEDOff(LED2);
      STM_EVAL_LEDOff(LED3);
      STM_EVAL_LEDOff(LED4);
    }
#endif /* IOE_POLLING_MODE */  
    
#ifdef BUTTON_POLLING_MODE
    /* Insert 10 ms delay */
    Delay(1);
    
    if (STM_EVAL_PBGetState(BUTTON_KEY) == 0)
    {
      /* Toggle LD1 */
      STM_EVAL_LEDToggle(LED1);

      LCD_DisplayStringLine(Line4, (uint8_t *)"Pol:   KEY Pressed  ");
    }

    if (STM_EVAL_PBGetState(BUTTON_TAMPER) == 0)
    {
      /* Toggle LD2 */
      STM_EVAL_LEDToggle(LED2);

      LCD_DisplayStringLine(Line4, (uint8_t *)"Pol: TAMPER Pressed ");
    }

    if (STM_EVAL_PBGetState(BUTTON_WAKEUP) != 0)
    {
      /* Toggle LD3 */
      STM_EVAL_LEDToggle(LED3);
      LCD_DisplayStringLine(Line4, (uint8_t *)"Pol: WAKEUP Pressed ");
    }
#endif
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
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

  LCD_DisplayStringLine(Line0, "assert_param error!!");
  
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
