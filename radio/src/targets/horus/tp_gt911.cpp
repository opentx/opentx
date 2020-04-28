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
uint8_t touchPanelEvent = 0;
struct TouchData touchData;
struct TouchState touchState;

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
  uint8_t buf[2];
  uint8_t i = 0;
  buf[0] = 0;
  buf[1] = mode;
  for (i = 0; i < sizeof(GT911_Cfg); i++)
    buf[0] += GT911_Cfg[i];//check sum

  buf[0] = (~buf[0]) + 1;
  gt911WriteRegister(GT_CFGS_REG, (uint8_t *) GT911_Cfg, sizeof(GT911_Cfg));//
  gt911WriteRegister(GT_CHECK_REG, buf, 2);//write checksum
  return 0;
}

static void I2C_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(I2C_RCC_AHB1Periph, ENABLE);
  GPIO_InitStructure.GPIO_Pin = I2C_SCL_GPIO_PIN | I2C_SDA_GPIO_PIN;
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

void SDA_IN(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = I2C_SDA_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);
}

void SDA_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = I2C_SDA_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);
}

void I2C_Start(void)
{
  SDA_OUT();
  I2C_SDA_H();
  I2C_SCL_H();
  delay_us(2);
  I2C_SDA_L();//START:when CLK is high,DATA change form high to low
  delay_us(2);
  I2C_SCL_L();//
  delay_us(2);
}

void I2C_Stop(void)
{
  SDA_OUT();
  I2C_SCL_L();
  I2C_SDA_L();//STOP:when CLK is high DATA change form low to high
  delay_us(2);
  I2C_SCL_H();
  I2C_SDA_H();//
  delay_us(2);
}

uint8_t I2C_Wait_Ack(void)
{
  uint8_t ucErrTime = 0;
  SDA_IN();
  I2C_SDA_H();
  delay_us(2);
  I2C_SCL_H();
  while (READ_SDA) {
    ucErrTime++;
    if (ucErrTime > 166) {
      I2C_Stop();
      return 1;
    }
    delay_us(1);
  }
  I2C_SDA_L();
  I2C_SCL_L();
  return 0;
}

void I2C_Ack(void)
{
  I2C_SCL_L();
  SDA_OUT();
  I2C_SDA_L();
  delay_us(2);
  I2C_SCL_H();
  delay_us(2);
  I2C_SCL_L();
}

void I2C_NAck(void)
{
  I2C_SCL_L();
  SDA_OUT();
  I2C_SDA_H();
  delay_us(2);
  I2C_SCL_H();
  delay_us(2);
  I2C_SCL_L();
}

void I2C_Send_Byte(uint8_t txd)
{
  uint8_t t;

  SDA_OUT();
  I2C_SCL_L();
  for (t = 0; t < 8; t++) {
    if ((txd & 0x80) >> 7)
      I2C_SDA_H();
    else
      I2C_SDA_L();
    txd <<= 1;
    delay_us(1);
    I2C_SCL_H();
    delay_us(2);
    I2C_SCL_L();
    delay_us(1);
  }
}

uint8_t I2C_Read_Byte(unsigned char ack)
{
  unsigned char i, receive = 0;

  SDA_IN();
  for (i = 0; i < 8; i++) {
    I2C_SCL_L();
    delay_us(2);
    I2C_SCL_H();
    receive <<= 1;
    if (READ_SDA)receive++;
    delay_us(1);
  }
  if (!ack)
    I2C_NAck();
  else
    I2C_Ack();
  return receive;
}

uint8_t gt911WriteRegister(uint16_t reg, uint8_t * buf, uint8_t len)
{
  {
    uint8_t i;
    uint8_t ret = 0;

    I2C_Start();
    I2C_Send_Byte(GT_CMD_WR);         //send cmd
    I2C_Wait_Ack();
    I2C_Send_Byte(reg >> 8);      //send hi
    I2C_Wait_Ack();
    I2C_Send_Byte(reg & 0XFF);    //send low
    I2C_Wait_Ack();
    for (i = 0; i < len; i++) {
      I2C_Send_Byte(buf[i]);
      ret = I2C_Wait_Ack();
      if (ret)break;
    }
    I2C_Stop();

    return ret;
  }
}

void gt911ReadRegister(u16 reg, uint8_t * buf, uint8_t len)
{
  uint8_t i;

  I2C_Start();
  I2C_Send_Byte(GT_CMD_WR);       //send addr
  I2C_Wait_Ack();
  I2C_Send_Byte(reg >> 8);    //send hi
  I2C_Wait_Ack();
  I2C_Send_Byte(reg & 0XFF);  //send low
  I2C_Wait_Ack();
  I2C_Start();
  I2C_Send_Byte(GT_CMD_RD);       //send rd
  I2C_Wait_Ack();

  for (i = 0; i < len; i++) {
    buf[i] = I2C_Read_Byte(i == (len - 1) ? 0 : 1); //send data
  }
  I2C_Stop();
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
  I2C_Config();

  TPRST_LOW();
  TPINT_HIGH();
  delay_us(200);

  TPRST_HIGH();
  delay_ms(6);

  TPINT_LOW();
  delay_ms(55);

  GT911_INT_Change();  //Set INT INPUT INT=LOW

  delay_ms(50);

  TRACE("Reading Touch registry");
  gt911ReadRegister(GT_PID_REG, tmp, 4);

  if (strcmp((char *) tmp, "911") == 0) //ID==9147
  {
    TRACE("GT911 chip detected");
    tmp[0] = 0X02;
    gt911WriteRegister(GT_CTRL_REG, tmp, 1);
    gt911ReadRegister(GT_CFGS_REG, tmp, 1);

    TRACE("Chip config Ver:%x\r\n",tmp[0]);
    if (tmp[0] < GT911_CFG_NUMER)  //Config ver
    {
      TRACE("Sending new config %d", GT911_CFG_NUMER);
      GT911_Send_Cfg(1);
    }

    delay_ms(10);
    tmp[0] = 0X00;
    gt911WriteRegister(GT_CTRL_REG, tmp, 1);  //end reset
    touchGT911Flag = true;

    TOUCH_AF_ExtiConfig();

    return true;
  }
  TRACE("GT911 chip NOT FOUND");
  return false;
}

void touchPanelRead()
{
  uint8_t state = 0;
  gt911ReadRegister(GT911_READ_XY_REG, &state, 1);

  touchPanelEvent = false;

  if ((state & 0x80u) == 0x00) {
    // not ready
    return;
  }

  uint8_t pointsCount = (state & 0x0Fu);

  if (pointsCount > 0 && pointsCount < GT911_MAX_TP) {
    gt911ReadRegister(GT911_READ_XY_REG + 1, touchData.data, pointsCount * sizeof(TouchPoint));
    if (touchData.pointsCount == 0) {
      touchState.event = TE_DOWN;
      touchState.startX = touchState.x = touchData.points[0].x;
      touchState.startY = touchState.y = touchData.points[0].y;
    }
    else {
      touchState.deltaX = touchData.points[0].x - touchState.x;
      touchState.deltaY = touchData.points[0].y - touchState.y;
      if (touchState.event == TE_SLIDE || abs(touchState.deltaX) >= SLIDE_RANGE || abs(touchState.deltaY) >= SLIDE_RANGE) {
        touchState.event = TE_SLIDE;
        touchState.x = touchData.points[0].x;
        touchState.y = touchData.points[0].y;
      }
    }
    touchData.pointsCount = pointsCount;
  }
  else {
    if (touchData.pointsCount > 0) {
      touchData.pointsCount = 0;
      if (touchState.event == TE_SLIDE)
        touchState.event = TE_SLIDE_END;
      else
        touchState.event = TE_UP;
    }
  }

  uint8_t zero = 0;
  gt911WriteRegister(GT911_READ_XY_REG, &zero, 1);
}

extern "C" void TOUCH_INT_EXTI_IRQHandler1(void)
{
  if (EXTI_GetITStatus(TOUCH_INT_EXTI_LINE1) != RESET) {
    TRACE("TI");
    touchPanelEvent = 1;
    EXTI_ClearITPendingBit(TOUCH_INT_EXTI_LINE1);
  }
}
