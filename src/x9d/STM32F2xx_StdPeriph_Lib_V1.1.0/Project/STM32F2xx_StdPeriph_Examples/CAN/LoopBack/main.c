/**
  ******************************************************************************
  * @file    CAN/LoopBack/main.c 
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

/** @addtogroup CAN_LoopBack
  * @{
  */ 

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t ret = 0; /* for return of the interrupt handling */
volatile TestStatus TestRx;

/* Private function prototypes -----------------------------------------------*/
void NVIC_Config(void);
TestStatus CAN_Polling(void);
TestStatus CAN_Interrupt(void);

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

#ifdef USE_CAN1
  /* CANx Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
#else /* USE_CAN2 */
  /* CAN1 & 2  Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2, ENABLE);
#endif  /* USE_CAN1 */

  /* NVIC Configuration */
  NVIC_Config();
  
  /* Configures LED 1..4 */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* CAN transmit at 125Kb/s and receive by polling in loopback mode */
  TestRx = CAN_Polling();

  if (TestRx !=  FAILED)
  { /* OK */

    /* Turn on LED1 */
    STM_EVAL_LEDOn(LED1);
  }
  else
  { /* KO */

    /* Turn on LED3 */
    STM_EVAL_LEDOn(LED3);
  }

  /* CAN transmit at 500Kb/s and receive by interrupt in loopback mode */
  TestRx = CAN_Interrupt();

  if (TestRx !=  FAILED)
  { /* OK */

    /* Turn on LED4 */
    STM_EVAL_LEDOn(LED4);
  }
  else
  { /* KO */

    /* Turn on LED2 */
    STM_EVAL_LEDOn(LED2);
  }

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  Configures the CAN, transmit and receive by polling
  * @param  None
  * @retval PASSED if the reception is well done, FAILED in other case
  */
TestStatus CAN_Polling(void)
{
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  CanTxMsg TxMessage;
  CanRxMsg RxMessage;
  uint32_t i = 0;
  uint8_t TransmitMailbox = 0;

  /* CAN register init */
  CAN_DeInit(CANx);
  
  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

  /* CAN Baudrate = 125kbps (CAN clocked at 30 MHz) */
  CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
  CAN_InitStructure.CAN_Prescaler = 16;
  CAN_Init(CANx, &CAN_InitStructure);

  /* CAN filter init */
#ifdef  USE_CAN1
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
#else /* USE_CAN2 */
  CAN_FilterInitStructure.CAN_FilterNumber = 14;
#endif  /* USE_CAN1 */
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;  
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;

  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  /* transmit */
  TxMessage.StdId = 0x11;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
  TxMessage.DLC = 2;
  TxMessage.Data[0] = 0xCA;
  TxMessage.Data[1] = 0xFE;

  TransmitMailbox = CAN_Transmit(CANx, &TxMessage);
  i = 0;
  while((CAN_TransmitStatus(CANx, TransmitMailbox)  !=  CANTXOK) && (i  !=  0xFFFF))
  {
    i++;
  }

  i = 0;
  while((CAN_MessagePending(CANx, CAN_FIFO0) < 1) && (i  !=  0xFFFF))
  {
    i++;
  }

  /* receive */
  RxMessage.StdId = 0x00;
  RxMessage.IDE = CAN_ID_STD;
  RxMessage.DLC = 0;
  RxMessage.Data[0] = 0x00;
  RxMessage.Data[1] = 0x00;
  CAN_Receive(CANx, CAN_FIFO0, &RxMessage);

  if (RxMessage.StdId != 0x11)
  {
    return FAILED;  
  }

  if (RxMessage.IDE != CAN_ID_STD)
  {
    return FAILED;
  }

  if (RxMessage.DLC != 2)
  {
    return FAILED;  
  }

  if ((RxMessage.Data[0]<<8|RxMessage.Data[1]) != 0xCAFE)
  {
    return FAILED;
  }
  
  return PASSED; /* Test Passed */
}

/**
  * @brief  Configures the CAN, transmit and receive using interrupt.
  * @param  None
  * @retval PASSED if the reception is well done, FAILED in other case
  */
TestStatus CAN_Interrupt(void)
{
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  CanTxMsg TxMessage;
  uint32_t i = 0;

  /* CAN register init */
  CAN_DeInit(CANx);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  
  /* Baudrate = 500 Kbps */
  CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
  CAN_InitStructure.CAN_Prescaler = 4;
  CAN_Init(CANx, &CAN_InitStructure);

  /* CAN filter init */
#ifdef  USE_CAN1
  CAN_FilterInitStructure.CAN_FilterNumber = 1;
#else /* USE_CAN2 */
  CAN_FilterInitStructure.CAN_FilterNumber = 15;
#endif  /* USE_CAN1 */
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  /* CAN FIFO0 message pending interrupt enable */ 
  CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);

  /* transmit 1 message */
  TxMessage.StdId = 0;
  TxMessage.ExtId = 0x1234;
  TxMessage.IDE = CAN_ID_EXT;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.DLC = 2;
  TxMessage.Data[0] = 0xDE;
  TxMessage.Data[1] = 0xCA;
  CAN_Transmit(CANx, &TxMessage);

  /* initialize the value that will be returned */
  ret = 0xFF;
       
  /* receive message with interrupt handling */
  i = 0;
  while((ret ==  0xFF) && (i < 0xFFF))
  {
    i++;
  }
  
  if (i ==  0xFFF)
  {
    ret = 0;  
  }

  /* disable interrupt handling */
  CAN_ITConfig(CANx, CAN_IT_FMP0, DISABLE);

  return (TestStatus)ret;
}

/**
  * @brief  Configures the NVIC.
  * @param  None
  * @retval None
  */
void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable CANx RX0 interrupt IRQ channel */
#ifdef  USE_CAN1 
  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
#else  /* USE_CAN2 */
  NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
#endif /* USE_CAN1 */

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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
