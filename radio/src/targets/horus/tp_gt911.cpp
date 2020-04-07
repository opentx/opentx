/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "tp_gt911.h"

uint8_t touchGT911Flag = 0;
uint8_t TOUCH_SCAN_MODE = 0;
uint8_t touchEventFlag = 0;

void GT911_INT_Change(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = TOUCH_INT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(TOUCH_INT_GPIO, &GPIO_InitStructure);
}

uint8_t GT911_Send_Cfg(uint8_t mode)
{
  u8 buf[2];
  u8 i = 0;
  buf[0] = 0;
  buf[1] = mode;    //
  for (i = 0; i < sizeof(GT911_Cfg); i++)
    buf[0] += GT911_Cfg[i];//check sum

  buf[0] = (~buf[0]) + 1;
  GT911_WR_Reg(GT_CFGS_REG, (u8 *) GT911_Cfg, sizeof(GT911_Cfg));//
  GT911_WR_Reg(GT_CHECK_REG, buf, 2);//write checksum
  return 0;
}

static void IIC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  //I2C_InitTypeDef     I2C_InitStructure;

  /* GPIO Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(I2C_RCC_AHB1Periph, ENABLE);
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  //PB6: I2C1_SCL  PB7: I2C1_SDA
  GPIO_InitStructure.GPIO_Pin = I2C_SCL_GPIO_PIN | I2C_SDA_GPIO_PIN;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);

  GPIO_SetBits(I2C_GPIO, I2C_SCL_GPIO_PIN | I2C_SDA_GPIO_PIN);
}

static void TOUCH_AF_ExtiStop(void)
{
  SYSCFG_EXTILineConfig(TOUCH_INT_EXTI_PortSource, TOUCH_INT_EXTI_PinSource1);

  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = TOUCH_INT_EXTI_LINE1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);


  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TOUCH_INT_EXTI_IRQn1;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; /* Not used as 4 bits are used for the pre-emption priority. */;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static void TOUCH_AF_ExtiConfig(void)
{
  SYSCFG_EXTILineConfig(TOUCH_INT_EXTI_PortSource, TOUCH_INT_EXTI_PinSource1);

  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = TOUCH_INT_EXTI_LINE1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);


  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TOUCH_INT_EXTI_IRQn1;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; /* Not used as 4 bits are used for the pre-emption priority. */;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static void TOUCH_AF_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = TOUCH_RST_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(TOUCH_RST_GPIO, &GPIO_InitStructure);

  GPIO_ResetBits(TOUCH_RST_GPIO, TOUCH_RST_GPIO_PIN);

  GPIO_InitStructure.GPIO_Pin = TOUCH_INT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(TOUCH_INT_GPIO, &GPIO_InitStructure);

  GPIO_ResetBits(TOUCH_INT_GPIO, TOUCH_INT_GPIO_PIN);
}

void touchPanelDeInit(void)
{
  TOUCH_AF_ExtiStop();
}

bool touchPanelInit(void)
{
  uint8_t tmp[4] = {0};

  TRACE("Touchpanel init start ...");

  TOUCH_AF_GPIOConfig(); //SET RST=OUT INT=OUT INT=LOW
  IIC_Config();

  TPRST_LOW();
  TPINT_HIGH();
  delay_us(200);   //DELAY 200US

  TPRST_HIGH();
  delay_ms(6);   //DELAY 6MS

  TPINT_LOW();
  delay_ms(55);   //DELAY 55MS

  GT911_INT_Change();  //Set INT INPUT INT=LOW

  delay_ms(50);   //DELAY 55MS

  GT911_RD_Reg(GT_PID_REG, tmp, 4);

  if (strcmp((char *) tmp, "911") == 0)//ID==9147
  {
    tmp[0] = 0X02;
    GT911_WR_Reg(GT_CTRL_REG, tmp, 1);//
    GT911_RD_Reg(GT_CFGS_REG, tmp, 1);//

    if (tmp[0] < 0X69)//ver
    {
      //printf("Default Ver:%x\r\n",temp[0]);
      GT911_Send_Cfg(1);  //
    }

    delay_ms(10);
    tmp[0] = 0X00;
    GT911_WR_Reg(GT_CTRL_REG, tmp, 1);//end reset
    touchGT911Flag = true;

    TOUCH_AF_ExtiConfig();

    return true;
  }

  return false;
}

extern "C" void TOUCH_INT_EXTI_IRQHandler1(void)
{
  if (EXTI_GetITStatus(TOUCH_INT_EXTI_LINE1) != RESET) {
    TRACE("TOUCH INTERUPT");
    touchEventFlag = 1;
    EXTI_ClearITPendingBit(TOUCH_INT_EXTI_LINE1);
  }
}
