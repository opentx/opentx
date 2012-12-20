/**
  ******************************************************************************
  * @file    USART/Smartcard/platform_config.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Evaluation board specific configuration file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */
#if !defined (USE_STM322xG_EVAL)
 //#define USE_STM322xG_EVAL
#endif

/* Define the STM32F2xx hardware depending on the used evaluation board */
#ifdef USE_STM322xG_EVAL
  #define SC_USART                           USART6
  #define SC_USART_CLK                       RCC_APB2Periph_USART6
  #define SC_USART_APBPERIPHCLOCK            RCC_APB2PeriphClockCmd
  #define SC_USART_IRQn                      USART6_IRQn
  #define SC_USART_IRQHandler                USART6_IRQHandler

  #define SC_USART_TX_PIN                    GPIO_Pin_6                
  #define SC_USART_TX_GPIO_PORT              GPIOC                       
  #define SC_USART_TX_GPIO_CLK               RCC_AHB1Periph_GPIOC
  #define SC_USART_TX_SOURCE                 GPIO_PinSource6
  #define SC_USART_TX_AF                     GPIO_AF_USART6

  #define SC_USART_CK_PIN                    GPIO_Pin_7                
  #define SC_USART_CK_GPIO_PORT              GPIOG                    
  #define SC_USART_CK_GPIO_CLK               RCC_AHB1Periph_GPIOG
  #define SC_USART_CK_SOURCE                 GPIO_PinSource7
  #define SC_USART_CK_AF                     GPIO_AF_USART6


  /* Smartcard Inteface GPIO pins */
  #define SC_3_5V_PIN                        GPIO_Pin_15
  #define SC_3_5V_GPIO_PORT                  GPIOH
  #define SC_3_5V_GPIO_CLK                   RCC_AHB1Periph_GPIOH
  
  #define SC_RESET_PIN                       GPIO_Pin_7
  #define SC_RESET_GPIO_PORT                 GPIOF
  #define SC_RESET_GPIO_CLK                  RCC_AHB1Periph_GPIOF

  #define SC_CMDVCC_PIN                      GPIO_Pin_12
  #define SC_CMDVCC_GPIO_PORT                GPIOG
  #define SC_CMDVCC_GPIO_CLK                 RCC_AHB1Periph_GPIOG

  #define SC_OFF_PIN                         GPIO_Pin_6
  #define SC_OFF_GPIO_PORT                   GPIOF
  #define SC_OFF_GPIO_CLK                    RCC_AHB1Periph_GPIOF
  #define SC_OFF_EXTI_LINE                   EXTI_Line6
  #define SC_OFF_EXTI_PORT_SOURCE            EXTI_PortSourceGPIOF
  #define SC_OFF_EXTI_PIN_SOURCE             EXTI_PinSource6
  #define SC_OFF_EXTI_IRQn                   EXTI9_5_IRQn 
  #define SC_OFF_EXTI_IRQHandler             EXTI9_5_IRQHandler
  
#endif /* USE_STM322xG_EVAL */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
