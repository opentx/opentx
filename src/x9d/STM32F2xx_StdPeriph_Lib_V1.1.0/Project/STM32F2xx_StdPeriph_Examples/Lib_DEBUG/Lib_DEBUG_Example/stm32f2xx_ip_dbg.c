/**
  ******************************************************************************
  * @file    Lib_DEBUG/Lib_DEBUG_Example/stm32f21xx_ip_dbg.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   This file provides all peripherals pointers initialization.
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
#include "stm32f2xx_ip_dbg.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup Lib_DEBUG_Example
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_Common_TypeDef  * ADC_DBG;
ADC_TypeDef         * ADC1_DBG;
ADC_TypeDef         * ADC2_DBG;
ADC_TypeDef         * ADC3_DBG;
CAN_TypeDef         * CAN1_DBG;
CAN_TypeDef         * CAN2_DBG;
CRC_TypeDef         * CRC_DBG;
CRYP_TypeDef        * CRYP_DBG;
DAC_TypeDef         * DAC_DBG;
DBGMCU_TypeDef      * DBGMCU_DBG;
DCMI_TypeDef        * DCMI_DBG;
DMA_TypeDef         * DMA1_DBG;
DMA_Stream_TypeDef  * DMA1_Stream0_DBG;
DMA_Stream_TypeDef  * DMA1_Stream1_DBG;
DMA_Stream_TypeDef  * DMA1_Stream2_DBG;
DMA_Stream_TypeDef  * DMA1_Stream3_DBG;
DMA_Stream_TypeDef  * DMA1_Stream4_DBG;
DMA_Stream_TypeDef  * DMA1_Stream5_DBG;
DMA_Stream_TypeDef  * DMA1_Stream6_DBG;
DMA_Stream_TypeDef  * DMA1_Stream7_DBG;
DMA_TypeDef         * DMA2_DBG;
DMA_Stream_TypeDef  * DMA2_Stream0_DBG;
DMA_Stream_TypeDef  * DMA2_Stream1_DBG;
DMA_Stream_TypeDef  * DMA2_Stream2_DBG;
DMA_Stream_TypeDef  * DMA2_Stream3_DBG;
DMA_Stream_TypeDef  * DMA2_Stream4_DBG;
DMA_Stream_TypeDef  * DMA2_Stream5_DBG;
DMA_Stream_TypeDef  * DMA2_Stream6_DBG;
DMA_Stream_TypeDef  * DMA2_Stream7_DBG;
EXTI_TypeDef        * EXTI_DBG;
ETH_TypeDef         * ETH_DBG;
FLASH_TypeDef       * FLASH_DBG;
FSMC_Bank1_TypeDef  * FSMC_Bank1_DBG;
FSMC_Bank1E_TypeDef * FSMC_Bank1E_DBG;
FSMC_Bank2_TypeDef  * FSMC_Bank2_DBG;
FSMC_Bank3_TypeDef  * FSMC_Bank3_DBG;
FSMC_Bank4_TypeDef  * FSMC_Bank4_DBG;
GPIO_TypeDef        * GPIOA_DBG;
GPIO_TypeDef        * GPIOB_DBG;
GPIO_TypeDef        * GPIOC_DBG;
GPIO_TypeDef        * GPIOD_DBG;
GPIO_TypeDef        * GPIOE_DBG;
GPIO_TypeDef        * GPIOF_DBG;
GPIO_TypeDef        * GPIOG_DBG;
GPIO_TypeDef        * GPIOH_DBG;
GPIO_TypeDef        * GPIOI_DBG;
HASH_TypeDef        * HASH_DBG;
I2C_TypeDef         * I2C1_DBG;
I2C_TypeDef         * I2C2_DBG;
I2C_TypeDef         * I2C3_DBG;
IWDG_TypeDef        * IWDG_DBG;
PWR_TypeDef         * PWR_DBG;
RCC_TypeDef         * RCC_DBG;
RNG_TypeDef         * RNG_DBG;
RTC_TypeDef         * RTC_DBG;
SDIO_TypeDef        * SDIO_DBG;
SPI_TypeDef         * SPI1_DBG;
SPI_TypeDef         * SPI2_DBG;
SPI_TypeDef         * SPI3_DBG;
SYSCFG_TypeDef      * SYSCFG_DBG;
TIM_TypeDef         * TIM1_DBG;
TIM_TypeDef         * TIM2_DBG;
TIM_TypeDef         * TIM3_DBG;
TIM_TypeDef         * TIM4_DBG;
TIM_TypeDef         * TIM5_DBG;
TIM_TypeDef         * TIM6_DBG;
TIM_TypeDef         * TIM7_DBG;
TIM_TypeDef         * TIM8_DBG;
TIM_TypeDef         * TIM9_DBG;
TIM_TypeDef         * TIM10_DBG;
TIM_TypeDef         * TIM11_DBG;
TIM_TypeDef         * TIM12_DBG;
TIM_TypeDef         * TIM13_DBG;
TIM_TypeDef         * TIM14_DBG;
USART_TypeDef       * USART1_DBG;
USART_TypeDef       * USART2_DBG;
USART_TypeDef       * USART3_DBG;
USART_TypeDef       * UART4_DBG;
USART_TypeDef       * UART5_DBG;
USART_TypeDef       * USART6_DBG;
WWDG_TypeDef        * WWDG_DBG;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function initialize peripherals pointers..
  * @param  None
  * @retval None
  */
void IP_Debug(void)
{
/************************************* ADC ************************************/
  ADC_DBG  = (ADC_Common_TypeDef *)  ADC_BASE;
  ADC1_DBG = (ADC_TypeDef *)  ADC1_BASE;
  ADC2_DBG = (ADC_TypeDef *)  ADC2_BASE;
  ADC3_DBG = (ADC_TypeDef *)  ADC3_BASE;

/************************************* CAN ************************************/
  CAN1_DBG = (CAN_TypeDef *)  CAN1_BASE;
  CAN2_DBG = (CAN_TypeDef *)  CAN2_BASE;

/************************************* CRC ************************************/  
  CRC_DBG = (CRC_TypeDef *)  CRC_BASE;

/************************************* CRYP ***********************************/
  CRYP_DBG = (CRYP_TypeDef *)  CRYP_BASE;

/************************************* DAC ************************************/
  DAC_DBG = (DAC_TypeDef *)  DAC_BASE;

/************************************* DBGMCU *********************************/
	DBGMCU_DBG = (DBGMCU_TypeDef *)  DBGMCU_BASE;

/************************************* DCMI ***********************************/
  DCMI_DBG = (DCMI_TypeDef *)  DCMI_BASE;

/************************************* DMA ************************************/
  DMA1_DBG = (DMA_TypeDef *)  DMA1_BASE;
  DMA1_Stream0_DBG = (DMA_Stream_TypeDef *)  DMA1_Stream0_BASE;
  DMA1_Stream1_DBG = (DMA_Stream_TypeDef *)  DMA1_Stream1_BASE;
  DMA1_Stream2_DBG = (DMA_Stream_TypeDef *)  DMA1_Stream2_BASE;
  DMA1_Stream3_DBG = (DMA_Stream_TypeDef *)  DMA1_Stream3_BASE;
  DMA1_Stream4_DBG = (DMA_Stream_TypeDef *)  DMA1_Stream4_BASE;
  DMA1_Stream5_DBG = (DMA_Stream_TypeDef *)  DMA1_Stream5_BASE;
  DMA1_Stream6_DBG = (DMA_Stream_TypeDef *)  DMA1_Stream6_BASE;
  DMA1_Stream7_DBG = (DMA_Stream_TypeDef *)  DMA1_Stream7_BASE;
  DMA2_DBG = (DMA_TypeDef *)  DMA2_BASE;
  DMA2_Stream0_DBG = (DMA_Stream_TypeDef *)  DMA2_Stream0_BASE;
  DMA2_Stream1_DBG = (DMA_Stream_TypeDef *)  DMA2_Stream1_BASE;
  DMA2_Stream2_DBG = (DMA_Stream_TypeDef *)  DMA2_Stream2_BASE;
  DMA2_Stream3_DBG = (DMA_Stream_TypeDef *)  DMA2_Stream3_BASE;
  DMA2_Stream4_DBG = (DMA_Stream_TypeDef *)  DMA2_Stream4_BASE;
  DMA2_Stream5_DBG = (DMA_Stream_TypeDef *)  DMA2_Stream5_BASE;
  DMA2_Stream6_DBG = (DMA_Stream_TypeDef *)  DMA2_Stream6_BASE;
	DMA2_Stream7_DBG = (DMA_Stream_TypeDef *) DMA2_Stream7_BASE;

/************************************* EXTI ***********************************/
  EXTI_DBG = (EXTI_TypeDef *)  EXTI_BASE;

/************************************* ETH ************************************/
  ETH_DBG = (ETH_TypeDef *)  ETH_BASE;

/************************************* FLASH **********************************/
  FLASH_DBG = (FLASH_TypeDef *)  FLASH_R_BASE;

/************************************* FSMC ***********************************/
  FSMC_Bank1_DBG  = (FSMC_Bank1_TypeDef *)  FSMC_Bank1_R_BASE;
  FSMC_Bank1E_DBG = (FSMC_Bank1E_TypeDef *)  FSMC_Bank1E_R_BASE;
  FSMC_Bank2_DBG  = (FSMC_Bank2_TypeDef *)  FSMC_Bank2_R_BASE;
  FSMC_Bank3_DBG  = (FSMC_Bank3_TypeDef *)  FSMC_Bank3_R_BASE;
  FSMC_Bank4_DBG  = (FSMC_Bank4_TypeDef *)  FSMC_Bank4_R_BASE;

/************************************* GPIO ***********************************/
  GPIOA_DBG = (GPIO_TypeDef *)  GPIOA_BASE;
  GPIOB_DBG = (GPIO_TypeDef *)  GPIOB_BASE;
  GPIOC_DBG = (GPIO_TypeDef *)  GPIOC_BASE;
  GPIOD_DBG = (GPIO_TypeDef *)  GPIOD_BASE;
  GPIOE_DBG = (GPIO_TypeDef *)  GPIOE_BASE;
  GPIOF_DBG = (GPIO_TypeDef *)  GPIOF_BASE;
  GPIOG_DBG = (GPIO_TypeDef *)  GPIOG_BASE;
  GPIOH_DBG = (GPIO_TypeDef *)  GPIOH_BASE;
  GPIOI_DBG = (GPIO_TypeDef *)  GPIOI_BASE;

/************************************* HASH ***********************************/
  HASH_DBG = (HASH_TypeDef *)  HASH_BASE;

/************************************* I2C ************************************/
  I2C1_DBG = (I2C_TypeDef *)  I2C1_BASE;
  I2C2_DBG = (I2C_TypeDef *)  I2C2_BASE;
  I2C3_DBG = (I2C_TypeDef *)  I2C3_BASE;

/************************************* IWDG ***********************************/
  IWDG_DBG = (IWDG_TypeDef *)  IWDG_BASE;

/************************************* PWR ************************************/
  PWR_DBG = (PWR_TypeDef *)  PWR_BASE;

/************************************* RCC ************************************/
  RCC_DBG = (RCC_TypeDef *)  RCC_BASE;

/************************************* RNG ************************************/
  RNG_DBG = (RNG_TypeDef *)  RNG_BASE;

/************************************* RTC ************************************/
  RTC_DBG = (RTC_TypeDef *)  RTC_BASE;

/************************************* SDIO ***********************************/
  SDIO_DBG = (SDIO_TypeDef *)  SDIO_BASE;

/************************************* SPI ************************************/
  SPI1_DBG = (SPI_TypeDef *)  SPI1_BASE;
  SPI2_DBG = (SPI_TypeDef *)  SPI2_BASE;
  SPI3_DBG = (SPI_TypeDef *)  SPI3_BASE;

/************************************* SYSCFG *********************************/
  SYSCFG_DBG = (SYSCFG_TypeDef *)  SYSCFG_BASE;

/************************************* TIM ************************************/
  TIM1_DBG  = (TIM_TypeDef *)  TIM1_BASE;
  TIM2_DBG  = (TIM_TypeDef *)  TIM2_BASE;
  TIM3_DBG  = (TIM_TypeDef *)  TIM3_BASE;
  TIM4_DBG  = (TIM_TypeDef *)  TIM4_BASE;
  TIM5_DBG  = (TIM_TypeDef *)  TIM5_BASE;
  TIM6_DBG  = (TIM_TypeDef *)  TIM6_BASE;
  TIM7_DBG  = (TIM_TypeDef *)  TIM7_BASE;
  TIM8_DBG  = (TIM_TypeDef *)  TIM8_BASE;
  TIM9_DBG  = (TIM_TypeDef *)  TIM9_BASE;
  TIM10_DBG = (TIM_TypeDef *)  TIM10_BASE;
  TIM11_DBG = (TIM_TypeDef *)  TIM11_BASE;
  TIM12_DBG = (TIM_TypeDef *)  TIM12_BASE;
  TIM13_DBG = (TIM_TypeDef *)  TIM13_BASE;
  TIM14_DBG = (TIM_TypeDef *)  TIM14_BASE;

/************************************* USART **********************************/
  USART1_DBG = (USART_TypeDef *)  USART1_BASE;
  USART2_DBG = (USART_TypeDef *)  USART2_BASE;
  USART3_DBG = (USART_TypeDef *)  USART3_BASE;
  UART4_DBG  = (USART_TypeDef *)  UART4_BASE;
  UART5_DBG  = (USART_TypeDef *)  UART5_BASE;
  USART6_DBG = (USART_TypeDef *)  USART6_BASE;

/************************************* WWDG************************************/
  WWDG_DBG = (WWDG_TypeDef *)  WWDG_BASE;
}

/**
  * @}
  */

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
