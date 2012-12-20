/**
  ******************************************************************************
  * @file    CRYP/AES_modes/main.c 
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

/** @addtogroup AES_modes
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define AES_TEXT_SIZE    64

#define ECB              1
#define CBC              2
#define CTR              3

 uint8_t AES128key[16] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,
                      0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c}; /* key size 128 bytes */

 uint8_t AES192key[24] = {0x8e,0x73,0xb0,0xf7,0xda,0x0e,0x64,0x52,
                      0xc8,0x10,0xf3,0x2b,0x80,0x90,0x79,0xe5,
                      0x62,0xf8,0xea,0xd2,0x52,0x2c,0x6b,0x7b}; /* key size 192 bytes */

 uint8_t AES256key[32] = {0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,
                      0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,
                      0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,
                      0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4}; /* key size 256 bytes */

 uint8_t IV_1[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                     0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f}; /* initialization vector */

 uint8_t Plaintext[AES_TEXT_SIZE] = 
                        {0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,
                         0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a,
                         0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,
                         0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51,
                         0x30,0xc8,0x1c,0x46,0xa3,0x5c,0xe4,0x11,
                         0xe5,0xfb,0xc1,0x19,0x1a,0x0a,0x52,0xef,
                         0xf6,0x9f,0x24,0x45,0xdf,0x4f,0x9b,0x17,
                         0xad,0x2b,0x41,0x7b,0xe6,0x6c,0x37,0x10}; /* plaintext */

uint8_t Ciphertext[AES_TEXT_SIZE] = 
                        {0x76,0x49,0xab,0xac,0x81,0x19,0xb2,0x46,
                         0xce,0xe9,0x8e,0x9b,0x12,0xe9,0x19,0x7d,
                         0x50,0x86,0xcb,0x9b,0x50,0x72,0x19,0xee,
                         0x95,0xdb,0x11,0x3a,0x91,0x76,0x78,0xb2,
                         0x73,0xbe,0xd6,0xb8,0xe3,0xc1,0x74,0x3b,
                         0x71,0x16,0xe6,0x9e,0x22,0x22,0x95,0x16,
                         0x3f,0xf1,0xca,0xa1,0x68,0x1f,0xac,0x09,
                         0x12,0x0e,0xca,0x30,0x75,0x86,0xe1,0xa7}; /* ciphertext */

uint8_t Encryptedtext[AES_TEXT_SIZE]; /* Encrypted text */
uint8_t Decryptedtext[AES_TEXT_SIZE]; /* Decrypted text */

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
void Display_EncryptedData(uint8_t mode,uint16_t keysize,uint32_t datalength);
void Display_DecryptedData(uint8_t mode,uint16_t keysize,uint32_t datalength);
char PressToContinue(void);
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

   /* Enable CRYP clock */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);

  while(1)
  { 

    /* Display Plain Data*/
    Display_PlainData(AES_TEXT_SIZE);

/******************************************************************************/
/*                             AES mode ECB                                   */
/******************************************************************************/

/*=====================================================
  Encryption ECB mode                                        
======================================================*/
    PressToContinue();
/****************************************/
/*                           AES 128   **/
/****************************************/
    /* Encrypt the plaintext message*/
    CRYP_AES_ECB(MODE_ENCRYPT,AES128key,128,Plaintext,AES_TEXT_SIZE,Encryptedtext);
  
    /* Display encrypted Data*/
    Display_EncryptedData(ECB,128,AES_TEXT_SIZE);
/****************************************/
/*                           AES 192   **/
/****************************************/
    /* Encrypt the plaintext message*/
    CRYP_AES_ECB(MODE_ENCRYPT,AES192key,192,Plaintext,AES_TEXT_SIZE,Encryptedtext);

    /* Display encrypted Data*/
    Display_EncryptedData(ECB,192,AES_TEXT_SIZE);
/****************************************/
/*                           AES 256   **/
/****************************************/
    /* Encrypt the plaintext message*/
    CRYP_AES_ECB(MODE_ENCRYPT,AES256key,256,Plaintext,AES_TEXT_SIZE,Encryptedtext);
  
    /* Display encrypted Data*/
    Display_EncryptedData(ECB, 256,AES_TEXT_SIZE);

/*=====================================================
    Decryption in ECB mode                                       
======================================================*/
   PressToContinue();
/****************************************/
/*                           AES 128   **/
/****************************************/
    /* Decrypt the plaintext message  */
    CRYP_AES_ECB(MODE_DECRYPT,AES128key,128,Ciphertext,AES_TEXT_SIZE,Decryptedtext); 
  
    /* Display decrypted data*/
    Display_DecryptedData(ECB,128,AES_TEXT_SIZE);
/****************************************/
/*                           AES 192   **/
/****************************************/
    /* Decrypt the plaintext message  */
    CRYP_AES_ECB(MODE_DECRYPT,AES192key, 192,Ciphertext, AES_TEXT_SIZE,Decryptedtext); 
  
    /* Display decrypted data*/
    Display_DecryptedData(ECB, 192,AES_TEXT_SIZE);
/****************************************/
/*                           AES 256   **/
/****************************************/
    /* Decrypt the plaintext message  */
    CRYP_AES_ECB(MODE_DECRYPT,AES256key, 256,Ciphertext, AES_TEXT_SIZE,Decryptedtext); 
  
    /* Display decrypted data*/
    Display_DecryptedData(ECB,256,AES_TEXT_SIZE);


/******************************************************************************/
/*                             AES mode CBC                                   */
/******************************************************************************/
    PressToContinue();
/*=====================================================
  Encryption CBC mode                                        
======================================================*/

/****************************************/
/*                           AES 128   **/
/****************************************/
    /* Encrypt the plaintext message*/
    CRYP_AES_CBC(MODE_ENCRYPT,IV_1,AES128key,128,Plaintext,AES_TEXT_SIZE,Encryptedtext);
  
    /* Display encrypted Data*/
    Display_EncryptedData(CBC,128,AES_TEXT_SIZE);
/****************************************/
/*                           AES 192   **/
/****************************************/
    /* Encrypt the plaintext message*/
    CRYP_AES_CBC(MODE_ENCRYPT,IV_1,AES192key,192,Plaintext,AES_TEXT_SIZE,Encryptedtext);

    /* Display encrypted Data*/
    Display_EncryptedData(CBC,192,AES_TEXT_SIZE);
/****************************************/
/*                           AES 256   **/
/****************************************/
    /* Encrypt the plaintext message*/
    CRYP_AES_CBC(MODE_ENCRYPT,IV_1,AES256key,256,Plaintext,AES_TEXT_SIZE,Encryptedtext);
  
    /* Display encrypted Data*/
    Display_EncryptedData(CBC, 256,AES_TEXT_SIZE);

/*=====================================================
    Decryption in CBC mode                                       
======================================================*/
    PressToContinue();
    /* Deinitializes the CRYP peripheral */
    CRYP_DeInit();
/****************************************/
/*                           AES 128   **/
/****************************************/
    /* Decrypt the plaintext message  */
    CRYP_AES_CBC(MODE_DECRYPT,IV_1,AES128key,128,Ciphertext,AES_TEXT_SIZE,Decryptedtext); 
  
    /* Display decrypted data*/
    Display_DecryptedData(CBC,128,AES_TEXT_SIZE);
/****************************************/
/*                           AES 192   **/
/****************************************/
    /* Deinitializes the CRYP peripheral */
    CRYP_DeInit();

    /* Decrypt the plaintext message  */
    CRYP_AES_CBC(MODE_DECRYPT,IV_1,AES192key, 192,Ciphertext, AES_TEXT_SIZE,Decryptedtext); 
  
    /* Display decrypted data*/
    Display_DecryptedData(CBC, 192,AES_TEXT_SIZE);
/****************************************/
/*                           AES 256   **/
/****************************************/
    /* Deinitializes the CRYP peripheral */
    CRYP_DeInit();

    /* Decrypt the plaintext message  */
    CRYP_AES_CBC(MODE_DECRYPT,IV_1,AES256key, 256,Ciphertext, AES_TEXT_SIZE,Decryptedtext); 
  
    /* Display decrypted data*/
    Display_DecryptedData(CBC,256,AES_TEXT_SIZE);


/******************************************************************************/
/*                             AES mode CTR                                   */
/******************************************************************************/

/*=====================================================
  Encryption CTR mode                                        
======================================================*/
   PressToContinue();
/****************************************/
/*                           AES 128   **/
/****************************************/
    /* Encrypt the plaintext message*/
    CRYP_AES_CTR(MODE_ENCRYPT,IV_1,AES128key,128,Plaintext,AES_TEXT_SIZE,Encryptedtext);
  
    /* Display encrypted Data*/
    Display_EncryptedData(CTR,128, AES_TEXT_SIZE);
/****************************************/
/*                           AES 192   **/
/****************************************/
    /* Encrypt the plaintext message*/
    CRYP_AES_CTR(MODE_ENCRYPT,IV_1,AES192key,192,Plaintext,AES_TEXT_SIZE,Encryptedtext);

    /* Display encrypted Data*/
    Display_EncryptedData(CTR,192, AES_TEXT_SIZE);
/****************************************/
/*                           AES 256   **/
/****************************************/
    /* Encrypt the plaintext message*/
    CRYP_AES_CTR(MODE_ENCRYPT,IV_1,AES256key,256,Plaintext,AES_TEXT_SIZE,Encryptedtext);
  
    /* Display encrypted Data*/
    Display_EncryptedData(CTR, 256, AES_TEXT_SIZE);

/*=====================================================
    Decryption in CTR mode                                       
======================================================*/
   PressToContinue();
/****************************************/
/*                           AES 128   **/
/****************************************/
    /* Decrypt the plaintext message  */
    CRYP_AES_CTR(MODE_DECRYPT,IV_1,AES128key,128,Ciphertext,AES_TEXT_SIZE,Decryptedtext); 
  
    /* Display decrypted data*/
    Display_DecryptedData(CTR, 128, AES_TEXT_SIZE);
/****************************************/
/*                           AES 192   **/
/****************************************/
    /* Decrypt the plaintext message  */
    CRYP_AES_CTR(MODE_DECRYPT,IV_1,AES192key,192,Ciphertext,AES_TEXT_SIZE,Decryptedtext); 
  
    /* Display decrypted data*/
    Display_DecryptedData(CTR, 192, AES_TEXT_SIZE);
/****************************************/
/*                           AES 256   **/
/****************************************/
    /* Decrypt the plaintext message  */
    CRYP_AES_CTR(MODE_DECRYPT,IV_1,AES256key, 256,Ciphertext, AES_TEXT_SIZE,Decryptedtext); 
  
    /* Display decrypted data*/
    Display_DecryptedData(CTR, 256, AES_TEXT_SIZE);

/******************************************************************************/
    PressToContinue();
    printf("\n\r Example restarted...\n ");
  }  
}

/**
  * @brief  Display Plain Data 
  * @param  datalength: length of the data to display
  * @retval None
  */
void Display_PlainData(uint32_t datalength)
{
  uint32_t BufferCounter =0;
  uint32_t count = 0;
  
  printf("\n\r =============================================================\n\r");
  printf(" ================= Crypt Using HW Crypto  ====================\n\r");
  printf(" ============================================================\n\r");
  printf(" ---------------------------------------\n\r");
  printf(" Plain Data :\n\r");
  printf(" ---------------------------------------\n\r");
  
  for(BufferCounter = 0; BufferCounter < datalength; BufferCounter++)
  {
    printf("[0x%02X]", Plaintext[BufferCounter]);
    count++;

    if(count == 16)
    { 
      count = 0;
      printf("  Block %d \n\r", BufferCounter/16);
    }
  }
}

/**
  * @brief  Display Encrypted Data 
  * @param  mode: chaining mode
  * @param  keysize: AES key size used
  * @param  datalength: length of the data to display
  * @retval None
  */
void Display_EncryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;

  printf("\n\r =======================================\n\r");
  printf(" Encrypted Data with AES %d  Mode  ",keysize );

  if(mode == ECB)
  {
    printf("ECB\n\r");
  }    
  else if(mode == CBC)     
  {
    printf("CBC\n\r");
  }      
  else /* if(mode == CTR)*/ 
  {
    printf("CTR\n\r");
  }   

  printf(" ---------------------------------------\n\r");
  
  for(BufferCounter = 0; BufferCounter < datalength; BufferCounter++)
  {
    printf("[0x%02X]", Encryptedtext[BufferCounter]);

    count++;
    if(count == 16)
    { 
      count = 0;
      printf(" Block %d \n\r", BufferCounter/16);
    }
  }
}

/**
  * @brief  Display Decrypted Data 
  * @param  mode: chaining mode
  * @param  keysize: AES key size used
  * @param  datalength: length of the data to display
  * @retval None
  */
void Display_DecryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;

  printf("\n\r =======================================\n\r");
  printf(" Decrypted Data with AES %d  Mode  ",keysize );

  if(mode == ECB)
  {
    printf("ECB\n\r");
  }    
  else if(mode == CBC)     
  {
    printf("CBC\n\r");
  }      
  else /* if(mode == CTR)*/ 
  {
    printf("CTR\n\r");
  }   
 
  printf(" ---------------------------------------\n\r");
  
  for(BufferCounter = 0; BufferCounter < datalength; BufferCounter++)
  {
    printf("[0x%02X]", Decryptedtext[BufferCounter]);
    count++;

    if(count == 16)
    { 
      count = 0;
      printf(" Block %d \n\r", BufferCounter/16);
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
  * @brief  Wait till a character is received by the USART
  * @param  None
  * @retval Keyboard pressed character
  */
char PressToContinue(void)
{
  char c; 
  printf("\n\r Press any key to continue...\n\r ");

  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) == RESET)
  {
  }

  c = USART_ReceiveData(EVAL_COM1);

  return(c);
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
