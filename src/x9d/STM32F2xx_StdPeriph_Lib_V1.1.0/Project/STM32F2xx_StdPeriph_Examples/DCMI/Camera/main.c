/**
  ******************************************************************************
  * @file    DCMI/Camera/main.c 
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
#include "dcmi_ov9655.h"
#include "dcmi_ov2640.h"
#include  "lcd_log.h"

/** @addtogroup stm32f2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup DCMI_Camera
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks;
OV9655_IDTypeDef OV9655_Camera_ID;
OV2640_IDTypeDef OV2640_Camera_ID;
__IO uint16_t ADCVal = 0;
uint8_t buffer[40];
extern Camera_TypeDef Camera;
extern ImageFormat_TypeDef ImageFormat;
extern __IO uint8_t ValueMax;
extern const uint8_t *ImageForematArray[];

/* Private function prototypes -----------------------------------------------*/
void ADC_Config(void);

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

  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

  /* Initialize LEDs and push-buttons mounted on STM322xG-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  STM_EVAL_LEDOn(LED1);

 /* Initialize the LCD */
  STM322xG_LCD_Init();
  LCD_Clear(Black);
  LCD_SetTextColor(White);

  LCD_LOG_SetHeader("STM32 Camera Demo");
  LCD_LOG_SetFooter ("   Copyright (c) STMicroelectronics" );

  /* ADC configuration */
  ADC_Config();

  /* Initializes the DCMI interface (I2C and GPIO) used to configure the camera */
  OV2640_HW_Init();

  /* Read the OV9655/OV2640 Manufacturer identifier */
  OV9655_ReadID(&OV9655_Camera_ID);
  OV2640_ReadID(&OV2640_Camera_ID);

  if(OV9655_Camera_ID.PID  == 0x96)
  {
    Camera = OV9655_CAMERA;
    sprintf((char*)buffer, "OV9655 Camera ID 0x%x", OV9655_Camera_ID.PID);
    ValueMax = 2;
  }
  else if(OV2640_Camera_ID.PIDH  == 0x26)
  {
    Camera = OV2640_CAMERA;
    sprintf((char*)buffer, "OV2640 Camera ID 0x%x", OV2640_Camera_ID.PIDH);
    ValueMax = 2;
  }
  else
  {
    LCD_SetTextColor(LCD_COLOR_RED);
    LCD_DisplayStringLine(LINE(4), "Check the Camera HW and try again");
    while(1);  
  }

  LCD_SetTextColor(LCD_COLOR_YELLOW);
  LCD_DisplayStringLine(LINE(4), (uint8_t*)buffer);
  LCD_SetTextColor(LCD_COLOR_WHITE);
  Delay(200);

  /* Initialize demo */
  ImageFormat = (ImageFormat_TypeDef)Demo_Init();

  /* Configure the Camera module mounted on STM322xG-EVAL board */
  Demo_LCD_Clear();
  LCD_DisplayStringLine(LINE(4), "Camera Init..               ");
  Camera_Config();

  sprintf((char*)buffer, " Image selected: %s", ImageForematArray[ImageFormat]);
  LCD_DisplayStringLine(LINE(4),(uint8_t*)buffer);

  /* Enable DMA2 stream 1 and DCMI interface then start image capture */
  DMA_Cmd(DMA2_Stream1, ENABLE); 
  DCMI_Cmd(ENABLE); 

  /* Insert 100ms delay: wait 100ms */
  Delay(200); 

  DCMI_CaptureCmd(ENABLE); 

  LCD_ClearLine(LINE(4));
  Demo_LCD_Clear();

  if(ImageFormat == BMP_QQVGA)
  {
    /* LCD Display window */
    LCD_SetDisplayWindow(179, 239, 120, 160);
    LCD_WriteReg(LCD_REG_3, 0x1038);
    LCD_WriteRAM_Prepare(); 
  }
  else if(ImageFormat == BMP_QVGA)
  {
    /* LCD Display window */
    LCD_SetDisplayWindow(239, 319, 240, 320);
    LCD_WriteReg(LCD_REG_3, 0x1038);
    LCD_WriteRAM_Prepare(); 
  }

  while(1)
  {
    /* Blink LD1, LED2 and LED4 */
    STM_EVAL_LEDToggle(LED1);
    STM_EVAL_LEDToggle(LED2);
    STM_EVAL_LEDToggle(LED3);
    STM_EVAL_LEDToggle(LED4);

    /* Insert 100ms delay */
    Delay(10);

    /* Get the last ADC3 conversion result data */
    ADCVal = ADC_GetConversionValue(ADC3);
    
    /* Change the Brightness of camera using "Brightness Adjustment" register:
       For OV9655 camera Brightness can be positively (0x01 ~ 0x7F) and negatively (0x80 ~ 0xFF) adjusted
       For OV2640 camera Brightness can be positively (0x20 ~ 0x40) and negatively (0 ~ 0x20) adjusted */
    if(Camera == OV9655_CAMERA)
    {
      OV9655_BrightnessConfig(ADCVal);
    }
    if(Camera == OV2640_CAMERA)
    {
      OV2640_BrightnessConfig(ADCVal/2);
    }
  }
}

/**
  * @brief  Configures the ADC.
  * @param  None
  * @retval None
  */
void ADC_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable ADC3 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

  /* GPIOF clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 

  /* Configure ADC Channel7 as analog */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* ADC Common Init */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div6;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; 
  ADC_CommonInit(&ADC_CommonInitStructure); 

  /* ADC3 Configuration ------------------------------------------------------*/
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC3, &ADC_InitStructure);

  /* ADC3 Regular Channel Config */
  ADC_RegularChannelConfig(ADC3, ADC_Channel_7, 1, ADC_SampleTime_56Cycles);

  /* Enable ADC3 */
  ADC_Cmd(ADC3, ENABLE);

  /* ADC3 regular Software Start Conv */ 
  ADC_SoftwareStartConv(ADC3);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
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
