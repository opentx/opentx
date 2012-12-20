/**
  ******************************************************************************
  * @file    CRYP/DES_TDES_modes/main.c 
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
#include "stdio.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup DES_TDES_modes
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PLAIN_TEXT_SIZE              8
#define ECB                          1
#define CBC                          2

#define DES                         ' '
#define TDES                        'T'

/* key size 64 bytes */
 uint8_t DESkey[8] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6}; 

/* key : 192 bits  */
uint8_t TDESkey[24] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,  /* key 1 */
                       0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10,  /* key 2 */
                       0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67}; /* key 3 */

uint8_t Plaintext[PLAIN_TEXT_SIZE] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38}; /* Plaintext */	  

/* initialization vector */
uint8_t IV_1[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};

uint8_t Encryptedtext[PLAIN_TEXT_SIZE]; /* Encrypted text */
uint8_t Decryptedtext[PLAIN_TEXT_SIZE]; /* Decrypted text */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void USART_Config(void);

void Display_PlainData(uint32_t datalength);
void Display_EncryptedData(uint8_t Algo, uint8_t mode,uint32_t datalength);
void Display_DecryptedData(uint8_t Algo, uint8_t mode,uint32_t datalength);

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
       
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_Config();

  /* Display Plain Data*/
  Display_PlainData(PLAIN_TEXT_SIZE);

   /* Enable CRYP clock */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);

/******************************************************************************/
/*                             DES mode ECB                                   */
/******************************************************************************/

/*=====================================================
  Encryption ECB mode                                        
======================================================*/

  /* Encrypt the plaintext message */
  CRYP_DES_ECB(MODE_ENCRYPT,DESkey,Plaintext,PLAIN_TEXT_SIZE,Encryptedtext);
  
 /* Display encrypted Data*/
  Display_EncryptedData(DES,ECB,PLAIN_TEXT_SIZE);

/*=====================================================
    Decryption in ECB mode                                       
======================================================*/

  /* Decrypt the plaintext message  */
  CRYP_DES_ECB(MODE_DECRYPT,DESkey,Encryptedtext,PLAIN_TEXT_SIZE,Decryptedtext); 
  
  /* Display decrypted data*/
  Display_DecryptedData(DES,ECB,PLAIN_TEXT_SIZE);


/******************************************************************************/
/*                             DES mode CBC                                   */
/******************************************************************************/

/*=====================================================
  Encryption CBC mode                                        
======================================================*/

  /* Encrypt the plaintext message*/
  CRYP_DES_CBC(MODE_ENCRYPT,DESkey,IV_1,Plaintext,PLAIN_TEXT_SIZE,Encryptedtext);
  
 /* Display encrypted Data*/
  Display_EncryptedData(DES,CBC,PLAIN_TEXT_SIZE);

/*=====================================================
    Decryption in CBC mode                                       
======================================================*/

  /* Decrypt the plaintext message  */
  CRYP_DES_CBC(MODE_DECRYPT,DESkey,IV_1,Encryptedtext,PLAIN_TEXT_SIZE,Decryptedtext); 
  
  /* Display decrypted data*/
  Display_DecryptedData(DES,CBC,PLAIN_TEXT_SIZE);


/******************************************************************************/
/*                             TDES mode ECB                                   */
/******************************************************************************/

/*=====================================================
  Encryption ECB mode                                        
======================================================*/

  /* Encrypt the plaintext message*/
  CRYP_TDES_ECB(MODE_ENCRYPT,TDESkey,Plaintext,PLAIN_TEXT_SIZE,Encryptedtext);
  
 /* Display encrypted Data*/
  Display_EncryptedData(TDES,ECB,PLAIN_TEXT_SIZE);

/*=====================================================
    Decryption in ECB mode                                       
======================================================*/

  /* Decrypt the plaintext message  */
  CRYP_TDES_ECB(MODE_DECRYPT,TDESkey,Encryptedtext,PLAIN_TEXT_SIZE,Decryptedtext); 
  
  /* Display decrypted data*/
  Display_DecryptedData(TDES,ECB,PLAIN_TEXT_SIZE);


/******************************************************************************/
/*                             TDES mode CBC                                   */
/******************************************************************************/

/*=====================================================
  Encryption CBC mode                                        
======================================================*/

  /* Encrypt the plaintext message*/
  CRYP_TDES_CBC(MODE_ENCRYPT,TDESkey,IV_1,Plaintext,PLAIN_TEXT_SIZE,Encryptedtext);
  
 /* Display encrypted Data*/
  Display_EncryptedData(TDES,CBC,PLAIN_TEXT_SIZE);

/*=====================================================
    Decryption in CBC mode                                       
======================================================*/

  /* Decrypt the plaintext message  */
  CRYP_TDES_CBC(MODE_DECRYPT,TDESkey,IV_1,Encryptedtext,PLAIN_TEXT_SIZE,Decryptedtext); 
  
  /* Display decrypted data*/
  Display_DecryptedData(TDES,CBC,PLAIN_TEXT_SIZE);  
/******************************************************************************/

  while(1);  
}

/**
  * @brief  Display Plain Data 
  * @param  datalength: length of the data to display
  * @retval None
  */
void Display_PlainData(uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;
  
  printf("\n\r =============================================================\n\r");
  printf(" ================= Crypt Using HW Crypto  ====================\n\r");
  printf(" =============================================================\n\r");
  printf(" ---------------------------------------\n\r");
  printf(" Plain Data :\n\r");
  printf(" ---------------------------------------\n\r");
  
  for(BufferCounter = 0; BufferCounter < datalength; BufferCounter++)
  {
    printf("[0x%02X]", Plaintext[BufferCounter]);

    count++;
    if(count == 8)
    { 
      count = 0;
      printf("  Block %d \n\r", BufferCounter/8);
    }
  }
}
/**
  * @brief  Display Encrypted Data 
  * @param  Algo: Algorithm used (DES or TDES)
  * @param  mode: chaining mode
  * @param  datalength: length of the data to display
  * @retval None
  */
void Display_EncryptedData(uint8_t Algo, uint8_t mode, uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;

  printf("\n\r =======================================\n\r");
  printf("  Encrypted Data %cDES Mode ",Algo);

  if(mode == ECB)
  {
    printf("ECB\n\r");
  } 
  else/* if(mode == CBC)*/
  {
    printf("CBC\n\r");
  }
     
  printf(" ---------------------------------------\n\r");
  
  for(BufferCounter = 0; BufferCounter < datalength; BufferCounter++)
  {
    printf("[0x%02X]", Encryptedtext[BufferCounter]);
    count++;

    if(count == 8)
    { 
      count = 0;
      printf(" Block %d \n\r", BufferCounter/8);
    }
  }
}

/**
  * @brief  Display Decrypted Data 
  * @param  Algo: Algorithm used (DES or TDES)
  * @param  mode: chaining mode
  * @param  datalength: length of the data to display
  * @retval None
  */
void Display_DecryptedData(uint8_t Algo, uint8_t mode, uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;

  printf("\n\r =======================================\n\r");
  printf(" Decrypted Data %cDES Mode ",Algo);

  if(mode == ECB)
  {
    printf("ECB\n\r");
  } 
  else/* if(mode == CBC)*/
  {
    printf("CBC\n\r");
  }
   
  printf(" ---------------------------------------\n\r");
  
  for(BufferCounter = 0; BufferCounter < datalength; BufferCounter++)
  {
    printf("[0x%02X]", Decryptedtext[BufferCounter]);
    count++;

    if(count == 8)
    { 
      count = 0;
      printf(" Block %d \n\r", BufferCounter/8);
    }
  }
}

/**
  * @brief  USART configuration 
  * @param  None
  * @retval None
  */
void USART_Config(void)
{
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
