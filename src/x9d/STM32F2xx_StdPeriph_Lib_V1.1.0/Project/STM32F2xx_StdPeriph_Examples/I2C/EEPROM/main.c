/**
  ******************************************************************************
  * @file    I2C/EEPROM/main.c 
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
#include "stm322xg_eval_i2c_ee.h"
#include "stm322xg_eval_lcd.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2C_EEPROM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
/* Uncomment the following line to enable using LCD screen for messages display */
#define ENABLE_LCD_MSG_DISPLAY

#define sEE_WRITE_ADDRESS1        0x50
#define sEE_READ_ADDRESS1         0x50
#define BUFFER_SIZE1             (countof(Tx1_Buffer)-1)
#define BUFFER_SIZE2             (countof(Tx2_Buffer)-1)
#define sEE_WRITE_ADDRESS2       (sEE_WRITE_ADDRESS1 + BUFFER_SIZE1)
#define sEE_READ_ADDRESS2        (sEE_READ_ADDRESS1 + BUFFER_SIZE1)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t Tx1_Buffer[] = "/* STM32F20xx I2C Firmware Library EEPROM driver example: \
                        buffer 1 transfer into address sEE_WRITE_ADDRESS1 */ \
                        Example Description \
                        This firmware provides a basic example of how to use the I2C firmware library and\
                        an associate I2C EEPROM driver to communicate with an I2C EEPROM device (here the\
                        example is interfacing with M24C64 EEPROM)\
                          \
                        I2C peripheral is configured in Master transmitter during write operation and in\
                        Master receiver during read operation from I2C EEPROM. \
                          \
                        The peripheral used is I2C1 but can be configured by modifying the defines values\
                        in stm322xg_eval.h file. The speed is set to 100kHz and can be configured by \
                        modifying the relative define in stm32_eval_i2c_ee.h file.\
                         \
                        For M24C64 devices all the memory is accessible through the two-bytes \
                        addressing mode and need to define block addresses. In this case, only the physical \
                        address has to be defined (according to the address pins (E0,E1 and E2) connection).\
                        This address is defined in i2c_ee.h (default is 0xA0: E0, E1 and E2 tied to ground).\
                        The EEPROM addresses where the program start the write and the read operations \
                        is defined in the main.c file. \
                         \
                        First, the content of Tx1_Buffer is written to the EEPROM_WriteAddress1 and the\
                        written data are read. The written and the read buffers data are then compared.\
                        Following the read operation, the program waits that the EEPROM reverts to its \
                        Standby state. A second write operation is, then, performed and this time, Tx2_Buffer\
                        is written to EEPROM_WriteAddress2, which represents the address just after the last \
                        written one in the first write. After completion of the second write operation, the \
                        written data are read. The contents of the written and the read buffers are compared.\
                         \
                        All transfers are managed in DMA mode (except when 1-byte read/write operation is\
                        required). Once sEE_ReadBuffer() or sEE_WriteBuffer() function is called, the \
                        use application may perform other tasks in parallel while Read/Write operation is\
                        managed by DMA.\
                          \
                        This example provides the possibility to use the STM322xG-EVAL LCD screen for\
                        messages display (transfer status: Ongoing, PASSED, FAILED).\
                        To enable this option uncomment the define ENABLE_LCD_MSG_DISPLAY in the main.c\
                        file.                                                                              ";
uint8_t Tx2_Buffer[] = "/* STM32F20xx I2C Firmware Library EEPROM driver example: \
                        buffer 2 transfer into address sEE_WRITE_ADDRESS2 */";
uint8_t Rx1_Buffer[BUFFER_SIZE1], Rx2_Buffer[BUFFER_SIZE2];
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
volatile uint16_t NumDataRead = 0;

/* Private functions ---------------------------------------------------------*/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

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

#ifdef ENABLE_LCD_MSG_DISPLAY
  /* Initialize the LCD screen for information display */
  STM322xG_LCD_Init();
  LCD_Clear(LCD_COLOR_BLUE);  
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_0, (uint8_t *)"SMT32F2xx FW Library");
  LCD_DisplayStringLine(LCD_LINE_1, (uint8_t *)"   EEPROM Example   ");
#endif /* ENABLE_LCD_MSG_DISPLAY */  
  
  /* Initialize the I2C EEPROM driver ----------------------------------------*/
  sEE_Init();  

  /* First write in the memory followed by a read of the written data --------*/
  /* Write on I2C EEPROM from sEE_WRITE_ADDRESS1 */
  sEE_WriteBuffer(Tx1_Buffer, sEE_WRITE_ADDRESS1, BUFFER_SIZE1); 

  /* Wait for EEPROM standby state */
  sEE_WaitEepromStandbyState();  
  
  /* Set the Number of data to be read */
  NumDataRead = BUFFER_SIZE1;
  
  /* Read from I2C EEPROM from sEE_READ_ADDRESS1 */
  sEE_ReadBuffer(Rx1_Buffer, sEE_READ_ADDRESS1, (uint16_t *)(&NumDataRead)); 


  /* Starting from this point, if the requested number of data is higher than 1, 
     then only the DMA is managing the data transfer. Meanwhile, CPU is free to 
     perform other tasks:
  
    // Add your code here: 
    //...
    //...

     For simplicity reasons, this example is just waiting till the end of the 
     transfer. */
 
#ifdef ENABLE_LCD_MSG_DISPLAY  
  LCD_DisplayStringLine(LCD_LINE_3, (uint8_t *)" Transfer 1 Ongoing ");
#endif /* ENABLE_LCD_MSG_DISPLAY */ 
  
  /* Wait till DMA transfer is complete (Transfer complete interrupt handler 
    resets the variable holding the number of data to be read) */
  while (NumDataRead > 0)
  {}  
  
  /* Check if the data written to the memory is read correctly */
  TransferStatus1 = Buffercmp(Tx1_Buffer, Rx1_Buffer, BUFFER_SIZE1);
  /* TransferStatus1 = PASSED, if the transmitted and received data 
     to/from the EEPROM are the same */
  /* TransferStatus1 = FAILED, if the transmitted and received data 
     to/from the EEPROM are different */
#ifdef ENABLE_LCD_MSG_DISPLAY  
  if (TransferStatus1 == PASSED)
  {
    LCD_DisplayStringLine(LCD_LINE_3, (uint8_t *)" Transfer 1 PASSED  ");
  }
  else
  {
    LCD_DisplayStringLine(LCD_LINE_3, (uint8_t *)" Transfer 1 FAILED  ");
  }  
#endif /* ENABLE_LCD_MSG_DISPLAY */  

  /* Second write in the memory followed by a read of the written data -------*/
  /* Write on I2C EEPROM from sEE_WRITE_ADDRESS2 */
  sEE_WriteBuffer(Tx2_Buffer, sEE_WRITE_ADDRESS2, BUFFER_SIZE2); 

  /* Wait for EEPROM standby state */
  sEE_WaitEepromStandbyState();  
  
  /* Set the Number of data to be read */
  NumDataRead = BUFFER_SIZE2;  
  
  /* Read from I2C EEPROM from sEE_READ_ADDRESS2 */
  sEE_ReadBuffer(Rx2_Buffer, sEE_READ_ADDRESS2, (uint16_t *)(&NumDataRead));


  /* Starting from this point, if the requested number of data is higher than 1, 
     then only the DMA is managing the data transfer. Meanwhile, CPU is free to 
     perform other tasks:
     
    // Add your code here: 
    //...
    //...

     For simplicity reasons, this example is just waiting till the end of the 
     transfer. */

#ifdef ENABLE_LCD_MSG_DISPLAY   
  LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)" Transfer 2 Ongoing ");
#endif /* ENABLE_LCD_MSG_DISPLAY */  
  
  /* Wait till DMA transfer is complete (Transfer complete interrupt handler 
    resets the variable holding the number of data to be read) */
  while (NumDataRead > 0)
  {}
  
  /* Check if the data written to the memory is read correctly */
  TransferStatus2 = Buffercmp(Tx2_Buffer, Rx2_Buffer, BUFFER_SIZE2);
  /* TransferStatus2 = PASSED, if the transmitted and received data 
     to/from the EEPROM are the same */
  /* TransferStatus2 = FAILED, if the transmitted and received data 
     to/from the EEPROM are different */
#ifdef ENABLE_LCD_MSG_DISPLAY   
  if (TransferStatus1 == PASSED)
  {
    LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)" Transfer 2 PASSED  ");
  }
  else
  {
    LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)" Transfer 2 FAILED  ");
  }  
#endif /* ENABLE_LCD_MSG_DISPLAY */
  
  /* Free all used resources */
  sEE_DeInit();

  while (1)
  {
  }
}

#ifndef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Example of timeout situation management.
  * @param  None.
  * @retval None.
  */
uint32_t sEE_TIMEOUT_UserCallback(void)
{
  /* Use application may try to recover the communication by resetting I2C
    peripheral (calling the function I2C_SoftwareResetCmd()) then restart
    the transmission/reception from a previously stored recover point.
    For simplicity reasons, this example only shows a basic way for errors 
    managements which consists of stopping all the process and requiring system
    reset. */
  
#ifdef ENABLE_LCD_MSG_DISPLAY   
  /* Display error message on screen */
  LCD_Clear(LCD_COLOR_RED);  
  LCD_DisplayStringLine(LCD_LINE_4, (uint8_t *)"Communication ERROR!");
  LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)"Try again after res-");
  LCD_DisplayStringLine(LCD_LINE_6, (uint8_t *)"  etting the Board  ");
#endif /* ENABLE_LCD_MSG_DISPLAY */
  
  /* Block communication and all processes */
  while (1)
  {   
  }  
}
#endif /* USE_DEFAULT_TIMEOUT_CALLBACK */

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;  
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
