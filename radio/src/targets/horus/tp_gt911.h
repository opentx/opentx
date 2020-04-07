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

#include "opentx.h"

#define HAS_TOUCH_PANEL()     touchGT911Flag == true
#define TOUCH_INT_STATUS()    (GPIO_ReadInputDataBit(TOUCH_INT_GPIO, TOUCH_INT_GPIO_PIN))

bool touchPanelInit();
void touchPanelDeInit();

extern uint8_t touchGT911Flag;
extern uint8_t TOUCH_SCAN_MODE;
extern uint8_t touchEventFlag;

uint8_t GT911_WR_Reg(uint16_t reg, uint8_t * buf, uint8_t len);
void GT911_RD_Reg(uint16_t reg, uint8_t * buf, uint8_t len);


//GT911 param table
const uint8_t GT911_Cfg[]=   //
  {
    0x69,	        //ver 0x68
    0xE0,0x01,          //x 480  480X272
    0x10,0x01,	        //y 272
    0x05,		//pass max count
    0x38,		//bit4= xy change Int mode
    0x00,
    0x02,		//press ct
    0x48,
    0x28,0x0D,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x18,0x1A,0x1E,0x14,0x8A,0x2A,0x0C,
    0x30,0x38,0x31,0x0D,0x00,0x00,0x02,0xB9,0x03,0x2D,
    0x00,0x00,0x00,0x00,0x00,0x03,0x64,0x32,0x00,0x00,
    0x00,0x1D,0x41,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,
    0xA5,0x1F,0x00,0x94,0x25,0x00,0x88,0x2B,0x00,0x7D,
    0x33,0x00,0x74,0x3C,0x00,0x74,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x18,0x16,0x14,0x12,0x10,0x0E,0x0C,0x0A,
    0x08,0x06,0x04,0x02,0xFF,0xFF,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x24,0x22,0x21,0x20,0x1F,0x1E,0x1D,0x1C,
    0x18,0x16,0x13,0x12,0x10,0x0F,0x0A,0x08,0x06,0x04,
    0x02,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
  };

//I2C
#define GT_CMD_WR 		0X28     	//write
#define GT_CMD_RD 		0X29		//read

#define SLAVE_Write_Address 0X28

//GT9147
#define GT_CTRL_REG 	        0X8040
#define GT_CFGS_REG 	        0X8047
#define GT_CHECK_REG 	        0X80FF
#define GT_PID_REG 		0X8140   	//ID

#define GT_GSTID_REG 	        0X814E   	//
#define GT_TP1_REG 		0X8150  	//
#define GT_TP2_REG 		0X8158		//
#define GT_TP3_REG 		0X8160		//
#define GT_TP4_REG 		0X8168		//
#define GT_TP5_REG 		0X8170		//

#define GT911_READ_XY_REG               0x814E
#define GT911_CLEARBUF_REG 		0x814E
#define GT911_CONFIG_REG 		0x8047
#define GT911_COMMAND_REG 		0x8040
#define GT911_PRODUCT_ID_REG 		0x8140
#define GT911_VENDOR_ID_REG 		0x814A
#define GT911_CONFIG_VERSION_REG 	0x8047
#define GT911_CONFIG_CHECKSUM_REG 	0x80FF
#define GT911_FIRMWARE_VERSION_REG	0x8144


typedef struct{
  uint16_t x;
  uint16_t y;
}POINT;

typedef struct{
  uint16_t cnt;
  POINT points[5];
}GT911_POINT_DATA;

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
