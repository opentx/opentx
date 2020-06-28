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

#ifndef _FT5X06_H_
#define _FT5X06_H_

#define HAS_TOUCH_PANEL()     touchGT911Flag == true

bool touchPanelInit();
void touchPanelDeInit();

extern uint8_t touchGT911Flag;
extern uint8_t touchPanelEvent;
extern bool TouchInit( void );

uint8_t gt911WriteRegister(uint16_t reg, uint8_t * buf, uint8_t len);
bool gt911ReadRegister(uint16_t reg, uint8_t * buf, uint8_t len);
void touchPanelRead();
bool touchPanelEventOccured();

#define GT911_MAX_TP      5
#define GT911_CFG_NUMER   0x6A

//I2C
#define GT_CMD_WR 		0X28
#define GT_CMD_RD 		0X29

#define SLAVE_Write_Address 0X28

//GT9147
#define GT_CTRL_REG 	        0X8040
#define GT_CFGS_REG 	        0X8047
#define GT_CHECK_REG 	        0X80FF
#define GT_PID_REG 		0X8140

#define GT_GSTID_REG 	        0X814E
#define GT_TP1_REG 		0X8150
#define GT_TP2_REG 		0X8158
#define GT_TP3_REG 		0X8160
#define GT_TP4_REG 		0X8168
#define GT_TP5_REG 		0X8170

#define GT911_READ_XY_REG               0x814E
#define GT911_CLEARBUF_REG              0x814E
#define GT911_CONFIG_REG                0x8047
#define GT911_COMMAND_REG               0x8040
#define GT911_PRODUCT_ID_REG            0x8140
#define GT911_VENDOR_ID_REG             0x814A
#define GT911_CONFIG_VERSION_REG        0x8047
#define GT911_CONFIG_CHECKSUM_REG       0x80FF
#define GT911_FIRMWARE_VERSION_REG      0x8144

PACK(typedef struct
{
  uint8_t track;
  uint16_t x;
  uint16_t y;
  uint16_t size;
  uint8_t reserved;
}) TouchPoint;

PACK(struct TouchData
{
  uint8_t pointsCount;
  union
  {
    TouchPoint points[GT911_MAX_TP];
    uint8_t data[GT911_MAX_TP * sizeof(TouchPoint)];
  };
});

extern struct TouchData touchData;

#define TPRST_LOW()   do { TOUCH_RST_GPIO->BSRRH = TOUCH_RST_GPIO_PIN; } while(0)
#define TPRST_HIGH()  do { TOUCH_RST_GPIO->BSRRL = TOUCH_RST_GPIO_PIN; } while(0)

#define TPINT_LOW()   do { TOUCH_INT_GPIO->BSRRH = TOUCH_INT_GPIO_PIN; } while(0)
#define TPINT_HIGH()  do { TOUCH_INT_GPIO->BSRRL = TOUCH_INT_GPIO_PIN; } while(0)


#define I2C_SCL_L()   do { I2C_GPIO->BSRRH = I2C_SCL_GPIO_PIN; } while(0)
#define I2C_SCL_H()   do { I2C_GPIO->BSRRL = I2C_SCL_GPIO_PIN; } while(0)

#define I2C_SDA_L()   do { I2C_GPIO->BSRRH = I2C_SDA_GPIO_PIN; } while(0)
#define I2C_SDA_H()   do { I2C_GPIO->BSRRL = I2C_SDA_GPIO_PIN; } while(0)

#define READ_SDA GPIO_ReadInputDataBit(I2C_GPIO, I2C_SDA_GPIO_PIN)

#endif // _FT5X06_H_
