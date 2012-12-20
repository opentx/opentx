/**
  ******************************************************************************
  * @file    RTC/BKP_Domain/main.c 
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

/** @addtogroup BKP_Domain
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Uncomment the corresponding line to select the RTC Clock source */
#define RTC_CLOCK_SOURCE_LSE   /* LSE used as RTC source clock */
/* #define RTC_CLOCK_SOURCE_LSI */ /* LSI used as RTC source clock. The RTC Clock
                                      may varies due to LSI frequency dispersion. */
#define RTC_BKP_DR_NUMBER   0x14
#define FIRST_DATA          0x32F2

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_InitTypeDef   RTC_InitStructure;
RTC_TimeTypeDef   RTC_TimeStructure;
RTC_DateTypeDef   RTC_DateStructure;
__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;
__IO uint32_t TimeDisplay = 0;
uint32_t errorindex = 0, i = 0;

uint32_t BKPDataReg[RTC_BKP_DR_NUMBER] =
  {
    RTC_BKP_DR0, RTC_BKP_DR1, RTC_BKP_DR2, 
    RTC_BKP_DR3, RTC_BKP_DR4, RTC_BKP_DR5,
    RTC_BKP_DR6, RTC_BKP_DR7, RTC_BKP_DR8, 
    RTC_BKP_DR9, RTC_BKP_DR10, RTC_BKP_DR11, 
    RTC_BKP_DR12, RTC_BKP_DR13, RTC_BKP_DR14, 
    RTC_BKP_DR15, RTC_BKP_DR16, RTC_BKP_DR17, 
    RTC_BKP_DR18,  RTC_BKP_DR19
  };

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
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;

  /* Configure the external interrupt "WAKEUP", "KEY" and "TAMPER" buttons */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO); 
  STM_EVAL_PBInit(BUTTON_TAMPER , BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_WAKEUP , BUTTON_MODE_GPIO);

  /* Initialize the LCD */
  STM322xG_LCD_Init();

  /* Configure the LCD Log Module */
  LCD_LOG_Init();
  LCD_LOG_SetHeader("RTC Backup Domain Example");
  LCD_LOG_SetFooter ("   Copyright (c) STMicroelectronics" );

  /* Display the default RCC BDCR and RTC TAFCR Registers */
  LCD_UsrLog ("Entry Point \n");
  LCD_UsrLog ("RCC BDCR = 0x%x\n", RCC->BDCR);
  LCD_UsrLog ("RTC TAFCR = 0x%x\n", RTC->TAFCR);

  /* Enable the PWR APB1 Clock Interface */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* EXTI configuration *******************************************************/
  EXTI_ClearITPendingBit(EXTI_Line22);
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  if(RTC_ReadBackupRegister(RTC_BKP_DR0) != FIRST_DATA)
  {
    LCD_UsrLog ("RTC Config PLZ Wait. \n");

    /* RTC Configuration */
    RTC_Config();

    /* Adjust Current Time */
    Time_Adjust();

    /* Adjust Current Date */
    Date_Adjust();
  }
  else
  {
    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line22);

/*  Backup SRAM ***************************************************************/
    /* Enable BKPSRAM Clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

    /* Check the written Data */
    for (i = 0x0; i < 0x1000; i += 4)
    {
      if ((*(__IO uint32_t *) (BKPSRAM_BASE + i)) != i)
      {
        errorindex++;
      }
    }
    if(errorindex)
    {
      LCD_ErrLog ("BKP SRAM Number of errors = %d\n", errorindex);
    }
    else
    {
      LCD_UsrLog ("BKP SRAM Content OK  \n");
    }
/* RTC Backup Data Registers **************************************************/
    /* Check if RTC Backup DRx registers data are correct */
    if (CheckBackupReg(FIRST_DATA) == 0x00)
    { 
      /* OK, RTC Backup DRx registers data are correct */
      LCD_UsrLog ("OK, RTC Backup DRx registers data are correct. \n");
    }
    else
    { 
      /* Error, RTC Backup DRx registers data are not correct */
      LCD_ErrLog ("RTC Backup DRx registers data are not correct\n");
    }
  }

  /* Infinite loop */
  Calendar_Show();

  while (1)
  {
  }
}

/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
    
#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
/* The RTC Clock may varies due to LSI frequency dispersion. */
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  
  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

#else
  #error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* Write to the first RTC Backup Data Register */
  RTC_WriteBackupRegister(RTC_BKP_DR0, FIRST_DATA);

  /* Display the new RCC BDCR and RTC TAFCR Registers */
  LCD_UsrLog ("RTC Reconfig \n");
  LCD_UsrLog ("RCC BDCR = 0x%x\n", RCC->BDCR);
  LCD_UsrLog ("RTC TAFCR = 0x%x\n", RTC->TAFCR); 

  /* Set the Time */
  RTC_TimeStructure.RTC_Hours   = 0x08;
  RTC_TimeStructure.RTC_Minutes = 0x00;
  RTC_TimeStructure.RTC_Seconds = 0x00;

  /* Set the Date */
  RTC_DateStructure.RTC_Month = RTC_Month_March;
  RTC_DateStructure.RTC_Date = 0x18;  
  RTC_DateStructure.RTC_Year = 0x11; 
  RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Friday; 

  /* Calendar Configuration */
  RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
  RTC_InitStructure.RTC_SynchPrediv =  SynchPrediv;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);
  
  /* Set Current Time and Date */
  RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);  
  RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure); 

  /* Configure the RTC Wakeup Clock source and Counter (Wakeup event each 1 second) */
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
  RTC_SetWakeUpCounter(0x7FF);
  
  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);

  /* Enable Wakeup Counter */
  RTC_WakeUpCmd(ENABLE); 

/*  Backup SRAM ***************************************************************/
  /* Enable BKPRAM Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

  /* Write to Backup SRAM with 32-Bit Data */
  for (i = 0x0; i < 0x1000; i += 4)
  {
    *(__IO uint32_t *) (BKPSRAM_BASE + i) = i;
  }
  /* Check the written Data */
  for (i = 0x0; i < 0x1000; i += 4)
  {
    if ((*(__IO uint32_t *) (BKPSRAM_BASE + i)) != i)
    {
      errorindex++;
    }
  }

  if(errorindex)
  {
    LCD_ErrLog ("BKP SRAM Number of errors = %d\n", errorindex);
  }
  else
  {
    LCD_UsrLog ("BKP SRAM write OK \n");
  }

  /* Enable the Backup SRAM low power Regulator to retain it's content in VBAT mode */
  PWR_BackupRegulatorCmd(ENABLE);

  /* Wait until the Backup SRAM low power Regulator is ready */
  while(PWR_GetFlagStatus(PWR_FLAG_BRR) == RESET)
  {
  }

/* RTC Backup Data Registers **************************************************/
  /* Write to RTC Backup Data Registers */
  WriteToBackupReg(FIRST_DATA);
}

/**
  * @brief  Reads digit entered by user, using menu navigation keys.
  * @param  None
  * @retval Digit value
  */
uint8_t ReadDigit(uint16_t LineBegin, uint16_t ColBegin, uint8_t CountBegin, uint8_t ValueMax, uint8_t ValueMin)
{
  uint8_t tmpValue = 0;
  Button_TypeDef keystate = (Button_TypeDef)10;
  
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_RED);

  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);

  /* Initialize tmpValue */
  tmpValue = CountBegin;

  /* Display  */
  LCD_DisplayChar(LineBegin, ColBegin, (tmpValue + 0x30));

  /* Endless loop */
  while(1)
  {
    /* Get the Joytick State */
    keystate = ReadKey();

    /* If "BUTTON_TAMPER" push button is pressed */
    if(keystate == BUTTON_TAMPER)
    {
      /* Increase the value of the digit */
      if(tmpValue == ValueMax)
      {
        tmpValue = (ValueMin - 1);
      }
      /* Display new value */
      LCD_DisplayChar(LineBegin, ColBegin,((++tmpValue) + 0x30));
    }
    /* If "BUTTON_WAKEUP" push button is pressed */
    if(keystate == BUTTON_WAKEUP)
    {
      /* Decrease the value of the digit */
      if(tmpValue == ValueMin)
      {
        tmpValue = (ValueMax + 1);
      }
      /* Display new value */
      LCD_DisplayChar(LineBegin, ColBegin,((--tmpValue) + 0x30));
    }
    /* If "BUTTON_KEY" push button is pressed */
    if(keystate == BUTTON_KEY)
    {
      /* Set the Back Color */
      LCD_SetBackColor(LCD_COLOR_WHITE);
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_RED);
      /* Display new value */
      LCD_DisplayChar(LineBegin, ColBegin, (tmpValue + 0x30));
      /* Return the digit value and exit */
      return tmpValue;
    }
  } 
}

/**
  * @brief  Returns the time entered by user, using menu navigation keys.
  * @param  None
  * @retval Current time RTC counter value
  */
void Time_Regulate(void)
{
  uint8_t Tmp_HH = 0, Tmp_MM = 0, Tmp_SS = 0;

  LCD_DisplayStringLine(LCD_LINE_12, "Set time: hh:mm:ss");  

  /* Read time hours */
  Tmp_HH = ReadDigit(LCD_LINE_13, 244, (RTC_TimeStructure.RTC_Hours / 10), 0x2, 0x0);

  if(Tmp_HH == 2)
  {
    if((RTC_TimeStructure.RTC_Hours % 10) > 3)
    {
      RTC_TimeStructure.RTC_Hours = 0;
    }
    Tmp_HH = Tmp_HH * 10 + ReadDigit(LCD_LINE_13, 228, (RTC_TimeStructure.RTC_Hours % 10), 0x3, 0x0);
  }
  else
  {
    Tmp_HH = Tmp_HH * 10 + ReadDigit(LCD_LINE_13, 228, (RTC_TimeStructure.RTC_Hours % 10), 0x9, 0x0);
  }
  /* Read time  minutes */
  Tmp_MM = ReadDigit(LCD_LINE_13, 196, (RTC_TimeStructure.RTC_Minutes / 10), 5, 0x0);
  Tmp_MM = Tmp_MM * 10 + ReadDigit(LCD_LINE_13, 182, (RTC_TimeStructure.RTC_Minutes % 10), 0x9, 0x0);

  /* Read time seconds */
  Tmp_SS = ReadDigit(LCD_LINE_13, 150, (RTC_TimeStructure.RTC_Seconds / 10), 5, 0x0);
  Tmp_SS = Tmp_SS * 10 + ReadDigit(LCD_LINE_13, 134, (RTC_TimeStructure.RTC_Seconds % 10), 0x9, 0x0);

  RTC_TimeStructure.RTC_Hours = Tmp_HH;
  RTC_TimeStructure.RTC_Minutes = Tmp_MM;
  RTC_TimeStructure.RTC_Seconds = Tmp_SS;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);

  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLACK);

  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);

  /* Clear Line12 */
  LCD_ClearLine(LCD_LINE_12);
}

/**
  * @brief  Returns the time entered by user, using demo board keys.
  * @param  None
  * @retval None
  */
void Time_Adjust(void)
{
  /* Display the current time */
  Time_Display();

  /* Change the current time */
  Time_Regulate();
}

/**
  * @brief  Displays the current time.
  * @param  None
  * @retval None
  */
void Time_Display(void)
{
  /* Clear Line13 */
  LCD_ClearLine(LCD_LINE_13);

  /* Display time separators ":" on Line4 */
  LCD_DisplayChar(LCD_LINE_13, 212, ':');
  LCD_DisplayChar(LCD_LINE_13, 166, ':');

  /* Get the current Time */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  
  /* Display time hours */
  LCD_DisplayChar(LCD_LINE_13, 244,((RTC_TimeStructure.RTC_Hours / 10) + 0x30));
  LCD_DisplayChar(LCD_LINE_13, 228,((RTC_TimeStructure.RTC_Hours % 10) + 0x30));

  /* Display time minutes */
  LCD_DisplayChar(LCD_LINE_13, 196,((RTC_TimeStructure.RTC_Minutes /10) + 0x30));
  LCD_DisplayChar(LCD_LINE_13, 182,((RTC_TimeStructure.RTC_Minutes % 10) + 0x30));

  /* Display time seconds */
  LCD_DisplayChar(LCD_LINE_13, 150,((RTC_TimeStructure.RTC_Seconds / 10) + 0x30));
  LCD_DisplayChar(LCD_LINE_13, 134,((RTC_TimeStructure.RTC_Seconds % 10) + 0x30));
}

/**
  * @brief  Returns the time entered by user, using menu navigation keys.
  * @param  None
  * @retval Current date value
  */
void Date_Regulate(void)
{
  uint8_t weekday = 0, date = 0, month = 0, year = 0;

  LCD_DisplayStringLine(LCD_LINE_15, "Set date: Weekday / Date / Month / Year");  
  
  /* Read Date Weekday */
  weekday = ReadDigit(LCD_LINE_16, 276, (RTC_DateStructure.RTC_WeekDay / 10), 0x7, 0x1);

  /* Read Date Day */
  date = ReadDigit(LCD_LINE_16, 244, (RTC_DateStructure.RTC_Date / 10), 3, 0x0);
  
  if(date == 3)
  {
    if((RTC_DateStructure.RTC_Date % 10) > 1)
    {
      RTC_DateStructure.RTC_Date = 0;
    }
    date = date * 10 + ReadDigit(LCD_LINE_16, 228, (RTC_DateStructure.RTC_Date % 10), 0x1, 0x0);
  }
  else
  {
    date = date * 10 + ReadDigit(LCD_LINE_16, 228, (RTC_DateStructure.RTC_Date % 10), 0x9, 0x0);
  }

  /* Read Date Month */
  month = ReadDigit(LCD_LINE_16, 196, (RTC_DateStructure.RTC_Month / 10), 1, 0x0);
  
  if(month == 1)
  {
    if((RTC_DateStructure.RTC_Month % 10) > 2)
    {
      RTC_DateStructure.RTC_Month = 0;
    }
    month = month * 10 + ReadDigit(LCD_LINE_16, 182, (RTC_DateStructure.RTC_Month % 10), 0x2, 0x0);
  }
  else
  {
    month = month * 10 + ReadDigit(LCD_LINE_16, 182, (RTC_DateStructure.RTC_Month % 10), 0x9, 0x0);
  }

  /* Read Date Year */
  LCD_DisplayChar(LCD_LINE_16, 150, '2');
  LCD_DisplayChar(LCD_LINE_16, 134, '0');
  year = ReadDigit(LCD_LINE_16, 118, (RTC_DateStructure.RTC_Year / 10), 0x9, 0x0);
  year = year * 10 + ReadDigit(LCD_LINE_16, 102, (RTC_DateStructure.RTC_Year % 10), 0x9, 0x0);

  RTC_DateStructure.RTC_WeekDay = weekday;
  RTC_DateStructure.RTC_Date = date;
  RTC_DateStructure.RTC_Month = month;
  RTC_DateStructure.RTC_Year = year;
  RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);

  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLACK);

  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);

  /* Clear Line15 */
  LCD_ClearLine(LCD_LINE_15);
}

/**
  * @brief  Returns the date entered by user, using demoboard keys.
  * @param  None
  * @retval None
  */
void Date_Adjust(void)
{
  /* Display the current Date */
  Date_Display();

  /* Change the current Date */
  Date_Regulate();
}

/**
  * @brief  Displays the current date.
  * @param  None
  * @retval None
  */
void Date_Display(void)
{
  /* Clear Line16 */
  LCD_ClearLine(LCD_LINE_16);

  /* Display time separators "/" on Line14 */
  LCD_DisplayChar(LCD_LINE_16, 260, '/');
  LCD_DisplayChar(LCD_LINE_16, 212, '/');
  LCD_DisplayChar(LCD_LINE_16, 166, '/');

  /* Get the current Date */
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

  /* Display Date WeekDay */
  LCD_DisplayChar(LCD_LINE_16, 276,((RTC_DateStructure.RTC_WeekDay) + 0x30));

  /* Display Date Day */
  LCD_DisplayChar(LCD_LINE_16, 244,((RTC_DateStructure.RTC_Date /10) + 0x30));
  LCD_DisplayChar(LCD_LINE_16, 228,((RTC_DateStructure.RTC_Date % 10) + 0x30));

  /* Display Date Month */
  LCD_DisplayChar(LCD_LINE_16, 196,((RTC_DateStructure.RTC_Month / 10) + 0x30));
  LCD_DisplayChar(LCD_LINE_16, 182,((RTC_DateStructure.RTC_Month % 10) + 0x30));
  
  /* Display Date Year */
  LCD_DisplayChar(LCD_LINE_16, 150, '2');
  LCD_DisplayChar(LCD_LINE_16, 134, '0');
  LCD_DisplayChar(LCD_LINE_16, 118,((RTC_DateStructure.RTC_Year / 10) + 0x30));
  LCD_DisplayChar(LCD_LINE_16, 102,((RTC_DateStructure.RTC_Year % 10) + 0x30));
}

/**
  * @brief  Shows the current time and date on LCD.
  * @param  None
  * @retval None
  */
void Calendar_Show(void)
{
  while(1)
  {
    /* If 1s has elapsed */
    if (TimeDisplay == 1)
    {
      /* Display current time */
      Time_Display();

      /* Display current date */
      Date_Display();

      TimeDisplay = 0;
    }
  }
}

/**
  * @brief  Writes data to all Backup data registers.
  * @param  FirstBackupData: data to write to first backup data register.
  * @retval None
  */
void WriteToBackupReg(uint16_t FirstBackupData)
{
  uint32_t index = 0;

  for (index = 0; index < RTC_BKP_DR_NUMBER; index++)
  {
    RTC_WriteBackupRegister(BKPDataReg[index], FirstBackupData + (index * 0x5A));
  }

}

/**
  * @brief  Checks if the Backup data registers values are correct or not.
  * @param  FirstBackupData: data to read from first backup data register
  * @retval - 0: All Backup DRx registers data are correct
  *         - Value different from 0: Number of the first Backup register which 
  *           value is not correct
  */
uint32_t CheckBackupReg(uint16_t FirstBackupData)
{
  uint32_t index = 0;

  for (index = 0; index < RTC_BKP_DR_NUMBER; index++)
  {
    if (RTC_ReadBackupRegister(BKPDataReg[index]) != (FirstBackupData + (index * 0x5A)))
    {
      return (index + 1);
    }
  }

  return 0;
}

/**
  * @brief  Reads key from demoboard.
  * @param  None
  * @retval Return KEY, TAMPER, WAKEUP.
  */
Button_TypeDef ReadKey(void)
{
  /* "BUTTON_TAMPER" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_TAMPER))
  {
    while (STM_EVAL_PBGetState(BUTTON_TAMPER) == Bit_RESET);
    return BUTTON_TAMPER;
  }
  /* "BUTTON_KEY" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_KEY))
  {
    while (STM_EVAL_PBGetState(BUTTON_KEY) == Bit_RESET);
    return BUTTON_KEY;
  }
  /* "BUTTON_WAKEUP" key is pressed */
  if (STM_EVAL_PBGetState(BUTTON_WAKEUP))
  {
    while (STM_EVAL_PBGetState(BUTTON_WAKEUP) != Bit_RESET);
    return BUTTON_WAKEUP;
  }
  /* No key is pressed */
  else
  {
    return (Button_TypeDef)10;
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
