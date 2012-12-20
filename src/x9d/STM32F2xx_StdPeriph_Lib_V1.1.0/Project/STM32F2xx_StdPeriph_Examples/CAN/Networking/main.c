/**
  ******************************************************************************
  * @file    CAN/Networking/main.c 
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

/** @addtogroup CAN_Networking
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define KEY_PRESSED     0x00
#define KEY_NOT_PRESSED 0x01

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CAN_InitTypeDef        CAN_InitStructure;
CAN_FilterInitTypeDef  CAN_FilterInitStructure;
CanTxMsg TxMessage;
uint8_t KeyNumber = 0x0;

/* Private function prototypes -----------------------------------------------*/
void NVIC_Config(void);
void CAN_Config(void);
void Init_RxMes(CanRxMsg *RxMessage);
void Delay(void);

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
       
  /* NVIC configuration */
  NVIC_Config();

  /* Configures LED 1..4 */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  /* Configure Push button key */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO); 
   
  /* CAN configuration */
  CAN_Config();
  
  /* Infinite loop */
  while(1)
  {
    while(STM_EVAL_PBGetState(BUTTON_KEY) == KEY_PRESSED)
    {
      if(KeyNumber == 0x4) 
      {
        KeyNumber = 0x00;
      }
      else
      {
        LED_Display(++KeyNumber);
        TxMessage.Data[0] = KeyNumber;
        CAN_Transmit(CANx, &TxMessage);
        Delay();
        
        while(STM_EVAL_PBGetState(BUTTON_KEY) != KEY_NOT_PRESSED)
        {
        }
      }
    }
  }
}

/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
void CAN_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* CAN GPIOs configuration **************************************************/

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(CAN_GPIO_CLK, ENABLE);

  /* Connect CAN pins to AF9 */
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT); 
  
  /* Configure CAN RX and TX pins */
  GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

  /* CAN configuration ********************************************************/  
  /* Enable CAN clock */
  RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);
  
  /* CAN register init */
  CAN_DeInit(CANx);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    
  /* CAN Baudrate = 1MBps (CAN clocked at 30 MHz) */
  CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
  CAN_InitStructure.CAN_Prescaler = 2;
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
  
  /* Transmit Structure preparation */
  TxMessage.StdId = 0x321;
  TxMessage.ExtId = 0x01;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
  TxMessage.DLC = 1;
  
  /* Enable FIFO 0 message pending Interrupt */
  CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}

/**
  * @brief  Configures the NVIC for CAN.
  * @param  None
  * @retval None
  */
void NVIC_Config(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;

#ifdef  USE_CAN1 
  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
#else  /* USE_CAN2 */
  NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
#endif /* USE_CAN1 */

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Initializes the Rx Message.
  * @param  RxMessage: pointer to the message to initialize
  * @retval None
  */
void Init_RxMes(CanRxMsg *RxMessage)
{
  uint8_t i = 0;

  RxMessage->StdId = 0x00;
  RxMessage->ExtId = 0x00;
  RxMessage->IDE = CAN_ID_STD;
  RxMessage->DLC = 0;
  RxMessage->FMI = 0;
  for (i = 0;i < 8;i++)
  {
    RxMessage->Data[i] = 0x00;
  }
}

/**
  * @brief  Turn ON/OFF the dedicated led
  * @param  Ledstatus: Led number from 0 to 3.
  * @retval None
  */
void LED_Display(uint8_t Ledstatus)
{
  /* Turn off all leds */
  STM_EVAL_LEDOff(LED1);
  STM_EVAL_LEDOff(LED2);
  STM_EVAL_LEDOff(LED3);
  STM_EVAL_LEDOff(LED4);
  
  switch(Ledstatus)
  {
    case(1): 
      STM_EVAL_LEDOn(LED1);
      break;
   
    case(2): 
      STM_EVAL_LEDOn(LED2);
      break;
 
    case(3): 
      STM_EVAL_LEDOn(LED3);
      break;

    case(4): 
      STM_EVAL_LEDOn(LED4);
      break;
    default:
      break;
  }
}

/**
  * @brief  Delay
  * @param  None
  * @retval None
  */
void Delay(void)
{
  uint16_t nTime = 0x0000;

  for(nTime = 0; nTime <0xFFF; nTime++)
  {
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
