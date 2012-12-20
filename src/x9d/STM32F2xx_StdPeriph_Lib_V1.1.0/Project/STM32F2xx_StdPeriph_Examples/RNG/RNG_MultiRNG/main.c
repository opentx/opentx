/**
  ******************************************************************************
  * @file    RNG/RNG_MultiRNG/main.c
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
#include "stm322xg_eval_lcd.h"
#include <stdio.h>

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup RNG_MultiRNG
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/

//#define PRINT_ON_USART
#define PRINT_ON_LCD

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void RNG_Config(void);
void Display_Init(void);
void Display(uint32_t rng, uint8_t line);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
 uint32_t random32bit = 0;
 uint8_t Counter = 0;

  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f2xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f2xx.c file
     */

  /* Display init (LCD or/and USART)*/
  Display_Init();
  
  /* Key Button configuration */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /* RNG configuration */
  RNG_Config();

  while (1)
  {
    /* Wait until Key button is pressed */
    while(STM_EVAL_PBGetState(BUTTON_KEY) != RESET)
    {
    }
    /* Loop while Key button is maintained pressed */
    while(STM_EVAL_PBGetState(BUTTON_KEY) == RESET)
    {
    }

    for(Counter = 0; Counter < 8; Counter++)
    {
      /* Wait until one RNG number is ready */
      while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET)
      {
      }

      /* Get a 32bit Random number */       
      random32bit = RNG_GetRandomNumber();

      /* Display the Random number value on the LCD or/and USART */
      Display(random32bit, Counter+1);
    }
  }
}

/**
  * @brief  RNG configuration
  * @param  None
  * @retval None
  */
void RNG_Config(void)
{  
 /* Enable RNG clock source */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

  /* RNG Peripheral enable */
  RNG_Cmd(ENABLE);
}

/**
  * @brief  Display Init (LCD or/and USART)
  * @param  None
  * @retval None
  */
void Display_Init(void)
{
#ifdef PRINT_ON_USART 

 USART_InitTypeDef USART_InitStructure;
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);

  printf("\n\r ========================================\n");
  printf("\n\r ==== Multiple RNG Generator Example ====\n");
  printf("\n\r ======================================== \n\n\r");
  printf("\n\r  Press key button to generate 8 x 32bit random number\n");
#endif

#ifdef PRINT_ON_LCD  
/* Initialize the LCD */
  STM322xG_LCD_Init();

  /* Clear the LCD */ 
  LCD_Clear(White);

  /* Set the LCD Text size */
  LCD_SetFont(&Font8x12);

  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);

  LCD_DisplayStringLine(LINE(0x13), "  To generate 8x32bit RNG, Press Key  >>");

  /* Set the LCD Text size */
  LCD_SetFont(&Font16x24);

  LCD_DisplayStringLine(LINE(0), "*** RNG  Example ***");

  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue); 

  LCD_DisplayStringLine(LINE(3),"  Press KEY button ");
  LCD_DisplayStringLine(LINE(5),"     to START     ");
#endif
}

/**
  * @brief  Display the Random number value on LCD or/and USART
  * @param  rnumber: random number to display
  * @param  line: LCD line number
  * @retval None
  */
void Display(uint32_t rnumber, uint8_t line)
{
#ifdef PRINT_ON_LCD
  uint8_t text[50];
#endif

#ifdef PRINT_ON_USART  
  printf("\r [ 0x%08x ]\n", rnumber);
  if (line == 8)
  {  
    printf("\n\r  Press key button to generate 8 x 32bit random number\n");
  }
#endif

#ifdef PRINT_ON_LCD
  sprintf((char*)text,"   [ 0x%08X ]   ", rnumber);
  LCD_DisplayStringLine(LINE(line),text);
#endif
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}

  return ch;
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
