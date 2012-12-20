/**
  ******************************************************************************
  * @file    CRYP/AES_ECBmode_DMA/main.c 
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

/** @addtogroup AES_ECBmode_DMA
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLOCKS_NBR             2  /* User can change this value to enlarge or reduce 
                                     the number of blocks to encrypt  */
#define DATA_SIZE              ((BLOCKS_NBR *128)/32)
#define CRYP_DIN_REG_ADDR      ((uint32_t)0x50060008)  /* Crypto DIN register address */
#define CRYP_DOUT_REG_ADDR     ((uint32_t)0x5006000C)  /* Crypto DOUT register address */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

__IO uint32_t AES128key[4]={0x40414243,0x44454647,0x48494a4b,0x4c4d4e4f};

__IO uint32_t PlainData[DATA_SIZE]={
           0x20212223,0x24252627,0x28292a2b,0x2c2d2e2f,  /* Block 0 */
           0xFFEEDDCC,0xBBAA9988,0x77665544,0x33221100}; /* Block 1 */
__IO uint32_t EncryptedData[DATA_SIZE];
__IO uint32_t DecryptedData[DATA_SIZE];

/* Private function prototypes -----------------------------------------------*/
void AES128_Encrypt_DMA(void);
void AES128_Decrypt_DMA(void);

void Display_PlainData(void);
void Display_EncryptedData(void);
void Display_DecryptedData(void);
void USART_Config(void);

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
  /* USART Configuration */     
  USART_Config();

  /* Display Plain Data*/
  Display_PlainData();

/*=====================================================
    Encryption                                          
======================================================*/
  /* Encryption */
  AES128_Encrypt_DMA();
  
  /* Display encrypted Data */
  Display_EncryptedData();

/*=====================================================
    Decryption                                          
======================================================*/
  /* Decryption */
  AES128_Decrypt_DMA();

  /* Display decrypted data */
  Display_DecryptedData();

  while(1);  
}

/**
  * @brief  Encrypt Data using AES 
  * @note   DATA transfer is done by the DMA
  * @note   DMA2 stream6 channel2 is used to transfer data from memory (the 
  *         PlainData Table) to CRYP Peripheral (the INPUT data register). 
  * @note   DMA2 stream5 channel2 is used to transfer data from CRYP Peripheral
  *        (the OUTPUT data register to memory (the EncryptedData Table). 
  * @param  None
  * @retval None
  */
void AES128_Encrypt_DMA(void)
{
  CRYP_InitTypeDef CRYP_InitStructure;
  CRYP_KeyInitTypeDef CRYP_KeyInitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  /* Enable CRYP clock */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);

  /* Enable DMA2 clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
 
  /* CRYP configuration********************************************************/
  /* Crypto key structure initialisation */ 
  CRYP_KeyStructInit(&CRYP_KeyInitStructure);
  
  /* Crypto Init for Encryption process */      
  CRYP_InitStructure.CRYP_AlgoDir  = CRYP_AlgoDir_Encrypt;
  CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_ECB;
  CRYP_InitStructure.CRYP_DataType = CRYP_DataType_32b;
  CRYP_InitStructure.CRYP_KeySize  = CRYP_KeySize_128b;
  CRYP_Init(&CRYP_InitStructure);

  /* Key Initialisation */
  CRYP_KeyInitStructure.CRYP_Key2Left = AES128key[0];
  CRYP_KeyInitStructure.CRYP_Key2Right= AES128key[1];
  CRYP_KeyInitStructure.CRYP_Key3Left = AES128key[2];
  CRYP_KeyInitStructure.CRYP_Key3Right= AES128key[3];
  CRYP_KeyInit(&CRYP_KeyInitStructure);

  /* Enable DMA Requests ******************************************************/
  CRYP_DMACmd(CRYP_DMAReq_DataIN, ENABLE);
  CRYP_DMACmd(CRYP_DMAReq_DataOUT, ENABLE);
  
  /* DMA Configuration*********************************************************/
  DMA_DeInit(DMA2_Stream5);
  DMA_DeInit(DMA2_Stream6);
  /* set common DMA parameters for Stream 5 and 6*/  
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_BufferSize = DATA_SIZE;

  /* Set the parameters to be configured for stream 6*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = CRYP_DIN_REG_ADDR; 
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)PlainData;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;

  /* Configure the DMA Stream 6 */
  DMA_Init(DMA2_Stream6, &DMA_InitStructure);

  /* Set the parameters to be configured for stream 5*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = CRYP_DOUT_REG_ADDR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)EncryptedData;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;

  /* Configure the DMA Stream 5 */
  DMA_Init(DMA2_Stream5, &DMA_InitStructure);

  /* Enable DMA streams*******************************************************/
  DMA_Cmd(DMA2_Stream6, ENABLE);
  DMA_Cmd(DMA2_Stream5, ENABLE);
  
  /* Enable Crypto processor *************************************************/
  CRYP_Cmd(ENABLE);
  
  /* wait until the last transfer from OUT FIFO :
   all encrypted Data are transferred from crypt processor */
  while (DMA_GetFlagStatus(DMA2_Stream5, DMA_FLAG_TCIF5) == RESET);

  /* Disable Crypto and DMA **************************************************/
  CRYP_Cmd(DISABLE);
  CRYP_DMACmd(CRYP_DMAReq_DataIN, DISABLE);
  CRYP_DMACmd(CRYP_DMAReq_DataOUT, DISABLE);
  DMA_Cmd(DMA2_Stream5, DISABLE);
  DMA_Cmd(DMA2_Stream6, DISABLE); 
}

/**
  * @brief  Decrypt Data using AES 
  * @note   DATA transfer is done by DMA  
  * @note   DMA2 stream6 channel2 is used to transfer data from memory (the 
  *         EncryptedData Table) to CRYP Peripheral (the INPUT data register). 
  * @note   DMA2 stream5 channel2 is used to transfer data from CRYP Peripheral
  *        (the OUTPUT data register to memory (the DecryptedData Table). 
  * @param  None
  * @retval None
  */
void AES128_Decrypt_DMA(void)
{
  CRYP_InitTypeDef CRYP_InitStructure;
  CRYP_KeyInitTypeDef CRYP_KeyInitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  /* Enable CRYP clock */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);

  /* Enable DMA2 clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

  /* CRYP configuration*******************************************************/
  /* Crypto Init for Key preparation for Decryption process */ 
  CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;
  CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_Key;
  CRYP_InitStructure.CRYP_DataType = CRYP_DataType_32b;
  CRYP_InitStructure.CRYP_KeySize  = CRYP_KeySize_128b;
  CRYP_Init(&CRYP_InitStructure);

  /* Key Initialisation */
  CRYP_KeyInitStructure.CRYP_Key2Left = AES128key[0];
  CRYP_KeyInitStructure.CRYP_Key2Right= AES128key[1];
  CRYP_KeyInitStructure.CRYP_Key3Left = AES128key[2];
  CRYP_KeyInitStructure.CRYP_Key3Right= AES128key[3];
  CRYP_KeyInit(&CRYP_KeyInitStructure);

  /* Enable Crypto processor */
  CRYP_Cmd(ENABLE);

  /* wait until the Busy flag is reset */
   while (CRYP_GetFlagStatus(CRYP_FLAG_BUSY) != RESET);
   
  /* Disable Crypto processor */  
  CRYP_Cmd(DISABLE);

  /* Crypto Init for Decryption process */ 
  CRYP_InitStructure.CRYP_AlgoDir = CRYP_AlgoDir_Decrypt;
  CRYP_InitStructure.CRYP_AlgoMode = CRYP_AlgoMode_AES_ECB;
  CRYP_InitStructure.CRYP_DataType = CRYP_DataType_32b;
  CRYP_InitStructure.CRYP_KeySize  = CRYP_KeySize_128b;

  CRYP_Init(&CRYP_InitStructure);

  
  /* Enable DMA Requests ******************************************************/
  CRYP_DMACmd(CRYP_DMAReq_DataIN, ENABLE);
  CRYP_DMACmd(CRYP_DMAReq_DataOUT, ENABLE);  
  
  /* DMA Configuration*********************************************************/
  DMA_DeInit(DMA2_Stream5);
  DMA_DeInit(DMA2_Stream6);
  /* set common DMA parameters for Stream 5 and 6*/  
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;;  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_BufferSize = DATA_SIZE;

  /* Set the parameters to be configured for stream 6*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = CRYP_DIN_REG_ADDR; 
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)EncryptedData;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;

  /* Configure the DMA Stream 6 */
  DMA_Init(DMA2_Stream6, &DMA_InitStructure);

  /* Set the parameters to be configured for stream 5*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = CRYP_DOUT_REG_ADDR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DecryptedData;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;

  /* Configure the DMA Stream 5 */
  DMA_Init(DMA2_Stream5, &DMA_InitStructure);
  
  /* Enable DMA streams*******************************************************/
  DMA_Cmd(DMA2_Stream6, ENABLE);
  DMA_Cmd(DMA2_Stream5, ENABLE);
  
  /* Enable Crypto processor *************************************************/
  CRYP_Cmd(ENABLE);

  /* wait until the last transfer from OUT FIFO :
   all encrypted Data are transferred from crypt processor */
  while (DMA_GetFlagStatus(DMA2_Stream5, DMA_FLAG_TCIF5) == RESET);


  /* Disable Crypto and DMA **************************************************/
  CRYP_Cmd(DISABLE);
  CRYP_DMACmd(CRYP_DMAReq_DataIN, DISABLE);
  CRYP_DMACmd(CRYP_DMAReq_DataOUT, DISABLE);
  DMA_Cmd(DMA2_Stream5, DISABLE);
  DMA_Cmd(DMA2_Stream6, DISABLE);

  DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF5);  
}

/**
  * @brief  Display Plain Data 
  * @param  None
  * @retval None
  */
void Display_PlainData(void)
{
  uint32_t i=0;
  uint8_t count=0;
  
  printf(" \n\r======================================\n\r");
  printf(" === CRYP AES-128 Using DMA Example ===\n\r");
  printf(" ======================================\n\r");
  printf(" ---------------------------------------\n\r");
  printf(" Plain Data :\n\r");
  printf(" ---------------------------------------\n\r");
  
  for(i = 0; i < DATA_SIZE; i++)
  {
    printf("[0x%8X]", PlainData[i]);
    count++;

    if(count == 4)
    { 
      count = 0;
      printf("  Block %d \n\r", i/4);
    }
  }

  printf(" ---------------------------------------\n\r");
  printf(" Key :\n\r");
  printf(" ---------------------------------------\n\r");
  
  for(i = 0; i < 4; i++)
  {
    printf("[0x%8X]", AES128key[i]);
    count++;
  }

}

/**
  * @brief  Display Encrypted Data 
  * @param  None
  * @retval None
  */
void Display_EncryptedData(void)
{
  uint32_t i=0;
  uint8_t count=0;

  printf("\n\r ---------------------------------------\n\r");
  printf(" AES-128 Encrypted Data:\n\r");
  printf(" ---------------------------------------\n\r");
  
  for(i = 0; i < DATA_SIZE; i++)
  {
    printf("[0x%8X]", EncryptedData[i]);
    count++;

    if(count == 4)
    { 
      count = 0;
      printf("  Block %d \n\r", i/4);
    }
  }
}

/**
  * @brief  Display Decrypted Data 
  * @param  None
  * @retval None
  */
void Display_DecryptedData(void)
{
  uint32_t i=0;
  uint8_t count=0;

  printf("\n\r ---------------------------------------\n\r");
  printf(" AES-128 Decrypted Data:\n\r");
  printf(" ---------------------------------------\n\r");
  
  for(i = 0; i < DATA_SIZE; i++)
  {
    printf("[0x%8X]", DecryptedData[i]);
    count++;

    if(count == 4)
    { 
      count = 0;
      printf("  Block %d \n\r", i/4);
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
