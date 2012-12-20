/**
  ******************************************************************************
  * @file    SPI/SPI_TwoBoards/DataExchangeInterrupt/main.c
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

/** @addtogroup SPI_DataExchangeInterrupt
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitTypeDef  SPI_InitStructure;
uint8_t TxBuffer[] = "SPI Interrupt Example: Communication between two SPI using Interrupts";
__IO uint32_t TimeOut = 0;

#ifdef SPI_SLAVE
 __IO uint8_t RxBuffer [RXBUFFERSIZE];
 __IO uint8_t Rx_Idx = 0;
#endif
#ifdef SPI_MASTER
 JOYState_TypeDef ReleasedButton  = JOY_NONE;
 JOYState_TypeDef PressedButton   = JOY_NONE;
 __IO uint8_t Tx_Idx = 0;
 __IO uint8_t CmdTransmitted = CMD_NONE;
 __IO uint8_t CmdStatus = 0x00;
 __IO uint8_t NumberOfByte = 0;
#endif

/* Private function prototypes -----------------------------------------------*/
static void SPI_Config(void);
static void SysTickConfig(void);

#ifdef SPI_MASTER
 static void TimeOut_UserCallback(void);
#endif

#ifdef SPI_SLAVE
 static TestStatus Buffercmp(uint8_t* pBuffer1, __IO uint8_t* pBuffer2, uint16_t BufferLength);
 static void Fill_Buffer(__IO uint8_t *pBuffer, uint16_t BufferLength);
#endif

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

  /* SPI configuration -------------------------------------------------------*/
  SPI_Config();

  /* SysTick configuration ---------------------------------------------------*/
  SysTickConfig();

  /* LEDs configuration ------------------------------------------------------*/
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

#ifdef SPI_MASTER
  /* Master board configuration ----------------------------------------------*/
  
  /* Initialize push-buttons mounted on STM322xG-EVAL board */
  TimeOut = USER_TIMEOUT;
  while ((IOE_Config() != IOE_OK) && (TimeOut != 0x00))
  {
  }
  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Initializes the SPI communication */
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPIx, &SPI_InitStructure);
  
  /* The Data transfer is performed in the SPI interrupt routine */
  /* Enable the SPI peripheral */
  SPI_Cmd(SPIx, ENABLE);

  while (1)
  {
    /* Prepare Command to be transmitted -------------------------------------*/
    
    /* Waiting joystick pressed */  
    PressedButton = JOY_NONE;  
    while (PressedButton == JOY_NONE)
    {
      PressedButton = IOE_JoyStickGetState();
    }
    
    /* Waiting joystick released */  
    ReleasedButton = IOE_JoyStickGetState();  
    while ((PressedButton == ReleasedButton) && (ReleasedButton != JOY_NONE))
    {
      ReleasedButton = IOE_JoyStickGetState();    
    }
    
    /* For each joystick state correspond to a Transaction command -----------*/
    CmdTransmitted = CMD_NONE;
    switch (PressedButton)
    {
      /* JOY_RIGHT button pressed */
      case JOY_RIGHT:
        CmdTransmitted = CMD_RIGHT;
        NumberOfByte = CMD_RIGHT_SIZE;
        break;
      /* JOY_LEFT button pressed */ 
      case JOY_LEFT:
        CmdTransmitted = CMD_LEFT;
        NumberOfByte = CMD_LEFT_SIZE;
        break;
      /* JOY_UP button pressed */
      case JOY_UP:
        CmdTransmitted = CMD_UP;
        NumberOfByte = CMD_UP_SIZE;
        break;
      /* JOY_DOWN button pressed */
      case JOY_DOWN:
        CmdTransmitted = CMD_DOWN;
        NumberOfByte = CMD_DOWN_SIZE;
        break;
      /* JOY_SEL button pressed */
      case JOY_SEL:
        CmdTransmitted = CMD_SEL;
        NumberOfByte = CMD_SEL_SIZE;
        break;
      default:
        break;
    }
    
    /* Master Transmit Command followed by Data Transaction ------------------*/
    CmdStatus = 0x00;
    Tx_Idx = 0;
    if (CmdTransmitted != CMD_NONE)
    {
      /* Enable the Tx buffer empty interrupt */
      SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, ENABLE);
      
      /* Waiting the end of Data transfer */
      while (Tx_Idx < GetVar_NbrOfData())
      {
      }
    }
  }
#endif /* SPI_MASTER */

#ifdef SPI_SLAVE
  /* Slave board configuration -----------------------------------------------*/
  
  /* Initializes the SPI communication */
  SPI_I2S_DeInit(SPIx);
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_Init(SPIx, &SPI_InitStructure);
  /* Enable the Rx buffer not empty interrupt */
  SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);
  /* Enable the SPI peripheral */
  SPI_Cmd(SPIx, ENABLE);

  /* Infinite Loop */
  while (1)
  {

    Rx_Idx = 0;
    /* Clear the RxBuffer */
    Fill_Buffer(RxBuffer, TXBUFFERSIZE);
    
    /* Waiting Transaction Command Byte reception ----------------------------*/
    while (RxBuffer[0] == CMD_NONE)
    {
    }
    
    /* Waiting Transaction Data reception ------------------------------------*/
    TimeOut = 1;
    while (TimeOut != 0x00)
    {
    }

    /* Check Transaction data correctness ------------------------------------*/
    switch (RxBuffer[0])
    {
      /* CMD_RIGHT command received */
      case CMD_RIGHT:
        if (Buffercmp(TxBuffer, &RxBuffer[1], CMD_RIGHT_SIZE) != FAILED) 
        {
          /* Turn ON LED2 and LED3 */
          STM_EVAL_LEDOn(LED2);
          STM_EVAL_LEDOn(LED3);
          /* Turn OFF LED4 */
          STM_EVAL_LEDOff(LED4);
        }
        break;
      /* CMD_LEFT command received */
      case CMD_LEFT:
        if (Buffercmp(TxBuffer, &RxBuffer[1], CMD_LEFT_SIZE) != FAILED)
        {
          /* Turn ON LED4 */
          STM_EVAL_LEDOn(LED4);
          /* Turn OFF LED2 and LED3 */
          STM_EVAL_LEDOff(LED2);
          STM_EVAL_LEDOff(LED3);
        }
        break;
      /* CMD_UP command received */
      case CMD_UP:
        if (Buffercmp(TxBuffer, &RxBuffer[1], CMD_UP_SIZE) != FAILED)
        {
          /* Turn ON LED2 */
          STM_EVAL_LEDOn(LED2);
          /* Turn OFF LED3 and LED4 */
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED4);
        }
        break;
      /* CMD_DOWN command received */
      case CMD_DOWN:
        if (Buffercmp(TxBuffer, &RxBuffer[1], CMD_DOWN_SIZE) != FAILED)
        {
          /* Turn ON LED3 */
          STM_EVAL_LEDOn(LED3);
          /* Turn OFF LED2 and LED4 */
          STM_EVAL_LEDOff(LED2);
          STM_EVAL_LEDOff(LED4);
        }
        break;
      /* CMD_SEL command received */
      case CMD_SEL:
        if (Buffercmp(TxBuffer, &RxBuffer[1], CMD_SEL_SIZE) != FAILED)
        {
          /* Turn ON LED2, LED3 and LED4 */
          STM_EVAL_LEDOn(LED2);
          STM_EVAL_LEDOn(LED3);
          STM_EVAL_LEDOn(LED4);
        }
        break;
      default:
        break;   
    }
  }
#endif /* SPI_SLAVE */
}

/**
  * @brief  Configures the SPI Peripheral.
  * @param  None
  * @retval None
  */
static void SPI_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the SPI clock */
  SPIx_CLK_INIT(SPIx_CLK, ENABLE);
  
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(SPIx_SCK_GPIO_CLK | SPIx_MISO_GPIO_CLK | SPIx_MOSI_GPIO_CLK, ENABLE);

  /* SPI GPIO Configuration --------------------------------------------------*/
  /* Connect SPI pins to AF5 */  
  GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT, SPIx_SCK_SOURCE, SPIx_SCK_AF);
  GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_SOURCE, SPIx_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = SPIx_SCK_PIN;
  GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  SPIx_MOSI_PIN;
  GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);
 
  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(SPIx);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  
  /* Configure the Priority Group to 1 bit */                
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Configure the SPI interrupt priority */
  NVIC_InitStructure.NVIC_IRQChannel = SPIx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configure a SysTick Base time to 10 ms.
  * @param  None
  * @retval None
  */
static void SysTickConfig(void)
{
  /* Setup SysTick Timer for 10ms interrupts  */
  if (SysTick_Config(SystemCoreClock / 100))
  {
    /* Capture error */
    while (1);
  }

  /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}

#ifdef SPI_MASTER
/**
  * @brief  Returns NumberOfByte variable value.
  * @param  None
  * @retval Tx Buffer Size (NumberOfByte).
  */
uint8_t GetVar_NbrOfData(void)
{
  return NumberOfByte;
}
/**
* @brief  Basic management of the timeout situation.
* @param  None.
* @retval None.
*/
static void TimeOut_UserCallback(void)
{
  /* User can add his own implementation to manage TimeOut Communication failure */
  /* Block communication and all processes */
  while(1)
  {
  }
}
#endif

#ifdef SPI_SLAVE
/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
static TestStatus Buffercmp(uint8_t* pBuffer1, __IO uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

/**
  * @brief  Fills buffer.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferLength: size of the buffer to fill
  * @retval None
  */
static void Fill_Buffer(__IO uint8_t *pBuffer, uint16_t BufferLength)
{
  uint32_t index = 0;

  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++ )
  {
    pBuffer[index] = 0x00;
  }
}
#endif /* SPI_SLAVE */


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
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
