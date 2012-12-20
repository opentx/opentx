/**
  ******************************************************************************
  * @file    HASH/Context_Swap/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main program body.
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

/** @addtogroup HASH_Context_Swap
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* "testing123" */
const uint8_t Sha1Input[SHA1_INPUT_TAB_SIZE] = {0x74,0x65,0x73,0x74,0x69,
                                                0x6e,0x67,0x31,0x32,0x33};


/* 
3 x 
   "The hash processor is a fully compliant implementation of the secure
    hash algorithm (SHA-1), the MD5 (message-digest algorithm 5) hash 
    algorithm and the HMAC (keyed-hash message authentication code)
    algorithm suitable for a variety of applications.*** STM32 ***"
*/
const uint8_t Md5Input[MD5_INPUT_TAB_SIZE]=
                          {0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,
                           0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                           0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                           0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                           0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                           0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                           0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                           0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                           0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                           0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                           0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                           0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                           0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                           0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                           0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                           0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                           0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                           0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                           0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                           0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                           0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                           0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                           0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                           0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                           0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                           0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                           0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                           0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                           0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                           0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                           0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                           0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                           0x32,0x20,0x2a,0x2a,0x2a,
                           0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,
                           0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                           0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                           0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                           0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                           0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                           0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                           0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                           0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                           0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                           0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                           0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                           0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                           0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                           0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                           0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                           0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                           0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                           0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                           0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                           0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                           0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                           0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                           0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                           0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                           0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                           0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                           0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                           0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                           0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                           0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                           0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                           0x32,0x20,0x2a,0x2a,0x2a,
                           0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,
                           0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                           0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                           0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                           0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                           0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                           0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                           0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                           0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                           0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                           0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                           0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                           0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                           0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                           0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                           0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                           0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                           0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                           0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                           0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                           0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                           0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                           0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                           0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                           0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                           0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                           0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                           0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                           0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                           0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                           0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                           0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                           0x32,0x20,0x2a,0x2a,0x2a};

uint8_t  Md5output[16];
uint8_t  Sha1output[20];
uint32_t ContextSwapCounter=0;

/* Private function prototypes -----------------------------------------------*/
void USART_Config(void);
void TIM6_Config(void);
void Display_MainMessage(void);
void Display_MD5Digest(void);

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
/*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f2xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f2xx.c file
     */


  /* TIM6 configuration to get a regular interrupt */
  TIM6_Config();

  /* USART configuration */
  USART_Config();

  /* Display the original message */
  Display_MainMessage();

  /* Enable HASH clock */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_HASH, ENABLE);

/*=============================================================================
  SHA1/ MD5 Digest Computation without context swap
==============================================================================*/
  printf("\n\r>> MD5 Digest Computation without context swap \n\r");
  printf(">>> Start \n\r");

  /* MD5 Digest Computation *******************************/
  HASH_MD5((uint8_t*)Md5Input, MD5_INPUT_TAB_SIZE,Md5output); 
  printf(">>> Done \n\r");

  /* Display the MD5 digest */
  Display_MD5Digest();

  printf("\n\r>> SHA1 Digest Computation without context swap \n\r");
  printf(">>> Start \n\r");

  /* SHA1 Digest Computation */
  HASH_SHA1((uint8_t*)Sha1Input, SHA1_INPUT_TAB_SIZE, Sha1output); 
  printf(">>> Done \n\r");

  /* Display the SHA1 digest */
  Display_SHA1Digest();

/*=============================================================================
   SHA1 / MD5 Digest Computation with context swap
==============================================================================*/
  printf("\n\r>> MD5 Digest Computation with context swap \n\r");
  printf(">>> Start \n\r");

  /* Enable TIM6 */
  TIM_Cmd(TIM6, ENABLE);

  /* MD5 Digest Computation */
  HASH_MD5((uint8_t*)Md5Input, MD5_INPUT_TAB_SIZE, Md5output); 

  /* Disable TIM2 : no more interrupts */
  TIM_Cmd(TIM6, DISABLE);

  printf(" ====> During MD5 digest calculation, the context is saved and restored (%d) times to calculate the SHA1 digest \n\r", ContextSwapCounter);
  printf(">>> Done \n\r");

  /* Display the MD5 digest */
  Display_MD5Digest();

  printf(">> SHA1 Digest computed during MD5 context swap \n\r");

  /* Display the SHA1 digest */
  Display_SHA1Digest();

  while(1);  
}

/**
  * @brief  Display the original messages to be hashed.
  * @param  None
  * @retval None
  */
void Display_MainMessage(void)
{
  uint32_t BufferCounter = 0;
  printf("\n\r ====================================== \n\r");
  printf(" ====    Context Swap Example      ==== \n\r");
  printf(" ====================================== \n\r");
  printf("\r\n --------------------------------------- \n\r");
  printf(" MD5 Text to be Hashed : \n\r");
  printf(" ---------------------------------------\n\r ");
  for(BufferCounter = 0; BufferCounter<MD5_INPUT_TAB_SIZE; BufferCounter++)
  {
    printf("%c", Md5Input[BufferCounter]);
  }
  printf("\n\r --------------------------------------- \n\r");
  printf(" SHA1 Text to be Hashed : \n\r");
  printf(" ---------------------------------------\n\r");
  for(BufferCounter=0; BufferCounter<SHA1_INPUT_TAB_SIZE; BufferCounter++)
  {
    printf("%c", Sha1Input[BufferCounter]);    
  }
}

/**
  * @brief  Display the SHA-1 digest.
  * @param  None
  * @retval None
  */
void Display_SHA1Digest(void)
{
  printf(" --------------------------------------- \n\r");
  printf("   SHA1 Message Digest (160 bits): \n\r");
  printf(" ---------------------------------------\n\r");

  printf(" H0 = [0x%02x%02x%02x%02x]\n\r",Sha1output[0],
                                          Sha1output[1],
                                          Sha1output[2],
                                          Sha1output[3]);
  printf(" H1 = [0x%02x%02x%02x%02x]\n\r",Sha1output[4],
                                          Sha1output[5],
                                          Sha1output[6],
                                          Sha1output[7]);
  printf(" H2 = [0x%02x%02x%02x%02x]\n\r",Sha1output[8],
                                          Sha1output[9],
                                          Sha1output[10],
                                          Sha1output[11]);
  printf(" H3 = [0x%02x%02x%02x%02x]\n\r",Sha1output[12],
                                          Sha1output[13],
                                          Sha1output[14],
                                          Sha1output[15]);
  printf(" H4 = [0x%02x%02x%02x%02x]\n\r",Sha1output[16],
                                          Sha1output[17],
                                          Sha1output[18],
                                          Sha1output[19]);
}

/**
  * @brief  Display the MD5 digest.
  * @param  None
  * @retval None
  */
void Display_MD5Digest(void)
{
  printf(" --------------------------------------- \n\r");
  printf("     MD5 Message Digest (128 bits): \n\r");
  printf(" ---------------------------------------\n\r");
  printf(" A = [0x%02x%02x%02x%02x]\n\r", Md5output[0],
                                          Md5output[1],
                                          Md5output[2],
                                          Md5output[3]);
  printf(" B = [0x%02x%02x%02x%02x]\n\r", Md5output[4],
                                          Md5output[5],
                                          Md5output[6],
                                          Md5output[7]);
  printf(" C = [0x%02x%02x%02x%02x]\n\r", Md5output[8],
                                          Md5output[9],
                                          Md5output[10],
                                          Md5output[11]);
  printf(" D = [0x%02x%02x%02x%02x]\n\r", Md5output[12],
                                          Md5output[13],
                                          Md5output[14],
                                          Md5output[15]);
}

/**
  * @brief  USART configuration 
  * @param  None
  * @retval None
  */
void USART_Config(void)
{
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);
}

/**
  * @brief  TIM6 configuration to get a regular interrupt rate (200KHz)
  *  
  * @note   TIM6CLK = (HCLK/4)x2 = (120 MHz/4)x2 = 60 MHz 
  *         so TIM6 update interrupt = TIM6CLK/((Precaler+1) x Period) 
  *                                  = 60 MHz / (2x150) = 200 KHz
  *
  * @param  None
  * @retval None
  */
void TIM6_Config(void)
{
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* PCLK1 = HCLK/4 */
  RCC_PCLK1Config(RCC_HCLK_Div4);


  /* TIM6 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

  /* Disable TIM6*/
  TIM_Cmd(TIM6, DISABLE);

  /* NVIC Configuration */
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 150;
  TIM_TimeBaseStructure.TIM_Prescaler = 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  /* TIM IT enable */
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

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
